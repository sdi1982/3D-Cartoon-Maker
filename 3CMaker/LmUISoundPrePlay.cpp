// LmUISoundPrePlay.cpp : ���� �����Դϴ�.
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
		//������ ����
		if(pSoundPrePlayWnd->m_SoundViewState != SOUNDVIEW_PLAY)
			break;
				
		//�÷��̰� �� �Ǹ� ���� �Ѵ�. �÷��̰� �� �Ǹ� ��ȭ���� ���� ������.
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

// LmUISoundPrePlay ��ȭ �����Դϴ�.

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


// LmUISoundPrePlay �޽��� ó�����Դϴ�.

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
	// [����] ���⿡ �����̴� ���� ���� �о� �� �� ����
	LmSound *pSound = TheSoundManager->GetSound(m_strSoundName.GetBuffer(0));
	if(!pSound)
	{
		CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
		return;
	}

	//�켱 ������ Seek �Լ��� ȣ������..
	//�׷��� �� �Լ��� �÷��� �߿����� ������ ������ �� �𸣰ڳ� ;;;
	DWORD dwPos = (DWORD)m_ctrlSoundSlider.GetPos();
	//m_ctrlSoundSlider.SetPos(nPos);
	pSound->Seek(dwPos);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL LmUISoundPrePlay::OnInitDialog()
{
	CDialog::OnInitDialog();

	// [����] ���⿡ �ʱ� �����̴� ���� ������ ����
	LmSound *pSound = TheSoundManager->GetSound(m_strSoundName);	
	int nMax = pSound->GetTotalTime();

	m_ctrlSoundSlider.SetRange(0,nMax);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void LmUISoundPrePlay::OnBnClickedPlay()
{
	// [����] 
	// m_strSoundName


	// �����̴����� �� �����ϱ� 
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_SoundViewState = SOUNDVIEW_STOP;
	
	LmSound *pSound = TheSoundManager->GetSound(m_strSoundName);
	pSound->Stop();

	WaitForSingleObject(m_pThreadPlayView, INFINITE);
		
	CloseHandle(g_hEventPlayView);

	OnCancel();

}


