#pragma once
#include "afxcmn.h"


// LmUICreateMovieClipDlg 대화 상자입니다.

class LmUIMotionThread;

class LmUICreateMovieClipDlg : public CDialog
{
	DECLARE_DYNAMIC(LmUICreateMovieClipDlg)

public:
	LmUICreateMovieClipDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~LmUICreateMovieClipDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CREATE_MOVIE_CLIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	void SetState(DWORD cFrame);
	// 시간 표시용 변수
	CString m_strTime;
	// 현재상태
	CString m_strCurrState;
	// Progress Bar
	CProgressCtrl m_progCreateMovieClip;
	afx_msg void OnBnClickedButton1();

private:
	// 저장될 AVI 파일 이름
	CString m_FileName;
	LmUIMotionThread *m_pMotionThread;
public:
	afx_msg void OnBnClickedCancel();
};
