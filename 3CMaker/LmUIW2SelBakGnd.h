#pragma once
#include "LmUIThumbNailCtrl.h"

// LmUIW2SelBakGnd ��ȭ �����Դϴ�.
class LmUISceneView;
class LmUIW2SelBakGnd : public CDialog
{
	DECLARE_DYNAMIC(LmUIW2SelBakGnd)

public:
	LmUIW2SelBakGnd(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~LmUIW2SelBakGnd();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_WIZARD2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

	//��� �Լ�
public: 
	LmUISceneView * GetSceneView();			// SceneView�� �ּҸ� �޾ƿ´�.

	//��� ����
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
