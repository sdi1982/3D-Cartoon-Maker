//LmUIThumbnailView.cpp : CLmUIThumbnailView 클래스의 구현
//


#include "stdafx.h"
#include "3CMaker.h"
#include "3CMakerDoc.h"
#include "LmUISceneView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLmUIThumbnailView

IMPLEMENT_DYNCREATE(LmUISceneView, CScrollView)

BEGIN_MESSAGE_MAP(LmUISceneView, CScrollView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZING()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CLmUIThumbnailView 생성/소멸

LmUISceneView::LmUISceneView()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_nThumWidth = 75;
	m_nThumHeight = 75;
	m_nRowSpace = 20;
	m_nLeftSpace = 20;
	m_nTitleTopSpace = 4;
	m_nTitleHeight = 20;
	m_nCaret = 0;
	m_isLoadComplete = FALSE;
	m_pThreadLoadPicture=NULL;

	m_nIndexCnt = 0; //저장/불러오기 할 때 저장이 되어야 함.

	memset(&m_ThumbNailCaret, 0, sizeof(m_ThumbNailCaret));

	//저장된 이미지 리스트의 크기
	m_nImageWidth = 0;	
	m_nImageHeight = 0;	

	m_bChangeScene = TRUE;

}

LmUISceneView::~LmUISceneView()
{
}

BOOL LmUISceneView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CScrollView::PreCreateWindow(cs);
}

// CLmUIThumbnailView 그리기

void LmUISceneView::OnDraw(CDC* /*pDC*/)
{
	
	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.

	int nListCnt = (INT)m_listThumb.GetCount();

	CClientDC dc(this);
	OnPrepareDC(&dc);

	int nCntCaret=0; //커서를 위해 사용되는 Count 변수, 액자들의 위치계산에도 사용됨.
	POSITION pPos = m_listThumb.GetHeadPosition();
	CRectPicture data;
	memset(&data, 0, sizeof(data));

	TRACE(L"m_nThumHeight : %d \t m_nThumWidth : %d \r\n",	m_nThumHeight, m_nThumWidth);

	while(pPos)
	{
		//현재 위치를 백업 받아둔다.
		POSITION nowPos = pPos;
		data = m_listThumb.GetNext(pPos);

		// 수열식 적용- 액자의 위치를 잡는다.
		data.m_ptLocation.x = m_nLeftSpace;
		data.m_ptLocation.y = m_nRowSpace + (m_nRowSpace + m_nThumHeight + 
											 m_nTitleTopSpace + m_nTitleHeight) * nCntCaret; 

		// 화면에 그리는 위치를 리스트에 저장해 둔다.
		m_listThumb.SetAt(nowPos, data);
		
		// 화면에 출력하면서 각 ThumbNail들의 위치가 정해지고
		// 그 값을 저장한다.
		//만일 출력하는 화면이 현재 캐럿일 경우 액자에 두께를 두껍게 찍는다.
		if(m_nCaret == data.nIndex)
			ThmDrawCaret(data.m_ptLocation);

		//m_imageList.Draw(&dc, data.nImgIndex, data.m_ptLocation ,ILD_NORMAL);
		CSize sizeImage(m_nThumWidth, m_nThumHeight);
		CRect rectLine(data.m_ptLocation.x-1, data.m_ptLocation.y-1,
						data.m_ptLocation.x + m_nThumWidth +1 ,
						data.m_ptLocation.y + m_nThumHeight +1);
		
		dc.SelectStockObject(NULL_BRUSH);
		CPen pen(PS_SOLID, 1, RGB(192,192,192));
		CPen *pOldPen = dc.SelectObject(&pen);
		dc.Rectangle(&rectLine);
		dc.SelectObject(pOldPen);

		//INT nDrawResult = m_imageList.DrawEx(&dc, data.nImgIndex, data.m_ptLocation, sizeImage
		//					,RGB(255,255,255), RGB(255,255,255), ILD_NORMAL);
		
		//m_imageList.Draw(&dc, data.nImgIndex, data.m_ptLocation ,ILD_NORMAL);

		CBitmap * pBitmap = MakeImage(&m_imageList, data.nImgIndex);

		CDC dcMem;
		dcMem.CreateCompatibleDC (&dc); 

		CBitmap* pBmpOld = dcMem.SelectObject (pBitmap);
		
		dc.StretchBlt(data.m_ptLocation.x, data.m_ptLocation.y, 
						m_nThumWidth, m_nThumHeight, &dcMem, 
						0, 0, m_nImageWidth, m_nImageHeight , SRCCOPY);
		
		dcMem.SelectObject (pBmpOld);

		delete pBitmap;

		INT nDrawResult =0;
		TRACE(L"data.nImgIndex : %d \t result : %d \t sizeImage.cx : %d \t sizeImage.cy : %d \r\n",
				data.nImgIndex, nDrawResult,  sizeImage.cx, sizeImage.cy);

		WCHAR strPrtSceneName[12];
		memset(strPrtSceneName, 0, sizeof(strPrtSceneName) );

		//유니코드가 8글자 넘는가?
		if(wcslen(data.strSceneName)>8)
		{
			//넘으면 8자까지 출력후 [...]을 추가

			CStringW str;
			str.Format(L"%s",data.strSceneName);

			//8자를 먼저 잘라낸 후 [...]을 붙인다
			CStringW strLeft_8 = str.Left(8);
			str.Format(L"%s...",strLeft_8.GetBuffer());
			wcsncpy_s(strPrtSceneName, str.GetBuffer(), 255);

		}
		else
		{
			//8자 이하면 그냥 출력
			CStringW str;
			str.Format(L"%s",data.strSceneName);
			wcsncpy_s(strPrtSceneName , str.GetBuffer(),255);
		}

		//화면에 글자를 출력하기 위한 Rect 영역 지정
		CPoint pointTop = data.m_ptLocation;
		pointTop.y += (m_nThumHeight+ m_nTitleTopSpace);

		CPoint pointBottom;
		pointBottom.x = data.m_ptLocation.x + m_nThumWidth;
		pointBottom.y = pointTop.y + m_nTitleHeight;

		CRect rectPrtSceneName(pointTop, pointBottom);
		int nStrLen = (int)wcslen(strPrtSceneName);

		//윈도우에 이미지의 이름을 출력
		dc.DrawText(strPrtSceneName, nStrLen,&rectPrtSceneName, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		nCntCaret++;
	}
}

void LmUISceneView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: 이 뷰의 전체 크기를 계산합니다.
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	//m_ImageList 초기화
	CRect rect;
	GetClientRect(&rect);
	

	m_nThumWidth = rect.Width() - (2 * m_nLeftSpace);
	m_nThumHeight =(int)( (rect.Width() *3/4.0) );

	////뷰 크기 백업
	m_sizeWin.cx = rect.Width();
	m_sizeWin.cy = rect.Height();

	ClientToScreen(&rect);
	//현재 View크기 백업 --> LmUISplitterWnd 클래스에서 크기 제한을 위해 참조.

	m_rectWin = rect;
	

	m_nImageWidth = 500 - (2 * m_nLeftSpace);
	m_nImageHeight =(int)( (rect.Width() *3/4.0) );

	InitSceneView();

	LoadThumbNail();
	m_isLoadComplete=TRUE;
}


// CLmUIThumbnailView 인쇄

BOOL LmUISceneView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void LmUISceneView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void LmUISceneView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CLmUIThumbnailView 진단

#ifdef _DEBUG
void LmUISceneView::AssertValid() const
{
	CScrollView::AssertValid();
}

void LmUISceneView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

#endif //_DEBUG


// CLmUIThumbnailView 메시지 처리기

HBITMAP LmUISceneView::LoadPicture( LPCTSTR szBMPFileName )
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

	return LoadedPicture(pPic);
}


HBITMAP LmUISceneView::LoadedPicture(IPicture* pPic)
{
	HBITMAP hPic = NULL;

	//핸들을 이용해서 hPic에 BITMAP에 그림을 등록한다.
	pPic->get_Handle((UINT*)&hPic);

	//가로, 세로 값을 정해둔다.
	long nWidth = m_nImageWidth;
	long nHeight = m_nImageHeight;

	//현재 그림의 가로, 세로 길이를 얻어온다.
	long mWid, mHei;
	pPic->get_Height(&mHei);
	pPic->get_Width(&mWid);

	//핸들을 이용해서 이미지 켄버스를 생성하고 복사하고 할당하기 전에 있던 이미지를 삭제한다.
	HBITMAP hPicRet = (HBITMAP)CopyImage(hPic, IMAGE_BITMAP, nWidth ,nHeight, LR_COPYDELETEORG);

	// 백그라운드와 테두리를 위한 브러시 생성
	HBRUSH hBrushBorder=::CreateSolidBrush(RGB(192, 192, 192));
	HBRUSH hBrushBk=::CreateSolidBrush(RGB(255, 255, 255));

	// 테두리 사이즈
	RECT rcBorder;
	rcBorder.left=rcBorder.top=0;
	rcBorder.right=m_nImageWidth;
	rcBorder.bottom=m_nImageHeight;

	// 비율 값 계산
	const float fRatio=(float)m_nImageHeight/m_nImageWidth;

	// 켄버스의 비율 값을 계산해서 가로가 큰지 세로가 큰지에 따라서 이미지를 조절한다.

	INT XDest, YDest, nDestWidth, nDestHeight;
	const float fImgRatio = (float)mHei/mWid;

	//이미지가 켄버스 보다 길죽한가?
	if(fImgRatio > fRatio)
	{
		//이미지가 켄버스 보다 길쭉하면 생성될 이미지의 가로 길이를 세로 길이의 비율에 맞춘다
		nDestWidth = (int)((float)m_nImageHeight*mWid/mHei);
		XDest = (m_nImageWidth-nDestWidth)/2;
		YDest = 0;
		nDestHeight = m_nImageHeight;
	}
	else
	{
		//이미지가 켄버스 보다 길쭉하면 생성될 이미지의 세로 길이를 가로 길이의 비율에 맞춘다
		XDest = 0;
		nDestWidth = m_nImageWidth;
		nDestHeight = (int)((float)m_nImageWidth * mHei / mWid );
		YDest = (m_nImageHeight - nDestHeight)/2;
	}

	CClientDC cdc(NULL);
	HDC hDC = ::CreateCompatibleDC(cdc.m_hDC); //메모리 DC 생성
	HBITMAP bm = CreateCompatibleBitmap(cdc.m_hDC, m_nImageWidth, m_nImageHeight); //메모리 비트맵 생성
	HBITMAP pOldBitmapImage = (HBITMAP)SelectObject(hDC, bm);	//메모리 DC가 비트맵 객체를 선택한다.
	//반환 값은 대체된 객체의 핸들이다.즉, bm 비트메모리가 선택된 hDC 메모리 DC의 핸들


	//메모리 bitmap에서 그림 -------
	// 백그라운드 그리기
	::FillRect(hDC, &rcBorder, hBrushBk);
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
	//hdcDst.StretchBlt(XDest+1, YDest+1, nDestWidth-2, nDestHeight-2, &hdcSrc, 0,0, m_nImageWidth, m_nImageHeight , SRCCOPY);
	//Scene에 맞게 수정됨
	hdcDst.StretchBlt(0,0, m_nImageWidth, m_nImageHeight, &hdcSrc, 0,0, m_nImageWidth, m_nImageHeight , SRCCOPY);

	pOldBitmapImage = (HBITMAP)SelectObject(hdcDst.m_hDC, bm);

	DeleteDC(hDC);
	DeleteObject(hBrushBorder);
	DeleteObject(hBrushBk);

	DeleteObject(pPic);
	DeleteObject(hPicRet);


	return pOldBitmapImage;
}

// 새로운 개념이 적용된 방식 
int LmUISceneView::AddThumbNail(LPCTSTR szBMPFileName, LPCTSTR szSceneName)
{
	//비트맵을 읽어온다.
	HBITMAP bitm = LoadPicture(szBMPFileName);

	//입력 받은 BMP파일이 없는가? 그렇다면 return 하라!
	if(bitm == NULL)
		return -1;

	CBitmap* pImage = new CBitmap();
	pImage->Attach(bitm); //bitmap 핸들을 받아온다.

	int nImgEnd = m_imageList.Add(pImage, RGB(0,0,0)); //이미지 리스트에 썸네일 Image를 삽입한다.

	//m_listThumb에 썸네일 정보를 등록한다.

	int nCntNode = (int)m_listThumb.GetCount();
	CRectPicture NewNode;
	NewNode.m_ptLocation.x = 0;
	NewNode.m_ptLocation.y = 0;
	NewNode.nIndex = m_nIndexCnt++; //추가 후 증가
	NewNode.nImgIndex = nImgEnd;
	wcsncpy_s(NewNode.strSceneName,szSceneName, MAX_STR);

	m_listThumb.AddTail(NewNode);

	delete pImage;
	onRefreshScroll();
	return (m_nIndexCnt-1);
}

VOID LmUISceneView::SetErrorMsg(LPCTSTR lpszErrMsg)
{
	wcsncpy_s(m_szErrMsg, lpszErrMsg, MAXIMUM_ERR_MSG);	
}

void LmUISceneView::onRefreshScroll()
{
	//InvalidateRect(NULL);

	if(!m_listThumb.GetCount())
		return;

	//CRectPicture data = m_listThumb.GetTail();
	UINT nDataCnt = (UINT)m_listThumb.GetCount();
	UINT nLastYData = m_nRowSpace + (m_nRowSpace + m_nThumHeight + 
					m_nTitleTopSpace + m_nTitleHeight) * (nDataCnt-1); 
	CRect rect;
	GetClientRect(&rect);
	//10을 빼는 이유는 전체 스크린 좌표에서 최고 크기를 생각했기 때문임.
	CSize sizeTotal( rect.Width() ,nLastYData + m_nThumHeight + m_nTitleTopSpace + m_nTitleHeight + 30);
	CSize sizePage(rect.Width()-30, rect.Height()-30);
	CSize sizeLine(50,50);

	SetScrollSizes(MM_TEXT,sizeTotal,sizePage, sizeLine);
}

void LmUISceneView::OnSize(UINT nType, int cx, int cy)
{

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//TRACE(L"OnSize Message \r\n");
	// 재진입..(함수가 호출된 상태에서 다시 돌아오는 경우)을 막기 위한 코딩
	static bool bInOnSize = false;
	if(bInOnSize == true)
		return;

	bInOnSize = true;
	
	TRACE(L"OnSize Message no reCall \r\n");
	
	TRACE(L"CX Size : %d\r\n", cx);
	
	if(m_isLoadComplete)
	{
		
		//뷰가 없을때 새로 이미지를 그릴려 하면(m_ImageList가 NULL임) 에러가 난다.
		if(!cx)
			return;

		if(m_sizeWin.cx != cx)
		{
			
			//문제는 스크롤이 생기면 한번더 호출되는데 화면을 전체 화면모드로 하면
			//다시 호출된다.
			
				//뷰 크기 백업
				m_sizeWin.cx = cx;
				m_sizeWin.cy = cy;

				m_nThumWidth = cx - (2 * m_nLeftSpace);
				m_nThumHeight = (int)( cx * 3/4.0);

		}

	}
	//현재 View크기 백업 --> LmUISplitterWnd 클래스에서 크기 제한을 위해 참조.
	CRect rect;
	GetClientRect(&rect);	
	ClientToScreen(&rect);
	m_rectWin = rect;

	onRefreshScroll();

	//수평 스크롤 바는 감춘다.
	ShowScrollBar(SB_HORZ, FALSE);
	bInOnSize = false;
	CScrollView::OnSize(nType, cx, cy);

}

void LmUISceneView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	POSITION pPos = m_listThumb.GetHeadPosition();
	CRectPicture data;

	//데이터가 없을때는 그냥 리턴~
	if(!m_listThumb.GetCount())
		return;

	//Wizard가 실행될 때는 Scene이 변경되어서는 안된다.
	if(!m_bChangeScene)
		return; 

	while(pPos)
	{
		data = m_listThumb.GetNext(pPos);
		CPoint pointBottom(data.m_ptLocation.x + m_nThumWidth , 
			data.m_ptLocation.y + m_nThumHeight + m_nTitleTopSpace + m_nTitleHeight);
		CRect rectArea(data.m_ptLocation , pointBottom);

		//클릭한 위치가 그림의 액자 영역에 들어갔다면
		//그 그림액자를 두껍게 그려준다.
		CClientDC dc(this);
		OnPrepareDC(&dc);

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

			//현재 커서에 있는 정보를 저장한다.
			m_ThumbNailCaret.m_ptLocation = data.m_ptLocation;
			m_ThumbNailCaret.nImgIndex = data.nImgIndex;
			m_ThumbNailCaret.nIndex = data.nIndex;
			wcsncpy_s(m_ThumbNailCaret.strSceneName, data.strSceneName, sizeof(data.strSceneName));

			// [동인] 현재씬을 선택한다.
			TheCartoonMaker->SelectNowScene(data.strSceneName);	
			TheCartoonMaker->SetStateOfUI(SCENE_CHANGED);

//			data.strSceneName
			break;
		}
	}

	CScrollView::OnLButtonDown(nFlags, point);
}

void LmUISceneView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int SCrollDirect=0;

	//현재 등록된 씬이 하나도 없거나 씬의 변경이 금지(Wizard실행시) 되었을때는 바로 리턴한다. 
	if(!m_listThumb.GetCount() || !m_bChangeScene)
	{
		CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}

	CClientDC dc(this);
	OnPrepareDC(&dc);  //좌표 매핑
	
	//Scene을 지우고 삽입할때 제대로 할 수 동작할 수 있도록 고쳤음
	
	POSITION pos = m_listThumb.GetHeadPosition();
	POSITION posHead = m_listThumb.GetHeadPosition();
	POSITION posTail = m_listThumb.GetTailPosition();
	POSITION posFinder = m_listThumb.Find(m_ThumbNailCaret);


	if(nChar==VK_UP)
	{
		//먼저 이동해 본다.
		CRectPicture PicCaret = m_listThumb.GetPrev(posFinder);
		//TRACE(L"VK_UP pre : %d\r\n", PicCaret.nIndex);

		//Position의 위치가 Head인가?? Head경우 posFinder는 0을 가진다!
		if(posFinder)
		{
			PicCaret = m_listThumb.GetPrev(posFinder);	
			//TRACE(L"VK_UP will Go : %d\r\n", PicCaret.nIndex);
			SetThumbNailCaret(PicCaret.nIndex);
			SCrollDirect=0; //스크롤 방향 위로
		}
		else 
		//헤더면 그냥 움직이지 않는다.
		{
			CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
			return;
		}
	}
	else if(nChar == VK_DOWN)
	{
		CRectPicture PicCaret = m_listThumb.GetNext(posFinder);
		//TRACE(L"VK_DOWN pre : %d\r\n", PicCaret.nIndex);
		//Position의 위치가 Tail인가?? Tail경우 posFinder는 0을 가진다!
		if(posFinder)
		{
			PicCaret = m_listThumb.GetNext(posFinder);		
			//TRACE(L"VK_DOWN Go : %d\r\n", PicCaret.nIndex);

			SetThumbNailCaret(PicCaret.nIndex);
			SCrollDirect = 1;  //스크롤 방향 아래로
		}
		else
		{
			CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
			return;
		}
	}

	//현재 변경된 Caret의 정보를 얻어온다.
	CRectPicture data;
	GetThumbNailCaret(data);

	CPoint ptDataMapping = data.m_ptLocation;

	//스크롤 검사.. 만일 화면 밖일 경우 스크롤 한다.
	dc.LPtoDP(&ptDataMapping);
	CRect rectView;
	GetClientRect(&rectView);

	//스크롤 검사를 하기 위해 아래쪽 하단의 액자 좌표값을 계산해 낸다. Device Client 좌표임.
	CPoint ptBottomMapping;
	ptBottomMapping.x = ptDataMapping.x;
	ptBottomMapping.y = ptDataMapping.y + m_nThumHeight + m_nTitleHeight + m_nTitleTopSpace;

	//화면 밖인가?
	if(!rectView.PtInRect(ptBottomMapping) || !rectView.PtInRect(ptDataMapping))
	{
		//스크롤 방향이 아래쪽인가?
		if(SCrollDirect)
		{
			CPoint ptBottom;
			ptBottom.x = 0;
			//ptBottom.y = data.m_ptLocation.y + m_nThumHeight + m_nTitleHeight + m_nTitleTopSpace;
			ptBottom.y = GetScrollPos(SB_VERT) + m_nThumHeight + m_nTitleHeight + m_nTitleTopSpace + m_nRowSpace;
			ScrollToPosition(ptBottom);
		}
		else
			//그렇지 않으면 스크롤 방향이 위쪽인가?
		{
			CPoint ptTop;
			ptTop.x = 0;
			ptTop.y = data.m_ptLocation.y - m_nRowSpace;
			ScrollToPosition(ptTop);
		}

	}

	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void LmUISceneView::ThmDrawCaret( CPoint leftTop )
{
	CClientDC dc(this);
	OnPrepareDC(&dc);

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


void LmUISceneView::LoadThumbNail()
{
	//HIMAGELIST hScreens = ImageList_Create(m_nThumWidth, m_nThumHeight, ILC_COLOR32, 0, 1);
	//m_imageList.Attach(hScreens);

	////////////////////////////////////////////////////////////////////////////
	//// 테스트용..
	////////////////////////////////////////////////////////////////////////////
	//TRACE(L"LoadThumbNail() -- start --\r\n");

	//CString strFolder =  g_strFilePath;
	//INT FirstSlash = strFolder.ReverseFind(L'\\');
	//strFolder = strFolder.Left(FirstSlash);
	//strFolder+=L"\\Data";
	//strFolder+=L"\\*.bmp";

	//CFileFind Finder;
	//BOOL bWorking = Finder.FindFile(strFolder.GetBuffer());
	//while(bWorking)
	//{
	//	bWorking
	//		= Finder.FindNextFile();
	//	InsertThumbNail(Finder.GetFilePath(),Finder.GetFileTitle());
	//	
	//	OutputDebugString(Finder.GetFilePath());
	//	TRACE(L"\r\n");
	//}
	//TRACE(L"LoadThumbNail() -- end ----------------------- \r\n");
	////////////////////////////////////////////////////////////////////////////


}
BOOL LmUISceneView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(zDelta<0)
		SendMessage(WM_VSCROLL, MAKELONG(SB_PAGEDOWN,0), 0);
	else
		SendMessage(WM_VSCROLL, MAKELONG(SB_PAGEUP,0), 0);


	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}

void LmUISceneView::OnSizing(UINT fwSide, LPRECT pRect)
{
	CScrollView::OnSizing(fwSide, pRect);
}

BOOL LmUISceneView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//if(pMsg->message == LM_CHANGE_SCENEVIEW)	
	//	onLmChangeSize();

	return CScrollView::PreTranslateMessage(pMsg);
}

//void LmUISceneView::onLmChangeSize()
//{
//	if(m_isLoadComplete)
//	{
//		//뷰가 없을때 새로 이미지를 그릴려 하면(m_ImageList가 NULL임) 에러가 난다.
//		CRect rectView;
//		GetClientRect(&rectView);
//
//		if(!rectView.Width())
//			return;
//
//		//윈도우 크기의 변화가 있다면 1번만 실행하도록 하기 위해..
//		//분할 뷰라 보니.. WM_SIZE가 거의 5번 정도는 호출되고 이 함수는 쓰레드로 처리된다. 
//		//그래서 데이터가 꼬임.. 그런 현상을 막기 위해 아래와 같이 코딩함.
//
//
//		if(m_sizeWin.cx != rectView.Width())
//		{
//
//			//뷰 크기 백업
//			m_sizeWin.cx = rectView.Width();
//			m_sizeWin.cy = rectView.Height();
//
//			m_nThumWidth = m_sizeWin.cx - (2 * m_nLeftSpace);
//			m_nThumHeight = (int)( m_sizeWin.cx * 3/4.0);
//
//			//m_imageList.Detach();
//			m_imageList.DeleteImageList();
//			m_listThumb.RemoveAll();
//
//			TRACE(L"OnSize Message \r\n");
//			LoadThumbNail();
//
//		}
//
//	}
//
//}

// 현재 커서를 지정해주는 함수
// OnLButtonDown함수에서는 이 함수가 사용되지 않음!! - 속도를 위한 조치임.
void LmUISceneView::SetThumbNailCaret( UINT nCaret )
{
	POSITION pPos = m_listThumb.GetHeadPosition();
	CRectPicture data;

	//데이터가 없을때는 그냥 리턴~
	if(!m_listThumb.GetCount())
		return;

	UINT nCntCaret = 0;

	while(pPos)
	{
		data = m_listThumb.GetNext(pPos);

		data.m_ptLocation.x = m_nLeftSpace;
		data.m_ptLocation.y = m_nRowSpace + (m_nRowSpace + m_nThumHeight + 
			m_nTitleTopSpace + m_nTitleHeight) * nCntCaret; 

		CPoint pointBottom(data.m_ptLocation.x + m_nThumWidth , 
			data.m_ptLocation.y + m_nThumHeight + m_nTitleTopSpace + m_nTitleHeight);
		CRect rectArea(data.m_ptLocation , pointBottom);

		//클릭한 위치가 그림의 액자 영역에 들어갔다면
		//그 그림액자를 두껍게 그려준다.
		CClientDC dc(this);
		OnPrepareDC(&dc);

		if(nCaret == data.nIndex)
		{
			m_nCaret = data.nIndex;

			CRect rectMapping;
			rectMapping=m_rectPreCaret;
			dc.LPtoDP(&rectMapping);

			InvalidateRect(&rectMapping);
			ThmDrawCaret(data.m_ptLocation);

			//현재 커서에 있는 정보를 저장한다.
			m_ThumbNailCaret.m_ptLocation = data.m_ptLocation;
			m_ThumbNailCaret.nImgIndex = data.nImgIndex;
			m_ThumbNailCaret.nIndex = data.nIndex;
			wcsncpy_s(m_ThumbNailCaret.strSceneName, data.strSceneName, wcslen(data.strSceneName));

			break;
		}
		nCntCaret++;
	}
	
	// [동인6] 현재씬을 선택한다.
	TheCartoonMaker->SelectNowScene(data.strSceneName);	
	TheCartoonMaker->SetStateOfUI(SCENE_CHANGED);
}

//현재 캐럿의 정보를 반환해 준다.
void LmUISceneView::GetThumbNailCaret(CRectPicture & getCaretData)
{
	getCaretData.m_ptLocation = m_ThumbNailCaret.m_ptLocation;
	getCaretData.nImgIndex = m_ThumbNailCaret.nImgIndex;
	getCaretData.nIndex = m_ThumbNailCaret.nIndex;
	wcsncpy_s(getCaretData.strSceneName, m_ThumbNailCaret.strSceneName, wcslen(m_ThumbNailCaret.strSceneName));

	return;
}

INT LmUISceneView::GetThumbNailCaret()
{
	return m_ThumbNailCaret.nIndex;
}
//입력 받은 ThumbNail index를 검색하여 삭제한다.
// - 좀더 간단하게 Find 함수를 사용하여 구현할 것!
// nIndex 값을 검색하도록 하자!
// GetThumbNail() 함수를 사용할 것!
// Require
void LmUISceneView::RemoveThumbNail( UINT ThumIndex )
{
	POSITION pos = m_listThumb.GetHeadPosition();
	POSITION Finder = NULL;
	POSITION FinderAfter = NULL;
	
	CRectPicture data;
	memset(&data, 0, sizeof(data));
	//GetThumbNailCaret(data);
	data.nIndex = ThumIndex;

	Finder = m_listThumb.Find(data);
	
	//찾는 Thumbnail이 없으면 리턴한다.
	if(!Finder)
		return;

	FinderAfter = Finder;
	
	data = m_listThumb.GetNext(FinderAfter);
	
	//Caret에 있는 내용을 지운다.
	m_listThumb.RemoveAt(Finder);
	m_imageList.Remove(data.nImgIndex);

	INT nImageIndexDeleted = data.nImgIndex;

	//Caret을 설정한다.
	//다음 링크의 위치에 NULL이 아니고 Node가 존재하는가?
	CRectPicture newCaret;
	if(FinderAfter)
		newCaret= m_listThumb.GetAt(FinderAfter);
	//다음 링크의 위치가 NULL인 경우
	else
	{
		//남아있는 노드의 갯수를 파악한다.
		//남아 있는 노드의 갯수가 1 이상이면 마지막 노드에 Caret을 위치 시킨다.
		UINT nNodeCnt = (UINT)m_listThumb.GetCount();
		if(nNodeCnt)
			newCaret = m_listThumb.GetTail();
		else
		// else --> Thumb가 있다가 다시 사라져서 Node수가 없어지면 알아서 추가를 시킬때 Caret이 추가 된다.
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			pMainFrame->EnableMovieClipMenu(FALSE);
		}
	}
	SetThumbNailCaret(newCaret.nIndex);
		
	POSITION nowPos = m_listThumb.GetHeadPosition();
	while(nowPos)
	{
		CRectPicture DataChanging = m_listThumb.GetAt(nowPos);
		
		//ImageList 의 Index를 맞추기 위해 지워진 이미지 Index보다 큰 이미지 Index를 모두 1씩 빼준다.
		if( nImageIndexDeleted < DataChanging.nImgIndex)
			--DataChanging.nImgIndex;
		m_listThumb.SetAt(nowPos, DataChanging);
		m_listThumb.GetNext(nowPos);

	}
	
	//View의 크기를 갱신해 준다.
	onRefreshScroll();
	return;
}

int LmUISceneView::InsertThumbNail( LPCTSTR szBMPFileName, LPCTSTR szSceneName )
{
	//비트맵을 읽어온다.
	HBITMAP bitm = LoadPicture(szBMPFileName);

	//입력 받은 BMP파일이 없는가? 그렇다면 return 하라!
	if(bitm == NULL)
		return -1;

	CBitmap* pImage = new CBitmap();
	pImage->Attach(bitm); //bitmap 핸들을 받아온다.

	int nImgEnd = m_imageList.Add(pImage, RGB(0,0,0)); //이미지 리스트에 썸네일 Image를 삽입한다.

	//m_listThumb에 썸네일 정보를 등록한다.

	int nCntNode = (int)m_listThumb.GetCount();
	
	CRectPicture NewNode;

	//그림 액자의 위치는 onDraw에서 정해진다.
	//2007. 12. 06. 변경
	NewNode.m_ptLocation.x = 0;
	NewNode.m_ptLocation.y = 0; 

	NewNode.nIndex = m_nIndexCnt++; //추가 후 증가
	NewNode.nImgIndex = nImgEnd;
	wcsncpy_s(NewNode.strSceneName,szSceneName, MAX_STR);

	//ThumbNail이 하나도 없는가? 그럴 경우 추가한다.
	if(!nCntNode)
		m_listThumb.AddTail(NewNode);
	//ThumbNail이 있을 경우 Caret의 바로 밑에 ThumbNail을 삽입한다.
	else
	{
		POSITION pos = m_listThumb.GetHeadPosition();
		while(pos)
		{
			CRectPicture data = m_listThumb.GetAt(pos);
			 
			//현재 커서의 위치를 찾아낸 후 그 자리에 삽입한다.
			if(data.nIndex == m_nCaret)
			{
				m_listThumb.InsertAfter(pos,NewNode);
				break;
			}
			m_listThumb.GetNext(pos);
		}
	}
	delete pImage;
	onRefreshScroll();
	return (m_nIndexCnt-1);
}

int LmUISceneView::ReplaceThumbNail( UINT ThumIndex, IPicture *pPic)
{
	//비트맵을 읽어온다.
	HBITMAP bitm = LoadedPicture(pPic);

	//입력 받은 BMP파일이 없는가? 그렇다면 return 하라!
	if(bitm == NULL)
		return -1;

	CBitmap* pImage = new CBitmap();
	pImage->Attach(bitm); //bitmap 핸들을 받아온다.
	
	POSITION pos = m_listThumb.GetHeadPosition();
	while(pos)
	{
		CRectPicture data = m_listThumb.GetAt(pos);

		//현재 커서의 위치를 찾아낸 후 그 자리에 그림을 대체한다.
		if(data.nIndex == ThumIndex)
		{
			m_imageList.Replace(data.nImgIndex, pImage, RGB(0,0,0)); //이미지 리스트에 썸네일 Image를 삽입한다.		
			break;
		}
		m_listThumb.GetNext(pos);
	}

	InvalidateCaret();
	
	delete pImage;
	return 0;
}

void LmUISceneView::InvalidateCaret()
{
	CClientDC dc(this);
	CRect rectMapping;
	rectMapping=m_rectPreCaret;
	dc.LPtoDP(&rectMapping);

	InvalidateRect(&rectMapping);
}

VOID LmUISceneView::EnableChangeScene( BOOL bEnable )
{
	m_bChangeScene = bEnable;
}

BOOL LmUISceneView::GetChangeSceneSw()
{
	return m_bChangeScene;
}

INT LmUISceneView::CopyThumbNail()
{
	//임시 영역 초기화
	m_vectorMemThumb.clear();	
	m_imageListMem.DeleteImageList();

	m_imageListMem.Create(m_nImageWidth, m_nImageHeight, ILC_COLOR32, 0, 1);

	//현재 Caret의 정보를 얻어온다. 
	CRectPicture rCaretData;
	GetThumbNailCaret(rCaretData);
	
	//현재 Caret의 정보를 백터에 저장한다. 
	m_vectorMemThumb.push_back(rCaretData);
	
	//현재 Caret이 가진 이미지 정보도 저장해 둔다. 
	CopyImgListTOMemList(rCaretData);

	TheCartoonMaker->CopyScene();

	return 0;
}

VOID LmUISceneView::InitSceneView()
{
	m_listThumb.RemoveAll();
	m_imageList.DeleteImageList();

	m_imageList.Create(m_nImageWidth, m_nImageHeight, ILC_COLOR32, 0, 1);

	//ThumbNail 임시 공간 초기화
	m_vectorMemThumb.clear();	
	m_imageListMem.DeleteImageList();

	m_imageListMem.Create(m_nImageWidth, m_nImageHeight, ILC_COLOR32, 0, 1);

}

INT LmUISceneView::PasteThumbNail()
{
	INT nVectorSize = (INT)m_vectorMemThumb.size();
	
	//메모리에 저장된 내용이 없으면 리턴
	if(!nVectorSize)
		return -1;
	
	//그림을 대입한다. 
	//INT nImageCount = m_imageList.GetImageCount();
	//m_imageList.SetImageCount(++nImageCount);
	//m_imageList.Copy(nImageCount-1, &m_imageListMem, 0, ILCF_SWAP);

	INT nNewImageIndex = CopyMemListTOImgList();

	//m_listThumb에 썸네일 정보를 등록한다.
	int nCntNode = (int)m_listThumb.GetCount();
	CRectPicture NewNode;

	//그림 액자의 위치는 onDraw에서 정해진다.
	//2007. 12. 06. 변경
	NewNode.m_ptLocation.x = 0;
	NewNode.m_ptLocation.y = 0; 

	NewNode.nIndex = m_nIndexCnt++; //추가 후 증가
	NewNode.nImgIndex = nNewImageIndex;
	wcsncpy_s(NewNode.strSceneName, m_vectorMemThumb[0].strSceneName, MAX_STR);

	INT nIsThereSameName = IsThereSceneName(m_vectorMemThumb[0].strSceneName);
	CString strSceneName = m_vectorMemThumb[0].strSceneName;
	INT nCnt = 0;

	while(nIsThereSameName)
	{
		strSceneName.Format(L"%s(%d)",m_vectorMemThumb[0].strSceneName, ++nCnt);
		nIsThereSameName = IsThereSceneName(strSceneName.GetBuffer(0));
	}
	
	wcsncpy_s(NewNode.strSceneName, strSceneName.GetBuffer(0), MAX_STR);

	
	//ThumbNail이 하나도 없는가? 그럴 경우 추가한다.
	if(!nCntNode)
		m_listThumb.AddTail(NewNode);
	//ThumbNail이 있을 경우 Caret의 바로 밑에 ThumbNail을 삽입한다.
	else
	{
		POSITION pos = m_listThumb.GetHeadPosition();
		while(pos)
		{
			CRectPicture data = m_listThumb.GetAt(pos);

			//현재 커서의 위치를 찾아낸 후 그 자리에 삽입한다.
			if(data.nIndex == m_nCaret)
			{
				m_listThumb.InsertAfter(pos,NewNode);
				break;
			}
			m_listThumb.GetNext(pos);
		}
	}
	
	onRefreshScroll();
	Invalidate();

	TheCartoonMaker->PasteScene(NewNode.strSceneName);

	return (m_nIndexCnt-1);

}

INT LmUISceneView::CutThumbNail()
{
	//임시 영역 초기화
	m_vectorMemThumb.clear();	
	m_imageListMem.DeleteImageList();

	m_imageListMem.Create(m_nImageWidth, m_nImageHeight, ILC_COLOR32, 0, 1);

	//현재 Caret의 정보를 얻어온다. 
	CRectPicture rCaretData;
	GetThumbNailCaret(rCaretData);

	//현재 Caret의 정보를 백터에 저장한다. 
	m_vectorMemThumb.push_back(rCaretData);

	//현재 Caret이 가진 이미지 정보도 저장해 둔다. 
	CopyImgListTOMemList(rCaretData);

	//이번에도 순서 때문에 삽질했다. ㅡㅡ^
	//모든 SceneView의 TheCartoonMaker의 호출 시기는 ThumbNailCaret이 변경되기 전에 해야만 한다!
	TheCartoonMaker->CutScene();

	RemoveThumbNail(rCaretData.nIndex);
	Invalidate();

	return 0;
}

CBitmap* LmUISceneView::MakeImage(CImageList * pImageList, INT nImageIndex)
{
	CClientDC dc(NULL);
	CDC dcMem;
	dcMem.CreateCompatibleDC (&dc); 

	CBitmap *pBmp = new CBitmap();
	pBmp->CreateCompatibleBitmap (&dc, m_nImageWidth, m_nImageHeight);

	CBitmap* pBmpOld = dcMem.SelectObject (pBmp);
	pImageList->Draw(&dcMem, nImageIndex, CPoint (0, 0), ILD_NORMAL);
	dcMem.SelectObject (pBmpOld);

	return pBmp;
}

void LmUISceneView::CopyImgListTOMemList( CRectPicture &rCaretData )
{
	// 이미지 리스트로 부터 이미지 정보를 얻어온다.	
	
	CBitmap *pBmp = MakeImage(&m_imageList, rCaretData.nImgIndex );

	m_imageListMem.Add(pBmp,RGB(0,0,0));
	delete pBmp;
}

INT LmUISceneView::CopyMemListTOImgList()
{
	// 이미지 리스트로 부터 이미지 정보를 얻어온다.	

	CBitmap *pBmp = MakeImage(&m_imageListMem, 0);

	INT nNewImageIndex = m_imageList.Add(pBmp,RGB(0,0,0));
	
	delete pBmp;

	return nNewImageIndex;
	
}

INT LmUISceneView::IsThereSceneName( LPCWSTR pcwsSceneName )
{
	POSITION NameFinder = m_listThumb.GetHeadPosition();
	while(NameFinder)
	{
		CRectPicture data = m_listThumb.GetAt(NameFinder);

		//현재 커서의 위치를 찾아낸 후 그 자리에 삽입한다.
		if(!wcscmp(data.strSceneName, pcwsSceneName))
			return 1;

		m_listThumb.GetNext(NameFinder);

	}
	return 0;
}

INT LmUISceneView::SaveSceneView( CFile & File )
{
	UINT uFilePos = (UINT)File.GetPosition();
	TheCartoonMaker->SetSceneViewStart(uFilePos);

	//////////////////////////////////////////////////////////////////////////
	//CRectPicture의 CList Template을 저장한다.
	//////////////////////////////////////////////////////////////////////////

	//List의 갯수를 저장한다.
	UINT nListCount = (UINT)m_listThumb.GetCount();
	File.Write(&nListCount , sizeof(nListCount));

	POSITION pos = m_listThumb.GetHeadPosition();
	while(pos)
	{
		//각 클래스 내부에 있는 데이터를 하나씩 순서대로 저장한다.
		CRectPicture data = m_listThumb.GetNext(pos);
		File.Write(&(data), sizeof(data) );
		//File.Write(&(data.m_ptLocation.x), sizeof(LONG) );
		//File.Write(&(data.m_ptLocation.y), sizeof(LONG) );
		//File.Write(&(data.nImgIndex), sizeof(INT));
		//File.Write(&(data.nIndex), sizeof(UINT));
		//File.Write(&(data.strSceneName), sizeof(data.strSceneName));
	}

	//////////////////////////////////////////////////////////////////////////
	// CImageList를 저장한다.
	//////////////////////////////////////////////////////////////////////////

	//m_ImageList는 CImageList에서 지원해 주는 CArchive를 이용해서 파일로 저장한 후 그 파일을 다시 저장한다. 	
	CString strImageListFile;
	CString strResDirectory;
	GetResourceDirectory(strResDirectory);
	strImageListFile.Format(L"%s\\imageFile.dat", strResDirectory.GetBuffer(0));

	CFile ImageListFile( strImageListFile.GetBuffer(0), CFile::modeCreate | CFile::modeWrite );
	CArchive ar(&ImageListFile, CArchive::store);

	// Store the image list in the archive.
	m_imageList.Write(&ar);

	// 아카이브를 닫아준다.
	ar.Close();

	// 파일을 닫아준다.
	ImageListFile.Close();

	CFile ReloadImageList( strImageListFile.GetBuffer(0),CFile::modeRead );
	UINT uImageListSize = (UINT)ReloadImageList.GetLength();
	BYTE* byteBuffer = new BYTE[uImageListSize];

	ReloadImageList.Read(byteBuffer , uImageListSize);
	File.Write(byteBuffer, uImageListSize);

	ReloadImageList.Close();

	uFilePos = (UINT)File.GetPosition();
	TheCartoonMaker->SetSceneViewEnd(uFilePos);

	//메모리 해제
	delete[] byteBuffer;
	return 0;
}

INT LmUISceneView::LoadSceneView( CFile & File )
{
	UINT uFilePosStart = TheCartoonMaker->GetSceneViewStart();
	File.Seek(uFilePosStart, CFile::begin);

	//////////////////////////////////////////////////////////////////////////
	//CRectPicture의 CList Template을 로딩한다.
	//////////////////////////////////////////////////////////////////////////

	//썸네일 리스트 초기화
	m_listThumb.RemoveAll();
	Invalidate();
	onRefreshScroll();

	//List의 갯수를 읽어온다.
	UINT nListCount = 0;
	File.Read(&nListCount , sizeof(nListCount));

	POSITION pos = m_listThumb.GetHeadPosition();
	UINT nCount = 0;
	for( nCount = 0; nCount < nListCount ; nCount++ )
	{
		//각 클래스 내부에 있는 데이터를 하나씩 순서대로 저장한다.
		CRectPicture data;
		File.Read(&(data), sizeof(data) );
		
		m_listThumb.AddTail(data);
		//File.Read(&(data.m_ptLocation.x), sizeof(LONG) );
		//File.Read(&(data.m_ptLocation.y), sizeof(LONG) );
		//File.Read(&(data.nImgIndex), sizeof(INT));
		//File.Read(&(data.nIndex), sizeof(UINT));
		//File.Read(&(data.strSceneName), sizeof(data.strSceneName));
	}

	//////////////////////////////////////////////////////////////////////////
	// CImageList를 로딩한다.
	//////////////////////////////////////////////////////////////////////////
	CString strImageListFile;
	CString strResDirectory;
	GetResourceDirectory(strResDirectory);
	strImageListFile.Format(L"%s\\imageFile.dat",strResDirectory.GetBuffer(0));

	UINT uFilePosEnd = TheCartoonMaker->GetSceneViewEnd();
	UINT uImageListSize = uFilePosEnd - uFilePosStart + 1;
	
	//이미지 리스트를 읽어 들이기 위한 버퍼 생성
	BYTE* byteBuffer = new BYTE[uImageListSize];
	File.Read(byteBuffer, uImageListSize);

	//저장된 이미지 리스트를 CArchive로 읽어 들이기 위해 imageFile.dat라는 파일명으로 기록한다.
	CFile ReloadImageList(strImageListFile.GetBuffer(0), CFile::modeCreate | CFile::modeWrite );
	ReloadImageList.Write(byteBuffer , uImageListSize);
	ReloadImageList.Close();

	//저장된 imageFile.dat를 CArchive로 읽어서 로딩한다.
	CFile ImageListFile(strImageListFile.GetBuffer(0), CFile::modeRead);
	CArchive ar(&ImageListFile, CArchive::load);

	// Store the image list in the archive.
	m_imageList.DeleteImageList();
	m_imageList.Read(&ar);

	// 아카이브를 닫아준다.
	ar.Close();

	// 파일을 닫아준다.
	ImageListFile.Close();

	//메모리 해제
	delete[] byteBuffer;

	Invalidate();
	onRefreshScroll();

	return 0;
}