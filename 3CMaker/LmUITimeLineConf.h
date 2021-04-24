#pragma once
#include "afxwin.h"


// LmUITimeLineConf ��ȭ �����Դϴ�.
#include "LmUILineEditCtrl.h"
class LmUITimeLineConf : public CDialog
{
	DECLARE_DYNAMIC(LmUITimeLineConf)

public:
	LmUITimeLineConf(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~LmUITimeLineConf();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_TIMELINECONF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	// ������ ����
	LmUILineEditCtrl m_ctrlFrmLength;
	// ������ ���� - Data ����
	int m_nFrameLength;
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
