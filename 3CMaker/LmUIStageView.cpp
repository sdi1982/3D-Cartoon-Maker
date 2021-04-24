// StageView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUIStageView.h"
#include "MainFrm.h"
#include "LmUIMainToolDlg.h"
#include "LmUIMainToolFormView.h"
#include "LmUISceneView.h"

// CStageView

IMPLEMENT_DYNCREATE(LmUIStageView, CView)

LmUIStageView::LmUIStageView()
{

}

LmUIStageView::~LmUIStageView()
{
}

BEGIN_MESSAGE_MAP(LmUIStageView, CView)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_ACTIVATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CStageView �׸����Դϴ�.

void LmUIStageView::OnDraw(CDC* pDC)
{
}


// CStageView �����Դϴ�.

#ifdef _DEBUG
void LmUIStageView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void LmUIStageView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CStageView �޽��� ó�����Դϴ�.

void LmUIStageView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CRect rect;
	GetClientRect(&rect);

	// Initialize Direct3D (e.g. acquire a IDirect3DDevice9 poniter).
	HRESULT hr = TheCartoonMaker->InitD3D( GetSafeHwnd(), rect.right, rect.bottom, D3DDEVTYPE_HAL);
	if(FAILED(hr))
	{
		MessageBox(L"InitD3D() - Failed", L"Error");
		::PostQuitMessage(0);
	}
	
	hr = TheCartoonMaker->SetupD3D(rect.right, rect.bottom);
	if(FAILED(hr))
	{
		MessageBox(L"SetupD3D() - Failed", L"Error");
		::PostQuitMessage(0);
	}

	SetTimer(1, 10, 0);
}

void LmUIStageView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	TheCartoonMaker->ResizeD3D(cx, cy);
	TheCartoonMaker->SetScreenSize(cx, cy);
}

BOOL LmUIStageView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	//return CView::OnEraseBkgnd(pDC);
}

void LmUIStageView::OnDestroy()
{
	CView::OnDestroy();

	KillTimer(1);
	TheCartoonMaker->DestroyD3D();
}
void LmUIStageView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

	// ȿ������ ó���� ���ؼ� ���ڵ��� ���¿� ���� ó���� �и��Ѵ�.<�̱���>	

	switch(TheCartoonMaker->GetStateOfUI())
	{
	case INIT_STATE: break;
	case WIZARD1: break;
	case WIZARD2: break;
	case WIZARD3:
		// 3 ��ü�� ��ŷ�ߴٸ� ��ü ȸ���޴��� Ȱ��ȭ ��Ų��.
		if(TheCartoonMaker->GetThing())
		{
			//[����]
			pMainFrame->EnableEditThingsMenu(TRUE);
		}

		else // ��ŷ�� �����ϸ� ������ ��ü�� ���ش�.
		{
			TheCartoonMaker->CancelGotThing();
			pMainFrame->EnableEditThingsMenu(FALSE);
		}

		// 3 ��ü�� ���������� ��ġ�Ѵ�.
		TheCartoonMaker->PlaceSelectedThing();
		break;
	case WIZARD4:
		// 4 ĳ���͸� ��ŷ�ߴٸ� ĳ���� ȸ���޴��� Ȱ��ȭ ��Ų��.
		if(TheCartoonMaker->GetChar())
		{
			//[����]
			//Scene ����� ���ΰ�?
			//Scene ����� ���̸�, ĳ���� ȸ�� �޴� [�����޴��� ����]�� Ȱ��ȭ ��Ų��. 
			//���� ĳ���� ��ŷ�� Wizard�� ������ �� ���Ŀ��� �ȵȴ�. 
			if(!GetSceneView()->GetChangeSceneSw())
			{
				pMainFrame->EnableEditCharacterMenu(TRUE);
			}
			else
				//�׷��� ������, ĳ���� �޴��� Ȱ��ȭ ��Ų��. 
			{
				pMainFrame->EnableEditCharacterMenu(FALSE);
			}
		}
		else // ĳ���� ��ŷ�� �����ϸ� ������ ĳ���͸� ���ش�.
		{
			TheCartoonMaker->CancelGotChar();

			//ĳ���� ��ŷ�� �������� ���� ���� �޴��� ĳ���� �Ŵ��� ��� Disable ��Ų��. 
			pMainFrame->EnableEditCharacterMenu(FALSE);
		}

		//////////////////////////////////////////////////////////////////////////

		// 4 ĳ���͸� ���������� ��ġ�Ѵ�.
		TheCartoonMaker->PlaceSelectedChar();
		break;

	case SCENE_CHANGED:
	case TIMELINE_CHANGED:
	default: 
		if(TheCartoonMaker->GetChar())
		{
			ThePickBoard->Show(TRUE);
			pMainFrame->EnableCharacterMenu(TRUE);
			pMainFrame->EnableInsertTalkBoxBtn(TRUE);
		}
		else
		{
			ThePickBoard->Show(FALSE);
			pMainFrame->EnableCharacterMenu(FALSE);
			pMainFrame->EnableInsertTalkBoxBtn(FALSE);
		}

		break;
	}


	CView::OnLButtonDown(nFlags, point);
}

BOOL LmUIStageView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	WPARAM wparam = zDelta;
	wparam = wparam << 16;
	wparam = wparam | nFlags;

	POINT pt1;
	GetCursorPos(&pt1);
	ScreenToClient(&pt1);

	LPARAM lparam = pt1.y;
	lparam = lparam << 16;
	lparam = lparam | pt1.x;

	TheCartoonMaker->CameraMsgHandler(GetSafeHwnd(),WM_MOUSEWHEEL, wparam, lparam );

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void LmUIStageView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	WPARAM wparam = nFlags;

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);

	LPARAM lparam = pt.y;
	lparam = lparam << 16;
	lparam = lparam | pt.x;

	TheCartoonMaker->CameraMsgHandler(GetSafeHwnd(),WM_RBUTTONDOWN, wparam, lparam );

	CView::OnRButtonDown(nFlags, point);
}

void LmUIStageView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	WPARAM wparam = nFlags;

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);

	LPARAM lparam = pt.y;
	lparam = lparam << 16;
	lparam = lparam | pt.x;

	TheCartoonMaker->CameraMsgHandler(GetSafeHwnd(),WM_RBUTTONUP, wparam, lparam );

	CView::OnRButtonUp(nFlags, point);
}

void LmUIStageView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	// ī�޶� ó��
	WPARAM wparam = nFlags;

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);

	LPARAM lparam = pt.y;
	lparam = lparam << 16;
	lparam = lparam | pt.x;

	TheCartoonMaker->CameraMsgHandler(GetSafeHwnd(),WM_MOUSEMOVE, wparam, lparam );

	// ��ŷ���� ���콺 ���� ó��
	RECT rt;
	GetClientRect(&rt);

	TheCartoonMaker->TrackingMouse(pt.x, pt.y, rt.bottom, rt.right);

	//CView::OnMouseMove(nFlags, point);
}
void LmUIStageView::OnTimer(UINT_PTR nIDEvent)
{
	// Save last time.
	static float lastTime = (float)timeGetTime();
	// Compute time now.
	float currentTime = (float)timeGetTime();

	// Compute the difference: time elapsed in seconds.
	float deltaTime = (currentTime - lastTime) * 0.001f;
	// Last time is now current time.
	lastTime = currentTime;

	// ī�޶� ���� ��ǥ ����
	if (TheCartoonMaker->IsTimerOn())
	{
		// ī�޶� ���� ��ǥ ����
		static D3DXVECTOR3 vOldEye(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 vEye = *TheCartoonMaker->GetCameraEyePt();

		if(vOldEye != vEye)
		{
			FLOAT fDist = TheCartoonMaker->GetCameraDist();
			CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
			LmUIMainToolFormView* pToolFormView= (LmUIMainToolFormView*) ( pMainFrame->m_wndSplitUpCol.GetPane(0,1) );
			pToolFormView->m_MainToolDlg.m_Camera.SetCameraUIValue(vEye.x, vEye.y, vEye.z, fDist);

			vOldEye = vEye;
		}

		TheCartoonMaker->UpdateD3D(1.0f);
		TheCartoonMaker->RenderD3D();
	}

	CView::OnTimer(nIDEvent);
}


LmUISceneView* LmUIStageView::GetSceneView()
{
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
	LmUISceneView* pSceneView = (LmUISceneView*) ( pMainFrame->m_wndSplitUpColExpand.GetPane(0,0) );
	return pSceneView;
}