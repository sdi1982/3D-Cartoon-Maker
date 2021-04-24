// LmUISnowContentDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUISnowContentDlg.h"


// LmUISnowContentDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(LmUISnowContentDlg, CDialog)

LmUISnowContentDlg::LmUISnowContentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(LmUISnowContentDlg::IDD, pParent)
	, m_strSnowName(_T(""))
	, m_nStartFrm(1)
	, m_nSnowFrmLength(40)
{
	m_isInsert = TRUE;
	m_nOldStartFrm = 0;
}

LmUISnowContentDlg::~LmUISnowContentDlg()
{
}

void LmUISnowContentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strSnowName);
	DDX_Text(pDX, IDC_EDIT3, m_nStartFrm);
	DDX_Text(pDX, IDC_EDIT2, m_nSnowFrmLength);
	DDX_Control(pDX, IDC_EDIT1, m_CtrlSnowName);
	DDX_Control(pDX, IDC_EDIT3, m_ctrlStartFrm);
	DDX_Control(pDX, IDC_EDIT2, m_ctrlSnowFrmLength);
}


BEGIN_MESSAGE_MAP(LmUISnowContentDlg, CDialog)
	ON_BN_CLICKED(IDOK, &LmUISnowContentDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// LmUISnowContentDlg �޽��� ó�����Դϴ�.

BOOL LmUISnowContentDlg::PreTranslateMessage(MSG* pMsg)
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

void LmUISnowContentDlg::SetInputMode( BOOL isInsert )
{
	m_isInsert = isInsert;
}
void LmUISnowContentDlg::OnBnClickedOk()
{
	CString strSnowName;
	CString strStartFrm;
	CString strSnowFrmLength;

	m_CtrlSnowName.GetWindowText(strSnowName);
	m_ctrlStartFrm.GetWindowText(strStartFrm);
	m_ctrlSnowFrmLength.GetWindowText(strSnowFrmLength);

	INT nStartFrm = _wtoi(strStartFrm.GetBuffer(0));
	INT nSnowFrmLength = _wtoi(strSnowFrmLength.GetBuffer(0));
	INT nEndFrm = nStartFrm + nSnowFrmLength - 1;


	if(strSnowName.IsEmpty() ||
		strStartFrm.IsEmpty() ||
		strSnowFrmLength.IsEmpty())
	{
		AfxMessageBox(L"�������� ȿ�� ������ ��� �Է��� �ּ���!");
		return;
	}

	//Insert ��� �� ���, �������Ⱑ �̹� �����ϸ�, �ٽ� �Է� �޾ƶ�!
	// () == 0 �ؾ� �ϴ� ������ ���̾ ������ ������ ��ȯ�ϱ� ����
	CString strError = L"�̹� �����ӿ� �������� ȿ���� ���� �� �ִ� Frame�� �����մϴ�. �ٽ� �Է��� �ּ���!";
	if( m_isInsert && (TheCartoonMaker->CheckSnowSectionArea(nStartFrm, nEndFrm) <= 0) )
	{	
		AfxMessageBox(strError.GetBuffer(0));
		return;
	}

	if( !m_isInsert && (TheCartoonMaker->CheckSnowSectionArea( m_nOldStartFrm, nStartFrm, nEndFrm) == 0) )
	{	
		AfxMessageBox(strError.GetBuffer(0));
		return;
	}

	if(nStartFrm<=0)
	{
		AfxMessageBox(L"���� �������� 1���� �����մϴ�. �ٽ� �Է� ���ּ���!");
		return;
	}

	if(nSnowFrmLength<=0)
	{
		AfxMessageBox(L"���� ������ ���� �ּ� 1 ������ �̻��Դϴ�. �ٽ� �Է� ���ּ���!");
		return;
	}

	OnOK();
}
