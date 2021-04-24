//
//
//  @ Project : 3D 카툰 메이커
//  @ File Name : LmModelManager.h
//  @ Date : 2007-10-11
//  @ Author : 신동인
//
//


#if !defined(_LMMODELMANAGER_H)
#define _LMMODELMANAGER_H

#include "LmModel.h"
#include "LmThing.h"
#include "LmChar.h"

using namespace std;

class LmModelManager{

public:
	virtual ~LmModelManager();
	static LmModelManager* GetInstance();

	/* 리소스 로드 - */
	BOOL RegisterBG(LPCWSTR _pcwsBG);
	BOOL RegisterThing(LPCWSTR _pcwsThing);
	BOOL RegisterChar(LPCWSTR _pcwsChar);

	BOOL UnregisterBG(LPCWSTR _pcwsBG);
	BOOL UnregisterThing(LPCWSTR _pcwsThing);
	BOOL UnregisterChar(LPCWSTR _pcwsChar);

	/* 객체 엑세스 */
	LmBackGround* GetBG(LPCWSTR _pcwsBG){ return m_mapBG[_pcwsBG]; }
	LmThing* GetThing(LPCWSTR _pcwsThing){ return m_mapThing[_pcwsThing]; }
	LmChar* GetChar(LPCWSTR _pcwsChar){ return m_mapChar[_pcwsChar]; }

	wstring GetLIstBGFirst();
	wstring GetLIstBGNext();
	wstring GetLIstThingFirst();
	wstring GetLIstThingNext();
	wstring GetLIstCharFirst();
	wstring GetLIstCharNext();


	BOOL Setup(UINT _uScreenWidth, UINT _uScreenHeight);
	VOID Cleanup();
private:
	LmModelManager();

	BOOL CheckSameBG(LPCWSTR _pcwsBG);
	BOOL CheckSameThing(LPCWSTR _pcwsThing);
	BOOL CheckSameChar(LPCWSTR _pcwsChar);

	map<wstring,LmBackGround*> m_mapBG;
	map<wstring,LmThing*> m_mapThing;
	map<wstring,LmChar*> m_mapChar;

	map<wstring,LmBackGround*>::iterator m_itrBG;
	map<wstring,LmThing*>::iterator m_itrThing;
	map<wstring,LmChar*>::iterator m_itrChar;
};

#define TheModelManager LmModelManager::GetInstance()
#endif  //_LMMODELMANAGER_H
