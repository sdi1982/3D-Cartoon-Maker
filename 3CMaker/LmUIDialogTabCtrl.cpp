// DialogTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "LmUIDialogTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogTabCtrl

// ���� �׵θ��� ���̾�α� ������ ����
#define CONST_TAB_GAP		4


LmUIDialogTabCtrl::LmUIDialogTabCtrl()
{

}

LmUIDialogTabCtrl::~LmUIDialogTabCtrl()
{
}


BEGIN_MESSAGE_MAP(LmUIDialogTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CDialogTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGING, OnSelchanging)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogTabCtrl message handlers

void LmUIDialogTabCtrl::OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// ���� �� ��ȣ�� ��´�
	int nTab = GetCurSel();

	// �� �׸��� ���̸� ���Ͽ� ���̾�α��� ��ġ�� ����Ѵ�
	CRect rcItem;
	GetItemRect(0, &rcItem);
	int x = CONST_TAB_GAP;
	int y = rcItem.Height() * GetRowCount() + CONST_TAB_GAP;

	// �� ��ȣ�� �����̶��
	if(nTab >= 0 && nTab < m_Dialogs.GetSize())
	{
		// ���� �����ϴ� ���̾�α� �����͸� ��´�
		CDialog *pDialog = m_Dialogs.GetAt(nTab);
		// ���̾�α׸� �����
		pDialog->SetWindowPos(this, x, y, 0, 0,
			SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOZORDER | SWP_HIDEWINDOW);
	}
	
	// �θ� ���̾�α׿��� ���� �ٲ�� ������ �˸���
	GetParent()->SendMessage(WM_AT_SELCHANGING, GetDlgCtrlID(), nTab);
	
	*pResult = 0;
}

void LmUIDialogTabCtrl::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// ���� �� ��ȣ�� ��´�
	int nTab = GetCurSel();

	// �� �׸��� ���̸� ���Ͽ� ���̾�α��� ��ġ�� ����Ѵ�
	CRect rcItem;
	GetItemRect(0, &rcItem);
	int x = CONST_TAB_GAP;
	int y = rcItem.Height() * GetRowCount() + CONST_TAB_GAP;

	// �� ��ȣ�� �����̶��
	if(nTab >= 0 && nTab < m_Dialogs.GetSize())
	{
		// ���� �����ϴ� ���̾�α� �����͸� ��´�
		CDialog *pDialog = m_Dialogs.GetAt(nTab);
		// ���̾�α׸� ���̰� �����
		pDialog->SetWindowPos(this, x, y, 0, 0,
			SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
	}

	// �θ� ���̾�α׿��� ���� �ٲ������ �˸���
	GetParent()->SendMessage(WM_AT_SELCHANGE, GetDlgCtrlID(), nTab);
	
	//////////////////////////////////////////////////////////////////////////
	// ���� �޽��� ���� ����ġ
	int nCur = GetCurSel();
	switch(nCur)
	{
	case 0: 
		TheCartoonMaker->SetStateOfUI(TALKBOX_TAB);
		break;
	case 1: 
		TheCartoonMaker->SetStateOfUI(SOUND_TAB);
		break;
	case 2: 
		TheCartoonMaker->SetStateOfUI(SNOW_TAB);
		break;
	case 3: 
		TheCartoonMaker->SetStateOfUI(FIREWORKS_TAB);
		break;
	case 4: 
		TheCartoonMaker->SetStateOfUI(CAMERA_TAB);
		break;
	default:
		TheCartoonMaker->SetStateOfUIBefore();
		break;
	}

	*pResult = 0;
}

// Ư�� ���� �����ϴ� �Լ�
int LmUIDialogTabCtrl::SetCurSel(int nItem)
{
	// ���� �� ��ȣ�� ��´�
	int nCur = GetCurSel();
	CDialog *pDialog;

	// ���� ���� �� ��ȣ�� ���� �����ϴ� �� ��ȣ�� �ٸ��ٸ�
	if(nCur != nItem)
	{
		// ���� �� ��ȣ�� �����̶��
		if(nCur >= 0 && nCur < m_Dialogs.GetSize())
		{
			// ���� �����ϴ� ���̾�α� �����͸� ���Ѵ�
			pDialog = m_Dialogs.GetAt(nCur);
			// ���̾�α׸� �����
			pDialog->SetWindowPos(this, 0, 0, 0, 0,
				SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOZORDER | SWP_HIDEWINDOW);
		}

		// ���ο� ���� �����Ѵ�
		CTabCtrl::SetCurSel(nItem);
	}

	CRect rcItem;
	GetItemRect(0, &rcItem);
	int x = CONST_TAB_GAP;
	int y = rcItem.Height() * GetRowCount() + CONST_TAB_GAP;

	// ���� ���ο� �� ��ȣ�� �����̶��
	if(nItem >= 0 && nItem < m_Dialogs.GetSize())
	{
		// ���� �����ϴ� ���̾�α� �����͸� ���Ѵ�
		pDialog = m_Dialogs.GetAt(nItem);
		// ���̾�α׸� ���̰� �����
		pDialog->SetWindowPos(this, x, y, 0, 0,
			SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
	}

	// ���� �� ��ȣ�� �����Ѵ�
	return nCur;
}

// ���� ���� �����ϴ� ���̾�α׸� �����ϴ� �Լ�
CDialog* LmUIDialogTabCtrl::GetCurSelDlg()
{
	// ���� ���õ� �� ��ȣ�� ���Ѵ�
	int nItem = GetCurSel();

	// ���� �� ��ȣ�� ������ �ƴ϶�� �����Ѵ�
	if(nItem < 0 || nItem >= m_Dialogs.GetSize())		return NULL;

	// ���̾�α� �迭���� ���� ���� �����ϴ� ���̾�α� �����͸� �����Ѵ�
	return m_Dialogs.GetAt(nItem);
}

// Ư�� ���� �����ϴ� ���̾�α׸� �����ϴ� �Լ�
CDialog* LmUIDialogTabCtrl::GetDialog(int nItem)
{
	// ���� �� ��ȣ�� ������ �ƴ϶�� �����Ѵ�
	if(nItem < 0 || nItem >= m_Dialogs.GetSize())		return NULL;

	// ���̾�α� �迭���� �ش� ���� �����ϴ� ���̾�α� �����͸� �����Ѵ�
	return m_Dialogs.GetAt(nItem);
}

// ���ο� ���� �����ϴ� �Լ�
BOOL LmUIDialogTabCtrl::InsertItem(int nItem, TCITEM* pTabCtrlItem, CDialog *pDialog)
{
	// ���� �߰��Ѵ�
	if(CTabCtrl::InsertItem(nItem, pTabCtrlItem) < 0)	return FALSE;

	// ���̾�α׸� �����
	pDialog->ShowWindow(SW_HIDE);

	// ���� �� �߰��� �����ϴ� ���̶��
	if(nItem < m_Dialogs.GetSize())
		// �ش� ��ġ�� �����Ѵ�
		m_Dialogs.InsertAt(nItem, pDialog);
	else
		// �迭�� �߰��Ѵ�
		m_Dialogs.Add(pDialog);

	return TRUE;
}

// ���ο� ���� �����ϴ� �Լ�
BOOL LmUIDialogTabCtrl::InsertItem(int nItem, LPCTSTR lpszItem, CDialog *pDialog)
{
	// ���� �߰��Ѵ�
	if(CTabCtrl::InsertItem(nItem, lpszItem) < 0)	return FALSE;

	// ���̾�α׸� �����
	pDialog->ShowWindow(SW_HIDE);

	// ���� �� �߰��� �����ϴ� ���̶��
	if(nItem < m_Dialogs.GetSize())
		// �ش� ��ġ�� �����Ѵ�
		m_Dialogs.InsertAt(nItem, pDialog);
	else
		// �迭�� �߰��Ѵ�
		m_Dialogs.Add(pDialog);

	return TRUE;
}

// ���ο� ���� �����ϴ� �Լ�
BOOL LmUIDialogTabCtrl::InsertItem(int nItem, LPCTSTR lpszItem, int nImage, CDialog *pDialog)
{
	// ���� �߰��Ѵ�
	if(CTabCtrl::InsertItem(nItem, lpszItem, nImage) < 0)	return FALSE;

	// ���̾�α׸� �����
	pDialog->ShowWindow(SW_HIDE);

	// ���� �� �߰��� �����ϴ� ���̶��
	if(nItem < m_Dialogs.GetSize())
		// �ش� ��ġ�� �����Ѵ�
		m_Dialogs.InsertAt(nItem, pDialog);
	else
		// �迭�� �߰��Ѵ�
		m_Dialogs.Add(pDialog);

	return TRUE;
}

// ���ο� ���� �����ϴ� �Լ�
BOOL LmUIDialogTabCtrl::InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, int nImage, LPARAM lParam, CDialog *pDialog)
{
	// ���� �߰��Ѵ�
	if(CTabCtrl::InsertItem(nMask, nItem, lpszItem, nImage, lParam) < 0)	return FALSE;

	// ���̾�α׸� �����
	pDialog->ShowWindow(SW_HIDE);

	// ���� �� �߰��� �����ϴ� ���̶��
	if(nItem < m_Dialogs.GetSize())
		// �ش� ��ġ�� �����Ѵ�
		m_Dialogs.InsertAt(nItem, pDialog);
	else
		// �迭�� �߰��Ѵ�
		m_Dialogs.Add(pDialog);

	return TRUE;
}

// Ư�� ���� �����ϴ� �Լ�
BOOL LmUIDialogTabCtrl::DeleteItem(int nItem)
{
	// ���� �����Ѵ�
	if(!CTabCtrl::DeleteItem(nItem))		return FALSE;

	// �� ��ȣ�� �����̶��
	if(nItem >= 0 && nItem < m_Dialogs.GetSize())
	{
		// �迭���� �ش� ���̾�α� �����͸� �����Ѵ�
		m_Dialogs.RemoveAt(nItem);
	}

	return TRUE;
}

// ��� ���� �����ϴ� �Լ�
BOOL LmUIDialogTabCtrl::DeleteAllItems()
{
	// ��� ���� �����Ѵ�
	if(!CTabCtrl::DeleteAllItems())			return FALSE;

	// �迭�� �ʱ�ȭ�Ѵ�
	m_Dialogs.RemoveAll();

	return TRUE;
}

BOOL LmUIDialogTabCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_RETURN)
		return TRUE;

	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_ESCAPE)
		return TRUE;
 
	if(pMsg->message == WM_LBUTTONDOWN)
	{
		//////////////////////////////////////////////////////////////////////////
		// ����
		// �̷��� �� �� ������ ���� ���� ��� ���¸� ���� �ϱ� ������.
		// �׷��� ���� �ٲ�� �ٷ� �ٸ� ���� ������.
		int nCur = GetCurSel();
		switch(nCur)
		{
		case 0: 
			TheCartoonMaker->SetStateOfUI(TALKBOX_TAB);
			break;
		case 1: 
			TheCartoonMaker->SetStateOfUI(SOUND_TAB);
			break;
		case 2: 
			TheCartoonMaker->SetStateOfUI(SNOW_TAB);
			break;
		case 3: 
			TheCartoonMaker->SetStateOfUI(FIREWORKS_TAB);
			break;
		case 4: 
			TheCartoonMaker->SetStateOfUI(CAMERA_TAB);
			break;
		default:
			TheCartoonMaker->SetStateOfUIBefore();
			break;
		}
	}

	return CTabCtrl::PreTranslateMessage(pMsg);
}
