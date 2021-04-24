#include "stdafx.h"
#include "LmEncoder.h"

#ifndef __countof
#define __countof(x)	((sizeof(x)/sizeof(x[0])))
#endif

#define ENCODER_VIDEO		0
#define ENCODER_AUDIO		1


//------------------------------------------------------------------------------
// 생성자
// 멤버 변수 초기화
//------------------------------------------------------------------------------
LmEncoder::LmEncoder()
{
	m_pAVIfile = NULL;					// AVI파일 핸들.
	m_WriteBuffer = NULL;				// 쓰기 버퍼.
	m_Codec = NULL;
	m_pAviCompressedStream = NULL;
	m_isFirstFrame = TRUE;
	m_lSample = 0;
	m_Quality = 0;
	ZeroMemory(m_pAVIStream, sizeof(PAVISTREAM) * MAXSTREAM);
	ZeroMemory(m_AVIStreamInfo, sizeof(AVISTREAMINFO) * MAXSTREAM);	
	ZeroMemory(m_szErrMsg, sizeof(TCHAR) * MAXMSG);
	ZeroMemory(m_szFileName, sizeof(TCHAR) * MAXPATH);

}

//------------------------------------------------------------------------------
// 소멸자
//------------------------------------------------------------------------------
LmEncoder::~LmEncoder()
{
	// 현재 동적 생성 아무것도 없음

}


//------------------------------------------------------------------------------
// AVI및 wav파일을 열고 초기화
// lpszFileName		: 파일이름
// dwCodec			: 코덱종류(mmioFOURCC 로 얻어와야 함)
// dwFrameRate		: 영상의 초당 프레임 수
// dwQuality		: 화질. 0~100 사이의 값. 100에 가까울수록 화질 좋음.
//------------------------------------------------------------------------------
BOOL LmEncoder::OpenAVIFile(LPCTSTR lpszFileName, DWORD dwCodec, DWORD dwFrameRate , DWORD dwQuality)
{
	LONG				hr;

	AVIFileInit();

	hr = AVIFileOpen(&m_pAVIfile, lpszFileName, OF_CREATE|OF_WRITE, NULL);
	
	m_Codec = dwCodec;
	m_FPS = dwFrameRate;
	m_Quality = dwQuality;


	return FALSE;
}


//------------------------------------------------------------------------------
// AVI및 wav파일 닫기
//------------------------------------------------------------------------------
BOOL LmEncoder::CloseAVIFile()
{
	ReleaseMemory();
	AVIFileExit();

	return FALSE;
}


//------------------------------------------------------------------------------
// 영상 초기화
// nFrameWidth		: 영상의 가로 크기(픽셀 단위)
// nFrameHeight		: 영상의 세로 크기(픽셀 단위)
// nBitsPerPixel	: 한 픽셀당 비트수(보통 rgb 24비트)
//------------------------------------------------------------------------------
BOOL LmEncoder::InitMovie(int nFrameWidth, int nFrameHeight, int nBitsPerPixel)
{
	// 나중에 DDB DIB 변환에 사용
	m_hAviDC = CreateCompatibleDC(NULL);
	if(m_hAviDC==NULL)	
	{
		SetErrorMsg(_T("Unable to Create Compatible DC"));
		return E_FAIL;
	}

	// 영상 한장 만큼 버퍼 할당
	m_WriteBuffer = new BYTE[nFrameHeight * nFrameWidth * 4];
	if (!m_WriteBuffer){
		SetErrorMsg(_T("Unable to Allocate Memory"));
		return FALSE;
	}

	// 생성될 영상 스트림 정보 대입
	ZeroMemory(&m_AVIStreamInfo[ENCODER_VIDEO], sizeof(AVISTREAMINFO));
	m_AVIStreamInfo[ENCODER_VIDEO].fccType			= streamtypeVIDEO;
	m_AVIStreamInfo[ENCODER_VIDEO].fccHandler		= m_Codec;
	m_AVIStreamInfo[ENCODER_VIDEO].dwScale			= 1;
	m_AVIStreamInfo[ENCODER_VIDEO].dwRate			= m_FPS;
	m_AVIStreamInfo[ENCODER_VIDEO].dwQuality		= -1;			// 0 ~ 10000 사이의 값 설정. -1 일경우는 기본값.
	m_AVIStreamInfo[ENCODER_VIDEO].dwSuggestedBufferSize = nFrameWidth * nFrameHeight * 4;
	SetRect(&m_AVIStreamInfo[ENCODER_VIDEO].rcFrame, 0, 0, nFrameWidth, nFrameHeight);
	_tcscpy_s(m_AVIStreamInfo[ENCODER_VIDEO].szName, _T("Video Stream"));

	// 영상 스트림 생성
	if (FAILED(AVIFileCreateStream(m_pAVIfile, &m_pAVIStream[ENCODER_VIDEO], &m_AVIStreamInfo[ENCODER_VIDEO]))){
		SetErrorMsg(_T("Unable to Create Video Stream in the Movie File"));
		return FALSE;
	}

	// 압축 옵션 설정
	ZeroMemory(&m_AVICompOpt[ENCODER_VIDEO], sizeof(AVICOMPRESSOPTIONS));
	m_AVICompOpt[ENCODER_VIDEO].fccType				= streamtypeVIDEO;
	m_AVICompOpt[ENCODER_VIDEO].fccHandler			= m_AVIStreamInfo[ENCODER_VIDEO].fccHandler;
	m_AVICompOpt[ENCODER_VIDEO].dwFlags				= AVICOMPRESSF_KEYFRAMES|AVICOMPRESSF_VALID;
	m_AVICompOpt[ENCODER_VIDEO].dwKeyFrameEvery		= 20;
	m_AVICompOpt[ENCODER_VIDEO].dwQuality			= m_Quality;

	if (FAILED(AVIMakeCompressedStream(&m_pAviCompressedStream, m_pAVIStream[ENCODER_VIDEO], &m_AVICompOpt[ENCODER_VIDEO], NULL))){
		SetErrorMsg(_T("Unable to Create Compressed Stream: Check your CODEC options"));
	}

	// 스트림 영상에 대한 정보 설정
	BITMAPINFO bmpInfo;
	ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
	bmpInfo.bmiHeader.biPlanes		= 1;
	bmpInfo.bmiHeader.biWidth		= nFrameWidth;
	bmpInfo.bmiHeader.biHeight		= nFrameHeight;
	bmpInfo.bmiHeader.biCompression	= BI_RGB;
	bmpInfo.bmiHeader.biBitCount	= nBitsPerPixel;
	bmpInfo.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biSizeImage	= bmpInfo.bmiHeader.biWidth*bmpInfo.bmiHeader.biHeight*bmpInfo.bmiHeader.biBitCount/8;

	// 포멧 적용
	if (FAILED(AVIStreamSetFormat(m_pAviCompressedStream, 0, (LPVOID)&bmpInfo, bmpInfo.bmiHeader.biSize))){
		SetErrorMsg(_T("Unable to Set Video Stream Format"));
		return FALSE;
	}	

	return TRUE;
}


//------------------------------------------------------------------------------
// 프레임 추가
// 비트맵으로 데이터가 들어오면 영상에 추가.
// hBitmap	: 추가 할 비트맵 핸들
//------------------------------------------------------------------------------
BOOL LmEncoder::AppendFrame(HBITMAP hBitmap)
{
	// 첫번째 프레임일 경우 출력할 동영상 파일 초기화를 해준다.
	if (!hBitmap){
		SetErrorMsg(_T("Append Frame error"));
		return FALSE;
	}

	if (m_isFirstFrame){
		BITMAP Bitmap;
		GetObject(hBitmap, sizeof(BITMAP), &Bitmap);
		if (FAILED(InitMovie(Bitmap.bmWidth, Bitmap.bmHeight, Bitmap.bmBitsPixel))){
			return FALSE;
		}
		m_isFirstFrame = FALSE;
	}
	
	BITMAPINFO	bmpInfo;
	bmpInfo.bmiHeader.biBitCount = 0;
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	// 비트맵 정보를 얻어옴
	GetDIBits(m_hAviDC, hBitmap, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS);
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	// DDB를 DIB로 변환
	GetDIBits(m_hAviDC, hBitmap, 0, bmpInfo.bmiHeader.biHeight, m_WriteBuffer, &bmpInfo, DIB_RGB_COLORS);


	// Video 스트림에 쓰기
	if(FAILED(AVIStreamWrite(m_pAviCompressedStream , m_lSample++, 1, m_WriteBuffer,
							 bmpInfo.bmiHeader.biSizeImage, 0, NULL, NULL)))
	{
		SetErrorMsg(_T("Unable to Write Video Stream to the output Movie File"));

		ReleaseMemory();
		return FALSE;
	}

	return TRUE;
}


//------------------------------------------------------------------------------
// 프레임 추가
// 비트맵으로 데이터가 들어오면 영상에 추가.
// 단, 비트맵 헤더가 없는 경우 이 함수를 사용.
// nWidth			: 가로 크기(픽셀 단위)
// nHeight			: 세로 크기(픽셀 단위)
// pBuf				: 데이터가 있는 곳의 포인터
// nBitsPerPixel	: 한 픽셀당 비트수(보통 rgb 24비트)
//------------------------------------------------------------------------------
BOOL LmEncoder::AppendFrame(int nWidth, int nHeight, LPVOID pBuf, int nBitsPerPixel)
{
	if (m_isFirstFrame){
		if (FAILED(InitMovie(nWidth, nHeight, nBitsPerPixel)))
			return FALSE;

		m_isFirstFrame = FALSE;
	}

	DWORD	dwSize = nWidth * nHeight * nBitsPerPixel / 8;

	if(FAILED(AVIStreamWrite(m_pAviCompressedStream, m_lSample++, 1, m_WriteBuffer, dwSize, 0, NULL, NULL)))
	{
		SetErrorMsg(_T("Unable to Write Video Stream to the output Movie File"));

		ReleaseMemory();
		return FALSE;
	}

	return TRUE;
}


//------------------------------------------------------------------------------
// 에러 메시지 설정 함수
//------------------------------------------------------------------------------
VOID LmEncoder::SetErrorMsg(LPCTSTR lpszErrMsg)
{
	_tcsncpy_s(m_szErrMsg, lpszErrMsg, __countof(m_szErrMsg) - 1);
}


//------------------------------------------------------------------------------
// 열었던 AVI 스트림 및 파일 릴리즈
//------------------------------------------------------------------------------
VOID LmEncoder::ReleaseMemory()
{
	if(m_hAviDC)
	{
		DeleteDC(m_hAviDC);
		m_hAviDC=NULL;
	}
	if(m_pAviCompressedStream)
	{
		AVIStreamRelease(m_pAviCompressedStream);
		m_pAviCompressedStream=NULL;
	}
	if(m_pAVIStream[ENCODER_VIDEO])
	{
		AVIStreamRelease(m_pAVIStream[ENCODER_VIDEO]);
		m_pAVIStream[ENCODER_VIDEO]=NULL;
	}
	if(m_pAVIfile)
	{
		AVIFileRelease(m_pAVIfile);
		m_pAVIfile=NULL;
	}
	if(m_WriteBuffer)
	{
		delete[] m_WriteBuffer;
		m_WriteBuffer=NULL;
	}


}


//------------------------------------------------------------------------------
// 따로 구현.
// 현재 필요 없음.
//------------------------------------------------------------------------------
BOOL LmEncoder::AVMixing(LPCTSTR lpszVideoFileName, LPCTSTR lpszAudioFileName)
{

	return FALSE;
}