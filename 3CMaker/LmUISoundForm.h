#pragma once
#include "afxcmn.h"
#include "LmUISoundContentDlg.h"

// LmUISoundForm 대화 상자입니다.
class LmUITimeLineView;

class LmUISoundForm : public CDialog
{
	DECLARE_DYNAMIC(LmUISoundForm)

public:
	LmUISoundForm(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~LmUISoundForm();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SOUND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()


	// 멤버 함수
public:
	void InsertSoundContent(LmUISoundContentDlg & dlgLmUISoundContent, INT iListCount); //m_listSound에 iListCount번째 Row에
																						//LmUISoundContentDlg Class의 값을 삽입해준다.
	void SetSoundContent(LmUISoundContentDlg & dlgLmUISoundContent, INT iListCount);	//m_listSound에 iListCount번째 Row에
																						//LmUISoundContentDlg Class의 값을 세팅해준다.
	void SetSoundListSubItems(LmUISoundContentDlg &dlgLmUISoundContent, INT iListCount ); //m_listTalk에 subItems의 값을 세팅해준다.

	LmUITimeLineView*	GetTimeLineView();		// 타임라인의 주소를 받아온다.

	//씬이 바뀔때 사용하는 함수들
	void InsertSoundContent( LPCWSTR pcwsSoundName, UINT uStartFrame  );
	void InitSoundList();


	// 멤버 변수
public:
	// 사운드 리스트
	CListCtrl m_listSound;


public:
	afx_msg void OnPaint();


private:
	CBrush m_brush;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	
	afx_msg void OnBnClickedSoundInsert();
	afx_msg void OnBnClickedSoundModify();
	afx_msg void OnBnClickedSoundDelete();
	afx_msg void OnBnClickedSoundPrePlay();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
