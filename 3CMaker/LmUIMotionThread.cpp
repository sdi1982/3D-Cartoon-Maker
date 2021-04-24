// LmUIMotionThread.cpp : ���� �����Դϴ�.
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
	// TODO: ���⿡�� �� �����忡 ���� �ʱ�ȭ�� �����մϴ�.
	return TRUE;
}

int LmUIMotionThread::ExitInstance()
{
	// TODO: ���⿡�� �� �����忡 ���� ������ �����մϴ�.
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

		// ���� ���
		UINT nSoundPlay = 0;
		SectionVector vSoundPlay;
		vSoundPlay.clear();

		vSoundPlay = TheCartoonMaker->GetNowSoundSection( CurFrame, &nSoundPlay);

		SectionVectorItr itrSection;
		for(itrSection = vSoundPlay.begin(); itrSection != vSoundPlay.end() ; itrSection ++)
		{
			TheCartoonMaker->SoundPlay(itrSection->m_wstrSectionName.c_str());
		}

		// ���� ���������� �Ѿ��
		INT Result = (INT)m_pTimeLIneView->GoNextCaret();
		if(Result<0)
		{
			m_pTimeLIneView->ChangeStopToPlayBtn();
			return;
		}

		//�޽����� �޾ƿ��� �κ�
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
		
	// ���̾�α׿� ������ �����ϱ� ����.
	LmUICreateMovieClipDlg* CreateMovieClopDlg = (LmUICreateMovieClipDlg*)lParam;
	HWND hParent = CreateMovieClopDlg->m_hWnd;
	
	// ���� ���� �̸� ����
	BMPFileName = g_strFilePath + _T("\\temp.bmp");
	WAVFileName = g_strFilePath + _T("\\temp.wav");	
	AVIFileNameTemp = g_strFilePath + _T("\\temp.avi");
	AVIFileName = (LPCTSTR)wParam;

	// AVIFileNameTemp == AVIFileName �϶� ó�� �������.

	// �ͽ� �ʱ�ȭ
	LmMixer Mixer;
	if (!Mixer.Init(TheCartoonMaker->GetSoundLayerCount(),
				TheCartoonMaker->GetFramePerSec(),
				WAVFileName.GetBuffer())){
					return;
	}
	WAVFileName.ReleaseBuffer();

	// Video �ʱ�ȭ
	if (encoder.OpenAVIFile(AVIFileNameTemp, mmioFOURCC('c','v','i','d'), 20, 100)){		//MSVC, cvid
		return;
	}

	while(1)
	{			
		// ���� ������ ������
		INT CurFrame = TheCartoonMaker->GetCurFrame();

		TheCartoonMaker->RenderOnce();
		// Video �۾�
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

		// Sound �۾�
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

	// ������ �۾� ������ �κ�
	encoder.CloseAVIFile();
	DeleteFile(BMPFileName);
	Mixer.CloseWaveFile();

	// Video�� Sound ��ħ
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


// LmUIMotionThread �޽��� ó�����Դϴ�.
