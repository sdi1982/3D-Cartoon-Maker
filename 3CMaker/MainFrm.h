// MainFrm.h : CMainFrame 클래스의 인터페이스
//


#pragma once
//크기 변경을 막기 위해 사용했었지만, 지금은 사용하지 않음.
//이 프로젝트가 완료되어서도 사용되지 않는다면 삭제해야 할것임.
#include "LmUIThinSplitterWnd.h"

//프로그램이 실행된 실행경로.

class CMainFrame : public CFrameWnd
{

//멤버함수
public:
	VOID EnableAllMenu(BOOL bEnableMenu);						//모든 메뉴를 Enable, Disable하는데 사용하는 멤버 함수
	VOID EnableFileMenu( BOOL bEnableMenu );					//파일 메뉴를 Enable, Disable하는데 사용하는 멤버 함수
	VOID EnableSceneMenu(BOOL bEnableMenu);						//씬의 메뉴를 Enable, Disable하는데 사용하는 멤버 함수
	VOID EnableCharacterMenu( BOOL bEnableMenu);				//캐릭터 메뉴를 Enable, Disable하는데 사용하는 멤버 함수
	VOID EnableEditMenu(BOOL bEnableMenu);						//편집의 메뉴를 Enable, Disable하는데 사용하는 멤버 함수
	VOID EnableMovieClipMenu(BOOL bEnableMenu);					//동영상의 메뉴를 Enable, Disable하는데 사용하는 멤버 함수
	VOID EnableEditBackgroundMenu(BOOL bEnableMenu);			//편집 내에 있는 배경메뉴를 Enable, Disable하는데 사용하는 함수
	VOID EnableEditCharacterMenu(BOOL bEnableMenu);				//편집 내에 있는 캐릭터메뉴를 Enable, Disable하는데 사용하는 함수
	VOID EnableEditThingsMenu(BOOL bEnableMenu);				//편집 내에 있는 물건메뉴를 Enable, Disable하는데 사용하는 함수

	VOID EnablePopMenu(UINT nMenuID, BOOL bEnableMenu);			//상위 메뉴를 활성화 시키거나 비활성화 시킨다.
	VOID EnableSubMenu(UINT nPopMenuIndex, UINT nSubMenuIndex,  
						BOOL bEnableMenu );						//하위 메뉴를 활성화 시키거나 비활성화 시킨다. 

	VOID OnPickCharacterMenu();									//캐릭터가 선택되었을 때, 캐릭터가 할 수 있는 액션에 맞게 메뉴가 나오도록 하는 함수
	VOID OnCharacterAction();									//캐릭터 액션
	void EnableInsertTalkBoxBtn(BOOL bEnable);								//말풍선 삽입 버튼 설정

protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);


// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);


//멤버변수
protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	
public:
	CSplitterWnd m_wndSplitRow;      // 먼저 전체 2행 1열로 분열 할때 사용
	CSplitterWnd m_wndSplitUpCol;	 // 2행 1열을 분열 후 상위 2칸으로 분열 때 사용
	LmUIThinSplitterWnd m_wndSplitUpColExpand;	 // 2행 1열을 분열 후 상위 3칸으로 분열 때 사용
	//CSplitterWnd m_wndSplitUpColExpand;	 // 2행 1열을 분열 후 상위 3칸으로 분열 때 사용


public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnMakeScene();
	afx_msg void OnSceneDelete();
	afx_msg void OnThingDelete();
	afx_msg void OnThingsTurn();
	afx_msg void OnRemoveBackground();
	afx_msg void OnTurnCharacter();
	afx_msg void OnRemoveCharacter();
	afx_msg void OnSceneCut();

	afx_msg void OnSaveScreen();
	afx_msg void OnMovieCreate();

	afx_msg void OnTurnLeft();
	afx_msg void OnTurnRight();
	afx_msg void OnWalk();
	afx_msg void OnShakeHand();
	afx_msg void OnBow();
	afx_msg void OnJump();
	afx_msg void OnTurnning();
	afx_msg void OnDelAction();
	
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnSceneCopy();
	afx_msg void OnScenePaste();
	afx_msg void OnLmFileNew();
	afx_msg void OnLmFileOpen();
	afx_msg void OnLmFileSave();
	afx_msg void OnLmFileSaveAs();
	

};


