// TimeLineView.cpp : ���� �����Դϴ�.
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

//TheCartoonMaker�� ���� Layer List�� const�� �޾ƿ´�.
LmUITimeLineView::LmUITimeLineView()
{
	m_isLoadComplete = FALSE;

	m_nScrWidth = 0;
	m_nRowSize = 25;		//25px�� ��ũ��
	m_nLayerColSize = 200;	//250px Layer ĭ ����ũ��
	m_nFrmColSize = 10;		//Frame 1ĭ�� 10px
	m_nCellSpacing = 3;		//�簢�� ���� ����
	m_nLeftSpacing = 10;	//���ڸ� ���� ���� �簢�� ���� ����(CellSpaing �ٷ� ���� ���� ����)
	m_nLayerNameLength = 20;

	m_ptScrPos.SetPoint(0,0); //��ũ�� ��ġ �ʱ�ȭ
	m_ptScrMax.SetPoint(0,0); //��ũ�� �ִ밪

	m_isPlaying = FALSE; //���� Play ���ΰ�?

	m_pMotionThread = NULL;
	m_pMotionThread = new LmUIMotionThread(this);
	m_pMotionThread->CreateThread();

/*
	nFrameLength = 1000;	//Ÿ�Ӷ����� ������ ��
	m_nFrmPerSec = 20;		//1�ʴ� �� 10���� �������� ���ȴ�. 
	nCurCaret = 1;		//1 ���� ����!
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


// CTimeLineView �׸����Դϴ�.
//INT a=0;
void LmUITimeLineView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();

	if(m_sizeView.cy > m_ptScrMax.y)
	{
		CClientDC dc(this);

		//������ ���� ��ũ�� �� ���� ����
		dc.ExcludeClipRect( m_sizeView.cx - m_nScrWidth ,0,
			m_sizeView.cx , m_sizeView.cy);
		//�ϴ� ��ũ�� �� ���� ����
		dc.ExcludeClipRect(0 ,m_sizeView.cy - m_nScrWidth + 1,
			m_sizeView.cx , m_sizeView.cy);
		
		INT nListSize = TheCartoonMaker->GetListLayerSize();

		CRect rectWhiteArea(0, (nListSize+1) * m_nRowSize, m_sizeView.cx, m_sizeView.cy);
		dc.SelectStockObject(NULL_PEN);
		dc.SelectStockObject(WHITE_BRUSH);
		dc.Rectangle(&rectWhiteArea);
	}

	//���� ���̾� �̸� �κ� ���
	OnDrawLeft();
	
	//���� ���̾ ���� Action���� ����
	OnDrawMain();
	
	//���� ������ ������ ����
	OnDrawTop();
	

	//SetCurrFrame(++a);
	//m_hBtnPlay.Invalidate();
}


// CTimeLineView �����Դϴ�.

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


// CTimeLineView �޽��� ó�����Դϴ�.

void LmUITimeLineView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: �� ���� ��ü ũ�⸦ ����մϴ�.
	CRect rect;
	GetClientRect(&rect);

	//�� ũ�⸦ ��������� ����� �ΰ�, Ÿ�Ӷ����� �׸��� ����Ѵ�.
	m_sizeView .cx = rect.Width();
	m_sizeView .cy = rect.Height();
	
	//��ũ�� �ٸ� �����Ѵ�.
	CRect rectVScroll(0,0,0,0);
	CRect rectHScroll(0,0,0,0);
	m_scrVertical.Create(WS_CHILD | WS_VISIBLE | SBS_VERT, rectVScroll , this, SCROLLBAR_VERTICAL);
	m_scrHorizontal.Create(WS_CHILD | WS_VISIBLE | SBS_HORZ, rectHScroll, this, SCROLLBAR_HORIZONTAL);
	//SetScrollSizes(MM_TEXT, m_ViewSize);
	
	m_nScrWidth = GetSystemMetrics(SM_CYHSCROLL);
	m_isLoadComplete = TRUE;

	//��ư�� �����Ѵ�. ��ġ�� OnSize���� �Ѵ�.
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | WS_TABSTOP;

	CRect rectInitBtn(0,0,10,10);
	m_hBtnPlay.Create(L"PLAY",dwStyle, rectInitBtn, this, HOVERBUTTON_PLAY);
	m_hBtnStop.Create(L"STOP",dwStyle, rectInitBtn, this, HOVERBUTTON_STOP);
	m_hBtnStop.ShowWindow(SW_HIDE); //stop�� toggle��. �ʱ⿡�� �÷��� ��ư��.

	m_hBtnNextFrm.Create(L"NEXT",dwStyle, rectInitBtn, this, HOVERBUTTON_NEXT);	
	m_hBtnPrevFrm.Create(L"PREV",dwStyle, rectInitBtn, this, HOVERBUTTON_PREV);
	m_hBtnFirstFrm.Create(L"FIRST",dwStyle, rectInitBtn, this, HOVERBUTTON_FIRST);
	m_hBtnLastFrm.Create(L"LAST",dwStyle, rectInitBtn, this, HOVERBUTTON_LAST);
	m_hBtnConfig.Create(L"CONFIG",dwStyle, rectInitBtn, this, HOVERBUTTON_CONFIG);


	//��ư�� �׸� ���
	m_hBtnPlay.LoadBitmap(IDB_BTNPLAY);
	m_hBtnStop.LoadBitmap(IDB_BTNSTOP);
	m_hBtnNextFrm.LoadBitmap(IDB_BTNNEXT);
	m_hBtnPrevFrm.LoadBitmap(IDB_BTNPREV);
	m_hBtnFirstFrm.LoadBitmap(IDB_BTNFIRST);
	m_hBtnLastFrm.LoadBitmap(IDB_BTNLAST);
	m_hBtnConfig.LoadBitmap(IDB_BTNCONFIG);

	//�켱�� ���⿡ �д�.[InitTimeLIne] [�׽�Ʈ��]
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
	//OnInitialUpdate �� ��� ����Ǿ��°�? ������� ������ �����Ѵ�. 
	if(!m_isLoadComplete)
		return;

	//�� ũ�⸦ ��������� ����� �ΰ�, Ÿ�Ӷ����� �׸��� ����Ѵ�.
	m_sizeView.cx = cx;
	m_sizeView.cy = cy;

	INT nHorzXMetrix = m_nLayerColSize;						// HSCROLL X ��ǥ
	INT nHorzYMetrix = cy - m_nScrWidth;					// HSCROLL Y ��ǥ	
	INT nHorzWidth   = cx - nHorzXMetrix -m_nScrWidth;		// HSCROLL Length
	INT nHorzHeight  = m_nScrWidth;							// HSCROLL Width

	INT nVertXMetrix = cx - m_nScrWidth;					// VSCROLL X ��ǥ
	INT nVertYMetrix = 0;									// VSCROLL Y ��ǥ	
	INT nVertWidth   = m_nScrWidth;							// VSCROLL Width
	INT nVertHeight  = cy - m_nScrWidth;					// VSCROLL Length

	INT nMinWidth	 = nVertXMetrix - m_nLayerColSize - m_nScrWidth;    // Ÿ�Ӷ��� �ּ� Width ũ��
																		// �̰��� �ʹ� ������ ȭ�鿡 ����� ����

	//��ũ�ѹ� ��ġ �ڵ� ����
	m_scrHorizontal.MoveWindow(nHorzXMetrix, nHorzYMetrix, nHorzWidth, nHorzHeight,TRUE);
	m_scrVertical.MoveWindow(nVertXMetrix, nVertYMetrix, nVertWidth, nVertHeight, TRUE);

	//CWnd::GetClientRect --> ClientToScreen�� �̿��ϸ� ��ġ�� ũ�⸦ ���� �� 
	//������ DC���� �׸��� ������ ���� ���ѹ�����!!
	//��ư ��ġ �ڵ� ����
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


	// ȭ�麸�� ���� ��� (ȭ�� ũ��- m_nLayerColSize) ��ŭ �Ҵ��Ѵ�.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

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

	//���ο� ��ġ�� �ּ��� 0 �̻� 
	if(m_ptScrPos.y + yIncrease < 0)
		yIncrease = -m_ptScrPos.y; //���� Scroll Thumb�� �������� ����� ��.

	CRect rectView;
	GetClientRect(&rectView);

	// ���� ��ũ�� Thumb ũ�� ������ ��ũ�� ũ�� ������ �޶�� ��.
	// ��ũ�� ũ�� ������ Thumb ũ�� ���� ���� ��ũ�� ȭ���� Ŭ���̾�Ʈ ���� ��ŭ�� �۴�.
	INT ynMax = m_ptScrMax.y- m_sizeView.cy + m_nScrWidth + m_nRowSize;

	if( (m_ptScrPos.y + yIncrease ) > ynMax)
		yIncrease = ynMax - m_ptScrPos.y;

	//���ο� ��ġ ���
	m_ptScrPos.y +=  yIncrease;

	CRect rectScroll;
	GetClientRect(&rectScroll);
	rectScroll.left = 0;
	rectScroll.right -= m_nScrWidth;
	rectScroll.top += m_nRowSize;
	rectScroll.bottom -= m_nScrWidth;

	//�ʿ��� ������ ��ũ�� �Ѵ�.
	ScrollWindow(0,-yIncrease, rectScroll, rectScroll);
	m_scrVertical.SetScrollPos( m_ptScrPos.y, TRUE);

//	TRACE(L"yIncrease : %d \t\t ynMax : %d \t\t m_yPos : %d \r\n", yIncrease, m_ptScrMax.y, m_ptScrPos.y);

	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void LmUITimeLineView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	// �����Ǿƿ��� ��ũ�� ó�� �Ұ�.. 16��Ʈ �̻� ��ũ�� ���� �޾ƿ;���!!;
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

	//���ο� ��ġ�� �ּ��� 0 �̻� 
	if(m_ptScrPos.x + xIncrease < 0)
		xIncrease = -m_ptScrPos.x; // 0�� ������ ���� ��Ų��.

	// ���� ��ũ�� Thumb ũ�� ������ ��ũ�� ũ�� ������ �޶�� ��.
	// ��ũ�� ũ�� ������ Thumb ũ�� ���� ���� ��ũ�� ȭ���� Ŭ���̾�Ʈ ���� ��ŭ�� �۴�.
	INT xnMax = m_ptScrMax.x- m_sizeView.cx + m_nScrWidth + m_nLayerColSize;

	if( (m_ptScrPos.x + xIncrease ) > xnMax)
		xIncrease = xnMax - m_ptScrPos.x;

	//���ο� ��ġ ���
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

//Ÿ�Ӷ����� ��ũ�� ���� �⺻ ���� ��� �� ���̾� �̸� ���
void LmUITimeLineView::OnDrawLeft()
{
	
	CClientDC dc(this);

	//DC ��ü ����(ȸ���� ���� ��)
	CPen pen(PS_SOLID, 1, RGB(0xc8, 0xc8, 0xc8));
	CPen * pOldPen = dc.SelectObject(&pen);

	CBrush brush(RGB(0xF1, 0xF1, 0xF1));
	CBrush * pOldBrush = dc.SelectObject(&brush);

	//��ư�� �ִ� ���� ����!
	OnExceptButtonArea(dc);

	//��ư�� ��µ� ��ҿ� �׵θ�
	CRect rectButtonArea(-1, m_sizeView.cy - m_nScrWidth+1, m_nLayerColSize+1, m_sizeView.cy+1);
	dc.Rectangle(&rectButtonArea);

	//������ �ϴ� ���簢��
	CRect rectBottomLeft(m_sizeView.cx - m_nScrWidth, m_sizeView.cy - m_nScrWidth ,
		m_sizeView.cx+1, m_sizeView.cy+1);
	dc.Rectangle(&rectBottomLeft);

	// ���� Ÿ��Ʋ 
	CRect rectTopLeft(0, 0, m_nLayerColSize, m_nRowSize);
	dc.Rectangle(&rectTopLeft);

	CRect rectPrtTitle(m_nCellSpacing, m_nCellSpacing, 
		m_nLayerColSize - m_nCellSpacing,
		m_nRowSize - m_nCellSpacing);
	WCHAR strPrtTitle[] = L"  Time Line";

	int nStrLen = (int)wcslen(strPrtTitle);

	//�����쿡 ���̾��� �̸��� ���
	dc.SetBkMode(TRANSPARENT); //���� ȣ��� DrawText�� ���� ����� �����ϰ� �ϱ� ���� ����
	dc.DrawText(strPrtTitle, nStrLen,&rectPrtTitle, DT_LEFT | DT_SINGLELINE | DT_VCENTER);


	//Ŭ���� ���� ����! - ���̾� �̸��� ��µǴ� �����̿ܿ��� ��� Ŭ���� �������� ���ܽ�Ų��.
	dc.SetMapMode(MM_TEXT);
	
	//���̾� �̸� ������ ������ ������ ������ Frame Action ���� �� ��ũ�ѹ� ���� ����
	dc.ExcludeClipRect( m_nLayerColSize,  0,
						m_sizeView.cx, m_sizeView.cy);
	//���� �ϴ� ��ư���� �ִ� �簢�� ��������
	dc.ExcludeClipRect( 0, m_sizeView.cy - m_nScrWidth+1,
						m_nLayerColSize, m_sizeView.cy);
	//��� Ÿ��Ʋ ��� ���� ����
	dc.ExcludeClipRect( 0,0,
						m_nLayerColSize, m_nRowSize);


	CPoint ptVpOrg(0, -m_scrVertical.GetScrollPos());       // assume no shift for printing
	dc.SetViewportOrg(ptVpOrg);

	
	//////////////////////////////////////////////////////////////////////////
	//���̾� Ÿ��Ʋ ��� �κ� ����
	//////////////////////////////////////////////////////////////////////////

	INT nCntLayer=0;
	LayerListItr itr = TheCartoonMaker->GetListLayerBeginItr();
	for( ; itr != TheCartoonMaker->GetListLayerEndItr() ; itr++)
	{
		nCntLayer++; //������ ������ ����߱� ������.
		//Layer ������ ��µ� �簢 �׸�ĭ
		CRect rectLayerTitle(0, nCntLayer * m_nRowSize-1,
							 m_nLayerColSize, (nCntLayer+1)*m_nRowSize);
		dc.Rectangle(&rectLayerTitle);
		
		//���� ����� �������� ������ �� ���̾� ������ ����Ѵ�.
		dc.SetBkMode(TRANSPARENT);
		CStringW strData = itr->m_wstrLayerName.c_str();
		if(strData.GetLength()> m_nLayerNameLength)
		{
			//������ 8�ڱ��� ����� [...]�� �߰�
			CStringW str;
			
			//8�ڸ� ���� �߶� �� [...]�� ���δ�
			CStringW strLeft = strData.Left(m_nLayerNameLength-3);
			str.Format(L"%s...",strLeft.GetBuffer());
			dc.TextOut(m_nCellSpacing + m_nLeftSpacing,
						nCntLayer * m_nRowSize + m_nCellSpacing-1, str);
		}
		else
		{
			//m_nLayerNameLength�� ���ϸ� �׳� ���
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
	// �޸� DC���
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc); //�޸� DC ����
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&dc, nWidthMemDC , nHeightMemDC);
	dcMem.SelectObject(&bitmap);

	//���� ��ư ��� �� ��� ������ ���� 
	dc.SetMapMode(MM_TEXT);

	//���̾� �̸� ���� ����
	dc.ExcludeClipRect(0,  0,
		m_nLayerColSize, m_sizeView.cy);
	//���̾ �Ҽӵ� Section Frame ���� ����
	dc.ExcludeClipRect( m_nLayerColSize , m_nRowSize,
		m_sizeView.cx, m_sizeView.cy);
	//������ ���� ��ũ�� �� ���� ����
	dc.ExcludeClipRect( m_sizeView.cx - m_nScrWidth ,0,
		m_sizeView.cx , m_sizeView.cy);

	CPoint ptVpOrg(-m_scrHorizontal.GetScrollPos(),0);       // assume no shift for printing
	//dc.SetViewportOrg(ptVpOrg);

	//DC ��ü ����(ȸ���� ���� ��)
	CPen pen(PS_SOLID, 1, RGB(0xc8, 0xc8, 0xc8));
	CPen * pOldPen = dcMem.SelectObject(&pen);

	CBrush brush(RGB(0xF1, 0xF1, 0xF1));
	CBrush * pOldBrush = dcMem.SelectObject(&brush);

	CRect rectTopRight(-1, 0, 
		nWidthMemDC +1, 
		m_nRowSize);
	dcMem.Rectangle(&rectTopRight);

	//�����쿡 ������ ������ ���
	dcMem.SetBkMode(TRANSPARENT); //���� ȣ��� DrawText�� ���� ����� �����ϰ� �ϱ� ���� ����

	// View Port ���ʿ� �׷��� Section Frame ���� ����
	INT nFrontLeft = (INT)ptVpOrg.x / m_nFrmColSize;

	// View Port ���ʿ� �׷��� Section Frame ���� ������ ������ ��(���� Section Frame�� ���� ���� ������ �˷���)
	INT nFrontLeftRemain = ptVpOrg.x % m_nFrmColSize;

	// View Port ���Ŀ� �׷����� �� Section Frame�� ���� ����
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
			dcMem.TextOut( nTextX , //x��ǥ
							nTextY, //y ��ǥ
							str);
			//CString strDbg;
			//strDbg.Format(L"Top nText X : %d  \t\t nText Y: %d \r\n", nTextX, nTextY);
			//OutputDebugString(strDbg);
		}
	}

	//Ŀ���� Section Frame �����ڿ� �׸���.
	CPen penCaret(PS_SOLID, 1, RGB(255, 0, 0));
	dcMem.SelectStockObject(NULL_BRUSH);
	dcMem.SelectObject(&penCaret);

	INT nLeft	= m_nFrmColSize * (nCurCaret-1) + ptVpOrg.x-1;
	INT nTop	= 0;	
	INT nRight	= (m_nFrmColSize * nCurCaret ) + ptVpOrg.x;	
	INT nBottom = m_nRowSize+2;

	//ù��° Caret�� Left���� ������ �ʴ� ������ ���� ����
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

	//���̾ �ϳ��� ������ ����!
	INT nListSize = TheCartoonMaker->GetListLayerSize();

	if(!nListSize)
		return;

	//////////////////////////////////////////////////////////////////////////
	// �޸� DC���

	//�޸� DC�� Width, Height�� ũ��
	INT nWidthMemDC = m_sizeView.cx - m_nLayerColSize - m_nScrWidth;
	INT nHeightMemDC = m_sizeView.cy - m_nRowSize - m_nScrWidth;

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc); //�޸� DC ����
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&dc, nWidthMemDC, nHeightMemDC);
	dcMem.SelectObject(&bitmap);

	dc.SetMapMode(MM_TEXT);
	
	//���� ���̾� �̸� ���� ����
	dc.ExcludeClipRect(0, 0,
		m_nLayerColSize, m_sizeView.cy);
	//��� Frame ������ ���� ����
	dc.ExcludeClipRect(m_nLayerColSize, 0,
		m_sizeView.cx - m_nScrWidth , m_nRowSize);
	//�ϴ� ��ũ�� �� ���� ����
	dc.ExcludeClipRect(0 ,m_sizeView.cy - m_nScrWidth,
		m_sizeView.cx , m_sizeView.cy);
	//������ ��ũ�� �� ���� ����
	dc.ExcludeClipRect( m_sizeView.cx - m_nScrWidth ,0,
		m_sizeView.cx , m_sizeView.cy);

	CPoint ptVpOrg( -m_scrHorizontal.GetScrollPos(), -m_scrVertical.GetScrollPos());       // assume no shift for printing

	CRect rectWhiteArea(0,0, nWidthMemDC, nHeightMemDC);
	dcMem.SelectStockObject(WHITE_PEN);
	dcMem.SelectStockObject(WHITE_BRUSH);
	dcMem.Rectangle(&rectWhiteArea);

	//5ĭ ���� ���̺��������� ǥ���Ѵ�.

	// ���� ��ġ�� �ľ��Ѵ�.
	// ���� ��ġ���ٰ� ������ ĭ�� ������.
	// �׷��� �ϸ� ���° ������ ���� ���� �׾�� �ϴ��� ���´�.
	// �������� ���´ٸ� ���� ������ ������ �̵��� �� �������� �׾��ش�.
	
	// View Port ���ʿ� �׷��� Section Frame ���� ����
	INT nFrontLeft = (INT)ptVpOrg.x / m_nFrmColSize;

	// View Port ���ʿ� �׷��� Section Frame ���� ������ ������ ��(���� Section Frame�� ���� ���� ������ �˷���)
	INT nFrontLeftRemain = ptVpOrg.x % m_nFrmColSize;

	// View Port ���Ŀ� �׷����� �� Section Frame�� ���� ����
	INT nAfterLeft = nFrameLength - nFrontLeft;

	//���� �ҽ��� ����ϰ� �ϱ� ���� ������
	INT left = 0;
	INT top	 = 0;	
	INT right = 0;	
	INT bottom = 0;

	INT nCntActFrm = 0;

	//5ĭ ���� ���̺��������� ǥ���Ѵ�.
	for(nCntActFrm = 0 ; right <= nWidthMemDC  ; nCntActFrm+=5)
	{
		CBrush brush(RGB(233, 228, 255));
		CBrush * pOldBrush = dcMem.SelectObject(&brush);

		//������ �� ���� 5 ������ ���п� ���̺��� �簢���� �׷�����.
		//(nFrontLeft %5)�� ���� ������ ���ͼ� �׳� ������.. ���� ����� �Ǵ� ���̱� ����..
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

	// View Port ���ʿ� �׷��� Section Frame ��
	INT nFrontTop = (INT) ptVpOrg.y / m_nRowSize;

	// View Port ���ʿ� �׷��� Section Frame ���� ������ ��(���� Section Frame�� ���� ���� ������ �˷���)
	INT nFrontTopRemain = ptVpOrg.y % m_nRowSize;

	// View Port ���Ŀ� �׷����� �� Section Frame�� �� ����
	INT nAfterTop = nListSize + nFrontTop; 
	
	//���� �ҽ��� ����ϰ� �ϱ� ���� ������
	INT nStartX =0;
	INT nStartY =0;
	INT nEndX	=0;
	INT nEndy	=0;

	// ���μ��� ���� �ߴ´�.
	// ���� ��ǥ�� ViewPort�� ����� ������ ���� �׸���. �Ǵ� ���̾� ���� ��ŭ �׸���.
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

	// ���μ��� �ߴ´�.
	// ���� ��ǥ�� ViewPort�� ����� ������ ���� �׸���. �Ǵ� Section Frame�� ���� ��ŭ �׸���.
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

	//Ÿ�Ӷ��� ���ο� Section Frame�� �׸���.
	nCntLayer=0;
	
	LayerListItr itr = TheCartoonMaker->GetListLayerBeginItr();
	for( ; itr != TheCartoonMaker->GetListLayerEndItr() ; itr++)
	{
		//Layer ������ ��µ� �簢 �׸�ĭ
		SectionMapItr mItr;
		INT nCntSection=1;
		for(mItr = itr->m_mapSection.begin() ; mItr!= itr->m_mapSection.end() ; mItr++)
		{
			//ó���� ���� ActionFrame�� ���� �˾Ƴ���.
			//
			UINT nStartActFrm =  mItr->second.m_uStartFrm;
			UINT nEndActFrm   =  mItr->second.m_uEndFrm;

			INT nLeft	= (nStartActFrm-1) * m_nFrmColSize-1 + ptVpOrg.x;
			INT nTop	= nCntLayer * m_nRowSize-1 + ptVpOrg.y;
			INT nRight = (nEndActFrm) * m_nFrmColSize+ ptVpOrg.x;
			INT nBottom= (nCntLayer+1)*m_nRowSize + ptVpOrg.y;

			//���� ���� �ִ� Layer�� ��� ���� ��Ÿ���� �ʴ� ������ ���� �ҽ���.
			if(nCntLayer == 0)
				nTop=0;

			//1�� �������� ���� ���� �׷��ֱ� ����
			if(nLeft == -1 )
				nLeft=0;

			CRect rLyrActFrm( nLeft, nTop, nRight, nBottom );
			BOOL bIsDraw = FALSE;

			//TRACE(L"(nLeft, nTop, nRight, nBottom) ( %d, %d, %d, %d ) \r\n",nLeft, nTop, nRight, nBottom);

			COLORREF ColorBrush;
			COLORREF ColorPen;
			LmKIND_OF_LAYER enumLayerType = itr->m_enumKindofLayer;

			//Section ������ 1���� ������!
			//ī�޶� ���̾��̸�, ī�޶� Section�� ������ 2�� �̻��ΰ�?
			//�׸��� Section ������ 2��° Section �̻��ΰ�?
			//**//�׸��� Section ������ ������ Section ���� ������?
			if( (enumLayerType == CAMERA_LAYER) &&
				(( itr->m_mapSection.size()) >= 2) &&
				(nCntSection >= 2) )
			{
				INT nPrevTop	= nCntLayer * m_nRowSize-1 + ptVpOrg.y;
				INT nPrevRight  = (m_nPrevSectionPos) * m_nFrmColSize+ ptVpOrg.x;

				//������ Section ������ �׷��ش�.
				CRect InterpolationRect( CPoint(nPrevRight-1, nPrevTop), CPoint(nLeft+1, nBottom) );
				BOOL bResult = IsDrawRectInMemDC(nWidthMemDC, nHeightMemDC, InterpolationRect);

				//���� �簢���� MemDC�� �׷����� �ϴ°�?
//				TRACE(L"\r\nbResult %d  nWidthMemDC : %d \t nHeightMemDC : %d \r\n",bResult, nWidthMemDC, nHeightMemDC);
//				TRACE(L"(nLeft, nTop, nRight, nBottom) ( %d, %d, %d, %d ) \r\n",nLeft, nTop, nRight, nBottom);
//				TRACE(L"interpolation(nLeft, nTop, nRight, nBottom) ( %d, %d, %d, %d ) \r\n",nPrevRight, nPrevTop, nLeft, nBottom);

				if(bResult)
				{	
					//�׷����� �Ѵٸ� �׷��ش�.
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
		nCntLayer++;  //���̾ 1���� �����ϹǷ�
		//TRACE(L"Layer Cnt * Row Size : %d\r\n",nCntLayer * m_nRowSize);
	}
	
	//Section Frame�� �ɷ��� �׸���.
	CPen penCaret(PS_SOLID, 1, RGB(255, 0, 0));
	dcMem.SelectObject(&penCaret);

	INT nLeft	= m_nFrmColSize * (nCurCaret-1) + ptVpOrg.x -1;
	INT nTop	= -2;	
	INT nRight	= m_nFrmColSize * nCurCaret  + ptVpOrg.x;	
	INT nBottom = nListSize* m_nRowSize + ptVpOrg.y;

	//ù��° Caret�� Left���� ������ �ʴ� ������ ���� ����
	if(nLeft<0)
		nLeft =0;

	CRect rectCaret(nLeft, nTop, nRight, nBottom);

	dcMem.SelectStockObject(NULL_BRUSH);
	dcMem.Rectangle(&rectCaret);

	dcMem.SelectObject(pOldPen);

	dc.BitBlt(m_nLayerColSize, m_nRowSize, nWidthMemDC , nHeightMemDC, // ���� ���� ũ��
		&dcMem, 0,0, SRCCOPY);
	bitmap.DeleteObject();
	dcMem.DeleteDC();
}

void LmUITimeLineView::SetCurrFrame(INT nFrame )
{
	INT nCurCaret = TheCartoonMaker->GetCurFrame();
	INT nListSize = TheCartoonMaker->GetListLayerSize();

	//�� ��ǥ�� ����̽� ��ǥ�� ��ȯ�Ѵ�
	CClientDC dc(this);
	dc.SetMapMode(MM_TEXT);
	CPoint ptVpOrg(-m_scrHorizontal.GetScrollPos(), -m_scrVertical.GetScrollPos());       // assume no shift for printing
	dc.SetViewportOrg(ptVpOrg);

	INT nPreCaret = nCurCaret;
	
	//ī�� ����Ŀ���� Ŀ���� ������ �˸���.
	TheCartoonMaker->SetCurFrame(nFrame);
	TheCartoonMaker->SetStateOfUI(TIMELINE_CHANGED);

	nCurCaret = nFrame;

	//������ ĳ���� �־��� ������ �ٽ� �׷� �ش�.
	CRect rectPreCaret(m_nLayerColSize + (m_nFrmColSize * (nPreCaret-1) ), 
						0, 
						m_nLayerColSize + (m_nFrmColSize * nPreCaret ), 
						(LONG)(nListSize * m_nRowSize) );
	dc.LPtoDP(&rectPreCaret);
	
	//TRACE(L"rectPreCaret : %d %d %d %d\r\n", rectPreCaret.left, rectPreCaret.top , rectPreCaret.right, rectPreCaret.bottom);
	//Caret�� ��ǥ�� ���̳ʽ� ���� ������ �׳� ���� ������ �ڵ�����!!! 2007. 11. 29. 
	
	if( !(rectPreCaret.left <0 || rectPreCaret.right<0 ||
		  rectPreCaret.top<0 || rectPreCaret.bottom<0) )
		InvalidateRect(&rectPreCaret,FALSE);

	//ĳ���� ����� ���� �ٽ� �׷��ش�.
	CRect rectCaret(m_nLayerColSize + (m_nFrmColSize * (nCurCaret-1) ), 
					0, 
					m_nLayerColSize + (m_nFrmColSize * nCurCaret ), 
					(nListSize +1) * m_nRowSize );
	

	dc.LPtoDP(&rectCaret);

	//TRACE(L"rectCaret : %d %d %d %d\r\n", rectCaret.left, rectCaret.top , rectCaret.right, rectCaret.bottom);
	InvalidateRect(&rectCaret);
}

//���⿡�ٰ� ���콺�� Ŭ�������� ��� FrameCol���� �ν��ؼ� �� ���������� �̵��ϰ� ����!
void LmUITimeLineView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CRect rFrameAct;  //
	GetClientRect(&rFrameAct);
	
	rFrameAct.left += m_nLayerColSize;
	//���콺�� FrameActoin ������ �ִ°�? 
	//������ ����~
	if(!rFrameAct.PtInRect(point))
		return;

	//���⼭ ���ʹ� FrameAction ������ �ִ� ��츦 ó��.
	CClientDC dc(this);
	dc.SetMapMode(MM_TEXT);
	CPoint ptVpOrg(-m_scrHorizontal.GetScrollPos(), -m_scrVertical.GetScrollPos());       // assume no shift for printing
	dc.SetViewportOrg(ptVpOrg);

	CPoint LPoint = point;
	dc.DPtoLP(&LPoint);

	INT nFrame = ( (LPoint.x - m_nLayerColSize) / m_nFrmColSize ) +1;
	//TRACE(L"LPoint.x : %d  FrameNum ; %d\r\n", LPoint.x, nFrame);

	//���� ������ ����
	SetCurrFrame(nFrame);

	CView::OnLButtonDown(nFlags, point);
}

BOOL LmUITimeLineView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	return TRUE;
	//return CView::OnEraseBkgnd(pDC);
}

//��ư������ DC���� �����ϴ� �Լ�
//�̷��� ���ϸ� HoverButton ��ü �׸��� �׷�����. 
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

	//�����带 �����Ѵ�.
	m_pMotionThread->PostThreadMessage(WM_MOTION_PLAY,0,0);
	
	//ī�޶� ���콺�� ������ �� ����
	TheCartoonMaker->EnableCameraMouse(FALSE);
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->EnableAllMenu(FALSE);

}

void LmUITimeLineView::OnBtnClickedStop()
{
	ChangeStopToPlayBtn();

	//�����带 �����.
	m_pMotionThread->PostThreadMessage(WM_MOTION_STOP,0,0);
	
	//ī�޶� ���콺�� ������ �� ����.
	TheCartoonMaker->EnableCameraMouse(TRUE);
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

	//�޴��� �ٽ� ������.
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

	//���� �� ��ŭ ��ũ��~
	ScrollWindow(-nFirstFrmAmount,0, rectScroll, rectScroll); 
	m_scrHorizontal.SetScrollPos( 0, FALSE);
	m_scrHorizontal.Invalidate();
}

void LmUITimeLineView::OnBtnClickedLast()
{
	INT	nFrameLength = TheCartoonMaker->GetFrameLength();
	SetCurrFrame(nFrameLength);	
	
	//���ο� ��ġ ���
	INT nLastFrmAmount = m_ptScrMax.x - m_ptScrPos.x;
	m_ptScrPos.x = m_ptScrMax.x;

	CRect rectScroll;
	GetClientRect(&rectScroll);
	rectScroll.left += m_nLayerColSize;
	rectScroll.right -= m_nScrWidth;
	rectScroll.top = 0 ;
	rectScroll.bottom -= m_nScrWidth;

	//���� �� ��ŭ ��ũ��~
	ScrollWindow(-nLastFrmAmount,0, rectScroll, rectScroll); 
	m_scrHorizontal.SetScrollPos( m_ptScrMax.x, FALSE);
	m_scrHorizontal.Invalidate();
}

//Frame�� ���̸� �����Ѵ�.
void LmUITimeLineView::OnBtnClickedConfig()
{
	LmUITimeLineConf dlg;

	INT nFrameLength = TheCartoonMaker->GetFrameLength();
	INT nListSize = TheCartoonMaker->GetListLayerSize();

	dlg.m_nFrameLength = nFrameLength;
	
	if(dlg.DoModal() == IDOK)
	{
		//�Է¹��� Frame ���̿� �ʴ� Frame ���� �ݿ��Ѵ�.
		nFrameLength = dlg.m_nFrameLength;
		TheCartoonMaker->SetFrameLength(nFrameLength);

		ReSizeFrameLength(nListSize, nFrameLength);

	}
}
BOOL LmUITimeLineView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	//cs.lpszClass = AfxRegisterWndClass(0); ������ NULL��.. 
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
	//Section Frame Rectangle�� 4���� �� �߿� 1���� MemDC�� �ҼӵǾ� ������ �׷��� �Ѵ�!
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

	//4���� ���� memory DC�� ������ ������ ���� �׷���� �Ѵ�.
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

	INT nMinWidth	 = m_sizeView.cx - m_nLayerColSize - m_nScrWidth;      // Ÿ�Ӷ��� �ּ� Width ũ��
	// �̰��� �ʹ� ������ ȭ�鿡 ����� ����

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