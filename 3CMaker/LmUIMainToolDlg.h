#pragma once
#include "afxcmn.h"

#include "LmUICameraForm.h"
#include "LmUISoundForm.h"
#include "LmUITalkBoxForm.h"
#include "LmUIDialogTabCtrl.h"
#include "LmUISnowForm.h"
#include "LmUIFireWorksForm.h"

// LmUIDlgMainTool ��ȭ �����Դϴ�.

class LmUIMainToolDlg : public CDialog
{
	DECLARE_DYNAMIC(LmUIMainToolDlg)

public:
	LmUIMainToolDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~LmUIMainToolDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MAINTOOL };

//����Լ�
public:
	void InitToolList();			//MainTool�� �ִ� ����Ʈ���� �ʱ�ȭ ���ѹ�����.
	
	VOID InsertSound( LPCWSTR pcwsSoundName, UINT uStartFrame  );

	VOID InsertSnow( UINT uFrameStart, UINT uFrameEnd, LPCWSTR pcwsSnowName );

	VOID InsertTalkBox( LPCWSTR pcwsCharacterName, UINT uFrameStart, UINT uFrameEnd, LPCWSTR pcwsTalkContent, LPCWSTR pcwsTalkBoxStyle  );


	// ��ǳ��, ����, ī�޶� �Է��ϱ� ���� ����Ʈ��
	LmUIDialogTabCtrl	m_tabCtrlTool;  //���̾� �α� �� ��Ʈ��

	// �Ʒ��� 3���� Form�� ���� ���̾�α� �� ��Ʈ�ѿ� ��ϵȴ�.!
	LmUISoundForm		m_LmUISound;				//���� ��
	LmUITalkBoxForm		m_LmUITalkBox;				//��ǳ�� ��
	LmUICameraForm		m_Camera;					//ī�޶� ��
	LmUISnowForm		m_LmUISnow;					//�������� ȿ�� ��
	LmUIFireWorksForm	m_LmUIFireWorks;			//�Ҳɳ��� ȿ�� ��

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
