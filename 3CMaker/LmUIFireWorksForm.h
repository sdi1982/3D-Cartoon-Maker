#pragma once
#include "afxcmn.h"


// LmUIFireWorksForm 대화 상자입니다.

class LmUIFireWorksForm : public CDialog
{
	DECLARE_DYNAMIC(LmUIFireWorksForm)

public:
	LmUIFireWorksForm(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~LmUIFireWorksForm();

//멤버 함수
public:

//멤버 변수
public:

// 대화 상자 데이터입니다.
	enum { IDD = IDD_FIREWORKS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	CBrush m_brush; //Static 배경 색을 바꿀 때 사용할 브러시

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedFireworksInsert();
	afx_msg void OnBnClickedFireworksDelete();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
