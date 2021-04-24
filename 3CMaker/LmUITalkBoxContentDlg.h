#pragma once
#include "afxwin.h"


// LmUITalkBoxContentDlg ��ȭ �����Դϴ�.

class LmUITalkBoxContentDlg : public CDialog
{
	DECLARE_DYNAMIC(LmUITalkBoxContentDlg)

public:
	LmUITalkBoxContentDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~LmUITalkBoxContentDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_TALKBOX_IN_MOD };

// ��� �Լ�
public:
	void SetInputMode(BOOL isInsert);		//�Է� ��尡 Insert���� Modify���� �����ϱ� ���� �Լ�

// ��� ����
public:
	BOOL m_isInsert;

	CString m_strTalkContent;
	UINT m_nStartFrm;
	UINT m_nTalkFrmLength;
	UINT m_nOldStartFrm;

	CEdit m_ctrlTalkContent;
	CEdit m_ctrlStartFrm;
	CEdit m_ctrlTalkFrmLength;
	COLORREF m_TalkBoxColor;

	INT m_nTalkBoxStyle;
	CBrush m_brush;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	afx_msg void OnBnClickedChangeColor();
	afx_msg void OnPaint();
};
