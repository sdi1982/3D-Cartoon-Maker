// LmUINoticeForm.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUIMainNoticeDlg.h"


// LmUINoticeForm ��ȭ �����Դϴ�.

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


// LmUINoticeForm �޽��� ó�����Դϴ�.

void LmUIMainNoticeDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CDialog::OnPaint()��(��) ȣ������ ���ʽÿ�.

	CDC MemDC;
	MemDC.CreateCompatibleDC(&dc);

	//��Ʈ�� �ε�
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
