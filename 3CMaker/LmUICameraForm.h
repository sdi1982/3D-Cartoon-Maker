#pragma once
#include "afxwin.h"

// LmUICameraForm 대화 상자입니다.
#include "LmUILineEditCtrl.h"
#include "afxcmn.h"

class LmUITimeLineView;
class LmUICameraForm : public CDialog
{
	DECLARE_DYNAMIC(LmUICameraForm)

public:
	LmUICameraForm(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~LmUICameraForm();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CAMERA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()

//멤버 함수
public:
	void SetCameraUIValue(FLOAT x, FLOAT y, FLOAT z, FLOAT d);
	LmUITimeLineView* GetTimeLineView();
public:
	afx_msg void OnPaint();


//멤버 변수
private:
	CBrush m_brush; //Static 배경 색을 바꿀 때 사용할 브러시
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
	LmUILineEditCtrl		m_ctrlCameraXVal;	// 카메라 X 좌표값을 입력받는 컨트롤 변수
	FLOAT					m_fCameraXVal;		// 카메라 X 좌표값
	LmUILineEditCtrl		m_ctrlCameraYVal;	// 카메라 Y 좌표값을 입력받는 컨트롤 변수
	FLOAT					m_fCameraYVal;		// 카메라 Y 좌표값
	LmUILineEditCtrl		m_ctrlCameraZVal;	// 카메라 Z 좌표값을 입력받는 컨트롤 변수
	FLOAT					m_fCameraZVal;		// 카메라 Z 좌표값
	FLOAT m_fDist; // 중점과의 거리
	
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
