// LmUIThumbnailView.h : CLmUIThumbnailView 클래스의 인터페이스
//


#pragma once
#include<vector>

#define MAXIMUM_ERR_MSG 255
#define MAX_STR 128
class CRectPicture
{
public:
	CRectPicture() {}
	~CRectPicture() {}

	CPoint m_ptLocation;		  //썸네일의 좌표
	INT nImgIndex;				  //썸네일 그림이 저장된 이미지 리스트의 인덱스 값
	UINT nIndex;				  //썸네일의 인덱스 값.
	WCHAR strSceneName[MAX_STR];  //Scene 이름
	
	//CList Templete에서 Find 함수를 사용하기 위해 연산자 오버라이딩을 했음.
	friend int operator==(const CRectPicture & leftPic,const CRectPicture & rightPic)
	{
		//if(leftPic.m_ptLocation != rightPic.m_ptLocation)
		//	return FALSE;
		//if(leftPic.nImgIndex != rightPic.nImgIndex)
		//	return FALSE;
		if(leftPic.nIndex != rightPic.nIndex)
			return FALSE;
//		if( wcscmp(leftPic.strSceneName, rightPic.strSceneName))
//			return FALSE;
		return TRUE;
	}
};



typedef vector<CRectPicture> RectPicVec;
typedef RectPicVec::iterator RectPicVecItr;

typedef CList<CRectPicture, CRectPicture &> PictureList;

class LmUISceneView : public CScrollView
{
protected: // serialization에서만 만들어집니다.
	LmUISceneView();
	DECLARE_DYNCREATE(LmUISceneView)

// 특성입니다.
public:

	HBITMAP	LoadPicture(LPCTSTR szBMPFileName);						//그림파일을 읽어들이는 함수
	HBITMAP	LoadedPicture(IPicture* pPic);							//그림을 읽어들이는 함수
	VOID	InitSceneView();										//SceneView를 초기화 시키는 함수

	INT		AddThumbNail(LPCTSTR szBMPFileName, LPCTSTR szSceneName);		//썸네일 추가
	INT		InsertThumbNail(LPCTSTR szBMPFileName, LPCTSTR szSceneName);	//썸네일 삽입
	VOID	RemoveThumbNail(UINT ThumIndex);								//썸네일 삭제
	INT		ReplaceThumbNail(UINT ThumIndex, IPicture *pPic);				//썸네일의 그림 수정

	INT		CopyThumbNail();												//현재 썸네일을 메모리에 복사한다.
	INT		PasteThumbNail();												//메모리에 있는 썸네일을 현재 커서 바로 뒤에 붙여넣는다.
	INT		CutThumbNail();													//현재 썸네일을 메모리에 복사한 후 현재 씬을 삭제 해버린다. 

	VOID	InvalidateCaret();										//커서를 다시 그린다.
	VOID	onRefreshScroll();										//스크롤 크기를 다시 지정해 준다.
	VOID	ThmDrawCaret(CPoint leftTop);							//캐럿을 그려주는 함수
	VOID	LoadThumbNail();										//썸네일의 리스트를 등록시켜주는 함수 - 임시

//	VOID	onLmChangeSize();										//Client 영역의 변경이 있을 때 처리해 주는 함수
	VOID	SetThumbNailCaret(UINT nCaret);							//캐럿을 설정하고 그림도 바꿔주는 함수
	VOID	GetThumbNailCaret(CRectPicture & getCaretData);		    //현재 캐럿을 세팅하는 함수
	INT		GetThumbNailCaret();									//현재 캐럿의 정보를 얻어오는 함수

	BOOL	GetChangeSceneSw();										//현재 Wizard가 실행중인지 알려주는 함수임.

	INT		GetImageHeight() { return m_nImageHeight; }				
	INT		GetImageWidth() { return m_nImageWidth; }
	INT		GetThumNailCount() { return (INT)m_listThumb.GetSize(); }

	VOID	EnableChangeScene(BOOL bEnable);

	CBitmap* MakeImage( CImageList * pImageList, 
										INT nImageIndex);		//이미지 리스트에서 CBitmap으로 뽑아내는 함수

	void     CopyImgListTOMemList( CRectPicture &rCaretData );	//이미지 리스트에서 메모리로 백업
	INT      CopyMemListTOImgList();							//메모리에 있는 이미지를 이미지 리스트로 다시 복사
	INT		 IsThereSceneName( LPCWSTR pcwsSceneName );			//Scene ThumbNail이 이미 존재하는지 이름으로 검색한다. 

	INT		SaveSceneView(CFile & File);						//SceneView 저장을 위한 루틴
	INT		LoadSceneView(CFile & File);						//SceneView 로딩을 위한 루틴

private:
	//에러 처리를 위한 멤버 함수
	LPCTSTR		GetLastError() const { return m_szErrMsg; }
	VOID		SetErrorMsg(LPCTSTR lpszErrMsg);

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다. //등록된 썸네일을 화면에 출력
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	
// 구현입니다.
public:
	virtual ~LmUISceneView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 멤버 변수
public:
	RectPicVec		m_vectorMemThumb;	    //복사, 자르기, 붙이기에 사용될 벡터
	CImageList		m_imageListMem;			//복사, 자르기, 붙이기에 사용될 썸네일 이미지들을 모아둘 이미지 리스트

	CImageList		m_imageList;			//썸네일 이미지들을 모아둘 이미지 리스트
	INT				m_nRowSpace;			//썸네일 사이의 상하 여백
	INT				m_nLeftSpace;		 	//썸네일 왼쪽 여백
	PictureList		m_listThumb;			//썸네일 정보(위치좌표값, 썸네일인덱스)를 가지고 있는 썸네일 속성 리스트
	INT				m_nThumWidth;			//출력되는 썸네일 1개의 넓이 
	INT				m_nThumHeight;			//출력되는 썸네일 1개의 높이

	INT				m_nImageWidth;			//저장된 m_imageList 의 넓이
	INT				m_nImageHeight;			//저장된 m_imageList 의 높이

	INT				m_nTitleTopSpace;		//썸네일과 제목과의 상하여백
	INT				m_nTitleHeight;			//제목 RECT의 Height

	INT				m_nCaret;				//현재 캐럿의 위치(썸네일 인덱스값을 가짐) - 0 based Index!
	CRect			m_rectPreCaret;			//이전 캐럿의 사각형 크기 (invalidaterect에 쓰임)

	BOOL			m_isLoadComplete;		//윈도우가 완전히 로딩되었는가? OnSize가 윈도우가 먼저 뜨기전에 한번 호출되기 때문
	CSize			m_sizeWin;				//이 View의 크기를 가지고 있다. 
											//WM_SIZE가 분할 뷰로 인해 4번 가량 호출되기 때문에 크기가 변경되었을때만
											//다시 로딩하기 위함.

	CRect			m_rectWin;				// View의 전체 Screen 윈도우 Rect 영역값을 가지고 있다. 
		
	TCHAR			m_szErrMsg[MAXIMUM_ERR_MSG];	//에러 메시지를 보내 주기 위한 스트링
	
	CWinThread*		m_pThreadLoadPicture;   // 썸네일 로딩을 위한 쓰레드
	CRectPicture	m_ThumbNailCaret;		//현재 썸네일 커서의 정보를 가지고 있는 멤버 변수

	UINT			m_nIndexCnt;			// 썸네일이 하나씩 추가(또는 삽입)될 때 마다 하나씩 늘어남
											// 새로 만들기를 할 때 초기화 되어야 할 부분임.
	
	BOOL			m_bChangeScene;		// Wizard가 진행 될때는 다른 씬으로 선택되어서는 안된다.


// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


