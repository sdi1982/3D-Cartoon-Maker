#pragma once
#include "afxwin.h"


// LmUIW1SceneName 대화 상자입니다.

#include "LmUILineEditCtrl.h"
class LmUISceneView;
class LmUIW1SceneName : public CDialog
{
	DECLARE_DYNAMIC(LmUIW1SceneName)

public:
	LmUIW1SceneName(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~LmUIW1SceneName();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_WIZARD1 };

//멥버 함수
	LmUISceneView *		GetSceneView();						//LmUISceneView의 주소를 받아온다.

//멤버 변수
public:
	LmUILineEditCtrl	m_ctrlSceneName;				// Scene 이름을 입력받는 컨트롤 변수
	CString				m_strSceneName;					// Scene 이름

	LmUILineEditCtrl	m_ctrlFrameLength;				// 프레임 길이를 입력받는 컨트롤
	UINT				m_nFrameLength;					// Scene의 프레임 길이
	
	UINT				m_nInsertedIndex;				// 방금 추가된 ThumbNail의 Index임. 
	CString				m_nInsertedName;				// 방금 추가된 ThumbNail의 이름임. 

	INT					m_nTerrainTex;					// 지형을 전달하기 위한 변수
	INT					m_nTerrainTexUI;				// 지형을 전달하기 UI용 위한 변수
	CButton				m_ctrlGrass;					// 초원 라디오 버튼의 컨트롤 멤버 변수
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedWicancle();
	afx_msg void OnBnClickedWinext();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedGrass();
	afx_msg void OnBnClickedDesert();
	afx_msg void OnBnClickedWinter();
	virtual BOOL OnInitDialog();
};
