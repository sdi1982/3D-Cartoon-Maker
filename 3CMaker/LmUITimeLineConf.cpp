// LmUITimeLineConf.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUITimeLineConf.h"


// LmUITimeLineConf ��ȭ �����Դϴ�.

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


// LmUITimeLineConf �޽��� ó�����Դϴ�.

BOOL LmUITimeLineConf::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_ctrlFrmLength.SetLimitText(5); //������ ���� 99999�� �����Ѵ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
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
