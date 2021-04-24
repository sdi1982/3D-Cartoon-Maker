#include "dxstdafx.h"
#include "LmPickBoard.h"

CONST DWORD LmPickBoard::LmBVertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

BOOL LmPickBoard::Init(LPDIRECT3DDEVICE9 _pDevice, INT _iVertPerRow, INT _iVertPerCol, INT _iTileSize)

{
	m_iVertPerRow = _iVertPerRow;
	m_iVertPerCol = _iVertPerCol;
	m_iTileSize = _iTileSize;

	m_iTilePerRow = m_iVertPerRow - 1;
	m_iTilePerCol = m_iVertPerCol - 1;
	m_iWidth = m_iTilePerRow * _iTileSize;
	m_iDepth = m_iTilePerCol * _iTileSize;
	m_iVertexNum = m_iVertPerRow * m_iVertPerCol;
	m_iTriangleNum = m_iTilePerRow * m_iTilePerCol * 2;

	// 버텍스를 계산한다.
	if( !CalcVertex(_pDevice) )
	{
		return FALSE;
	}

	m_pDragThing = NULL;
	m_pDragChar = NULL;
	return TRUE;
}

LmPickBoard::LmPickBoard()
{
	m_pVB = NULL;
	m_pDragThing = NULL;
	m_bInter= FALSE;
	m_bShow = FALSE;
}
LmPickBoard::~LmPickBoard()
{
	SAFE_RELEASE(m_pVB);
}

BOOL LmPickBoard::CalcVertex( LPDIRECT3DDEVICE9 _pDevice )
{
	HRESULT hr = FALSE;

	// 정점 버퍼를 생성한다.
	hr = _pDevice->CreateVertexBuffer( 
		m_iVertexNum * sizeof(LmBVertex),
		D3DUSAGE_WRITEONLY,
		LmBVertex::FVF,
		D3DPOOL_MANAGED,
		&m_pVB,
		NULL);

	if(FAILED(hr))
		return FALSE;


	LmBVertex* pV = NULL;
	m_pVB->Lock( 0, 0, (VOID**)&pV, 0);

	int index = 0;
	// 가로줄 점찍기
	for(INT d = m_iDepth/2; d >= -m_iDepth/2; d -= m_iTileSize)
	{
		pV[index++] = LmBVertex((FLOAT)-m_iWidth/2, 0.0f, (FLOAT)d, D3DCOLOR_ARGB(128,0,255,0));
		pV[index++] = LmBVertex((FLOAT)m_iWidth/2, 0.0f, (FLOAT)d, D3DCOLOR_ARGB(128,0,255,0));
	}

	// 세로줄 점찍기
	for(INT w = -m_iWidth/2; w <= m_iWidth/2; w += m_iTileSize)
	{
		pV[index++] = LmBVertex((FLOAT)w, 0.0f, (FLOAT)m_iDepth/2, D3DCOLOR_ARGB(128,0,255,0));
		pV[index++] = LmBVertex((FLOAT)w, 0.0f, (FLOAT)-m_iDepth/2, D3DCOLOR_ARGB(128,0,255,0));
	}

	m_pVB->Unlock();

	// 타일의 버텍스 버퍼를 생성한다.
	hr = _pDevice->CreateVertexBuffer( 
		4 * sizeof(LmBVertex),
		D3DUSAGE_WRITEONLY,
		LmBVertex::FVF,
		D3DPOOL_MANAGED,
		&m_pTileVB,
		NULL);

	if(FAILED(hr))
		return FALSE;

	return TRUE;
}

VOID LmPickBoard::Render( LPDIRECT3DDEVICE9 _pDevice )
{
	if( _pDevice == NULL)
		return;

	if( m_bShow)
	{
		D3DXMatrixIdentity(&m_matWorld);

		// 겹치는 것을 방지 하기 위해 약간 띄운다.
		D3DXMatrixTranslation( &m_matWorld, 0.0f, 1.0f, 0.0f );
		_pDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

		_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(LmBVertex));
		_pDevice->SetFVF(LmBVertex::FVF);
		_pDevice->SetTexture(0, NULL);

		_pDevice->SetRenderState(D3DRS_LIGHTING, false);
		_pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
		_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
		
		_pDevice->DrawPrimitive( D3DPT_LINELIST, 0, m_iVertPerRow+m_iVertPerCol);

		_pDevice->SetRenderState(D3DRS_LIGHTING, true);

		// 선택된 모델을 그린다.
		if(IsInter())
		{
			// 물체
			if(m_pDragThing)
			{
				m_pDragThing->Place(GetTileX(), GetTileZ());

				_pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
				m_pDragThing->Render(_pDevice);
				_pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
			}

			// 캐릭터
			if(m_pDragChar)
			{
				m_pDragChar->Place(GetTileX(), GetTileZ());

				_pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
				m_pDragChar->Render(_pDevice);
				_pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
			}
		}
	}
}

VOID LmPickBoard::SetFloorPt( FLOAT _fX, FLOAT _fZ )
{
	m_fFloorX = _fX;
	m_fFloorZ = _fZ;
}

LmPickBoard* LmPickBoard::GetInstance()
{
	static LmPickBoard g_pickBoard;

	return &g_pickBoard;
}

FLOAT LmPickBoard::GetTileX()
{
	INT TileX = (INT)(m_fFloorX / m_iTileSize);

	if(m_fFloorX >= 0)
		return (FLOAT)(TileX * m_iTileSize + m_iTileSize/2);
	else
		return (FLOAT)(TileX * m_iTileSize - m_iTileSize/2);
}

FLOAT LmPickBoard::GetTileZ()
{
	INT TileZ = (INT)(m_fFloorZ / m_iTileSize);

	if(m_fFloorZ >= 0)
		return (FLOAT)(TileZ * m_iTileSize + m_iTileSize/2);
	else
		return (FLOAT)(TileZ * m_iTileSize - m_iTileSize/2);
}

VOID LmPickBoard::Update( FLOAT _fTimeDelta )
{
	if(m_pDragThing)
		m_pDragThing->Update(_fTimeDelta);

	if(m_pDragChar)
		m_pDragChar->Update(_fTimeDelta);
}

VOID LmPickBoard::Show( BOOL _bShow )
{
	m_pDragChar = NULL;
	m_pDragThing = NULL;
	m_bShow = _bShow;
}