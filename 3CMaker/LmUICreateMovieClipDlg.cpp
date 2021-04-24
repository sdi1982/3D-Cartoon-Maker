// LmUICreateMovieClipDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "3CMaker.h"
#include "LmUICreateMovieClipDlg.h"


// LmUICreateMovieClipDlg 대화 상자입니다.

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


// LmUICreateMovieClipDlg 메시지 처리기입니다.

void LmUICreateMovieClipDlg::OnBnClickedButton1()
{
	// 파일 다이얼로그 띄우기
	WCHAR szFilter[] = L"AVI Files(*.avi)";
	CFileDialog FileDlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, szFilter);	

	if (FileDlg.DoModal() != IDOK)
		return;

	m_FileName = FileDlg.GetPathName();

	// 확장자 검사. 없으면 붙여줌. 파일명이 ".avi" ".AVI"가 입력 될때도 처리해야함.
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
	// [주의] 스레드에서 szFileName 사용전에 릴리즈 버퍼 해버리면 안될듯. 슬립이나 이벤트 처리해야함.
	m_FileName.ReleaseBuffer();
}

void LmUICreateMovieClipDlg::SetState(DWORD cFrame)
{
	m_progCreateMovieClip.SetPos(cFrame);
}

void LmUICreateMovieClipDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (!m_pMotionThread)
		return;	

	m_pMotionThread->PostThreadMessage(WM_MOTION_MOVIESTOP, 0, 0);
	
	OnCancel();
}
