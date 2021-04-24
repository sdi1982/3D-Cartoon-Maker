#pragma once
#include "LmUIThumbNailCtrl.h"

// LmUIW2SelBakGnd 대화 상자입니다.
class LmUISceneView;
class LmUIW2SelBakGnd : public CDialog
{
	DECLARE_DYNAMIC(LmUIW2SelBakGnd)

public:
	LmUIW2SelBakGnd(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~LmUIW2SelBakGnd();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_WIZARD2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	//멤버 함수
public: 
	LmUISceneView * GetSceneView();			// SceneView의 주소를 받아온다.

	//멤버 변수
public: 
	LmUIThumbNailCtrl		m_tbBackGround;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedWiiBefore();
	afx_msg void OnBnClickedWiiNext();
	//afx_msg void OnBnClickedWiiCancle();
	//afx_msg void OnBnClickedWiiComplete();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
