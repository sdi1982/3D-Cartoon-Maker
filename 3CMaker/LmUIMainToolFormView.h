#pragma once
#include "afxcmn.h"

// CFormMainTool 폼 뷰입니다.
#include "LmUIMainToolDlg.h"
#include "LmUIW1SceneName.h"
#include "LmUIW2SelBakGnd.h"
#include "LmUIW3SelObj.h"
#include "LmUIW4SelPerson.h"
#include "LmUIMainNoticeDlg.h"
#include "LmUITalkBoxContentDlg.h"

class LmUISceneView;
typedef map<wstring, CDialog*> MAPDIALOG;
class LmUIMainToolFormView : public CFormView
{
	DECLARE_DYNCREATE(LmUIMainToolFormView)

protected:
	LmUIMainToolFormView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~LmUIMainToolFormView();

public:
	enum { IDD = IDD_FORMMAINTOOL };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

//멤버 함수
	void InsertpDlg(CDialog * pInDlg, wstring strDlgName);  //Dialog를 등록한다.
	void ChangeCurDlg(wstring strDlgName);					//보이는 다이얼로그를 지정한다.
	void DeleteAllMap();									//map에 있는 데이터를 지운다.
	void InitWizard();
	LmUISceneView * GetSceneView();

//멤버 변수
public:
	MAPDIALOG	m_mappDlg;
	CDialog *	m_pCurDlg;
	wstring		m_wstCurDlgName;

	//Wizard 형 Dialog
	LmUIW1SceneName		m_DlgW1SceneName;
	LmUIW2SelBakGnd		m_DlgW2SelBakGnd;
	LmUIW3SelObj		m_DlgW3SelObj;
	LmUIW4SelPerson		m_DlgW4SelPerson;
	
	//MainTool Dialog
	LmUIMainToolDlg		m_MainToolDlg;
	LmUIMainNoticeDlg	m_MainNoticeDlg;
	LmUITalkBoxContentDlg m_TalkBoxContentDlg;	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};