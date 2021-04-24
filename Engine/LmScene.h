//
//
//  @ Project : 3D 카툰 메이커
//  @ File Name : LmScene.h
//  @ Date : 2007-10-11
//  @ Author : 신동인
//
//


#if !defined(_LMSCENE_H)
#define _LMSCENE_H

#include "LmCartoonMaker.h"
#include "LmCamera.h"
#include "LmSection.h"
/*
바닥의 크기는 100.0, 100.0
타일의 수는 50, 50
즉, 타일의 크기는 2.0 * 2.0
*/

typedef struct LmScene_NUMBER_INFO
{
	UINT m_uFrameSize;	// 프레임 총 길이
	UINT m_uFPS;		// 초당 프레임 수 기본 10
	UINT m_uTerrainType;
	UINT m_uOldFrame;	// 같은 프레임 중복 클릭을 막는다.
}LmSceneNumberInfo;

class LmBackGround;
class LmThing;
class LmChar;

class LmScene {
public:
	LmScene();
	LmScene(LPCWSTR _pcwsName, UINT _uFrameSize, UINT _uFPS, UINT _uTerTex);
	virtual ~LmScene();
	LmScene* Clone(LPCWSTR _pcwsClone);	// 복사본을 리턴한다.

	// 복사, 대입연산자
	LmScene(const LmScene& _rhs);
	LmScene& operator=(LmScene& _rhs);

	void ClearCharAndThing(); //캐릭터와 물체를 클리어한다.
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

	//사운드 멤버 함수 - 사운드 수정은 타임라인 매니져에서 알아서 해준다. 
	VOID InsertSoundSection(LPCWSTR _pcwsSoundName, UINT _uStartFrm, UINT _uEndFrm, DWORD _dwFadeIn, DWORD _dwFadeOut);			//사운드 Section 삽입
	INT	RemoveSoundSection(LPCWSTR _pcwsSoundName);																				//사운드 Section 삭제

	VOID InitTimeLine();
	UINT GetFrameSize();

	//파일 저장 관련
	VOID SaveScene(CFile& _File);
	VOID LoadScene( CFile& _File );

	VOID GetNumberInfo(LmSceneNumberInfo * _pSceneInfo);

	
private:

	BOOL CheckThingName(LPCWSTR _pcwsThing);	// 같은 물건 이름이 있는지 확인한다.
	BOOL CheckCharName(LPCWSTR _pcwsChar);		// 같은 캐릭터 이름이 있는지 확인한다.
 	BOOL CheckPos(FLOAT _fStepX, FLOAT _fStepZ);


	VOID LerpFrame(UINT _uPreFrameNo, UINT _uPostFrameNo );
	VOID ResetFrame( UINT _uPreFrameNo, UINT _uPostFrameNo );

	INT FindPreKey(UINT _uFrameNo);
	INT FindPostKey(UINT _uFrameNo);

	wstring m_wstrName;
	LmBackGround* m_pBG;
	map<wstring,LmChar*> m_mapChar;
	map<wstring,LmThing*> m_mapThing;

	vector<LmInfoCamera> m_listInfoCamera;		// 카메라 프레임 리스트
	vector<UINT>m_listKeyCamera;				// 카메라 키 리스트

	vector<LmKeySnow>	   m_listKeySnow;			// 눈내리기 이펙트 키 리스트
	vector<LmInfoFirework> m_listInfoFirework;		// 불꽃놀이 이펙트 키 리스트
	vector<UINT>		   m_listKeyFirework;

	vector<LmSection> m_listInfoSound;		// 사운드 키 리스트

	UINT m_uFrameSize;	// 프레임 총 길이
	UINT m_uFPS;		// 초당 프레임 수 기본 10
	UINT m_uTerrainType;
	UINT m_uOldFrame;	// 같은 프레임 중복 클릭을 막는다.
	LPDIRECT3DDEVICE9 m_pDevice;
};

#endif  //_LMSCENE_H
