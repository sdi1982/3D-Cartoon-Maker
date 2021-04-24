// LmUISoundContentDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUISoundContentDlg.h"


// LmUISoundContentDlg ��ȭ �����Դϴ�.

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


// LmUISoundContentDlg �޽��� ó�����Դϴ�.

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
	WCHAR szFilter[] = L"Wav Files(*.wav)|*.wav|��� ����(*.*)|*.*|";
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
		AfxMessageBox(L"���� ������ ��� �Է��� �ּ���!");
		return;
	}

	//Insert ��� �� ���, SoundName�� �����ϸ�, �ٽ� �Է� �޾ƶ�!
	if( m_isInsert && !(TheCartoonMaker->SoundIDCheck(strSoundName)) )
	{	
		AfxMessageBox(L"�̹� ���� ���� �̸��� �����մϴ�. �ٽ� �Է��� �ּ���!");
		return;
	}

	INT nStartFrm = _wtoi(strStartFrm.GetBuffer(0));
	if(nStartFrm<=0)
	{
		AfxMessageBox(L"���� �������� 1���� �����մϴ�. �ٽ� �Է� ���ּ���!");
		return;
	}

	OnOK();
}

void LmUISoundContentDlg::SetInputMode( BOOL isInsert )
{
	m_isInsert = isInsert;
}