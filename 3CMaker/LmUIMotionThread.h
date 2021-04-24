#pragma once


// LmUIMotionThread
#define WM_MOTION_PLAY	 WM_USER+1100
#define WM_MOTION_STOP	 WM_USER+1101
#define WM_MOTION_MOVIE	 WM_USER+1200
#define WM_MOTION_MOVIESTOP WM_USER+1201

class LmUITimeLineView;
class LmUIMotionThread : public CWinThread
{
	DECLARE_DYNCREATE(LmUIMotionThread)
public:
	LmUIMotionThread(LmUITimeLineView* _pTimeLineView);           // TimeLine과 연결되는 생성자
	virtual ~LmUIMotionThread();

protected:
	LmUIMotionThread();           // 동적 만들기에 사용되는 protected 생성자입니다.
	
	

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

//멤버변수
public:
	LmUITimeLineView* m_pTimeLIneView;

//멤버함수
public:
	afx_msg void OnMotionPlay(WPARAM wParam, LPARAM lParam);
	
	// 동영상 출력 스레드. wParam은 LPCTSTR 형의 파일명.
	afx_msg void OnCreateMovie(WPARAM wParam, LPARAM lParam);
protected:
	DECLARE_MESSAGE_MAP()


};


