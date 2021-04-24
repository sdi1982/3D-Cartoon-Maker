#pragma once

#include "LmUIThumbNailCtrl.h"

// LmUIW3SelObj ��ȭ �����Դϴ�.
class LmUISceneView;

class LmUIW3SelObj : public CDialog
{
	DECLARE_DYNAMIC(LmUIW3SelObj)

public:
	LmUIW3SelObj(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~LmUIW3SelObj();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_WIZARD3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.


	//��� �Լ�
public: 
	LmUISceneView * GetSceneView();			//SceneView�� �ּҸ� �޾ƿ´�.

	//��� ����
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
