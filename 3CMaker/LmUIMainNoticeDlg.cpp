// LmUINoticeForm.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUIMainNoticeDlg.h"


// LmUINoticeForm 대화 상자입니다.

IMPLEMENT_DYNAMIC(LmUIMainNoticeDlg, CDialog)

LmUIMainNoticeDlg::LmUIMainNoticeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(LmUIMainNoticeDlg::IDD, pParent)
{

}

LmUIMainNoticeDlg::~LmUIMainNoticeDlg()
{
}

void LmUIMainNoticeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(LmUIMainNoticeDlg, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// LmUINoticeForm 메시지 처리기입니다.

void LmUIMainNoticeDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.

	CDC MemDC;
	MemDC.CreateCompatibleDC(&dc);

	//비트맵 로딩
	CBitmap bitmapTiTi;
	CBitmap bitmapTiTiMask;

	bitmapTiTi.LoadBitmap(IDB_TITI);
	bitmapTiTiMask.LoadBitmap(IDB_TITI_MASK);


	BITMAP bitinfoTiTi, bitInfoTiTiMask;

	bitmapTiTi.GetBitmap(&bitinfoTiTi);
	bitmapTiTiMask.GetBitmap(&bitInfoTiTiMask);

	CBitmap *OldBitmap = MemDC.SelectObject(&bitmapTiTiMask);

	dc.BitBlt(110,120,bitInfoTiTiMask.bmWidth,bitInfoTiTiMask.bmHeight,&MemDC,0,0,SRCAND);	

	MemDC.SelectObject(&bitmapTiTi);

	dc.BitBlt(110,120,bitinfoTiTi.bmWidth,bitinfoTiTi.bmHeight,&MemDC,0,0,SRCPAINT);	
	
}
