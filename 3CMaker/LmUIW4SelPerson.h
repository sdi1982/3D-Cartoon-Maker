#pragma once

#include "LmUIThumbNailCtrl.h"

// LmUIW4SelPerson ��ȭ �����Դϴ�.
class LmUISceneView;

class LmUIW4SelPerson : public CDialog
{
	DECLARE_DYNAMIC(LmUIW4SelPerson)

public:
	LmUIW4SelPerson(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~LmUIW4SelPerson();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_WIZARD4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	
	//��� �Լ�
public: 
	LmUISceneView * GetSceneView();				// SceneView�� �ּҸ� �޾ƿ´�.

	//��� ����
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
