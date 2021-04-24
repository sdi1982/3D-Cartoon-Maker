// MainFrm.cpp : CMainFrame Ŭ������ ����
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
	ID_SEPARATOR,           // ���� �� ǥ�ñ�
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame ����/�Ҹ�

CMainFrame::CMainFrame()
{
	// TODO: ���⿡ ��� �ʱ�ȭ �ڵ带 �߰��մϴ�.
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//�켱�� ���ٴ� ��� ���Ѵ�.
	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
	//	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	//{
	//	TRACE0("���� ������ ������ ���߽��ϴ�.\n");
	//	return -1;      // ������ ���߽��ϴ�.
	//}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("���� ǥ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}

	// TODO: ���� ������ ��ŷ�� �� ���� �Ϸ��� �� �� ���� �����Ͻʽÿ�.
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndToolBar);

	//int display_height = GetSystemMetrics(SM_CYSCREEN);
	//int display_width = GetSystemMetrics(SM_CXSCREEN);

	//::SetWindowPos(this->m_hWnd, HWND_NOTOPMOST,0, 0, display_width,display_height ,SWP_DRAWFRAME);

//	SendMessage(WM_SYSCOMMAND,SC_MAXIMIZE); /* �ִ�ȭ*/

	// ī������Ŀ ���ҽ� ����Ʈ ���
	TheCartoonMaker->Read3cmFile();

	// Sound Handle ���
	TheCartoonMaker->SetHMainWnd(m_hWnd);

	CFileFind finder;
	BOOL bWorking = finder.FindFile(L".");
	bWorking = finder.FindNextFile();
	g_strFilePath = finder.GetFilePath();

	//�� ������ FALSE���� �ƴϸ� �޴��� ������� ������ �� ����.
	m_bAutoMenuEnable = FALSE;
	
	//���� �޴��� Disable�� �Ѵ�.
	EnableEditMenu(FALSE);
	EnableCharacterMenu(FALSE);

	//������ �޴��� Disable�� �Ѵ�. 
	EnableMovieClipMenu(FALSE);
	
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.
	//cs.style &= WS_MAXIMIZEBOX;
	//cs.style = WS_OVERLAPPED | WS_SYSMENU | WS_BORDER;
	//cs.style |= WS_MAXIMIZE;

	return TRUE;
}


// CMainFrame ����

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


// CMainFrame �޽��� ó����



//���� �並 �����Ѵ�.
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	int display_height = GetSystemMetrics(SM_CYSCREEN);
	int display_width = GetSystemMetrics(SM_CXSCREEN);

	//�ʱ� 1�� 1���� �����Ǵµ� ���η� ���� 2���� ����
	if(!m_wndSplitRow.CreateStatic(this, 2 , 1))
	{
		TRACE0("Fail to create static m_wmdSplitter object");
		return FALSE;
	}

 	//1�� 1���� �ٽ� ���η� ������ 2���� ���� ����.
	if(!m_wndSplitUpCol.CreateStatic(&m_wndSplitRow, 1,2, WS_CHILD | WS_VISIBLE ,
		m_wndSplitRow.IdFromRowCol(0,0))  )
	{
		TRACE0("Fail to create static m_wndSplitRow_expo object");
		return FALSE;
	}

	//�ʱ� 2��2���� �����Ǵµ�, �ű⿡�� (0,0)�� ���η� ������ 3������,
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
	//���� ũ�⸦ �����Ѵ�.
	m_wndSplitRow.SetRowInfo(0,display_height-400,10);

	m_wndSplitRow.RecalcLayout();
	m_wndSplitUpCol.RecalcLayout();

	//�ý��� �޴����� �̵� ����
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	
/*	//â�� �̵��� ���� ���� -  ������ ���ᵵ ���� �ʴ´ٴ°� !!
	int display_height = GetSystemMetrics(SM_CYSCREEN);
	int display_width = GetSystemMetrics(SM_CXSCREEN);
	CRect rectTest;
	rectTest.SetRect(0, 0, display_width-30,30);
	
	//���� ����(�����ư�� �ִ� ��) �ۿ� �ִ°�?
	if(rectTest.PtInRect(point))
	   return FALSE;
*/
	return CFrameWnd::OnNcHitTest(point);
}

void CMainFrame::OnMakeScene()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	LmUIMainToolFormView* pToolFormView = (LmUIMainToolFormView*) m_wndSplitUpCol.GetPane(0,1) ;

	// Wizard���� �Ϸ� ��ư�� ������ �׶� Scene�� �߰� �Ǵ� ������� �ڵ��Ѵ�.!
	pToolFormView->InitWizard();

	//////////////////////////////////////////////////////////////////////////
	// �׽�Ʈ
	//OnPickCharacterMenu();
	//////////////////////////////////////////////////////////////////////////
	//�����. �ʿ����� �����ּ���~~
	//TheCartoonMaker->
	//���� ���õ� �޴��� Disable�Ѵ�.

	//��� �޴��� Disable�Ѵ�.
	EnableAllMenu(FALSE);

	//[����] �޴� Ȱ��ȭ
	EnableEditMenu(TRUE);

	EnableEditCharacterMenu(FALSE);
	EnableEditThingsMenu(FALSE);
	EnableEditBackgroundMenu(FALSE);

//	EnableCharacterMenu(TRUE);
	TheCartoonMaker->SetStateOfUI(WIZARD1);
}

void CMainFrame::OnSceneDelete()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	LmUISceneView* pSceneView = (LmUISceneView*) m_wndSplitUpColExpand.GetPane(0,0);

	// [����] �� ����
	CRectPicture data;
	pSceneView->GetThumbNailCaret(data);
	
	TheCartoonMaker->DeleteScene(data.strSceneName);

	// [����] UI���� ����
	pSceneView->RemoveThumbNail( pSceneView->m_nCaret );
	pSceneView->Invalidate();

	LmUIMainToolFormView* pToolFormView = (LmUIMainToolFormView*) m_wndSplitUpCol.GetPane(0,1) ;
	INT nThumbNailCnt = pSceneView->GetThumNailCount();

	//���� �ϳ��� ������ Main Tool�� ����� �ʴ´�. 
	if(!nThumbNailCnt)
		pToolFormView->ChangeCurDlg(L"MainNotice");
	else
		pToolFormView->ChangeCurDlg(L"MainTool");
}


void CMainFrame::OnThingDelete()
{
	//[��ü] -> [�����ϱ�] �޴��� ���õǾ��� �� ����Ǵ� ��� �Լ���.
	
	//[����] Things�� �������� �� �� �ʿ��� �Լ��� ȣ���ϸ� ��

	TheCartoonMaker->DeleteGotThing();
	return;
}

void CMainFrame::OnThingsTurn()
{
	//[ȸ���ϱ�]�� ���õǾ����� ȣ��Ǵ� ��� �Լ���.
	//[����] ���õ� ��ü�� ȸ����Ų��.
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
	//���õ� ĳ���� ���� �ٸ� �޴��� �ߵ��� �ϴ� �Լ���.
	// � ĳ���Ͱ� ���õǾ�����,
	// �� ĳ���Ͱ� ���� ������ ������ �ִ����� ���� 
	// ����� �ʿ���.
	// ����� ��Ÿ���� 
	// [Action Name] , [Action Name�� ID��] 2���� ���� �ʼ���.

	CMenu *pMenu = GetMenu();
	CMenu newCharacterMenu;

	newCharacterMenu.CreatePopupMenu();
	for(int nCnt = 0; nCnt < 2 ; nCnt++)
		newCharacterMenu.AppendMenu(MF_STRING & (~MF_GRAYED), menuData[nCnt].nMenuID, menuData[nCnt].strMenuName);

	//pMenu->RemoveMenu(2);
	pMenu->ModifyMenu(2, MF_POPUP | MF_BYPOSITION | MF_ENABLED, (UINT_PTR)newCharacterMenu.Detach(), L"ĳ����");
	//pMenu->AppendMenu(MF_POPUP, (UINT_PTR)newCharacterMenu.Detach(), L"ĳ����");

	SetMenu(pMenu);

}

void CMainFrame::OnCharacterAction()
{
	//ĳ���� �׼�	
	return;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if(pMsg->message == WM_COMMAND)
	{
		//����ٰ� �� �׼��� ���� �����ϰ� OnCharacterAction() �Լ� ȣ��
		if(LOWORD(pMsg->wParam) >= 3000 && LOWORD(pMsg->wParam) <= 3001)
			OnCharacterAction();
	}
	return CFrameWnd::PreTranslateMessage(pMsg);
}

//���� �޴��� Ȱ��ȭ ��Ű�ų� ��Ȱ��ȭ ��Ų��.
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
	// ������ ����� �޴� Ŭ�� ������.
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
		AfxMessageBox(L"�׼��� �߰��� �� �����ϴ�.");
}

void CMainFrame::OnTurnRight()
{
	if(!TheCartoonMaker->InsertAction(ACT_TURN_RIGHT))
		AfxMessageBox(L"�׼��� �߰��� �� �����ϴ�.");
}

void CMainFrame::OnWalk()
{
	if(!TheCartoonMaker->InsertAction(ACT_WALK))
		AfxMessageBox(L"�׼��� �߰��� �� �����ϴ�.");
}

void CMainFrame::OnShakeHand()
{
	if(!TheCartoonMaker->InsertAction(ACT_SHAKE_HAND))
		AfxMessageBox(L"�׼��� �߰��� �� �����ϴ�.");
}

void CMainFrame::OnBow()
{
	if(!TheCartoonMaker->InsertAction(ACT_BOW))
		AfxMessageBox(L"�׼��� �߰��� �� �����ϴ�.");
}

void CMainFrame::OnTurnning()
{
	if(!TheCartoonMaker->InsertAction(ACT_TURNNING))
		AfxMessageBox(L"�׼��� �߰��� �� �����ϴ�.");
}

void CMainFrame::OnJump()
{
	if(!TheCartoonMaker->InsertAction(ACT_JUMP))
		AfxMessageBox(L"�׼��� �߰��� �� �����ϴ�.");
}

void CMainFrame::OnDelAction()
{
	if(!TheCartoonMaker->RemoveAction())
		AfxMessageBox(L"���̻� ���� ���� �����ϴ�.");
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
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	LmUISceneView* pSceneView = (LmUISceneView*) m_wndSplitUpColExpand.GetPane(0,0);
	pSceneView->CopyThumbNail();
}

void CMainFrame::OnScenePaste()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	LmUISceneView* pSceneView = (LmUISceneView*) m_wndSplitUpColExpand.GetPane(0,0);
	pSceneView->PasteThumbNail();
}

void CMainFrame::OnSceneCut()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.

	LmUISceneView* pSceneView = (LmUISceneView*) m_wndSplitUpColExpand.GetPane(0,0);
	LmUIMainToolFormView* pToolFormView = (LmUIMainToolFormView*) m_wndSplitUpCol.GetPane(0,1) ;
	INT nThumbNailCnt = pSceneView->GetThumNailCount();

	pSceneView->CutThumbNail();

	//���� �ϳ��� ������ Main Tool�� ����� �ʴ´�. 
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
	// [����] ���⿡ �� ���� �����͸� �ʱ�ȭ ���Ѿ� �Ѵ�.
	// �׷��� ����.. �����Ͻðڽ��ϱ�? ��� �ѹ� ����� �ִ� ������ �־�� �ڳ�..

}

void CMainFrame::OnLmFileOpen()
{
	// [����] �켱 ������ �����Ѵ�.
	// ������ ������ ���� �� ���� �����͸� �� �Ŵ��� �� Ŭ������ ���� �Ѵ�. 
	// ��¼�� TheCartoonMaker���� ������ ó���� ��� �� ��
	// UI���� ���� �ڵ��ؾ� �� ���� �ִ�.
	LmUISceneView* pSceneView = (LmUISceneView*) m_wndSplitUpColExpand.GetPane(0,0);	
	
	WCHAR szFilter[] = L"3D Cartoon Maker Files(*.3CM)|*.3cm|";
	CFileDialog fileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,szFilter);

	//����ڰ� [����] ��ư�� ������ �ʾ��� ��� �������� �ʰ� ��ƾ�� ����������.
	if(fileDlg.DoModal() != IDOK)
		return;

	//������ ����� ���Ͽ� �̻��� ������, ������ ����Ѵ�.
	CString strFilePathName = fileDlg.GetPathName();
	CFileException e;
	CFile fileOpen;
	BOOL bResult = fileOpen.Open(strFilePathName.GetBuffer(0), CFile::modeRead);
	if(!bResult)
	{
		e.ReportError();
		return;
	}
	
	//���� Open�� �ʿ��� �ʱ�ȭ �۾�
	TheCartoonMaker->InitFileOpen();
	TheCartoonMaker->SetCurrFileName(strFilePathName.GetBuffer(0));

	//���� ���� ����� �д´�. 
	TheCartoonMaker->OpenFileHeader(fileOpen);

	//�� �� �ε�
	pSceneView->LoadSceneView(fileOpen);

	//���� �Ŵ����� ���� ���� �ε�
	TheCartoonMaker->LoadSoundLowData(fileOpen);

	//�� ����Ʈ �ε�
	TheCartoonMaker->LoadSceneList(fileOpen);

	//����� ������ ������ �ݴ´�.
	fileOpen.Close();

}

void CMainFrame::OnLmFileSave()
{
	// [����] 
	// ���� ���� �̸��� �����Ǿ��ٸ�, �׳� �����ƾ�� �ҷ� �ְ�
	// �׷��� �ʴٸ� ������ ���ϸ��� ���Ѵ�.
	// ������ ������ ���� �� ���� �����͸� �� �Ŵ��� �� Ŭ������ ���� �Ѵ�.
	
	LmUISceneView* pSceneView = (LmUISceneView*) m_wndSplitUpColExpand.GetPane(0,0);	

	WCHAR szFilter[] = L"3D Cartoon Maker Files(*.3CM)|*.3cm|";
	CFileDialog fileDlg(FALSE,NULL,NULL,OFN_HIDEREADONLY,szFilter);
	
	//����ڰ� [����] ��ư�� ������ �ʾ��� ��� �������� �ʰ� ��ƾ�� ����������.
	if(fileDlg.DoModal() != IDOK)
		return;

	CString strFileName = fileDlg.GetPathName();
	// Ȯ���� �˻�. ������ �ٿ���. ���ϸ��� ".avi" ".AVI"�� �Է� �ɶ��� ó���ؾ���.
	if (strFileName.Mid(strFileName.GetLength() - 4, 4) != _T(".3cm") &&
		strFileName.Mid(strFileName.GetLength() - 4, 4) != _T(".3CM")){
			strFileName += _T(".3cm");
	}

	//������ ����� ���Ͽ� �̻��� ������, ������ ����Ѵ�.
	CFileException e;
	CFile fileSave;
	BOOL bResult = fileSave.Open(strFileName,  CFile::modeCreate | CFile::modeWrite);
	if(!bResult)
	{
		e.ReportError();
		return;
	}

	//���� ��� ĳ�ø� ����.
	TheCartoonMaker->InitFileHeader();

	//���� �����ŭ �̸� �̵��� �� �д�. 
	fileSave.Seek(FILE_HEADER_SIZE, CFile::begin);

	//�� ���� ��� �Լ����� ȣ���Ѵ�. UI�� �����ϰ�� �Ŵ����� Ŭ�������̰���.. 
	pSceneView->SaveSceneView(fileSave);
	
	//���� LowData�� �����Ѵ�. 
	TheCartoonMaker->SaveSoundLowData(fileSave);

	//�� ����Ʈ ����
	TheCartoonMaker->SaveSceneList(fileSave);

	//���� �������� ����� ����Ѵ�. 
	TheCartoonMaker->SaveFileHeader(fileSave);
	//TRACE(L"SceneView start : %d \t SceneView end : %d \r\n", TheCartoonMaker->GetSceneViewStart(), TheCartoonMaker->GetSceneViewEnd());
	
	//����� ������ ������ �ݴ´�.
	fileSave.Close();
}

void CMainFrame::OnLmFileSaveAs()
{
	// OnFileSave ��ƾ���� 
	// ���� �̸��� �������� �ʾ��� ���� ��ƾ�� �̿��ϸ� �� ���̴�. 
}

void CMainFrame::EnableInsertTalkBoxBtn(BOOL bEnable)
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	LmUIMainToolFormView* pToolFormView = (LmUIMainToolFormView*) m_wndSplitUpCol.GetPane(0,1) ;
	pToolFormView->m_MainToolDlg.m_LmUITalkBox.EnableInsertBtn(bEnable);
	
	//pToolFormView->m_MainToolDlg.m_LmUITalkBox.OnBnClickedTalkInsert();

	//pToolFormView->ChangeCurDlg(L"TalkBoxContent");
}
