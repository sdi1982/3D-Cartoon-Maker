// TimeLineView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUITimeLineView.h"
#include "LmUITimeLineConf.h"
#include "MainFrm.h"

#include "3CMakerDoc.h"

#define SB_CARETDOWN	10 
#define SB_CARETUP		11

// CTimeLineView

IMPLEMENT_DYNCREATE(LmUITimeLineView, CView)

//TheCartoonMaker로 부터 Layer List를 const로 받아온다.
LmUITimeLineView::LmUITimeLineView()
{
	m_isLoadComplete = FALSE;

	m_nScrWidth = 0;
	m_nRowSize = 25;		//25px의 줄크기
	m_nLayerColSize = 200;	//250px Layer 칸 세로크기
	m_nFrmColSize = 10;		//Frame 1칸에 10px
	m_nCellSpacing = 3;		//사각형 안쪽 여백
	m_nLeftSpacing = 10;	//글자를 쓰기 위한 사각형 왼쪽 여백(CellSpaing 바로 다음 부터 시작)
	m_nLayerNameLength = 20;

	m_ptScrPos.SetPoint(0,0); //스크롤 위치 초기화
	m_ptScrMax.SetPoint(0,0); //스크롤 최대값

	m_isPlaying = FALSE; //지금 Play 중인가?

	m_pMotionThread = NULL;
	m_pMotionThread = new LmUIMotionThread(this);
	m_pMotionThread->CreateThread();

/*
	nFrameLength = 1000;	//타임라인의 프레임 수
	m_nFrmPerSec = 20;		//1초당 몇 10개의 프레임이 사용된다. 
	nCurCaret = 1;		//1 부터 시작!
*/

}

LmUITimeLineView::~LmUITimeLineView()
{
	m_pMotionThread->PostThreadMessage(WM_QUIT, 0, 0);
	m_pMotionThread = NULL;
}

BEGIN_MESSAGE_MAP(LmUITimeLineView, CView)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_COMMAND(HOVERBUTTON_PLAY		,OnBtnClickedPlay)
	ON_COMMAND(HOVERBUTTON_STOP		,OnBtnClickedStop)
	ON_COMMAND(HOVERBUTTON_NEXT		,OnBtnClickedNext)
	ON_COMMAND(HOVERBUTTON_PREV		,OnBtnClickedPrev)
	ON_COMMAND(HOVERBUTTON_FIRST	,OnBtnClickedFirst)
	ON_COMMAND(HOVERBUTTON_LAST		,OnBtnClickedLast)
	ON_COMMAND(HOVERBUTTON_CONFIG	,OnBtnClickedConfig)
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CTimeLineView 그리기입니다.
//INT a=0;
void LmUITimeLineView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();

	if(m_sizeView.cy > m_ptScrMax.y)
	{
		CClientDC dc(this);

		//오른쪽 상하 스크롤 바 영역 제외
		dc.ExcludeClipRect( m_sizeView.cx - m_nScrWidth ,0,
			m_sizeView.cx , m_sizeView.cy);
		//하단 스크롤 바 영역 제외
		dc.ExcludeClipRect(0 ,m_sizeView.cy - m_nScrWidth + 1,
			m_sizeView.cx , m_sizeView.cy);
		
		INT nListSize = TheCartoonMaker->GetListLayerSize();

		CRect rectWhiteArea(0, (nListSize+1) * m_nRowSize, m_sizeView.cx, m_sizeView.cy);
		dc.SelectStockObject(NULL_PEN);
		dc.SelectStockObject(WHITE_BRUSH);
		dc.Rectangle(&rectWhiteArea);
	}

	//왼쪽 레이어 이름 부분 출력
	OnDrawLeft();
	
	//실제 레이어가 가진 Action들의 집합
	OnDrawMain();
	
	//위쪽 프레임 눈금자 영역
	OnDrawTop();
	

	//SetCurrFrame(++a);
	//m_hBtnPlay.Invalidate();
}


// CTimeLineView 진단입니다.

#ifdef _DEBUG
void LmUITimeLineView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void LmUITimeLineView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CTimeLineView 메시지 처리기입니다.

void LmUITimeLineView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 이 뷰의 전체 크기를 계산합니다.
	CRect rect;
	GetClientRect(&rect);

	//뷰 크기를 멤버변수로 백업해 두고, 타임라인을 그릴때 사용한다.
	m_sizeView .cx = rect.Width();
	m_sizeView .cy = rect.Height();
	
	//스크롤 바를 생성한다.
	CRect rectVScroll(0,0,0,0);
	CRect rectHScroll(0,0,0,0);
	m_scrVertical.Create(WS_CHILD | WS_VISIBLE | SBS_VERT, rectVScroll , this, SCROLLBAR_VERTICAL);
	m_scrHorizontal.Create(WS_CHILD | WS_VISIBLE | SBS_HORZ, rectHScroll, this, SCROLLBAR_HORIZONTAL);
	//SetScrollSizes(MM_TEXT, m_ViewSize);
	
	m_nScrWidth = GetSystemMetrics(SM_CYHSCROLL);
	m_isLoadComplete = TRUE;

	//버튼을 생성한다. 배치는 OnSize에서 한다.
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | WS_TABSTOP;

	CRect rectInitBtn(0,0,10,10);
	m_hBtnPlay.Create(L"PLAY",dwStyle, rectInitBtn, this, HOVERBUTTON_PLAY);
	m_hBtnStop.Create(L"STOP",dwStyle, rectInitBtn, this, HOVERBUTTON_STOP);
	m_hBtnStop.ShowWindow(SW_HIDE); //stop는 toggle임. 초기에는 플레이 버튼임.

	m_hBtnNextFrm.Create(L"NEXT",dwStyle, rectInitBtn, this, HOVERBUTTON_NEXT);	
	m_hBtnPrevFrm.Create(L"PREV",dwStyle, rectInitBtn, this, HOVERBUTTON_PREV);
	m_hBtnFirstFrm.Create(L"FIRST",dwStyle, rectInitBtn, this, HOVERBUTTON_FIRST);
	m_hBtnLastFrm.Create(L"LAST",dwStyle, rectInitBtn, this, HOVERBUTTON_LAST);
	m_hBtnConfig.Create(L"CONFIG",dwStyle, rectInitBtn, this, HOVERBUTTON_CONFIG);


	//버튼에 그림 등록
	m_hBtnPlay.LoadBitmap(IDB_BTNPLAY);
	m_hBtnStop.LoadBitmap(IDB_BTNSTOP);
	m_hBtnNextFrm.LoadBitmap(IDB_BTNNEXT);
	m_hBtnPrevFrm.LoadBitmap(IDB_BTNPREV);
	m_hBtnFirstFrm.LoadBitmap(IDB_BTNFIRST);
	m_hBtnLastFrm.LoadBitmap(IDB_BTNLAST);
	m_hBtnConfig.LoadBitmap(IDB_BTNCONFIG);

	//우선에 여기에 둔다.[InitTimeLIne] [테스트용]
//	TheCartoonMaker->InitTimeLIne();

	//LayerListItr itr;
	//itr = m_listLayer.begin();
	//
	//for(int i =0 ; i< 20; i++)
	//{
	//	LmSection secData;
	//	CString strTest = L"asdf";
	//	wcsncpy_s(secData.m_strActionName, strTest.GetBuffer(0), wcslen(strTest));
	//	secData.m_nStartFrm = 2*i+20 ;
	//	secData.m_nEndFrm = 3*i+30;
	//	itr->m_mapSection[secData.m_nStartFrm]=secData;
	//	itr++;
	//}

	

	SetCurrFrame(1);

	///////////////////////////////////////////////////////////////////////////////////////////
}

void LmUITimeLineView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	INT nFrameLength = TheCartoonMaker->GetFrameLength();
	//OnInitialUpdate 가 모두 실행되었는가? 실행되지 않으면 리턴한다. 
	if(!m_isLoadComplete)
		return;

	//뷰 크기를 멤버변수로 백업해 두고, 타임라인을 그릴때 사용한다.
	m_sizeView.cx = cx;
	m_sizeView.cy = cy;

	INT nHorzXMetrix = m_nLayerColSize;						// HSCROLL X 좌표
	INT nHorzYMetrix = cy - m_nScrWidth;					// HSCROLL Y 좌표	
	INT nHorzWidth   = cx - nHorzXMetrix -m_nScrWidth;		// HSCROLL Length
	INT nHorzHeight  = m_nScrWidth;							// HSCROLL Width

	INT nVertXMetrix = cx - m_nScrWidth;					// VSCROLL X 좌표
	INT nVertYMetrix = 0;									// VSCROLL Y 좌표	
	INT nVertWidth   = m_nScrWidth;							// VSCROLL Width
	INT nVertHeight  = cy - m_nScrWidth;					// VSCROLL Length

	INT nMinWidth	 = nVertXMetrix - m_nLayerColSize - m_nScrWidth;    // 타임라인 최소 Width 크기
																		// 이것이 너무 작으면 화면에 출력이 깨짐

	//스크롤바 위치 자동 조절
	m_scrHorizontal.MoveWindow(nHorzXMetrix, nHorzYMetrix, nHorzWidth, nHorzHeight,TRUE);
	m_scrVertical.MoveWindow(nVertXMetrix, nVertYMetrix, nVertWidth, nVertHeight, TRUE);

	//CWnd::GetClientRect --> ClientToScreen을 이용하면 위치와 크기를 얻을 수 
	//있으니 DC에서 그리기 영역을 제외 시켜버리자!!
	//버튼 위치 자동 조절
	m_hBtnPlay.MoveWindow(m_nLayerColSize/2 -8, nHorzYMetrix+2,17, 17,TRUE);
	m_hBtnStop.MoveWindow(m_nLayerColSize/2 -8, nHorzYMetrix+2,17, 17,TRUE);
	
	m_hBtnPrevFrm.MoveWindow(m_nLayerColSize/2 -26, nHorzYMetrix+2,17, 17,TRUE);
	m_hBtnFirstFrm.MoveWindow(m_nLayerColSize/2 -48, nHorzYMetrix+2,17, 17,TRUE);
	m_hBtnNextFrm.MoveWindow(m_nLayerColSize/2 +9, nHorzYMetrix+2,17, 17,TRUE);
	m_hBtnLastFrm.MoveWindow(m_nLayerColSize/2 +31, nHorzYMetrix+2,17, 17,TRUE);
	m_hBtnConfig.MoveWindow(m_nLayerColSize-27, 1, 22, 22, TRUE);	

	INT nListSize = TheCartoonMaker->GetListLayerSize();

	m_ptScrMax.y = nListSize * m_nRowSize;
	m_ptScrMax.x = ( m_nFrmColSize * nFrameLength );


	// 화면보다 작을 경우 (화면 크기- m_nLayerColSize) 만큼 할당한다.
	if(m_ptScrMax.x < nMinWidth)
		m_ptScrMax.x = nMinWidth;

	if(m_ptScrMax.y <0)
		m_ptScrMax.y = 0;

	SCROLLINFO vert, horz;

	vert.cbSize = sizeof(SCROLLINFO);
	vert.nMin = 0;
	vert.nMax = m_ptScrMax.y;
	vert.nPos = 0;
	vert.fMask = SIF_ALL;
	vert.nPage = m_sizeView.cy-m_nScrWidth -m_nRowSize;

	m_scrVertical.SetScrollInfo(&vert, TRUE);
	
	horz.cbSize = sizeof(SCROLLINFO);
	horz.nMin = 0;
	horz.nMax = m_ptScrMax.x;
	horz.nPos = 0;
	horz.fMask = SIF_ALL;
	horz.nPage = m_sizeView.cx-m_nScrWidth - m_nLayerColSize;

	m_scrHorizontal.SetScrollInfo(&horz, TRUE);

}

void LmUITimeLineView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	long yIncrease=0;

	switch(nSBCode)
	{
	case SB_LINEUP:
		yIncrease = -5;
		break;
	case SB_LINEDOWN:
		yIncrease = 5;
		break;
	case SB_PAGEUP:
		yIncrease = -20;
		break;
	case SB_PAGEDOWN:
		yIncrease = 20;
		break;
	case SB_THUMBTRACK:
		SCROLLINFO scrHinfo;
		scrHinfo.cbSize = sizeof(SCROLLINFO);
		scrHinfo.fMask = SIF_TRACKPOS;
		m_scrVertical.GetScrollInfo(&scrHinfo);
		yIncrease = scrHinfo.nTrackPos - m_ptScrPos.y;
		break;
	}

	//새로운 위치는 최소한 0 이상 
	if(m_ptScrPos.y + yIncrease < 0)
		yIncrease = -m_ptScrPos.y; //점점 Scroll Thumb가 위쪽으로 가까워 짐.

	CRect rectView;
	GetClientRect(&rectView);

	// 실제 스크롤 Thumb 크기 지정과 스크롤 크기 지정은 달라야 함.
	// 스크롤 크기 지정이 Thumb 크기 지정 보다 스크롤 화면의 클라이언트 높이 만큼의 작다.
	INT ynMax = m_ptScrMax.y- m_sizeView.cy + m_nScrWidth + m_nRowSize;

	if( (m_ptScrPos.y + yIncrease ) > ynMax)
		yIncrease = ynMax - m_ptScrPos.y;

	//새로운 위치 계산
	m_ptScrPos.y +=  yIncrease;

	CRect rectScroll;
	GetClientRect(&rectScroll);
	rectScroll.left = 0;
	rectScroll.right -= m_nScrWidth;
	rectScroll.top += m_nRowSize;
	rectScroll.bottom -= m_nScrWidth;

	//필요한 영역만 스크롤 한다.
	ScrollWindow(0,-yIncrease, rectScroll, rectScroll);
	m_scrVertical.SetScrollPos( m_ptScrPos.y, TRUE);

//	TRACE(L"yIncrease : %d \t\t ynMax : %d \t\t m_yPos : %d \r\n", yIncrease, m_ptScrMax.y, m_ptScrPos.y);

	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void LmUITimeLineView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	// 데브피아에서 스크롤 처리 할것.. 16비트 이상 스크롤 값을 받아와야함!!;
	long xIncrease=0;

	switch(nSBCode)
	{
	case SB_LINEUP:
		xIncrease = -5;
		break;
	case SB_LINEDOWN:
		xIncrease = 5;
		break;
	case SB_PAGEUP:
		xIncrease = -20;
		break;
	case SB_PAGEDOWN:
		xIncrease = 20;
		break;
	case SB_CARETDOWN:
		xIncrease = m_nFrmColSize;
		break;
	case SB_CARETUP:
		xIncrease = -m_nFrmColSize;
		break;
	case SB_THUMBTRACK:
		SCROLLINFO scrHinfo;
		scrHinfo.cbSize = sizeof(SCROLLINFO);
		scrHinfo.fMask = SIF_TRACKPOS;
		m_scrHorizontal.GetScrollInfo(&scrHinfo);
		xIncrease = scrHinfo.nTrackPos - m_ptScrPos.x;
		break;
	}

	//새로운 위치는 최소한 0 이상 
	if(m_ptScrPos.x + xIncrease < 0)
		xIncrease = -m_ptScrPos.x; // 0에 가깝게 근접 시킨다.

	// 실제 스크롤 Thumb 크기 지정과 스크롤 크기 지정은 달라야 함.
	// 스크롤 크기 지정이 Thumb 크기 지정 보다 스크롤 화면의 클라이언트 넓이 만큼의 작다.
	INT xnMax = m_ptScrMax.x- m_sizeView.cx + m_nScrWidth + m_nLayerColSize;

	if( (m_ptScrPos.x + xIncrease ) > xnMax)
		xIncrease = xnMax - m_ptScrPos.x;

	//새로운 위치 계산
	m_ptScrPos.x +=  xIncrease;
    
	CRect rectScroll;
	GetClientRect(&rectScroll);
	rectScroll.left += m_nLayerColSize;
	rectScroll.right -= m_nScrWidth;
	rectScroll.top = 0 ;
	rectScroll.bottom -= m_nScrWidth;

	ScrollWindow(-xIncrease,0, rectScroll, rectScroll);
	m_scrHorizontal.SetScrollPos( m_ptScrPos.x, TRUE);

//	TRACE(L"yIncrease : %d \t\t ynMax : %d \t\t m_yPos : %d \r\n", xIncrease, m_ptScrMax.x, m_ptScrPos.x);

	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

//타임라인의 스크롤 바쪽 기본 모형 출력 및 레이어 이름 출력
void LmUITimeLineView::OnDrawLeft()
{
	
	CClientDC dc(this);

	//DC 객체 선택(회색의 옅은 선)
	CPen pen(PS_SOLID, 1, RGB(0xc8, 0xc8, 0xc8));
	CPen * pOldPen = dc.SelectObject(&pen);

	CBrush brush(RGB(0xF1, 0xF1, 0xF1));
	CBrush * pOldBrush = dc.SelectObject(&brush);

	//버튼이 있는 지역 제외!
	OnExceptButtonArea(dc);

	//버튼이 출력될 장소에 테두리
	CRect rectButtonArea(-1, m_sizeView.cy - m_nScrWidth+1, m_nLayerColSize+1, m_sizeView.cy+1);
	dc.Rectangle(&rectButtonArea);

	//오른쪽 하단 정사각형
	CRect rectBottomLeft(m_sizeView.cx - m_nScrWidth, m_sizeView.cy - m_nScrWidth ,
		m_sizeView.cx+1, m_sizeView.cy+1);
	dc.Rectangle(&rectBottomLeft);

	// 왼쪽 타이틀 
	CRect rectTopLeft(0, 0, m_nLayerColSize, m_nRowSize);
	dc.Rectangle(&rectTopLeft);

	CRect rectPrtTitle(m_nCellSpacing, m_nCellSpacing, 
		m_nLayerColSize - m_nCellSpacing,
		m_nRowSize - m_nCellSpacing);
	WCHAR strPrtTitle[] = L"  Time Line";

	int nStrLen = (int)wcslen(strPrtTitle);

	//윈도우에 레이어의 이름을 출력
	dc.SetBkMode(TRANSPARENT); //다음 호출될 DrawText의 글자 배경을 투명하게 하기 위해 지정
	dc.DrawText(strPrtTitle, nStrLen,&rectPrtTitle, DT_LEFT | DT_SINGLELINE | DT_VCENTER);


	//클리핑 영역 지정! - 레이어 이름이 출력되는 영역이외에는 모두 클리핑 영역에서 제외시킨다.
	dc.SetMapMode(MM_TEXT);
	
	//레이어 이름 영역을 제외한 나머지 오른쪽 Frame Action 영역 및 스크롤바 영역 제외
	dc.ExcludeClipRect( m_nLayerColSize,  0,
						m_sizeView.cx, m_sizeView.cy);
	//왼쪽 하단 버튼들이 있는 사각형 영역제외
	dc.ExcludeClipRect( 0, m_sizeView.cy - m_nScrWidth+1,
						m_nLayerColSize, m_sizeView.cy);
	//상단 타이틀 출력 영역 제외
	dc.ExcludeClipRect( 0,0,
						m_nLayerColSize, m_nRowSize);


	CPoint ptVpOrg(0, -m_scrVertical.GetScrollPos());       // assume no shift for printing
	dc.SetViewportOrg(ptVpOrg);

	
	//////////////////////////////////////////////////////////////////////////
	//레이어 타이틀 출력 부분 시작
	//////////////////////////////////////////////////////////////////////////

	INT nCntLayer=0;
	LayerListItr itr = TheCartoonMaker->GetListLayerBeginItr();
	for( ; itr != TheCartoonMaker->GetListLayerEndItr() ; itr++)
	{
		nCntLayer++; //상위에 제목을 출력했기 때문임.
		//Layer 제목이 출력될 사각 네모칸
		CRect rectLayerTitle(0, nCntLayer * m_nRowSize-1,
							 m_nLayerColSize, (nCntLayer+1)*m_nRowSize);
		dc.Rectangle(&rectLayerTitle);
		
		//글자 배경을 투명으로 지정한 후 레이어 제목을 출력한다.
		dc.SetBkMode(TRANSPARENT);
		CStringW strData = itr->m_wstrLayerName.c_str();
		if(strData.GetLength()> m_nLayerNameLength)
		{
			//넘으면 8자까지 출력후 [...]을 추가
			CStringW str;
			
			//8자를 먼저 잘라낸 후 [...]을 붙인다
			CStringW strLeft = strData.Left(m_nLayerNameLength-3);
			str.Format(L"%s...",strLeft.GetBuffer());
			dc.TextOut(m_nCellSpacing + m_nLeftSpacing,
						nCntLayer * m_nRowSize + m_nCellSpacing-1, str);
		}
		else
		{
			//m_nLayerNameLength자 이하면 그냥 출력
			dc.TextOut(m_nCellSpacing + m_nLeftSpacing, 
						nCntLayer * m_nRowSize + m_nCellSpacing-1, strData);
		}
		
	}
	
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);

}

void LmUITimeLineView::OnDrawTop()
{
	INT nCnt=0;
	CClientDC dc(this);

	INT	nFrameLength = TheCartoonMaker->GetFrameLength();
	INT nCurCaret = TheCartoonMaker->GetCurFrame();

	INT nWidthMemDC = m_sizeView.cx - m_nLayerColSize - m_nScrWidth;
	INT nHeightMemDC = m_nRowSize;

	//////////////////////////////////////////////////////////////////////////
	// 메모리 DC사용
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc); //메모리 DC 생성
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&dc, nWidthMemDC , nHeightMemDC);
	dcMem.SelectObject(&bitmap);

	//설정 버튼 출력 및 상단 프레임 눈금 
	dc.SetMapMode(MM_TEXT);

	//레이어 이름 영역 제외
	dc.ExcludeClipRect(0,  0,
		m_nLayerColSize, m_sizeView.cy);
	//레이어에 소속된 Section Frame 영역 제외
	dc.ExcludeClipRect( m_nLayerColSize , m_nRowSize,
		m_sizeView.cx, m_sizeView.cy);
	//오른쪽 상하 스크롤 바 영역 제외
	dc.ExcludeClipRect( m_sizeView.cx - m_nScrWidth ,0,
		m_sizeView.cx , m_sizeView.cy);

	CPoint ptVpOrg(-m_scrHorizontal.GetScrollPos(),0);       // assume no shift for printing
	//dc.SetViewportOrg(ptVpOrg);

	//DC 객체 선택(회색의 옅은 선)
	CPen pen(PS_SOLID, 1, RGB(0xc8, 0xc8, 0xc8));
	CPen * pOldPen = dcMem.SelectObject(&pen);

	CBrush brush(RGB(0xF1, 0xF1, 0xF1));
	CBrush * pOldBrush = dcMem.SelectObject(&brush);

	CRect rectTopRight(-1, 0, 
		nWidthMemDC +1, 
		m_nRowSize);
	dcMem.Rectangle(&rectTopRight);

	//윈도우에 프레임 눈금자 출력
	dcMem.SetBkMode(TRANSPARENT); //다음 호출될 DrawText의 글자 배경을 투명하게 하기 위해 지정

	// View Port 앞쪽에 그려진 Section Frame 열의 갯수
	INT nFrontLeft = (INT)ptVpOrg.x / m_nFrmColSize;

	// View Port 앞쪽에 그려진 Section Frame 열의 갯수의 나머지 값(실제 Section Frame의 앞쪽 시작 지점을 알려줌)
	INT nFrontLeftRemain = ptVpOrg.x % m_nFrmColSize;

	// View Port 이후에 그려져야 할 Section Frame의 열의 갯수
	INT nAfterLeft = nFrameLength + nFrontLeft;
	
	INT nTextX = 0; 
	INT nTextY = m_nCellSpacing;

	for(nCnt = -nFrontLeft; nTextX <= nWidthMemDC ; nCnt++)
	{
		if(!(nCnt%5))
		{
			CString str;
			nTextX = m_nCellSpacing + (nCnt * m_nFrmColSize) + ptVpOrg.x -1;
			str.Format(L"%d",nCnt+1);
			dcMem.TextOut( nTextX , //x좌표
							nTextY, //y 좌표
							str);
			//CString strDbg;
			//strDbg.Format(L"Top nText X : %d  \t\t nText Y: %d \r\n", nTextX, nTextY);
			//OutputDebugString(strDbg);
		}
	}

	//커서를 Section Frame 눈금자에 그린다.
	CPen penCaret(PS_SOLID, 1, RGB(255, 0, 0));
	dcMem.SelectStockObject(NULL_BRUSH);
	dcMem.SelectObject(&penCaret);

	INT nLeft	= m_nFrmColSize * (nCurCaret-1) + ptVpOrg.x-1;
	INT nTop	= 0;	
	INT nRight	= (m_nFrmColSize * nCurCaret ) + ptVpOrg.x;	
	INT nBottom = m_nRowSize+2;

	//첫번째 Caret의 Left선이 나오지 않는 현상을 막기 위해
	if(nLeft<0)
		nLeft =0;

	CRect rectCaret(nLeft, nTop, nRight, nBottom);

	dcMem.Rectangle(&rectCaret);

	dcMem.SelectObject(pOldPen);
	dcMem.SelectObject(pOldBrush);

	dc.BitBlt(m_nLayerColSize, 0, m_ptScrMax.x , m_nRowSize, &dcMem, 0,0, SRCCOPY);
	bitmap.DeleteObject();
	dcMem.DeleteDC();

}


void LmUITimeLineView::OnDrawMain()
{
	CClientDC dc(this);

	INT	nFrameLength = TheCartoonMaker->GetFrameLength();
	INT nCurCaret = TheCartoonMaker->GetCurFrame();

	//레이어가 하나도 없으면 리턴!
	INT nListSize = TheCartoonMaker->GetListLayerSize();

	if(!nListSize)
		return;

	//////////////////////////////////////////////////////////////////////////
	// 메모리 DC사용

	//메모리 DC의 Width, Height의 크기
	INT nWidthMemDC = m_sizeView.cx - m_nLayerColSize - m_nScrWidth;
	INT nHeightMemDC = m_sizeView.cy - m_nRowSize - m_nScrWidth;

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc); //메모리 DC 생성
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&dc, nWidthMemDC, nHeightMemDC);
	dcMem.SelectObject(&bitmap);

	dc.SetMapMode(MM_TEXT);
	
	//왼쪽 레이어 이름 영역 제외
	dc.ExcludeClipRect(0, 0,
		m_nLayerColSize, m_sizeView.cy);
	//상단 Frame 눈금자 영역 제외
	dc.ExcludeClipRect(m_nLayerColSize, 0,
		m_sizeView.cx - m_nScrWidth , m_nRowSize);
	//하단 스크롤 바 영역 제외
	dc.ExcludeClipRect(0 ,m_sizeView.cy - m_nScrWidth,
		m_sizeView.cx , m_sizeView.cy);
	//오른쪽 스크롤 바 영역 제외
	dc.ExcludeClipRect( m_sizeView.cx - m_nScrWidth ,0,
		m_sizeView.cx , m_sizeView.cy);

	CPoint ptVpOrg( -m_scrHorizontal.GetScrollPos(), -m_scrVertical.GetScrollPos());       // assume no shift for printing

	CRect rectWhiteArea(0,0, nWidthMemDC, nHeightMemDC);
	dcMem.SelectStockObject(WHITE_PEN);
	dcMem.SelectStockObject(WHITE_BRUSH);
	dcMem.Rectangle(&rectWhiteArea);

	//5칸 마다 아이보리색으로 표시한다.

	// 현재 위치를 파악한다.
	// 현재 위치에다가 프레임 칸을 나눈다.
	// 그렇게 하면 몇번째 프레임 부터 선을 그어야 하는지 나온다.
	// 나머지가 나온다면 먼저 나머지 값으로 이동한 후 새로줄을 그어준다.
	
	// View Port 앞쪽에 그려진 Section Frame 열의 갯수
	INT nFrontLeft = (INT)ptVpOrg.x / m_nFrmColSize;

	// View Port 앞쪽에 그려진 Section Frame 열의 갯수의 나머지 값(실제 Section Frame의 앞쪽 시작 지점을 알려줌)
	INT nFrontLeftRemain = ptVpOrg.x % m_nFrmColSize;

	// View Port 이후에 그려져야 할 Section Frame의 열의 갯수
	INT nAfterLeft = nFrameLength - nFrontLeft;

	//내부 소스를 깔끔하게 하기 위한 변수들
	INT left = 0;
	INT top	 = 0;	
	INT right = 0;	
	INT bottom = 0;

	INT nCntActFrm = 0;

	//5칸 마다 아이보리색으로 표시한다.
	for(nCntActFrm = 0 ; right <= nWidthMemDC  ; nCntActFrm+=5)
	{
		CBrush brush(RGB(233, 228, 255));
		CBrush * pOldBrush = dcMem.SelectObject(&brush);

		//나머지 값 부터 5 프레임 구분용 아이보리 사각형이 그려진다.
		//(nFrontLeft %5)의 값은 음수가 나와서 그냥 더해줌.. 원래 빼줘야 되는 값이기 때문..
		left	= nFrontLeftRemain +  (nFrontLeft %5) * m_nFrmColSize +  nCntActFrm * m_nFrmColSize -1;
		top		= 0 ;
		right	= nFrontLeftRemain + (nFrontLeft %5) * m_nFrmColSize + (nCntActFrm+1) * m_nFrmColSize;
		bottom	= nListSize * m_nRowSize;
		if(bottom >= nHeightMemDC)
			bottom	= nHeightMemDC;

		CRect rLyrActFrm( left, top, right, bottom);
		dcMem.SelectStockObject(NULL_PEN);
		dcMem.Rectangle(&rLyrActFrm);			
		dcMem.SelectObject(pOldBrush);	
		//TRACE(L"nFrontLeft %%5 %d \r\n", nFrontLeft %5);
	}


	CPen pen(PS_SOLID, 1, RGB(0xc8, 0xc8, 0xc8));
	CPen * pOldPen = dcMem.SelectObject(&pen);

	INT nCntLayer=0;

	// View Port 위쪽에 그려진 Section Frame 수
	INT nFrontTop = (INT) ptVpOrg.y / m_nRowSize;

	// View Port 앞쪽에 그려진 Section Frame 수의 나머지 값(실제 Section Frame의 위쪽 시작 지점을 알려줌)
	INT nFrontTopRemain = ptVpOrg.y % m_nRowSize;

	// View Port 이후에 그려져야 할 Section Frame의 행 갯수
	INT nAfterTop = nListSize + nFrontTop; 
	
	//내부 소스를 깔끔하게 하기 위한 변수들
	INT nStartX =0;
	INT nStartY =0;
	INT nEndX	=0;
	INT nEndy	=0;

	// 가로선을 먼저 긋는다.
	// 세로 좌표가 ViewPort를 벗어나지 않을때 까지 그린다. 또는 레이어 갯수 만큼 그린다.
	for(nCntLayer=0; (nEndy <= nHeightMemDC) && ( nCntLayer <= nAfterTop ); nCntLayer++)
	{
		nStartX = 0;
		nStartY = nFrontTopRemain + nCntLayer * m_nRowSize-1;
		nEndX= nWidthMemDC;
		nEndy= nFrontTopRemain + nCntLayer * m_nRowSize-1;

		CPoint ptStartRowLine(nStartX, nStartY);
		CPoint ptEndRowLine( nEndX, nEndy);
		dcMem.MoveTo(ptStartRowLine);
		dcMem.LineTo(ptEndRowLine);
	}
	nStartX =0;
	nStartY =0;
	nEndX	=0;
	nEndy	=0;

	// 세로선을 긋는다.
	// 가로 좌표가 ViewPort를 벗어나지 않을때 까지 그린다. 또는 Section Frame의 갯수 만큼 그린다.
	for(nCntActFrm=0; (nEndX <= nWidthMemDC ) && (nCntActFrm <= nAfterLeft); nCntActFrm++)
	{
		nStartX = nFrontLeftRemain + (nCntActFrm) * m_nFrmColSize-1;
		nStartY = 0;
		nEndX	= nFrontLeftRemain + (nCntActFrm) * m_nFrmColSize-1;;
		nEndy	= nListSize * m_nRowSize;
		if(nEndy > nHeightMemDC)
			nEndy = nHeightMemDC;

		//TRACE(L"nEndy : %d \t\t nHeightMemDC : %d \r\n ",nEndy,nHeightMemDC);
		CPoint ptStartRowLine( nStartX, nStartY);
		CPoint ptEndRowLine( nEndX,nEndy);
		dcMem.MoveTo(ptStartRowLine);
		dcMem.LineTo(ptEndRowLine);
	}

	//TRACE(L"nCntLayer : %d\r\n ",nCntLayer);

	//타임라인 내부에 Section Frame을 그린다.
	nCntLayer=0;
	
	LayerListItr itr = TheCartoonMaker->GetListLayerBeginItr();
	for( ; itr != TheCartoonMaker->GetListLayerEndItr() ; itr++)
	{
		//Layer 제목이 출력될 사각 네모칸
		SectionMapItr mItr;
		INT nCntSection=1;
		for(mItr = itr->m_mapSection.begin() ; mItr!= itr->m_mapSection.end() ; mItr++)
		{
			//처음과 끝의 ActionFrame의 값을 알아낸다.
			//
			UINT nStartActFrm =  mItr->second.m_uStartFrm;
			UINT nEndActFrm   =  mItr->second.m_uEndFrm;

			INT nLeft	= (nStartActFrm-1) * m_nFrmColSize-1 + ptVpOrg.x;
			INT nTop	= nCntLayer * m_nRowSize-1 + ptVpOrg.y;
			INT nRight = (nEndActFrm) * m_nFrmColSize+ ptVpOrg.x;
			INT nBottom= (nCntLayer+1)*m_nRowSize + ptVpOrg.y;

			//가장 위에 있는 Layer에 상단 선이 나타나지 않는 문제를 위한 소스임.
			if(nCntLayer == 0)
				nTop=0;

			//1번 프레임의 왼쪽 선을 그려주기 위해
			if(nLeft == -1 )
				nLeft=0;

			CRect rLyrActFrm( nLeft, nTop, nRight, nBottom );
			BOOL bIsDraw = FALSE;

			//TRACE(L"(nLeft, nTop, nRight, nBottom) ( %d, %d, %d, %d ) \r\n",nLeft, nTop, nRight, nBottom);

			COLORREF ColorBrush;
			COLORREF ColorPen;
			LmKIND_OF_LAYER enumLayerType = itr->m_enumKindofLayer;

			//Section 순번은 1부터 시작함!
			//카메라 레이어이며, 카메라 Section의 갯수가 2개 이상인가?
			//그리고 Section 순번이 2번째 Section 이상인가?
			//**//그리고 Section 순번이 마지막 Section 보다 작은가?
			if( (enumLayerType == CAMERA_LAYER) &&
				(( itr->m_mapSection.size()) >= 2) &&
				(nCntSection >= 2) )
			{
				INT nPrevTop	= nCntLayer * m_nRowSize-1 + ptVpOrg.y;
				INT nPrevRight  = (m_nPrevSectionPos) * m_nFrmColSize+ ptVpOrg.x;

				//보간된 Section 영역을 그려준다.
				CRect InterpolationRect( CPoint(nPrevRight-1, nPrevTop), CPoint(nLeft+1, nBottom) );
				BOOL bResult = IsDrawRectInMemDC(nWidthMemDC, nHeightMemDC, InterpolationRect);

				//보간 사각형이 MemDC에 그려져야 하는가?
//				TRACE(L"\r\nbResult %d  nWidthMemDC : %d \t nHeightMemDC : %d \r\n",bResult, nWidthMemDC, nHeightMemDC);
//				TRACE(L"(nLeft, nTop, nRight, nBottom) ( %d, %d, %d, %d ) \r\n",nLeft, nTop, nRight, nBottom);
//				TRACE(L"interpolation(nLeft, nTop, nRight, nBottom) ( %d, %d, %d, %d ) \r\n",nPrevRight, nPrevTop, nLeft, nBottom);

				if(bResult)
				{	
					//그려져야 한다면 그려준다.
					CBrush brush(RGB(248,222,254));
					CBrush * pOldBrush = dcMem.SelectObject(&brush);
					CPen pen(PS_SOLID, 1, RGB(0,0,0));
					CPen *pOldPen = dcMem.SelectObject(&pen);
					dcMem.Rectangle(&InterpolationRect);			
					dcMem.SelectObject(pOldBrush);
					dcMem.SelectObject(pOldPen);
				}
			}

			switch (enumLayerType)
			{
			case CAMERA_LAYER:
				ColorBrush = RGB(244, 174, 232);
				ColorPen = RGB(199, 27, 169);
				m_nPrevSectionPos = mItr->second.m_uStartFrm;
				break;
			case SOUND_LAYER:
				ColorBrush = RGB(203, 221, 252);
				ColorPen = RGB(71, 63, 133);
				break;
			default:
				ColorBrush = RGB(255, 255, 255);
				ColorPen = RGB(0, 0, 0);
				break;
			}

			nCntSection++;
			bIsDraw = IsDrawRectInMemDC(nWidthMemDC, nHeightMemDC, rLyrActFrm);
			if(!bIsDraw)
				continue;

			CBrush brush(ColorBrush);
			CBrush * pOldBrush = dcMem.SelectObject(&brush);
			CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
			CPen *pOldPen = dcMem.SelectObject(&pen);
			
			dcMem.Rectangle(&rLyrActFrm);			
			dcMem.SelectObject(pOldBrush);
			dcMem.SelectObject(pOldPen);

			
		}
		nCntLayer++;  //레이어가 1부터 시작하므로
		//TRACE(L"Layer Cnt * Row Size : %d\r\n",nCntLayer * m_nRowSize);
	}
	
	//Section Frame에 케럿을 그린다.
	CPen penCaret(PS_SOLID, 1, RGB(255, 0, 0));
	dcMem.SelectObject(&penCaret);

	INT nLeft	= m_nFrmColSize * (nCurCaret-1) + ptVpOrg.x -1;
	INT nTop	= -2;	
	INT nRight	= m_nFrmColSize * nCurCaret  + ptVpOrg.x;	
	INT nBottom = nListSize* m_nRowSize + ptVpOrg.y;

	//첫번째 Caret의 Left선이 나오지 않는 현상을 막기 위해
	if(nLeft<0)
		nLeft =0;

	CRect rectCaret(nLeft, nTop, nRight, nBottom);

	dcMem.SelectStockObject(NULL_BRUSH);
	dcMem.Rectangle(&rectCaret);

	dcMem.SelectObject(pOldPen);

	dc.BitBlt(m_nLayerColSize, m_nRowSize, nWidthMemDC , nHeightMemDC, // 가로 세로 크기
		&dcMem, 0,0, SRCCOPY);
	bitmap.DeleteObject();
	dcMem.DeleteDC();
}

void LmUITimeLineView::SetCurrFrame(INT nFrame )
{
	INT nCurCaret = TheCartoonMaker->GetCurFrame();
	INT nListSize = TheCartoonMaker->GetListLayerSize();

	//논리 좌표를 디바이스 좌표로 변환한다
	CClientDC dc(this);
	dc.SetMapMode(MM_TEXT);
	CPoint ptVpOrg(-m_scrHorizontal.GetScrollPos(), -m_scrVertical.GetScrollPos());       // assume no shift for printing
	dc.SetViewportOrg(ptVpOrg);

	INT nPreCaret = nCurCaret;
	
	//카툰 메이커에게 커서의 변경을 알린다.
	TheCartoonMaker->SetCurFrame(nFrame);
	TheCartoonMaker->SetStateOfUI(TIMELINE_CHANGED);

	nCurCaret = nFrame;

	//이전의 캐럿이 있었던 영역만 다시 그려 준다.
	CRect rectPreCaret(m_nLayerColSize + (m_nFrmColSize * (nPreCaret-1) ), 
						0, 
						m_nLayerColSize + (m_nFrmColSize * nPreCaret ), 
						(LONG)(nListSize * m_nRowSize) );
	dc.LPtoDP(&rectPreCaret);
	
	//TRACE(L"rectPreCaret : %d %d %d %d\r\n", rectPreCaret.left, rectPreCaret.top , rectPreCaret.right, rectPreCaret.bottom);
	//Caret의 좌표에 마이너스 값이 있으면 그냥 하지 말도록 코딩하자!!! 2007. 11. 29. 
	
	if( !(rectPreCaret.left <0 || rectPreCaret.right<0 ||
		  rectPreCaret.top<0 || rectPreCaret.bottom<0) )
		InvalidateRect(&rectPreCaret,FALSE);

	//캐럿이 변경된 곳을 다시 그려준다.
	CRect rectCaret(m_nLayerColSize + (m_nFrmColSize * (nCurCaret-1) ), 
					0, 
					m_nLayerColSize + (m_nFrmColSize * nCurCaret ), 
					(nListSize +1) * m_nRowSize );
	

	dc.LPtoDP(&rectCaret);

	//TRACE(L"rectCaret : %d %d %d %d\r\n", rectCaret.left, rectCaret.top , rectCaret.right, rectCaret.bottom);
	InvalidateRect(&rectCaret);
}

//여기에다가 마우스를 클릭했을때 어느 FrameCol인지 인식해서 그 프레임으로 이동하게 하자!
void LmUITimeLineView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CRect rFrameAct;  //
	GetClientRect(&rFrameAct);
	
	rFrameAct.left += m_nLayerColSize;
	//마우스가 FrameActoin 영역에 있는가? 
	//없으면 리턴~
	if(!rFrameAct.PtInRect(point))
		return;

	//여기서 부터는 FrameAction 영역에 있는 경우를 처리.
	CClientDC dc(this);
	dc.SetMapMode(MM_TEXT);
	CPoint ptVpOrg(-m_scrHorizontal.GetScrollPos(), -m_scrVertical.GetScrollPos());       // assume no shift for printing
	dc.SetViewportOrg(ptVpOrg);

	CPoint LPoint = point;
	dc.DPtoLP(&LPoint);

	INT nFrame = ( (LPoint.x - m_nLayerColSize) / m_nFrmColSize ) +1;
	//TRACE(L"LPoint.x : %d  FrameNum ; %d\r\n", LPoint.x, nFrame);

	//현재 프레임 변경
	SetCurrFrame(nFrame);

	CView::OnLButtonDown(nFlags, point);
}

BOOL LmUITimeLineView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return TRUE;
	//return CView::OnEraseBkgnd(pDC);
}

//버튼영역을 DC에서 제외하는 함수
//이렇게 안하면 HoverButton 위체 그림이 그려진다. 
void LmUITimeLineView::OnExceptButtonArea( CClientDC &dc )
{
	
	CRect rectPlay,rectStop, rectNext, rectPrev, rectFirst, rectLast, rectConfig;
	GetButtonRect(rectPlay,		m_hBtnPlay);
	GetButtonRect(rectStop,		m_hBtnStop);
	GetButtonRect(rectNext,		m_hBtnNextFrm);
	GetButtonRect(rectPrev,		m_hBtnPrevFrm);
	GetButtonRect(rectFirst,	m_hBtnFirstFrm);
	GetButtonRect(rectLast,		m_hBtnLastFrm);
	GetButtonRect(rectConfig,	m_hBtnConfig);


	dc.SetMapMode(MM_TEXT);
	dc.ExcludeClipRect(&rectPlay);
	dc.ExcludeClipRect(&rectStop);
	dc.ExcludeClipRect(&rectNext);
	dc.ExcludeClipRect(&rectPrev);
	dc.ExcludeClipRect(&rectFirst);
	dc.ExcludeClipRect(&rectLast);
	dc.ExcludeClipRect(&rectConfig);
	
	return;
}

void LmUITimeLineView::GetButtonRect( CRect & rect , CHoverButton & button)
{
	WINDOWINFO wndInfo;
	button.GetWindowInfo(&wndInfo);
	rect = wndInfo.rcClient;
	ScreenToClient(&rect);
}

void LmUITimeLineView::OnBtnClickedPlay()
{
	ChangePlaytoStopBtn();

	//쓰레드를 시작한다.
	m_pMotionThread->PostThreadMessage(WM_MOTION_PLAY,0,0);
	
	//카메라를 마우스로 조작할 수 없음
	TheCartoonMaker->EnableCameraMouse(FALSE);
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->EnableAllMenu(FALSE);

}

void LmUITimeLineView::OnBtnClickedStop()
{
	ChangeStopToPlayBtn();

	//쓰레드를 멈춘다.
	m_pMotionThread->PostThreadMessage(WM_MOTION_STOP,0,0);
	
	//카메라를 마우스로 조작할 수 있음.
	TheCartoonMaker->EnableCameraMouse(TRUE);
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

	//메뉴를 다시 돌린다.
	pMainFrame->EnableAllMenu(TRUE);
	pMainFrame->EnableCharacterMenu( FALSE);
	pMainFrame->EnableEditMenu( FALSE );
	pMainFrame->EnableMovieClipMenu(TRUE);


}

void LmUITimeLineView::OnBtnClickedNext()
{
	GoNextCaret();
}

void LmUITimeLineView::OnBtnClickedPrev()
{
	GoPrevCaret();
}

void LmUITimeLineView::OnBtnClickedFirst()
{
	SetCurrFrame(1);	
	INT nFirstFrmAmount =  - m_ptScrPos.x;
	m_ptScrPos.x = 0;

	CRect rectScroll;
	GetClientRect(&rectScroll);
	rectScroll.left += m_nLayerColSize;
	rectScroll.right -= m_nScrWidth;
	rectScroll.top = 0 ;
	rectScroll.bottom -= m_nScrWidth;

	//남은 양 만큼 스크롤~
	ScrollWindow(-nFirstFrmAmount,0, rectScroll, rectScroll); 
	m_scrHorizontal.SetScrollPos( 0, FALSE);
	m_scrHorizontal.Invalidate();
}

void LmUITimeLineView::OnBtnClickedLast()
{
	INT	nFrameLength = TheCartoonMaker->GetFrameLength();
	SetCurrFrame(nFrameLength);	
	
	//새로운 위치 계산
	INT nLastFrmAmount = m_ptScrMax.x - m_ptScrPos.x;
	m_ptScrPos.x = m_ptScrMax.x;

	CRect rectScroll;
	GetClientRect(&rectScroll);
	rectScroll.left += m_nLayerColSize;
	rectScroll.right -= m_nScrWidth;
	rectScroll.top = 0 ;
	rectScroll.bottom -= m_nScrWidth;

	//남은 양 만큼 스크롤~
	ScrollWindow(-nLastFrmAmount,0, rectScroll, rectScroll); 
	m_scrHorizontal.SetScrollPos( m_ptScrMax.x, FALSE);
	m_scrHorizontal.Invalidate();
}

//Frame의 길이를 변경한다.
void LmUITimeLineView::OnBtnClickedConfig()
{
	LmUITimeLineConf dlg;

	INT nFrameLength = TheCartoonMaker->GetFrameLength();
	INT nListSize = TheCartoonMaker->GetListLayerSize();

	dlg.m_nFrameLength = nFrameLength;
	
	if(dlg.DoModal() == IDOK)
	{
		//입력받은 Frame 길이와 초당 Frame 수를 반영한다.
		nFrameLength = dlg.m_nFrameLength;
		TheCartoonMaker->SetFrameLength(nFrameLength);

		ReSizeFrameLength(nListSize, nFrameLength);

	}
}
BOOL LmUITimeLineView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//cs.lpszClass = AfxRegisterWndClass(0); 배경색이 NULL임.. 
	return CView::PreCreateWindow(cs);
}


UINT LmUITimeLineView::GetCurrFrame()
{
	return TheCartoonMaker->GetCurFrame();
}

void LmUITimeLineView::ReFreshVertScroll()
{
	INT nListSize = TheCartoonMaker->GetListLayerSize();
	INT nFrameLength = TheCartoonMaker->GetFrameLength();

	m_ptScrMax.y = nListSize * m_nRowSize;
	
	if(m_ptScrMax.y <0)
		m_ptScrMax.y = 0;

	SCROLLINFO vert;

	vert.cbSize = sizeof(SCROLLINFO);
	vert.nMin = 0;
	vert.nMax = m_ptScrMax.y;
	vert.nPos = 0;
	vert.fMask = SIF_ALL;
	vert.nPage = m_sizeView.cy-m_nScrWidth -m_nRowSize;

	m_scrVertical.SetScrollInfo(&vert, TRUE);

}

INT LmUITimeLineView::GoNextCaret()
{
	INT	nFrameLength = TheCartoonMaker->GetFrameLength();
	INT nCurCaret = TheCartoonMaker->GetCurFrame();

	if(nCurCaret<nFrameLength)
	{
		//SendMessage(WM_HSCROLL, MAKELONG(SB_CARETDOWN,0), 0);
		PostMessage(WM_HSCROLL, MAKELONG(SB_CARETDOWN,0), 0);
		SetCurrFrame(++nCurCaret);
	}
	else
		return -1;
	return 0L;
}

INT LmUITimeLineView::GoPrevCaret()
{
	INT nCurCaret = TheCartoonMaker->GetCurFrame();

	if(nCurCaret>1)
	{
		PostMessage(WM_HSCROLL, MAKELONG(SB_CARETUP,0), 0);
		SetCurrFrame(--nCurCaret);
	}
	else
		return -1;
	return 0;
}

void LmUITimeLineView::ChangeStopToPlayBtn()
{
	if(!m_isPlaying)
	{
		m_hBtnPlay.ShowWindow(SW_SHOW);
		m_hBtnStop.ShowWindow(SW_HIDE);
	}
}

void LmUITimeLineView::ChangePlaytoStopBtn()
{
	if(!m_isPlaying)
	{
		m_hBtnPlay.ShowWindow(SW_HIDE);
		m_hBtnStop.ShowWindow(SW_SHOW);
		Invalidate();
	}
}

BOOL LmUITimeLineView::IsDrawRectInMemDC( INT nWidthMemDC, INT nHeightMemDC, CRect rectSection )
{
	//Section Frame Rectangle의 4개의 점 중에 1개라도 MemDC에 소속되어 있으면 그려야 한다!
	BOOL bIsDraw = FALSE;
	CRect rectMemDC(0, 0, nWidthMemDC, nHeightMemDC);
	INT nLeft = rectSection.left;
	INT nRight = rectSection.right;
	INT nBottom = rectSection.bottom;
	INT nTop = rectSection.top;

	if( rectMemDC.PtInRect( CPoint(nLeft, nTop)) )
		bIsDraw = TRUE;
	if( rectMemDC.PtInRect( CPoint(nRight, nTop)) )
		bIsDraw = TRUE;
	if( rectMemDC.PtInRect( CPoint(nLeft, nBottom)) )
		bIsDraw = TRUE;
	if( rectMemDC.PtInRect( CPoint(nRight, nBottom)) )
		bIsDraw = TRUE;

	//4개의 점이 memory DC를 포함해 버렸을 때도 그려줘야 한다.
	if( (nLeft <= 0) && (nRight >= nWidthMemDC) )
		bIsDraw = TRUE;
	if( (nTop <= 0) && (nBottom >= nHeightMemDC) )
		bIsDraw = TRUE;
	return bIsDraw;
}

void LmUITimeLineView::ReSizeFrameLength( INT nListSize, INT nFrameLength )
{
	m_ptScrMax.y = (nListSize+1) * m_nRowSize;
	m_ptScrMax.y -= (3*m_nScrWidth);

	m_ptScrMax.x = ( m_nFrmColSize * nFrameLength );

	INT nMinWidth	 = m_sizeView.cx - m_nLayerColSize - m_nScrWidth;      // 타임라인 최소 Width 크기
	// 이것이 너무 작으면 화면에 출력이 깨짐

	if(m_ptScrMax.y <0)
		m_ptScrMax.y = 0;

	if(m_ptScrMax.x < nMinWidth)
		m_ptScrMax.x = nMinWidth;

	SCROLLINFO vert, horz;

	vert.cbSize = sizeof(SCROLLINFO);
	vert.nMin = 0;
	vert.nMax = m_ptScrMax.y;
	vert.nPos = 0;
	vert.fMask = SIF_ALL;
	vert.nPage = m_sizeView.cy-65;

	m_scrVertical.SetScrollInfo(&vert, TRUE);

	horz.cbSize = sizeof(SCROLLINFO);
	horz.nMin = 0;
	horz.nMax = m_ptScrMax.x;
	horz.nPos = 0;
	horz.fMask = SIF_ALL;
	horz.nPage = m_sizeView.cx-m_nScrWidth - m_nLayerColSize;

	m_scrHorizontal.SetScrollInfo(&horz, TRUE);


	Invalidate();
}
//inline CMy3CMakerDoc* LmUITimeLineView::GetDocument() const
//{ return reinterpret_cast<CMy3CMakerDoc*>(m_pDocument); }