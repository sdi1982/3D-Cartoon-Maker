#include "StdAfx.h"
#include "LmLayer.h"

LmLayer::LmLayer(void)
{
}

LmLayer::LmLayer( LPCWSTR _pcwsLayerName, LmKIND_OF_LAYER enumKindOfLayer )
{
	m_enumKindofLayer = enumKindOfLayer;
	m_wstrLayerName = _pcwsLayerName;
	m_mapSection.clear();
}
LmLayer::~LmLayer(void)
{
	m_mapSection.clear();
}



INT LmLayer::CheckSectionArea( UINT _uStartFrm, UINT _uEndFrm )
{
	/*
	* 리턴값  0 : 현재 레이어에 삽입을 할 수 있는 충분한 Section Area가 없음. 
	* 리턴값  1 : 현재 레이어에 삽입을 할 수 있는 충분한 Section Area가 있음.
	*/

	SectionMap::iterator itr = m_mapSection.begin();
	SectionMap::iterator itrEnd = m_mapSection.end();
	for(; itr != itrEnd ; itr++)
	{
		UINT uSecStart = itr->second.m_uStartFrm;
		UINT uSecEnd = itr->second.m_uEndFrm;

		//기존의 Section 시작점은 입력받는 끝점(nEndFrm)보다 크면 포함되지 않는다.
		if( !(  (uSecStart > _uEndFrm) || (uSecEnd < _uStartFrm)	)  )
			//만약 Section에 기존의 데이터를 포함 시킬 수 없다면 0으로 리턴
			return 0;
	}

	//현재 레이어에 삽입을 할 수 있는 충분한 Section Area가 있음.
	return 1;	
}

INT LmLayer::InsertSection( LPCWSTR _pcwsSectionName, UINT _uStartFrm, UINT _uEndFrm )
{
	//Section에 삽입할 수 없으면 -1 리턴
	if( !CheckSectionArea(_uStartFrm, _uEndFrm) )
		return -1;

	LmSection NewSection(_pcwsSectionName, _uStartFrm, _uEndFrm);

	m_mapSection[_uStartFrm] = NewSection;
	
	return 0;
}

INT LmLayer::InsertSection( LPCWSTR _pcwsSectionName, UINT _uStartFrm, UINT _uEndFrm , DWORD _dwFadeIn, DWORD _dwFadeOut)
{
	//Section에 삽입할 수 없으면 -1 리턴
	if( !CheckSectionArea(_uStartFrm, _uEndFrm) )
		return -1;

	LmSection NewSection(_pcwsSectionName, _uStartFrm, _uEndFrm, _dwFadeIn, _dwFadeOut);

	m_mapSection[_uStartFrm] = NewSection;

	return 0;
}


INT LmLayer::InsertSection( LmSection &_Section ) /*Section 삽입 */
{
	//Section에 삽입할 수 없으면 -1 리턴
	if( !CheckSectionArea(_Section.m_uStartFrm , _Section.m_uEndFrm) )
		return -1;

	m_mapSection[_Section.m_uStartFrm] = _Section;

	return 0;
}
INT LmLayer::RemoveSection( UINT _uStartFrm )
{
	SectionMapItr itrFinder =  m_mapSection.find(_uStartFrm);
	
	//찾는 값이 없으면 -1 리턴
	if(itrFinder == m_mapSection.end())
		return -1;

	m_mapSection.erase(itrFinder);
	
	//이상없이 삭제 되었으면 0 리턴
	return 0;
}

INT LmLayer::RemoveSection( LPCWSTR _pcwsSectionName )
{
	SectionMapItr itrFinder =  FindForSecName(_pcwsSectionName);

	//찾는 값이 없으면 -1 리턴
	if(itrFinder == m_mapSection.end())
		return -1;

	m_mapSection.erase(itrFinder);

	//이상없이 삭제 되었으면 0 리턴
	return 0;
}

INT LmLayer::ModifySection( UINT _uStartFrm, LPCWSTR _pcwsSectionName,UINT _uNewStartFrm, UINT _uNewEndFrm )
{
	//새로 수정된 Section에 다른 Section이 있다면 -2 리턴
	if( !CheckSectionArea(_uNewStartFrm, _uNewEndFrm) )
		return -2;

	//삭제할 때 이상이 있으면 -1로 리턴한다. 
	if( RemoveSection( _uStartFrm )<0) 
		return -1;

	//Section을 삽입한다. 만약 여기에서 문제가 있다면, CheckSectonArea함수에 문제가 있는 것임.
	InsertSection(_pcwsSectionName, _uNewStartFrm, _uNewEndFrm);

	//이상없이 삭제 되었으면 0 리턴
	return 0;
}

INT LmLayer::ModifySection( LPCWSTR _pcwsSectionName, LPCWSTR _pcwsNewSectionName,UINT _uNewStartFrm, UINT _uNewEndFrm )
{
	//새로 수정된 Section에 다른 Section이 있다면 -2 리턴
	if( !CheckSectionArea(_uNewStartFrm, _uNewEndFrm) )
		return -2;

	//삭제할 때 이상이 있으면 -1로 리턴한다. 
	if( RemoveSection( _pcwsSectionName )<0) 
		return -1;

	//Section을 삽입한다. 만약 여기에서 문제가 있다면, CheckSectonArea함수에 문제가 있는 것임.
	InsertSection(_pcwsNewSectionName, _uNewStartFrm, _uNewEndFrm);

	//이상없이 삭제 되었으면 0 리턴
	return 0;
}

INT LmLayer::ModifySection(LPCWSTR _pcwsSectionName, LPCWSTR _pcwsNewSectionName,  UINT _uNewStartFrm, UINT _uNewEndFrm, DWORD _dwNewFadeIn, DWORD _dwNewFadeOut)	//Section 수정 사운드에서 사용
{
	//새로 수정된 Section에 다른 Section이 있다면 -2 리턴
	if( !CheckSectionArea(_uNewStartFrm, _uNewEndFrm) )
		return -2;

	//삭제할 때 이상이 있으면 -1로 리턴한다. 
	if( RemoveSection( _pcwsSectionName )<0) 
		return -1;

	//Section을 삽입한다. 만약 여기에서 문제가 있다면, CheckSectonArea함수에 문제가 있는 것임.
	InsertSection(_pcwsNewSectionName, _uNewStartFrm, _uNewEndFrm, _dwNewFadeIn , _dwNewFadeOut);

	//이상없이 삭제 되었으면 0 리턴
	return 0;

}

//단, 이 멤버 함수는 Section이 같은 이름을 가질 수 없다는 전제 하에 사용할 수 있음
//같은 이름이 있을 경우 먼저 삽입된 Section의 위치를 반환한다.
SectionMapItr LmLayer::FindForSecName( LPCWSTR _pcwsSectionName )
{
	SectionMapItr itrSecFinder = m_mapSection.begin();
	SectionMapItr itrSecEnd = m_mapSection.end();

	UINT uException = 0;

	for(; itrSecFinder != itrSecEnd ; itrSecFinder++)
	{
		wstring wstrMapSecName = itrSecFinder->second.m_wstrSectionName;
		wstring wstrInputSecName = _pcwsSectionName;

		//찾았으면 리턴
		if( wstrMapSecName == wstrInputSecName )
			return itrSecFinder;
	}

	//못찾았으면 itrSecEnd를 리턴
	return itrSecFinder;
}