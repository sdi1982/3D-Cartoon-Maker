#pragma once


#define MAXIMUM_ERR_MSG 255
#define MAX_STR 128
typedef struct PictureItem
{
	CPoint m_ptLocation;		  //������� ��ǥ
	INT nImgIndex;				  //����� �׸��� ����� �̹��� ����Ʈ�� �ε��� ��
	UINT nIndex;				  //������� �ε��� ��.
	WCHAR strItemName[MAX_STR];   //Item �̸�
}CRectItem;

enum ITEMTYPE {NOTHING ,CHARECTERS=1, THINGS, BACKGROUNDS};


// LmUIThumbNailCtrl
typedef CList<CRectItem, CRectItem &> ItemList;

class LmUIThumbNailCtrl : public CWnd
{
	DECLARE_DYNAMIC(LmUIThumbNailCtrl)
	
	//��� �Լ�
public:
	LmUIThumbNailCtrl();
	virtual ~LmUIThumbNailCtrl();

	HBITMAP LoadPicture(LPCTSTR szBMPFileName);						//�׸������� �о���̴� �Լ�
	void	ThmInsert(LPCTSTR szBMPFileName, LPCTSTR szSceneName);	//����� ���
	void	onRefreshScroll();										//��ũ�� ũ�⸦ �ٽ� ������ �ش�.
	void	ThmDrawCaret(CPoint leftTop);							//ĳ���� �׷��ִ� �Լ�
	void	ThmDrawCaret(CPoint leftTop , CDC & dcMem);				//ĳ���� �׷��ִ� �Լ� - OnDraw���� ���
	void	ThmScroll(CSize sizeInput);								//��ũ���� �ϱ� ���� �Լ�

	void	SetThumbNailCaret(UINT nCaret);							//ĳ���� �����ϰ� �׸��� �ٲ��ִ� �Լ�
	void	GettThumbNailCaret(CRectItem & getCaretData);			//���� ĳ���� ������ ������ �Լ�
	
	void	SetItemType(ITEMTYPE itemtype);							//ItemType�� �����Ѵ�.
	void	OnSelectedItem(CString strItemName);

private:
	//���� ó���� ���� ��� �Լ�
	LPCTSTR		GetLastError() const { return m_szErrMsg; }
	VOID		SetErrorMsg(LPCTSTR lpszErrMsg);


	// ��� ����
public:
	ITEMTYPE		m_itemType;				//������� �ٷ�� �������� Ÿ���� �����ش�.
	CImageList		m_imageList;			//����� �̹������� ��Ƶ� �̹��� ����Ʈ
	INT				m_nRowSpace;			//����� ������ ���� ����
	INT				m_nLeftSpace;		 	//����� ���� ����
	INT				m_nSecLeftSpace;		//����� ù��° ���� �ι�° �� ������ ����
	ItemList		m_listThumb;			//����� ����(��ġ��ǥ��, ������ε���)�� ������ �ִ� ����� �Ӽ� ����Ʈ
	INT				m_nThumWidth;			//����� 1���� ����
	INT				m_nThumHeight;			//����� 1���� ����
	INT				m_nTitleTopSpace;		//����ϰ� ������� ���Ͽ���
	INT				m_nTitleHeight;			//���� RECT�� Height
	
	CRect			m_rectPreCaret;			//���� ĳ���� �簢�� ũ�� (invalidaterect�� ����) ���� ���� �ణ ũ��

	TCHAR			m_szErrMsg[MAXIMUM_ERR_MSG];				//���� �޽����� ���� �ֱ� ���� ��Ʈ��
	
	//������ ������ ����ϴ� ����
	CString			m_ClassName;

	//��ũ���� ���� ����
	INT				m_yPos;
	INT				m_yMax;

	INT				m_nCaret;				//���� ĳ���� ��ġ(����� �ε������� ����) - 0 based Index!
	CRectItem		m_ThumbNailCaret;		//���� ����� Ŀ���� ������ ������ �ִ� ��� ����

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


