#pragma once
#include "afxwin.h"


// LmUITalkBoxContentDlg 대화 상자입니다.

class LmUITalkBoxContentDlg : public CDialog
{
	DECLARE_DYNAMIC(LmUITalkBoxContentDlg)

public:
	LmUITalkBoxContentDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~LmUITalkBoxContentDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TALKBOX_IN_MOD };

// 멤버 함수
public:
	void SetInputMode(BOOL isInsert);		//입력 모드가 Insert인지 Modify인지 구분하기 위한 함수

// 멤버 변수
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	afx_msg void OnBnClickedChangeColor();
	afx_msg void OnPaint();
};
