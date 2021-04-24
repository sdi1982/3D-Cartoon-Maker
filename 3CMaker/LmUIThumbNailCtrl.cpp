// LmUIThumbNailCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LmUIThumbNailCtrl.h"

// LmUIThumbNailCtrl

IMPLEMENT_DYNAMIC(LmUIThumbNailCtrl, CWnd)

LmUIThumbNailCtrl::LmUIThumbNailCtrl()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_nThumWidth = 150;
	m_nThumHeight = 150;
	m_nRowSpace = 20;
	m_nLeftSpace = 20;
	m_nSecLeftSpace = 40;
	m_nTitleTopSpace = 4;
	m_nTitleHeight = 20;
	m_nCaret = -1;
	m_itemType = NOTHING;
	m_yMax=0;
	m_yPos=0;

	HIMAGELIST hScreens = ImageList_Create(m_nThumWidth, m_nThumHeight, ILC_COLOR32, 0, 1);
	m_imageList.Attach(hScreens);

}

LmUIThumbNailCtrl::~LmUIThumbNailCtrl()
{
}


BEGIN_MESSAGE_MAP(LmUIThumbNailCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

HBITMAP LmUIThumbNailCtrl::LoadPicture( LPCTSTR szBMPFileName )
{
	IPicture * pPic; //사진을 위한 클래스

	CString strChangeMode =szBMPFileName; 
	WCHAR wszFilename[255];
	wcsncpy_s(wszFilename, strChangeMode.GetBuffer(), 255);
	//OLE 객체형식으로 새로운 사진 객체를 만들고 초기화 시킴
	OleLoadPicturePath(wszFilename, NULL, NULL, NULL, IID_IPicture,(LPVOID*)&pPic);

	//IPicture 객체가 제대로 생성이 되었는가?
	if (pPic==NULL) 
	{
		SetErrorMsg(L"찾는 파일이 없습니다. OleLoadPicturePath 함수에서 IPicture 객체 생성 실패!");
		return NULL;
	}

	HBITMAP hPic = NULL;

	//핸들을 이용해서 hPic에 BITMAP에 그림을 등록한다.
	pPic->get_Handle((UINT*)&hPic);

	//가로, 세로 값을 정해둔다.
	long nWidth = m_nThumWidth;
	long nHeight = m_nThumHeight;

	//현재 그림의 가로, 세로 길이를 얻어온다.
	long mWid, mHei;
	pPic->get_Height(&mHei);
	pPic->get_Width(&mWid);

	//핸들을 이용해서 이미지 켄버스를 생성하고 복사하고 할당하기 전에 있던 이미지를 삭제한다.
	HBITMAP hPicRet = (HBITMAP)CopyImage(hPic, IMAGE_BITMAP, nWidth, nHeight, LR_COPYDELETEORG);

	// 백그라운드와 테두리를 위한 브러시 생성
	HBRUSH hBrushBorder=::CreateSolidBrush(RGB(192, 192, 192));
	HBRUSH hBrushBk=::CreateSolidBrush(RGB(255, 255, 255));

	// 테두리 사이즈
	RECT rcBorder;
	rcBorder.left=rcBorder.top=0;
	rcBorder.right=m_nThumWidth;
	rcBorder.bottom=m_nThumHeight;

	// 비율 값 계산
	const float fRatio=(float)m_nThumHeight/m_nThumWidth;

	// 켄버스의 비율 값을 계산해서 가로가 큰지 세로가 큰지에 따라서 이미지를 조절한다.

	INT XDest, YDest, nDestWidth, nDestHeight;
	const float fImgRatio = (float)mHei/mWid;

	//이미지가 켄버스 보다 길죽한가?
	if(fImgRatio > fRatio)
	{
		//이미지가 켄버스 보다 길쭉하면 생성될 이미지의 가로 길이를 세로 길이의 비율에 맞춘다
		nDestWidth = (int)((float)m_nThumHeight*mWid/mHei);
		XDest = (m_nThumWidth-nDestWidth)/2;
		YDest = 0;
		nDestHeight = m_nThumHeight;
	}
	else
	{
		//이미지가 켄버스 보다 길쭉하면 생성될 이미지의 세로 길이를 가로 길이의 비율에 맞춘다
		XDest = 0;
		nDestWidth = m_nThumWidth;
		nDestHeight = (int)((float)m_nThumWidth * mHei / mWid );
		YDest = (m_nThumHeight - nDestHeight)/2;
	}

	CClientDC cdc(this);
	HDC hDC = ::CreateCompatibleDC(cdc.m_hDC); //메모리 DC 생성
	HBITMAP bm = CreateCompatibleBitmap(cdc.m_hDC, m_nThumWidth, m_nThumHeight); //메모리 비트맵 생성
	HBITMAP pOldBitmapImage = (HBITMAP)SelectObject(hDC, bm);	//메모리 DC가 비트맵 객체를 선택한다.
	//반환 값은 대체된 객체의 핸들이다.즉, bm 비트메모리가 선택된 hDC 메모리 DC의 핸들


	//메모리 bitmap에서 그림 -------
	// 백그라운드 그리기
	::FillRect(hDC, &rcBorder, hBrushBk);
	// 테두리 그리기
	::FrameRect(hDC, &rcBorder, hBrushBorder);
	//메모리 bitmap에서 그림 ------- 여기까지

	HBITMAP hBmReturn = (HBITMAP)::SelectObject(hDC, pOldBitmapImage); // 대체한 HBITMAP 핸들을 원상복구 한다.


	//CDC를 생성한다. 
	CDC hdcSrc, hdcDst;

	//메모리 DC를 생성
	hdcSrc.CreateCompatibleDC(NULL);
	hdcDst.CreateCompatibleDC(NULL);

	// 켄버스를 메모리 DC에 로딩한다.
	CBitmap* hbmSrcT = (CBitmap*) hdcSrc.SelectObject(hPicRet);		//원본 이미지를 메모리 비트맵에 올린다.
	CBitmap* hbmDstT = (CBitmap*) hdcDst.SelectObject(hBmReturn);	//축소시킬 이미지를 메모리 비트맵에 올린다.

	//메모리 DC로 생성된 Thumbnail(새롭게 생성된 hdcDst DC임)에 그림을 그린다.
	hdcDst.StretchBlt(XDest+1, YDest+1, nDestWidth-2, nDestHeight-2, &hdcSrc, 0,0, m_nThumWidth, m_nThumHeight , SRCCOPY);

	pOldBitmapImage = (HBITMAP)SelectObject(hdcDst.m_hDC, bm);

	DeleteDC(hDC);
	DeleteObject(hBrushBorder);
	DeleteObject(hBrushBk);

	DeleteObject(pPic);
	DeleteObject(hPicRet);

	return pOldBitmapImage;
}

void LmUIThumbNailCtrl::ThmInsert( LPCTSTR szBMPFileName, LPCTSTR szSceneName )
{
	//비트맵을 읽어온다.
	HBITMAP bitm = LoadPicture(szBMPFileName);

	//입력 받은 BMP파일이 없는가? 그렇다면 return 하라!
	if(bitm == NULL)
		return;

	CBitmap* pImage = new CBitmap();
	pImage->Attach(bitm); //bitmap 핸들을 받아온다.

	int nImgEnd = m_imageList.Add(pImage, RGB(0,0,0)); //이미지 리스트에 썸네일 Image를 삽입한다.

	//m_listThumb에 썸네일 정보를 등록한다.

	int nCntNode = (int)m_listThumb.GetCount();
	CRectItem NewNode;
	NewNode.m_ptLocation.x = m_nLeftSpace;
	
	if(nCntNode%2)  
		// 0 - index 부터 시작하는 카운터를 2로 나눈 나머지가 1일때 오른쪽에 액자가 출력되어야 함.
		// 오른쪽 액자 인가?
	{
		NewNode.m_ptLocation.x += m_nSecLeftSpace; //여백을 더한다.
		NewNode.m_ptLocation.x += m_nThumWidth; //넓이를 더한다.
	}
	
	NewNode.m_ptLocation.y = m_nRowSpace + (m_nRowSpace + m_nThumHeight + m_nTitleTopSpace + m_nTitleHeight) * (nCntNode / 2) ; // 수열식 적용

	NewNode.nIndex = nCntNode ;
	NewNode.nImgIndex = nImgEnd;
	wcsncpy_s(NewNode.strItemName,szSceneName, MAX_STR);

	m_listThumb.AddTail(NewNode);

	//메모리 해제
	delete pImage;

	onRefreshScroll();
}

void LmUIThumbNailCtrl::onRefreshScroll()
{
	if(!m_listThumb.GetCount())
		return;

	CRectItem data = m_listThumb.GetTail();

	CRect rect;
	GetClientRect(&rect);
	//10을 빼는 이유는 전체 스크린 좌표에서 최고 크기를 생각했기 때문임.
	
	m_yMax = data.m_ptLocation.y + m_nThumHeight + m_nTitleTopSpace + m_nTitleHeight;// - rect.Height();

	SCROLLINFO vert;

	vert.cbSize = sizeof(SCROLLINFO);
	vert.nMin = 0;
	vert.nMax = m_yMax;//+65;
	vert.nPos = 0;
	vert.fMask = SIF_ALL;
	vert.nPage = rect.Height();

	SetScrollInfo(SB_VERT ,&vert, TRUE);
}

void LmUIThumbNailCtrl::ThmDrawCaret( CPoint leftTop )
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	
	CClientDC dc(this);
	
	//ScrollView::OnPrepareDC(&dc)의 실제 코드를 직접 구현.
	dc.SetMapMode(MM_TEXT);
	CPoint ptVpOrg(-GetScrollPos(SB_HORZ), -GetScrollPos(SB_VERT));       // assume no shift for printing
	dc.SetViewportOrg(ptVpOrg);
	

	CPoint pointTCaret, pointBCaret;
	pointTCaret.x = leftTop.x - 4;
	pointTCaret.y = leftTop.y - 4;

	pointBCaret.x = leftTop.x + m_nThumWidth + 4;
	pointBCaret.y = leftTop.y + m_nThumHeight + 4;

	//캐럿을 그린다.
	CRect rectCaret(pointTCaret, pointBCaret);
	dc.SelectStockObject(NULL_BRUSH);
	CPen penCaret(PS_SOLID, 4, RGB(0,0,0));
	CPen *pOldPen = dc.SelectObject(&penCaret);

	dc.Rectangle(&rectCaret);
	dc.SelectObject(pOldPen);

	//캐럿 보다 좀더 큰영역을 저장해 두었다가 캐럿이 바뀔때 invalidaterect를 한다.

	pointTCaret.x -=2;
	pointTCaret.y -=2;

	pointBCaret.x +=2;
	pointBCaret.y +=2;

	CRect rectTmp(pointTCaret, pointBCaret);
	m_rectPreCaret = rectTmp;

}

void LmUIThumbNailCtrl::ThmDrawCaret( CPoint leftTop , CDC & dcMem )
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CClientDC dc(this);

	//ScrollView::OnPrepareDC(&dc)의 실제 코드를 직접 구현.
	dc.SetMapMode(MM_TEXT);
	CPoint ptVpOrg(-GetScrollPos(SB_HORZ), -GetScrollPos(SB_VERT));       // assume no shift for printing
	//dc.SetViewportOrg(ptVpOrg);


	CPoint pointTCaret, pointBCaret;
	pointTCaret.x = leftTop.x - 4;
	pointTCaret.y = leftTop.y - 4;

	pointBCaret.x = leftTop.x + m_nThumWidth + 4;
	pointBCaret.y = leftTop.y + m_nThumHeight + 4;

	//캐럿을 그린다.
	CRect rectCaret(pointTCaret + ptVpOrg, pointBCaret + ptVpOrg);
	dcMem.SelectStockObject(NULL_BRUSH);
	CPen penCaret(PS_SOLID, 4, RGB(0,0,0));
	CPen *pOldPen = dcMem.SelectObject(&penCaret);

	dcMem.Rectangle(&rectCaret);
	dcMem.SelectObject(pOldPen);

	//캐럿 보다 좀더 큰영역을 저장해 두었다가 캐럿이 바뀔때 invalidaterect를 한다.

	pointTCaret.x -=2;
	pointTCaret.y -=2;

	pointBCaret.x +=2;
	pointBCaret.y +=2;

	CRect rectTmp(pointTCaret, pointBCaret);
	m_rectPreCaret = rectTmp;

}



// LmUIThumbNailCtrl 메시지 처리기입니다.



void LmUIThumbNailCtrl::OnPaint()
{
	CPaintDC paintDc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CWnd::OnPaint()을(를) 호출하지 마십시오.

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	CClientDC dc(this);

	CRect rectScreen;
	GetClientRect(&rectScreen);

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc); //메모리 DC 생성
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&dc, rectScreen.Width(), rectScreen.Height());
	dcMem.SelectObject(&bitmap);

	dcMem.SelectStockObject(WHITE_PEN);
	dcMem.Rectangle(&rectScreen);

	INT nListCnt = (INT)m_listThumb.GetCount();
	INT nItemCnt = 0; //2개의 열을 출력하기 위한 카운터

	//ScrollView::OnPrepareDC(&dc)의 실제 코드를 직접 구현.
	dc.SetMapMode(MM_TEXT);
	CPoint ptVpOrg(-GetScrollPos(SB_HORZ), -GetScrollPos(SB_VERT));       // assume no shift for printing
	//dc.SetViewportOrg(ptVpOrg);

	int nCntCaret=0; //커서를 위해 사용되는 Count 변수
	POSITION pPos = m_listThumb.GetHeadPosition();
	CRectItem data;
	memset(&data, 0, sizeof(data));

	while(pPos)
	{
		data = m_listThumb.GetNext(pPos);


		//화면에 출력을 할 것인지 하지 않을 것인지 검사한다.
		// MemDC에 4개의 액자([이미지공간 + 이미지 이름 공간]을 구성하는 사각형)의 4개의 점 중에서 1개라도
		// MemDC의 영역에 들어와 있으면 그림을 그리고 그렇지 않으면 다음으로 넘어간다.
		INT nLeft	= data.m_ptLocation.x + ptVpOrg.x;
		INT nTop	= data.m_ptLocation.y + ptVpOrg.y;
		INT nRight = data.m_ptLocation.x + m_nThumWidth + ptVpOrg.x;
		INT nBottom= data.m_ptLocation.y + m_nThumHeight + m_nTitleTopSpace + m_nTitleHeight +ptVpOrg.y;

		//Section Frame Rectangle의 4개의 점 중에 1개라도 MemDC에 소속되어 있으면 그려야 한다!
		CRect rectMemDC(0, 0, rectScreen.Width(), rectScreen.Height());

		BOOL bIsDraw = FALSE;

		if( rectMemDC.PtInRect( CPoint(nLeft, nTop)) )
			bIsDraw = TRUE;
		if( rectMemDC.PtInRect( CPoint(nRight, nTop)) )
			bIsDraw = TRUE;
		if( rectMemDC.PtInRect( CPoint(nLeft, nBottom)) )
			bIsDraw = TRUE;
		if( rectMemDC.PtInRect( CPoint(nRight, nBottom)) )
			bIsDraw = TRUE;

		//4개의 점이 memory DC를 포함해 버렸을 때도 그려줘야 한다.
		if( (nLeft <= 0) && (nRight >= rectScreen.Width()) )
			bIsDraw = TRUE;
		if( (nTop <= 0) && (nBottom >= rectScreen.Height()) )
			bIsDraw = TRUE;

		// 4개의 점이 하나도 MemDC 영역 위에 있지 않으면 그리지 않는다.
		if(!bIsDraw)
		{
			nCntCaret++;
			nItemCnt++;
			continue;
		}
		
		//만일 출력하는 화면이 현재 캐럿일 경우 액자에 두께를 두껍게 찍는다.
		if(m_nCaret == nCntCaret)
			ThmDrawCaret(data.m_ptLocation, dcMem);

		m_imageList.Draw(&dcMem, data.nImgIndex, data.m_ptLocation + ptVpOrg ,ILD_NORMAL);

		WCHAR strPrtSceneName[12];
		memset(strPrtSceneName, 0, sizeof(strPrtSceneName) );

		//유니코드가 8글자 넘는가?
		if(wcslen(data.strItemName)>8)
		{
			//넘으면 8자까지 출력후 [...]을 추가

			CStringW str;
			str.Format(L"%s",data.strItemName);

			//8자를 먼저 잘라낸 후 [...]을 붙인다
			CStringW strLeft_8 = str.Left(8);
			str.Format(L"%s...",strLeft_8.GetBuffer());
			wcsncpy_s(strPrtSceneName, str.GetBuffer(), 255);

		}
		else
		{
			//8자 이하면 그냥 출력
			CStringW str;
			str.Format(L"%s",data.strItemName);
			wcsncpy_s(strPrtSceneName , str.GetBuffer(),255);
		}

		//화면에 글자를 출력하기 위한 Rect 영역 지정
		CPoint pointTop = data.m_ptLocation;
		pointTop.y += (m_nThumHeight+ m_nTitleTopSpace);

		CPoint pointBottom;
		pointBottom.x = data.m_ptLocation.x + m_nThumWidth;
		pointBottom.y = pointTop.y + m_nTitleHeight;

		CRect rectPrtSceneName(pointTop + ptVpOrg, pointBottom + ptVpOrg);
		int nStrLen = (int)wcslen(strPrtSceneName);

		//윈도우에 이미지의 이름을 출력
		dcMem.DrawText(strPrtSceneName, nStrLen,&rectPrtSceneName, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		nCntCaret++;
		nItemCnt++;
	}

	dc.BitBlt(0, 0, rectScreen.Width() ,rectScreen.Height(), &dcMem, 0,0, SRCCOPY);
	bitmap.DeleteObject();
	dcMem.DeleteDC();

}

void LmUIThumbNailCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	onRefreshScroll();
}

void LmUIThumbNailCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	POSITION pPos = m_listThumb.GetHeadPosition();
	CRectItem data;

	//데이터가 없을때는 그냥 리턴~
	if(!m_listThumb.GetCount())
		return;

	SetFocus();
	while(pPos)
	{
		data = m_listThumb.GetNext(pPos);
		CPoint pointBottom(data.m_ptLocation.x + m_nThumWidth , 
			data.m_ptLocation.y + m_nThumHeight + m_nTitleTopSpace + m_nTitleHeight);
		CRect rectArea(data.m_ptLocation , pointBottom);

		//클릭한 위치가 그림의 액자 영역에 들어갔다면
		//그 그림액자를 두껍게 그려준다.
		CClientDC dc(this);

		//ScrollView::OnPrepareDC(&dc)의 실제 코드를 직접 구현.
		dc.SetMapMode(MM_TEXT);
		CPoint ptVpOrg(-GetScrollPos(SB_HORZ), -GetScrollPos(SB_VERT));       // assume no shift for printing
		dc.SetViewportOrg(ptVpOrg);


		CPoint LogicPoint = point;
		dc.DPtoLP(&LogicPoint);

		if(rectArea.PtInRect(LogicPoint))
		{
			m_nCaret = data.nIndex;

			CRect rectMapping;
			rectMapping=m_rectPreCaret;
			dc.LPtoDP(&rectMapping);

			InvalidateRect(&rectMapping);
			ThmDrawCaret(data.m_ptLocation);


			//Scene 변화 함수를 부르는 곳.
			OnSelectedItem(data.strItemName);
			//data.strItemName //--- 아이템 이름!


			break;
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void LmUIThumbNailCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

//마우스 휠을 처리하기 위한 함수
BOOL LmUIThumbNailCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(zDelta<0)
		SendMessage(WM_VSCROLL, MAKELONG(SB_PAGEDOWN,0), 0);
	else
		SendMessage(WM_VSCROLL, MAKELONG(SB_PAGEUP,0), 0);
		

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}


//에러메시지를 출력하기 위한 함수
VOID LmUIThumbNailCtrl::SetErrorMsg( LPCTSTR lpszErrMsg )
{
	wcsncpy_s(m_szErrMsg, lpszErrMsg, MAXIMUM_ERR_MSG);	
}

//스크롤을 해주는 함수
void LmUIThumbNailCtrl::ThmScroll( CSize sizeInput )
{
	//SendMessage(LVM_SCROLL, sizeInput.cx, sizeInput.cy);
}


void LmUIThumbNailCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int yIncrease=0;
	CRect rectView;
	GetClientRect(&rectView);
	
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
		GetScrollInfo(SB_VERT,&scrHinfo);
		yIncrease = scrHinfo.nTrackPos - m_yPos;
		break;
	}

	//새로운 위치는 최소한 0 이상 
	if(m_yPos + yIncrease < 0)
		yIncrease = -m_yPos; //흠... 

	// 새로운 위치는 최대한 yMax 이하
	// 실제 스크롤 Thumb 크기 지정과 스크롤 크기 지정은 달라야 함.
	// 스크롤 크기 지정이 Thumb 크기 지정 보다 스크롤 화면의 클라이언트 높이 만큼의 작다.
	INT ynMax = m_yMax- rectView.Height();
	if( (m_yPos + yIncrease) > ynMax)
	{
		yIncrease = ynMax - m_yPos;
	}

	//새로운 위치 계산
	m_yPos +=  yIncrease;

	ScrollWindow(0,-yIncrease, NULL, NULL);
	SetScrollPos(SB_VERT, m_yPos, TRUE);

	TRACE(L"yIncrease : %d \t\t ynMax : %d \t\t m_yPos : %d \r\n", yIncrease, ynMax, m_yPos);

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

int LmUIThumbNailCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &Lines,0);

	SetCursor(LoadCursor(NULL, IDC_ARROW));
	
	return 0;
}

BOOL LmUIThumbNailCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_ClassName = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW,0,(HBRUSH)::GetStockObject(NULL_BRUSH),0);
	cs.lpszClass = m_ClassName;

	return CWnd::PreCreateWindow(cs);
}

BOOL LmUIThumbNailCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return TRUE;
	//return CWnd::OnEraseBkgnd(pDC);
}

void LmUIThumbNailCtrl::SetThumbNailCaret( UINT nCaret )
{
	POSITION pPos = m_listThumb.GetHeadPosition();
	CRectItem data;

	//데이터가 없을때는 그냥 리턴~
	if(!m_listThumb.GetCount())
		return;

	SetFocus();
	while(pPos)
	{
		data = m_listThumb.GetNext(pPos);
		CPoint pointBottom(data.m_ptLocation.x + m_nThumWidth , 
			data.m_ptLocation.y + m_nThumHeight + m_nTitleTopSpace + m_nTitleHeight);
		CRect rectArea(data.m_ptLocation , pointBottom);

		//클릭한 위치가 그림의 액자 영역에 들어갔다면
		//그 그림액자를 두껍게 그려준다.
		CClientDC dc(this);

		//ScrollView::OnPrepareDC(&dc)의 실제 코드를 직접 구현.
		dc.SetMapMode(MM_TEXT);
		CPoint ptVpOrg(-GetScrollPos(SB_HORZ), -GetScrollPos(SB_VERT));       // assume no shift for printing
		dc.SetViewportOrg(ptVpOrg);

		if(data.nIndex = nCaret)
		{
			m_nCaret = data.nIndex;

			CRect rectMapping;
			rectMapping=m_rectPreCaret;
			dc.LPtoDP(&rectMapping);

			InvalidateRect(&rectMapping);
			ThmDrawCaret(data.m_ptLocation);

			//멤버 변수에 현재 Caret의 정보를 백업 받아 둔다.
			m_ThumbNailCaret.m_ptLocation = data.m_ptLocation;
			m_ThumbNailCaret.nImgIndex = data.nImgIndex;
			m_ThumbNailCaret.nIndex = data.nIndex;
			wcsncpy_s(m_ThumbNailCaret.strItemName, data.strItemName, wcslen(data.strItemName));

			//Scene 변화 함수를 부르는 곳.
			OnSelectedItem(data.strItemName);
			break;
		}
	}
}

void LmUIThumbNailCtrl::GettThumbNailCaret( CRectItem & getCaretData )
{
	getCaretData.m_ptLocation = m_ThumbNailCaret.m_ptLocation;
	getCaretData.nImgIndex = m_ThumbNailCaret.nImgIndex;
	getCaretData.nIndex = m_ThumbNailCaret.nIndex;
	wcsncpy_s(getCaretData.strItemName, m_ThumbNailCaret.strItemName, wcslen(m_ThumbNailCaret.strItemName));

}

void LmUIThumbNailCtrl::SetItemType( ITEMTYPE itemtype )
{
	m_itemType = itemtype;
	return;
}

void LmUIThumbNailCtrl::OnSelectedItem( CString strItemName )
{
	// [동인]
	
	switch(m_itemType)
	{
	// CHARECTERS - 캐릭터가 선택되었을때 루틴
	case CHARECTERS :
		TheCartoonMaker->SelectChar(strItemName.GetBuffer(0));
		break;
	// THINGS - 물체가 선택되었을때 루틴
	case THINGS:
		TheCartoonMaker->SelectThing(strItemName.GetBuffer(0));
		break;
	// BACKGROUNDS - 배경이 선택되었을때 루틴
	case BACKGROUNDS:
		TheCartoonMaker->PlaceBGNowScean(strItemName.GetBuffer(0));
		break;
	default:
		break;
	}
}
BOOL LmUIThumbNailCtrl::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_RETURN)
		return TRUE;

	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_ESCAPE)
		return TRUE;

	return CWnd::PreTranslateMessage(pMsg);
}
