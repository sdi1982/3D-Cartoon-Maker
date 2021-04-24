// FormMainTool.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUIMainToolFormView.h"

#include "MainFrm.h"
#include "LmUISceneView.h"

// CFormMainTool

IMPLEMENT_DYNCREATE(LmUIMainToolFormView, CFormView)

LmUIMainToolFormView::LmUIMainToolFormView()
	: CFormView(LmUIMainToolFormView::IDD)
{
	m_pCurDlg=NULL;
}

LmUIMainToolFormView::~LmUIMainToolFormView()
{
	DeleteAllMap();
}

void LmUIMainToolFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(LmUIMainToolFormView, CFormView)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CFormMainTool �����Դϴ�.

#ifdef _DEBUG
void LmUIMainToolFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void LmUIMainToolFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFormMainTool �޽��� ó�����Դϴ�.



void LmUIMainToolFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	//���̾�α� ����
	m_MainToolDlg.Create(IDD_MAINTOOL, this);
	m_DlgW1SceneName.Create(IDD_WIZARD1, this);
	m_DlgW2SelBakGnd.Create(IDD_WIZARD2, this);
	m_DlgW3SelObj.Create(IDD_WIZARD3, this);
	m_DlgW4SelPerson.Create(IDD_WIZARD4, this);
	m_MainNoticeDlg.Create(IDD_INITNOTICE, this);
	m_TalkBoxContentDlg.Create(IDD_TALKBOX_IN_MOD, this);

	InsertpDlg(&m_MainToolDlg, L"MainTool");
	InsertpDlg(&m_MainNoticeDlg, L"MainNotice");
	InsertpDlg(&m_DlgW1SceneName, L"Wizard1");
	InsertpDlg(&m_DlgW2SelBakGnd, L"Wizard2");
	InsertpDlg(&m_DlgW3SelObj, L"Wizard3");
	InsertpDlg(&m_DlgW4SelPerson, L"Wizard4");
	InsertpDlg(&m_TalkBoxContentDlg, L"TalkBoxContent");

	//���� ���̾� �α� ����
	//	m_pCurDlg = &m_DlgMainTool;
	ChangeCurDlg(L"MainNotice");
	//ChangeCurDlg(L"Wizard1");
}

void LmUIMainToolFormView::InsertpDlg( CDialog * pInDlg, wstring strDlgName )
{
	//Dialog�� map�� ����Ѵ�.
	pInDlg->ShowWindow(SW_HIDE);
	m_mappDlg[strDlgName.c_str()] = pInDlg;
	//TRACE(L"%s\r\n",strDlgName.c_str());
}

void LmUIMainToolFormView::ChangeCurDlg( wstring strDlgName )
{
	
	CRect rectSize;
	GetClientRect(&rectSize);
	
	if(m_pCurDlg)
	{
		m_pCurDlg->SetWindowPos(this, 0, 0, 0, 0,
			SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOZORDER | SWP_HIDEWINDOW);
	}
	
	MAPDIALOG::iterator iTrResult;
	iTrResult = m_mappDlg.find(strDlgName);
	m_pCurDlg = iTrResult->second;

	m_pCurDlg->SetWindowPos(this, 10, 10, 0, 0,
				SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
}

void LmUIMainToolFormView::DeleteAllMap()
{
	m_mappDlg.clear();
}

void LmUIMainToolFormView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	
	//CRect rectSize;
	//GetClientRect(&rectSize);
	//if(m_pCurDlg)
	//	m_pCurDlg->SetWindowPos(this, 10, 10, rectSize.Width(), rectSize.Height(),
	//							SWP_FRAMECHANGED | SWP_NOZORDER | SWP_SHOWWINDOW);
}



BOOL LmUIMainToolFormView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_RETURN)
		return TRUE;

	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_ESCAPE)
		return TRUE;

	return CFormView::PreTranslateMessage(pMsg);
}

void LmUIMainToolFormView::InitWizard()
{
	//ToolFormView�� Wizard1 â�� ����.
	//Wizard1�� �ʱ�ġ ����!
	m_DlgW1SceneName.m_nFrameLength= 1000; //�� 1000 Frame
	m_DlgW1SceneName.m_strSceneName=L"";  //�� �̸�
	m_DlgW1SceneName.m_ctrlGrass.SetCheck(BST_CHECKED);
	m_DlgW1SceneName.m_nTerrainTexUI = 0;
	m_DlgW1SceneName.UpdateData(FALSE);
	
	//�� ������� �ʱ�ȭ �Ѵ�.
	m_DlgW2SelBakGnd.m_tbBackGround.m_nCaret= -1;
	m_DlgW3SelObj.m_tbObject.m_nCaret= -1;
	m_DlgW4SelPerson.m_tbPerson.m_nCaret= -1;
	
	GetSceneView()->EnableChangeScene(FALSE);
	ChangeCurDlg(L"Wizard1");
}

LmUISceneView * LmUIMainToolFormView::GetSceneView()
{
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
	LmUISceneView* pSceneView = (LmUISceneView*) ( pMainFrame->m_wndSplitUpColExpand.GetPane(0,0) );
	return pSceneView;
}
