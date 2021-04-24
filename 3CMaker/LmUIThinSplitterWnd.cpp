// ThinSplitterWnd.cpp : ���� �����Դϴ�.
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



// CThinSplitterWnd �޽��� ó�����Դϴ�.


void LmUIThinSplitterWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	
	//��輱�� Ŭ���� ���¿��� ���콺�� ������ ���
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

		//���⿡�ٰ� ������ ��� �Ѵ�.
		if( abs(nCurrentWidth) >= 240)
		{
			point.x = 240;
		}

		
	}
	CSplitterWnd::OnMouseMove(nFlags, point);
}

