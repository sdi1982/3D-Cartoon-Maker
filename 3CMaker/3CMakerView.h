// 3CMakerView.h : CMy3CMakerView Ŭ������ �������̽�
//


#pragma once


class CMy3CMakerView : public CView
{
protected: // serialization������ ��������ϴ�.
	CMy3CMakerView();
	DECLARE_DYNCREATE(CMy3CMakerView)

// Ư���Դϴ�.
public:
	CMy3CMakerDoc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �����Դϴ�.
public:
	virtual ~CMy3CMakerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
public:

};

#ifndef _DEBUG  // 3CMakerView.cpp�� ����� ����
inline CMy3CMakerDoc* CMy3CMakerView::GetDocument() const
   { return reinterpret_cast<CMy3CMakerDoc*>(m_pDocument); }
#endif

