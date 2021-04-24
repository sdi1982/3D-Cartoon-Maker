// LmUISoundPrePlay.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUISoundPrePlay.h"

HANDLE g_hEventPlayView;

UINT ThreadPlayView(LPVOID pParam)
{
	LmUISoundPrePlay *pSoundPrePlayWnd = (LmUISoundPrePlay *)pParam;
	while(1)
	{
		Sleep(10);
		//쓰레드 종료
		if(pSoundPrePlayWnd->m_SoundViewState != SOUNDVIEW_PLAY)
			break;
				
		//플레이가 다 되면 종료 한다. 플레이가 다 되면 변화량이 없기 때문임.
		if(!TheCartoonMaker->IsPlay(pSoundPrePlayWnd->m_strSoundName))
			break;

		HWND hWnd = pSoundPrePlayWnd->GetSafeHwnd();
		if(!hWnd)
			break;

		pSoundPrePlayWnd->m_ctrlSoundSlider.SetPos(TheCartoonMaker->GetCurrentPos(pSoundPrePlayWnd->m_strSoundName));
		
		TRACE(L"dSoundCaret : %d %d \r\n", TheCartoonMaker->GetCurrentPos(pSoundPrePlayWnd->m_strSoundName), TheCartoonMaker->GetTotalTime(pSoundPrePlayWnd->m_strSoundName));		
	}
	SetEvent(g_hEventPlayView);
	return 0;

}

// LmUISoundPrePlay 대화 상자입니다.

IMPLEMENT_DYNAMIC(LmUISoundPrePlay, CDialog)

LmUISoundPrePlay::LmUISoundPrePlay(CWnd* pParent /*=NULL*/)
	: CDialog(LmUISoundPrePlay::IDD, pParent)
	, m_strSoundName(_T(""))
{

}

LmUISoundPrePlay::~LmUISoundPrePlay()
{
}

void LmUISoundPrePlay::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FILE_FULL_NAME, m_strSoundName);
	DDX_Control(pDX, IDC_SLIDER1, m_ctrlSoundSlider);
}


BEGIN_MESSAGE_MAP(LmUISoundPrePlay, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(ID_PLAY, &LmUISoundPrePlay::OnBnClickedPlay)
	ON_BN_CLICKED(IDCANCEL, &LmUISoundPrePlay::OnBnClickedCancel)
END_MESSAGE_MAP()


// LmUISoundPrePlay 메시지 처리기입니다.

BOOL LmUISoundPrePlay::PreTranslateMessage(MSG* pMsg)
{

	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_RETURN)
		return TRUE;

	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void LmUISoundPrePlay::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// [진우] 여기에 슬라이더 바의 값을 읽어 올 수 있음
	LmSound *pSound = TheSoundManager->GetSound(m_strSoundName.GetBuffer(0));
	if(!pSound)
	{
		CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
		return;
	}

	//우선 진우의 Seek 함수를 호출했음..
	//그런데 이 함수가 플레이 중에서만 가능한 것인지 잘 모르겠네 ;;;
	DWORD dwPos = (DWORD)m_ctrlSoundSlider.GetPos();
	//m_ctrlSoundSlider.SetPos(nPos);
	pSound->Seek(dwPos);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL LmUISoundPrePlay::OnInitDialog()
{
	CDialog::OnInitDialog();

	// [진우] 여기에 초기 슬라이더 바의 범위를 지정
	LmSound *pSound = TheSoundManager->GetSound(m_strSoundName);	
	int nMax = pSound->GetTotalTime();

	m_ctrlSoundSlider.SetRange(0,nMax);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void LmUISoundPrePlay::OnBnClickedPlay()
{
	// [진우] 
	// m_strSoundName


	// 슬라이더바의 값 설정하기 
	//m_ctrlSoundSlider.SetPos(INT var);

	LmSound *pSound = TheSoundManager->GetSound(m_strSoundName);	
	pSound->Play();

	g_hEventPlayView = CreateEvent(NULL, FALSE, TRUE, NULL);
	m_SoundViewState = SOUNDVIEW_PLAY;
	ResetEvent(g_hEventPlayView);

	m_pThreadPlayView = AfxBeginThread(ThreadPlayView, this);
	
}


void LmUISoundPrePlay::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_SoundViewState = SOUNDVIEW_STOP;
	
	LmSound *pSound = TheSoundManager->GetSound(m_strSoundName);
	pSound->Stop();

	WaitForSingleObject(m_pThreadPlayView, INFINITE);
		
	CloseHandle(g_hEventPlayView);

	OnCancel();

}


