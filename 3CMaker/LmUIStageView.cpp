// StageView.cpp : 구현 파일입니다.
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


// CStageView 그리기입니다.

void LmUIStageView::OnDraw(CDC* pDC)
{
}


// CStageView 진단입니다.

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


// CStageView 메시지 처리기입니다.

void LmUIStageView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

	// 효율적인 처리를 위해서 위자드의 상태에 따라 처리를 분리한다.<미구현>	

	switch(TheCartoonMaker->GetStateOfUI())
	{
	case INIT_STATE: break;
	case WIZARD1: break;
	case WIZARD2: break;
	case WIZARD3:
		// 3 물체를 피킹했다면 물체 회전메뉴를 활성화 시킨다.
		if(TheCartoonMaker->GetThing())
		{
			//[진왕]
			pMainFrame->EnableEditThingsMenu(TRUE);
		}

		else // 피킹을 실패하면 선택한 물체를 없앤다.
		{
			TheCartoonMaker->CancelGotThing();
			pMainFrame->EnableEditThingsMenu(FALSE);
		}

		// 3 물체를 스테이지에 배치한다.
		TheCartoonMaker->PlaceSelectedThing();
		break;
	case WIZARD4:
		// 4 캐릭터를 피킹했다면 캐릭터 회전메뉴를 활성화 시킨다.
		if(TheCartoonMaker->GetChar())
		{
			//[진왕]
			//Scene 만들기 중인가?
			//Scene 만들기 중이면, 캐릭터 회전 메뉴 [편집메뉴에 있음]를 활성화 시킨다. 
			//현재 캐릭터 픽킹이 Wizard가 끝나고 난 이후에는 안된다. 
			if(!GetSceneView()->GetChangeSceneSw())
			{
				pMainFrame->EnableEditCharacterMenu(TRUE);
			}
			else
				//그렇지 않으면, 캐릭터 메뉴를 활성화 시킨다. 
			{
				pMainFrame->EnableEditCharacterMenu(FALSE);
			}
		}
		else // 캐릭터 피킹을 실패하면 선택한 캐릭터를 없앤다.
		{
			TheCartoonMaker->CancelGotChar();

			//캐릭터 피킹을 실패했을 때는 편집 메뉴와 캐릭터 매뉴를 모두 Disable 시킨다. 
			pMainFrame->EnableEditCharacterMenu(FALSE);
		}

		//////////////////////////////////////////////////////////////////////////

		// 4 캐릭터를 스테이지에 배치한다.
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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	// 카메라 처리
	WPARAM wparam = nFlags;

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);

	LPARAM lparam = pt.y;
	lparam = lparam << 16;
	lparam = lparam | pt.x;

	TheCartoonMaker->CameraMsgHandler(GetSafeHwnd(),WM_MOUSEMOVE, wparam, lparam );

	// 피킹보드 마우스 추적 처리
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

	// 카메라 시점 좌표 갱신
	if (TheCartoonMaker->IsTimerOn())
	{
		// 카메라 시점 좌표 갱신
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