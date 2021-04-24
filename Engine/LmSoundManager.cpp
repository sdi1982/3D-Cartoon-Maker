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

	// LmSound �޸� ����
	for (itr = m_SoundTable.begin(); itr != m_SoundTable.end(); itr++){
		if (itr->second)
			delete itr->second;
	}
}


//------------------------------------------------------------------------------
// ���� ���
//------------------------------------------------------------------------------
BOOL LmSoundManager::Register(LPCTSTR lpszFileName, LPCTSTR lpszID)
{
	Map::const_iterator		itr;
	itr = m_SoundTable.find(lpszID);

	// ID �ߺ� �˻�
	if (itr != m_SoundTable.end()){
		SetErrorMsg(_T("Duplicated ID."));
		return FALSE;
	}

	// ���� ���� �� ���
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
// ���� ����
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
// ���� ���
//------------------------------------------------------------------------------
LmSound *LmSoundManager::GetSound(LPCTSTR lpszID)
{
	Map::const_iterator		itr;

	// ID ã��
	itr = m_SoundTable.find(lpszID);

	// �ִ��� ������ �˻�
	if (itr != m_SoundTable.end()){
		return itr->second;
	}

	SetErrorMsg(_T("ID is not exist."));
	return NULL;
}

//------------------------------------------------------------------------------
// ���� �޽��� ����
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

	// ��ȸ�ϸ鼭 LmSound ����
	for (itr = m_SoundTable.begin(); itr != m_SoundTable.end(); itr++){
		if (itr->second){
			temp = itr->first.c_str();
			if (temp.Right(temp.GetLength() - temp.Find(_T("/")) - 1) == lpszSrcName){
				temp = temp.Left(temp.Find(_T("/")) + 1);
				temp += lpszDstName;

				// temp���� ���ο� ���� ID
				Register(itr->second->GetFileName(), temp);
			}			
		}
	}

	return TRUE;
}


//------------------------------------------------------------------------------
// ���� ������ ���Ϸ� �����ϱ� ���� �޸� ������ ����.
// *dwSize	: �Ҵ�� �޸� ���� ũ��
// ����		: �Ҵ�� �޸��� �ּ�
//------------------------------------------------------------------------------
BYTE* LmSoundManager::SaveSound(DWORD *dwSize)
{
	// ���� �̸���, ID�� ����

	Map::iterator		itr;
	SAVE_DATA			*data;
	INT					i = 0;

	data = new SAVE_DATA[m_SoundTable.size()];
	*dwSize = 0;

	if (!data){
		SetErrorMsg(_T("Memory allocation error"));
		return NULL;
	}

	// ��ȸ�ϸ鼭 ����
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
// Ư�� ���� ���带 ��� ����
// lpszSceneName	: �� �̸�
// ����				: ����/����
//------------------------------------------------------------------------------
BOOL LmSoundManager::DeleteSceneSound(LPCTSTR lpszSceneName)
{
	if (!lpszSceneName){
		SetErrorMsg(_T("DeleteSceneSound error"));
		return FALSE;
	}
	CString temp;
	Map::iterator		itr;

	// ��ȸ�ϸ鼭 LmSound ����
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
// ���� ������ ����� �޸� ������ ���� �ν��Ͻ��� ����(�ҷ����� ��ɿ��� ���)
// pData	: ���� �޸� �ּ�
// dsSize	: �޸� ũ��
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