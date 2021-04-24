// LmUIDlgMainTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUIMainToolDlg.h"


// LmUIDlgMainTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(LmUIMainToolDlg, CDialog)

LmUIMainToolDlg::LmUIMainToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(LmUIMainToolDlg::IDD, pParent)
{

}

LmUIMainToolDlg::~LmUIMainToolDlg()
{
}

void LmUIMainToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TABCTL, m_tabCtrlTool);
}


BEGIN_MESSAGE_MAP(LmUIMainToolDlg, CDialog)
END_MESSAGE_MAP()


// LmUIDlgMainTool 메시지 처리기입니다.

BOOL LmUIMainToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//TAB에 들어갈 다이얼로그를 생성한다
	m_Camera.Create(IDD_CAMERA, &m_tabCtrlTool);
	m_LmUISound.Create(IDD_SOUND, &m_tabCtrlTool);
	m_LmUITalkBox.Create(IDD_TALKBOX, &m_tabCtrlTool);
	m_LmUISnow.Create(IDD_SNOW, &m_tabCtrlTool);
	m_LmUIFireWorks.Create(IDD_FIREWORKS, &m_tabCtrlTool);

	// 다이얼로그 탭 컨트롤에 탭을 추가한다 - 탭을 구현하는 다이얼로그를 함께 전달한다

	m_tabCtrlTool.InsertItem(0, _T("말풍선"), &m_LmUITalkBox);
	m_tabCtrlTool.InsertItem(1, _T("사운드"), &m_LmUISound);
	m_tabCtrlTool.InsertItem(2, _T("눈내리기"), &m_LmUISnow);
	m_tabCtrlTool.InsertItem(3, _T("불꽃놀이"), &m_LmUIFireWorks);
	m_tabCtrlTool.InsertItem(4, _T("카메라"), &m_Camera);
	
	// 초기 탭을 0번으로 설정한다
	m_tabCtrlTool.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL LmUIMainToolDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_RETURN)
		return TRUE;

	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void LmUIMainToolDlg::InitToolList()
{
	m_LmUISound.InitSoundList();
	m_LmUITalkBox.InitTalkBoxList();
	m_LmUISnow.InitSnowList();
}

VOID LmUIMainToolDlg::InsertTalkBox( LPCWSTR pcwsCharacterName, UINT uFrameStart, UINT uFrameEnd, LPCWSTR pcwsTalkContent,  LPCWSTR pcwsTalkBoxStyle )
{
	m_LmUITalkBox.InsertTalkBoxContent(  pcwsCharacterName,  uFrameStart,  uFrameEnd,  pcwsTalkContent, pcwsTalkBoxStyle );
}

VOID LmUIMainToolDlg::InsertSnow( UINT uFrameStart, UINT uFrameEnd, LPCWSTR pcwsSnowName )
{
	m_LmUISnow.InsertSnowContent( uFrameStart, uFrameEnd, pcwsSnowName );
}

VOID LmUIMainToolDlg::InsertSound( LPCWSTR pcwsSoundName, UINT uStartFrame )
{
	m_LmUISound.InsertSoundContent( pcwsSoundName, uStartFrame );
}