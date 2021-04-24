#if !defined(AFX_DIALOGTABCTRL_H__E1DB44BB_1780_4B19_B3A2_63DE9E2CE1AA__INCLUDED_)
#define AFX_DIALOGTABCTRL_H__E1DB44BB_1780_4B19_B3A2_63DE9E2CE1AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogTabCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogTabCtrl window

// CArray를 사용하기 위한 헤더 파일
//#include <afxtempl.h>

// 다이얼로그 탭에서 부모 다이얼로그에게 보내는 메시지 정의
#define WM_AT_SELCHANGING		(WM_USER + 1)	// 선택 탭이 변경됨을 알리는 메시지
#define WM_AT_SELCHANGE			(WM_USER + 2)	// 선택 탭이 변경되었음을 알리는 메시지


// 다이얼로그 탭 컨트롤 클래스
class LmUIDialogTabCtrl : public CTabCtrl
{
// Construction
public:
	LmUIDialogTabCtrl();

// Attributes
public:
	// 탭 컨트롤의 탭에 대한 다이얼로그 배열
	CArray<CDialog*, CDialog*>	m_Dialogs;

// Operations
public:
	// 특정 탭을 선택하는 함수
	int SetCurSel(int nItem);
	// 현재 탭을 구현하는 다이얼로그를 리턴하는 함수
	CDialog* GetCurSelDlg();
	// 특정 탭을 구현하는 다이얼로그를 리턴하는 함수
	CDialog* GetDialog(int nItem);

	// 새로운 탭을 생성하는 함수
	BOOL InsertItem(int nItem, TCITEM* pTabCtrlItem, CDialog *pDialog);
	// 새로운 탭을 생성하는 함수
	BOOL InsertItem(int nItem, LPCTSTR lpszItem, CDialog *pDialog);
	// 새로운 탭을 생성하는 함수
	BOOL InsertItem(int nItem, LPCTSTR lpszItem, int nImage, CDialog *pDialog);
	// 새로운 탭을 생성하는 함수
	BOOL InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, int nImage, LPARAM lParam, CDialog *pDialog);
	// 특정 탭을 삭제하는 함수
	BOOL DeleteItem(int nItem);
	// 모든 탭을 삭제하는 함수
	BOOL DeleteAllItems();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogTabCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~LmUIDialogTabCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDialogTabCtrl)
	afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

//멤버변수
private:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGTABCTRL_H__E1DB44BB_1780_4B19_B3A2_63DE9E2CE1AA__INCLUDED_)
