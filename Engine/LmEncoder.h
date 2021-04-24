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


	/// ���� ����.
	BOOL	OpenAVIFile(LPCTSTR lpszFileName,
								DWORD dwCodec = mmioFOURCC('M', 'S', 'V', 'C'),
								DWORD dwFrameRate = 30, DWORD dwQuality = -1);

	BOOL	AVMixing(LPCTSTR lpszVideoFileName, LPCTSTR lpszAudioFileName);

	/// ���� �ݱ�.
	BOOL	CloseAVIFile();


	/// ������ �߰�.
	BOOL	AppendFrame(HBITMAP hBitmap);
	BOOL	AppendFrame(int nWidth, int nHeight, LPVOID pBuf, int nBitsPerPixel);


	/// ���� �޽��� ����.
	LPCTSTR	GetLastErrorMsg() const{ return m_szErrMsg; }





private:
	PAVIFILE		m_pAVIfile;					// AVI���� �ڵ�.
	HDC				m_hAviDC;
	PAVISTREAM		m_pAVIStream[MAXSTREAM];	// ����, ����� ��Ʈ��.
	PAVISTREAM		m_pAviCompressedStream;
	AVISTREAMINFO	m_AVIStreamInfo[MAXSTREAM];
	LPVOID			m_WriteBuffer;				// ���� ����.
	TCHAR			m_szErrMsg[MAXMSG];			// ���� �޽���.
	TCHAR			m_szFileName[MAXPATH];
	DWORD			m_Codec;					// �ڵ�.
	DWORD			m_FPS;
	AVICOMPRESSOPTIONS	m_AVICompOpt[MAXSTREAM];
	BOOL			m_isFirstFrame;
	LONG			m_lSample;
	DWORD			m_Quality;

	
	/// ��Ʈ�� ���� �� �ʱ�ȭ.
	BOOL	InitMovie(int nFrameWidth, int nFrameHeight, int nBitsPerPixel);
	
	/// ���� �޽��� ����.
	VOID	SetErrorMsg(LPCTSTR lpszErrMsg);
	
	/// �޸� ����.
	VOID	ReleaseMemory();


};

#endif




// ��� ��
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
