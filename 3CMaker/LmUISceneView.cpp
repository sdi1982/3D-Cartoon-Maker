//LmUIThumbnailView.cpp : CLmUIThumbnailView Ŭ������ ����
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
	// ǥ�� �μ� ����Դϴ�.
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

// CLmUIThumbnailView ����/�Ҹ�

LmUISceneView::LmUISceneView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	m_nThumWidth = 75;
	m_nThumHeight = 75;
	m_nRowSpace = 20;
	m_nLeftSpace = 20;
	m_nTitleTopSpace = 4;
	m_nTitleHeight = 20;
	m_nCaret = 0;
	m_isLoadComplete = FALSE;
	m_pThreadLoadPicture=NULL;

	m_nIndexCnt = 0; //����/�ҷ����� �� �� ������ �Ǿ�� ��.

	memset(&m_ThumbNailCaret, 0, sizeof(m_ThumbNailCaret));

	//����� �̹��� ����Ʈ�� ũ��
	m_nImageWidth = 0;	
	m_nImageHeight = 0;	

	m_bChangeScene = TRUE;

}

LmUISceneView::~LmUISceneView()
{
}

BOOL LmUISceneView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CScrollView::PreCreateWindow(cs);
}

// CLmUIThumbnailView �׸���

void LmUISceneView::OnDraw(CDC* /*pDC*/)
{
	
	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.

	int nListCnt = (INT)m_listThumb.GetCount();

	CClientDC dc(this);
	OnPrepareDC(&dc);

	int nCntCaret=0; //Ŀ���� ���� ���Ǵ� Count ����, ���ڵ��� ��ġ��꿡�� ����.
	POSITION pPos = m_listThumb.GetHeadPosition();
	CRectPicture data;
	memset(&data, 0, sizeof(data));

	TRACE(L"m_nThumHeight : %d \t m_nThumWidth : %d \r\n",	m_nThumHeight, m_nThumWidth);

	while(pPos)
	{
		//���� ��ġ�� ��� �޾Ƶд�.
		POSITION nowPos = pPos;
		data = m_listThumb.GetNext(pPos);

		// ������ ����- ������ ��ġ�� ��´�.
		data.m_ptLocation.x = m_nLeftSpace;
		data.m_ptLocation.y = m_nRowSpace + (m_nRowSpace + m_nThumHeight + 
											 m_nTitleTopSpace + m_nTitleHeight) * nCntCaret; 

		// ȭ�鿡 �׸��� ��ġ�� ����Ʈ�� ������ �д�.
		m_listThumb.SetAt(nowPos, data);
		
		// ȭ�鿡 ����ϸ鼭 �� ThumbNail���� ��ġ�� ��������
		// �� ���� �����Ѵ�.
		//���� ����ϴ� ȭ���� ���� ĳ���� ��� ���ڿ� �β��� �β��� ��´�.
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

		//�����ڵ尡 8���� �Ѵ°�?
		if(wcslen(data.strSceneName)>8)
		{
			//������ 8�ڱ��� ����� [...]�� �߰�

			CStringW str;
			str.Format(L"%s",data.strSceneName);

			//8�ڸ� ���� �߶� �� [...]�� ���δ�
			CStringW strLeft_8 = str.Left(8);
			str.Format(L"%s...",strLeft_8.GetBuffer());
			wcsncpy_s(strPrtSceneName, str.GetBuffer(), 255);

		}
		else
		{
			//8�� ���ϸ� �׳� ���
			CStringW str;
			str.Format(L"%s",data.strSceneName);
			wcsncpy_s(strPrtSceneName , str.GetBuffer(),255);
		}

		//ȭ�鿡 ���ڸ� ����ϱ� ���� Rect ���� ����
		CPoint pointTop = data.m_ptLocation;
		pointTop.y += (m_nThumHeight+ m_nTitleTopSpace);

		CPoint pointBottom;
		pointBottom.x = data.m_ptLocation.x + m_nThumWidth;
		pointBottom.y = pointTop.y + m_nTitleHeight;

		CRect rectPrtSceneName(pointTop, pointBottom);
		int nStrLen = (int)wcslen(strPrtSceneName);

		//�����쿡 �̹����� �̸��� ���
		dc.DrawText(strPrtSceneName, nStrLen,&rectPrtSceneName, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		nCntCaret++;
	}
}

void LmUISceneView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: �� ���� ��ü ũ�⸦ ����մϴ�.
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	//m_ImageList �ʱ�ȭ
	CRect rect;
	GetClientRect(&rect);
	

	m_nThumWidth = rect.Width() - (2 * m_nLeftSpace);
	m_nThumHeight =(int)( (rect.Width() *3/4.0) );

	////�� ũ�� ���
	m_sizeWin.cx = rect.Width();
	m_sizeWin.cy = rect.Height();

	ClientToScreen(&rect);
	//���� Viewũ�� ��� --> LmUISplitterWnd Ŭ�������� ũ�� ������ ���� ����.

	m_rectWin = rect;
	

	m_nImageWidth = 500 - (2 * m_nLeftSpace);
	m_nImageHeight =(int)( (rect.Width() *3/4.0) );

	InitSceneView();

	LoadThumbNail();
	m_isLoadComplete=TRUE;
}


// CLmUIThumbnailView �μ�

BOOL LmUISceneView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void LmUISceneView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void LmUISceneView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CLmUIThumbnailView ����

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


// CLmUIThumbnailView �޽��� ó����

HBITMAP LmUISceneView::LoadPicture( LPCTSTR szBMPFileName )
{
	IPicture * pPic; //������ ���� Ŭ����

	CString strChangeMode =szBMPFileName; 
	WCHAR wszFilename[255];
	wcsncpy_s(wszFilename, strChangeMode.GetBuffer(), 255);
	//OLE ��ü�������� ���ο� ���� ��ü�� ����� �ʱ�ȭ ��Ŵ
	OleLoadPicturePath(wszFilename, NULL, NULL, NULL, IID_IPicture,(LPVOID*)&pPic);

	//IPicture ��ü�� ����� ������ �Ǿ��°�?
	if (pPic==NULL) 
	{
		SetErrorMsg(L"ã�� ������ �����ϴ�. OleLoadPicturePath �Լ����� IPicture ��ü ���� ����!");
		return NULL;
	}

	return LoadedPicture(pPic);
}


HBITMAP LmUISceneView::LoadedPicture(IPicture* pPic)
{
	HBITMAP hPic = NULL;

	//�ڵ��� �̿��ؼ� hPic�� BITMAP�� �׸��� ����Ѵ�.
	pPic->get_Handle((UINT*)&hPic);

	//����, ���� ���� ���صд�.
	long nWidth = m_nImageWidth;
	long nHeight = m_nImageHeight;

	//���� �׸��� ����, ���� ���̸� ���´�.
	long mWid, mHei;
	pPic->get_Height(&mHei);
	pPic->get_Width(&mWid);

	//�ڵ��� �̿��ؼ� �̹��� �˹����� �����ϰ� �����ϰ� �Ҵ��ϱ� ���� �ִ� �̹����� �����Ѵ�.
	HBITMAP hPicRet = (HBITMAP)CopyImage(hPic, IMAGE_BITMAP, nWidth ,nHeight, LR_COPYDELETEORG);

	// ��׶���� �׵θ��� ���� �귯�� ����
	HBRUSH hBrushBorder=::CreateSolidBrush(RGB(192, 192, 192));
	HBRUSH hBrushBk=::CreateSolidBrush(RGB(255, 255, 255));

	// �׵θ� ������
	RECT rcBorder;
	rcBorder.left=rcBorder.top=0;
	rcBorder.right=m_nImageWidth;
	rcBorder.bottom=m_nImageHeight;

	// ���� �� ���
	const float fRatio=(float)m_nImageHeight/m_nImageWidth;

	// �˹����� ���� ���� ����ؼ� ���ΰ� ū�� ���ΰ� ū���� ���� �̹����� �����Ѵ�.

	INT XDest, YDest, nDestWidth, nDestHeight;
	const float fImgRatio = (float)mHei/mWid;

	//�̹����� �˹��� ���� �����Ѱ�?
	if(fImgRatio > fRatio)
	{
		//�̹����� �˹��� ���� �����ϸ� ������ �̹����� ���� ���̸� ���� ������ ������ �����
		nDestWidth = (int)((float)m_nImageHeight*mWid/mHei);
		XDest = (m_nImageWidth-nDestWidth)/2;
		YDest = 0;
		nDestHeight = m_nImageHeight;
	}
	else
	{
		//�̹����� �˹��� ���� �����ϸ� ������ �̹����� ���� ���̸� ���� ������ ������ �����
		XDest = 0;
		nDestWidth = m_nImageWidth;
		nDestHeight = (int)((float)m_nImageWidth * mHei / mWid );
		YDest = (m_nImageHeight - nDestHeight)/2;
	}

	CClientDC cdc(NULL);
	HDC hDC = ::CreateCompatibleDC(cdc.m_hDC); //�޸� DC ����
	HBITMAP bm = CreateCompatibleBitmap(cdc.m_hDC, m_nImageWidth, m_nImageHeight); //�޸� ��Ʈ�� ����
	HBITMAP pOldBitmapImage = (HBITMAP)SelectObject(hDC, bm);	//�޸� DC�� ��Ʈ�� ��ü�� �����Ѵ�.
	//��ȯ ���� ��ü�� ��ü�� �ڵ��̴�.��, bm ��Ʈ�޸𸮰� ���õ� hDC �޸� DC�� �ڵ�


	//�޸� bitmap���� �׸� -------
	// ��׶��� �׸���
	::FillRect(hDC, &rcBorder, hBrushBk);
	//�޸� bitmap���� �׸� ------- �������

	HBITMAP hBmReturn = (HBITMAP)::SelectObject(hDC, pOldBitmapImage); // ��ü�� HBITMAP �ڵ��� ���󺹱� �Ѵ�.


	//CDC�� �����Ѵ�. 
	CDC hdcSrc, hdcDst;

	//�޸� DC�� ����
	hdcSrc.CreateCompatibleDC(NULL);
	hdcDst.CreateCompatibleDC(NULL);

	// �˹����� �޸� DC�� �ε��Ѵ�.
	CBitmap* hbmSrcT = (CBitmap*) hdcSrc.SelectObject(hPicRet);		//���� �̹����� �޸� ��Ʈ�ʿ� �ø���.
	CBitmap* hbmDstT = (CBitmap*) hdcDst.SelectObject(hBmReturn);	//��ҽ�ų �̹����� �޸� ��Ʈ�ʿ� �ø���.

	//�޸� DC�� ������ Thumbnail(���Ӱ� ������ hdcDst DC��)�� �׸��� �׸���.
	//hdcDst.StretchBlt(XDest+1, YDest+1, nDestWidth-2, nDestHeight-2, &hdcSrc, 0,0, m_nImageWidth, m_nImageHeight , SRCCOPY);
	//Scene�� �°� ������
	hdcDst.StretchBlt(0,0, m_nImageWidth, m_nImageHeight, &hdcSrc, 0,0, m_nImageWidth, m_nImageHeight , SRCCOPY);

	pOldBitmapImage = (HBITMAP)SelectObject(hdcDst.m_hDC, bm);

	DeleteDC(hDC);
	DeleteObject(hBrushBorder);
	DeleteObject(hBrushBk);

	DeleteObject(pPic);
	DeleteObject(hPicRet);


	return pOldBitmapImage;
}

// ���ο� ������ ����� ��� 
int LmUISceneView::AddThumbNail(LPCTSTR szBMPFileName, LPCTSTR szSceneName)
{
	//��Ʈ���� �о�´�.
	HBITMAP bitm = LoadPicture(szBMPFileName);

	//�Է� ���� BMP������ ���°�? �׷��ٸ� return �϶�!
	if(bitm == NULL)
		return -1;

	CBitmap* pImage = new CBitmap();
	pImage->Attach(bitm); //bitmap �ڵ��� �޾ƿ´�.

	int nImgEnd = m_imageList.Add(pImage, RGB(0,0,0)); //�̹��� ����Ʈ�� ����� Image�� �����Ѵ�.

	//m_listThumb�� ����� ������ ����Ѵ�.

	int nCntNode = (int)m_listThumb.GetCount();
	CRectPicture NewNode;
	NewNode.m_ptLocation.x = 0;
	NewNode.m_ptLocation.y = 0;
	NewNode.nIndex = m_nIndexCnt++; //�߰� �� ����
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
	//10�� ���� ������ ��ü ��ũ�� ��ǥ���� �ְ� ũ�⸦ �����߱� ������.
	CSize sizeTotal( rect.Width() ,nLastYData + m_nThumHeight + m_nTitleTopSpace + m_nTitleHeight + 30);
	CSize sizePage(rect.Width()-30, rect.Height()-30);
	CSize sizeLine(50,50);

	SetScrollSizes(MM_TEXT,sizeTotal,sizePage, sizeLine);
}

void LmUISceneView::OnSize(UINT nType, int cx, int cy)
{

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	//TRACE(L"OnSize Message \r\n");
	// ������..(�Լ��� ȣ��� ���¿��� �ٽ� ���ƿ��� ���)�� ���� ���� �ڵ�
	static bool bInOnSize = false;
	if(bInOnSize == true)
		return;

	bInOnSize = true;
	
	TRACE(L"OnSize Message no reCall \r\n");
	
	TRACE(L"CX Size : %d\r\n", cx);
	
	if(m_isLoadComplete)
	{
		
		//�䰡 ������ ���� �̹����� �׸��� �ϸ�(m_ImageList�� NULL��) ������ ����.
		if(!cx)
			return;

		if(m_sizeWin.cx != cx)
		{
			
			//������ ��ũ���� ����� �ѹ��� ȣ��Ǵµ� ȭ���� ��ü ȭ����� �ϸ�
			//�ٽ� ȣ��ȴ�.
			
				//�� ũ�� ���
				m_sizeWin.cx = cx;
				m_sizeWin.cy = cy;

				m_nThumWidth = cx - (2 * m_nLeftSpace);
				m_nThumHeight = (int)( cx * 3/4.0);

		}

	}
	//���� Viewũ�� ��� --> LmUISplitterWnd Ŭ�������� ũ�� ������ ���� ����.
	CRect rect;
	GetClientRect(&rect);	
	ClientToScreen(&rect);
	m_rectWin = rect;

	onRefreshScroll();

	//���� ��ũ�� �ٴ� �����.
	ShowScrollBar(SB_HORZ, FALSE);
	bInOnSize = false;
	CScrollView::OnSize(nType, cx, cy);

}

void LmUISceneView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	POSITION pPos = m_listThumb.GetHeadPosition();
	CRectPicture data;

	//�����Ͱ� �������� �׳� ����~
	if(!m_listThumb.GetCount())
		return;

	//Wizard�� ����� ���� Scene�� ����Ǿ�� �ȵȴ�.
	if(!m_bChangeScene)
		return; 

	while(pPos)
	{
		data = m_listThumb.GetNext(pPos);
		CPoint pointBottom(data.m_ptLocation.x + m_nThumWidth , 
			data.m_ptLocation.y + m_nThumHeight + m_nTitleTopSpace + m_nTitleHeight);
		CRect rectArea(data.m_ptLocation , pointBottom);

		//Ŭ���� ��ġ�� �׸��� ���� ������ ���ٸ�
		//�� �׸����ڸ� �β��� �׷��ش�.
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

			//���� Ŀ���� �ִ� ������ �����Ѵ�.
			m_ThumbNailCaret.m_ptLocation = data.m_ptLocation;
			m_ThumbNailCaret.nImgIndex = data.nImgIndex;
			m_ThumbNailCaret.nIndex = data.nIndex;
			wcsncpy_s(m_ThumbNailCaret.strSceneName, data.strSceneName, sizeof(data.strSceneName));

			// [����] ������� �����Ѵ�.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	int SCrollDirect=0;

	//���� ��ϵ� ���� �ϳ��� ���ų� ���� ������ ����(Wizard�����) �Ǿ������� �ٷ� �����Ѵ�. 
	if(!m_listThumb.GetCount() || !m_bChangeScene)
	{
		CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}

	CClientDC dc(this);
	OnPrepareDC(&dc);  //��ǥ ����
	
	//Scene�� ����� �����Ҷ� ����� �� �� ������ �� �ֵ��� ������
	
	POSITION pos = m_listThumb.GetHeadPosition();
	POSITION posHead = m_listThumb.GetHeadPosition();
	POSITION posTail = m_listThumb.GetTailPosition();
	POSITION posFinder = m_listThumb.Find(m_ThumbNailCaret);


	if(nChar==VK_UP)
	{
		//���� �̵��� ����.
		CRectPicture PicCaret = m_listThumb.GetPrev(posFinder);
		//TRACE(L"VK_UP pre : %d\r\n", PicCaret.nIndex);

		//Position�� ��ġ�� Head�ΰ�?? Head��� posFinder�� 0�� ������!
		if(posFinder)
		{
			PicCaret = m_listThumb.GetPrev(posFinder);	
			//TRACE(L"VK_UP will Go : %d\r\n", PicCaret.nIndex);
			SetThumbNailCaret(PicCaret.nIndex);
			SCrollDirect=0; //��ũ�� ���� ����
		}
		else 
		//����� �׳� �������� �ʴ´�.
		{
			CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
			return;
		}
	}
	else if(nChar == VK_DOWN)
	{
		CRectPicture PicCaret = m_listThumb.GetNext(posFinder);
		//TRACE(L"VK_DOWN pre : %d\r\n", PicCaret.nIndex);
		//Position�� ��ġ�� Tail�ΰ�?? Tail��� posFinder�� 0�� ������!
		if(posFinder)
		{
			PicCaret = m_listThumb.GetNext(posFinder);		
			//TRACE(L"VK_DOWN Go : %d\r\n", PicCaret.nIndex);

			SetThumbNailCaret(PicCaret.nIndex);
			SCrollDirect = 1;  //��ũ�� ���� �Ʒ���
		}
		else
		{
			CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
			return;
		}
	}

	//���� ����� Caret�� ������ ���´�.
	CRectPicture data;
	GetThumbNailCaret(data);

	CPoint ptDataMapping = data.m_ptLocation;

	//��ũ�� �˻�.. ���� ȭ�� ���� ��� ��ũ�� �Ѵ�.
	dc.LPtoDP(&ptDataMapping);
	CRect rectView;
	GetClientRect(&rectView);

	//��ũ�� �˻縦 �ϱ� ���� �Ʒ��� �ϴ��� ���� ��ǥ���� ����� ����. Device Client ��ǥ��.
	CPoint ptBottomMapping;
	ptBottomMapping.x = ptDataMapping.x;
	ptBottomMapping.y = ptDataMapping.y + m_nThumHeight + m_nTitleHeight + m_nTitleTopSpace;

	//ȭ�� ���ΰ�?
	if(!rectView.PtInRect(ptBottomMapping) || !rectView.PtInRect(ptDataMapping))
	{
		//��ũ�� ������ �Ʒ����ΰ�?
		if(SCrollDirect)
		{
			CPoint ptBottom;
			ptBottom.x = 0;
			//ptBottom.y = data.m_ptLocation.y + m_nThumHeight + m_nTitleHeight + m_nTitleTopSpace;
			ptBottom.y = GetScrollPos(SB_VERT) + m_nThumHeight + m_nTitleHeight + m_nTitleTopSpace + m_nRowSpace;
			ScrollToPosition(ptBottom);
		}
		else
			//�׷��� ������ ��ũ�� ������ �����ΰ�?
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

	//ĳ���� �׸���.
	CRect rectCaret(pointTCaret, pointBCaret);
	dc.SelectStockObject(NULL_BRUSH);
	CPen penCaret(PS_SOLID, 4, RGB(0,0,0));
	CPen *pOldPen = dc.SelectObject(&penCaret);

	dc.Rectangle(&rectCaret);
	dc.SelectObject(pOldPen);

	//ĳ�� ���� ���� ū������ ������ �ξ��ٰ� ĳ���� �ٲ� invalidaterect�� �Ѵ�.

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
	//// �׽�Ʈ��..
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	//if(pMsg->message == LM_CHANGE_SCENEVIEW)	
	//	onLmChangeSize();

	return CScrollView::PreTranslateMessage(pMsg);
}

//void LmUISceneView::onLmChangeSize()
//{
//	if(m_isLoadComplete)
//	{
//		//�䰡 ������ ���� �̹����� �׸��� �ϸ�(m_ImageList�� NULL��) ������ ����.
//		CRect rectView;
//		GetClientRect(&rectView);
//
//		if(!rectView.Width())
//			return;
//
//		//������ ũ���� ��ȭ�� �ִٸ� 1���� �����ϵ��� �ϱ� ����..
//		//���� ��� ����.. WM_SIZE�� ���� 5�� ������ ȣ��ǰ� �� �Լ��� ������� ó���ȴ�. 
//		//�׷��� �����Ͱ� ����.. �׷� ������ ���� ���� �Ʒ��� ���� �ڵ���.
//
//
//		if(m_sizeWin.cx != rectView.Width())
//		{
//
//			//�� ũ�� ���
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

// ���� Ŀ���� �������ִ� �Լ�
// OnLButtonDown�Լ������� �� �Լ��� ������ ����!! - �ӵ��� ���� ��ġ��.
void LmUISceneView::SetThumbNailCaret( UINT nCaret )
{
	POSITION pPos = m_listThumb.GetHeadPosition();
	CRectPicture data;

	//�����Ͱ� �������� �׳� ����~
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

		//Ŭ���� ��ġ�� �׸��� ���� ������ ���ٸ�
		//�� �׸����ڸ� �β��� �׷��ش�.
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

			//���� Ŀ���� �ִ� ������ �����Ѵ�.
			m_ThumbNailCaret.m_ptLocation = data.m_ptLocation;
			m_ThumbNailCaret.nImgIndex = data.nImgIndex;
			m_ThumbNailCaret.nIndex = data.nIndex;
			wcsncpy_s(m_ThumbNailCaret.strSceneName, data.strSceneName, wcslen(data.strSceneName));

			break;
		}
		nCntCaret++;
	}
	
	// [����6] ������� �����Ѵ�.
	TheCartoonMaker->SelectNowScene(data.strSceneName);	
	TheCartoonMaker->SetStateOfUI(SCENE_CHANGED);
}

//���� ĳ���� ������ ��ȯ�� �ش�.
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
//�Է� ���� ThumbNail index�� �˻��Ͽ� �����Ѵ�.
// - ���� �����ϰ� Find �Լ��� ����Ͽ� ������ ��!
// nIndex ���� �˻��ϵ��� ����!
// GetThumbNail() �Լ��� ����� ��!
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
	
	//ã�� Thumbnail�� ������ �����Ѵ�.
	if(!Finder)
		return;

	FinderAfter = Finder;
	
	data = m_listThumb.GetNext(FinderAfter);
	
	//Caret�� �ִ� ������ �����.
	m_listThumb.RemoveAt(Finder);
	m_imageList.Remove(data.nImgIndex);

	INT nImageIndexDeleted = data.nImgIndex;

	//Caret�� �����Ѵ�.
	//���� ��ũ�� ��ġ�� NULL�� �ƴϰ� Node�� �����ϴ°�?
	CRectPicture newCaret;
	if(FinderAfter)
		newCaret= m_listThumb.GetAt(FinderAfter);
	//���� ��ũ�� ��ġ�� NULL�� ���
	else
	{
		//�����ִ� ����� ������ �ľ��Ѵ�.
		//���� �ִ� ����� ������ 1 �̻��̸� ������ ��忡 Caret�� ��ġ ��Ų��.
		UINT nNodeCnt = (UINT)m_listThumb.GetCount();
		if(nNodeCnt)
			newCaret = m_listThumb.GetTail();
		else
		// else --> Thumb�� �ִٰ� �ٽ� ������� Node���� �������� �˾Ƽ� �߰��� ��ų�� Caret�� �߰� �ȴ�.
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
		
		//ImageList �� Index�� ���߱� ���� ������ �̹��� Index���� ū �̹��� Index�� ��� 1�� ���ش�.
		if( nImageIndexDeleted < DataChanging.nImgIndex)
			--DataChanging.nImgIndex;
		m_listThumb.SetAt(nowPos, DataChanging);
		m_listThumb.GetNext(nowPos);

	}
	
	//View�� ũ�⸦ ������ �ش�.
	onRefreshScroll();
	return;
}

int LmUISceneView::InsertThumbNail( LPCTSTR szBMPFileName, LPCTSTR szSceneName )
{
	//��Ʈ���� �о�´�.
	HBITMAP bitm = LoadPicture(szBMPFileName);

	//�Է� ���� BMP������ ���°�? �׷��ٸ� return �϶�!
	if(bitm == NULL)
		return -1;

	CBitmap* pImage = new CBitmap();
	pImage->Attach(bitm); //bitmap �ڵ��� �޾ƿ´�.

	int nImgEnd = m_imageList.Add(pImage, RGB(0,0,0)); //�̹��� ����Ʈ�� ����� Image�� �����Ѵ�.

	//m_listThumb�� ����� ������ ����Ѵ�.

	int nCntNode = (int)m_listThumb.GetCount();
	
	CRectPicture NewNode;

	//�׸� ������ ��ġ�� onDraw���� ��������.
	//2007. 12. 06. ����
	NewNode.m_ptLocation.x = 0;
	NewNode.m_ptLocation.y = 0; 

	NewNode.nIndex = m_nIndexCnt++; //�߰� �� ����
	NewNode.nImgIndex = nImgEnd;
	wcsncpy_s(NewNode.strSceneName,szSceneName, MAX_STR);

	//ThumbNail�� �ϳ��� ���°�? �׷� ��� �߰��Ѵ�.
	if(!nCntNode)
		m_listThumb.AddTail(NewNode);
	//ThumbNail�� ���� ��� Caret�� �ٷ� �ؿ� ThumbNail�� �����Ѵ�.
	else
	{
		POSITION pos = m_listThumb.GetHeadPosition();
		while(pos)
		{
			CRectPicture data = m_listThumb.GetAt(pos);
			 
			//���� Ŀ���� ��ġ�� ã�Ƴ� �� �� �ڸ��� �����Ѵ�.
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
	//��Ʈ���� �о�´�.
	HBITMAP bitm = LoadedPicture(pPic);

	//�Է� ���� BMP������ ���°�? �׷��ٸ� return �϶�!
	if(bitm == NULL)
		return -1;

	CBitmap* pImage = new CBitmap();
	pImage->Attach(bitm); //bitmap �ڵ��� �޾ƿ´�.
	
	POSITION pos = m_listThumb.GetHeadPosition();
	while(pos)
	{
		CRectPicture data = m_listThumb.GetAt(pos);

		//���� Ŀ���� ��ġ�� ã�Ƴ� �� �� �ڸ��� �׸��� ��ü�Ѵ�.
		if(data.nIndex == ThumIndex)
		{
			m_imageList.Replace(data.nImgIndex, pImage, RGB(0,0,0)); //�̹��� ����Ʈ�� ����� Image�� �����Ѵ�.		
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
	//�ӽ� ���� �ʱ�ȭ
	m_vectorMemThumb.clear();	
	m_imageListMem.DeleteImageList();

	m_imageListMem.Create(m_nImageWidth, m_nImageHeight, ILC_COLOR32, 0, 1);

	//���� Caret�� ������ ���´�. 
	CRectPicture rCaretData;
	GetThumbNailCaret(rCaretData);
	
	//���� Caret�� ������ ���Ϳ� �����Ѵ�. 
	m_vectorMemThumb.push_back(rCaretData);
	
	//���� Caret�� ���� �̹��� ������ ������ �д�. 
	CopyImgListTOMemList(rCaretData);

	TheCartoonMaker->CopyScene();

	return 0;
}

VOID LmUISceneView::InitSceneView()
{
	m_listThumb.RemoveAll();
	m_imageList.DeleteImageList();

	m_imageList.Create(m_nImageWidth, m_nImageHeight, ILC_COLOR32, 0, 1);

	//ThumbNail �ӽ� ���� �ʱ�ȭ
	m_vectorMemThumb.clear();	
	m_imageListMem.DeleteImageList();

	m_imageListMem.Create(m_nImageWidth, m_nImageHeight, ILC_COLOR32, 0, 1);

}

INT LmUISceneView::PasteThumbNail()
{
	INT nVectorSize = (INT)m_vectorMemThumb.size();
	
	//�޸𸮿� ����� ������ ������ ����
	if(!nVectorSize)
		return -1;
	
	//�׸��� �����Ѵ�. 
	//INT nImageCount = m_imageList.GetImageCount();
	//m_imageList.SetImageCount(++nImageCount);
	//m_imageList.Copy(nImageCount-1, &m_imageListMem, 0, ILCF_SWAP);

	INT nNewImageIndex = CopyMemListTOImgList();

	//m_listThumb�� ����� ������ ����Ѵ�.
	int nCntNode = (int)m_listThumb.GetCount();
	CRectPicture NewNode;

	//�׸� ������ ��ġ�� onDraw���� ��������.
	//2007. 12. 06. ����
	NewNode.m_ptLocation.x = 0;
	NewNode.m_ptLocation.y = 0; 

	NewNode.nIndex = m_nIndexCnt++; //�߰� �� ����
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

	
	//ThumbNail�� �ϳ��� ���°�? �׷� ��� �߰��Ѵ�.
	if(!nCntNode)
		m_listThumb.AddTail(NewNode);
	//ThumbNail�� ���� ��� Caret�� �ٷ� �ؿ� ThumbNail�� �����Ѵ�.
	else
	{
		POSITION pos = m_listThumb.GetHeadPosition();
		while(pos)
		{
			CRectPicture data = m_listThumb.GetAt(pos);

			//���� Ŀ���� ��ġ�� ã�Ƴ� �� �� �ڸ��� �����Ѵ�.
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
	//�ӽ� ���� �ʱ�ȭ
	m_vectorMemThumb.clear();	
	m_imageListMem.DeleteImageList();

	m_imageListMem.Create(m_nImageWidth, m_nImageHeight, ILC_COLOR32, 0, 1);

	//���� Caret�� ������ ���´�. 
	CRectPicture rCaretData;
	GetThumbNailCaret(rCaretData);

	//���� Caret�� ������ ���Ϳ� �����Ѵ�. 
	m_vectorMemThumb.push_back(rCaretData);

	//���� Caret�� ���� �̹��� ������ ������ �д�. 
	CopyImgListTOMemList(rCaretData);

	//�̹����� ���� ������ �����ߴ�. �Ѥ�^
	//��� SceneView�� TheCartoonMaker�� ȣ�� �ñ�� ThumbNailCaret�� ����Ǳ� ���� �ؾ߸� �Ѵ�!
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
	// �̹��� ����Ʈ�� ���� �̹��� ������ ���´�.	
	
	CBitmap *pBmp = MakeImage(&m_imageList, rCaretData.nImgIndex );

	m_imageListMem.Add(pBmp,RGB(0,0,0));
	delete pBmp;
}

INT LmUISceneView::CopyMemListTOImgList()
{
	// �̹��� ����Ʈ�� ���� �̹��� ������ ���´�.	

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

		//���� Ŀ���� ��ġ�� ã�Ƴ� �� �� �ڸ��� �����Ѵ�.
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
	//CRectPicture�� CList Template�� �����Ѵ�.
	//////////////////////////////////////////////////////////////////////////

	//List�� ������ �����Ѵ�.
	UINT nListCount = (UINT)m_listThumb.GetCount();
	File.Write(&nListCount , sizeof(nListCount));

	POSITION pos = m_listThumb.GetHeadPosition();
	while(pos)
	{
		//�� Ŭ���� ���ο� �ִ� �����͸� �ϳ��� ������� �����Ѵ�.
		CRectPicture data = m_listThumb.GetNext(pos);
		File.Write(&(data), sizeof(data) );
		//File.Write(&(data.m_ptLocation.x), sizeof(LONG) );
		//File.Write(&(data.m_ptLocation.y), sizeof(LONG) );
		//File.Write(&(data.nImgIndex), sizeof(INT));
		//File.Write(&(data.nIndex), sizeof(UINT));
		//File.Write(&(data.strSceneName), sizeof(data.strSceneName));
	}

	//////////////////////////////////////////////////////////////////////////
	// CImageList�� �����Ѵ�.
	//////////////////////////////////////////////////////////////////////////

	//m_ImageList�� CImageList���� ������ �ִ� CArchive�� �̿��ؼ� ���Ϸ� ������ �� �� ������ �ٽ� �����Ѵ�. 	
	CString strImageListFile;
	CString strResDirectory;
	GetResourceDirectory(strResDirectory);
	strImageListFile.Format(L"%s\\imageFile.dat", strResDirectory.GetBuffer(0));

	CFile ImageListFile( strImageListFile.GetBuffer(0), CFile::modeCreate | CFile::modeWrite );
	CArchive ar(&ImageListFile, CArchive::store);

	// Store the image list in the archive.
	m_imageList.Write(&ar);

	// ��ī�̺긦 �ݾ��ش�.
	ar.Close();

	// ������ �ݾ��ش�.
	ImageListFile.Close();

	CFile ReloadImageList( strImageListFile.GetBuffer(0),CFile::modeRead );
	UINT uImageListSize = (UINT)ReloadImageList.GetLength();
	BYTE* byteBuffer = new BYTE[uImageListSize];

	ReloadImageList.Read(byteBuffer , uImageListSize);
	File.Write(byteBuffer, uImageListSize);

	ReloadImageList.Close();

	uFilePos = (UINT)File.GetPosition();
	TheCartoonMaker->SetSceneViewEnd(uFilePos);

	//�޸� ����
	delete[] byteBuffer;
	return 0;
}

INT LmUISceneView::LoadSceneView( CFile & File )
{
	UINT uFilePosStart = TheCartoonMaker->GetSceneViewStart();
	File.Seek(uFilePosStart, CFile::begin);

	//////////////////////////////////////////////////////////////////////////
	//CRectPicture�� CList Template�� �ε��Ѵ�.
	//////////////////////////////////////////////////////////////////////////

	//����� ����Ʈ �ʱ�ȭ
	m_listThumb.RemoveAll();
	Invalidate();
	onRefreshScroll();

	//List�� ������ �о�´�.
	UINT nListCount = 0;
	File.Read(&nListCount , sizeof(nListCount));

	POSITION pos = m_listThumb.GetHeadPosition();
	UINT nCount = 0;
	for( nCount = 0; nCount < nListCount ; nCount++ )
	{
		//�� Ŭ���� ���ο� �ִ� �����͸� �ϳ��� ������� �����Ѵ�.
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
	// CImageList�� �ε��Ѵ�.
	//////////////////////////////////////////////////////////////////////////
	CString strImageListFile;
	CString strResDirectory;
	GetResourceDirectory(strResDirectory);
	strImageListFile.Format(L"%s\\imageFile.dat",strResDirectory.GetBuffer(0));

	UINT uFilePosEnd = TheCartoonMaker->GetSceneViewEnd();
	UINT uImageListSize = uFilePosEnd - uFilePosStart + 1;
	
	//�̹��� ����Ʈ�� �о� ���̱� ���� ���� ����
	BYTE* byteBuffer = new BYTE[uImageListSize];
	File.Read(byteBuffer, uImageListSize);

	//����� �̹��� ����Ʈ�� CArchive�� �о� ���̱� ���� imageFile.dat��� ���ϸ����� ����Ѵ�.
	CFile ReloadImageList(strImageListFile.GetBuffer(0), CFile::modeCreate | CFile::modeWrite );
	ReloadImageList.Write(byteBuffer , uImageListSize);
	ReloadImageList.Close();

	//����� imageFile.dat�� CArchive�� �о �ε��Ѵ�.
	CFile ImageListFile(strImageListFile.GetBuffer(0), CFile::modeRead);
	CArchive ar(&ImageListFile, CArchive::load);

	// Store the image list in the archive.
	m_imageList.DeleteImageList();
	m_imageList.Read(&ar);

	// ��ī�̺긦 �ݾ��ش�.
	ar.Close();

	// ������ �ݾ��ش�.
	ImageListFile.Close();

	//�޸� ����
	delete[] byteBuffer;

	Invalidate();
	onRefreshScroll();

	return 0;
}