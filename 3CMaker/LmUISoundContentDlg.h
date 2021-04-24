#pragma once
#include "afxwin.h"

// LmUISoundContentDlg 대화 상자입니다.

class LmUISoundContentDlg : public CDialog
{
	DECLARE_DYNAMIC(LmUISoundContentDlg)

public:
	LmUISoundContentDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~LmUISoundContentDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SOUND_IN_MOD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

//멤버 함수
public:
	void SetInputMode(BOOL isInsert);		//입력 모드가 Insert인지 Modify인지 구분하기 위한 함수

//멤버 변수
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
