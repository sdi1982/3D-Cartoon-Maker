//
//
//  @ Project : 3D 카툰 메이커
//  @ File Name : LmModelManager.cpp
//  @ Date : 2007-10-11
//  @ Author : 신동인
//
//

#include "dxstdafx.h"
#include "LmModelManager.h"


LmModelManager::LmModelManager()
{
}

LmModelManager::~LmModelManager()
{
	// 등록된 객체를 모두 해제 한다.
	map<wstring,LmBackGround*>::iterator itrBG;
	for(itrBG = m_mapBG.begin(); itrBG != m_mapBG.end(); itrBG++)
	{
		if(m_mapBG[(*itrBG).first] != NULL)
			delete m_mapBG[(*itrBG).first];
	}
	m_mapBG.clear();

	map<wstring,LmThing*>::iterator itrThing;
	for(itrThing = m_mapThing.begin(); itrThing != m_mapThing.end(); itrThing++)
	{
		if(m_mapThing[(*itrThing).first] != NULL)
			delete m_mapThing[(*itrThing).first];
	}
	m_mapThing.clear();

	map<wstring,LmChar*>::iterator itrChar;
	for(itrChar = m_mapChar.begin(); itrChar != m_mapChar.end(); itrChar++)
	{
		if(m_mapChar[(*itrChar).first] != NULL)
			delete m_mapChar[(*itrChar).first];
	}
	m_mapChar.clear();
}

LmModelManager* LmModelManager::GetInstance()
{
	static LmModelManager ModelManager;

	return &ModelManager;
}

BOOL LmModelManager::RegisterBG(LPCWSTR _pcwsBG)
{
	if(CheckSameBG(_pcwsBG))
		return FALSE;

	WCHAR strXFile[MAX_PATH];
	StringCchCopy(strXFile,MAX_PATH,_pcwsBG);

	LmBackGround* NewBG = new LmBackGround(_pcwsBG, strXFile);

	if(NewBG == NULL)
		return FALSE;
	
	m_mapBG[_pcwsBG] = NewBG;

	return TRUE;
}

BOOL LmModelManager::RegisterThing(LPCWSTR _pcwsThing )
{
	if(CheckSameThing(_pcwsThing))
		return FALSE;

	WCHAR strXFile[MAX_PATH];
	StringCchCopy(strXFile,MAX_PATH,_pcwsThing);

	LmThing* NewThing = new LmThing(_pcwsThing, strXFile);

	if(NewThing == NULL)
		return FALSE;

	m_mapThing[_pcwsThing] = NewThing;

	return TRUE;
}

BOOL LmModelManager::RegisterChar(LPCWSTR _pcwsChar )
{
	if(CheckSameChar(_pcwsChar))
		return FALSE;

	WCHAR strXFile[MAX_PATH];
	StringCchCopy(strXFile,MAX_PATH,_pcwsChar);

	LmChar* NewChar = new LmChar(_pcwsChar, strXFile);

	if(NewChar == NULL)
		return FALSE;

	m_mapChar[_pcwsChar] = NewChar;

	return TRUE;
}

BOOL LmModelManager::UnregisterBG(LPCWSTR _pcwsBG)
{
	// 같은 이름이 존재하지 않는다면 FALSE를 리턴
	if(!CheckSameBG(_pcwsBG))
		return FALSE;

	// 객체가 비어있으면 FALSE를 리턴
	if(m_mapBG[_pcwsBG] == NULL)
		return FALSE;
	
	delete m_mapBG[_pcwsBG];
	m_mapBG.erase(_pcwsBG);

	return TRUE;
}

BOOL LmModelManager::UnregisterThing(LPCWSTR _pcwsThing)
{
	if(!CheckSameThing(_pcwsThing))
		return FALSE;

	// 객체가 비어있으면 FALSE를 리턴
	if(m_mapThing[_pcwsThing] == NULL)
		return FALSE;

	delete m_mapThing[_pcwsThing];
	m_mapThing.erase(_pcwsThing);

	return TRUE;
}

BOOL LmModelManager::UnregisterChar(LPCWSTR _pcwsChar)
{
	if(!CheckSameChar(_pcwsChar))
		return FALSE;

	// 객체가 비어있으면 FALSE를 리턴
	if(m_mapChar[_pcwsChar] == NULL)
		return FALSE;

	delete m_mapChar[_pcwsChar];
	m_mapChar.erase(_pcwsChar);

	return TRUE;
}

BOOL LmModelManager::CheckSameBG(LPCWSTR _pcwsBG )
{
	// 같은 배경의 이름이 존재하지 않는다면 FALSE를 리턴
	map<wstring,LmBackGround*>::iterator result;
	result  = m_mapBG.find(_pcwsBG);

	if(result == m_mapBG.end())
		return FALSE;

	return TRUE;
}

BOOL LmModelManager::CheckSameThing(LPCWSTR _pcwsThing )
{
	// 같은 물건의 이름이 존재하지 않는다면 FALSE를 리턴
	map<wstring,LmThing*>::iterator result;
	result  = m_mapThing.find(_pcwsThing);

	if(result == m_mapThing.end())
		return FALSE;

	return TRUE;
}

BOOL LmModelManager::CheckSameChar(LPCWSTR _pcwsChar )
{
	// 같은 캐릭터의 이름이 존재하지 않는다면 FALSE를 리턴
	map<wstring,LmChar*>::iterator result;
	result  = m_mapChar.find(_pcwsChar);

	if(result == m_mapChar.end())
		return FALSE;

	return TRUE;
}

std::wstring LmModelManager::GetLIstBGFirst()
{
	wstring str;

	m_itrBG = m_mapBG.begin(); 
	if(m_itrBG == m_mapBG.end())
		return L"";

	str = (*(m_itrBG)).first;
	m_itrBG++;

	return str;
}

std::wstring LmModelManager::GetLIstBGNext()
{
	wstring str;
	if( m_itrBG == m_mapBG.end())
		return L"";

	str = (*m_itrBG).first;
	m_itrBG++;

	return str;
}

std::wstring LmModelManager::GetLIstThingFirst()
{
	wstring str;

	m_itrThing = m_mapThing.begin(); 
	if(m_itrThing == m_mapThing.end())
		return L"";

	str = (*(m_itrThing)).first;
	m_itrThing++;

	return str;
}

std::wstring LmModelManager::GetLIstThingNext()
{
	wstring str;
	if( m_itrThing == m_mapThing.end())
		return L"";

	str = (*m_itrThing).first;
	m_itrThing++;

	return str;
}

std::wstring LmModelManager::GetLIstCharFirst()
{
	wstring str;

	m_itrChar = m_mapChar.begin(); 
	if(m_itrChar == m_mapChar.end())
		return L"";

	str = (*(m_itrChar)).first;
	m_itrChar++;

	return str;
}

std::wstring LmModelManager::GetLIstCharNext()
{
	wstring str;
	if( m_itrChar == m_mapChar.end())
		return L"";

	str = (*m_itrChar).first;
	m_itrChar++;

	return str;
}

BOOL LmModelManager::Setup(UINT _uScreenWidth, UINT _uScreenHeight)
{
	map<wstring,LmChar*>::iterator itrChar;
	for(itrChar = m_mapChar.begin(); itrChar != m_mapChar.end(); itrChar++)
	{
		if(m_mapChar[(*itrChar).first] != NULL)
			m_mapChar[(*itrChar).first]->Setup(_uScreenWidth, _uScreenHeight);
	}

	return TRUE;
}

VOID LmModelManager::Cleanup()
{
	map<wstring,LmChar*>::iterator itrChar;
	for(itrChar = m_mapChar.begin(); itrChar != m_mapChar.end(); itrChar++)
	{
		if(m_mapChar[(*itrChar).first] != NULL)
			m_mapChar[(*itrChar).first]->Cleanup();
	}
}