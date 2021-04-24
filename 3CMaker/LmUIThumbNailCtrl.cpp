// LmUIThumbNailCtrl.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "LmUIThumbNailCtrl.h"

// LmUIThumbNailCtrl

IMPLEMENT_DYNAMIC(LmUIThumbNailCtrl, CWnd)

LmUIThumbNailCtrl::LmUIThumbNailCtrl()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
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

	HBITMAP hPic = NULL;

	//�ڵ��� �̿��ؼ� hPic�� BITMAP�� �׸��� ����Ѵ�.
	pPic->get_Handle((UINT*)&hPic);

	//����, ���� ���� ���صд�.
	long nWidth = m_nThumWidth;
	long nHeight = m_nThumHeight;

	//���� �׸��� ����, ���� ���̸� ���´�.
	long mWid, mHei;
	pPic->get_Height(&mHei);
	pPic->get_Width(&mWid);

	//�ڵ��� �̿��ؼ� �̹��� �˹����� �����ϰ� �����ϰ� �Ҵ��ϱ� ���� �ִ� �̹����� �����Ѵ�.
	HBITMAP hPicRet = (HBITMAP)CopyImage(hPic, IMAGE_BITMAP, nWidth, nHeight, LR_COPYDELETEORG);

	// ��׶���� �׵θ��� ���� �귯�� ����
	HBRUSH hBrushBorder=::CreateSolidBrush(RGB(192, 192, 192));
	HBRUSH hBrushBk=::CreateSolidBrush(RGB(255, 255, 255));

	// �׵θ� ������
	RECT rcBorder;
	rcBorder.left=rcBorder.top=0;
	rcBorder.right=m_nThumWidth;
	rcBorder.bottom=m_nThumHeight;

	// ���� �� ���
	const float fRatio=(float)m_nThumHeight/m_nThumWidth;

	// �˹����� ���� ���� ����ؼ� ���ΰ� ū�� ���ΰ� ū���� ���� �̹����� �����Ѵ�.

	INT XDest, YDest, nDestWidth, nDestHeight;
	const float fImgRatio = (float)mHei/mWid;

	//�̹����� �˹��� ���� �����Ѱ�?
	if(fImgRatio > fRatio)
	{
		//�̹����� �˹��� ���� �����ϸ� ������ �̹����� ���� ���̸� ���� ������ ������ �����
		nDestWidth = (int)((float)m_nThumHeight*mWid/mHei);
		XDest = (m_nThumWidth-nDestWidth)/2;
		YDest = 0;
		nDestHeight = m_nThumHeight;
	}
	else
	{
		//�̹����� �˹��� ���� �����ϸ� ������ �̹����� ���� ���̸� ���� ������ ������ �����
		XDest = 0;
		nDestWidth = m_nThumWidth;
		nDestHeight = (int)((float)m_nThumWidth * mHei / mWid );
		YDest = (m_nThumHeight - nDestHeight)/2;
	}

	CClientDC cdc(this);
	HDC hDC = ::CreateCompatibleDC(cdc.m_hDC); //�޸� DC ����
	HBITMAP bm = CreateCompatibleBitmap(cdc.m_hDC, m_nThumWidth, m_nThumHeight); //�޸� ��Ʈ�� ����
	HBITMAP pOldBitmapImage = (HBITMAP)SelectObject(hDC, bm);	//�޸� DC�� ��Ʈ�� ��ü�� �����Ѵ�.
	//��ȯ ���� ��ü�� ��ü�� �ڵ��̴�.��, bm ��Ʈ�޸𸮰� ���õ� hDC �޸� DC�� �ڵ�


	//�޸� bitmap���� �׸� -------
	// ��׶��� �׸���
	::FillRect(hDC, &rcBorder, hBrushBk);
	// �׵θ� �׸���
	::FrameRect(hDC, &rcBorder, hBrushBorder);
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
	//��Ʈ���� �о�´�.
	HBITMAP bitm = LoadPicture(szBMPFileName);

	//�Է� ���� BMP������ ���°�? �׷��ٸ� return �϶�!
	if(bitm == NULL)
		return;

	CBitmap* pImage = new CBitmap();
	pImage->Attach(bitm); //bitmap �ڵ��� �޾ƿ´�.

	int nImgEnd = m_imageList.Add(pImage, RGB(0,0,0)); //�̹��� ����Ʈ�� ����� Image�� �����Ѵ�.

	//m_listThumb�� ����� ������ ����Ѵ�.

	int nCntNode = (int)m_listThumb.GetCount();
	CRectItem NewNode;
	NewNode.m_ptLocation.x = m_nLeftSpace;
	
	if(nCntNode%2)  
		// 0 - index ���� �����ϴ� ī���͸� 2�� ���� �������� 1�϶� �����ʿ� ���ڰ� ��µǾ�� ��.
		// ������ ���� �ΰ�?
	{
		NewNode.m_ptLocation.x += m_nSecLeftSpace; //������ ���Ѵ�.
		NewNode.m_ptLocation.x += m_nThumWidth; //���̸� ���Ѵ�.
	}
	
	NewNode.m_ptLocation.y = m_nRowSpace + (m_nRowSpace + m_nThumHeight + m_nTitleTopSpace + m_nTitleHeight) * (nCntNode / 2) ; // ������ ����

	NewNode.nIndex = nCntNode ;
	NewNode.nImgIndex = nImgEnd;
	wcsncpy_s(NewNode.strItemName,szSceneName, MAX_STR);

	m_listThumb.AddTail(NewNode);

	//�޸� ����
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
	//10�� ���� ������ ��ü ��ũ�� ��ǥ���� �ְ� ũ�⸦ �����߱� ������.
	
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	
	CClientDC dc(this);
	
	//ScrollView::OnPrepareDC(&dc)�� ���� �ڵ带 ���� ����.
	dc.SetMapMode(MM_TEXT);
	CPoint ptVpOrg(-GetScrollPos(SB_HORZ), -GetScrollPos(SB_VERT));       // assume no shift for printing
	dc.SetViewportOrg(ptVpOrg);
	

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

void LmUIThumbNailCtrl::ThmDrawCaret( CPoint leftTop , CDC & dcMem )
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CClientDC dc(this);

	//ScrollView::OnPrepareDC(&dc)�� ���� �ڵ带 ���� ����.
	dc.SetMapMode(MM_TEXT);
	CPoint ptVpOrg(-GetScrollPos(SB_HORZ), -GetScrollPos(SB_VERT));       // assume no shift for printing
	//dc.SetViewportOrg(ptVpOrg);


	CPoint pointTCaret, pointBCaret;
	pointTCaret.x = leftTop.x - 4;
	pointTCaret.y = leftTop.y - 4;

	pointBCaret.x = leftTop.x + m_nThumWidth + 4;
	pointBCaret.y = leftTop.y + m_nThumHeight + 4;

	//ĳ���� �׸���.
	CRect rectCaret(pointTCaret + ptVpOrg, pointBCaret + ptVpOrg);
	dcMem.SelectStockObject(NULL_BRUSH);
	CPen penCaret(PS_SOLID, 4, RGB(0,0,0));
	CPen *pOldPen = dcMem.SelectObject(&penCaret);

	dcMem.Rectangle(&rectCaret);
	dcMem.SelectObject(pOldPen);

	//ĳ�� ���� ���� ū������ ������ �ξ��ٰ� ĳ���� �ٲ� invalidaterect�� �Ѵ�.

	pointTCaret.x -=2;
	pointTCaret.y -=2;

	pointBCaret.x +=2;
	pointBCaret.y +=2;

	CRect rectTmp(pointTCaret, pointBCaret);
	m_rectPreCaret = rectTmp;

}



// LmUIThumbNailCtrl �޽��� ó�����Դϴ�.



void LmUIThumbNailCtrl::OnPaint()
{
	CPaintDC paintDc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CWnd::OnPaint()��(��) ȣ������ ���ʽÿ�.

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
	CClientDC dc(this);

	CRect rectScreen;
	GetClientRect(&rectScreen);

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc); //�޸� DC ����
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&dc, rectScreen.Width(), rectScreen.Height());
	dcMem.SelectObject(&bitmap);

	dcMem.SelectStockObject(WHITE_PEN);
	dcMem.Rectangle(&rectScreen);

	INT nListCnt = (INT)m_listThumb.GetCount();
	INT nItemCnt = 0; //2���� ���� ����ϱ� ���� ī����

	//ScrollView::OnPrepareDC(&dc)�� ���� �ڵ带 ���� ����.
	dc.SetMapMode(MM_TEXT);
	CPoint ptVpOrg(-GetScrollPos(SB_HORZ), -GetScrollPos(SB_VERT));       // assume no shift for printing
	//dc.SetViewportOrg(ptVpOrg);

	int nCntCaret=0; //Ŀ���� ���� ���Ǵ� Count ����
	POSITION pPos = m_listThumb.GetHeadPosition();
	CRectItem data;
	memset(&data, 0, sizeof(data));

	while(pPos)
	{
		data = m_listThumb.GetNext(pPos);


		//ȭ�鿡 ����� �� ������ ���� ���� ������ �˻��Ѵ�.
		// MemDC�� 4���� ����([�̹������� + �̹��� �̸� ����]�� �����ϴ� �簢��)�� 4���� �� �߿��� 1����
		// MemDC�� ������ ���� ������ �׸��� �׸��� �׷��� ������ �������� �Ѿ��.
		INT nLeft	= data.m_ptLocation.x + ptVpOrg.x;
		INT nTop	= data.m_ptLocation.y + ptVpOrg.y;
		INT nRight = data.m_ptLocation.x + m_nThumWidth + ptVpOrg.x;
		INT nBottom= data.m_ptLocation.y + m_nThumHeight + m_nTitleTopSpace + m_nTitleHeight +ptVpOrg.y;

		//Section Frame Rectangle�� 4���� �� �߿� 1���� MemDC�� �ҼӵǾ� ������ �׷��� �Ѵ�!
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

		//4���� ���� memory DC�� ������ ������ ���� �׷���� �Ѵ�.
		if( (nLeft <= 0) && (nRight >= rectScreen.Width()) )
			bIsDraw = TRUE;
		if( (nTop <= 0) && (nBottom >= rectScreen.Height()) )
			bIsDraw = TRUE;

		// 4���� ���� �ϳ��� MemDC ���� ���� ���� ������ �׸��� �ʴ´�.
		if(!bIsDraw)
		{
			nCntCaret++;
			nItemCnt++;
			continue;
		}
		
		//���� ����ϴ� ȭ���� ���� ĳ���� ��� ���ڿ� �β��� �β��� ��´�.
		if(m_nCaret == nCntCaret)
			ThmDrawCaret(data.m_ptLocation, dcMem);

		m_imageList.Draw(&dcMem, data.nImgIndex, data.m_ptLocation + ptVpOrg ,ILD_NORMAL);

		WCHAR strPrtSceneName[12];
		memset(strPrtSceneName, 0, sizeof(strPrtSceneName) );

		//�����ڵ尡 8���� �Ѵ°�?
		if(wcslen(data.strItemName)>8)
		{
			//������ 8�ڱ��� ����� [...]�� �߰�

			CStringW str;
			str.Format(L"%s",data.strItemName);

			//8�ڸ� ���� �߶� �� [...]�� ���δ�
			CStringW strLeft_8 = str.Left(8);
			str.Format(L"%s...",strLeft_8.GetBuffer());
			wcsncpy_s(strPrtSceneName, str.GetBuffer(), 255);

		}
		else
		{
			//8�� ���ϸ� �׳� ���
			CStringW str;
			str.Format(L"%s",data.strItemName);
			wcsncpy_s(strPrtSceneName , str.GetBuffer(),255);
		}

		//ȭ�鿡 ���ڸ� ����ϱ� ���� Rect ���� ����
		CPoint pointTop = data.m_ptLocation;
		pointTop.y += (m_nThumHeight+ m_nTitleTopSpace);

		CPoint pointBottom;
		pointBottom.x = data.m_ptLocation.x + m_nThumWidth;
		pointBottom.y = pointTop.y + m_nTitleHeight;

		CRect rectPrtSceneName(pointTop + ptVpOrg, pointBottom + ptVpOrg);
		int nStrLen = (int)wcslen(strPrtSceneName);

		//�����쿡 �̹����� �̸��� ���
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

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	onRefreshScroll();
}

void LmUIThumbNailCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	POSITION pPos = m_listThumb.GetHeadPosition();
	CRectItem data;

	//�����Ͱ� �������� �׳� ����~
	if(!m_listThumb.GetCount())
		return;

	SetFocus();
	while(pPos)
	{
		data = m_listThumb.GetNext(pPos);
		CPoint pointBottom(data.m_ptLocation.x + m_nThumWidth , 
			data.m_ptLocation.y + m_nThumHeight + m_nTitleTopSpace + m_nTitleHeight);
		CRect rectArea(data.m_ptLocation , pointBottom);

		//Ŭ���� ��ġ�� �׸��� ���� ������ ���ٸ�
		//�� �׸����ڸ� �β��� �׷��ش�.
		CClientDC dc(this);

		//ScrollView::OnPrepareDC(&dc)�� ���� �ڵ带 ���� ����.
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


			//Scene ��ȭ �Լ��� �θ��� ��.
			OnSelectedItem(data.strItemName);
			//data.strItemName //--- ������ �̸�!


			break;
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void LmUIThumbNailCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

//���콺 ���� ó���ϱ� ���� �Լ�
BOOL LmUIThumbNailCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if(zDelta<0)
		SendMessage(WM_VSCROLL, MAKELONG(SB_PAGEDOWN,0), 0);
	else
		SendMessage(WM_VSCROLL, MAKELONG(SB_PAGEUP,0), 0);
		

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}


//�����޽����� ����ϱ� ���� �Լ�
VOID LmUIThumbNailCtrl::SetErrorMsg( LPCTSTR lpszErrMsg )
{
	wcsncpy_s(m_szErrMsg, lpszErrMsg, MAXIMUM_ERR_MSG);	
}

//��ũ���� ���ִ� �Լ�
void LmUIThumbNailCtrl::ThmScroll( CSize sizeInput )
{
	//SendMessage(LVM_SCROLL, sizeInput.cx, sizeInput.cy);
}


void LmUIThumbNailCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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

	//���ο� ��ġ�� �ּ��� 0 �̻� 
	if(m_yPos + yIncrease < 0)
		yIncrease = -m_yPos; //��... 

	// ���ο� ��ġ�� �ִ��� yMax ����
	// ���� ��ũ�� Thumb ũ�� ������ ��ũ�� ũ�� ������ �޶�� ��.
	// ��ũ�� ũ�� ������ Thumb ũ�� ���� ���� ��ũ�� ȭ���� Ŭ���̾�Ʈ ���� ��ŭ�� �۴�.
	INT ynMax = m_yMax- rectView.Height();
	if( (m_yPos + yIncrease) > ynMax)
	{
		yIncrease = ynMax - m_yPos;
	}

	//���ο� ��ġ ���
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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	m_ClassName = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW,0,(HBRUSH)::GetStockObject(NULL_BRUSH),0);
	cs.lpszClass = m_ClassName;

	return CWnd::PreCreateWindow(cs);
}

BOOL LmUIThumbNailCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	return TRUE;
	//return CWnd::OnEraseBkgnd(pDC);
}

void LmUIThumbNailCtrl::SetThumbNailCaret( UINT nCaret )
{
	POSITION pPos = m_listThumb.GetHeadPosition();
	CRectItem data;

	//�����Ͱ� �������� �׳� ����~
	if(!m_listThumb.GetCount())
		return;

	SetFocus();
	while(pPos)
	{
		data = m_listThumb.GetNext(pPos);
		CPoint pointBottom(data.m_ptLocation.x + m_nThumWidth , 
			data.m_ptLocation.y + m_nThumHeight + m_nTitleTopSpace + m_nTitleHeight);
		CRect rectArea(data.m_ptLocation , pointBottom);

		//Ŭ���� ��ġ�� �׸��� ���� ������ ���ٸ�
		//�� �׸����ڸ� �β��� �׷��ش�.
		CClientDC dc(this);

		//ScrollView::OnPrepareDC(&dc)�� ���� �ڵ带 ���� ����.
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

			//��� ������ ���� Caret�� ������ ��� �޾� �д�.
			m_ThumbNailCaret.m_ptLocation = data.m_ptLocation;
			m_ThumbNailCaret.nImgIndex = data.nImgIndex;
			m_ThumbNailCaret.nIndex = data.nIndex;
			wcsncpy_s(m_ThumbNailCaret.strItemName, data.strItemName, wcslen(data.strItemName));

			//Scene ��ȭ �Լ��� �θ��� ��.
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
	// [����]
	
	switch(m_itemType)
	{
	// CHARECTERS - ĳ���Ͱ� ���õǾ����� ��ƾ
	case CHARECTERS :
		TheCartoonMaker->SelectChar(strItemName.GetBuffer(0));
		break;
	// THINGS - ��ü�� ���õǾ����� ��ƾ
	case THINGS:
		TheCartoonMaker->SelectThing(strItemName.GetBuffer(0));
		break;
	// BACKGROUNDS - ����� ���õǾ����� ��ƾ
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
