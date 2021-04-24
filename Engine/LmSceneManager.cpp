//
//
//  @ Project : 3D ī�� ����Ŀ
//  @ File Name : LmSceneManager.cpp
//  @ Date : 2007-10-11
//  @ Author : �ŵ���
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
	// ���� �Ҵ�� ���� ���� �����ؾ��Ѵ�.
	ClearSceneMap();

	// ���������� ������ ���� �������ش�.
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

	// ���� ���� ����� ���� ���õ� ���� ���� ���� �ȴ�.
	m_wstrNowScene = L"";

	return TRUE;
}

// ������ ���� �����Ѵ�.
LmScene* LmSceneManager::GetScene(LPCWSTR _pcwsScene)
{
	if(!CheckSameScene(_pcwsScene))
		return NULL;

	return m_mapScene[_pcwsScene];
}

// ���� �����Ѵ�.
BOOL LmSceneManager::CopyScene(LPCWSTR _pcwsSrcScene)
{
	if(m_pCopyScene)
		SAFE_DELETE(m_pCopyScene);

	// ���纻�� �����ؼ� �����ϰ� �ִ´�.
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
	// ������ ������ ���� ���� ������ �����.
	if(m_pCopyScene)
		SAFE_DELETE(m_pCopyScene);

	// "CutScene"�̶� �̸����� ������ ���´�.
	LmScene* pScene = GetScene(_pcwsSrcScene);
	if(pScene == NULL)
		return FALSE;

	m_pCopyScene = pScene->Clone(L"CutScene");
	if(m_pCopyScene == NULL)
		return FALSE;

	// ���� ���� �Ҽӵ� ���� ������ ���� �Ѵ�.
	TheCartoonMaker->DeleteSceneSound(_pcwsSrcScene);

	// ���� ���� �����.
	return DeleteScene(_pcwsSrcScene);
}

// ���� ���δ�.
BOOL LmSceneManager::PasteScene(LPCWSTR _pcwsDestScene)
{	
	// ������ ���°� �ƴϸ� �����Ѵ�.
	if(m_pCopyScene == NULL)
		return FALSE;
	
	// ����� ���� ���纻�� �߰��Ѵ�.
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

	// ���� ���� ���õǸ� Ÿ�Ӷ����� �ε��Ѵ�.
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
	// ���� ���õ� ���� ���ٸ�
	if(m_wstrNowScene == L"")
		return;

	GetNowScene()->Render(_pDevice);
}

BOOL LmSceneManager::CheckSameScene( LPCWSTR _pcwsScene )
{
	// ���� �̸��� �������� �ʴ´ٸ� FALSE�� ����
	map<wstring,LmScene*>::iterator result;
	result  = m_mapScene.find(_pcwsScene);

	if(result == m_mapScene.end())
		return FALSE;

	return TRUE;
}

VOID LmSceneManager::Update( FLOAT _fTimeDelta )
{
	// ���� ���õ� ���� ���ٸ�
	if(m_wstrNowScene == L"")
		return;

	GetNowScene()->Update(_fTimeDelta);

}

// �� ���� ��ȸ�ϸ� ��ü ������ ���� ���
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