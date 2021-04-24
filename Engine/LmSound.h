#ifndef _LMSOUND_H_
#define _LMSOUND_H_

#define FILE_NAME_LEN		512
#define SOUND_ID_LEN		128
#define MAXIMUM_ERR_MSG		255			// 에러 메시지 최대 크기

#include <Windows.h>
#include <mmsystem.h>
#include <dsound.h>

enum LmSOUND_STATE {SOUND_STOP, SOUND_PLAY, SOUND_PAUSE};

class LmSound
{
	struct THREAD_ARG{
		HMMIO				phmmio;
		LPDIRECTSOUNDBUFFER	pdsBuffer;
		DWORD				dsBufferSize;
		int					*pState;
		int					MyEventNum;
		DWORD				dwFadeIn;
		DWORD				dwFadeOut;
		DWORD				dwCurrentTime;
		DWORD				dwTotalTime;
		HANDLE				hEvent;
		HANDLE				hEventEndPos;
		DWORD				dwTotalByte;
		DWORD				dwCurrentPosByte;
		DWORD				PrePos, NowPos;
		DWORD				dwBufferCycle;
		int					ThreadState;
		WAVEFORMATEX		*pWaveFormatex;
		BOOL				IsSeek;
		DWORD				SeekPos;
	};
public:
	~LmSound();
	LmSound(LPCTSTR lpszFileName, HWND hwnd);
	
	BOOL		SetVolume(UINT Volume);
	UINT		GetVolume();
	BOOL		Play();
	BOOL		Seek(DWORD dwMSec);		// 중간부터 재생 하려면 구현 해야함.
	VOID		Pause();
	VOID		Stop();
	VOID		GoStart();				// 보류
	VOID		GoEnd();				// 보류
	VOID		FadeIn(DWORD dwTime);
	VOID		FadeOut(DWORD dwTime);
	DWORD		GetCurrentTime();
	DWORD		GetTotalTime();
	BOOL		IsPlay();
	BOOL		Init();
	LPCTSTR		GetFileName();

	LPCTSTR		GetLastError() const { return m_szErrMsg; }


private:
	TCHAR			m_szFileName[FILE_NAME_LEN];
	int				m_State;
	UINT			m_Volume;
	DWORD			m_dwFadeIn;
	DWORD			m_dwFadeOut;

	TCHAR			m_szErrMsg[MAXIMUM_ERR_MSG];
	HWND			m_hwnd;						// 어플리케이션 윈도우 핸들
	THREAD_ARG		m_THREAD_ARG;				// 스레드에 들어갈 인자
	int				m_dsBufferSize;
	DWORD			m_dwDataCkSize;
	
	int				GetEmptyEventNum();

	// 에러 메시지 설정
	VOID		SetErrorMsg(LPCTSTR lpszErrMsg);


	// WaveFile 관련
	WAVEFORMATEX		*m_pWaveFormatex;
	HMMIO				m_hmmio;					// Wave 파일 핸들	
	BOOL				WaveInit();
	DWORD				GetCurrentPosByte();
	DWORD				GetTotalByte();
	BOOL				CheckWfxFormat(WAVEFORMATEX *pWfx);

	// DSound 관련
	LPDIRECTSOUND		m_lpds;
	LPDIRECTSOUNDBUFFER	m_lpdsBuffer;
	BOOL				DSoundInit();
	BOOL				CreateDSBuffer();
	BOOL				ReleaseDSBuffer();
	HRESULT				SetNotification(HANDLE hEvent, DWORD dwOffset);

	static VOID			StreamThread(VOID *parg);
	static DWORD		FillBuffer(LPDIRECTSOUNDBUFFER lpdsBuffer, VOID *pArg, DWORD dwOffset, DWORD dwByte, BOOL IsSeek, DWORD ReadContinue);
	static VOID			NotifyThread(VOID *parg);
	

protected:	

};

#endif //_LMSOUND_H_