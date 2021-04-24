//
//
//  @ Project : 3D ī�� ����Ŀ
//  @ File Name : LmScene.h
//  @ Date : 2007-10-11
//  @ Author : �ŵ���
//
//


#if !defined(_LMSCENE_H)
#define _LMSCENE_H

#include "LmCartoonMaker.h"
#include "LmCamera.h"
#include "LmSection.h"
/*
�ٴ��� ũ��� 100.0, 100.0
Ÿ���� ���� 50, 50
��, Ÿ���� ũ��� 2.0 * 2.0
*/

typedef struct LmScene_NUMBER_INFO
{
	UINT m_uFrameSize;	// ������ �� ����
	UINT m_uFPS;		// �ʴ� ������ �� �⺻ 10
	UINT m_uTerrainType;
	UINT m_uOldFrame;	// ���� ������ �ߺ� Ŭ���� ���´�.
}LmSceneNumberInfo;

class LmBackGround;
class LmThing;
class LmChar;

class LmScene {
public:
	LmScene();
	LmScene(LPCWSTR _pcwsName, UINT _uFrameSize, UINT _uFPS, UINT _uTerTex);
	virtual ~LmScene();
	LmScene* Clone(LPCWSTR _pcwsClone);	// ���纻�� �����Ѵ�.

	// ����, ���Կ�����
	LmScene(const LmScene& _rhs);
	LmScene& operator=(LmScene& _rhs);

	void ClearCharAndThing(); //ĳ���Ϳ� ��ü�� Ŭ�����Ѵ�.
	BOOL CopyChar( LPCWSTR _pcwsChar, FLOAT _fStepX, FLOAT _fStepZ );

	BOOL PlaceBG(LPCWSTR _pcwsBG);
	BOOL PlaceThing(LPCWSTR _pcwsThing, FLOAT _fStepX, FLOAT _fStepZ);
	BOOL PlaceChar(LPCWSTR _pcwsChar, FLOAT _fStepX, FLOAT _fStepZ );
	BOOL DeleteBG();
	BOOL DeleteThing(LPCWSTR _pcwsName);
	BOOL DeleteChar(LPCWSTR _pcwsName);

	LmChar* GetChar(LPCWSTR _pcwsName);
	LmThing* GetThing(LPCWSTR _pcwsName);
	LmBackGround* GetBG(LPCWSTR _pcwsName){ return m_pBG; }

	LPCWSTR GetSamePosThing(FLOAT _fStepX, FLOAT _fStepZ);
	LPCWSTR GetSamePosChar(FLOAT _fStepX, FLOAT _fStepZ);

	VOID SetCurrFrame(UINT _uFrame);

	VOID Update(FLOAT _fTimeDelta);
	VOID Render(LPDIRECT3DDEVICE9 _pDevice);
	VOID Setup(UINT _uScreenWidth, UINT _uScreenHeight);
	VOID Cleanup();

	VOID SetTerrainType(UINT _TexType){ m_uTerrainType = _TexType; }
	UINT GetTerrainType(){return m_uTerrainType; }

	CONST LPCWSTR GetName() CONST { return  m_wstrName.c_str(); }
	LmInfoCamera GetCameraInfo(UINT _uFrame);

	BOOL InsertCameraKey(UINT _uFrameNo, LmInfoCamera _CameraEye);
	BOOL RemoveCameraKey(UINT _uFrameNo);
	
	BOOL InsertAction(LPCWSTR _pcwsChar, ACT _uActType, UINT _uStartFrame);
	BOOL RemoveAction(UINT _uFrameNo);

	BOOL InsertSnowKey(UINT _uStartFrameNo, UINT _uEndFrameNo);
	BOOL RemoveSnowKey(UINT _uStartFrameNo);

	BOOL InsertFireworkKey(UINT _uStartFrameNo, UINT _uEndFrameNo);
	BOOL RemoveFireworkKey(UINT _uStartFrameNo);

	BOOL IsInSnowKey(UINT _uFrame );

	//���� ��� �Լ� - ���� ������ Ÿ�Ӷ��� �Ŵ������� �˾Ƽ� ���ش�. 
	VOID InsertSoundSection(LPCWSTR _pcwsSoundName, UINT _uStartFrm, UINT _uEndFrm, DWORD _dwFadeIn, DWORD _dwFadeOut);			//���� Section ����
	INT	RemoveSoundSection(LPCWSTR _pcwsSoundName);																				//���� Section ����

	VOID InitTimeLine();
	UINT GetFrameSize();

	//���� ���� ����
	VOID SaveScene(CFile& _File);
	VOID LoadScene( CFile& _File );

	VOID GetNumberInfo(LmSceneNumberInfo * _pSceneInfo);

	
private:

	BOOL CheckThingName(LPCWSTR _pcwsThing);	// ���� ���� �̸��� �ִ��� Ȯ���Ѵ�.
	BOOL CheckCharName(LPCWSTR _pcwsChar);		// ���� ĳ���� �̸��� �ִ��� Ȯ���Ѵ�.
 	BOOL CheckPos(FLOAT _fStepX, FLOAT _fStepZ);


	VOID LerpFrame(UINT _uPreFrameNo, UINT _uPostFrameNo );
	VOID ResetFrame( UINT _uPreFrameNo, UINT _uPostFrameNo );

	INT FindPreKey(UINT _uFrameNo);
	INT FindPostKey(UINT _uFrameNo);

	wstring m_wstrName;
	LmBackGround* m_pBG;
	map<wstring,LmChar*> m_mapChar;
	map<wstring,LmThing*> m_mapThing;

	vector<LmInfoCamera> m_listInfoCamera;		// ī�޶� ������ ����Ʈ
	vector<UINT>m_listKeyCamera;				// ī�޶� Ű ����Ʈ

	vector<LmKeySnow>	   m_listKeySnow;			// �������� ����Ʈ Ű ����Ʈ
	vector<LmInfoFirework> m_listInfoFirework;		// �Ҳɳ��� ����Ʈ Ű ����Ʈ
	vector<UINT>		   m_listKeyFirework;

	vector<LmSection> m_listInfoSound;		// ���� Ű ����Ʈ

	UINT m_uFrameSize;	// ������ �� ����
	UINT m_uFPS;		// �ʴ� ������ �� �⺻ 10
	UINT m_uTerrainType;
	UINT m_uOldFrame;	// ���� ������ �ߺ� Ŭ���� ���´�.
	LPDIRECT3DDEVICE9 m_pDevice;
};

#endif  //_LMSCENE_H
