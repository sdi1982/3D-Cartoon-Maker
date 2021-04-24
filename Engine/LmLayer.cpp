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
	* ���ϰ�  0 : ���� ���̾ ������ �� �� �ִ� ����� Section Area�� ����. 
	* ���ϰ�  1 : ���� ���̾ ������ �� �� �ִ� ����� Section Area�� ����.
	*/

	SectionMap::iterator itr = m_mapSection.begin();
	SectionMap::iterator itrEnd = m_mapSection.end();
	for(; itr != itrEnd ; itr++)
	{
		UINT uSecStart = itr->second.m_uStartFrm;
		UINT uSecEnd = itr->second.m_uEndFrm;

		//������ Section �������� �Է¹޴� ����(nEndFrm)���� ũ�� ���Ե��� �ʴ´�.
		if( !(  (uSecStart > _uEndFrm) || (uSecEnd < _uStartFrm)	)  )
			//���� Section�� ������ �����͸� ���� ��ų �� ���ٸ� 0���� ����
			return 0;
	}

	//���� ���̾ ������ �� �� �ִ� ����� Section Area�� ����.
	return 1;	
}

INT LmLayer::InsertSection( LPCWSTR _pcwsSectionName, UINT _uStartFrm, UINT _uEndFrm )
{
	//Section�� ������ �� ������ -1 ����
	if( !CheckSectionArea(_uStartFrm, _uEndFrm) )
		return -1;

	LmSection NewSection(_pcwsSectionName, _uStartFrm, _uEndFrm);

	m_mapSection[_uStartFrm] = NewSection;
	
	return 0;
}

INT LmLayer::InsertSection( LPCWSTR _pcwsSectionName, UINT _uStartFrm, UINT _uEndFrm , DWORD _dwFadeIn, DWORD _dwFadeOut)
{
	//Section�� ������ �� ������ -1 ����
	if( !CheckSectionArea(_uStartFrm, _uEndFrm) )
		return -1;

	LmSection NewSection(_pcwsSectionName, _uStartFrm, _uEndFrm, _dwFadeIn, _dwFadeOut);

	m_mapSection[_uStartFrm] = NewSection;

	return 0;
}


INT LmLayer::InsertSection( LmSection &_Section ) /*Section ���� */
{
	//Section�� ������ �� ������ -1 ����
	if( !CheckSectionArea(_Section.m_uStartFrm , _Section.m_uEndFrm) )
		return -1;

	m_mapSection[_Section.m_uStartFrm] = _Section;

	return 0;
}
INT LmLayer::RemoveSection( UINT _uStartFrm )
{
	SectionMapItr itrFinder =  m_mapSection.find(_uStartFrm);
	
	//ã�� ���� ������ -1 ����
	if(itrFinder == m_mapSection.end())
		return -1;

	m_mapSection.erase(itrFinder);
	
	//�̻���� ���� �Ǿ����� 0 ����
	return 0;
}

INT LmLayer::RemoveSection( LPCWSTR _pcwsSectionName )
{
	SectionMapItr itrFinder =  FindForSecName(_pcwsSectionName);

	//ã�� ���� ������ -1 ����
	if(itrFinder == m_mapSection.end())
		return -1;

	m_mapSection.erase(itrFinder);

	//�̻���� ���� �Ǿ����� 0 ����
	return 0;
}

INT LmLayer::ModifySection( UINT _uStartFrm, LPCWSTR _pcwsSectionName,UINT _uNewStartFrm, UINT _uNewEndFrm )
{
	//���� ������ Section�� �ٸ� Section�� �ִٸ� -2 ����
	if( !CheckSectionArea(_uNewStartFrm, _uNewEndFrm) )
		return -2;

	//������ �� �̻��� ������ -1�� �����Ѵ�. 
	if( RemoveSection( _uStartFrm )<0) 
		return -1;

	//Section�� �����Ѵ�. ���� ���⿡�� ������ �ִٸ�, CheckSectonArea�Լ��� ������ �ִ� ����.
	InsertSection(_pcwsSectionName, _uNewStartFrm, _uNewEndFrm);

	//�̻���� ���� �Ǿ����� 0 ����
	return 0;
}

INT LmLayer::ModifySection( LPCWSTR _pcwsSectionName, LPCWSTR _pcwsNewSectionName,UINT _uNewStartFrm, UINT _uNewEndFrm )
{
	//���� ������ Section�� �ٸ� Section�� �ִٸ� -2 ����
	if( !CheckSectionArea(_uNewStartFrm, _uNewEndFrm) )
		return -2;

	//������ �� �̻��� ������ -1�� �����Ѵ�. 
	if( RemoveSection( _pcwsSectionName )<0) 
		return -1;

	//Section�� �����Ѵ�. ���� ���⿡�� ������ �ִٸ�, CheckSectonArea�Լ��� ������ �ִ� ����.
	InsertSection(_pcwsNewSectionName, _uNewStartFrm, _uNewEndFrm);

	//�̻���� ���� �Ǿ����� 0 ����
	return 0;
}

INT LmLayer::ModifySection(LPCWSTR _pcwsSectionName, LPCWSTR _pcwsNewSectionName,  UINT _uNewStartFrm, UINT _uNewEndFrm, DWORD _dwNewFadeIn, DWORD _dwNewFadeOut)	//Section ���� ���忡�� ���
{
	//���� ������ Section�� �ٸ� Section�� �ִٸ� -2 ����
	if( !CheckSectionArea(_uNewStartFrm, _uNewEndFrm) )
		return -2;

	//������ �� �̻��� ������ -1�� �����Ѵ�. 
	if( RemoveSection( _pcwsSectionName )<0) 
		return -1;

	//Section�� �����Ѵ�. ���� ���⿡�� ������ �ִٸ�, CheckSectonArea�Լ��� ������ �ִ� ����.
	InsertSection(_pcwsNewSectionName, _uNewStartFrm, _uNewEndFrm, _dwNewFadeIn , _dwNewFadeOut);

	//�̻���� ���� �Ǿ����� 0 ����
	return 0;

}

//��, �� ��� �Լ��� Section�� ���� �̸��� ���� �� ���ٴ� ���� �Ͽ� ����� �� ����
//���� �̸��� ���� ��� ���� ���Ե� Section�� ��ġ�� ��ȯ�Ѵ�.
SectionMapItr LmLayer::FindForSecName( LPCWSTR _pcwsSectionName )
{
	SectionMapItr itrSecFinder = m_mapSection.begin();
	SectionMapItr itrSecEnd = m_mapSection.end();

	UINT uException = 0;

	for(; itrSecFinder != itrSecEnd ; itrSecFinder++)
	{
		wstring wstrMapSecName = itrSecFinder->second.m_wstrSectionName;
		wstring wstrInputSecName = _pcwsSectionName;

		//ã������ ����
		if( wstrMapSecName == wstrInputSecName )
			return itrSecFinder;
	}

	//��ã������ itrSecEnd�� ����
	return itrSecFinder;
}