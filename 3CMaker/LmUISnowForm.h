#pragma once
#include "afxcmn.h"
#include "LmUITimeLineView.h"
#include "LmUISnowContentDlg.h"

// LmUISnowForm ��ȭ �����Դϴ�.
class LmUITimeLineView;
class LmUISnowForm : public CDialog
{
	DECLARE_DYNAMIC(LmUISnowForm)

public:
	LmUISnowForm(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~LmUISnowForm();

//����Լ�
public: 
	LmUITimeLineView* GetTimeLineView();
	void				SetSnowContent(	LmUISnowContentDlg &dlgSnowContent, 
										INT iListCount );	//m_listSnow�� iListCount��° Row��
															//SnowContentDlg Class�� ���� �������ش�.
	void				InsertSnowContent(   LmUISnowContentDlg &dlgSnowContent, 
										INT iListCount );	//m_listSnow�� iListCount��° Row��
															//SnowContentDlg Class�� ���� �������ش�.
	void				SetSnowListSubItems( LmUISnowContentDlg &dlgSnowContent, 
											INT iListCount ); //m_listSnow�� subItems�� ���� �������ش�.
															  //SnowContentDlg Class�� ���� �������ش�.
														
	//���� �ٲ� ����ϴ� �Լ���
	void InsertSnowContent( UINT uFrameStart, UINT uFrameEnd, LPCWSTR pcwsSnowName );
	void InitSnowList();

//�������
public: 
	CListCtrl m_listSnow;


// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SNOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedSnowInsert();
	afx_msg void OnBnClickedSnowModify();
	afx_msg void OnBnClickedSnowDelete();
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

};
