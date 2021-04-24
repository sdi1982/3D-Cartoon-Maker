// LmUICameraForm.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUICameraForm.h"

#include "MainFrm.h"
#include "LmUITimeLineView.h"


// LmUICameraForm ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(LmUICameraForm, CDialog)

LmUICameraForm::LmUICameraForm(CWnd* pParent /*=NULL*/)
	: CDialog(LmUICameraForm::IDD, pParent)
	, m_fCameraXVal(0)
	, m_fCameraYVal(0)
	, m_fCameraZVal(0)
	, m_fDist(0)
{
	m_brush.CreateSolidBrush(RGB(255,255,255));
}

LmUICameraForm::~LmUICameraForm()
{
}

void LmUICameraForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_XVAL, m_ctrlCameraXVal);
	DDX_Text(pDX, IDC_XVAL, m_fCameraXVal);
	DDX_Control(pDX, IDC_YVAL, m_ctrlCameraYVal);
	DDX_Text(pDX, IDC_YVAL, m_fCameraYVal);
	DDX_Control(pDX, IDC_ZVAL, m_ctrlCameraZVal);
	DDX_Text(pDX, IDC_ZVAL, m_fCameraZVal);
	DDX_Text(pDX, IDC_DIST, m_fDist);
}


BEGIN_MESSAGE_MAP(LmUICameraForm, CDialog)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_DEFAULT_CAMERA, &LmUICameraForm::OnBnClickedDefaultCamera)
	ON_NOTIFY(UDN_DELTAPOS, IDC_X_VAL_SPIN, &LmUICameraForm::OnDeltaposXValSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_Y_VAL_SPIN, &LmUICameraForm::OnDeltaposYValSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_Z_VAL_SPIN, &LmUICameraForm::OnDeltaposZValSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DISTANCE_VAL_SPIN, &LmUICameraForm::OnDeltaposDistanceValSpin)
	ON_BN_CLICKED(IDC_USER_VIEW_VAL, &LmUICameraForm::OnBnClickedUserViewVal)
	ON_BN_CLICKED(IDC_CAMKEY_DEL, &LmUICameraForm::OnBnClickedCamkeyDel)
END_MESSAGE_MAP()


// LmUICameraForm �޽��� ó�����Դϴ�.

void LmUICameraForm::OnPaint()
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

HBRUSH LmUICameraForm::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkColor(RGB(255,255,255));
	return m_brush;
}

BOOL LmUICameraForm::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void LmUICameraForm::SetCameraUIValue( FLOAT x, FLOAT y, FLOAT z , FLOAT d)
{
	m_fCameraXVal = x;		// ī�޶� X ��ǥ��
	m_fCameraYVal = y;		// ī�޶� Y ��ǥ��
	m_fCameraZVal = z;		// ī�޶� Z ��ǥ��

	m_fDist = d;
	UpdateData(FALSE);
}
void LmUICameraForm::OnBnClickedDefaultCamera()
{
	TheCartoonMaker->ResetCameraEyePt();
}

BOOL LmUICameraForm::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_RETURN)
		return TRUE;

	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void LmUICameraForm::OnDeltaposXValSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	D3DVECTOR vEye = *TheCartoonMaker->GetCameraEyePt();
	TheCartoonMaker->SetCameraEyePt(vEye.x - (FLOAT)pNMUpDown->iDelta, vEye.y, vEye.z);
}

void LmUICameraForm::OnDeltaposYValSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	D3DVECTOR vEye = *TheCartoonMaker->GetCameraEyePt();
	TheCartoonMaker->SetCameraEyePt(vEye.x, vEye.y - (FLOAT)pNMUpDown->iDelta, vEye.z);
}

void LmUICameraForm::OnDeltaposZValSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	D3DVECTOR vEye = *TheCartoonMaker->GetCameraEyePt();
	TheCartoonMaker->SetCameraEyePt(vEye.x, vEye.y, vEye.z - (FLOAT)pNMUpDown->iDelta);
}

void LmUICameraForm::OnDeltaposDistanceValSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	TheCartoonMaker->AddCameraDist((FLOAT)-pNMUpDown->iDelta);
}

void LmUICameraForm::OnBnClickedUserViewVal()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	TheCartoonMaker->InsertCameraKey();
}

void LmUICameraForm::OnBnClickedCamkeyDel()
{
	TheCartoonMaker->DeleteCameraKey();
}
	
LmUITimeLineView* LmUICameraForm::GetTimeLineView()
{
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
	LmUITimeLineView* pTimeLineView = (LmUITimeLineView*) ( pMainFrame->m_wndSplitRow.GetPane(1, 0) );
	return pTimeLineView;
}