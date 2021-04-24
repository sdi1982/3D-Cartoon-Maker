// LmUISnowForm.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUISnowForm.h"


// LmUISnowForm 대화 상자입니다.

IMPLEMENT_DYNAMIC(LmUISnowForm, CDialog)

LmUISnowForm::LmUISnowForm(CWnd* pParent /*=NULL*/)
	: CDialog(LmUISnowForm::IDD, pParent)
{

}

LmUISnowForm::~LmUISnowForm()
{
}

void LmUISnowForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SNOW_LIST, m_listSnow);
}


BEGIN_MESSAGE_MAP(LmUISnowForm, CDialog)
	ON_BN_CLICKED(IDC_SNOW_INSERT, &LmUISnowForm::OnBnClickedSnowInsert)
	ON_BN_CLICKED(IDC_SNOW_MODIFY, &LmUISnowForm::OnBnClickedSnowModify)
	ON_BN_CLICKED(IDC_SNOW_DELETE, &LmUISnowForm::OnBnClickedSnowDelete)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// LmUISnowForm 메시지 처리기입니다.

void LmUISnowForm::OnBnClickedSnowInsert()
{
	// 현재 타임라인의 Caret 위치를 읽어온다.
	LmUISnowContentDlg dlgSnowContent;

	LmUITimeLineView* pTimeLineView = GetTimeLineView();

	dlgSnowContent.m_nStartFrm = pTimeLineView->GetCurrFrame();
	dlgSnowContent.SetInputMode(TRUE);

	INT iListCount = m_listSnow.GetItemCount();

	//눈내리기 효과 리스트에 추가 한다.
	if(dlgSnowContent.DoModal() == IDOK)
	{
		InsertSnowContent(dlgSnowContent, iListCount);
		TheCartoonMaker->InsertSnowEffect(dlgSnowContent.m_strSnowName.GetBuffer(0), 
			dlgSnowContent.m_nStartFrm, dlgSnowContent.m_nSnowFrmLength);
		
		UINT iCurFrame = TheCartoonMaker->GetCurFrame();
		TheCartoonMaker->SetCurFrame(iCurFrame);
	}

}

void LmUISnowForm::OnBnClickedSnowModify()
{
	// 몇 개가 선택되었는가?
	UINT uSelectedCount = m_listSnow.GetSelectedCount();
	int  nItem = -1;

	// 1개가 선택되었다면 수정 창이 뜬다.
	if (uSelectedCount > 0)
	{
		//선택된 아이템의 번호를 얻어온다.
		nItem = m_listSnow.GetNextItem(nItem, LVNI_SELECTED);
		ASSERT(nItem != -1);

		CString strFrameStart		= m_listSnow.GetItemText(nItem,0);
		CString strFrameLength		= m_listSnow.GetItemText(nItem,1);
		CString strSnowName			= m_listSnow.GetItemText(nItem,2);

		LmUISnowContentDlg dlgSnowContent;

		dlgSnowContent.m_nStartFrm = _wtoi(strFrameStart);
		dlgSnowContent.m_nSnowFrmLength = _wtoi(strFrameLength) - dlgSnowContent.m_nStartFrm + 1;
		dlgSnowContent.m_strSnowName = strSnowName;
		dlgSnowContent.m_nOldStartFrm = _wtoi(strFrameStart);
		dlgSnowContent.SetInputMode(FALSE);

		INT nOldStart = _wtoi(strFrameStart);

		if(dlgSnowContent.DoModal() == IDOK)
		{
			SetSnowContent(dlgSnowContent, nItem);
			TheCartoonMaker->ModifySnowEffect(nOldStart, 
				dlgSnowContent.m_strSnowName, 
				dlgSnowContent.m_nStartFrm,
				dlgSnowContent.m_nSnowFrmLength);

			UINT iCurFrame = TheCartoonMaker->GetCurFrame();
			TheCartoonMaker->SetCurFrame(iCurFrame);
		}
	}
}

void LmUISnowForm::OnBnClickedSnowDelete()
{
	UINT uSelectedCount = m_listSnow.GetSelectedCount();
	int  nItem = -1;

	//1개가 선택되었다면 그 Row(행)을 지운다.
	if (uSelectedCount > 0)
	{
		//선택된 아이템의 번호를 얻어온다.
		nItem = m_listSnow.GetNextItem(nItem, LVNI_SELECTED);
		ASSERT(nItem != -1);

		CString strFrameStart		= m_listSnow.GetItemText(nItem,0);
		//CString strFrameEnd			= m_listSnow.GetItemText(nItem,1);
		INT nStart = _wtoi(strFrameStart);
		//INT nEnd   = _wtoi(strFrameEnd);

		m_listSnow.DeleteItem(nItem);
		TheCartoonMaker->RemoveSnowEffect(nStart);

		UINT iCurFrame = TheCartoonMaker->GetCurFrame();
		TheCartoonMaker->SetCurFrame(iCurFrame);
	}
}

BOOL LmUISnowForm::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_listSnow.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	
	m_listSnow.InsertColumn(0,L"시작 프레임",LVCFMT_CENTER, 90);
	m_listSnow.InsertColumn(1,L"끝 프레임",LVCFMT_CENTER, 90);
	m_listSnow.InsertColumn(2,L"눈내리기 효과 이름",LVCFMT_CENTER, 195);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL LmUISnowForm::PreTranslateMessage(MSG* pMsg)
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

//타임라인의 주소를 얻어온다.
LmUITimeLineView* LmUISnowForm::GetTimeLineView()
{
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
	LmUITimeLineView* pTimeLineView = (LmUITimeLineView*) ( pMainFrame->m_wndSplitRow.GetPane(1, 0) );
	return pTimeLineView;
}

void LmUISnowForm::InsertSnowContent(  LmUISnowContentDlg &dlgSnowContent, INT iListCount )
{
	//같은 Row에서 나머지 SubItems 세팅
	CString strStartFrame;
	strStartFrame.Format(L"%d", dlgSnowContent.m_nStartFrm);
	m_listSnow.InsertItem(iListCount, strStartFrame);

	SetSnowListSubItems(dlgSnowContent, iListCount);
}

void LmUISnowForm::InsertSnowContent( UINT uFrameStart, UINT uFrameEnd, LPCWSTR pcwsSnowName )
{
	CString strFrameStart;
	CString strFrameEnd;

	strFrameStart.Format(L"%d", uFrameStart);
	strFrameEnd.Format(L"%d" ,uFrameEnd);
	
	INT iListCount = m_listSnow.GetItemCount();

	m_listSnow.InsertItem(iListCount, strFrameStart);
	m_listSnow.SetItemText(iListCount, 1, strFrameEnd);
	m_listSnow.SetItemText(iListCount, 2, pcwsSnowName);

}

void LmUISnowForm::SetSnowListSubItems(  LmUISnowContentDlg &dlgSnowContent, INT iListCount )
{
	CString strFrameStart;
	CString strFrameEnd;
	CString strSnowContent	=	dlgSnowContent.m_strSnowName;

	strFrameStart.Format(L"%d", dlgSnowContent.m_nStartFrm);
	strFrameEnd.Format(L"%d", dlgSnowContent.m_nStartFrm + dlgSnowContent.m_nSnowFrmLength -1);

	m_listSnow.SetItemText(iListCount, 1, strFrameEnd);
	m_listSnow.SetItemText(iListCount, 2, strSnowContent);
}

void LmUISnowForm::SetSnowContent( LmUISnowContentDlg &dlgSnowContent, INT iListCount )
{
	//같은 Row에서 나머지 SubItems 세팅
	SetSnowListSubItems(dlgSnowContent, iListCount);
}

void LmUISnowForm::OnPaint()
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

void LmUISnowForm::InitSnowList()
{
	//리스트를 초기화 시킨다.
	m_listSnow.DeleteAllItems();
}