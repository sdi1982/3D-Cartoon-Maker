// 3CMakerView.cpp : CMy3CMakerView 클래스의 구현
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
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CMy3CMakerView 생성/소멸

CMy3CMakerView::CMy3CMakerView()
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CMy3CMakerView::~CMy3CMakerView()
{
}

BOOL CMy3CMakerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CMy3CMakerView 그리기

void CMy3CMakerView::OnDraw(CDC* /*pDC*/)
{
	CMy3CMakerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CMy3CMakerView 인쇄

BOOL CMy3CMakerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CMy3CMakerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CMy3CMakerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CMy3CMakerView 진단

#ifdef _DEBUG
void CMy3CMakerView::AssertValid() const
{
	CView::AssertValid();
}

void CMy3CMakerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMy3CMakerDoc* CMy3CMakerView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{ 
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy3CMakerDoc)));
	return (CMy3CMakerDoc*)m_pDocument;
}
#endif //_DEBUG



// CMy3CMakerView 메시지 처리기


