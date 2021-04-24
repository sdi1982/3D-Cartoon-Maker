// LmUITalkBoxContentDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUITalkBoxContentDlg.h"
#include "LmUIMainToolFormView.h"


// LmUITalkBoxContentDlg 대화 상자입니다.

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


// LmUITalkBoxContentDlg 메시지 처리기입니다.

void LmUITalkBoxContentDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
		AfxMessageBox(L"말풍선 정보를 모두 입력해 주세요!");
		return;
	}

	// [말풍선] 캐릭터 이름을 얻어온다.

	//Insert 모드 일 경우, 말풍선을 넣을 Frame이 부족하면, 다시 입력 받아라!
	CString strError = L"이미 프레임에 말풍선을 넣을 수 있는 Frame이 부족합니다. 다시 입력해 주세요!";
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
		AfxMessageBox(L"시작 프레임은 1부터 시작합니다. 다시 입력 해주세요!");
		return;
	}

	if(nTalkFrmLength<=0)
	{
		AfxMessageBox(L"지속 프레임 수는 최소 1 프레임 이상입니다. 다시 입력 해주세요!");
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
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.
	CRect rect(128,299,158, 321);
	CBrush * pOldBrush = dc.SelectObject(&m_brush);
	dc.Rectangle(&rect);
	dc.SelectObject(pOldBrush);
}
