// LmUISnowForm.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUISnowForm.h"


// LmUISnowForm ��ȭ �����Դϴ�.

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


// LmUISnowForm �޽��� ó�����Դϴ�.

void LmUISnowForm::OnBnClickedSnowInsert()
{
	// ���� Ÿ�Ӷ����� Caret ��ġ�� �о�´�.
	LmUISnowContentDlg dlgSnowContent;

	LmUITimeLineView* pTimeLineView = GetTimeLineView();

	dlgSnowContent.m_nStartFrm = pTimeLineView->GetCurrFrame();
	dlgSnowContent.SetInputMode(TRUE);

	INT iListCount = m_listSnow.GetItemCount();

	//�������� ȿ�� ����Ʈ�� �߰� �Ѵ�.
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
	// �� ���� ���õǾ��°�?
	UINT uSelectedCount = m_listSnow.GetSelectedCount();
	int  nItem = -1;

	// 1���� ���õǾ��ٸ� ���� â�� ���.
	if (uSelectedCount > 0)
	{
		//���õ� �������� ��ȣ�� ���´�.
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

	//1���� ���õǾ��ٸ� �� Row(��)�� �����.
	if (uSelectedCount > 0)
	{
		//���õ� �������� ��ȣ�� ���´�.
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
	
	m_listSnow.InsertColumn(0,L"���� ������",LVCFMT_CENTER, 90);
	m_listSnow.InsertColumn(1,L"�� ������",LVCFMT_CENTER, 90);
	m_listSnow.InsertColumn(2,L"�������� ȿ�� �̸�",LVCFMT_CENTER, 195);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL LmUISnowForm::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_RETURN)
		return TRUE;

	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

//Ÿ�Ӷ����� �ּҸ� ���´�.
LmUITimeLineView* LmUISnowForm::GetTimeLineView()
{
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
	LmUITimeLineView* pTimeLineView = (LmUITimeLineView*) ( pMainFrame->m_wndSplitRow.GetPane(1, 0) );
	return pTimeLineView;
}

void LmUISnowForm::InsertSnowContent(  LmUISnowContentDlg &dlgSnowContent, INT iListCount )
{
	//���� Row���� ������ SubItems ����
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
	//���� Row���� ������ SubItems ����
	SetSnowListSubItems(dlgSnowContent, iListCount);
}

void LmUISnowForm::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CDialog::OnPaint()��(��) ȣ������ ���ʽÿ�.
	CRect rect;
	GetClientRect(&rect);
	dc.SelectStockObject(WHITE_PEN);
	dc.SelectStockObject(WHITE_BRUSH);
	dc.Rectangle(&rect);
}

void LmUISnowForm::InitSnowList()
{
	//����Ʈ�� �ʱ�ȭ ��Ų��.
	m_listSnow.DeleteAllItems();
}