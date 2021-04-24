#pragma once
#include "afxcmn.h"

// CFormMainTool �� ���Դϴ�.
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
	LmUIMainToolFormView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~LmUIMainToolFormView();

public:
	enum { IDD = IDD_FORMMAINTOOL };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

//��� �Լ�
	void InsertpDlg(CDialog * pInDlg, wstring strDlgName);  //Dialog�� ����Ѵ�.
	void ChangeCurDlg(wstring strDlgName);					//���̴� ���̾�α׸� �����Ѵ�.
	void DeleteAllMap();									//map�� �ִ� �����͸� �����.
	void InitWizard();
	LmUISceneView * GetSceneView();

//��� ����
public:
	MAPDIALOG	m_mappDlg;
	CDialog *	m_pCurDlg;
	wstring		m_wstCurDlgName;

	//Wizard �� Dialog
	LmUIW1SceneName		m_DlgW1SceneName;
	LmUIW2SelBakGnd		m_DlgW2SelBakGnd;
	LmUIW3SelObj		m_DlgW3SelObj;
	LmUIW4SelPerson		m_DlgW4SelPerson;
	
	//MainTool Dialog
	LmUIMainToolDlg		m_MainToolDlg;
	LmUIMainNoticeDlg	m_MainNoticeDlg;
	LmUITalkBoxContentDlg m_TalkBoxContentDlg;	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};