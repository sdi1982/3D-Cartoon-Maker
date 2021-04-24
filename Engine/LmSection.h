//
//
//  @ Project : 3D Ä«Å÷ ¸ÞÀÌÄ¿
//  @ File Name : LmSection.h
//  @ Date : 2007-12-13
//  @ Author : ¾ÈÁø¿Õ
//
//

#if !defined(_LMSECTION_H)
#define _LMSECTION_H

typedef struct NumSecInfo
{
	UINT				m_uStartFrm;
	UINT				m_uEndFrm;
	DWORD				m_dwFadeIn;
	DWORD				m_dwFadeOut;
}LmNumSecInfo;

class LmSection
{
//¸â¹ö ÇÔ¼ö
public:
	LmSection(void);
	LmSection(LPCWSTR _pcwsName, UINT _uStartFrm, UINT _uEndFrm);
	LmSection(LPCWSTR _pcwsName, UINT _uStartFrm, UINT _uEndFrm, DWORD m_dwFadeIn, DWORD m_dwFadeOut);
	LmSection(const LmSection &); //copy Constructor
	~LmSection(void);

	VOID SaveSection(CFile& _File);
	VOID LoadSection(CFile& _File);
	VOID GetNumSecInfo(LmNumSecInfo * _numSecInfo);
	VOID SetNumSecInfo(LmNumSecInfo& _numSecInfo);


//¸â¹ö º¯¼ö
public:
	wstring				m_wstrSectionName;
	UINT				m_uStartFrm;
	UINT				m_uEndFrm;
//[»ç¿îµå]
	DWORD				m_dwFadeIn;
	DWORD				m_dwFadeOut;
};

#endif  //_LMSECTION_H