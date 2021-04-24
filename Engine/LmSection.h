//
//
//  @ Project : 3D ī�� ����Ŀ
//  @ File Name : LmSection.h
//  @ Date : 2007-12-13
//  @ Author : ������
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
//��� �Լ�
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


//��� ����
public:
	wstring				m_wstrSectionName;
	UINT				m_uStartFrm;
	UINT				m_uEndFrm;
//[����]
	DWORD				m_dwFadeIn;
	DWORD				m_dwFadeOut;
};

#endif  //_LMSECTION_H