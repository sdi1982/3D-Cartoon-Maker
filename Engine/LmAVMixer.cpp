#include "stdafx.h"
#include "LmAVMixer.h"



//------------------------------------------------------------------------------
// 생성자
// 멤버 변수 초기화
//------------------------------------------------------------------------------
LmAVMixer::LmAVMixer()
{
	m_npStream = 0;
	m_lpAudio = NULL;	
	m_pfile[0] = NULL;
	m_pfile[1] = NULL;

	memset(m_pAVIStream, 0, sizeof(PAVISTREAM) * MAXNUMSTREAMS);
	memset(m_AVIOptions, 0, sizeof(AVICOMPRESSOPTIONS) * MAXNUMSTREAMS);
	memset(m_lpAVIOptions, 0, sizeof(LPAVICOMPRESSOPTIONS) * MAXNUMSTREAMS);
	
	SetErrorMsg(_T(""));

	AVIFileInit();

}

LmAVMixer::~LmAVMixer()
{
	ULONG c;
	c = AVIFileRelease(m_pfile[0]);
	c = AVIFileRelease(m_pfile[0]);	
	c = AVIFileRelease(m_pfile[1]);
	c = AVIFileRelease(m_pfile[1]);
	AVIFileExit();
}


//------------------------------------------------------------------------------
// 스트림 초기화
//------------------------------------------------------------------------------
VOID LmAVMixer::InitStreams()
{
	AVISTREAMINFO     avis;
	LONG        lTemp;
	int         i;


	// 스트림의 갯수만큼 스트림을 초기화
	for (i = 0; i < m_npStream; i++) {

		// 스트림 정보 얻기
		AVIStreamInfo(m_pAVIStream[i], &avis, sizeof(avis));
		
		m_lpAVIOptions[i] = &m_AVIOptions[i];
		
		memset(m_lpAVIOptions[i], 0, sizeof(AVICOMPRESSOPTIONS));

		// 압축 옵션 설정. 원래 상태 그대로
		m_lpAVIOptions[i]->fccType = avis.fccType;

		// 비디오와 오디오를 구분해서 옵션을 만들어 줌.
		switch(avis.fccType) {

			case streamtypeVIDEO:
				m_lpAVIOptions[i]->dwFlags = AVICOMPRESSF_VALID |
					AVICOMPRESSF_KEYFRAMES | AVICOMPRESSF_DATARATE;
				m_lpAVIOptions[i]->fccHandler = 0;
				m_lpAVIOptions[i]->dwQuality = (DWORD)ICQUALITY_DEFAULT;
				m_lpAVIOptions[i]->dwKeyFrameEvery = (DWORD)-1; // Default
				m_lpAVIOptions[i]->dwBytesPerSecond = 0;
				m_lpAVIOptions[i]->dwInterleaveEvery = 1;
				break;

			case streamtypeAUDIO:
				m_lpAVIOptions[i]->dwFlags |= AVICOMPRESSF_VALID;
				m_lpAVIOptions[i]->dwInterleaveEvery = 1;
				AVIStreamReadFormat(m_pAVIStream[i],
					AVIStreamStart(m_pAVIStream[i]), NULL, &lTemp);
				m_lpAVIOptions[i]->cbFormat = lTemp;
				
				if (lTemp)
					m_lpAVIOptions[i]->lpFormat = GlobalAllocPtr(GHND, lTemp);
				
				if (m_lpAVIOptions[i]->lpFormat)
					AVIStreamReadFormat(m_pAVIStream[i],
					AVIStreamStart(m_pAVIStream[i]), m_lpAVIOptions[i]->lpFormat, &lTemp);
				break;

			default:
				break;
		}
	}
}


//------------------------------------------------------------------------------
// 리소스 해제
//------------------------------------------------------------------------------
VOID LmAVMixer::FreeAvi()
{
	int i;

	AVISaveOptionsFree(m_npStream, m_lpAVIOptions);

	for (i = 0; i < m_npStream; i++) {
		AVIStreamRelease(m_pAVIStream[i]);
	}

	// Good a place as any to make sure audio data gets freed
	if (m_lpAudio)
		GlobalFreePtr(m_lpAudio);
	m_lpAudio = NULL;

	m_npStream = 0;
}


//------------------------------------------------------------------------------
// 파일 추가
// pfile	: AVI파일 핸들
// lpszFile	: 입력 파일 이름
//------------------------------------------------------------------------------
BOOL LmAVMixer::InsertAVIFile(PAVIFILE pfile, LPCTSTR lpszFile)
{
	int         i;
	PAVISTREAM  pavi;
	// 파일에 몇개의 스트림이 있는지 검사
	for (i = m_npStream; i <= MAXNUMSTREAMS; i++) {		
		if (AVIFileGetStream(pfile, &pavi, 0L, i - m_npStream) != AVIERR_OK)
			break;		
		if (i == MAXNUMSTREAMS)
		{
			AVIStreamRelease(pavi);
			break;
		}
		if (CreateEditableStream(&m_pAVIStream[i], pavi) != AVIERR_OK) {
			AVIStreamRelease(pavi);
			break;
		}
		AVIStreamRelease(pavi);
	}	

	// 스트림이 너무 많을 경우
	if (m_npStream == i && i != MAXNUMSTREAMS)
	{
		SetErrorMsg(_T("Stream is over"));
		return FALSE;
	}
	m_npStream = i;

	InitStreams();

	return TRUE;
}

//------------------------------------------------------------------------------
// 파일 열고 스트림 얻기
//------------------------------------------------------------------------------
BOOL LmAVMixer::InitAvi(LPCTSTR szFile, int nOpenFile)
{
	HRESULT     hr;
	PAVIFILE    *pfile;

	if (!m_pfile[0])
		pfile = &m_pfile[0];		
	else
		pfile = &m_pfile[1];

	hr = AVIFileOpen(pfile, szFile, 0, 0L);

	if (hr != 0)
	{
		return FALSE;
	}

	InsertAVIFile(*pfile, szFile);

	return TRUE;
}

//------------------------------------------------------------------------------
// 스트림을 다시 저장하는 함수
// szFile	: 한글 경로 안됨
//------------------------------------------------------------------------------
BOOL LmAVMixer::Mixing(LPCTSTR szFile)
{
	DWORD       fccHandler[MAXNUMSTREAMS];
	int         i;
	HRESULT     hr;

	for (i = 0; i < m_npStream; i++)
		fccHandler[i] = m_lpAVIOptions[i]->fccHandler;

	// 생성될 AVI 파일
	hr = AVISaveV(szFile,
		NULL,
		NULL,
		m_npStream,
		m_pAVIStream,
		m_lpAVIOptions);

	if (hr != AVIERR_OK)
	{
		switch (hr) {
			case AVIERR_FILEOPEN:
				SetErrorMsg(_T("File Open Error. Check File Path"));
				return FALSE;
			default:
				SetErrorMsg(_T("AVI Save Error"));
				return FALSE;								
		}
	}

	// 비디오 압축 옵션 원래 대로
	for (i = 0; i < m_npStream; i++)
		m_lpAVIOptions[i]->fccHandler = fccHandler[i];

	return TRUE;
	
}

VOID LmAVMixer::SetErrorMsg(LPCTSTR lpszErrMsg)
{
	_tcscpy_s(m_szErrMsg, lpszErrMsg);
}