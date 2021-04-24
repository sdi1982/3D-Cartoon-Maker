// LmUIW2SelBakGnd.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUIW2SelBakGnd.h"
#include "LmUIMainToolFormView.h"
#include "MainFrm.h"
#include "LmUISceneView.h"

// LmUIW2SelBakGnd 대화 상자입니다.

IMPLEMENT_DYNAMIC(LmUIW2SelBakGnd, CDialog)

LmUIW2SelBakGnd::LmUIW2SelBakGnd(CWnd* pParent /*=NULL*/)
	: CDialog(LmUIW2SelBakGnd::IDD, pParent)
{

}

LmUIW2SelBakGnd::~LmUIW2SelBakGnd()
{
}

void LmUIW2SelBakGnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(LmUIW2SelBakGnd, CDialog)
	ON_BN_CLICKED(IDC_WII_BEFORE, &LmUIW2SelBakGnd::OnBnClickedWiiBefore)
	ON_BN_CLICKED(IDC_WII_NEXT, &LmUIW2SelBakGnd::OnBnClickedWiiNext)
//	ON_BN_CLICKED(IDC_WII_CANCLE, &LmUIW2SelBakGnd::OnBnClickedWiiCancle)
//	ON_BN_CLICKED(IDC_WII_COMPLETE, &LmUIW2SelBakGnd::OnBnClickedWiiComplete)
END_MESSAGE_MAP()


// LmUIW2SelBakGnd 메시지 처리기입니다.




BOOL LmUIW2SelBakGnd::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CRect rectSize(10, 35, 410, 550) ;

	m_tbBackGround.Create(NULL,NULL, WS_CHILD | WS_VISIBLE | WS_DLGFRAME ,rectSize, this,THUMBNAME_BACKGROUND);
	
	//초기 프로그램이 로딩될 때 리소스들을 등록한다.
	//만일 시간이 오래 걸린다면 Thread로 구현할 예정임.
	
	wstring wsBgName = TheCartoonMaker->GetListBGFirst();

	//배경이 없을 경우 그냥 리턴한다.
	if(wsBgName == L"")
		return TRUE;

	//배경이 1개라도 있을 경우.. 무조건 바로~ 등록한다.
	CString strResDirectory;
	GetResourceDirectory(strResDirectory);

	CString strFullFileName;
	strFullFileName.Format(L"%s\\%s.jpg", strResDirectory,wsBgName.c_str());

	m_tbBackGround.ThmInsert(strFullFileName, wsBgName.c_str());

	while(wsBgName != L"")
	{
		//리스트 끝이면 NULL wstring을 반환한다.
		wsBgName = TheCartoonMaker->GetListBGNext();
		TRACE("wsBgName - 2SelBakGnd.cpp : %s\r\n",wsBgName.c_str() );
		if(wsBgName != L"")
		{
			strFullFileName.Format(L"%s\\%s.jpg", strResDirectory,wsBgName.c_str());
			m_tbBackGround.ThmInsert(strFullFileName, wsBgName.c_str());
		}
	} 

	//배경을 다루는 ThumbNail을 정한다. 
	m_tbBackGround.SetItemType(BACKGROUNDS);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void LmUIW2SelBakGnd::OnBnClickedWiiBefore()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();

	// 이전을 클릭 했을때 처리 해야 할 부분
	//메인 프레임에서 LmUISceneView Class의 주소를 얻어온다. 
	LmUISceneView* pSceneView = GetSceneView();

	// [동인] 등록한 씬을 삭제한다.
	TheCartoonMaker->DeleteScene(pParentWnd->m_DlgW1SceneName.m_strSceneName);

	//이미 등록되어 버린 ThumbNail을 삭제 한다.
	pSceneView->RemoveThumbNail(pParentWnd->m_DlgW1SceneName.m_nInsertedIndex);
	pSceneView->Invalidate();

	pParentWnd->ChangeCurDlg(L"Wizard1");
	
	//이전의 상태로 되돌린다. 
	TheCartoonMaker->SetStateOfUIBefore();
}

void LmUIW2SelBakGnd::OnBnClickedWiiNext()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();

	// [동인] 다음 위자드에서 오브젝트의 배치를 위해서 피킹보드를 보여준다.
	TheCartoonMaker->ShowPickBoard(TRUE);

	//다음을 클릭 했을때 처리를 해야 할 부분
	CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->EnableEditBackgroundMenu(FALSE);

	pParentWnd->ChangeCurDlg(L"Wizard3");
	//현재 상태를 Wizard2로 지정한다. 
	TheCartoonMaker->SetStateOfUI(WIZARD3);
}

//void LmUIW2SelBakGnd::OnBnClickedWiiCancle()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();
//
//	// 취소를 클릭 했을때 처리를 해야 할 부분
//	// 이미 등록되어버린 ThumbNail을 삭제!
//
//	//메인 프레임에서 LmUISceneView Class의 주소를 얻어온다. 
//	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
//	LmUISceneView* pSceneView = GetSceneView();	
//
//	// 메뉴를 원래대로 돌린다
//	pMainFrame->EnableSceneMenu(TRUE);
//
//	//이미 등록되어 버린 ThumbNail을 삭제 한다.
//	pSceneView->RemoveThumbNail(pParentWnd->m_DlgW1SceneName.m_nInsertedIndex);
//	pSceneView->Invalidate();
//
//	// [동인] 등록한 씬을 삭제한다.
//	TheCartoonMaker->DeleteScene(pParentWnd->m_DlgW1SceneName.m_strSceneName);
//	TheCartoonMaker->ShowPickBoard(FALSE);
//	//씬의 변경이 가능하도록 한다.	
//	pSceneView->EnableChangeScene(TRUE);
//	pParentWnd->ChangeCurDlg(L"MainTool");
//
//}
//
//void LmUIW2SelBakGnd::OnBnClickedWiiComplete()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();
//
//	// 메뉴를 원래대로 돌린다
//	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
//	pMainFrame->EnableSceneMenu(TRUE);
//
//	// 완료를 클릭 했을때의 처리를 넣을 부분
//	pParentWnd->ChangeCurDlg(L"MainTool");
//}

//LmUISceneView의 주소를 받아온다.
LmUISceneView * LmUIW2SelBakGnd::GetSceneView()
{
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
	LmUISceneView* pSceneView = (LmUISceneView*) ( pMainFrame->m_wndSplitUpColExpand.GetPane(0,0) );
	return pSceneView;
}

BOOL LmUIW2SelBakGnd::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_RETURN)
		return TRUE;

	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}
