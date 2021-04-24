// LmUIW3SelObj.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUIW3SelObj.h"
#include "LmUIMainToolFormView.h"

#include "MainFrm.h"
#include "LmUISceneView.h"
// LmUIW3SelObj 대화 상자입니다.

IMPLEMENT_DYNAMIC(LmUIW3SelObj, CDialog)

LmUIW3SelObj::LmUIW3SelObj(CWnd* pParent /*=NULL*/)
	: CDialog(LmUIW3SelObj::IDD, pParent)
{

}

LmUIW3SelObj::~LmUIW3SelObj()
{
}

void LmUIW3SelObj::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(LmUIW3SelObj, CDialog)
	ON_BN_CLICKED(IDC_WIII_BEFORE, &LmUIW3SelObj::OnBnClickedWiiiBefore)
	ON_BN_CLICKED(IDC_WIII_NEXT, &LmUIW3SelObj::OnBnClickedWiiiNext)
//	ON_BN_CLICKED(IDC_WIII_CANCLE, &LmUIW3SelObj::OnBnClickedWiiiCancle)
//	ON_BN_CLICKED(IDC_WIII_COMPLETE, &LmUIW3SelObj::OnBnClickedWiiiComplete)
END_MESSAGE_MAP()


// LmUIW3SelObj 메시지 처리기입니다.

BOOL LmUIW3SelObj::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CRect rectSize(10, 35, 410, 550) ;

	m_tbObject.Create(NULL,NULL, WS_CHILD | WS_VISIBLE | WS_DLGFRAME ,rectSize, this,THUMBNAME_OBJECT);

	wstring wsObjName = TheCartoonMaker->GetListThingFirst();

	//배경이 없을 경우 그냥 리턴한다.
	if(wsObjName == L"")
		return TRUE;

	//배경이 있을 경우.. 등록한다.
	CString strResDirectory;
	GetResourceDirectory(strResDirectory);

	CString strFullFileName;
	strFullFileName.Format(L"%s\\%s.jpg", strResDirectory,wsObjName.c_str());

	m_tbObject.ThmInsert(strFullFileName, wsObjName.c_str());

	while(wsObjName != L"")
	{
		//리스트 끝이면 NULL wstring을 반환한다.
		wsObjName = TheCartoonMaker->GetListThingNext();
		if(wsObjName != L"")
		{
			strFullFileName.Format(L"%s\\%s.jpg", strResDirectory,wsObjName.c_str());
			m_tbObject.ThmInsert(strFullFileName, wsObjName.c_str());
		}
	} 

	//물체를 다루는 ThumbNail을 정한다. 
	m_tbObject.SetItemType(THINGS);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void LmUIW3SelObj::OnBnClickedWiiiBefore()
{
	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();

	// [동인] 위자드 2로 돌아가므로 피킹보드를 안보이게 한다.
	TheCartoonMaker->ShowPickBoard(FALSE);

	// [이전으로]을 클릭 했을때의 처리를 넣을 부분
	pParentWnd->ChangeCurDlg(L"Wizard2");

	TheCartoonMaker->SetStateOfUIBefore();
}

void LmUIW3SelObj::OnBnClickedWiiiNext()
{
	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();
	
	ThePickBoard->SetDragThing(NULL);

	// [다음으로]을 클릭 했을때의 처리를 넣을 부분
	pParentWnd->ChangeCurDlg(L"Wizard4");
	TheCartoonMaker->SetStateOfUI(WIZARD4);
}

//
//void LmUIW3SelObj::OnBnClickedWiiiCancle()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();
//
//	// [취  소]를 클릭 했을때의 처리를 넣을 부분
//	
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
//
//	pParentWnd->ChangeCurDlg(L"MainTool");
//  //씬의 변경이 가능하도록
//  pSceneView->EnableChangeScene(TRUE);
//}

//void LmUIW3SelObj::OnBnClickedWiiiComplete()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();
//	
//	// 메뉴를 원래대로 돌린다
//	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
//	pMainFrame->EnableSceneMenu(TRUE);
//
//	// [완  료]를 클릭 했을때의 처리를 넣을 부분
//
//	// [동인]
//
//	pParentWnd->ChangeCurDlg(L"MainTool");
//}

LmUISceneView * LmUIW3SelObj::GetSceneView()
{
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
	LmUISceneView* pSceneView = (LmUISceneView*) ( pMainFrame->m_wndSplitUpColExpand.GetPane(0,0) );
	return pSceneView;
}

BOOL LmUIW3SelObj::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_RETURN)
		return TRUE;

	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}
