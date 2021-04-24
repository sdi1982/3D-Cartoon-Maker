// LmUIFireWorksForm.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUIFireWorksForm.h"



// LmUIFireWorksForm 대화 상자입니다.

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


// LmUIFireWorksForm 메시지 처리기입니다.

void LmUIFireWorksForm::OnBnClickedFireworksInsert()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT nCurFrame = TheCartoonMaker->GetCurFrame();

	if(!TheCartoonMaker->InsertFireworkEffect(nCurFrame))
	{
		AfxMessageBox(L"불꽃놀이는 40프레임이며, 중복해서 사용할 수 없습니다.");
	}

	// 현재 화면을 갱신한다.
	UINT iCurFrame = TheCartoonMaker->GetCurFrame();
	TheCartoonMaker->SetCurFrame(iCurFrame);
}

void LmUIFireWorksForm::OnBnClickedFireworksDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT nCurFrame = TheCartoonMaker->GetCurFrame();

	if(!TheCartoonMaker->RemoveFireworkEffect(nCurFrame))
	{
		AfxMessageBox(L"불꽃놀이 섹션을 삭제할 수 없습니다.");
	}

	// 현재 화면을 갱신한다.
	UINT iCurFrame = TheCartoonMaker->GetCurFrame();
	TheCartoonMaker->SetCurFrame(iCurFrame);
}

void LmUIFireWorksForm::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.
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
