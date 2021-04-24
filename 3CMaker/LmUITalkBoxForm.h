#pragma once
#include "afxcmn.h"
#include "LmUITalkBoxContentDlg.h"
#include "afxwin.h"

// LmUITalkBoxForm 대화 상자입니다.
class LmUITimeLineView;


class LmUITalkBoxForm : public CDialog
{
	DECLARE_DYNAMIC(LmUITalkBoxForm)

public:
	LmUITalkBoxForm(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~LmUITalkBoxForm();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TALKBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();

// 멤버 함수
public:
	CString m_strTalkContent;
	UINT m_nStartFrm;
	UINT m_nTalkFrmLength;
	UINT m_nOldStartFrm;

	CEdit m_ctrlTalkContent;
	CEdit m_ctrlStartFrm;
	CEdit m_ctrlTalkFrmLength;
	COLORREF m_TalkBoxColor;
	CBrush m_brushColor;

	INT m_nTalkBoxStyle;
	
	BOOL m_isInsert;

//멤버 함수
public:
	LmUITimeLineView*	GetTimeLineView();		// 타임라인의 주소를 받아온다.
	
	void				SetTalkBoxContent( INT iListCount );	//m_listTalk에 iListCount번째 Row에
																//TalkBoxDlg Class의 값을 세팅해준다.

	void				InsertTalkBoxContent( INT iListCount ); //m_listTalk에 iListCount번째 Row에
																	//TalkBoxDlg Class의 값을 삽입해준다.

	void				SetTalkBoxListSubItems( INT iListCount ); //m_listTalk에 subItems의 값을 세팅해준다.

	void				OnBnClickedTalkDelete();					//m_listTalk에 있는 선택된 Row을 지운다.

	CString				GetTalkBoxStyleStr(INT nStyle);
	INT					GetTalkBoxStyleNum(LPCWSTR pcwsTalkBoxStr);
	BOOL				isOKTalkBoxInfo();
	void				SetInputMode(BOOL isInsert);

	//씬이 바뀔때 사용하는 함수들
	void InsertTalkBoxContent( LPCWSTR pcwsCharacterName, UINT uFrameStart, UINT uFrameEnd, LPCWSTR pcwsTalkContent , LPCWSTR pcwsTalkBoxStyle );
	void InitTalkBoxList();
	void EnableInsertBtn(BOOL bIsEnable) { m_ctrlInsertTalkBox.EnableWindow(bIsEnable); }

//멤버변수
private:
	CListCtrl m_listTalkBox;
	CButton m_ctrlInsertTalkBox;
	CBrush m_brush;


	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedTalkModify();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void OnBnClickedChangeColor();
	afx_msg void OnNMDblclkTalkbox(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedTalkNowFrame();
	afx_msg void OnBnClickedTalkNewInput();
	afx_msg void OnBnClickedTalkInsert();
	
};
