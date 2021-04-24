// LmUIDlgMainTool.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUIMainToolDlg.h"


// LmUIDlgMainTool ��ȭ �����Դϴ�.

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


// LmUIDlgMainTool �޽��� ó�����Դϴ�.

BOOL LmUIMainToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//TAB�� �� ���̾�α׸� �����Ѵ�
	m_Camera.Create(IDD_CAMERA, &m_tabCtrlTool);
	m_LmUISound.Create(IDD_SOUND, &m_tabCtrlTool);
	m_LmUITalkBox.Create(IDD_TALKBOX, &m_tabCtrlTool);
	m_LmUISnow.Create(IDD_SNOW, &m_tabCtrlTool);
	m_LmUIFireWorks.Create(IDD_FIREWORKS, &m_tabCtrlTool);

	// ���̾�α� �� ��Ʈ�ѿ� ���� �߰��Ѵ� - ���� �����ϴ� ���̾�α׸� �Բ� �����Ѵ�

	m_tabCtrlTool.InsertItem(0, _T("��ǳ��"), &m_LmUITalkBox);
	m_tabCtrlTool.InsertItem(1, _T("����"), &m_LmUISound);
	m_tabCtrlTool.InsertItem(2, _T("��������"), &m_LmUISnow);
	m_tabCtrlTool.InsertItem(3, _T("�Ҳɳ���"), &m_LmUIFireWorks);
	m_tabCtrlTool.InsertItem(4, _T("ī�޶�"), &m_Camera);
	
	// �ʱ� ���� 0������ �����Ѵ�
	m_tabCtrlTool.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL LmUIMainToolDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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