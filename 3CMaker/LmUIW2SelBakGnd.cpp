// LmUIW2SelBakGnd.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUIW2SelBakGnd.h"
#include "LmUIMainToolFormView.h"
#include "MainFrm.h"
#include "LmUISceneView.h"

// LmUIW2SelBakGnd ��ȭ �����Դϴ�.

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


// LmUIW2SelBakGnd �޽��� ó�����Դϴ�.




BOOL LmUIW2SelBakGnd::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CRect rectSize(10, 35, 410, 550) ;

	m_tbBackGround.Create(NULL,NULL, WS_CHILD | WS_VISIBLE | WS_DLGFRAME ,rectSize, this,THUMBNAME_BACKGROUND);
	
	//�ʱ� ���α׷��� �ε��� �� ���ҽ����� ����Ѵ�.
	//���� �ð��� ���� �ɸ��ٸ� Thread�� ������ ������.
	
	wstring wsBgName = TheCartoonMaker->GetListBGFirst();

	//����� ���� ��� �׳� �����Ѵ�.
	if(wsBgName == L"")
		return TRUE;

	//����� 1���� ���� ���.. ������ �ٷ�~ ����Ѵ�.
	CString strResDirectory;
	GetResourceDirectory(strResDirectory);

	CString strFullFileName;
	strFullFileName.Format(L"%s\\%s.jpg", strResDirectory,wsBgName.c_str());

	m_tbBackGround.ThmInsert(strFullFileName, wsBgName.c_str());

	while(wsBgName != L"")
	{
		//����Ʈ ���̸� NULL wstring�� ��ȯ�Ѵ�.
		wsBgName = TheCartoonMaker->GetListBGNext();
		TRACE("wsBgName - 2SelBakGnd.cpp : %s\r\n",wsBgName.c_str() );
		if(wsBgName != L"")
		{
			strFullFileName.Format(L"%s\\%s.jpg", strResDirectory,wsBgName.c_str());
			m_tbBackGround.ThmInsert(strFullFileName, wsBgName.c_str());
		}
	} 

	//����� �ٷ�� ThumbNail�� ���Ѵ�. 
	m_tbBackGround.SetItemType(BACKGROUNDS);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void LmUIW2SelBakGnd::OnBnClickedWiiBefore()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();

	// ������ Ŭ�� ������ ó�� �ؾ� �� �κ�
	//���� �����ӿ��� LmUISceneView Class�� �ּҸ� ���´�. 
	LmUISceneView* pSceneView = GetSceneView();

	// [����] ����� ���� �����Ѵ�.
	TheCartoonMaker->DeleteScene(pParentWnd->m_DlgW1SceneName.m_strSceneName);

	//�̹� ��ϵǾ� ���� ThumbNail�� ���� �Ѵ�.
	pSceneView->RemoveThumbNail(pParentWnd->m_DlgW1SceneName.m_nInsertedIndex);
	pSceneView->Invalidate();

	pParentWnd->ChangeCurDlg(L"Wizard1");
	
	//������ ���·� �ǵ�����. 
	TheCartoonMaker->SetStateOfUIBefore();
}

void LmUIW2SelBakGnd::OnBnClickedWiiNext()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();

	// [����] ���� ���ڵ忡�� ������Ʈ�� ��ġ�� ���ؼ� ��ŷ���带 �����ش�.
	TheCartoonMaker->ShowPickBoard(TRUE);

	//������ Ŭ�� ������ ó���� �ؾ� �� �κ�
	CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->EnableEditBackgroundMenu(FALSE);

	pParentWnd->ChangeCurDlg(L"Wizard3");
	//���� ���¸� Wizard2�� �����Ѵ�. 
	TheCartoonMaker->SetStateOfUI(WIZARD3);
}

//void LmUIW2SelBakGnd::OnBnClickedWiiCancle()
//{
//	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
//	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();
//
//	// ��Ҹ� Ŭ�� ������ ó���� �ؾ� �� �κ�
//	// �̹� ��ϵǾ���� ThumbNail�� ����!
//
//	//���� �����ӿ��� LmUISceneView Class�� �ּҸ� ���´�. 
//	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
//	LmUISceneView* pSceneView = GetSceneView();	
//
//	// �޴��� ������� ������
//	pMainFrame->EnableSceneMenu(TRUE);
//
//	//�̹� ��ϵǾ� ���� ThumbNail�� ���� �Ѵ�.
//	pSceneView->RemoveThumbNail(pParentWnd->m_DlgW1SceneName.m_nInsertedIndex);
//	pSceneView->Invalidate();
//
//	// [����] ����� ���� �����Ѵ�.
//	TheCartoonMaker->DeleteScene(pParentWnd->m_DlgW1SceneName.m_strSceneName);
//	TheCartoonMaker->ShowPickBoard(FALSE);
//	//���� ������ �����ϵ��� �Ѵ�.	
//	pSceneView->EnableChangeScene(TRUE);
//	pParentWnd->ChangeCurDlg(L"MainTool");
//
//}
//
//void LmUIW2SelBakGnd::OnBnClickedWiiComplete()
//{
//	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
//	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();
//
//	// �޴��� ������� ������
//	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
//	pMainFrame->EnableSceneMenu(TRUE);
//
//	// �ϷḦ Ŭ�� �������� ó���� ���� �κ�
//	pParentWnd->ChangeCurDlg(L"MainTool");
//}

//LmUISceneView�� �ּҸ� �޾ƿ´�.
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
