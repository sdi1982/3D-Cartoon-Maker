#pragma once


#define MAXIMUM_ERR_MSG 255
#define MAX_STR 128
typedef struct PictureItem
{
	CPoint m_ptLocation;		  //썸네일의 좌표
	INT nImgIndex;				  //썸네일 그림이 저장된 이미지 리스트의 인덱스 값
	UINT nIndex;				  //썸네일의 인덱스 값.
	WCHAR strItemName[MAX_STR];   //Item 이름
}CRectItem;

enum ITEMTYPE {NOTHING ,CHARECTERS=1, THINGS, BACKGROUNDS};


// LmUIThumbNailCtrl
typedef CList<CRectItem, CRectItem &> ItemList;

class LmUIThumbNailCtrl : public CWnd
{
	DECLARE_DYNAMIC(LmUIThumbNailCtrl)
	
	//멤버 함수
public:
	LmUIThumbNailCtrl();
	virtual ~LmUIThumbNailCtrl();

	HBITMAP LoadPicture(LPCTSTR szBMPFileName);						//그림파일을 읽어들이는 함수
	void	ThmInsert(LPCTSTR szBMPFileName, LPCTSTR szSceneName);	//썸네일 등록
	void	onRefreshScroll();										//스크롤 크기를 다시 지정해 준다.
	void	ThmDrawCaret(CPoint leftTop);							//캐럿을 그려주는 함수
	void	ThmDrawCaret(CPoint leftTop , CDC & dcMem);				//캐럿을 그려주는 함수 - OnDraw에서 사용
	void	ThmScroll(CSize sizeInput);								//스크롤을 하기 위한 함수

	void	SetThumbNailCaret(UINT nCaret);							//캐럿을 설정하고 그림도 바꿔주는 함수
	void	GettThumbNailCaret(CRectItem & getCaretData);			//현재 캐럿을 정보를 얻어오는 함수
	
	void	SetItemType(ITEMTYPE itemtype);							//ItemType을 지정한다.
	void	OnSelectedItem(CString strItemName);

private:
	//에러 처리를 위한 멤버 함수
	LPCTSTR		GetLastError() const { return m_szErrMsg; }
	VOID		SetErrorMsg(LPCTSTR lpszErrMsg);


	// 멤버 변수
public:
	ITEMTYPE		m_itemType;				//썸네일이 다루는 아이템의 타입을 보여준다.
	CImageList		m_imageList;			//썸네일 이미지들을 모아둘 이미지 리스트
	INT				m_nRowSpace;			//썸네일 사이의 상하 여백
	INT				m_nLeftSpace;		 	//썸네일 왼쪽 여백
	INT				m_nSecLeftSpace;		//썸네일 첫번째 열과 두번째 열 사이의 여백
	ItemList		m_listThumb;			//썸네일 정보(위치좌표값, 썸네일인덱스)를 가지고 있는 썸네일 속성 리스트
	INT				m_nThumWidth;			//썸네일 1개의 넓이
	INT				m_nThumHeight;			//썸네일 1개의 높이
	INT				m_nTitleTopSpace;		//썸네일과 제목과의 상하여백
	INT				m_nTitleHeight;			//제목 RECT의 Height
	
	CRect			m_rectPreCaret;			//이전 캐럿의 사각형 크기 (invalidaterect에 쓰임) 실제 보다 약간 크다

	TCHAR			m_szErrMsg[MAXIMUM_ERR_MSG];				//에러 메시지를 보내 주기 위한 스트링
	
	//윈도우 생성때 사용하는 변수
	CString			m_ClassName;

	//스크롤을 위한 변수
	INT				m_yPos;
	INT				m_yMax;

	INT				m_nCaret;				//현재 캐럿의 위치(썸네일 인덱스값을 가짐) - 0 based Index!
	CRectItem		m_ThumbNailCaret;		//현재 썸네일 커서의 정보를 가지고 있는 멤버 변수

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


