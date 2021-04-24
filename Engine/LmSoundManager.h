#ifndef _LMSOUNDMANAGER_H_
#define _LMSOUNDMANAGER_H_

#define MAXIMUM_ERR_MSG		255			// 에러 메시지 최대 크기

#include <map>

class LmSound;
class LmSoundManager
{
public:	
	virtual		~LmSoundManager();
	
	BOOL		Register(LPCTSTR lpszFileName, LPCTSTR lpszID);
	BOOL		Unregister(LPCTSTR lpszID);
	LmSound		*GetSound(LPCTSTR lpszID);
	BOOL		SetHwnd(HWND phwnd);
	BOOL		CopySound(LPCTSTR lpszDstName, LPCTSTR lpszSrcName);
	BYTE		*SaveSound(DWORD *dwSize);
	BOOL		LoadSound(BYTE *pData, DWORD dwSize);
	BOOL		DeleteSceneSound(LPCTSTR lpszSceneName);



	
	// 마지막 에러 메시지 리턴
	LPCTSTR		GetLastError() const { return m_szErrMsg; }

	static		LmSoundManager* GetInstance();


private:
	LmSoundManager();
	HWND		m_hwnd;								// 어플리케이션 윈도우 핸들
	// 에러 메시지 설정
	VOID		SetErrorMsg(LPCTSTR lpszErrMsg);

	typedef std::map<wstring, LmSound*> Map;		// 나중에 string로 변경?
	Map			m_SoundTable;
	TCHAR		m_szErrMsg[MAXIMUM_ERR_MSG];

};

#define TheSoundManager LmSoundManager::GetInstance()

#endif //_LMSOUNDMANAGER_H_