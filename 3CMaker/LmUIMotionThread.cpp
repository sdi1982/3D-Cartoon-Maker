// LmUIMotionThread.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUIMotionThread.h"
#include "LmUITimeLineView.h"
#include "../Engine/LmEncoder.h"
#include "../Engine/LmMixer.h"
#include "../Engine/LmAVMixer.h"
#include "LmUICreateMovieClipDlg.h"


// LmUIMotionThread

IMPLEMENT_DYNCREATE(LmUIMotionThread, CWinThread)

LmUIMotionThread::LmUIMotionThread()
{
}

LmUIMotionThread::LmUIMotionThread( LmUITimeLineView* _pTimeLineView )
{
	m_pTimeLIneView = _pTimeLineView;
}
LmUIMotionThread::~LmUIMotionThread()
{
}

BOOL LmUIMotionThread::InitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 초기화를 수행합니다.
	return TRUE;
}

int LmUIMotionThread::ExitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 정리를 수행합니다.
	return CWinThread::ExitInstance();
}

void LmUIMotionThread::OnMotionPlay( WPARAM wParam, LPARAM lParam )
{
	INT nFramePerSec = TheCartoonMaker->GetFramePerSec();
	INT nSleepTime = 1000 / nFramePerSec;
	CString PlaySoundID;
	while(1)
	{
		Sleep(nSleepTime);						
				
		INT CurFrame = TheCartoonMaker->GetCurFrame();

		// 사운드 재생
		UINT nSoundPlay = 0;
		SectionVector vSoundPlay;
		vSoundPlay.clear();

		vSoundPlay = TheCartoonMaker->GetNowSoundSection( CurFrame, &nSoundPlay);

		SectionVectorItr itrSection;
		for(itrSection = vSoundPlay.begin(); itrSection != vSoundPlay.end() ; itrSection ++)
		{
			TheCartoonMaker->SoundPlay(itrSection->m_wstrSectionName.c_str());
		}

		// 다음 프레임으로 넘어가기
		INT Result = (INT)m_pTimeLIneView->GoNextCaret();
		if(Result<0)
		{
			m_pTimeLIneView->ChangeStopToPlayBtn();
			return;
		}

		//메시지를 받아오는 부분
		MSG msg;
		PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		if(msg.message==WM_MOTION_STOP)
			break;
	}

}

void LmUIMotionThread::OnCreateMovie(WPARAM wParam, LPARAM lParam)
{		
	HBITMAP	MyBit;
	LmEncoder encoder;
	CString AVIFileName, BMPFileName, WAVFileName, AVIFileNameTemp;
	DWORD cFrame = 0;	
		
	// 다이얼로그에 정보를 갱신하기 위해.
	LmUICreateMovieClipDlg* CreateMovieClopDlg = (LmUICreateMovieClipDlg*)lParam;
	HWND hParent = CreateMovieClopDlg->m_hWnd;
	
	// 각종 파일 이름 설정
	BMPFileName = g_strFilePath + _T("\\temp.bmp");
	WAVFileName = g_strFilePath + _T("\\temp.wav");	
	AVIFileNameTemp = g_strFilePath + _T("\\temp.avi");
	AVIFileName = (LPCTSTR)wParam;

	// AVIFileNameTemp == AVIFileName 일때 처리 해줘야함.

	// 믹싱 초기화
	LmMixer Mixer;
	if (!Mixer.Init(TheCartoonMaker->GetSoundLayerCount(),
				TheCartoonMaker->GetFramePerSec(),
				WAVFileName.GetBuffer())){
					return;
	}
	WAVFileName.ReleaseBuffer();

	// Video 초기화
	if (encoder.OpenAVIFile(AVIFileNameTemp, mmioFOURCC('c','v','i','d'), 20, 100)){		//MSVC, cvid
		return;
	}

	while(1)
	{			
		// 현재 프레임 얻어오기
		INT CurFrame = TheCartoonMaker->GetCurFrame();

		TheCartoonMaker->RenderOnce();
		// Video 작업
		if (!TheCartoonMaker->ScreenToFile(BMPFileName)){

		}
		

		MyBit = (HBITMAP)LoadImage(NULL, BMPFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
		
		if (!encoder.AppendFrame(MyBit)){
			DeleteObject(MyBit);
			DeleteFile(BMPFileName);
			encoder.CloseAVIFile();;
			//return;
		}
			DeleteObject(MyBit);		

		// Sound 작업
		UINT nSoundPlay = 0;
		SectionVector vSoundPlay;
		vSoundPlay.clear();

		vSoundPlay = TheCartoonMaker->GetNowSoundSection( CurFrame, &nSoundPlay);
		
		SectionVectorItr itrSection;
		for(itrSection = vSoundPlay.begin(); itrSection != vSoundPlay.end() ; itrSection ++)
		{			
			Mixer.AddFile(TheCartoonMaker->GetSoundFileName(itrSection->m_wstrSectionName.c_str()));
		}

		Mixer.Mixing();		

		INT Result = (INT)m_pTimeLIneView->GoNextCaret();
		if(Result<0)
			break;

		
		MSG msg;		
		PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		if(msg.message==WM_MOTION_MOVIESTOP){			
			break;
		}

		CreateMovieClopDlg->SetState(++cFrame);
	}

	// 동영상 작업 마무리 부분
	encoder.CloseAVIFile();
	DeleteFile(BMPFileName);
	Mixer.CloseWaveFile();

	// Video와 Sound 합침
	LmAVMixer AVMixer;
	AVMixer.InitAvi(AVIFileNameTemp, OPEN_FILE);
	AVMixer.InitAvi(WAVFileName, MERGE_FILE);
	AVMixer.Mixing(AVIFileName);

	DeleteFile(AVIFileNameTemp);
	DeleteFile(WAVFileName);

	PostMessage(hParent, WM_CLOSE, 0, 0);
}
BEGIN_MESSAGE_MAP(LmUIMotionThread, CWinThread)
	ON_THREAD_MESSAGE(WM_MOTION_PLAY,OnMotionPlay)
	ON_THREAD_MESSAGE(WM_MOTION_MOVIE,OnCreateMovie)
END_MESSAGE_MAP()


// LmUIMotionThread 메시지 처리기입니다.
