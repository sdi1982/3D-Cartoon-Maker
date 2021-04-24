// 3CMakerView.cpp : CMy3CMakerView Ŭ������ ����
//

#include "stdafx.h"
#include "3CMaker.h"

#include "3CMakerDoc.h"
#include "3CMakerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy3CMakerView

IMPLEMENT_DYNCREATE(CMy3CMakerView, CView)

BEGIN_MESSAGE_MAP(CMy3CMakerView, CView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CMy3CMakerView ����/�Ҹ�

CMy3CMakerView::CMy3CMakerView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CMy3CMakerView::~CMy3CMakerView()
{
}

BOOL CMy3CMakerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CMy3CMakerView �׸���

void CMy3CMakerView::OnDraw(CDC* /*pDC*/)
{
	CMy3CMakerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CMy3CMakerView �μ�

BOOL CMy3CMakerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CMy3CMakerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CMy3CMakerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CMy3CMakerView ����

#ifdef _DEBUG
void CMy3CMakerView::AssertValid() const
{
	CView::AssertValid();
}

void CMy3CMakerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMy3CMakerDoc* CMy3CMakerView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{ 
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy3CMakerDoc)));
	return (CMy3CMakerDoc*)m_pDocument;
}
#endif //_DEBUG



// CMy3CMakerView �޽��� ó����


