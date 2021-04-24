#pragma once
#include "afxwin.h"


// LmUIW1SceneName ��ȭ �����Դϴ�.

#include "LmUILineEditCtrl.h"
class LmUISceneView;
class LmUIW1SceneName : public CDialog
{
	DECLARE_DYNAMIC(LmUIW1SceneName)

public:
	LmUIW1SceneName(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~LmUIW1SceneName();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_WIZARD1 };

//��� �Լ�
	LmUISceneView *		GetSceneView();						//LmUISceneView�� �ּҸ� �޾ƿ´�.

//��� ����
public:
	LmUILineEditCtrl	m_ctrlSceneName;				// Scene �̸��� �Է¹޴� ��Ʈ�� ����
	CString				m_strSceneName;					// Scene �̸�

	LmUILineEditCtrl	m_ctrlFrameLength;				// ������ ���̸� �Է¹޴� ��Ʈ��
	UINT				m_nFrameLength;					// Scene�� ������ ����
	
	UINT				m_nInsertedIndex;				// ��� �߰��� ThumbNail�� Index��. 
	CString				m_nInsertedName;				// ��� �߰��� ThumbNail�� �̸���. 

	INT					m_nTerrainTex;					// ������ �����ϱ� ���� ����
	INT					m_nTerrainTexUI;				// ������ �����ϱ� UI�� ���� ����
	CButton				m_ctrlGrass;					// �ʿ� ���� ��ư�� ��Ʈ�� ��� ����
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
