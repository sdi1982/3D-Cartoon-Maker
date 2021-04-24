//
//
//  @ Project : 3D ī�� ����Ŀ
//  @ File Name : LmLayer.h
//  @ Date : 2007-12-13
//  @ Author : ������
//
//

#if !defined(_LMLAYER_H)
#define _LMLAYER_H

#include "LmSection.h"
#include<map>

enum LmKIND_OF_LAYER {CHARACTER_LAYER=1, SOUND_LAYER, TALKBOX_LAYER, FIREWORKS_LAYER, SNOW_LAYER, CAMERA_LAYER };

//���� �̸��� ������ ���� �� ���� �ֱ� ����. ĳ���� �׼�(�ȱ�)�� ��ǥ�� ����.
//�׷��� <INT,LmSection>��.
typedef map<INT,LmSection>			SectionMap;		
typedef SectionMap::iterator		SectionMapItr;

class LmLayer
{
//��� �Լ�
public:
	LmLayer(void);
	LmLayer(LPCWSTR _pcwsLayerName, LmKIND_OF_LAYER enumKindOfLayer);
	~LmLayer(void);

	LmKIND_OF_LAYER GetKindOfLayer() const { return m_enumKindofLayer; }	//���̾��� ������ �޾ƿ´�. 
	wstring			GetLayerName() const { return m_wstrLayerName; }		//���̾��� �̸��� �޾ƿ´�.
	SectionMap		GetSection() const { return m_mapSection; }				//Section�� ������ �޾ƿ´�.

	INT			InsertSection(LPCWSTR _pcwsSectionName, UINT _uStartFrm, UINT _uEndFrm);		//Section ����
	INT			InsertSection(LPCWSTR _pcwsSectionName, UINT _uStartFrm, UINT _uEndFrm,
								DWORD _dwFadeIn, DWORD _dwFadeOut);								//Section ����
	INT			InsertSection(LmSection &_Section);												//Section ����
	
	INT			RemoveSection(UINT _uStartFrm);													//Section ����
	INT			RemoveSection(LPCWSTR _pcwsSectionName);										//�̸����� Section ����, ���� �̸��� ���� Layer������ ����Ǿ�� ��.
	
	INT			ModifySection(UINT _uStartFrm, LPCWSTR _pcwsSectionName,
							UINT _uNewStartFrm, UINT _uNewEndFrm);							//�̸����� Section ����, ���� �̸��� ���� Layer������ ����Ǿ�� ��.
	INT			ModifySection(LPCWSTR _pcwsSectionName, LPCWSTR _pcwsNewSectionName,
							UINT _uNewStartFrm, UINT _uNewEndFrm);							//Section ����

	INT			ModifySection(LPCWSTR _pcwsSectionName, LPCWSTR _pcwsNewSectionName,
								UINT _uNewStartFrm, UINT _uNewEndFrm, DWORD _dwNewFadeIn, DWORD _dwNewFadeOut);	//Section ���� ���忡�� ���


	INT			CheckSectionArea(UINT _uStartFrm, UINT _uEndFrm);							//���� ���̾�� Section ������ �������� Ȯ���Ѵ�.

	SectionMapItr FindForSecName(LPCWSTR _pcwsSectionName);

//��� ����
public:
	LmKIND_OF_LAYER			m_enumKindofLayer;			//���̾��� ����
	SectionMap				m_mapSection;				//INT �����Ӱ�
	wstring					m_wstrLayerName;			//���̾� �̸�

};

#endif  //_LMLAYER_H