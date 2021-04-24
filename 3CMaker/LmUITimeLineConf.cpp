// LmUITimeLineConf.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUITimeLineConf.h"


// LmUITimeLineConf 대화 상자입니다.

IMPLEMENT_DYNAMIC(LmUITimeLineConf, CDialog)

LmUITimeLineConf::LmUITimeLineConf(CWnd* pParent /*=NULL*/)
	: CDialog(LmUITimeLineConf::IDD, pParent)
	, m_nFrameLength(0)
{

}

LmUITimeLineConf::~LmUITimeLineConf()
{
}

void LmUITimeLineConf::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_ctrlFrmLength);
	DDX_Text(pDX, IDC_EDIT1, m_nFrameLength);
}


BEGIN_MESSAGE_MAP(LmUITimeLineConf, CDialog)
END_MESSAGE_MAP()


// LmUITimeLineConf 메시지 처리기입니다.

BOOL LmUITimeLineConf::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_ctrlFrmLength.SetLimitText(5); //프레임 수는 99999로 제한한다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL LmUITimeLineConf::PreTranslateMessage(MSG* pMsg)
{
	
	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_RETURN)
		return TRUE;

	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}
