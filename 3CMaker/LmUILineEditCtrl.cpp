// LmUILineEditCtrl.cpp : 구현 파일입니다.
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



// LmUILineEditCtrl 메시지 처리기입니다.


//
//void LmUILineEditCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
//	if(nChar == VK_RETURN)
//		return;
//	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
//}

BOOL LmUILineEditCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
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
