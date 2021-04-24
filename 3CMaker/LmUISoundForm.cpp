// LmUISoundForm.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUISoundForm.h"
#include "LmUITimeLineView.h"
#include "MainFrm.h"
#include "LmUISoundPrePlay.h"

// LmUISoundForm ��ȭ �����Դϴ�.

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


// LmUISoundForm �޽��� ó�����Դϴ�.

void LmUISoundForm::OnPaint()
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

HBRUSH LmUISoundForm::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkColor(RGB(255,255,255));
	return m_brush;
}

BOOL LmUISoundForm::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_listSound.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	m_listSound.InsertColumn(0,L"���� ������",LVCFMT_CENTER, 80);
	m_listSound.InsertColumn(1,L"���� �̸�",LVCFMT_CENTER, 100);
	m_listSound.InsertColumn(2,L"���� ���� �̸�",LVCFMT_CENTER, 195);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void LmUISoundForm::OnBnClickedSoundInsert()
{
	LmUISoundContentDlg dlgSoundContent;

	// ���� Ÿ�Ӷ����� Caret ��ġ�� �о�´�.
	LmUITimeLineView* pTimeLineView = GetTimeLineView();

	dlgSoundContent.m_nStartFrm = pTimeLineView->GetCurrFrame();

	// �Է� Dialog�� ��带 �������ش�. TRUE�� Insert Mode, FALSE�� Modify Mode
	dlgSoundContent.SetInputMode(TRUE);
	INT iListCount = m_listSound.GetItemCount();

	if(dlgSoundContent.DoModal() == IDOK)
	{
		//���带 ����Ѵ�.
		// ���̸��� ����.		
		if (!TheCartoonMaker->InsertSound(dlgSoundContent.m_strSoundFileName, dlgSoundContent.m_strSoundName)){
			AfxMessageBox(_T("�����ϴ� ������ �ƴϰų� �߸��� ���� �Դϴ�."));
			return;
		}

		//UI ListCtrl�� ����
		InsertSoundContent(dlgSoundContent, iListCount);

		DWORD dwSoundLength = TheCartoonMaker->GetTotalTime(dlgSoundContent.m_strSoundName);

		TheCartoonMaker->InsertSoundSection(dlgSoundContent.m_strSoundName.GetBuffer(0),
											dlgSoundContent.m_nStartFrm,
											dwSoundLength, 0 ,0);
	
	}
}

void LmUISoundForm::OnBnClickedSoundModify()
{
	// �� ���� ���õǾ��°�?
	UINT uSelectedCount = m_listSound.GetSelectedCount();
	int  nItem = -1;

	// ���� Ÿ�Ӷ����� Caret ��ġ�� �о�´�.
	LmUITimeLineView* pTimeLineView = GetTimeLineView();

	// 1���� ���õǾ��ٸ� ���� â�� ���.
	if (uSelectedCount > 0)
	{
		//���õ� �������� ��ȣ�� ���´�.
		nItem = m_listSound.GetNextItem(nItem, LVNI_SELECTED);
		ASSERT(nItem != -1);

		CString strFrameStart		= m_listSound.GetItemText(nItem,0);
		CString strSoundName		= m_listSound.GetItemText(nItem,1);
		CString strFileFullName		= m_listSound.GetItemText(nItem,2);
		
		LmUISoundContentDlg dlgSoundContent;

		//���̾� �α׸� ���� ���� ������ �̸� �ε��� �д�.
		dlgSoundContent.m_nStartFrm = _wtoi(strFrameStart);
		dlgSoundContent.m_strSoundFileName = strFileFullName;
		dlgSoundContent.m_strSoundName = strSoundName;
		dlgSoundContent.SetInputMode(FALSE);

		CString strOldSoundName = dlgSoundContent.m_strSoundName;

		if(dlgSoundContent.DoModal() == IDOK)
		{
			if(!TheCartoonMaker->RemoveSound(strOldSoundName)){
				AfxMessageBox(_T("���� ���� ����"));
				return;
			}
			
			if (!TheCartoonMaker->InsertSound(dlgSoundContent.m_strSoundFileName, dlgSoundContent.m_strSoundName)){
				AfxMessageBox(_T("���� ���� ����"));
				return;
			}

			SetSoundContent(dlgSoundContent, nItem);

			DWORD dwSoundLength = TheCartoonMaker->GetTotalTime(dlgSoundContent.m_strSoundName); //�̰��� �̿��ؼ� Frame�� ���Ѵ�.

			//Ÿ�Ӷ����� ������ �ش�.
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

	//1���� ���õǾ��ٸ� �� Row(��)�� �����.
	if (uSelectedCount > 0)
	{
		//���õ� �������� ��ȣ�� ���´�.
		nItem = m_listSound.GetNextItem(nItem, LVNI_SELECTED);
		ASSERT(nItem != -1);

		CString strSoundName		= m_listSound.GetItemText(nItem,1);
		if (!TheCartoonMaker->RemoveSound(strSoundName)){
			AfxMessageBox(_T("���� ���� ����."));
			return;
		}
		m_listSound.DeleteItem(nItem);
		
		//Ÿ�Ӷ��ο��� ��ϵ� ���带 �����.
		TheCartoonMaker->RemoveSoundSection(strSoundName.GetBuffer(0));

	}
}

void LmUISoundForm::OnBnClickedSoundPrePlay()
{
	UINT uSelectedCount = m_listSound.GetSelectedCount();
	int  nItem = -1;

	//1���� ���õǾ��ٸ� �� Row(��)�� �����.
	if (uSelectedCount > 0)
	{
		//���õ� �������� ��ȣ�� ���´�.
		nItem = m_listSound.GetNextItem(nItem, LVNI_SELECTED);
		ASSERT(nItem != -1);

		LmUISoundPrePlay dlgSoundPrePlay;
		dlgSoundPrePlay.m_strSoundName = m_listSound.GetItemText(nItem, 1);
		//////////////////////////////////////////////////////////////////////////
		// ���⿡ ���� �޴����� ���� ������ �޾Ƽ� �̸���� ���̾�α׿� ���� �����Ѵ�.
		// [����]

		dlgSoundPrePlay.DoModal();

	}

}

//Ÿ�Ӷ����� �ּҸ� ���´�.
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

	//���� Row���� ������ SubItems ����
	SetSoundListSubItems(dlgLmUISoundContent,iListCount);
}

void LmUISoundForm::SetSoundContent( LmUISoundContentDlg & dlgLmUISoundContent, INT iListCount )
{
	CString strStartFrame;
	strStartFrame.Format(L"%d", dlgLmUISoundContent.m_nStartFrm);
	m_listSound.SetItemText(iListCount, 0, strStartFrame);

	//���� Row���� ������ SubItems ����
	SetSoundListSubItems(dlgLmUISoundContent,iListCount);
}

//������ SubItem���� �����Ѵ�.
void LmUISoundForm::SetSoundListSubItems(LmUISoundContentDlg &dlgLmUISoundContent, INT iListCount )
{
	CString strSoundName = dlgLmUISoundContent.m_strSoundName;
	CString strFileFullName = dlgLmUISoundContent.m_strSoundFileName;

	m_listSound.SetItemText(iListCount, 1, strSoundName);
	m_listSound.SetItemText(iListCount, 2, strFileFullName);
}


BOOL LmUISoundForm::PreTranslateMessage(MSG* pMsg)
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

void LmUISoundForm::InitSoundList()
{
	//���� ����Ʈ�� �ʱ�ȭ ��Ų��. 
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