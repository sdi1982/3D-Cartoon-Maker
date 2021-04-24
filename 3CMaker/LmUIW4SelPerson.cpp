// LmUIW4SelPerson.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUIW4SelPerson.h"
#include "LmUIMainToolFormView.h"

#include "MainFrm.h"
#include "LmUISceneView.h"

// LmUIW4SelPerson 대화 상자입니다.

IMPLEMENT_DYNAMIC(LmUIW4SelPerson, CDialog)

LmUIW4SelPerson::LmUIW4SelPerson(CWnd* pParent /*=NULL*/)
	: CDialog(LmUIW4SelPerson::IDD, pParent)
{

}

LmUIW4SelPerson::~LmUIW4SelPerson()
{
}

void LmUIW4SelPerson::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(LmUIW4SelPerson, CDialog)
	ON_BN_CLICKED(IDC_WIV_CANCLE, &LmUIW4SelPerson::OnBnClickedWivCancle)
	ON_BN_CLICKED(IDC_WIV_BEFORE, &LmUIW4SelPerson::OnBnClickedWivBefore)
	ON_BN_CLICKED(IDC_WIV_COMPLETE, &LmUIW4SelPerson::OnBnClickedWivComplete)
END_MESSAGE_MAP()


// LmUIW4SelPerson 메시지 처리기입니다.

BOOL LmUIW4SelPerson::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CRect rectSize(10, 35, 410, 550) ;

	m_tbPerson.Create(NULL,NULL, WS_CHILD | WS_VISIBLE | WS_DLGFRAME ,rectSize, this,THUMBNAME_PERSON);

	//m_tbPerson.ShowWindow(SW_SHOW);
	
	wstring wsPersonName = TheCartoonMaker->GetListCharFirst();

	//배경이 없을 경우 그냥 리턴한다.
	if(wsPersonName == L"")
		return TRUE;

	//배경이 있을 경우.. 등록한다.
	CString strResDirectory;
	GetResourceDirectory(strResDirectory);

	CString strFullFileName;
	strFullFileName.Format(L"%s\\%s.jpg", strResDirectory,wsPersonName.c_str());

	m_tbPerson.ThmInsert(strFullFileName, wsPersonName.c_str());

	while(wsPersonName != L"")
	{
		//리스트 끝이면 NULL wstring을 반환한다.
		wsPersonName = TheCartoonMaker->GetListCharNext();
		if(wsPersonName != L"")
		{
			strFullFileName.Format(L"%s\\%s.jpg", strResDirectory,wsPersonName.c_str());
			m_tbPerson.ThmInsert(strFullFileName, wsPersonName.c_str());
		}
	} 

	m_tbPerson.SetItemType(CHARECTERS);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void LmUIW4SelPerson::OnBnClickedWivCancle()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();

	// [취  소]를 클릭 했을때의 처리를 넣을 부분
	
	// 이미 등록되어버린 ThumbNail을 삭제!

	//메인 프레임에서 LmUISceneView Class의 주소를 얻어온다. 
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
	LmUISceneView* pSceneView = GetSceneView();

	// 메뉴를 원래대로 돌린다
	pMainFrame->EnableSceneMenu(TRUE);

	//이미 등록되어 버린 ThumbNail을 삭제 한다.
	pSceneView->RemoveThumbNail(pParentWnd->m_DlgW1SceneName.m_nInsertedIndex);
	pSceneView->Invalidate();

	// [동인] 등록한 씬을 삭제한다.
	TheCartoonMaker->ShowPickBoard(FALSE);
	TheCartoonMaker->DeleteScene(pParentWnd->m_DlgW1SceneName.m_strSceneName);
	
	//메뉴 세팅
	pMainFrame->EnableAllMenu(TRUE);
	pMainFrame->EnableEditMenu(FALSE);
	pMainFrame->EnableCharacterMenu(FALSE);

	//씬의 변경이 가능하도록 한다.
	pSceneView->EnableChangeScene(TRUE);

	//씬이 없는경우 씬을 등록하라는 메시지를 오른쪽 Tool 영역에 띄운다.
	INT nThumbNailCnt = pSceneView->GetThumNailCount();
	if(!nThumbNailCnt)
		pParentWnd->ChangeCurDlg(L"MainNotice");
	else
		pParentWnd->ChangeCurDlg(L"MainTool");

}

void LmUIW4SelPerson::OnBnClickedWivBefore()
{
	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();

	ThePickBoard->SetDragChar(NULL);
	// [이전으로]를 클릭 했을때의 처리를 넣을 부분
	pParentWnd->ChangeCurDlg(L"Wizard3");
	TheCartoonMaker->SetStateOfUIBefore();
}

void LmUIW4SelPerson::OnBnClickedWivComplete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();
	LmUISceneView *pSceneView = GetSceneView();
	// 메뉴를 원래대로 돌린다
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();


	// [동인] 완료시에 피킹보드를 안보이게 처리한다
	TheCartoonMaker->ShowPickBoard(FALSE);
	
	// 현재화면을 첫번째 프레임으로 돌린다.
	TheCartoonMaker->SetCurFrame(1);
	// [동인] 화면을 캡쳐한다.
	// [진왕] 디스크 매시로 인해 하드에 스샷한 파일이 저장되지 않아서 멤버 함수를 직접 만들었음.
	//TheCartoonMaker->ScreenShot();   //이 함수에서는 필요 없음.
	
	// [취  소]를 클릭 했을때의 처리를 넣을 부분
	pParentWnd->ChangeCurDlg(L"MainTool");

	pMainFrame->EnableAllMenu( TRUE );
	
	//SceneView에서 화면을 갱신해준다.
	
	//화면을 캡쳐 한다.
	IPicture *pPic = TheCartoonMaker->ScreenToMemory();

	INT nThumbCaret = pSceneView->GetThumbNailCaret();
	pSceneView->ReplaceThumbNail(nThumbCaret, pPic);

	//메뉴 세팅
	pMainFrame->EnableAllMenu(TRUE);

	pMainFrame->EnableEditMenu(FALSE);
	pMainFrame->EnableCharacterMenu(FALSE);
	//씬의 변경이 가능하도록 한다.
	pSceneView->EnableChangeScene(TRUE);
	TheCartoonMaker->SetStateOfUI(SCENE_CREATED);
}

LmUISceneView * LmUIW4SelPerson::GetSceneView()
{
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
	LmUISceneView* pSceneView = (LmUISceneView*) ( pMainFrame->m_wndSplitUpColExpand.GetPane(0,0) );
	return pSceneView;
}

BOOL LmUIW4SelPerson::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_RETURN)
		return TRUE;

	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}
