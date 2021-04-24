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

	// �Ÿ��� ���� ��ƼŬ�� ũ�⸦ �����Ѵ�.
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_A, FtoDw(0.0f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_B, FtoDw(0.0f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_C, FtoDw(1.0f));

	// �ؽ��� ���ĸ� �̿��Ѵ�.
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
			// erase�� ���� �ݺ��ڸ� ����
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

	// ������ ���¸� �����Ѵ�.
	PreRender();

	_pDevice->SetTexture(NULL, m_pTexture);
	_pDevice->SetFVF(LmParticle::FVF);
	_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(LmParticle));

	// ���ؽ� ���۸� ��� ��� ó������ ����
	if( m_dwVBOffset >= m_dwVBSize)
		m_dwVBOffset = 0;

	LmParticle* v = 0;

	m_pVB->Lock( m_dwVBOffset * sizeof(LmParticle),
		m_dwVBBatchSize * sizeof(LmParticle), (VOID**)&v,
		m_dwVBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD );

	DWORD numParticlesInBatch = 0;

	// ��� ��ƼŬ�� �������� ������
	list<LmPartiAttr>::iterator itr;
	for(itr = m_listParticle.begin(); itr != m_listParticle.end(); itr++)
	{
		if(itr->bAlive)
		{
			// �� �ܰ��� ������ ��ƼŬ�� ���� 
			// ���ؽ����� ���׸�Ʈ�� �����Ѵ�.
			v->vPos = itr->vPos;
			v->dxColor = (D3DCOLOR)itr->dxColor;
			v++; // ���� ��ҷ�;
			numParticlesInBatch++; // �ܰ� ī���͸� ������Ų��.

			if(numParticlesInBatch == m_dwVBBatchSize)
			{
				// ���ؽ����۷� ����� ������ �ܰ���
				// ��ƼŬ���� �׸���.
				m_pVB->Unlock();
				_pDevice->DrawPrimitive(D3DPT_POINTLIST,
					m_dwVBOffset, m_dwVBBatchSize);

				// �ܰ谡 �׷����� ���� ���� �ܰ踦 ��ƼŬ�� ä���.
				
				// ���� �ܰ��� ó�� ���������� �̵��Ѵ�.
				m_dwVBOffset += m_dwVBBatchSize;

				// ���ؽ������� ��踦 �Ѵ� �޸𸮷� �������� ��������
				// �ʴ´�. ��踦 ���� ��� ó������ �����Ѵ�.
				if( m_dwVBOffset >= m_dwVBSize)
					m_dwVBOffset = 0;

				m_pVB->Lock( m_dwVBOffset * sizeof(LmParticle),
					m_dwVBSize * sizeof(LmParticle), (VOID**)&v,
					m_dwVBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

				numParticlesInBatch = 0; // ���� �ܰ踦 ���� ����
			}
		}
	} // for end

	m_pVB->Unlock();

	// ������ �ܰ谡 ������ ���� �ʴ� ���
	// �Ϻθ� ä���� �ܰ�� �̰����� �������ȴ�.
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
	if(_fLow >= _fHigh) // �߸��� �Է�
		return _fLow;

	// [0, 1]������ ���� float�� ��´�.
	FLOAT f = (rand() % 10000) * 0.0001f;

	// [low, high ]������ float�� �����Ѵ�.
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
