// LmUIThumbnailView.h : CLmUIThumbnailView Ŭ������ �������̽�
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

	CPoint m_ptLocation;		  //������� ��ǥ
	INT nImgIndex;				  //����� �׸��� ����� �̹��� ����Ʈ�� �ε��� ��
	UINT nIndex;				  //������� �ε��� ��.
	WCHAR strSceneName[MAX_STR];  //Scene �̸�
	
	//CList Templete���� Find �Լ��� ����ϱ� ���� ������ �������̵��� ����.
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
protected: // serialization������ ��������ϴ�.
	LmUISceneView();
	DECLARE_DYNCREATE(LmUISceneView)

// Ư���Դϴ�.
public:

	HBITMAP	LoadPicture(LPCTSTR szBMPFileName);						//�׸������� �о���̴� �Լ�
	HBITMAP	LoadedPicture(IPicture* pPic);							//�׸��� �о���̴� �Լ�
	VOID	InitSceneView();										//SceneView�� �ʱ�ȭ ��Ű�� �Լ�

	INT		AddThumbNail(LPCTSTR szBMPFileName, LPCTSTR szSceneName);		//����� �߰�
	INT		InsertThumbNail(LPCTSTR szBMPFileName, LPCTSTR szSceneName);	//����� ����
	VOID	RemoveThumbNail(UINT ThumIndex);								//����� ����
	INT		ReplaceThumbNail(UINT ThumIndex, IPicture *pPic);				//������� �׸� ����

	INT		CopyThumbNail();												//���� ������� �޸𸮿� �����Ѵ�.
	INT		PasteThumbNail();												//�޸𸮿� �ִ� ������� ���� Ŀ�� �ٷ� �ڿ� �ٿ��ִ´�.
	INT		CutThumbNail();													//���� ������� �޸𸮿� ������ �� ���� ���� ���� �ع�����. 

	VOID	InvalidateCaret();										//Ŀ���� �ٽ� �׸���.
	VOID	onRefreshScroll();										//��ũ�� ũ�⸦ �ٽ� ������ �ش�.
	VOID	ThmDrawCaret(CPoint leftTop);							//ĳ���� �׷��ִ� �Լ�
	VOID	LoadThumbNail();										//������� ����Ʈ�� ��Ͻ����ִ� �Լ� - �ӽ�

//	VOID	onLmChangeSize();										//Client ������ ������ ���� �� ó���� �ִ� �Լ�
	VOID	SetThumbNailCaret(UINT nCaret);							//ĳ���� �����ϰ� �׸��� �ٲ��ִ� �Լ�
	VOID	GetThumbNailCaret(CRectPicture & getCaretData);		    //���� ĳ���� �����ϴ� �Լ�
	INT		GetThumbNailCaret();									//���� ĳ���� ������ ������ �Լ�

	BOOL	GetChangeSceneSw();										//���� Wizard�� ���������� �˷��ִ� �Լ���.

	INT		GetImageHeight() { return m_nImageHeight; }				
	INT		GetImageWidth() { return m_nImageWidth; }
	INT		GetThumNailCount() { return (INT)m_listThumb.GetSize(); }

	VOID	EnableChangeScene(BOOL bEnable);

	CBitmap* MakeImage( CImageList * pImageList, 
										INT nImageIndex);		//�̹��� ����Ʈ���� CBitmap���� �̾Ƴ��� �Լ�

	void     CopyImgListTOMemList( CRectPicture &rCaretData );	//�̹��� ����Ʈ���� �޸𸮷� ���
	INT      CopyMemListTOImgList();							//�޸𸮿� �ִ� �̹����� �̹��� ����Ʈ�� �ٽ� ����
	INT		 IsThereSceneName( LPCWSTR pcwsSceneName );			//Scene ThumbNail�� �̹� �����ϴ��� �̸����� �˻��Ѵ�. 

	INT		SaveSceneView(CFile & File);						//SceneView ������ ���� ��ƾ
	INT		LoadSceneView(CFile & File);						//SceneView �ε��� ���� ��ƾ

private:
	//���� ó���� ���� ��� �Լ�
	LPCTSTR		GetLastError() const { return m_szErrMsg; }
	VOID		SetErrorMsg(LPCTSTR lpszErrMsg);

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�. //��ϵ� ������� ȭ�鿡 ���
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate(); // ���� �� ó�� ȣ��Ǿ����ϴ�.
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	
// �����Դϴ�.
public:
	virtual ~LmUISceneView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// ��� ����
public:
	RectPicVec		m_vectorMemThumb;	    //����, �ڸ���, ���̱⿡ ���� ����
	CImageList		m_imageListMem;			//����, �ڸ���, ���̱⿡ ���� ����� �̹������� ��Ƶ� �̹��� ����Ʈ

	CImageList		m_imageList;			//����� �̹������� ��Ƶ� �̹��� ����Ʈ
	INT				m_nRowSpace;			//����� ������ ���� ����
	INT				m_nLeftSpace;		 	//����� ���� ����
	PictureList		m_listThumb;			//����� ����(��ġ��ǥ��, ������ε���)�� ������ �ִ� ����� �Ӽ� ����Ʈ
	INT				m_nThumWidth;			//��µǴ� ����� 1���� ���� 
	INT				m_nThumHeight;			//��µǴ� ����� 1���� ����

	INT				m_nImageWidth;			//����� m_imageList �� ����
	INT				m_nImageHeight;			//����� m_imageList �� ����

	INT				m_nTitleTopSpace;		//����ϰ� ������� ���Ͽ���
	INT				m_nTitleHeight;			//���� RECT�� Height

	INT				m_nCaret;				//���� ĳ���� ��ġ(����� �ε������� ����) - 0 based Index!
	CRect			m_rectPreCaret;			//���� ĳ���� �簢�� ũ�� (invalidaterect�� ����)

	BOOL			m_isLoadComplete;		//�����찡 ������ �ε��Ǿ��°�? OnSize�� �����찡 ���� �߱����� �ѹ� ȣ��Ǳ� ����
	CSize			m_sizeWin;				//�� View�� ũ�⸦ ������ �ִ�. 
											//WM_SIZE�� ���� ��� ���� 4�� ���� ȣ��Ǳ� ������ ũ�Ⱑ ����Ǿ�������
											//�ٽ� �ε��ϱ� ����.

	CRect			m_rectWin;				// View�� ��ü Screen ������ Rect �������� ������ �ִ�. 
		
	TCHAR			m_szErrMsg[MAXIMUM_ERR_MSG];	//���� �޽����� ���� �ֱ� ���� ��Ʈ��
	
	CWinThread*		m_pThreadLoadPicture;   // ����� �ε��� ���� ������
	CRectPicture	m_ThumbNailCaret;		//���� ����� Ŀ���� ������ ������ �ִ� ��� ����

	UINT			m_nIndexCnt;			// ������� �ϳ��� �߰�(�Ǵ� ����)�� �� ���� �ϳ��� �þ
											// ���� ����⸦ �� �� �ʱ�ȭ �Ǿ�� �� �κ���.
	
	BOOL			m_bChangeScene;		// Wizard�� ���� �ɶ��� �ٸ� ������ ���õǾ�� �ȵȴ�.


// ������ �޽��� �� �Լ�
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


