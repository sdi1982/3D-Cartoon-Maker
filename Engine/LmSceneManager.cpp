//
//
//  @ Project : 3D 카툰 메이커
//  @ File Name : LmSceneManager.cpp
//  @ Date : 2007-10-11
//  @ Author : 신동인
//
//

#include "dxstdafx.h"
#include "LmSceneManager.h"
#include "LmTerrain.h"
#include "LmEffectManager.h"
#include "LmPickBoard.h"


LmSceneManager::LmSceneManager()
{
	m_mapScene.clear();
	m_wstrNowScene = L"";
	m_pCopyScene = NULL;
}

LmSceneManager::~LmSceneManager()
{	
	// 동적 할당된 씬을 전부 삭제해야한다.
	ClearSceneMap();

	// 내부적으로 복사한 씬을 해제해준다.
	SAFE_DELETE(m_pCopyScene);
}

LmSceneManager* LmSceneManager::GetInstance()
{
	static LmSceneManager g_SceneManager;
	
	return &g_SceneManager;
}

BOOL LmSceneManager::CreateScene( LPCWSTR _pcwsScene, UINT _uFrameSize, UINT _uFPS, UINT _uTerTex )
{
	if(CheckSameScene(_pcwsScene))
		return FALSE;

	LmScene* NewScene = new LmScene(_pcwsScene, _uFrameSize, _uFPS, _uTerTex);

	if(NewScene == NULL)
		return FALSE;

	m_mapScene[_pcwsScene] = NewScene;

	return TRUE;
}

BOOL LmSceneManager::DeleteScene(LPCWSTR _pcwsScene) 
{
	if(!CheckSameScene(_pcwsScene))
		return FALSE;

	SAFE_DELETE(m_mapScene[_pcwsScene]);
	m_mapScene.erase(_pcwsScene);

	// 현재 씬을 지우면 현재 선택된 씬이 없는 것이 된다.
	m_wstrNowScene = L"";

	return TRUE;
}

// 지정한 씬을 리턴한다.
LmScene* LmSceneManager::GetScene(LPCWSTR _pcwsScene)
{
	if(!CheckSameScene(_pcwsScene))
		return NULL;

	return m_mapScene[_pcwsScene];
}

// 씬을 복사한다.
BOOL LmSceneManager::CopyScene(LPCWSTR _pcwsSrcScene)
{
	if(m_pCopyScene)
		SAFE_DELETE(m_pCopyScene);

	// 본사본을 생성해서 저장하고 있는다.
	LmScene* pScene = GetScene(_pcwsSrcScene);
	if(pScene == NULL)
		return FALSE;

	m_pCopyScene = pScene->Clone(_pcwsSrcScene);
	if(m_pCopyScene == NULL)
		return FALSE;

	return TRUE;
}


BOOL LmSceneManager::CutScene( LPCWSTR _pcwsSrcScene )
{
	// 이전에 복사해 놓은 씬이 있으면 지운다.
	if(m_pCopyScene)
		SAFE_DELETE(m_pCopyScene);

	// "CutScene"이란 이름으로 복사해 놓는다.
	LmScene* pScene = GetScene(_pcwsSrcScene);
	if(pScene == NULL)
		return FALSE;

	m_pCopyScene = pScene->Clone(L"CutScene");
	if(m_pCopyScene == NULL)
		return FALSE;

	// 원본 씬에 소속된 사운드 정보도 삭제 한다.
	TheCartoonMaker->DeleteSceneSound(_pcwsSrcScene);

	// 원본 씬을 지운다.
	return DeleteScene(_pcwsSrcScene);
}

// 씬을 붙인다.
BOOL LmSceneManager::PasteScene(LPCWSTR _pcwsDestScene)
{	
	// 복사한 상태가 아니면 실패한다.
	if(m_pCopyScene == NULL)
		return FALSE;
	
	// 저장된 씬의 복사본을 추가한다.
	m_mapScene[_pcwsDestScene] = m_pCopyScene->Clone(_pcwsDestScene);

	TheCartoonMaker->CopySound( _pcwsDestScene ,m_pCopyScene->GetName() );

	return TRUE;
}

BOOL LmSceneManager::SelectNowScene( LPCWSTR _pcwsScene )
{
	if(!CheckSameScene(_pcwsScene))
		return FALSE;

	wstring wstrNowScene = _pcwsScene;

	if(m_wstrNowScene == wstrNowScene)
		return FALSE;

	// 현재 씬이 선택되면 타임라인을 로드한다.
	LmScene* pNowScene = GetScene(_pcwsScene);
	if(pNowScene == NULL)
		return FALSE;

	pNowScene->InitTimeLine();
	
	m_wstrNowScene = _pcwsScene;

	return TRUE;
}

LmScene* LmSceneManager::GetNowScene(VOID)
{
	if(m_wstrNowScene == L"")
		return NULL;

	return GetScene(m_wstrNowScene.c_str());
}

VOID LmSceneManager::Render( LPDIRECT3DDEVICE9 _pDevice )
{
	// 현재 선택된 씬이 없다면
	if(m_wstrNowScene == L"")
		return;

	GetNowScene()->Render(_pDevice);
}

BOOL LmSceneManager::CheckSameScene( LPCWSTR _pcwsScene )
{
	// 같은 이름이 존재하지 않는다면 FALSE를 리턴
	map<wstring,LmScene*>::iterator result;
	result  = m_mapScene.find(_pcwsScene);

	if(result == m_mapScene.end())
		return FALSE;

	return TRUE;
}

VOID LmSceneManager::Update( FLOAT _fTimeDelta )
{
	// 현재 선택된 씬이 없다면
	if(m_wstrNowScene == L"")
		return;

	GetNowScene()->Update(_fTimeDelta);

}

// 씬 맵을 순회하며 전체 프레임 수를 계산
DWORD LmSceneManager::GetTotalFrame()
{
	DWORD TotalFrame = 0;
	map<wstring,LmScene*>::iterator itrScene;
	for( itrScene = m_mapScene.begin(); itrScene != m_mapScene.end(); itrScene++)
	{
		TotalFrame += itrScene->second->GetFrameSize();
	}

	return TotalFrame;
}

BOOL LmSceneManager::Setup( UINT _uWidth, UINT _uHeight )
{
	map<wstring,LmScene*>::iterator itrScene;
	for( itrScene = m_mapScene.begin(); itrScene != m_mapScene.end(); itrScene++)
	{
		m_mapScene[(*itrScene).first]->Setup(_uWidth, _uHeight);
	}

	return TRUE;
}

VOID LmSceneManager::Cleanup()
{
	map<wstring,LmScene*>::iterator itrScene;
	for( itrScene = m_mapScene.begin(); itrScene != m_mapScene.end(); itrScene++)
	{
		m_mapScene[(*itrScene).first]->Cleanup();
	}
}

VOID LmSceneManager::SaveSceneList( CFile& _File )
{
	UINT uFilePos = (UINT)_File.GetPosition();
	TheCartoonMaker->SetSceneListStart(uFilePos);

	UINT uSceneList = (UINT)m_mapScene.size();
	_File.Write(&uSceneList, sizeof(UINT));

	map<wstring,LmScene*>::iterator itrScene;
	itrScene = m_mapScene.begin();
	for( ;itrScene != m_mapScene.end() ; itrScene++ )
	{
		itrScene->second->SaveScene(_File);
	}

	uFilePos = (UINT)_File.GetPosition();
	TheCartoonMaker->SetSceneListEnd(uFilePos);
}

VOID LmSceneManager::LoadSceneList( CFile& _File )
{
	UINT uFilePosStart = TheCartoonMaker->GetSceneListStart();
	_File.Seek(uFilePosStart, CFile::begin);
	
	UINT nCnt =0;
	ClearSceneMap();
	
	UINT uSceneList = 0;
	_File.Read(&uSceneList, sizeof(UINT));

	for(nCnt = 0 ; nCnt < uSceneList ; nCnt++)
	{
		LmScene *pNewNode  = new LmScene();;
		pNewNode->LoadScene(_File);
		m_mapScene[pNewNode->GetName()] = pNewNode;
	}

}

void LmSceneManager::ClearSceneMap()
{
	map<wstring,LmScene*>::iterator itrScene;
	for( itrScene = m_mapScene.begin(); itrScene != m_mapScene.end(); itrScene++)
	{
		delete m_mapScene[(*itrScene).first];
	}
	m_mapScene.clear();
}