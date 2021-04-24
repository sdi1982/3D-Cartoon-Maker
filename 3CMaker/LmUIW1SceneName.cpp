// LmUIW1SceneName.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUIW1SceneName.h"
#include "LmUIMainToolFormView.h"
#include "MainFrm.h"
#include "LmUISceneView.h"

// LmUIW1SceneName 대화 상자입니다.

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


// LmUIW1SceneName 메시지 처리기입니다.

//취소를 했을때는 다시 원래 Main Tool로 돌아간다.
void LmUIW1SceneName::OnBnClickedWicancle()
{
	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();
	
	// 메뉴를 원래대로 돌린다
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->EnableSceneMenu(TRUE);
	pMainFrame->EnableEditMenu(FALSE);

	//Scene의 변경을 가능하도록 한다.
	LmUISceneView *pSceneView = GetSceneView();
	pSceneView->EnableChangeScene(TRUE);
	
	INT nThumbNailCnt = pSceneView->GetThumNailCount();
	
	if(!nThumbNailCnt)
		pParentWnd->ChangeCurDlg(L"MainNotice");
	else
		pParentWnd->ChangeCurDlg(L"MainTool");

	//이전의 상태로 되돌아 간다. 
	TheCartoonMaker->SetStateOfUIBefore();
}


void LmUIW1SceneName::OnBnClickedWinext()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	LmUIMainToolFormView * pParentWnd = (LmUIMainToolFormView*)GetParent();
	
	UpdateData(TRUE);

	if(!m_strSceneName.GetLength())
	{
		AfxMessageBox(L"씬의 이름이 필요합니다.");
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

	// [동인]  씬을 생성한다. -- 리턴값이 FALSE인 경우 리턴한다

	CString strFrameLength;
	m_ctrlFrameLength.GetWindowText(strFrameLength);
	m_nFrameLength = _wtoi(strFrameLength.GetBuffer(0));

	INT nFramePerSec = TheCartoonMaker->GetFramePerSec();
	BOOL bResult = TheCartoonMaker->CreateScene(m_strSceneName.GetBuffer(0), 
		m_nFrameLength, nFramePerSec, m_nTerrainTex);

	if(!bResult)
	{
		AfxMessageBox(L"같은 씬의 이름이 존재 합니다.");
		return;
	}

	// 씬을 생성하고 난 다음에 현재 지형 타입 정보를 저장한다.

	// [진왕]
	CString strResDirectory;
	GetResourceDirectory(strResDirectory);

	CString strFullFileName;
	strFullFileName.Format(L"%s\\blank.jpg", strResDirectory);

	LmUISceneView * pSceneView = GetSceneView();
	
	//Scene ThumbNail에 Scene을 추가한다.
	//Caret값이 있을 때는 ThmInsert, 없을때는 ThmAppend를 실행한다.
	UINT nThumbCnt = (UINT)pSceneView->m_listThumb.GetCount();

	int nInsIndex = -1;

	if(!nThumbCnt)
		nInsIndex = pSceneView->AddThumbNail(strFullFileName, m_strSceneName);
	else
		nInsIndex = pSceneView->InsertThumbNail(strFullFileName,m_strSceneName);
	
	//모두 삭제 후 커서가 자동으로 설정되지 않는 문제를 고침
	//또한, 다음 Wizard2에서 취소를 누를때를 대비하여 저장해 둔다.
	if(nInsIndex != -1) //-1은 에러(파일을 찾지 못함)를 의미함!
	{
		pSceneView->SetThumbNailCaret(nInsIndex);
		m_nInsertedIndex = nInsIndex;
	}
	else
		return; //에러임!

	//Scene ThumbNail을 다시 그린다.
	pSceneView->Invalidate();
	CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->EnableEditBackgroundMenu(TRUE);
	pParentWnd->ChangeCurDlg(L"Wizard2");

	//현재 상태를 Wizard2로 지정한다. 
	TheCartoonMaker->SetStateOfUI(WIZARD2);

}

//LmUISceneView의 주소를 받아온다.
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

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	
	m_ctrlGrass.SetCheck(BST_CHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
