#pragma once
#include "afxcmn.h"
#include "LmUISoundContentDlg.h"

// LmUISoundForm ��ȭ �����Դϴ�.
class LmUITimeLineView;

class LmUISoundForm : public CDialog
{
	DECLARE_DYNAMIC(LmUISoundForm)

public:
	LmUISoundForm(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~LmUISoundForm();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SOUND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()


	// ��� �Լ�
public:
	void InsertSoundContent(LmUISoundContentDlg & dlgLmUISoundContent, INT iListCount); //m_listSound�� iListCount��° Row��
																						//LmUISoundContentDlg Class�� ���� �������ش�.
	void SetSoundContent(LmUISoundContentDlg & dlgLmUISoundContent, INT iListCount);	//m_listSound�� iListCount��° Row��
																						//LmUISoundContentDlg Class�� ���� �������ش�.
	void SetSoundListSubItems(LmUISoundContentDlg &dlgLmUISoundContent, INT iListCount ); //m_listTalk�� subItems�� ���� �������ش�.

	LmUITimeLineView*	GetTimeLineView();		// Ÿ�Ӷ����� �ּҸ� �޾ƿ´�.

	//���� �ٲ� ����ϴ� �Լ���
	void InsertSoundContent( LPCWSTR pcwsSoundName, UINT uStartFrame  );
	void InitSoundList();


	// ��� ����
public:
	// ���� ����Ʈ
	CListCtrl m_listSound;


public:
	afx_msg void OnPaint();


private:
	CBrush m_brush;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	
	afx_msg void OnBnClickedSoundInsert();
	afx_msg void OnBnClickedSoundModify();
	afx_msg void OnBnClickedSoundDelete();
	afx_msg void OnBnClickedSoundPrePlay();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
