#pragma once
#include "afxcmn.h"

#include "LmUICameraForm.h"
#include "LmUISoundForm.h"
#include "LmUITalkBoxForm.h"
#include "LmUIDialogTabCtrl.h"
#include "LmUISnowForm.h"
#include "LmUIFireWorksForm.h"

// LmUIDlgMainTool 대화 상자입니다.

class LmUIMainToolDlg : public CDialog
{
	DECLARE_DYNAMIC(LmUIMainToolDlg)

public:
	LmUIMainToolDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~LmUIMainToolDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MAINTOOL };

//멤버함수
public:
	void InitToolList();			//MainTool에 있는 리스트들을 초기화 시켜버린다.
	
	VOID InsertSound( LPCWSTR pcwsSoundName, UINT uStartFrame  );

	VOID InsertSnow( UINT uFrameStart, UINT uFrameEnd, LPCWSTR pcwsSnowName );

	VOID InsertTalkBox( LPCWSTR pcwsCharacterName, UINT uFrameStart, UINT uFrameEnd, LPCWSTR pcwsTalkContent, LPCWSTR pcwsTalkBoxStyle  );


	// 말풍선, 사운드, 카메라를 입력하기 위한 탭컨트롤
	LmUIDialogTabCtrl	m_tabCtrlTool;  //다이얼 로그 탭 컨트롤

	// 아래의 3개의 Form이 위의 다이얼로그 탭 컨트롤에 등록된다.!
	LmUISoundForm		m_LmUISound;				//사운드 탭
	LmUITalkBoxForm		m_LmUITalkBox;				//말풍선 탭
	LmUICameraForm		m_Camera;					//카메라 탭
	LmUISnowForm		m_LmUISnow;					//눈내리기 효과 탭
	LmUIFireWorksForm	m_LmUIFireWorks;			//불꽃놀이 효과 탭

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
