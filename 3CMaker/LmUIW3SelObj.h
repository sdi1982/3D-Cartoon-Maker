#pragma once

#include "LmUIThumbNailCtrl.h"

// LmUIW3SelObj 대화 상자입니다.
class LmUISceneView;

class LmUIW3SelObj : public CDialog
{
	DECLARE_DYNAMIC(LmUIW3SelObj)

public:
	LmUIW3SelObj(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~LmUIW3SelObj();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_WIZARD3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.


	//멤버 함수
public: 
	LmUISceneView * GetSceneView();			//SceneView의 주소를 받아온다.

	//멤버 변수
public: 
	LmUIThumbNailCtrl		m_tbObject;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedWiiiBefore();
	afx_msg void OnBnClickedWiiiNext();
	//afx_msg void OnBnClickedWiiiCancle();
	//afx_msg void OnBnClickedWiiiComplete();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
