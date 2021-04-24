// LmUIFireWorksForm.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUIFireWorksForm.h"



// LmUIFireWorksForm ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(LmUIFireWorksForm, CDialog)

LmUIFireWorksForm::LmUIFireWorksForm(CWnd* pParent /*=NULL*/)
	: CDialog(LmUIFireWorksForm::IDD, pParent)
{

}

LmUIFireWorksForm::~LmUIFireWorksForm()
{
}

void LmUIFireWorksForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(LmUIFireWorksForm, CDialog)
	ON_BN_CLICKED(IDC_FIREWORKS_INSERT, &LmUIFireWorksForm::OnBnClickedFireworksInsert)
	ON_BN_CLICKED(IDC_FIREWORKS_DELETE, &LmUIFireWorksForm::OnBnClickedFireworksDelete)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// LmUIFireWorksForm �޽��� ó�����Դϴ�.

void LmUIFireWorksForm::OnBnClickedFireworksInsert()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	INT nCurFrame = TheCartoonMaker->GetCurFrame();

	if(!TheCartoonMaker->InsertFireworkEffect(nCurFrame))
	{
		AfxMessageBox(L"�Ҳɳ��̴� 40�������̸�, �ߺ��ؼ� ����� �� �����ϴ�.");
	}

	// ���� ȭ���� �����Ѵ�.
	UINT iCurFrame = TheCartoonMaker->GetCurFrame();
	TheCartoonMaker->SetCurFrame(iCurFrame);
}

void LmUIFireWorksForm::OnBnClickedFireworksDelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	INT nCurFrame = TheCartoonMaker->GetCurFrame();

	if(!TheCartoonMaker->RemoveFireworkEffect(nCurFrame))
	{
		AfxMessageBox(L"�Ҳɳ��� ������ ������ �� �����ϴ�.");
	}

	// ���� ȭ���� �����Ѵ�.
	UINT iCurFrame = TheCartoonMaker->GetCurFrame();
	TheCartoonMaker->SetCurFrame(iCurFrame);
}

void LmUIFireWorksForm::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CDialog::OnPaint()��(��) ȣ������ ���ʽÿ�.
	CRect rect;
	GetClientRect(&rect);
	dc.SelectStockObject(WHITE_PEN);
	dc.SelectStockObject(WHITE_BRUSH);
	dc.Rectangle(&rect);
}

HBRUSH LmUIFireWorksForm::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkColor(RGB(255,255,255));
	return m_brush;
}
