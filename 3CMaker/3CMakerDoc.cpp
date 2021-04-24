// 3CMakerDoc.cpp : CMy3CMakerDoc 클래스의 구현
//

#include "stdafx.h"
#include "3CMaker.h"
#include "3CMakerDoc.h"
#include "MainFrm.h"
#include "LmUIMotionThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy3CMakerDoc

IMPLEMENT_DYNCREATE(CMy3CMakerDoc, CDocument)

BEGIN_MESSAGE_MAP(CMy3CMakerDoc, CDocument)
END_MESSAGE_MAP()


// CMy3CMakerDoc 생성/소멸

CMy3CMakerDoc::CMy3CMakerDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

	//창이 생성되지 않아서 불가능함.
	//CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
	//m_pTimeLineView = (LmUITimeLineView*) ( pMainFrame->m_wndSplitRow.GetPane(1, 0) );

	//m_pMotionThread = NULL;
	//m_pMotionThread = new LmUIMotionThread(m_pTimeLineView);
	//m_pMotionThread->CreateThread();


}

CMy3CMakerDoc::~CMy3CMakerDoc()
{
	//m_pMotionThread->PostThreadMessage(WM_QUIT, 0, 0);
	//m_pMotionThread = NULL;
}

BOOL CMy3CMakerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CMy3CMakerDoc serialization

void CMy3CMakerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}


// CMy3CMakerDoc 진단

#ifdef _DEBUG
void CMy3CMakerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMy3CMakerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMy3CMakerDoc 명령
