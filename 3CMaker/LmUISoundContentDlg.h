#pragma once
#include "afxwin.h"

// LmUISoundContentDlg ��ȭ �����Դϴ�.

class LmUISoundContentDlg : public CDialog
{
	DECLARE_DYNAMIC(LmUISoundContentDlg)

public:
	LmUISoundContentDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~LmUISoundContentDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SOUND_IN_MOD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

//��� �Լ�
public:
	void SetInputMode(BOOL isInsert);		//�Է� ��尡 Insert���� Modify���� �����ϱ� ���� �Լ�

//��� ����
public:
	CString			m_strSoundFileName;
	CString			m_strSoundName;
	UINT			m_nStartFrm;

	CEdit			m_ctrlFileFullName;
	CEdit			m_ctrlSoundName;
	CEdit			m_ctrlStartFrm;

	BOOL			m_isInsert;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedSoundFileOpen();
	afx_msg void OnBnClickedOk();
};
