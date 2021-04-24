// ThinSplitterWnd.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUIThinSplitterWnd.h"

#include "LmUISceneView.h"
#include "MainFrm.h"

// CThinSplitterWnd

IMPLEMENT_DYNAMIC(LmUIThinSplitterWnd, CSplitterWnd)

LmUIThinSplitterWnd::LmUIThinSplitterWnd()
{
	m_cxSplitter=5; 
	m_cySplitter=5; 
	m_cxBorderShare=0;
	m_cyBorderShare=0;
	m_cxSplitterGap=5;
	m_cySplitterGap=5;

}

LmUIThinSplitterWnd::~LmUIThinSplitterWnd()
{
}


BEGIN_MESSAGE_MAP(LmUIThinSplitterWnd, CSplitterWnd)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()



// CThinSplitterWnd 메시지 처리기입니다.


void LmUIThinSplitterWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	
	//경계선을 클릭한 상태에서 마우스가 움직일 경우
	if(nFlags && WM_LBUTTONDOWN)
	{
		CRect rect;
		GetClientRect(&rect);
		ClientToScreen(&rect);

		CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
		LmUISceneView* pSceneView = (LmUISceneView*) ( pMainFrame->m_wndSplitUpColExpand.GetPane(0,0) );
		
		CPoint ptScreen = point;
		ClientToScreen(&ptScreen);
		
		INT nLeftPoint = pSceneView->m_rectWin.left;
		INT nCurrentWidth = ptScreen.x -  nLeftPoint;

		//TRACE(L"%d\r\n", nCurrentWidth);

		//여기에다가 조건을 써야 한다.
		if( abs(nCurrentWidth) >= 240)
		{
			point.x = 240;
		}

		
	}
	CSplitterWnd::OnMouseMove(nFlags, point);
}

