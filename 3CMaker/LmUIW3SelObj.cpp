// LmUIW3SelObj.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUIW3SelObj.h"
#include "LmUIMainToolFormView.h"

#include "MainFrm.h"
#include "LmUISceneView.h"
// LmUIW3SelObj ��ȭ �����Դϴ�.

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


// LmUIW3SelObj �޽��� ó�����Դϴ�.

BOOL LmUIW3SelObj::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CRect rectSize(10, 35, 410, 550) ;

	m_tbObject.Create(NULL,NULL, WS_CHILD | WS_VISIBLE | WS_DLGFRAME ,rectSize, this,THUMBNAME_OBJECT);

	wstring wsObjName = TheCartoonMaker->GetListThingFirst();

	//����� ���� ��� �׳� �����Ѵ�.
	if(wsObjName == L"")
		return TRUE;

	//����� ���� ���.. ����Ѵ�.
	CString strResDirectory;
	GetResourceDirectory(strResDirectory);

	CString strFullFileName;
	strFullFileName.Format(L"%s\\%s.jpg", strResDirectory,wsObjName.c_str());

	m_tbObject.ThmInsert(strFullFileName, wsObjName.c_str());

	while(wsObjName != L"")
	{
		//����Ʈ ���̸� NULL wstring�� ��ȯ�Ѵ�.
		wsObjName = TheCartoonMaker->GetListThingNext();
		if(wsObjName != L"")
		{
			strFullFileName.Format(L"%s\\%s.jpg", strResDirectory,wsObjName.c_str());
			m_tbObject.ThmInsert(strFullFileName, wsObjName.c_str());
		}
	} 

	//��ü�� �ٷ�� ThumbNail�� ���Ѵ�. 
	m_tbObject.SetItemType(THINGS);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void LmUIW3SelObj::OnBnClickedWiiiBefore()
{
	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();

	// [����] ���ڵ� 2�� ���ư��Ƿ� ��ŷ���带 �Ⱥ��̰� �Ѵ�.
	TheCartoonMaker->ShowPickBoard(FALSE);

	// [��������]�� Ŭ�� �������� ó���� ���� �κ�
	pParentWnd->ChangeCurDlg(L"Wizard2");

	TheCartoonMaker->SetStateOfUIBefore();
}

void LmUIW3SelObj::OnBnClickedWiiiNext()
{
	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();
	
	ThePickBoard->SetDragThing(NULL);

	// [��������]�� Ŭ�� �������� ó���� ���� �κ�
	pParentWnd->ChangeCurDlg(L"Wizard4");
	TheCartoonMaker->SetStateOfUI(WIZARD4);
}

//
//void LmUIW3SelObj::OnBnClickedWiiiCancle()
//{
//	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
//	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();
//
//	// [��  ��]�� Ŭ�� �������� ó���� ���� �κ�
//	
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
//
//	pParentWnd->ChangeCurDlg(L"MainTool");
//  //���� ������ �����ϵ���
//  pSceneView->EnableChangeScene(TRUE);
//}

//void LmUIW3SelObj::OnBnClickedWiiiComplete()
//{
//	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
//	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();
//	
//	// �޴��� ������� ������
//	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
//	pMainFrame->EnableSceneMenu(TRUE);
//
//	// [��  ��]�� Ŭ�� �������� ó���� ���� �κ�
//
//	// [����]
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
