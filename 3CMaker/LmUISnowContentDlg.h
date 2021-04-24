#pragma once
#include "afxwin.h"


// LmUISnowContentDlg ��ȭ �����Դϴ�.

class LmUISnowContentDlg : public CDialog
{
	DECLARE_DYNAMIC(LmUISnowContentDlg)

public:
	LmUISnowContentDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~LmUISnowContentDlg();

//��� �Լ�
public: 
	void SetInputMode(BOOL isInsert);		//�Է� ��尡 Insert���� Modify���� �����ϱ� ���� �Լ�

//��� ����
public: 
	CString m_strSnowName;
	UINT m_nStartFrm;
	UINT m_nSnowFrmLength;
	BOOL m_isInsert;
	UINT m_nOldStartFrm;

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SNOW_IN_MOD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_CtrlSnowName;
	CEdit m_ctrlStartFrm;
	CEdit m_ctrlSnowFrmLength;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();
};
