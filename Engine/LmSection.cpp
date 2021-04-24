#include "StdAfx.h"
#include "LmSection.h"

LmSection::LmSection(void)
{
}

LmSection::LmSection( LPCWSTR _pcwsName, UINT _uStartFrm, UINT _uEndFrm )
{
	m_wstrSectionName = _pcwsName;
	m_uStartFrm = _uStartFrm;
	m_uEndFrm = _uEndFrm;
	m_dwFadeIn = 0;
	m_dwFadeOut = 0;
}

LmSection::LmSection( const LmSection & _sourceSection )
{
	m_wstrSectionName = _sourceSection.m_wstrSectionName;
	m_uStartFrm = _sourceSection.m_uStartFrm;
	m_uEndFrm = _sourceSection.m_uEndFrm;
	m_dwFadeIn = _sourceSection.m_dwFadeIn;
	m_dwFadeOut = _sourceSection.m_dwFadeOut;

}

LmSection::LmSection(LPCWSTR _pcwsName, UINT _uStartFrm, UINT _uEndFrm, DWORD _dwFadeIn, DWORD _dwFadeOut)
{
	m_wstrSectionName = _pcwsName;
	m_uStartFrm = _uStartFrm;
	m_uEndFrm = _uEndFrm;
	m_dwFadeIn = _dwFadeIn;
	m_dwFadeOut = _dwFadeOut;
}

LmSection::~LmSection(void)
{
}

void LmSection::GetNumSecInfo( LmNumSecInfo * _numSecInfo )
{
	_numSecInfo->m_uStartFrm = m_uStartFrm;
	_numSecInfo->m_uEndFrm = m_uEndFrm;
	_numSecInfo->m_dwFadeIn = m_dwFadeIn;
	_numSecInfo->m_dwFadeOut = m_dwFadeOut;
}

void LmSection::SetNumSecInfo( LmNumSecInfo& _numSecInfo )
{
	m_uStartFrm = _numSecInfo.m_uStartFrm ;
	m_uEndFrm = _numSecInfo.m_uEndFrm;
	m_dwFadeIn = _numSecInfo.m_dwFadeIn;
	m_dwFadeOut = _numSecInfo.m_dwFadeOut;
}

VOID LmSection::SaveSection( CFile& _File )
{
	DWORD dwSize;
	BYTE *pByteSoundName = TheCartoonMaker->StringToMemory(m_wstrSectionName, &dwSize);
	
	//���� ũ�⸦ �����Ѵ�.
	_File.Write(&dwSize, sizeof(dwSize));
	
	//���� ��Ʈ�� ����
	_File.Write(pByteSoundName, (UINT)dwSize);

	LmNumSecInfo numSecInfo;
	GetNumSecInfo(&numSecInfo);

	_File.Write(&numSecInfo, sizeof(LmNumSecInfo));

	//�޸� ����
	delete[] pByteSoundName;

}

VOID LmSection::LoadSection( CFile& _File )
{
	DWORD dwSize;

	//���� ũ�⸦ �д´�.
	_File.Read(&dwSize, sizeof(dwSize));

	BYTE* pByteSoundName = new BYTE[dwSize]  ;

	//���� ��Ʈ�� ����
	_File.Read(pByteSoundName, (UINT)dwSize);
	m_wstrSectionName = (WCHAR*)pByteSoundName;
	
	LmNumSecInfo numSecInfo;
	_File.Write(&numSecInfo, sizeof(LmNumSecInfo));

	SetNumSecInfo(numSecInfo);

	//�޸� ����
	delete[] pByteSoundName;
}