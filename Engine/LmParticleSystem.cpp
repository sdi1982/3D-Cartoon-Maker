#include "dxstdafx.h"
#include "LmParticleSystem.h"

const DWORD LmParticle::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

LmParticleSystem::LmParticleSystem()
{
	m_pVB = NULL;
	m_pTexture = NULL;
}

LmParticleSystem::~LmParticleSystem()
{
	Cleanup();
	SAFE_RELEASE(m_pTexture);
}

BOOL LmParticleSystem::Init( LPDIRECT3DDEVICE9 _pDevice, LPCWSTR _pcwsTexFileName )
{	
	HRESULT hr;

	m_pDevice = _pDevice;
	m_wstrTexFileName = _pcwsTexFileName;

	hr = D3DXCreateTextureFromFile(m_pDevice, m_wstrTexFileName.c_str(), &m_pTexture);
	if(FAILED(hr))
	{
		::MessageBox(0, L"D3DXCreateTextureFromFile() - FAILED", L"PSystem", 0);
		return FALSE;
	}

	return TRUE;
}


BOOL LmParticleSystem::Setup()
{
	HRESULT hr;

	hr = m_pDevice->CreateVertexBuffer( m_dwVBSize * sizeof(LmParticle),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		LmParticle::FVF, D3DPOOL_DEFAULT, &m_pVB, 0);

	if(FAILED(hr))
	{
		::MessageBox(0, L"CreateVertexBuffer() - FAILED", L"PSystem", 0);
		return FALSE;
	}

	return TRUE;
}

VOID LmParticleSystem::Cleanup()
{
	SAFE_RELEASE(m_pVB);
}

VOID LmParticleSystem::Reset()
{
	list<LmPartiAttr>::iterator itr;
	for( itr = m_listParticle.begin(); itr != m_listParticle.end(); itr++)
	{
		ResetParticle(&(*itr));
	}
}

VOID LmParticleSystem::AddParticle()
{
	LmPartiAttr attr;
	ResetParticle(&attr);
	m_listParticle.push_back(attr);
}

VOID LmParticleSystem::PreRender()
{
	m_pDevice->SetRenderState(D3DRS_LIGHTING, false);
	m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	//m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, true);
	m_pDevice->SetRenderState(D3DRS_POINTSIZE, FtoDw(7.0f));
	m_pDevice->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDw(0.0f));

	// 거리에 따른 파티클의 크기를 제어한다.
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_A, FtoDw(0.0f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_B, FtoDw(0.0f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_C, FtoDw(1.0f));

	// 텍스쳐 알파를 이용한다.
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

VOID LmParticleSystem::PostRender()
{
	m_pDevice->SetRenderState(D3DRS_LIGHTING, true);
	//m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, false);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}

VOID LmParticleSystem::RemoveDeadParticle()
{
	list<LmPartiAttr>::iterator itr;

	itr = m_listParticle.begin();

	while( itr != m_listParticle.end())
	{
		if( itr->bAlive == false)
		{
			// erase는 다음 반복자를 리턴
			itr = m_listParticle.erase(itr);
		}
		else
			itr++;
	}
}

VOID LmParticleSystem::Render(LPDIRECT3DDEVICE9 _pDevice)
{
	if(m_listParticle.empty())
		return;

	D3DXMATRIX matWorld;

	D3DXMatrixIdentity(&matWorld);

	_pDevice->SetTransform( D3DTS_WORLD, &matWorld );

	//D3DXMATRIX proj;
	//D3DXMatrixPerspectiveFovLH(
	//	&proj,
	//	D3DX_PI / 4.0f, // 45 - degree
	//	(FLOAT)m_uWidth / (FLOAT)m_uHeight,
	//	1.0f,
	//	5000.0f);

	//_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

	// 렌더링 상태를 지정한다.
	PreRender();

	_pDevice->SetTexture(NULL, m_pTexture);
	_pDevice->SetFVF(LmParticle::FVF);
	_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(LmParticle));

	// 버텍스 버퍼를 벗어날 경우 처음부터 시작
	if( m_dwVBOffset >= m_dwVBSize)
		m_dwVBOffset = 0;

	LmParticle* v = 0;

	m_pVB->Lock( m_dwVBOffset * sizeof(LmParticle),
		m_dwVBBatchSize * sizeof(LmParticle), (VOID**)&v,
		m_dwVBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD );

	DWORD numParticlesInBatch = 0;

	// 모든 파티클이 렌더링될 때까지
	list<LmPartiAttr>::iterator itr;
	for(itr = m_listParticle.begin(); itr != m_listParticle.end(); itr++)
	{
		if(itr->bAlive)
		{
			// 한 단계의 생존한 파티클을 다음 
			// 버텍스버퍼 세그먼트로 복사한다.
			v->vPos = itr->vPos;
			v->dxColor = (D3DCOLOR)itr->dxColor;
			v++; // 다음 요소로;
			numParticlesInBatch++; // 단계 카운터를 증가시킨다.

			if(numParticlesInBatch == m_dwVBBatchSize)
			{
				// 버텍스버퍼로 복사된 마지막 단계의
				// 파티클들을 그린다.
				m_pVB->Unlock();
				_pDevice->DrawPrimitive(D3DPT_POINTLIST,
					m_dwVBOffset, m_dwVBBatchSize);

				// 단계가 그려지는 동안 다음 단계를 파티클로 채운다.
				
				// 다음 단계의 처음 오프셋으로 이동한다.
				m_dwVBOffset += m_dwVBBatchSize;

				// 버텍스버퍼의 경계를 넘는 메모리로 오프셋을 설정하지
				// 않는다. 경계를 넘을 경우 처음부터 시작한다.
				if( m_dwVBOffset >= m_dwVBSize)
					m_dwVBOffset = 0;

				m_pVB->Lock( m_dwVBOffset * sizeof(LmParticle),
					m_dwVBSize * sizeof(LmParticle), (VOID**)&v,
					m_dwVBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

				numParticlesInBatch = 0; // 다음 단계를 위해 리셋
			}
		}
	} // for end

	m_pVB->Unlock();

	// 마지막 단계가 렌더링 되지 않는 경우
	// 일부만 채워진 단계는 이곳에서 렌더링된다.
	if(numParticlesInBatch)
	{
		_pDevice->DrawPrimitive(D3DPT_POINTLIST, m_dwVBOffset,
			numParticlesInBatch);
	}

	m_dwVBOffset += m_dwVBSize;

	PostRender();
}

FLOAT LmParticleSystem::GetRandomFloat( float _fLow, FLOAT _fHigh )
{
	if(_fLow >= _fHigh) // 잘못된 입력
		return _fLow;

	// [0, 1]범위의 임의 float를 얻는다.
	FLOAT f = (rand() % 10000) * 0.0001f;

	// [low, high ]범위의 float을 리턴한다.
	return (f * (_fHigh - _fLow) + _fLow);
}

VOID LmParticleSystem::GetRandomVector( D3DXVECTOR3* _pvOut, D3DXVECTOR3* _vMin, D3DXVECTOR3* _vMax )
{
	_pvOut->x = GetRandomFloat(_vMin->x, _vMax->x);
	_pvOut->y = GetRandomFloat(_vMin->y, _vMax->y);
	_pvOut->z = GetRandomFloat(_vMin->z, _vMax->z);
}

DWORD LmParticleSystem::FtoDw( FLOAT _fNum )
{
	return *((DWORD*)&_fNum);
}

BOOL LmParticleSystem::IsEmpty()
{
	return m_listParticle.empty();
}

BOOL LmParticleSystem::IsDead()
{
	list<LmPartiAttr>::iterator itr;
	for(itr = m_listParticle.begin(); itr != m_listParticle.end(); itr++)
	{
		if( itr->bAlive )
			return FALSE;
	}

	return TRUE;
}
