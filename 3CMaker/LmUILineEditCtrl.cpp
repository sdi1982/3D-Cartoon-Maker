// LmUILineEditCtrl.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUILineEditCtrl.h"


// LmUILineEditCtrl

IMPLEMENT_DYNAMIC(LmUILineEditCtrl, CEdit)

LmUILineEditCtrl::LmUILineEditCtrl()
{

}

LmUILineEditCtrl::~LmUILineEditCtrl()
{
}


BEGIN_MESSAGE_MAP(LmUILineEditCtrl, CEdit)
END_MESSAGE_MAP()



// LmUILineEditCtrl �޽��� ó�����Դϴ�.


//
//void LmUILineEditCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
//	if(nChar == VK_RETURN)
//		return;
//	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
//}

BOOL LmUILineEditCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_RETURN)
			 return TRUE;

	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_ESCAPE)
		return TRUE;

	if(pMsg->message == WM_CHAR &&
		pMsg->wParam ==  '/')
		return TRUE;


	return CEdit::PreTranslateMessage(pMsg);
}
