// 3CMakerDoc.cpp : CMy3CMakerDoc Ŭ������ ����
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


// CMy3CMakerDoc ����/�Ҹ�

CMy3CMakerDoc::CMy3CMakerDoc()
{
	// TODO: ���⿡ ��ȸ�� ���� �ڵ带 �߰��մϴ�.

	//â�� �������� �ʾƼ� �Ұ�����.
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

	// TODO: ���⿡ ���ʱ�ȭ �ڵ带 �߰��մϴ�.
	// SDI ������ �� ������ �ٽ� ����մϴ�.

	return TRUE;
}




// CMy3CMakerDoc serialization

void CMy3CMakerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}


// CMy3CMakerDoc ����

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


// CMy3CMakerDoc ���
