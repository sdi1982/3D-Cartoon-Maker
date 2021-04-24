#pragma once
#include "LmTimeLineManager.h"
#include "LmUIMotionThread.h"

class CHoverButton;
class CMy3CMakerDoc;

// CTimeLineView 뷰입니다.
typedef list<LmLayer> 			LayerList;
typedef LayerList::iterator		LayerListItr;

class LmUITimeLineView :public CView
{
	DECLARE_DYNCREATE(LmUITimeLineView)

protected:
	LmUITimeLineView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~LmUITimeLineView();
public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif


//멤버함수
public:
	//CMy3CMakerDoc* GetDocument() const;

	//타임라인 Caret 관련함수
	void SetCurrFrame(INT nFrame);										//Caret을 몇번째 프레임에 그릴지 지정한다.
	UINT GetCurrFrame();												//Caret을 몇번째 프레임에 있는지 리턴한다.

	//UI 관련 함수
	void OnDrawMain();
	void OnDrawLeft();
	void OnDrawTop();
	void OnExceptButtonArea(CClientDC &dc);											//버튼 영역을 제외 하기 위해
	void GetButtonRect(CRect & rect, CHoverButton & button);						//Hover Button의 크기를 얻어 준다.
	void ReFreshVertScroll();														//Vertical Scroll을 다시 갱신한다.
	INT  GoNextCaret();
	INT  GoPrevCaret();																//이전 칸으로 이동
	void ChangeStopToPlayBtn();														//Stop 버튼에서 Play버튼으로 변경
	void ChangePlaytoStopBtn();														//Play 버튼에서 Stop버튼으로 변경
	BOOL IsDrawRectInMemDC(INT nWidthMemDC, INT nHeightMemDC, CRect rectSection);	//타임라인에 Section 사각형을 그려야 할지를 결정해 주는 함수.


//멤버변수
public:

	CSize						m_sizeView;				//View의 크기
	CScrollBar					m_scrVertical;			//수직 스크롤바
	CScrollBar					m_scrHorizontal;		//수평 스크롤바
	INT							m_nScrWidth;			//스크롤바 넓이
	BOOL						m_isLoadComplete;		//윈도우가 완전히 로딩되었는가? OnSize가 윈도우가 먼저 뜨기전에 한번 호출되기 때문
	
	INT							m_nRowSize;				//레이어 세로 길이
	INT							m_nLayerColSize;		//레이어 이름 칸의 가로 크기
	INT							m_nFrmColSize;			//frame 1칸의 가로 길이
	INT							m_nCellSpacing;			//사각형 안쪽 여백
	INT							m_nLeftSpacing;			//글자를 쓰기 위한 사각형 왼쪽 여백(CellSpaing 바로 다음 부터 시작)
	INT							m_nLayerNameLength;		//Layer 이름의 제한 길이

	CPoint						m_ptScrPos;				//스크롤 위치
	CPoint						m_ptScrMax;				//스크롤 최대 크기

	CHoverButton				m_hBtnPlay;				//Play버튼 / Stop 버튼과 Toggle 형식으로 사용
	CHoverButton				m_hBtnStop;				//Stop버튼 / Play 버튼과 Toggle 형식으로 사용
	CHoverButton				m_hBtnNextFrm;			//다음 프레임으로 가기 버튼
	CHoverButton				m_hBtnPrevFrm;			//이전 프레임으로 가기 버튼
	CHoverButton				m_hBtnFirstFrm;			//첫번째 프레임으로 가기 버튼
	CHoverButton				m_hBtnLastFrm;			//마지막 프레임으로 가기 버튼
	CHoverButton				m_hBtnConfig;			//설정 버튼

	UINT						m_nPrevSectionPos;		//카메라 Section의 보간 된 부분을 그리기 위한 point임.

	BOOL						m_isPlaying;			//플레이 중인가? Yes면 TRUE, NO면 FALSE

	LmUIMotionThread*			m_pMotionThread;		//타임라인 및 전체 UI를 관할하는 Thread임.
								
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBtnClickedPlay();
	afx_msg void OnBtnClickedStop();
	afx_msg void OnBtnClickedNext();
	afx_msg void OnBtnClickedPrev();
	afx_msg void OnBtnClickedFirst();
	afx_msg void OnBtnClickedLast();
	afx_msg void OnBtnClickedConfig();

	void ReSizeFrameLength( INT nListSize, INT nFrameLength );
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};


