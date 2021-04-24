// MainFrm.h : CMainFrame Ŭ������ �������̽�
//


#pragma once
//ũ�� ������ ���� ���� ����߾�����, ������ ������� ����.
//�� ������Ʈ�� �Ϸ�Ǿ�� ������ �ʴ´ٸ� �����ؾ� �Ұ���.
#include "LmUIThinSplitterWnd.h"

//���α׷��� ����� ������.

class CMainFrame : public CFrameWnd
{

//����Լ�
public:
	VOID EnableAllMenu(BOOL bEnableMenu);						//��� �޴��� Enable, Disable�ϴµ� ����ϴ� ��� �Լ�
	VOID EnableFileMenu( BOOL bEnableMenu );					//���� �޴��� Enable, Disable�ϴµ� ����ϴ� ��� �Լ�
	VOID EnableSceneMenu(BOOL bEnableMenu);						//���� �޴��� Enable, Disable�ϴµ� ����ϴ� ��� �Լ�
	VOID EnableCharacterMenu( BOOL bEnableMenu);				//ĳ���� �޴��� Enable, Disable�ϴµ� ����ϴ� ��� �Լ�
	VOID EnableEditMenu(BOOL bEnableMenu);						//������ �޴��� Enable, Disable�ϴµ� ����ϴ� ��� �Լ�
	VOID EnableMovieClipMenu(BOOL bEnableMenu);					//�������� �޴��� Enable, Disable�ϴµ� ����ϴ� ��� �Լ�
	VOID EnableEditBackgroundMenu(BOOL bEnableMenu);			//���� ���� �ִ� ���޴��� Enable, Disable�ϴµ� ����ϴ� �Լ�
	VOID EnableEditCharacterMenu(BOOL bEnableMenu);				//���� ���� �ִ� ĳ���͸޴��� Enable, Disable�ϴµ� ����ϴ� �Լ�
	VOID EnableEditThingsMenu(BOOL bEnableMenu);				//���� ���� �ִ� ���Ǹ޴��� Enable, Disable�ϴµ� ����ϴ� �Լ�

	VOID EnablePopMenu(UINT nMenuID, BOOL bEnableMenu);			//���� �޴��� Ȱ��ȭ ��Ű�ų� ��Ȱ��ȭ ��Ų��.
	VOID EnableSubMenu(UINT nPopMenuIndex, UINT nSubMenuIndex,  
						BOOL bEnableMenu );						//���� �޴��� Ȱ��ȭ ��Ű�ų� ��Ȱ��ȭ ��Ų��. 

	VOID OnPickCharacterMenu();									//ĳ���Ͱ� ���õǾ��� ��, ĳ���Ͱ� �� �� �ִ� �׼ǿ� �°� �޴��� �������� �ϴ� �Լ�
	VOID OnCharacterAction();									//ĳ���� �׼�
	void EnableInsertTalkBoxBtn(BOOL bEnable);								//��ǳ�� ���� ��ư ����

protected: // serialization������ ��������ϴ�.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);


// �����Դϴ�.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// ������ �޽��� �� �Լ�
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);


//�������
protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	
public:
	CSplitterWnd m_wndSplitRow;      // ���� ��ü 2�� 1���� �п� �Ҷ� ���
	CSplitterWnd m_wndSplitUpCol;	 // 2�� 1���� �п� �� ���� 2ĭ���� �п� �� ���
	LmUIThinSplitterWnd m_wndSplitUpColExpand;	 // 2�� 1���� �п� �� ���� 3ĭ���� �п� �� ���
	//CSplitterWnd m_wndSplitUpColExpand;	 // 2�� 1���� �п� �� ���� 3ĭ���� �п� �� ���


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


