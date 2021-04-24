// LmUIW4SelPerson.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUIW4SelPerson.h"
#include "LmUIMainToolFormView.h"

#include "MainFrm.h"
#include "LmUISceneView.h"

// LmUIW4SelPerson ��ȭ �����Դϴ�.

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


// LmUIW4SelPerson �޽��� ó�����Դϴ�.

BOOL LmUIW4SelPerson::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CRect rectSize(10, 35, 410, 550) ;

	m_tbPerson.Create(NULL,NULL, WS_CHILD | WS_VISIBLE | WS_DLGFRAME ,rectSize, this,THUMBNAME_PERSON);

	//m_tbPerson.ShowWindow(SW_SHOW);
	
	wstring wsPersonName = TheCartoonMaker->GetListCharFirst();

	//����� ���� ��� �׳� �����Ѵ�.
	if(wsPersonName == L"")
		return TRUE;

	//����� ���� ���.. ����Ѵ�.
	CString strResDirectory;
	GetResourceDirectory(strResDirectory);

	CString strFullFileName;
	strFullFileName.Format(L"%s\\%s.jpg", strResDirectory,wsPersonName.c_str());

	m_tbPerson.ThmInsert(strFullFileName, wsPersonName.c_str());

	while(wsPersonName != L"")
	{
		//����Ʈ ���̸� NULL wstring�� ��ȯ�Ѵ�.
		wsPersonName = TheCartoonMaker->GetListCharNext();
		if(wsPersonName != L"")
		{
			strFullFileName.Format(L"%s\\%s.jpg", strResDirectory,wsPersonName.c_str());
			m_tbPerson.ThmInsert(strFullFileName, wsPersonName.c_str());
		}
	} 

	m_tbPerson.SetItemType(CHARECTERS);
	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void LmUIW4SelPerson::OnBnClickedWivCancle()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();

	// [��  ��]�� Ŭ�� �������� ó���� ���� �κ�
	
	// �̹� ��ϵǾ���� ThumbNail�� ����!

	//���� �����ӿ��� LmUISceneView Class�� �ּҸ� ���´�. 
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
	LmUISceneView* pSceneView = GetSceneView();

	// �޴��� ������� ������
	pMainFrame->EnableSceneMenu(TRUE);

	//�̹� ��ϵǾ� ���� ThumbNail�� ���� �Ѵ�.
	pSceneView->RemoveThumbNail(pParentWnd->m_DlgW1SceneName.m_nInsertedIndex);
	pSceneView->Invalidate();

	// [����] ����� ���� �����Ѵ�.
	TheCartoonMaker->ShowPickBoard(FALSE);
	TheCartoonMaker->DeleteScene(pParentWnd->m_DlgW1SceneName.m_strSceneName);
	
	//�޴� ����
	pMainFrame->EnableAllMenu(TRUE);
	pMainFrame->EnableEditMenu(FALSE);
	pMainFrame->EnableCharacterMenu(FALSE);

	//���� ������ �����ϵ��� �Ѵ�.
	pSceneView->EnableChangeScene(TRUE);

	//���� ���°�� ���� ����϶�� �޽����� ������ Tool ������ ����.
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
	// [��������]�� Ŭ�� �������� ó���� ���� �κ�
	pParentWnd->ChangeCurDlg(L"Wizard3");
	TheCartoonMaker->SetStateOfUIBefore();
}

void LmUIW4SelPerson::OnBnClickedWivComplete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();
	LmUISceneView *pSceneView = GetSceneView();
	// �޴��� ������� ������
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();


	// [����] �Ϸ�ÿ� ��ŷ���带 �Ⱥ��̰� ó���Ѵ�
	TheCartoonMaker->ShowPickBoard(FALSE);
	
	// ����ȭ���� ù��° ���������� ������.
	TheCartoonMaker->SetCurFrame(1);
	// [����] ȭ���� ĸ���Ѵ�.
	// [����] ��ũ �Ž÷� ���� �ϵ忡 ������ ������ ������� �ʾƼ� ��� �Լ��� ���� �������.
	//TheCartoonMaker->ScreenShot();   //�� �Լ������� �ʿ� ����.
	
	// [��  ��]�� Ŭ�� �������� ó���� ���� �κ�
	pParentWnd->ChangeCurDlg(L"MainTool");

	pMainFrame->EnableAllMenu( TRUE );
	
	//SceneView���� ȭ���� �������ش�.
	
	//ȭ���� ĸ�� �Ѵ�.
	IPicture *pPic = TheCartoonMaker->ScreenToMemory();

	INT nThumbCaret = pSceneView->GetThumbNailCaret();
	pSceneView->ReplaceThumbNail(nThumbCaret, pPic);

	//�޴� ����
	pMainFrame->EnableAllMenu(TRUE);

	pMainFrame->EnableEditMenu(FALSE);
	pMainFrame->EnableCharacterMenu(FALSE);
	//���� ������ �����ϵ��� �Ѵ�.
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
