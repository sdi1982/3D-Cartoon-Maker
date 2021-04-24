// LmUICameraForm.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUICameraForm.h"

#include "MainFrm.h"
#include "LmUITimeLineView.h"


// LmUICameraForm 대화 상자입니다.

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


// LmUICameraForm 메시지 처리기입니다.

void LmUICameraForm::OnPaint()
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

HBRUSH LmUICameraForm::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	pDC->SetBkColor(RGB(255,255,255));
	return m_brush;
}

BOOL LmUICameraForm::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void LmUICameraForm::SetCameraUIValue( FLOAT x, FLOAT y, FLOAT z , FLOAT d)
{
	m_fCameraXVal = x;		// 카메라 X 좌표값
	m_fCameraYVal = y;		// 카메라 Y 좌표값
	m_fCameraZVal = z;		// 카메라 Z 좌표값

	m_fDist = d;
	UpdateData(FALSE);
}
void LmUICameraForm::OnBnClickedDefaultCamera()
{
	TheCartoonMaker->ResetCameraEyePt();
}

BOOL LmUICameraForm::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	D3DVECTOR vEye = *TheCartoonMaker->GetCameraEyePt();
	TheCartoonMaker->SetCameraEyePt(vEye.x - (FLOAT)pNMUpDown->iDelta, vEye.y, vEye.z);
}

void LmUICameraForm::OnDeltaposYValSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	D3DVECTOR vEye = *TheCartoonMaker->GetCameraEyePt();
	TheCartoonMaker->SetCameraEyePt(vEye.x, vEye.y - (FLOAT)pNMUpDown->iDelta, vEye.z);
}

void LmUICameraForm::OnDeltaposZValSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	D3DVECTOR vEye = *TheCartoonMaker->GetCameraEyePt();
	TheCartoonMaker->SetCameraEyePt(vEye.x, vEye.y, vEye.z - (FLOAT)pNMUpDown->iDelta);
}

void LmUICameraForm::OnDeltaposDistanceValSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	TheCartoonMaker->AddCameraDist((FLOAT)-pNMUpDown->iDelta);
}

void LmUICameraForm::OnBnClickedUserViewVal()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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