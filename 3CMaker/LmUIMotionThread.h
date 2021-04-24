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
	LmUIMotionThread(LmUITimeLineView* _pTimeLineView);           // TimeLine�� ����Ǵ� ������
	virtual ~LmUIMotionThread();

protected:
	LmUIMotionThread();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	
	

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

//�������
public:
	LmUITimeLineView* m_pTimeLIneView;

//����Լ�
public:
	afx_msg void OnMotionPlay(WPARAM wParam, LPARAM lParam);
	
	// ������ ��� ������. wParam�� LPCTSTR ���� ���ϸ�.
	afx_msg void OnCreateMovie(WPARAM wParam, LPARAM lParam);
protected:
	DECLARE_MESSAGE_MAP()


};


