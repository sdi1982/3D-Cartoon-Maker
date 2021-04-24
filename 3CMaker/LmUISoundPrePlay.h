#pragma once
#include "afxcmn.h"

enum enumSoundViewState {SOUNDVIEW_PLAY, SOUNDVIEW_STOP};


// LmUISoundPrePlay ��ȭ �����Դϴ�.

class LmUISoundPrePlay : public CDialog
{
	DECLARE_DYNAMIC(LmUISoundPrePlay)

public:
	LmUISoundPrePlay(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~LmUISoundPrePlay();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SOUND_PRE_LISTEN };

//��� ����
public:
	CSliderCtrl				m_ctrlSoundSlider;
	enumSoundViewState		m_SoundViewState;
	CWinThread				*m_pThreadPlayView;

// [����] ���� ID 
	CString		m_strSoundName;

//��� �Լ�
public:
	void	SetSlideBar(INT); //�̰� ������!!

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedPlay();
	afx_msg void OnBnClickedCancel();
};



