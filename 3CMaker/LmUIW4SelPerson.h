#pragma once

#include "LmUIThumbNailCtrl.h"

// LmUIW4SelPerson 대화 상자입니다.
class LmUISceneView;

class LmUIW4SelPerson : public CDialog
{
	DECLARE_DYNAMIC(LmUIW4SelPerson)

public:
	LmUIW4SelPerson(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~LmUIW4SelPerson();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_WIZARD4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	
	//멤버 함수
public: 
	LmUISceneView * GetSceneView();				// SceneView의 주소를 받아온다.

	//멤버 변수
public: 
	LmUIThumbNailCtrl		m_tbPerson;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedWivCancle();
	afx_msg void OnBnClickedWivBefore();
	afx_msg void OnBnClickedWivComplete();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
