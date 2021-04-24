#pragma once
#include "afxwin.h"

// LmUICameraForm ��ȭ �����Դϴ�.
#include "LmUILineEditCtrl.h"
#include "afxcmn.h"

class LmUITimeLineView;
class LmUICameraForm : public CDialog
{
	DECLARE_DYNAMIC(LmUICameraForm)

public:
	LmUICameraForm(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~LmUICameraForm();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CAMERA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	DECLARE_MESSAGE_MAP()

//��� �Լ�
public:
	void SetCameraUIValue(FLOAT x, FLOAT y, FLOAT z, FLOAT d);
	LmUITimeLineView* GetTimeLineView();
public:
	afx_msg void OnPaint();


//��� ����
private:
	CBrush m_brush; //Static ��� ���� �ٲ� �� ����� �귯��
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
	LmUILineEditCtrl		m_ctrlCameraXVal;	// ī�޶� X ��ǥ���� �Է¹޴� ��Ʈ�� ����
	FLOAT					m_fCameraXVal;		// ī�޶� X ��ǥ��
	LmUILineEditCtrl		m_ctrlCameraYVal;	// ī�޶� Y ��ǥ���� �Է¹޴� ��Ʈ�� ����
	FLOAT					m_fCameraYVal;		// ī�޶� Y ��ǥ��
	LmUILineEditCtrl		m_ctrlCameraZVal;	// ī�޶� Z ��ǥ���� �Է¹޴� ��Ʈ�� ����
	FLOAT					m_fCameraZVal;		// ī�޶� Z ��ǥ��
	FLOAT m_fDist; // �������� �Ÿ�
	
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedDefaultCamera();
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDeltaposXValSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposYValSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposZValSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposDistanceValSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedUserViewVal();
	afx_msg void OnBnClickedCamkeyDel();
};
