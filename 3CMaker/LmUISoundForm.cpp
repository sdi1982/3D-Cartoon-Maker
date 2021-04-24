// LmUISoundForm.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUISoundForm.h"
#include "LmUITimeLineView.h"
#include "MainFrm.h"
#include "LmUISoundPrePlay.h"

// LmUISoundForm 대화 상자입니다.

IMPLEMENT_DYNAMIC(LmUISoundForm, CDialog)

LmUISoundForm::LmUISoundForm(CWnd* pParent /*=NULL*/)
	: CDialog(LmUISoundForm::IDD, pParent)
{
	m_brush.CreateSolidBrush(RGB(255,255,255));

}

LmUISoundForm::~LmUISoundForm()
{
}

void LmUISoundForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SOUND_LIST, m_listSound);
}


BEGIN_MESSAGE_MAP(LmUISoundForm, CDialog)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_SOUND_INSERT, &LmUISoundForm::OnBnClickedSoundInsert)
	ON_BN_CLICKED(IDC_SOUND_MODIFY, &LmUISoundForm::OnBnClickedSoundModify)
	ON_BN_CLICKED(IDC_SOUND_DELETE, &LmUISoundForm::OnBnClickedSoundDelete)
	ON_BN_CLICKED(IDC_SOUND_LISTEN, &LmUISoundForm::OnBnClickedSoundPrePlay)
END_MESSAGE_MAP()


// LmUISoundForm 메시지 처리기입니다.

void LmUISoundForm::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.
	CRect rect;
	GetClientRect(&rect);
	dc.SelectStockObject(WHITE_PEN);
	dc.SelectStockObject(WHITE_BRUSH);
	dc.Rectangle(&rect);
}

HBRUSH LmUISoundForm::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkColor(RGB(255,255,255));
	return m_brush;
}

BOOL LmUISoundForm::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_listSound.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	m_listSound.InsertColumn(0,L"시작 프레임",LVCFMT_CENTER, 80);
	m_listSound.InsertColumn(1,L"사운드 이름",LVCFMT_CENTER, 100);
	m_listSound.InsertColumn(2,L"사운드 파일 이름",LVCFMT_CENTER, 195);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void LmUISoundForm::OnBnClickedSoundInsert()
{
	LmUISoundContentDlg dlgSoundContent;

	// 현재 타임라인의 Caret 위치를 읽어온다.
	LmUITimeLineView* pTimeLineView = GetTimeLineView();

	dlgSoundContent.m_nStartFrm = pTimeLineView->GetCurrFrame();

	// 입력 Dialog의 모드를 세팅해준다. TRUE면 Insert Mode, FALSE면 Modify Mode
	dlgSoundContent.SetInputMode(TRUE);
	INT iListCount = m_listSound.GetItemCount();

	if(dlgSoundContent.DoModal() == IDOK)
	{
		//사운드를 등록한다.
		// 씬이름과 결합.		
		if (!TheCartoonMaker->InsertSound(dlgSoundContent.m_strSoundFileName, dlgSoundContent.m_strSoundName)){
			AfxMessageBox(_T("지원하는 형식이 아니거나 잘못된 파일 입니다."));
			return;
		}

		//UI ListCtrl에 대입
		InsertSoundContent(dlgSoundContent, iListCount);

		DWORD dwSoundLength = TheCartoonMaker->GetTotalTime(dlgSoundContent.m_strSoundName);

		TheCartoonMaker->InsertSoundSection(dlgSoundContent.m_strSoundName.GetBuffer(0),
											dlgSoundContent.m_nStartFrm,
											dwSoundLength, 0 ,0);
	
	}
}

void LmUISoundForm::OnBnClickedSoundModify()
{
	// 몇 개가 선택되었는가?
	UINT uSelectedCount = m_listSound.GetSelectedCount();
	int  nItem = -1;

	// 현재 타임라인의 Caret 위치를 읽어온다.
	LmUITimeLineView* pTimeLineView = GetTimeLineView();

	// 1개가 선택되었다면 수정 창이 뜬다.
	if (uSelectedCount > 0)
	{
		//선택된 아이템의 번호를 얻어온다.
		nItem = m_listSound.GetNextItem(nItem, LVNI_SELECTED);
		ASSERT(nItem != -1);

		CString strFrameStart		= m_listSound.GetItemText(nItem,0);
		CString strSoundName		= m_listSound.GetItemText(nItem,1);
		CString strFileFullName		= m_listSound.GetItemText(nItem,2);
		
		LmUISoundContentDlg dlgSoundContent;

		//다이얼 로그를 띄우기 전에 내용을 미리 로딩해 둔다.
		dlgSoundContent.m_nStartFrm = _wtoi(strFrameStart);
		dlgSoundContent.m_strSoundFileName = strFileFullName;
		dlgSoundContent.m_strSoundName = strSoundName;
		dlgSoundContent.SetInputMode(FALSE);

		CString strOldSoundName = dlgSoundContent.m_strSoundName;

		if(dlgSoundContent.DoModal() == IDOK)
		{
			if(!TheCartoonMaker->RemoveSound(strOldSoundName)){
				AfxMessageBox(_T("사운드 수정 실패"));
				return;
			}
			
			if (!TheCartoonMaker->InsertSound(dlgSoundContent.m_strSoundFileName, dlgSoundContent.m_strSoundName)){
				AfxMessageBox(_T("사운드 수정 실패"));
				return;
			}

			SetSoundContent(dlgSoundContent, nItem);

			DWORD dwSoundLength = TheCartoonMaker->GetTotalTime(dlgSoundContent.m_strSoundName); //이것을 이용해서 Frame을 구한다.

			//타임라인을 생신해 준다.
			TheCartoonMaker->ModifySoundSection(strSoundName,
												dlgSoundContent.m_strSoundName.GetBuffer(0),
												dlgSoundContent.m_nStartFrm,
												dwSoundLength, 0 , 0);

		}
	}
}

void LmUISoundForm::OnBnClickedSoundDelete()
{
	UINT uSelectedCount = m_listSound.GetSelectedCount();
	int  nItem = -1;

	LmUITimeLineView* pTimeLineView = GetTimeLineView();

	//1개가 선택되었다면 그 Row(행)을 지운다.
	if (uSelectedCount > 0)
	{
		//선택된 아이템의 번호를 얻어온다.
		nItem = m_listSound.GetNextItem(nItem, LVNI_SELECTED);
		ASSERT(nItem != -1);

		CString strSoundName		= m_listSound.GetItemText(nItem,1);
		if (!TheCartoonMaker->RemoveSound(strSoundName)){
			AfxMessageBox(_T("사운드 제거 실패."));
			return;
		}
		m_listSound.DeleteItem(nItem);
		
		//타임라인에서 등록된 사운드를 지운다.
		TheCartoonMaker->RemoveSoundSection(strSoundName.GetBuffer(0));

	}
}

void LmUISoundForm::OnBnClickedSoundPrePlay()
{
	UINT uSelectedCount = m_listSound.GetSelectedCount();
	int  nItem = -1;

	//1개가 선택되었다면 그 Row(행)을 지운다.
	if (uSelectedCount > 0)
	{
		//선택된 아이템의 번호를 얻어온다.
		nItem = m_listSound.GetNextItem(nItem, LVNI_SELECTED);
		ASSERT(nItem != -1);

		LmUISoundPrePlay dlgSoundPrePlay;
		dlgSoundPrePlay.m_strSoundName = m_listSound.GetItemText(nItem, 1);
		//////////////////////////////////////////////////////////////////////////
		// 여기에 사운드 메니져로 부터 정보를 받아서 미리듣기 다이얼로그에 값을 전달한다.
		// [진우]

		dlgSoundPrePlay.DoModal();

	}

}

//타임라인의 주소를 얻어온다.
LmUITimeLineView* LmUISoundForm::GetTimeLineView()
{
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
	LmUITimeLineView* pTimeLineView = (LmUITimeLineView*) ( pMainFrame->m_wndSplitRow.GetPane(1, 0) );
	return pTimeLineView;
}

void LmUISoundForm::InsertSoundContent( LmUISoundContentDlg & dlgLmUISoundContent, INT iListCount )
{
	CString strStartFrame;
	strStartFrame.Format(L"%d", dlgLmUISoundContent.m_nStartFrm);
	m_listSound.InsertItem(iListCount, strStartFrame);

	//같은 Row에서 나머지 SubItems 세팅
	SetSoundListSubItems(dlgLmUISoundContent,iListCount);
}

void LmUISoundForm::SetSoundContent( LmUISoundContentDlg & dlgLmUISoundContent, INT iListCount )
{
	CString strStartFrame;
	strStartFrame.Format(L"%d", dlgLmUISoundContent.m_nStartFrm);
	m_listSound.SetItemText(iListCount, 0, strStartFrame);

	//같은 Row에서 나머지 SubItems 세팅
	SetSoundListSubItems(dlgLmUISoundContent,iListCount);
}

//나머지 SubItem들을 세팅한다.
void LmUISoundForm::SetSoundListSubItems(LmUISoundContentDlg &dlgLmUISoundContent, INT iListCount )
{
	CString strSoundName = dlgLmUISoundContent.m_strSoundName;
	CString strFileFullName = dlgLmUISoundContent.m_strSoundFileName;

	m_listSound.SetItemText(iListCount, 1, strSoundName);
	m_listSound.SetItemText(iListCount, 2, strFileFullName);
}


BOOL LmUISoundForm::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_RETURN)
		return TRUE;

	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void LmUISoundForm::InitSoundList()
{
	//사운드 리스트를 초기화 시킨다. 
	m_listSound.DeleteAllItems();
}

void LmUISoundForm::InsertSoundContent( LPCWSTR pcwsSoundName, UINT uStartFrame  )
{
	CString strStartFrame;
	CString strFileFullName;

	strFileFullName.Format(L"%s", TheCartoonMaker->GetSoundFileName(pcwsSoundName));

	strStartFrame.Format(L"%d", uStartFrame);
	
	INT iListCount = m_listSound.GetItemCount();

	m_listSound.InsertItem(iListCount, strStartFrame);
	m_listSound.SetItemText(iListCount, 1, pcwsSoundName);
	m_listSound.SetItemText(iListCount, 2, strFileFullName);
}