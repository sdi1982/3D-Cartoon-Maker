#pragma once
#include "afxcmn.h"


// LmUIFireWorksForm ��ȭ �����Դϴ�.

class LmUIFireWorksForm : public CDialog
{
	DECLARE_DYNAMIC(LmUIFireWorksForm)

public:
	LmUIFireWorksForm(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~LmUIFireWorksForm();

//��� �Լ�
public:

//��� ����
public:

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_FIREWORKS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	CBrush m_brush; //Static ��� ���� �ٲ� �� ����� �귯��

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedFireworksInsert();
	afx_msg void OnBnClickedFireworksDelete();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
