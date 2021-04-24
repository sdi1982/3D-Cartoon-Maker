// MainFrm.cpp : CMainFrame 클래스의 구현
//



#include "stdafx.h"
#include "3CMaker.h"

#include "MainFrm.h"

#include "LmUISceneView.h"
#include "LmUIStageView.h"
#include "LmUITimeLineView.h"
#include "LmUIMainToolFormView.h"
#include "LmUICreateMovieClipDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_NCHITTEST()
	ON_WM_ACTIVATE()

	ON_COMMAND(ID_MAKE_SCENE, &CMainFrame::OnMakeScene)
	ON_COMMAND(ID_SCENE_DELETE, &CMainFrame::OnSceneDelete)
	ON_COMMAND(ID_REMOVE_THINGS, &CMainFrame::OnThingDelete)
	ON_COMMAND(ID_TURN_THINGS, &CMainFrame::OnThingsTurn)
	ON_COMMAND(ID_REMOVE_BACKGROUND, &CMainFrame::OnRemoveBackground)
	ON_COMMAND(ID_TURN_CHARACTER, &CMainFrame::OnTurnCharacter)
	ON_COMMAND(ID_REMOVE_CHARACTER, &CMainFrame::OnRemoveCharacter)
	ON_COMMAND(ID_SCENE_CUT, &CMainFrame::OnSceneCut)
	ON_COMMAND(ID_MOVIE_CREATE, &CMainFrame::OnMovieCreate)
	ON_COMMAND(ID_TURN_LEFT, &CMainFrame::OnTurnLeft)
	ON_COMMAND(ID_TURN_RIGHT, &CMainFrame::OnTurnRight)
	ON_COMMAND(ID_WALK, &CMainFrame::OnWalk)
	ON_COMMAND(ID_SHAKE_HAND, &CMainFrame::OnShakeHand)
	ON_COMMAND(ID_BOW, &CMainFrame::OnBow)
	ON_COMMAND(ID_JUMP, &CMainFrame::OnJump)
	ON_COMMAND(ID_TURNNING, &CMainFrame::OnTurnning)
	ON_COMMAND(ID_DEL_ACTION, &CMainFrame::OnDelAction)
	ON_COMMAND(ID_SAVE_SCREEN, &CMainFrame::OnSaveScreen)
	ON_COMMAND(ID_SCENE_COPY, &CMainFrame::OnSceneCopy)
	ON_COMMAND(ID_SCENE_PASTE, &CMainFrame::OnScenePaste)
	ON_COMMAND(ID_LM_FILE_NEW, &CMainFrame::OnLmFileNew)
	ON_COMMAND(ID_LM_FILE_OPEN, &CMainFrame::OnLmFileOpen)
	ON_COMMAND(ID_LM_FILE_SAVE, &CMainFrame::OnLmFileSave)
	ON_COMMAND(ID_LM_FILE_SAVE_AS, &CMainFrame::OnLmFileSaveAs)
	//ON_COMMAND(ID_INSERT_TALKBOX, &CMainFrame::OnInsertTalkbox)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//우선에 툴바는 사용 안한다.
	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
	//	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	//{
	//	TRACE0("도구 모음을 만들지 못했습니다.\n");
	//	return -1;      // 만들지 못했습니다.
	//}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndToolBar);

	//int display_height = GetSystemMetrics(SM_CYSCREEN);
	//int display_width = GetSystemMetrics(SM_CXSCREEN);

	//::SetWindowPos(this->m_hWnd, HWND_NOTOPMOST,0, 0, display_width,display_height ,SWP_DRAWFRAME);

//	SendMessage(WM_SYSCOMMAND,SC_MAXIMIZE); /* 최대화*/

	// 카툰메이커 리소스 리스트 등록
	TheCartoonMaker->Read3cmFile();

	// Sound Handle 등록
	TheCartoonMaker->SetHMainWnd(m_hWnd);

	CFileFind finder;
	BOOL bWorking = finder.FindFile(L".");
	bWorking = finder.FindNextFile();
	g_strFilePath = finder.GetFilePath();

	//이 변수에 FALSE값이 아니면 메뉴를 마음대로 제어할 수 없음.
	m_bAutoMenuEnable = FALSE;
	
	//편집 메뉴는 Disable로 한다.
	EnableEditMenu(FALSE);
	EnableCharacterMenu(FALSE);

	//동영상 메뉴는 Disable로 한다. 
	EnableMovieClipMenu(FALSE);
	
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	//cs.style &= WS_MAXIMIZEBOX;
	//cs.style = WS_OVERLAPPED | WS_SYSMENU | WS_BORDER;
	//cs.style |= WS_MAXIMIZE;

	return TRUE;
}


// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 메시지 처리기



//분할 뷰를 생성한다.
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	int display_height = GetSystemMetrics(SM_CYSCREEN);
	int display_width = GetSystemMetrics(SM_CXSCREEN);

	//초기 1행 1열이 생성되는데 가로로 갈라서 2행을 만듬
	if(!m_wndSplitRow.CreateStatic(this, 2 , 1))
	{
		TRACE0("Fail to create static m_wmdSplitter object");
		return FALSE;
	}

 	//1행 1열을 다시 세로로 나눠서 2개의 열을 만듬.
	if(!m_wndSplitUpCol.CreateStatic(&m_wndSplitRow, 1,2, WS_CHILD | WS_VISIBLE ,
		m_wndSplitRow.IdFromRowCol(0,0))  )
	{
		TRACE0("Fail to create static m_wndSplitRow_expo object");
		return FALSE;
	}

	//초기 2행2열이 생성되는데, 거기에서 (0,0)을 세로로 나눠서 3열만듬,
	if(!m_wndSplitUpColExpand.CreateStatic(&m_wndSplitUpCol, 1,2, WS_CHILD | WS_VISIBLE ,
		m_wndSplitUpCol.IdFromRowCol(0,0))  )
	{
		TRACE0("Fail to create static m_wndSplitUpColExpand object");
		return FALSE;
	}

	m_wndSplitUpColExpand.CreateView(0, 0, RUNTIME_CLASS(LmUISceneView), CSize(200, 300), pContext);
	m_wndSplitUpColExpand.CreateView(0, 1, RUNTIME_CLASS(LmUIStageView), CSize(display_width-650, 300), pContext);
	m_wndSplitUpCol.CreateView(0, 1, RUNTIME_CLASS(LmUIMainToolFormView), CSize(200, 300), pContext);
	
	m_wndSplitRow.CreateView(1, 0, RUNTIME_CLASS(LmUITimeLineView), CSize(300, 300), pContext);
	SetActiveView((CView *)m_wndSplitUpColExpand.GetPane(0,0));

	
	m_wndSplitUpCol.SetColumnInfo(0,display_width-450,10);
	//행의 크기를 제한한다.
	m_wndSplitRow.SetRowInfo(0,display_height-400,10);

	m_wndSplitRow.RecalcLayout();
	m_wndSplitUpCol.RecalcLayout();

	//시스템 메뉴에서 이동 막기
/*	CMenu* pSysMenu = GetSystemMenu(FALSE);

	if (pSysMenu != NULL)
	{
		pSysMenu->RemoveMenu(SC_MOVE, MF_BYCOMMAND);
	} 

*/
	return TRUE;
}



LRESULT CMainFrame::OnNcHitTest(CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	
/*	//창의 이동을 막기 위해 -  하지만 종료도 되지 않는다는거 !!
	int display_height = GetSystemMetrics(SM_CYSCREEN);
	int display_width = GetSystemMetrics(SM_CXSCREEN);
	CRect rectTest;
	rectTest.SetRect(0, 0, display_width-30,30);
	
	//지정 영역(종료버튼이 있는 곳) 밖에 있는가?
	if(rectTest.PtInRect(point))
	   return FALSE;
*/
	return CFrameWnd::OnNcHitTest(point);
}

void CMainFrame::OnMakeScene()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	LmUIMainToolFormView* pToolFormView = (LmUIMainToolFormView*) m_wndSplitUpCol.GetPane(0,1) ;

	// Wizard에서 완료 버튼을 누르면 그때 Scene이 추가 되는 방식으로 코딩한다.!
	pToolFormView->InitWizard();

	//////////////////////////////////////////////////////////////////////////
	// 테스트
	//OnPickCharacterMenu();
	//////////////////////////////////////////////////////////////////////////
	//배경을. 초원으로 맞춰주세요~~
	//TheCartoonMaker->
	//씬에 관련된 메뉴를 Disable한다.

	//모든 메뉴를 Disable한다.
	EnableAllMenu(FALSE);

	//[편집] 메뉴 활성화
	EnableEditMenu(TRUE);

	EnableEditCharacterMenu(FALSE);
	EnableEditThingsMenu(FALSE);
	EnableEditBackgroundMenu(FALSE);

//	EnableCharacterMenu(TRUE);
	TheCartoonMaker->SetStateOfUI(WIZARD1);
}

void CMainFrame::OnSceneDelete()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	LmUISceneView* pSceneView = (LmUISceneView*) m_wndSplitUpColExpand.GetPane(0,0);

	// [동인] 씬 삭제
	CRectPicture data;
	pSceneView->GetThumbNailCaret(data);
	
	TheCartoonMaker->DeleteScene(data.strSceneName);

	// [진왕] UI에서 삭제
	pSceneView->RemoveThumbNail( pSceneView->m_nCaret );
	pSceneView->Invalidate();

	LmUIMainToolFormView* pToolFormView = (LmUIMainToolFormView*) m_wndSplitUpCol.GetPane(0,1) ;
	INT nThumbNailCnt = pSceneView->GetThumNailCount();

	//씬이 하나도 없으면 Main Tool을 띄우지 않는다. 
	if(!nThumbNailCnt)
		pToolFormView->ChangeCurDlg(L"MainNotice");
	else
		pToolFormView->ChangeCurDlg(L"MainTool");
}


void CMainFrame::OnThingDelete()
{
	//[물체] -> [삭제하기] 메뉴가 선택되었을 때 실행되는 멤버 함수임.
	
	//[동인] Things가 지워져야 할 때 필요한 함수를 호출하면 돼

	TheCartoonMaker->DeleteGotThing();
	return;
}

void CMainFrame::OnThingsTurn()
{
	//[회전하기]가 선택되었을때 호출되는 멤버 함수임.
	//[동인] 선택된 물체를 회전시킨다.
	TheCartoonMaker->RotateGotThing();
	return;
}

typedef struct CharacterMenu
{
	CString strMenuName;
	UINT	nMenuID;
}ActionMenu;

ActionMenu menuData[] = { {L"asdf",3000}, {L"asdf2",3001}, {L"asdf3",3002}};

void CMainFrame::OnPickCharacterMenu()
{
	//선택된 캐릭터 마다 다른 메뉴가 뜨도록 하는 함수임.
	// 어떤 캐릭터가 선택되었으며,
	// 그 캐릭터가 가진 동작이 무엇이 있는지에 대한 
	// 목록이 필요함.
	// 목록의 스타일은 
	// [Action Name] , [Action Name의 ID값] 2개의 값이 필수임.

	CMenu *pMenu = GetMenu();
	CMenu newCharacterMenu;

	newCharacterMenu.CreatePopupMenu();
	for(int nCnt = 0; nCnt < 2 ; nCnt++)
		newCharacterMenu.AppendMenu(MF_STRING & (~MF_GRAYED), menuData[nCnt].nMenuID, menuData[nCnt].strMenuName);

	//pMenu->RemoveMenu(2);
	pMenu->ModifyMenu(2, MF_POPUP | MF_BYPOSITION | MF_ENABLED, (UINT_PTR)newCharacterMenu.Detach(), L"캐릭터");
	//pMenu->AppendMenu(MF_POPUP, (UINT_PTR)newCharacterMenu.Detach(), L"캐릭터");

	SetMenu(pMenu);

}

void CMainFrame::OnCharacterAction()
{
	//캐릭터 액션	
	return;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message == WM_COMMAND)
	{
		//여기다가 총 액션의 수를 제한하고 OnCharacterAction() 함수 호출
		if(LOWORD(pMsg->wParam) >= 3000 && LOWORD(pMsg->wParam) <= 3001)
			OnCharacterAction();
	}
	return CFrameWnd::PreTranslateMessage(pMsg);
}

//상위 메뉴를 활성화 시키거나 비활성화 시킨다.
void CMainFrame::EnablePopMenu( UINT nMenuID, BOOL bEnableMenu )
{
	CMenu * pMenu = GetMenu();
	if(bEnableMenu)
		pMenu->EnableMenuItem(nMenuID, MF_BYPOSITION | MF_ENABLED );
	else
		pMenu->EnableMenuItem(nMenuID, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
	SetMenu(pMenu);
	return; 
}

void CMainFrame::EnableFileMenu( BOOL bEnableMenu )
{
	EnablePopMenu(0,bEnableMenu );
	return;
}


void CMainFrame::EnableSceneMenu( BOOL bEnableMenu )
{
	EnablePopMenu(1,bEnableMenu );
	return;
}

void CMainFrame::EnableEditMenu( BOOL bEnableMenu )
{
	EnablePopMenu(2, bEnableMenu );
	return;
}

void CMainFrame::EnableCharacterMenu( BOOL bEnableMenu )
{
	EnablePopMenu(3, bEnableMenu );
	return;
}

void CMainFrame::EnableMovieClipMenu( BOOL bEnableMenu )
{
	EnablePopMenu(4, bEnableMenu );
	return;
}

void CMainFrame::EnableAllMenu(BOOL bEnableMenu)
{
	EnableFileMenu( bEnableMenu );
	EnableSceneMenu( bEnableMenu );
	EnableCharacterMenu( bEnableMenu );
	EnableEditMenu( bEnableMenu );
	EnableMovieClipMenu( bEnableMenu );
}

VOID CMainFrame::EnableEditBackgroundMenu( BOOL bEnableMenu )
{
	EnableSubMenu(2,0,bEnableMenu);
	return; 
}

void CMainFrame::EnableSubMenu( UINT nPopMenuIndex, UINT nSubMenuIndex, BOOL bEnableMenu )
{
	CMenu *pMenu = GetMenu();
	CMenu *pEditMenu = pMenu->GetSubMenu(nPopMenuIndex);

	if(bEnableMenu)
		pEditMenu->EnableMenuItem(nSubMenuIndex, MF_BYPOSITION | MF_ENABLED );
	else
		pEditMenu->EnableMenuItem(nSubMenuIndex, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);

	SetMenu(pMenu);
}

VOID CMainFrame::EnableEditCharacterMenu( BOOL bEnableMenu )
{
	EnableSubMenu(2,5,bEnableMenu);
	EnableSubMenu(2,6,bEnableMenu);
}

VOID CMainFrame::EnableEditThingsMenu( BOOL bEnableMenu )
{
	EnableSubMenu(2,2,bEnableMenu);
	EnableSubMenu(2,3,bEnableMenu);
}


void CMainFrame::OnRemoveBackground()
{
	TheCartoonMaker->DeleteBGNowScene();
}


void CMainFrame::OnTurnCharacter()
{
	TheCartoonMaker->RotateGotChar();
}

void CMainFrame::OnRemoveCharacter()
{
	TheCartoonMaker->DeleteGotChar();
}

void CMainFrame::OnMovieCreate()
{
	// 동영상 만들기 메뉴 클릭 했을때.
	LmUICreateMovieClipDlg dlg;

	TheCartoonMaker->SetMakeSize(480, 320);
	TheCartoonMaker->SetMakeMode(TRUE);	

	dlg.DoModal();

	Sleep(1000);
	TheCartoonMaker->SetMakeMode(FALSE);
	
}

void CMainFrame::OnTurnLeft()
{
	if(!TheCartoonMaker->InsertAction(ACT_TURN_LEFT))
		AfxMessageBox(L"액션을 추가할 수 없습니다.");
}

void CMainFrame::OnTurnRight()
{
	if(!TheCartoonMaker->InsertAction(ACT_TURN_RIGHT))
		AfxMessageBox(L"액션을 추가할 수 없습니다.");
}

void CMainFrame::OnWalk()
{
	if(!TheCartoonMaker->InsertAction(ACT_WALK))
		AfxMessageBox(L"액션을 추가할 수 없습니다.");
}

void CMainFrame::OnShakeHand()
{
	if(!TheCartoonMaker->InsertAction(ACT_SHAKE_HAND))
		AfxMessageBox(L"액션을 추가할 수 없습니다.");
}

void CMainFrame::OnBow()
{
	if(!TheCartoonMaker->InsertAction(ACT_BOW))
		AfxMessageBox(L"액션을 추가할 수 없습니다.");
}

void CMainFrame::OnTurnning()
{
	if(!TheCartoonMaker->InsertAction(ACT_TURNNING))
		AfxMessageBox(L"액션을 추가할 수 없습니다.");
}

void CMainFrame::OnJump()
{
	if(!TheCartoonMaker->InsertAction(ACT_JUMP))
		AfxMessageBox(L"액션을 추가할 수 없습니다.");
}

void CMainFrame::OnDelAction()
{
	if(!TheCartoonMaker->RemoveAction())
		AfxMessageBox(L"더이상 지울 수가 없습니다.");
}

void CMainFrame::OnSaveScreen()
{
	WCHAR szFilter[] = L"Jpeg File (*.jpg)|*.jpg|All Files (*.*)|*.*||";
	CFileDialog FileDlg(FALSE,L".jpg",L"Cartoon",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		szFilter,NULL,0);

	if(FileDlg.DoModal() == IDOK)
	{
		TheCartoonMaker->SetMakeMode(TRUE);
		CString FileName = FileDlg.GetFileName();
		TheCartoonMaker->ScreenToFile(FileName.GetBuffer(0));
		TheCartoonMaker->SetMakeMode(FALSE);
	}
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CFrameWnd::OnActivate(nState, pWndOther, bMinimized);

	//TheCartoonMaker->RenderD3D();
}

void CMainFrame::OnSceneCopy()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	LmUISceneView* pSceneView = (LmUISceneView*) m_wndSplitUpColExpand.GetPane(0,0);
	pSceneView->CopyThumbNail();
}

void CMainFrame::OnScenePaste()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	LmUISceneView* pSceneView = (LmUISceneView*) m_wndSplitUpColExpand.GetPane(0,0);
	pSceneView->PasteThumbNail();
}

void CMainFrame::OnSceneCut()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	LmUISceneView* pSceneView = (LmUISceneView*) m_wndSplitUpColExpand.GetPane(0,0);
	LmUIMainToolFormView* pToolFormView = (LmUIMainToolFormView*) m_wndSplitUpCol.GetPane(0,1) ;
	INT nThumbNailCnt = pSceneView->GetThumNailCount();

	pSceneView->CutThumbNail();

	//씬이 하나도 없으면 Main Tool을 띄우지 않는다. 
	if(!nThumbNailCnt)
	{
		pToolFormView->ChangeCurDlg(L"MainNotice");
		EnableMovieClipMenu(FALSE);
	}
	else
		pToolFormView->ChangeCurDlg(L"MainTool");

}

void CMainFrame::OnLmFileNew()
{
	// [진왕] 여기에 각 저장 데이터를 초기화 시켜야 한다.
	// 그러고 보니.. 저장하시겠습니까? 라고 한번 물어봐 주는 센스도 있어야 겠네..

}

void CMainFrame::OnLmFileOpen()
{
	// [진왕] 우선 파일을 선택한다.
	// 선택한 파일을 열고 그 파일 포인터를 각 매니져 급 클래스에 전달 한다. 
	// 어쩌면 TheCartoonMaker에서 데이터 처리를 모두 한 후
	// UI적인 면을 코딩해야 할 수도 있다.
	LmUISceneView* pSceneView = (LmUISceneView*) m_wndSplitUpColExpand.GetPane(0,0);	
	
	WCHAR szFilter[] = L"3D Cartoon Maker Files(*.3CM)|*.3cm|";
	CFileDialog fileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,szFilter);

	//사용자가 [저장] 버튼을 누르지 않았을 경우 저장하지 않고 루틴을 빠져나간다.
	if(fileDlg.DoModal() != IDOK)
		return;

	//파일을 열어보고 파일에 이상이 있으면, 에러를 출력한다.
	CString strFilePathName = fileDlg.GetPathName();
	CFileException e;
	CFile fileOpen;
	BOOL bResult = fileOpen.Open(strFilePathName.GetBuffer(0), CFile::modeRead);
	if(!bResult)
	{
		e.ReportError();
		return;
	}
	
	//파일 Open에 필요한 초기화 작업
	TheCartoonMaker->InitFileOpen();
	TheCartoonMaker->SetCurrFileName(strFilePathName.GetBuffer(0));

	//먼저 파일 헤더를 읽는다. 
	TheCartoonMaker->OpenFileHeader(fileOpen);

	//씬 뷰 로딩
	pSceneView->LoadSceneView(fileOpen);

	//사운드 매니져에 사운드 정보 로딩
	TheCartoonMaker->LoadSoundLowData(fileOpen);

	//씬 리스트 로딩
	TheCartoonMaker->LoadSceneList(fileOpen);

	//기록이 끝나면 파일을 닫는다.
	fileOpen.Close();

}

void CMainFrame::OnLmFileSave()
{
	// [진왕] 
	// 현재 파일 이름이 지정되었다면, 그냥 저장루틴을 불러 주고
	// 그렇지 않다면 저장할 파일명을 정한다.
	// 저장할 파일을 열고 그 파일 포인터를 각 매니져 급 클래스에 전달 한다.
	
	LmUISceneView* pSceneView = (LmUISceneView*) m_wndSplitUpColExpand.GetPane(0,0);	

	WCHAR szFilter[] = L"3D Cartoon Maker Files(*.3CM)|*.3cm|";
	CFileDialog fileDlg(FALSE,NULL,NULL,OFN_HIDEREADONLY,szFilter);
	
	//사용자가 [저장] 버튼을 누르지 않았을 경우 저장하지 않고 루틴을 빠져나간다.
	if(fileDlg.DoModal() != IDOK)
		return;

	CString strFileName = fileDlg.GetPathName();
	// 확장자 검사. 없으면 붙여줌. 파일명이 ".avi" ".AVI"가 입력 될때도 처리해야함.
	if (strFileName.Mid(strFileName.GetLength() - 4, 4) != _T(".3cm") &&
		strFileName.Mid(strFileName.GetLength() - 4, 4) != _T(".3CM")){
			strFileName += _T(".3cm");
	}

	//파일을 열어보고 파일에 이상이 있으면, 에러를 출력한다.
	CFileException e;
	CFile fileSave;
	BOOL bResult = fileSave.Open(strFileName,  CFile::modeCreate | CFile::modeWrite);
	if(!bResult)
	{
		e.ReportError();
		return;
	}

	//파일 헤더 캐시를 비운다.
	TheCartoonMaker->InitFileHeader();

	//파일 헤더만큼 미리 이동을 해 둔다. 
	fileSave.Seek(FILE_HEADER_SIZE, CFile::begin);

	//각 저장 멤버 함수들을 호출한다. UI를 제외하고는 매니져급 클래스들이겠지.. 
	pSceneView->SaveSceneView(fileSave);
	
	//사운드 LowData를 저장한다. 
	TheCartoonMaker->SaveSoundLowData(fileSave);

	//씬 리스트 저장
	TheCartoonMaker->SaveSceneList(fileSave);

	//가장 마지막에 헤더를 기록한다. 
	TheCartoonMaker->SaveFileHeader(fileSave);
	//TRACE(L"SceneView start : %d \t SceneView end : %d \r\n", TheCartoonMaker->GetSceneViewStart(), TheCartoonMaker->GetSceneViewEnd());
	
	//기록이 끝나면 파일을 닫는다.
	fileSave.Close();
}

void CMainFrame::OnLmFileSaveAs()
{
	// OnFileSave 루틴에서 
	// 파일 이름이 지정되지 않았을 때의 루틴을 이용하면 될 것이다. 
}

void CMainFrame::EnableInsertTalkBoxBtn(BOOL bEnable)
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	LmUIMainToolFormView* pToolFormView = (LmUIMainToolFormView*) m_wndSplitUpCol.GetPane(0,1) ;
	pToolFormView->m_MainToolDlg.m_LmUITalkBox.EnableInsertBtn(bEnable);
	
	//pToolFormView->m_MainToolDlg.m_LmUITalkBox.OnBnClickedTalkInsert();

	//pToolFormView->ChangeCurDlg(L"TalkBoxContent");
}
