#pragma once
#include "afxcmn.h"
#include "LmUITalkBoxContentDlg.h"
#include "afxwin.h"

// LmUITalkBoxForm ��ȭ �����Դϴ�.
class LmUITimeLineView;


class LmUITalkBoxForm : public CDialog
{
	DECLARE_DYNAMIC(LmUITalkBoxForm)

public:
	LmUITalkBoxForm(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~LmUITalkBoxForm();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_TALKBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();

// ��� �Լ�
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

//��� �Լ�
public:
	LmUITimeLineView*	GetTimeLineView();		// Ÿ�Ӷ����� �ּҸ� �޾ƿ´�.
	
	void				SetTalkBoxContent( INT iListCount );	//m_listTalk�� iListCount��° Row��
																//TalkBoxDlg Class�� ���� �������ش�.

	void				InsertTalkBoxContent( INT iListCount ); //m_listTalk�� iListCount��° Row��
																	//TalkBoxDlg Class�� ���� �������ش�.

	void				SetTalkBoxListSubItems( INT iListCount ); //m_listTalk�� subItems�� ���� �������ش�.

	void				OnBnClickedTalkDelete();					//m_listTalk�� �ִ� ���õ� Row�� �����.

	CString				GetTalkBoxStyleStr(INT nStyle);
	INT					GetTalkBoxStyleNum(LPCWSTR pcwsTalkBoxStr);
	BOOL				isOKTalkBoxInfo();
	void				SetInputMode(BOOL isInsert);

	//���� �ٲ� ����ϴ� �Լ���
	void InsertTalkBoxContent( LPCWSTR pcwsCharacterName, UINT uFrameStart, UINT uFrameEnd, LPCWSTR pcwsTalkContent , LPCWSTR pcwsTalkBoxStyle );
	void InitTalkBoxList();
	void EnableInsertBtn(BOOL bIsEnable) { m_ctrlInsertTalkBox.EnableWindow(bIsEnable); }

//�������
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
