#pragma once
#include "LmTimeLineManager.h"
#include "LmUIMotionThread.h"

class CHoverButton;
class CMy3CMakerDoc;

// CTimeLineView ���Դϴ�.
typedef list<LmLayer> 			LayerList;
typedef LayerList::iterator		LayerListItr;

class LmUITimeLineView :public CView
{
	DECLARE_DYNCREATE(LmUITimeLineView)

protected:
	LmUITimeLineView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~LmUITimeLineView();
public:
	virtual void OnDraw(CDC* pDC);      // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif


//����Լ�
public:
	//CMy3CMakerDoc* GetDocument() const;

	//Ÿ�Ӷ��� Caret �����Լ�
	void SetCurrFrame(INT nFrame);										//Caret�� ���° �����ӿ� �׸��� �����Ѵ�.
	UINT GetCurrFrame();												//Caret�� ���° �����ӿ� �ִ��� �����Ѵ�.

	//UI ���� �Լ�
	void OnDrawMain();
	void OnDrawLeft();
	void OnDrawTop();
	void OnExceptButtonArea(CClientDC &dc);											//��ư ������ ���� �ϱ� ����
	void GetButtonRect(CRect & rect, CHoverButton & button);						//Hover Button�� ũ�⸦ ��� �ش�.
	void ReFreshVertScroll();														//Vertical Scroll�� �ٽ� �����Ѵ�.
	INT  GoNextCaret();
	INT  GoPrevCaret();																//���� ĭ���� �̵�
	void ChangeStopToPlayBtn();														//Stop ��ư���� Play��ư���� ����
	void ChangePlaytoStopBtn();														//Play ��ư���� Stop��ư���� ����
	BOOL IsDrawRectInMemDC(INT nWidthMemDC, INT nHeightMemDC, CRect rectSection);	//Ÿ�Ӷ��ο� Section �簢���� �׷��� ������ ������ �ִ� �Լ�.


//�������
public:

	CSize						m_sizeView;				//View�� ũ��
	CScrollBar					m_scrVertical;			//���� ��ũ�ѹ�
	CScrollBar					m_scrHorizontal;		//���� ��ũ�ѹ�
	INT							m_nScrWidth;			//��ũ�ѹ� ����
	BOOL						m_isLoadComplete;		//�����찡 ������ �ε��Ǿ��°�? OnSize�� �����찡 ���� �߱����� �ѹ� ȣ��Ǳ� ����
	
	INT							m_nRowSize;				//���̾� ���� ����
	INT							m_nLayerColSize;		//���̾� �̸� ĭ�� ���� ũ��
	INT							m_nFrmColSize;			//frame 1ĭ�� ���� ����
	INT							m_nCellSpacing;			//�簢�� ���� ����
	INT							m_nLeftSpacing;			//���ڸ� ���� ���� �簢�� ���� ����(CellSpaing �ٷ� ���� ���� ����)
	INT							m_nLayerNameLength;		//Layer �̸��� ���� ����

	CPoint						m_ptScrPos;				//��ũ�� ��ġ
	CPoint						m_ptScrMax;				//��ũ�� �ִ� ũ��

	CHoverButton				m_hBtnPlay;				//Play��ư / Stop ��ư�� Toggle �������� ���
	CHoverButton				m_hBtnStop;				//Stop��ư / Play ��ư�� Toggle �������� ���
	CHoverButton				m_hBtnNextFrm;			//���� ���������� ���� ��ư
	CHoverButton				m_hBtnPrevFrm;			//���� ���������� ���� ��ư
	CHoverButton				m_hBtnFirstFrm;			//ù��° ���������� ���� ��ư
	CHoverButton				m_hBtnLastFrm;			//������ ���������� ���� ��ư
	CHoverButton				m_hBtnConfig;			//���� ��ư

	UINT						m_nPrevSectionPos;		//ī�޶� Section�� ���� �� �κ��� �׸��� ���� point��.

	BOOL						m_isPlaying;			//�÷��� ���ΰ�? Yes�� TRUE, NO�� FALSE

	LmUIMotionThread*			m_pMotionThread;		//Ÿ�Ӷ��� �� ��ü UI�� �����ϴ� Thread��.
								
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBtnClickedPlay();
	afx_msg void OnBtnClickedStop();
	afx_msg void OnBtnClickedNext();
	afx_msg void OnBtnClickedPrev();
	afx_msg void OnBtnClickedFirst();
	afx_msg void OnBtnClickedLast();
	afx_msg void OnBtnClickedConfig();

	void ReSizeFrameLength( INT nListSize, INT nFrameLength );
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};


