#pragma once
#include "afxcmn.h"
#include "LmUITimeLineView.h"
#include "LmUISnowContentDlg.h"

// LmUISnowForm 대화 상자입니다.
class LmUITimeLineView;
class LmUISnowForm : public CDialog
{
	DECLARE_DYNAMIC(LmUISnowForm)

public:
	LmUISnowForm(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~LmUISnowForm();

//멤버함수
public: 
	LmUITimeLineView* GetTimeLineView();
	void				SetSnowContent(	LmUISnowContentDlg &dlgSnowContent, 
										INT iListCount );	//m_listSnow에 iListCount번째 Row에
															//SnowContentDlg Class의 값을 세팅해준다.
	void				InsertSnowContent(   LmUISnowContentDlg &dlgSnowContent, 
										INT iListCount );	//m_listSnow에 iListCount번째 Row에
															//SnowContentDlg Class의 값을 삽입해준다.
	void				SetSnowListSubItems( LmUISnowContentDlg &dlgSnowContent, 
											INT iListCount ); //m_listSnow에 subItems의 값을 세팅해준다.
															  //SnowContentDlg Class의 값을 삽입해준다.
														
	//씬이 바뀔때 사용하는 함수들
	void InsertSnowContent( UINT uFrameStart, UINT uFrameEnd, LPCWSTR pcwsSnowName );
	void InitSnowList();

//멤버변수
public: 
	CListCtrl m_listSnow;


// 대화 상자 데이터입니다.
	enum { IDD = IDD_SNOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedSnowInsert();
	afx_msg void OnBnClickedSnowModify();
	afx_msg void OnBnClickedSnowDelete();
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

};
