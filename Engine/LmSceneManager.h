//
//
//  @ Project : 3D ī�� ����Ŀ
//  @ File Name : LmSceneManager.h
//  @ Date : 2007-10-11
//  @ Author : �ŵ���
//
//


#if !defined(_LMSCENEMANAGER_H)
#define _LMSCENEMANAGER_H

#include "LmScene.h"

using namespace std;

//
//
//  @ class : �� ������ Ŭ����
//  @ brief : �� ���� �� ���ҽ� �����ڸ� ������ Ŭ����
//  @ date : 2007-10-12
//  @ author : �ŵ���
//  
// 
//  @ ���ǻ��� : ���� ������ UI���� ó���Ѵ�. (m_mapScene������ wstring���� ���� ����, ������ ���常�Ѵ�.)
//				 �߶󳻱� ����� LmSceneManager �Լ��� ����ؼ� UI���� ó���Ѵ�.

class LmSceneManager {
public:
	virtual ~LmSceneManager();
	static LmSceneManager* GetInstance();

	/* �� ���� ���� */
	BOOL CreateScene(LPCWSTR _pcwsScene, UINT _uFrameSize, UINT _uFPS, UINT _uTerTex);	// �̸����� ���� �����Ѵ�.
	BOOL DeleteScene(LPCWSTR _pcwsScene);	// �̸����� ���� �����Ѵ�.

	BOOL CopyScene(LPCWSTR _pcwsSrcScene);	// ���� �����Ѵ�.
	BOOL CutScene(LPCWSTR _pcwsSrcScene);	// ���� �����Ѵ�.
	BOOL PasteScene(LPCWSTR _pcwsDestScene);	// ���� ���δ�.

	BOOL SelectNowScene(LPCWSTR _pcwsScene);		// ���� ���� �����Ѵ�.
	LmScene* GetNowScene(VOID);

	VOID Update(FLOAT _fTimeDelta);
	VOID Render(LPDIRECT3DDEVICE9 _pDevice);


	BOOL Setup(UINT _uWidth, UINT _uHeight);
	VOID Cleanup();

	DWORD GetTotalFrame();

	//���� ���� ����
	VOID SaveSceneList(CFile& _File);
	VOID LoadSceneList(CFile& _File);

	
private:
	LmSceneManager();
	BOOL CheckSameScene(LPCWSTR _pcwsScene);
	LmScene* GetScene(LPCWSTR _pcwsScene);	// ������ ���� �����Ѵ�.
	void ClearSceneMap();				// �� ����Ʈ�� �����.
	map<wstring,LmScene*> m_mapScene;	// �� ������ ���̺�
	
	LmScene* m_pCopyScene;

	wstring m_wstrNowScene;	// ���� ���õ� ��
};

#define TheSceneManager LmSceneManager::GetInstance()

#endif  //_LMSCENEMANAGER_H
