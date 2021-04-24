// LmUISoundContentDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUISoundContentDlg.h"


// LmUISoundContentDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(LmUISoundContentDlg, CDialog)

LmUISoundContentDlg::LmUISoundContentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(LmUISoundContentDlg::IDD, pParent)
	, m_strSoundFileName(_T(""))
	, m_nStartFrm(0)
	, m_strSoundName(_T(""))
{
	m_isInsert = TRUE;
}

LmUISoundContentDlg::~LmUISoundContentDlg()
{
}

void LmUISoundContentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strSoundFileName);
	DDX_Text(pDX, IDC_EDIT2, m_nStartFrm);
	DDX_Text(pDX, IDC_SOUND_NAME, m_strSoundName);
	DDX_Control(pDX, IDC_EDIT1, m_ctrlFileFullName);
	DDX_Control(pDX, IDC_SOUND_NAME, m_ctrlSoundName);
	DDX_Control(pDX, IDC_EDIT2, m_ctrlStartFrm);
}


BEGIN_MESSAGE_MAP(LmUISoundContentDlg, CDialog)
	ON_BN_CLICKED(IDC_SOUND_FILE_OPEN, &LmUISoundContentDlg::OnBnClickedSoundFileOpen)
	ON_BN_CLICKED(IDOK, &LmUISoundContentDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// LmUISoundContentDlg 메시지 처리기입니다.

BOOL LmUISoundContentDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_RETURN)
		return TRUE;

	if(pMsg->message == WM_KEYDOWN &&
		pMsg->wParam ==  VK_ESCAPE)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void LmUISoundContentDlg::OnBnClickedSoundFileOpen()
{
	WCHAR szFilter[] = L"Wav Files(*.wav)|*.wav|모든 파일(*.*)|*.*|";
	CFileDialog fileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,szFilter);
	if(fileDlg.DoModal() == IDOK)
		m_ctrlFileFullName.SetWindowText(fileDlg.GetPathName());
}

void LmUISoundContentDlg::OnBnClickedOk()
{
	CString strFileFullName;
	CString strSoundName;
	CString strStartFrm;

	m_ctrlFileFullName.GetWindowText(strFileFullName);
	m_ctrlSoundName.GetWindowText(strSoundName);
	m_ctrlStartFrm.GetWindowText(strStartFrm);


	if(strFileFullName.IsEmpty() ||
		strSoundName.IsEmpty() ||
		strStartFrm.IsEmpty())
	{
		AfxMessageBox(L"사운드 정보를 모두 입력해 주세요!");
		return;
	}

	//Insert 모드 일 경우, SoundName이 존재하면, 다시 입력 받아라!
	if( m_isInsert && !(TheCartoonMaker->SoundIDCheck(strSoundName)) )
	{	
		AfxMessageBox(L"이미 같은 사운드 이름이 존재합니다. 다시 입력해 주세요!");
		return;
	}

	INT nStartFrm = _wtoi(strStartFrm.GetBuffer(0));
	if(nStartFrm<=0)
	{
		AfxMessageBox(L"시작 프레임은 1부터 시작합니다. 다시 입력 해주세요!");
		return;
	}

	OnOK();
}

void LmUISoundContentDlg::SetInputMode( BOOL isInsert )
{
	m_isInsert = isInsert;
}