#ifndef __ENCODER_H
#define __ENCODER_H

#define MAXSTREAM		2
#define MAXMSG			255
#define MAXPATH			255

#include "vfw.h"

class LmEncoder{
public:
	LmEncoder();
	virtual ~LmEncoder();


	/// 파일 열기.
	BOOL	OpenAVIFile(LPCTSTR lpszFileName,
								DWORD dwCodec = mmioFOURCC('M', 'S', 'V', 'C'),
								DWORD dwFrameRate = 30, DWORD dwQuality = -1);

	BOOL	AVMixing(LPCTSTR lpszVideoFileName, LPCTSTR lpszAudioFileName);

	/// 파일 닫기.
	BOOL	CloseAVIFile();


	/// 프레임 추가.
	BOOL	AppendFrame(HBITMAP hBitmap);
	BOOL	AppendFrame(int nWidth, int nHeight, LPVOID pBuf, int nBitsPerPixel);


	/// 에러 메시지 리턴.
	LPCTSTR	GetLastErrorMsg() const{ return m_szErrMsg; }





private:
	PAVIFILE		m_pAVIfile;					// AVI파일 핸들.
	HDC				m_hAviDC;
	PAVISTREAM		m_pAVIStream[MAXSTREAM];	// 비디오, 오디오 스트림.
	PAVISTREAM		m_pAviCompressedStream;
	AVISTREAMINFO	m_AVIStreamInfo[MAXSTREAM];
	LPVOID			m_WriteBuffer;				// 쓰기 버퍼.
	TCHAR			m_szErrMsg[MAXMSG];			// 에러 메시지.
	TCHAR			m_szFileName[MAXPATH];
	DWORD			m_Codec;					// 코덱.
	DWORD			m_FPS;
	AVICOMPRESSOPTIONS	m_AVICompOpt[MAXSTREAM];
	BOOL			m_isFirstFrame;
	LONG			m_lSample;
	DWORD			m_Quality;

	
	/// 스트림 생성 및 초기화.
	BOOL	InitMovie(int nFrameWidth, int nFrameHeight, int nBitsPerPixel);
	
	/// 에러 메시지 설정.
	VOID	SetErrorMsg(LPCTSTR lpszErrMsg);
	
	/// 메모리 해제.
	VOID	ReleaseMemory();


};

#endif




// 사용 예
//HBITMAP			MyBit;
//LmEncoder		encoder;
//encoder.OpenAVIFile(_T("encoder.avi"), mmioFOURCC('c','v','i','d'), 30, 100);		//MSVC, cvid
//
//CString			FileName;
//for (int i = 1; i < 181; i++){
//	FileName.Format(_T("sample_bmp\\%d.bmp"), i);
//	MyBit = (HBITMAP)LoadImage(NULL, FileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
//
//	if(FAILED(encoder.AppendFrame(MyBit)))
//	{
//		AfxMessageBox(encoder.GetLastErrorMsg());
//		return;
//	}
//	DeleteObject(MyBit);
//}
//
//encoder.CloseAVIFile();
