// LmUITalkBoxContentDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUITalkBoxContentDlg.h"
#include "LmUIMainToolFormView.h"


// LmUITalkBoxContentDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(LmUITalkBoxContentDlg, CDialog)

LmUITalkBoxContentDlg::LmUITalkBoxContentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(LmUITalkBoxContentDlg::IDD, pParent)
	, m_strTalkContent(_T(""))
	, m_nStartFrm(1)
	, m_nTalkFrmLength(1)
	, m_nTalkBoxStyle(0)
{
	m_nOldStartFrm = 0;
	m_isInsert = TRUE;
}

LmUITalkBoxContentDlg::~LmUITalkBoxContentDlg()
{

}

void LmUITalkBoxContentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strTalkContent);
	DDX_Text(pDX, IDC_EDIT3, m_nStartFrm);
	DDX_Text(pDX, IDC_EDIT2, m_nTalkFrmLength);
	DDX_Control(pDX, IDC_EDIT1, m_ctrlTalkContent);
	DDX_Control(pDX, IDC_EDIT3, m_ctrlStartFrm);
	DDX_Control(pDX, IDC_EDIT2, m_ctrlTalkFrmLength);
	DDX_Radio(pDX, IDC_TALKBOX_BOX1, m_nTalkBoxStyle );
}


BEGIN_MESSAGE_MAP(LmUITalkBoxContentDlg, CDialog)
	ON_BN_CLICKED(IDOK, &LmUITalkBoxContentDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHANGE_COLOR, &LmUITalkBoxContentDlg::OnBnClickedChangeColor)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// LmUITalkBoxContentDlg �޽��� ó�����Դϴ�.

void LmUITalkBoxContentDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strTalkContent;
	CString strStartFrm;
	CString strTalkFrmLength;

	m_ctrlTalkContent.GetWindowText(strTalkContent);
	m_ctrlStartFrm.GetWindowText(strStartFrm);
	m_ctrlTalkFrmLength.GetWindowText(strTalkFrmLength);

	INT nStartFrm = _wtoi(strStartFrm.GetBuffer(0));
	INT nTalkFrmLength = _wtoi(strTalkFrmLength.GetBuffer(0));
	INT nEndFrm = nStartFrm + nTalkFrmLength - 1;


	if(strTalkContent.IsEmpty() ||
		strStartFrm.IsEmpty() ||
		strTalkFrmLength.IsEmpty())
	{
		AfxMessageBox(L"��ǳ�� ������ ��� �Է��� �ּ���!");
		return;
	}

	// [��ǳ��] ĳ���� �̸��� ���´�.

	//Insert ��� �� ���, ��ǳ���� ���� Frame�� �����ϸ�, �ٽ� �Է� �޾ƶ�!
	CString strError = L"�̹� �����ӿ� ��ǳ���� ���� �� �ִ� Frame�� �����մϴ�. �ٽ� �Է��� �ּ���!";
	if( m_isInsert && (TheCartoonMaker->CheckTalkBoxSectionArea(L"dingding#1", nStartFrm, nEndFrm) <= 0) )
	{	
		AfxMessageBox(strError);
		return;
	}

	if( !m_isInsert && (TheCartoonMaker->CheckTalkBoxSectionArea(L"dingding#1", m_nOldStartFrm, nStartFrm, nEndFrm) <= 0) )
	{	
		AfxMessageBox(strError);
		return;
	}

	if(nStartFrm<=0)
	{
		AfxMessageBox(L"���� �������� 1���� �����մϴ�. �ٽ� �Է� ���ּ���!");
		return;
	}

	if(nTalkFrmLength<=0)
	{
		AfxMessageBox(L"���� ������ ���� �ּ� 1 ������ �̻��Դϴ�. �ٽ� �Է� ���ּ���!");
		return;
	}

	UpdateData(TRUE);
	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();
	pParentWnd->ChangeCurDlg(L"MainTool");
	//OnOK();
}

BOOL LmUITalkBoxContentDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_RETURN)
		return TRUE;

	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void LmUITalkBoxContentDlg::SetInputMode( BOOL isInsert )
{
	m_isInsert = isInsert;
}
void LmUITalkBoxContentDlg::OnBnClickedChangeColor()
{
	CColorDialog dlg(m_TalkBoxColor);
	if(dlg.DoModal() == IDOK)
	{
		m_TalkBoxColor = dlg.GetColor();
		m_brush.DeleteObject();
		m_brush.CreateSolidBrush(m_TalkBoxColor);
		Invalidate();
	}
}

void LmUITalkBoxContentDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CDialog::OnPaint()��(��) ȣ������ ���ʽÿ�.
	CRect rect(128,299,158, 321);
	CBrush * pOldBrush = dc.SelectObject(&m_brush);
	dc.Rectangle(&rect);
	dc.SelectObject(pOldBrush);
}
