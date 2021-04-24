#include "stdafx.h"
#include "LmSoundManager.h"
#include "LmSound.h"

struct SAVE_DATA{
	TCHAR FileName[FILE_NAME_LEN];
	TCHAR SoundID[SOUND_ID_LEN];
};

typedef SAVE_DATA LOAD_DATA;

LmSoundManager::LmSoundManager()
{		
	ZeroMemory(m_szErrMsg, MAXIMUM_ERR_MSG);	
}

LmSoundManager::~LmSoundManager()
{
	Map::iterator		itr;

	// LmSound 메모리 해제
	for (itr = m_SoundTable.begin(); itr != m_SoundTable.end(); itr++){
		if (itr->second)
			delete itr->second;
	}
}


//------------------------------------------------------------------------------
// 사운드 등록
//------------------------------------------------------------------------------
BOOL LmSoundManager::Register(LPCTSTR lpszFileName, LPCTSTR lpszID)
{
	Map::const_iterator		itr;
	itr = m_SoundTable.find(lpszID);

	// ID 중복 검사
	if (itr != m_SoundTable.end()){
		SetErrorMsg(_T("Duplicated ID."));
		return FALSE;
	}

	// 사운드 생성 및 등록
	LmSound			*pLmSound;
	pLmSound = new LmSound(lpszFileName, m_hwnd);

	if (!pLmSound){
		SetErrorMsg(_T("Out of memory."));
		return FALSE;
	}

	m_SoundTable.insert(Map::value_type(lpszID, pLmSound));	
		
	return TRUE;
}


//------------------------------------------------------------------------------
// 사운드 제거
//------------------------------------------------------------------------------
BOOL LmSoundManager::Unregister(LPCTSTR lpszID)
{
	Map::iterator			itr;

	itr = m_SoundTable.find(lpszID);

	if (itr != m_SoundTable.end()){
		if (itr->second)
			delete itr->second;

		m_SoundTable.erase(lpszID);
		return TRUE;
	}

	SetErrorMsg(_T("ID is Not exist."));
	return FALSE;
}


//------------------------------------------------------------------------------
// 사운드 얻기
//------------------------------------------------------------------------------
LmSound *LmSoundManager::GetSound(LPCTSTR lpszID)
{
	Map::const_iterator		itr;

	// ID 찾기
	itr = m_SoundTable.find(lpszID);

	// 있는지 없는지 검사
	if (itr != m_SoundTable.end()){
		return itr->second;
	}

	SetErrorMsg(_T("ID is not exist."));
	return NULL;
}

//------------------------------------------------------------------------------
// 에러 메시지 설정
//------------------------------------------------------------------------------
VOID LmSoundManager::SetErrorMsg(LPCTSTR lpszErrMsg)
{
	_tcscpy_s(m_szErrMsg, lpszErrMsg);//, MAXIMUM_ERR_MSG);
}

LmSoundManager* LmSoundManager::GetInstance()
{
	static LmSoundManager g_SoundManager;

	return &g_SoundManager;
}

BOOL LmSoundManager::SetHwnd(HWND phwnd)
{
	if (!phwnd){
		SetErrorMsg(_T("hwnd is NULL"));
		return FALSE;
	}

	m_hwnd = phwnd;

	return TRUE;
}

BOOL LmSoundManager::CopySound(LPCTSTR lpszDstName, LPCTSTR lpszSrcName)
{
	if (!lpszDstName || !lpszSrcName){
		SetErrorMsg(_T("CopySound error"));
	}
	CString temp;
	Map::iterator		itr;

	// 순회하면서 LmSound 복사
	for (itr = m_SoundTable.begin(); itr != m_SoundTable.end(); itr++){
		if (itr->second){
			temp = itr->first.c_str();
			if (temp.Right(temp.GetLength() - temp.Find(_T("/")) - 1) == lpszSrcName){
				temp = temp.Left(temp.Find(_T("/")) + 1);
				temp += lpszDstName;

				// temp에는 새로운 사운드 ID
				Register(itr->second->GetFileName(), temp);
			}			
		}
	}

	return TRUE;
}


//------------------------------------------------------------------------------
// 사운드 정보를 파일로 저장하기 위해 메모리 블럭으로 만듬.
// *dwSize	: 할당된 메모리 블럭의 크기
// 리턴		: 할당된 메모리의 주소
//------------------------------------------------------------------------------
BYTE* LmSoundManager::SaveSound(DWORD *dwSize)
{
	// 파일 이름과, ID를 저장

	Map::iterator		itr;
	SAVE_DATA			*data;
	INT					i = 0;

	data = new SAVE_DATA[m_SoundTable.size()];
	*dwSize = 0;

	if (!data){
		SetErrorMsg(_T("Memory allocation error"));
		return NULL;
	}

	// 순회하면서 저장
	for (itr = m_SoundTable.begin(); itr != m_SoundTable.end(); itr++){
		if (itr->second){
			memset(&data[i], 0, sizeof(SAVE_DATA));
			_tcscpy_s(data[i].FileName, itr->second->GetFileName());
			_tcscpy_s(data[i].SoundID, itr->first.c_str());
				
			i++;
			}			
		}

	*dwSize = sizeof(SAVE_DATA) * i;
	
	return (BYTE*)data;
}

//------------------------------------------------------------------------------
// 특정 씬의 사운드를 모두 제거
// lpszSceneName	: 씬 이름
// 리턴				: 성공/실패
//------------------------------------------------------------------------------
BOOL LmSoundManager::DeleteSceneSound(LPCTSTR lpszSceneName)
{
	if (!lpszSceneName){
		SetErrorMsg(_T("DeleteSceneSound error"));
		return FALSE;
	}
	CString temp;
	Map::iterator		itr;

	// 순회하면서 LmSound 복사
	for (itr = m_SoundTable.begin(); itr != m_SoundTable.end(); itr++){
		if (itr->second){
			temp = itr->first.c_str();
			if (temp.Right(temp.GetLength() - temp.Find(_T("/")) - 1) == lpszSceneName){
				delete itr->second;
				m_SoundTable.erase(itr->first);
			}			
		}
	}

	return TRUE;
}


//------------------------------------------------------------------------------
// 사운드 정보가 저장된 메모리 블럭에서 실제 인스턴스로 생성(불러오기 기능에서 사용)
// pData	: 시작 메모리 주소
// dsSize	: 메모리 크기
//------------------------------------------------------------------------------
BOOL LmSoundManager::LoadSound(BYTE *pData, DWORD dwSize)
{
	if (!pData){
		SetErrorMsg(_T("LoadSound error"));
		return FALSE;
	}

	LOAD_DATA	*pLoadData = (LOAD_DATA*)pData;
	UINT		nSound = dwSize / sizeof(LOAD_DATA);

	LPCTSTR		lpszFileName, lpszSoundID;

	for (UINT i = 0; i < nSound; i++){
		lpszFileName = pLoadData[i].FileName;
		lpszSoundID = pLoadData[i].SoundID;

		Register(lpszFileName, lpszSoundID);
	}

	return FALSE;
}