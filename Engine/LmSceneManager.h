//
//
//  @ Project : 3D 카툰 메이커
//  @ File Name : LmSceneManager.h
//  @ Date : 2007-10-11
//  @ Author : 신동인
//
//


#if !defined(_LMSCENEMANAGER_H)
#define _LMSCENEMANAGER_H

#include "LmScene.h"

using namespace std;

//
//
//  @ class : 씬 관리자 클래스
//  @ brief : 씬 편집 및 리소스 관리자를 가지는 클래스
//  @ date : 2007-10-12
//  @ author : 신동인
//  
// 
//  @ 주의사항 : 씬의 순서는 UI에서 처리한다. (m_mapScene에서는 wstring으로 씬을 구별, 데이터 저장만한다.)
//				 잘라내기 기능은 LmSceneManager 함수를 사용해서 UI에서 처리한다.

class LmSceneManager {
public:
	virtual ~LmSceneManager();
	static LmSceneManager* GetInstance();

	/* 씬 편집 관련 */
	BOOL CreateScene(LPCWSTR _pcwsScene, UINT _uFrameSize, UINT _uFPS, UINT _uTerTex);	// 이름으로 씬을 생성한다.
	BOOL DeleteScene(LPCWSTR _pcwsScene);	// 이름으로 씬을 삭제한다.

	BOOL CopyScene(LPCWSTR _pcwsSrcScene);	// 씬을 복사한다.
	BOOL CutScene(LPCWSTR _pcwsSrcScene);	// 씬을 복사한다.
	BOOL PasteScene(LPCWSTR _pcwsDestScene);	// 씬을 붙인다.

	BOOL SelectNowScene(LPCWSTR _pcwsScene);		// 현재 씬을 선택한다.
	LmScene* GetNowScene(VOID);

	VOID Update(FLOAT _fTimeDelta);
	VOID Render(LPDIRECT3DDEVICE9 _pDevice);


	BOOL Setup(UINT _uWidth, UINT _uHeight);
	VOID Cleanup();

	DWORD GetTotalFrame();

	//파일 저장 관련
	VOID SaveSceneList(CFile& _File);
	VOID LoadSceneList(CFile& _File);

	
private:
	LmSceneManager();
	BOOL CheckSameScene(LPCWSTR _pcwsScene);
	LmScene* GetScene(LPCWSTR _pcwsScene);	// 지정한 씬을 리턴한다.
	void ClearSceneMap();				// 씬 리스트를 지운다.
	map<wstring,LmScene*> m_mapScene;	// 씬 포인터 테이블
	
	LmScene* m_pCopyScene;

	wstring m_wstrNowScene;	// 현재 선택된 씬
};

#define TheSceneManager LmSceneManager::GetInstance()

#endif  //_LMSCENEMANAGER_H
