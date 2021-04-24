#include "dxstdafx.h"
#include "LmEffectManager.h"


LmSnowParticle::LmSnowParticle( LmBoundBox* _pBoundBox, INT _numParticle )
{
	m_BoundBox = *_pBoundBox;
	m_fSize = 0.8f;
	m_dwVBSize = 2048;
	m_dwVBOffset = 0;
	m_dwVBBatchSize = 512;

	for(int i = 0; i < _numParticle; i++)
		AddParticle();
}

VOID LmSnowParticle::ResetParticle( LmPartiAttr* attr )
{
	// 임의의 x, z 좌표 위체에 눈송이를 만들어
	// y좌표를 경계 상자의 최상단과 같도록 지정하고 
	// 속도를 부여하여 아래 방향, 약간 왼쪽으로 떨어지도록 한다.

	attr->bAlive = true;

	// 눈송이의 위치 지정을 위해 임의의 x, y 좌표를 얻는다.
	GetRandomVector(&attr->vPos, &m_BoundBox.vMin, &m_BoundBox.vMax);

	// 높이(y좌표)는 항상 경계 상자의 최상단이 된다.
	attr->vPos.y = m_BoundBox.vMax.y;
	attr->vVel.x = GetRandomFloat(0.0f, 1.0f)* -3.0f;
	attr->vVel.y = GetRandomFloat(0.0f, 1.0f)* -10.0f;
	attr->vVel.z = 0.0f;

	attr->dxColor = D3DCOLOR_XRGB(255, 255, 255);
}

VOID LmSnowParticle::Update( FLOAT _fTimeDelta )
{
	list<LmPartiAttr>::iterator itr;
	for(itr = m_listParticle.begin(); itr != m_listParticle.end(); itr++)
	{
		itr->vPos += itr->vVel * _fTimeDelta;

		// 포인트가 경계를 벗어났는가?
		if(m_BoundBox.isPointInside(itr->vPos) == false)
		{
			ResetParticle(&(*itr));
		}
	}
}

//////////////////////////////////////////////////////////////////////////

LmFireworkParticle::LmFireworkParticle( D3DXVECTOR3* _vOrigin, INT _iNumParticles )
{

	m_vOrigin = *_vOrigin;
	m_fSize = 0.9f;
	m_dwVBSize = 2048;
	m_dwVBOffset = 0;
	m_dwVBBatchSize = 512;

	for(int i = 0; i < _iNumParticles; i++)
		AddParticle();
}

void LmFireworkParticle::ResetParticle( LmPartiAttr* pAttr )
{
	pAttr->bAlive  = true;
	pAttr->vPos = m_vOrigin;

	D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
	D3DXVECTOR3 max = D3DXVECTOR3( 1.0f,  1.0f,  1.0f);

	GetRandomVector( &pAttr->vVel, &min, &max);

	// normalize to make spherical
	D3DXVec3Normalize( &pAttr->vVel, &pAttr->vVel);

	pAttr->vVel *= 100.0f;

	pAttr->dxColor = D3DXCOLOR(
		GetRandomFloat(0.0f, 1.0f),
		GetRandomFloat(0.0f, 1.0f),
		GetRandomFloat(0.0f, 1.0f),
		1.0f);

	pAttr->fAge      = 0.0f;
	pAttr->fLifeTime = 10.0f; // lives for 2 seconds
}

void LmFireworkParticle::Update( FLOAT _fTimeDelta )
{
	list<LmPartiAttr>::iterator itr;

	for(itr = m_listParticle.begin(); itr != m_listParticle.end(); itr++)
		ResetParticle(&(*itr));

	for(itr = m_listParticle.begin(); itr != m_listParticle.end(); itr++)
	{
		// only update living particles
		if( itr->bAlive )
		{
			itr->vPos += itr->vVel * _fTimeDelta;

			itr->fAge += _fTimeDelta;

			if(itr->fAge > itr->fLifeTime) // kill 
				itr->bAlive = false;
		}
	}
}

void LmFireworkParticle::PreRender()
{
	LmParticleSystem::PreRender();

	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// read, but don't write particles to z-buffer
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void LmFireworkParticle::PostRender()
{
	LmParticleSystem::PostRender();

	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
}


//////////////////////////////////////////////////////////////////////////

LmEffectManager::LmEffectManager()
{
	m_pSnow = NULL;
	m_pExp = NULL;
	m_uEffectNo = 0;
	m_bShowSnow = FALSE;
	m_bShowFirework = FALSE;
	m_pDevice = NULL;
}

LmEffectManager::~LmEffectManager()
{
	SAFE_DELETE(m_pSnow);
	SAFE_DELETE(m_pExp);
}

LmEffectManager* LmEffectManager::GetInstance()
{
	static LmEffectManager g_EffectManager;
	return &g_EffectManager;
}

BOOL LmEffectManager::Init( LPDIRECT3DDEVICE9 _pDevice )
{
	HRESULT hr;
	WCHAR strPath[MAX_PATH];
	BOOL bResult;

	m_pDevice = _pDevice;

	srand((unsigned int)time(0));

	LmBoundBox boundingBox;
	boundingBox.vMin = D3DXVECTOR3(-400.0f,  -0.0f,  -400.0f);
	boundingBox.vMax = D3DXVECTOR3( 400.0f,  300.0f,  400.0f);

	m_pSnow = new LmSnowParticle(&boundingBox, 500);

	D3DXVECTOR3 origin(0.0f, 200.0f, 0.0f);
	m_pExp = new LmFireworkParticle(&origin, 2000);


	hr = DXUTFindDXSDKMediaFileCch( strPath, MAX_PATH, L"Data\\snowflake.dds" );
	if( FAILED(hr) )
		return FALSE;

	m_wstrSnow = strPath;

	hr = DXUTFindDXSDKMediaFileCch(strPath, MAX_PATH, L"Data\\flare.bmp" );
	if( FAILED(hr) )
		return FALSE;

	m_wstrFirework = strPath;

	bResult =m_pSnow->Init(m_pDevice, m_wstrSnow.c_str());
	if(!bResult)
		return FALSE;


	return m_pExp->Init(m_pDevice, m_wstrFirework.c_str());
}

VOID LmEffectManager::Render( LPDIRECT3DDEVICE9 _pDevice )
{
	if(_pDevice)
	{
		if(m_pSnow == NULL)
			return;

		if(m_pExp == NULL)
			return;

		switch(m_uEffectNo)
		{
		default: /* 효과 없음*/ break;
		case 1: 
			if(m_bShowSnow)
				m_pSnow->Render(_pDevice); 
			break;
		case 2: 
			if(m_bShowFirework)
				m_pExp->Render(_pDevice); 
			break;
		case 3:
			if(m_bShowSnow)
				m_pSnow->Render(_pDevice); 
			if(m_bShowFirework)
				m_pExp->Render(_pDevice); 
			break;

		}
	}
}

VOID LmEffectManager::Cleanup()
{
	m_pSnow->Cleanup();
	m_pExp->Cleanup();
}

BOOL LmEffectManager::Setup()
{	
	m_pSnow->Setup();
	m_pExp->Setup();

	return TRUE;
}

VOID LmEffectManager::SetSnowTime( FLOAT _fTimeDelta )
{
	if(m_bShowSnow)
		m_pSnow->Update(_fTimeDelta); 
}

VOID LmEffectManager::SetFireworkTime( FLOAT _fTimeDelta )
{
	if(m_bShowFirework)
		m_pExp->Update(_fTimeDelta); 
}