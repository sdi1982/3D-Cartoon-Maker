// LmUIW1SceneName.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUIW1SceneName.h"
#include "LmUIMainToolFormView.h"
#include "MainFrm.h"
#include "LmUISceneView.h"

// LmUIW1SceneName ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(LmUIW1SceneName, CDialog)

LmUIW1SceneName::LmUIW1SceneName(CWnd* pParent /*=NULL*/)
	: CDialog(LmUIW1SceneName::IDD, pParent)
	, m_strSceneName(_T(""))
	, m_nFrameLength(0)
{
	m_nTerrainTexUI = 0;
	m_nTerrainTex =	TERRAIN_GRASS;
}

LmUIW1SceneName::~LmUIW1SceneName()
{
}

void LmUIW1SceneName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SCENE_NAME, m_strSceneName);
	DDX_Control(pDX, IDC_SCENE_NAME, m_ctrlSceneName);
	DDX_Control(pDX, IDC_FRAME_LENGTH, m_ctrlFrameLength);
	DDX_Text(pDX, IDC_FRAME_LENGTH, m_nFrameLength);
	DDX_Control(pDX, IDC_FIELD, m_ctrlGrass);
	DDX_Radio(pDX, IDC_FIELD, m_nTerrainTexUI );
}


BEGIN_MESSAGE_MAP(LmUIW1SceneName, CDialog)
	ON_BN_CLICKED(IDC_WICANCLE, &LmUIW1SceneName::OnBnClickedWicancle)
	ON_BN_CLICKED(IDC_WINEXT, &LmUIW1SceneName::OnBnClickedWinext)
END_MESSAGE_MAP()

//ON_BN_CLICKED(IDC_FIELD, &LmUIW1SceneName::OnBnClickedGrass)
//ON_BN_CLICKED(IDC_DESERT, &LmUIW1SceneName::OnBnClickedDesert)
//ON_BN_CLICKED(IDC_WINTER, &LmUIW1SceneName::OnBnClickedWinter)


// LmUIW1SceneName �޽��� ó�����Դϴ�.

//��Ҹ� �������� �ٽ� ���� Main Tool�� ���ư���.
void LmUIW1SceneName::OnBnClickedWicancle()
{
	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();
	
	// �޴��� ������� ������
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->EnableSceneMenu(TRUE);
	pMainFrame->EnableEditMenu(FALSE);

	//Scene�� ������ �����ϵ��� �Ѵ�.
	LmUISceneView *pSceneView = GetSceneView();
	pSceneView->EnableChangeScene(TRUE);
	
	INT nThumbNailCnt = pSceneView->GetThumNailCount();
	
	if(!nThumbNailCnt)
		pParentWnd->ChangeCurDlg(L"MainNotice");
	else
		pParentWnd->ChangeCurDlg(L"MainTool");

	//������ ���·� �ǵ��� ����. 
	TheCartoonMaker->SetStateOfUIBefore();
}


void LmUIW1SceneName::OnBnClickedWinext()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();
	
	UpdateData(TRUE);

	if(!m_strSceneName.GetLength())
	{
		AfxMessageBox(L"���� �̸��� �ʿ��մϴ�.");
		return;
	}

	switch(m_nTerrainTexUI)
	{
	case 0:
		m_nTerrainTex = TERRAIN_GRASS;
		break;
	case 1:
		m_nTerrainTex = TERRAIN_DESERT;
		break;
	case 2:
		m_nTerrainTex = TERRAIN_SNOW;
		break;
	default:
		break;
	}

	// [����]  ���� �����Ѵ�. -- ���ϰ��� FALSE�� ��� �����Ѵ�

	CString strFrameLength;
	m_ctrlFrameLength.GetWindowText(strFrameLength);
	m_nFrameLength = _wtoi(strFrameLength.GetBuffer(0));

	INT nFramePerSec = TheCartoonMaker->GetFramePerSec();
	BOOL bResult = TheCartoonMaker->CreateScene(m_strSceneName.GetBuffer(0), 
		m_nFrameLength, nFramePerSec, m_nTerrainTex);

	if(!bResult)
	{
		AfxMessageBox(L"���� ���� �̸��� ���� �մϴ�.");
		return;
	}

	// ���� �����ϰ� �� ������ ���� ���� Ÿ�� ������ �����Ѵ�.

	// [����]
	CString strResDirectory;
	GetResourceDirectory(strResDirectory);

	CString strFullFileName;
	strFullFileName.Format(L"%s\\blank.jpg", strResDirectory);

	LmUISceneView * pSceneView = GetSceneView();
	
	//Scene ThumbNail�� Scene�� �߰��Ѵ�.
	//Caret���� ���� ���� ThmInsert, �������� ThmAppend�� �����Ѵ�.
	UINT nThumbCnt = (UINT)pSceneView->m_listThumb.GetCount();

	int nInsIndex = -1;

	if(!nThumbCnt)
		nInsIndex = pSceneView->AddThumbNail(strFullFileName, m_strSceneName);
	else
		nInsIndex = pSceneView->InsertThumbNail(strFullFileName,m_strSceneName);
	
	//��� ���� �� Ŀ���� �ڵ����� �������� �ʴ� ������ ��ħ
	//����, ���� Wizard2���� ��Ҹ� �������� ����Ͽ� ������ �д�.
	if(nInsIndex != -1) //-1�� ����(������ ã�� ����)�� �ǹ���!
	{
		pSceneView->SetThumbNailCaret(nInsIndex);
		m_nInsertedIndex = nInsIndex;
	}
	else
		return; //������!

	//Scene ThumbNail�� �ٽ� �׸���.
	pSceneView->Invalidate();
	CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->EnableEditBackgroundMenu(TRUE);
	pParentWnd->ChangeCurDlg(L"Wizard2");

	//���� ���¸� Wizard2�� �����Ѵ�. 
	TheCartoonMaker->SetStateOfUI(WIZARD2);

}

//LmUISceneView�� �ּҸ� �޾ƿ´�.
LmUISceneView * LmUIW1SceneName::GetSceneView()
{
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
	LmUISceneView* pSceneView = (LmUISceneView*) ( pMainFrame->m_wndSplitUpColExpand.GetPane(0,0) );
	return pSceneView;
}

BOOL LmUIW1SceneName::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_RETURN)
		return TRUE;

	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_ESCAPE)
		return TRUE;


	return CDialog::PreTranslateMessage(pMsg);
}

BOOL LmUIW1SceneName::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	
	m_ctrlGrass.SetCheck(BST_CHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}
