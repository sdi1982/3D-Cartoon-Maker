#include "stdafx.h"
#include "LmEncoder.h"

#ifndef __countof
#define __countof(x)	((sizeof(x)/sizeof(x[0])))
#endif

#define ENCODER_VIDEO		0
#define ENCODER_AUDIO		1


//------------------------------------------------------------------------------
// ������
// ��� ���� �ʱ�ȭ
//------------------------------------------------------------------------------
LmEncoder::LmEncoder()
{
	m_pAVIfile = NULL;					// AVI���� �ڵ�.
	m_WriteBuffer = NULL;				// ���� ����.
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
// �Ҹ���
//------------------------------------------------------------------------------
LmEncoder::~LmEncoder()
{
	// ���� ���� ���� �ƹ��͵� ����

}


//------------------------------------------------------------------------------
// AVI�� wav������ ���� �ʱ�ȭ
// lpszFileName		: �����̸�
// dwCodec			: �ڵ�����(mmioFOURCC �� ���;� ��)
// dwFrameRate		: ������ �ʴ� ������ ��
// dwQuality		: ȭ��. 0~100 ������ ��. 100�� �������� ȭ�� ����.
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
// AVI�� wav���� �ݱ�
//------------------------------------------------------------------------------
BOOL LmEncoder::CloseAVIFile()
{
	ReleaseMemory();
	AVIFileExit();

	return FALSE;
}


//------------------------------------------------------------------------------
// ���� �ʱ�ȭ
// nFrameWidth		: ������ ���� ũ��(�ȼ� ����)
// nFrameHeight		: ������ ���� ũ��(�ȼ� ����)
// nBitsPerPixel	: �� �ȼ��� ��Ʈ��(���� rgb 24��Ʈ)
//------------------------------------------------------------------------------
BOOL LmEncoder::InitMovie(int nFrameWidth, int nFrameHeight, int nBitsPerPixel)
{
	// ���߿� DDB DIB ��ȯ�� ���
	m_hAviDC = CreateCompatibleDC(NULL);
	if(m_hAviDC==NULL)	
	{
		SetErrorMsg(_T("Unable to Create Compatible DC"));
		return E_FAIL;
	}

	// ���� ���� ��ŭ ���� �Ҵ�
	m_WriteBuffer = new BYTE[nFrameHeight * nFrameWidth * 4];
	if (!m_WriteBuffer){
		SetErrorMsg(_T("Unable to Allocate Memory"));
		return FALSE;
	}

	// ������ ���� ��Ʈ�� ���� ����
	ZeroMemory(&m_AVIStreamInfo[ENCODER_VIDEO], sizeof(AVISTREAMINFO));
	m_AVIStreamInfo[ENCODER_VIDEO].fccType			= streamtypeVIDEO;
	m_AVIStreamInfo[ENCODER_VIDEO].fccHandler		= m_Codec;
	m_AVIStreamInfo[ENCODER_VIDEO].dwScale			= 1;
	m_AVIStreamInfo[ENCODER_VIDEO].dwRate			= m_FPS;
	m_AVIStreamInfo[ENCODER_VIDEO].dwQuality		= -1;			// 0 ~ 10000 ������ �� ����. -1 �ϰ��� �⺻��.
	m_AVIStreamInfo[ENCODER_VIDEO].dwSuggestedBufferSize = nFrameWidth * nFrameHeight * 4;
	SetRect(&m_AVIStreamInfo[ENCODER_VIDEO].rcFrame, 0, 0, nFrameWidth, nFrameHeight);
	_tcscpy_s(m_AVIStreamInfo[ENCODER_VIDEO].szName, _T("Video Stream"));

	// ���� ��Ʈ�� ����
	if (FAILED(AVIFileCreateStream(m_pAVIfile, &m_pAVIStream[ENCODER_VIDEO], &m_AVIStreamInfo[ENCODER_VIDEO]))){
		SetErrorMsg(_T("Unable to Create Video Stream in the Movie File"));
		return FALSE;
	}

	// ���� �ɼ� ����
	ZeroMemory(&m_AVICompOpt[ENCODER_VIDEO], sizeof(AVICOMPRESSOPTIONS));
	m_AVICompOpt[ENCODER_VIDEO].fccType				= streamtypeVIDEO;
	m_AVICompOpt[ENCODER_VIDEO].fccHandler			= m_AVIStreamInfo[ENCODER_VIDEO].fccHandler;
	m_AVICompOpt[ENCODER_VIDEO].dwFlags				= AVICOMPRESSF_KEYFRAMES|AVICOMPRESSF_VALID;
	m_AVICompOpt[ENCODER_VIDEO].dwKeyFrameEvery		= 20;
	m_AVICompOpt[ENCODER_VIDEO].dwQuality			= m_Quality;

	if (FAILED(AVIMakeCompressedStream(&m_pAviCompressedStream, m_pAVIStream[ENCODER_VIDEO], &m_AVICompOpt[ENCODER_VIDEO], NULL))){
		SetErrorMsg(_T("Unable to Create Compressed Stream: Check your CODEC options"));
	}

	// ��Ʈ�� ���� ���� ���� ����
	BITMAPINFO bmpInfo;
	ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
	bmpInfo.bmiHeader.biPlanes		= 1;
	bmpInfo.bmiHeader.biWidth		= nFrameWidth;
	bmpInfo.bmiHeader.biHeight		= nFrameHeight;
	bmpInfo.bmiHeader.biCompression	= BI_RGB;
	bmpInfo.bmiHeader.biBitCount	= nBitsPerPixel;
	bmpInfo.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biSizeImage	= bmpInfo.bmiHeader.biWidth*bmpInfo.bmiHeader.biHeight*bmpInfo.bmiHeader.biBitCount/8;

	// ���� ����
	if (FAILED(AVIStreamSetFormat(m_pAviCompressedStream, 0, (LPVOID)&bmpInfo, bmpInfo.bmiHeader.biSize))){
		SetErrorMsg(_T("Unable to Set Video Stream Format"));
		return FALSE;
	}	

	return TRUE;
}


//------------------------------------------------------------------------------
// ������ �߰�
// ��Ʈ������ �����Ͱ� ������ ���� �߰�.
// hBitmap	: �߰� �� ��Ʈ�� �ڵ�
//------------------------------------------------------------------------------
BOOL LmEncoder::AppendFrame(HBITMAP hBitmap)
{
	// ù��° �������� ��� ����� ������ ���� �ʱ�ȭ�� ���ش�.
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

	// ��Ʈ�� ������ ����
	GetDIBits(m_hAviDC, hBitmap, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS);
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	// DDB�� DIB�� ��ȯ
	GetDIBits(m_hAviDC, hBitmap, 0, bmpInfo.bmiHeader.biHeight, m_WriteBuffer, &bmpInfo, DIB_RGB_COLORS);


	// Video ��Ʈ���� ����
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
// ������ �߰�
// ��Ʈ������ �����Ͱ� ������ ���� �߰�.
// ��, ��Ʈ�� ����� ���� ��� �� �Լ��� ���.
// nWidth			: ���� ũ��(�ȼ� ����)
// nHeight			: ���� ũ��(�ȼ� ����)
// pBuf				: �����Ͱ� �ִ� ���� ������
// nBitsPerPixel	: �� �ȼ��� ��Ʈ��(���� rgb 24��Ʈ)
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
// ���� �޽��� ���� �Լ�
//------------------------------------------------------------------------------
VOID LmEncoder::SetErrorMsg(LPCTSTR lpszErrMsg)
{
	_tcsncpy_s(m_szErrMsg, lpszErrMsg, __countof(m_szErrMsg) - 1);
}


//------------------------------------------------------------------------------
// ������ AVI ��Ʈ�� �� ���� ������
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
// ���� ����.
// ���� �ʿ� ����.
//------------------------------------------------------------------------------
BOOL LmEncoder::AVMixing(LPCTSTR lpszVideoFileName, LPCTSTR lpszAudioFileName)
{

	return FALSE;
}