#pragma once
#include "afxcmn.h"

enum enumSoundViewState {SOUNDVIEW_PLAY, SOUNDVIEW_STOP};


// LmUISoundPrePlay 대화 상자입니다.

class LmUISoundPrePlay : public CDialog
{
	DECLARE_DYNAMIC(LmUISoundPrePlay)

public:
	LmUISoundPrePlay(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~LmUISoundPrePlay();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SOUND_PRE_LISTEN };

//멤버 변수
public:
	CSliderCtrl				m_ctrlSoundSlider;
	enumSoundViewState		m_SoundViewState;
	CWinThread				*m_pThreadPlayView;

// [진우] 사운드 ID 
	CString		m_strSoundName;

//멤버 함수
public:
	void	SetSlideBar(INT); //이거 만들자!!

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedPlay();
	afx_msg void OnBnClickedCancel();
};



