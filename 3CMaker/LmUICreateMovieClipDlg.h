#pragma once
#include "afxcmn.h"


// LmUICreateMovieClipDlg ��ȭ �����Դϴ�.

class LmUIMotionThread;

class LmUICreateMovieClipDlg : public CDialog
{
	DECLARE_DYNAMIC(LmUICreateMovieClipDlg)

public:
	LmUICreateMovieClipDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~LmUICreateMovieClipDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CREATE_MOVIE_CLIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	void SetState(DWORD cFrame);
	// �ð� ǥ�ÿ� ����
	CString m_strTime;
	// �������
	CString m_strCurrState;
	// Progress Bar
	CProgressCtrl m_progCreateMovieClip;
	afx_msg void OnBnClickedButton1();

private:
	// ����� AVI ���� �̸�
	CString m_FileName;
	LmUIMotionThread *m_pMotionThread;
public:
	afx_msg void OnBnClickedCancel();
};
