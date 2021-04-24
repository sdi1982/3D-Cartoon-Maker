// LmUICreateMovieClipDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUICreateMovieClipDlg.h"


// LmUICreateMovieClipDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(LmUICreateMovieClipDlg, CDialog)

LmUICreateMovieClipDlg::LmUICreateMovieClipDlg(CWnd* pParent /*=NULL*/)
	: CDialog(LmUICreateMovieClipDlg::IDD, pParent)
	, m_strTime(_T(""))
	, m_strCurrState(_T(""))
{
	m_pMotionThread = TheCartoonMaker->GetMotionThread();

}

LmUICreateMovieClipDlg::~LmUICreateMovieClipDlg()
{
}

void LmUICreateMovieClipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TIME, m_strTime);
	DDX_Text(pDX, IDC_CURRENT_STATE, m_strCurrState);
	DDX_Control(pDX, IDC_PROGRESS1, m_progCreateMovieClip);
}


BEGIN_MESSAGE_MAP(LmUICreateMovieClipDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &LmUICreateMovieClipDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDCANCEL, &LmUICreateMovieClipDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// LmUICreateMovieClipDlg �޽��� ó�����Դϴ�.

void LmUICreateMovieClipDlg::OnBnClickedButton1()
{
	// ���� ���̾�α� ����
	WCHAR szFilter[] = L"AVI Files(*.avi)";
	CFileDialog FileDlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, szFilter);	

	if (FileDlg.DoModal() != IDOK)
		return;

	m_FileName = FileDlg.GetPathName();

	// Ȯ���� �˻�. ������ �ٿ���. ���ϸ��� ".avi" ".AVI"�� �Է� �ɶ��� ó���ؾ���.
	if (m_FileName.Mid(m_FileName.GetLength() - 4, 4) != _T(".avi") &&
		m_FileName.Mid(m_FileName.GetLength() - 4, 4) != _T(".AVI")){
			m_FileName += _T(".avi");
	}
	
	if (!m_pMotionThread)
		return;

	m_progCreateMovieClip.SetRange32(0, TheCartoonMaker->GetTotalFrame());

	LPCTSTR szFileName;
	szFileName = m_FileName.GetBuffer();

	m_pMotionThread->PostThreadMessage(WM_MOTION_MOVIE, (WPARAM)szFileName, (LPARAM)this);
	// [����] �����忡�� szFileName ������� ������ ���� �ع����� �ȵɵ�. �����̳� �̺�Ʈ ó���ؾ���.
	m_FileName.ReleaseBuffer();
}

void LmUICreateMovieClipDlg::SetState(DWORD cFrame)
{
	m_progCreateMovieClip.SetPos(cFrame);
}

void LmUICreateMovieClipDlg::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (!m_pMotionThread)
		return;	

	m_pMotionThread->PostThreadMessage(WM_MOTION_MOVIESTOP, 0, 0);
	
	OnCancel();
}
