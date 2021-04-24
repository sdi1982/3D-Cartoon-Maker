//
//
//  @ Project : 3D 카툰 메이커
//  @ File Name : LmLayer.h
//  @ Date : 2007-12-13
//  @ Author : 안진왕
//
//

#if !defined(_LMLAYER_H)
#define _LMLAYER_H

#include "LmSection.h"
#include<map>

enum LmKIND_OF_LAYER {CHARACTER_LAYER=1, SOUND_LAYER, TALKBOX_LAYER, FIREWORKS_LAYER, SNOW_LAYER, CAMERA_LAYER };

//같은 이름의 섹션은 존재 할 수도 있기 때문. 캐릭터 액션(걷기)이 대표적 예임.
//그래서 <INT,LmSection>임.
typedef map<INT,LmSection>			SectionMap;		
typedef SectionMap::iterator		SectionMapItr;

class LmLayer
{
//멤버 함수
public:
	LmLayer(void);
	LmLayer(LPCWSTR _pcwsLayerName, LmKIND_OF_LAYER enumKindOfLayer);
	~LmLayer(void);

	LmKIND_OF_LAYER GetKindOfLayer() const { return m_enumKindofLayer; }	//레이어의 종류를 받아온다. 
	wstring			GetLayerName() const { return m_wstrLayerName; }		//레이어의 이름을 받아온다.
	SectionMap		GetSection() const { return m_mapSection; }				//Section의 정보를 받아온다.

	INT			InsertSection(LPCWSTR _pcwsSectionName, UINT _uStartFrm, UINT _uEndFrm);		//Section 삽입
	INT			InsertSection(LPCWSTR _pcwsSectionName, UINT _uStartFrm, UINT _uEndFrm,
								DWORD _dwFadeIn, DWORD _dwFadeOut);								//Section 삽입
	INT			InsertSection(LmSection &_Section);												//Section 삽입
	
	INT			RemoveSection(UINT _uStartFrm);													//Section 삭제
	INT			RemoveSection(LPCWSTR _pcwsSectionName);										//이름으로 Section 삭제, 같은 이름이 없는 Layer에서만 적용되어야 함.
	
	INT			ModifySection(UINT _uStartFrm, LPCWSTR _pcwsSectionName,
							UINT _uNewStartFrm, UINT _uNewEndFrm);							//이름으로 Section 수정, 같은 이름이 없는 Layer에서만 적용되어야 함.
	INT			ModifySection(LPCWSTR _pcwsSectionName, LPCWSTR _pcwsNewSectionName,
							UINT _uNewStartFrm, UINT _uNewEndFrm);							//Section 수정

	INT			ModifySection(LPCWSTR _pcwsSectionName, LPCWSTR _pcwsNewSectionName,
								UINT _uNewStartFrm, UINT _uNewEndFrm, DWORD _dwNewFadeIn, DWORD _dwNewFadeOut);	//Section 수정 사운드에서 사용


	INT			CheckSectionArea(UINT _uStartFrm, UINT _uEndFrm);							//현재 레이어에서 Section 삽입이 가능한지 확인한다.

	SectionMapItr FindForSecName(LPCWSTR _pcwsSectionName);

//멤버 변수
public:
	LmKIND_OF_LAYER			m_enumKindofLayer;			//레이어의 종류
	SectionMap				m_mapSection;				//INT 프레임값
	wstring					m_wstrLayerName;			//레이어 이름

};

#endif  //_LMLAYER_H