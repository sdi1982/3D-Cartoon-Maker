#pragma once
#include "afxwin.h"


// LmUITimeLineConf 대화 상자입니다.
#include "LmUILineEditCtrl.h"
class LmUITimeLineConf : public CDialog
{
	DECLARE_DYNAMIC(LmUITimeLineConf)

public:
	LmUITimeLineConf(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~LmUITimeLineConf();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TIMELINECONF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	// 프레임 길이
	LmUILineEditCtrl m_ctrlFrmLength;
	// 프레임 길이 - Data 변수
	int m_nFrameLength;
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
