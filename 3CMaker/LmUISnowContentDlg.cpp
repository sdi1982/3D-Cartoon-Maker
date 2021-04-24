// LmUISnowContentDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUISnowContentDlg.h"


// LmUISnowContentDlg 대화 상자입니다.

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


// LmUISnowContentDlg 메시지 처리기입니다.

BOOL LmUISnowContentDlg::PreTranslateMessage(MSG* pMsg)
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
		AfxMessageBox(L"눈내리기 효과 정보를 모두 입력해 주세요!");
		return;
	}

	//Insert 모드 일 경우, 눈내리기가 이미 존재하면, 다시 입력 받아라!
	// () == 0 해야 하는 이유는 레이어가 없을때 음수를 반환하기 때문
	CString strError = L"이미 프레임에 눈내리기 효과를 넣을 수 있는 Frame이 부족합니다. 다시 입력해 주세요!";
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
		AfxMessageBox(L"시작 프레임은 1부터 시작합니다. 다시 입력 해주세요!");
		return;
	}

	if(nSnowFrmLength<=0)
	{
		AfxMessageBox(L"지속 프레임 수는 최소 1 프레임 이상입니다. 다시 입력 해주세요!");
		return;
	}

	OnOK();
}
