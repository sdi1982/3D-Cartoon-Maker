#pragma once
#include "afxwin.h"


// LmUISnowContentDlg 대화 상자입니다.

class LmUISnowContentDlg : public CDialog
{
	DECLARE_DYNAMIC(LmUISnowContentDlg)

public:
	LmUISnowContentDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~LmUISnowContentDlg();

//멤버 함수
public: 
	void SetInputMode(BOOL isInsert);		//입력 모드가 Insert인지 Modify인지 구분하기 위한 함수

//멤버 변수
public: 
	CString m_strSnowName;
	UINT m_nStartFrm;
	UINT m_nSnowFrmLength;
	BOOL m_isInsert;
	UINT m_nOldStartFrm;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SNOW_IN_MOD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_CtrlSnowName;
	CEdit m_ctrlStartFrm;
	CEdit m_ctrlSnowFrmLength;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();
};
