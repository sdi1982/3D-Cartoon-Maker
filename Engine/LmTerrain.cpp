#include "dxstdafx.h"
#include "LmTerrain.h"

#include <fstream>
#include <cmath>

const D3DXCOLOR      WHITE( D3DCOLOR_XRGB(255, 255, 255) );
const D3DXCOLOR      BLACK( D3DCOLOR_XRGB(  0,   0,   0) );
const D3DXCOLOR        RED( D3DCOLOR_XRGB(255,   0,   0) );
const D3DXCOLOR      GREEN( D3DCOLOR_XRGB(  0, 255,   0) );

const D3DXCOLOR BEACH_SAND( D3DCOLOR_XRGB(255, 249, 157) );
const D3DXCOLOR DESERT_SAND( D3DCOLOR_XRGB(250, 205, 135) );

const D3DXCOLOR LIGHTGREEN( D3DCOLOR_XRGB( 60, 184, 120) );
const D3DXCOLOR  PUREGREEN( D3DCOLOR_XRGB(  0, 166,  81) );
const D3DXCOLOR  DARKGREEN( D3DCOLOR_XRGB(  0, 114,  54) );

const D3DXCOLOR LIGHT_YELLOW_GREEN( D3DCOLOR_XRGB(124, 197, 118) );
const D3DXCOLOR  PURE_YELLOW_GREEN( D3DCOLOR_XRGB( 57, 181,  74) );
const D3DXCOLOR  DARK_YELLOW_GREEN( D3DCOLOR_XRGB( 25, 123,  48) );

const D3DXCOLOR LIGHTBROWN(D3DCOLOR_XRGB(198, 156, 109));
const D3DXCOLOR DARKBROWN( D3DCOLOR_XRGB(115, 100,  87));

const DWORD LmTerrain::LmTVertex::FVF = D3DFVF_XYZ | D3DFVF_TEX1;

LmTerrain::LmTerrain( LPDIRECT3DDEVICE9 _pDevice,
					 LPCWSTR _pcwsFile,
					 INT _iVertPerRow, INT _iVertPerCol,
					 INT _iTileSize, /* 타일 간격 (짝수여야한다!!!) */
					 FLOAT _fHeightScale /* 높이 배율 */ )
{
	m_pDevice = _pDevice;
	m_iVertPerRow = _iVertPerRow;
	m_iVertPerCol = _iVertPerCol;
	m_iTileSize = _iTileSize;

	m_iTilePerRow = m_iVertPerRow - 1;
	m_iTilePerCol = m_iVertPerCol - 1;
	m_iWidth = m_iTilePerRow * _iTileSize;
	m_iDepth = m_iTilePerCol * _iTileSize;
	m_iVertexNum = m_iVertPerRow * m_iVertPerCol;
	m_iTriangleNum = m_iTilePerRow * m_iTilePerCol * 2;

	m_fHeightScale = _fHeightScale;

	// 기본은 그리지 않는다.
	m_bTriStrip	= FALSE;

	m_TexType = TERRAIN_GRASS;

	// 높이맵을 로드한다.
	if( !LoadRawFile(_pcwsFile) )
	{
		MessageBox(0, L"LoadRawFile - FAILED", 0, 0);
		PostQuitMessage(0);
	}

	// 높이를 확대한다.
	for(INT i = 0; i < (INT)m_vecHeight.size(); i++)
		m_vecHeight[i] = (INT)(m_vecHeight[i] * m_fHeightScale);

	// 버텍스를 계산한다.
	if( !CalcVertex() )
	{
		MessageBox(0, L"CalcVertex - FAILED", 0, 0);
		PostQuitMessage(0);
	}

	// 인덱스를 계산한다.
	if( !CalcIndex() )
	{
		MessageBox(0, L"CalcIndex - FAILED", 0, 0);
		PostQuitMessage(0);
	}
}

LmTerrain::~LmTerrain()
{
	SAFE_RELEASE(m_pGrassTexture);
	SAFE_RELEASE(m_pDesertTexture);
	SAFE_RELEASE(m_pSnowTexture);
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
}

BOOL LmTerrain::LoadRawFile( LPCWSTR _pcwsFile )
{
	// 각 벡터를 위한 높이
	vector<BYTE> buffer(m_iVertexNum);

	ifstream inFile(_pcwsFile, ios_base::binary);

	if(inFile == 0)
		return FALSE;

	inFile.read((char*)&buffer[0], (streamsize)buffer.size());
	inFile.close();

	// BYTE 벡터를 INT 벡터로 복사한다.
	m_vecHeight.resize(m_iVertexNum);

	for(int i = 0; i < (int)buffer.size(); i++)
		m_vecHeight[i] = buffer[i];

	buffer.clear();

	return TRUE;
}

INT LmTerrain::GetHeightEntry( INT _iRow, INT _iCol )
{
	return m_vecHeight[_iRow * m_iVertPerRow + _iCol];
}

VOID LmTerrain::GetHeightEntry( INT _iRow, INT _iCol, INT _iValue )
{
	m_vecHeight[_iRow * m_iVertPerRow + _iCol] = _iValue;
}

BOOL LmTerrain::CalcVertex( VOID )
{
	HRESULT hr = FALSE;

	// 정점 버퍼를 생성한다.
	hr = m_pDevice->CreateVertexBuffer( 
		m_iVertexNum * sizeof(LmTVertex),
		D3DUSAGE_WRITEONLY,
		LmTVertex::FVF,
		D3DPOOL_MANAGED,
		&m_pVB,
		NULL);

	if(FAILED(hr))
		return FALSE;

	// 버텍스 생성을 시작할 좌표
	INT StartX = -m_iWidth/2;
	INT StartZ = m_iDepth/2;

	// 버텍스 생성을 마칠 좌표
	INT EndX = m_iWidth/2;
	INT EndZ = -m_iDepth/2;

	// 버텍스에서 다음 버텍스로 증가할 텍스쳐 좌표 크기를 계산한다.

	FLOAT uIncSize = 1.0f / (FLOAT)m_iTilePerRow;
	FLOAT vIncSize = 1.0f / (FLOAT)m_iTilePerCol;

	LmTVertex* pV = NULL;
	m_pVB->Lock( 0, 0, (VOID**)&pV, 0);

	INT i = 0;
	for(INT z = StartZ; z >= EndZ; z -= m_iTileSize)
	{
		INT j = 0;
		for(INT x = StartX; x <= EndX; x += m_iTileSize)
		{
			// 중첩된 루프 내의 위치에 따라 버텍스 버퍼와
			// 높이 맵으로의 올바른 인덱스를 계산한다.
			int index = i * m_iVertPerRow + j;
			pV[index] = LmTVertex(
				(FLOAT)x,
				(FLOAT)m_vecHeight[index],
				(FLOAT)z,
				(FLOAT)j * uIncSize,
				(FLOAT)i * vIncSize);
			j++; // 다음 열
		}
		i++; // 다음 행
	}

	m_pVB->Unlock();

	return TRUE;
}

BOOL LmTerrain::CalcIndex( VOID )
{
	HRESULT hr = FALSE;

	hr = m_pDevice->CreateIndexBuffer(
		m_iTriangleNum * 3 * sizeof(WORD), // 삼각형 당 3개의 인덱스
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIB,
		NULL);

	if(FAILED(hr))
		return FALSE;

	WORD* pI = 0; // 인덱스 버퍼의 시작 포인터
	m_pIB->Lock(0, 0, (VOID**)&pI, 0);

	// 하나의 사각형을 구성하는 두개의 삼각형을 나타내기 위한 
	// 6개의 인덱스 그룹의 시작점
	INT index = 0;

	// 각 사각형의 삼각형을 계산하는 루프
	for(INT i = 0; i < m_iTilePerCol; i++ )
	{
		for(INT j = 0; j < m_iTilePerRow; j++)
		{
			pI[index]	  =	(WORD)(		i	* m_iVertPerRow + j		);
			pI[index + 1] =	(WORD)(		i	* m_iVertPerRow + j + 1	);
			pI[index + 2] = (WORD)((i + 1) * m_iVertPerRow + j	);

			pI[index + 3] = (WORD)(	(i + 1) * m_iVertPerRow + j	);
			pI[index + 4] =	(WORD)(		i	* m_iVertPerRow + j + 1	);
			pI[index + 5] = (WORD)(	(i + 1) * m_iVertPerRow + j + 1);

			index += 6;
		}
	}

	m_pIB->Unlock();

	return TRUE;
}

BOOL LmTerrain::LoadDefaultTexture()
{
	HRESULT hr = FALSE;
	WCHAR strPath[MAX_PATH];

	hr = DXUTFindDXSDKMediaFileCch( strPath, MAX_PATH, L"Data\\grass.bmp" );
	if( FAILED(hr) )
	{
		return DXTRACE_ERR( L"DXUTFindDXSDKMediaFileCch", hr );
	}

	hr = D3DXCreateTextureFromFile( m_pDevice, strPath, &m_pGrassTexture);

	if(FAILED(hr))
		return FALSE;

	hr = DXUTFindDXSDKMediaFileCch( strPath, MAX_PATH, L"Data\\desert.bmp" );
	if( FAILED(hr) )
	{
		return DXTRACE_ERR( L"DXUTFindDXSDKMediaFileCch", hr );
	}

	hr = D3DXCreateTextureFromFile(m_pDevice, strPath, &m_pDesertTexture);

	if(FAILED(hr))
		return FALSE;

	hr = DXUTFindDXSDKMediaFileCch( strPath, MAX_PATH, L"Data\\snow.bmp" );
	if( FAILED(hr) )
	{
		return DXTRACE_ERR( L"DXUTFindDXSDKMediaFileCch", hr );
	}

	hr = D3DXCreateTextureFromFile(m_pDevice, strPath, &m_pSnowTexture);

	if(FAILED(hr))
		return FALSE;

	return TRUE;
}

BOOL LmTerrain::CreateTexture( D3DXVECTOR3* _pLightDir )
{
	// 텍스쳐의 최상위 표면을 절차적 방식으로 채우며, 
	// 표면에 조명을 추가한다. D3DXFilterTexture를 이용해
	// 최상위 표면 데이터를 기반으로 다른 밉맵으로 채운다.

	HRESULT hr;

	INT iTexWidth = m_iTilePerRow;
	INT iTexHeight = m_iTilePerCol;


	// 빈 텍스쳐를 만든다.
	hr = D3DXCreateTexture(
		m_pDevice,
		iTexWidth, iTexHeight,
		D3DX_DEFAULT, // 완전한 밉맵 체인을 만든다.
		0, // 이용 - 없음
		D3DFMT_X8B8G8R8, // 32 비트 XRGB 포멧
		D3DPOOL_MANAGED,
		&m_pGrassTexture);

	if(FAILED(hr))
		return FALSE;

	// 특정 레벨의 면의 명세서를 얻어온다.
	D3DSURFACE_DESC TextureDesc;
	m_pGrassTexture->GetLevelDesc( 0, &TextureDesc); 

	// 우리의 코드는 32비트 픽셀을 이용해 텍스쳐를 채우므로
	// 올바른 포멧인지 확인한다.
	
	//if(TextureDesc.Format != D3DFMT_X8B8G8R8)
	//	return FALSE;

	D3DLOCKED_RECT LockedRect;

	m_pGrassTexture->LockRect(
		0,				// 최상위 레벨
		&LockedRect,	// 잠그는 영역의 명세서 포인터
		0,				// 텍스쳐 전체를 잠근다.
		0);

	// 텍스쳐를 체운다.
	DWORD* pImageData = (DWORD*)LockedRect.pBits;

	for(INT i = 0; i < iTexHeight; i++)
	{
		for(INT j = 0; j < iTexWidth; j++)
		{
			D3DXCOLOR color;

			// 사각형의 우측 상단 높이를 구한다.
			FLOAT height = (FLOAT)GetHeightEntry(i, j) / m_fHeightScale;

			// 대응되는 사각형의 높이에 따라 텍셀 컬러를 지정한다.
			if( height < 42.5f ) color = WHITE;
			else if( height < 85.0f) color = PUREGREEN;
			else if( height < 127.5f) color = LIGHTGREEN;
			else if( height < 170.0f) color = WHITE;
			else if( height < 212.5f) color = DARKBROWN;
			else color = WHITE;

			// 잠근 데이터를 채운다. 피치를 4로 나누었음을 확인하자.
			// 피치는 바이트로 주어지며 DWORD는 4바이트로 이루어져 있다.
			pImageData[i * LockedRect.Pitch / 4 + j] = (D3DXCOLOR)color;

		}
	}

	m_pGrassTexture->UnlockRect(0);

	// 지형에 조명을 적용한다.

	if(!LightTerrain(_pLightDir))
	{
		MessageBox(0, L"LightTerrain() - FAILED",0, 0);
		return FALSE;
	}

	// 밉맵을 채운다.
	hr = D3DXFilterTexture(
		m_pGrassTexture, 	// 밉맵 레벨을 채울 텍스쳐
		0,				// 디폴트 팔레트
		0,				// 하위 레벨의 원본으로 최상위 레벨을 이용한다.
		D3DX_DEFAULT);	// 디폴트 필터
	
	if(FAILED(hr))
	{
		MessageBox(0, L"D3DXFilterTexture() - FAILED", 0, 0);
		return FALSE;
	}

	return TRUE;
}

FLOAT LmTerrain::CalcShade( INT _iTileRow, INT _iTileCol, D3DXVECTOR3* pLightDir )
{
	// 사각형의 세 버텍스 높이를 얻는다.
	FLOAT heightA = (FLOAT)GetHeightEntry(_iTileRow,_iTileCol);
	FLOAT heightB = (FLOAT)GetHeightEntry(_iTileRow,_iTileCol+1); 
	FLOAT heightC = (FLOAT)GetHeightEntry(_iTileRow+1 ,_iTileCol);

	// 사각형의 두 벡터를 구성한다.
	D3DXVECTOR3 u((FLOAT)m_iTileSize, heightB - heightA, 0.0f);
	D3DXVECTOR3 v(0.0f, (FLOAT)(heightC - heightA), (FLOAT)(-m_iTileSize));

	// 사각형의 두 벡터 외적을 이용해 법선을 찾아낸다.
	D3DXVECTOR3 n;
	D3DXVec3Cross(&n, &u, &v);
	D3DXVec3Normalize(&n, &n);

	FLOAT cos = D3DXVec3Dot(&n, pLightDir);

	if(cos < 0.0f)
		cos = 0.0f;

	return cos;
}

BOOL LmTerrain::LightTerrain(D3DXVECTOR3* _pLightDir)
{
	HRESULT hr = FALSE;

	D3DSURFACE_DESC textureDesc; 
	m_pGrassTexture->GetLevelDesc(0, &textureDesc);

	// 요구되는 포멧이 32비트 텍스쳐로 채우도록한다.
	if( textureDesc.Format != D3DFMT_X8R8G8B8 )
		return FALSE;

	D3DLOCKED_RECT lockedRect;
	hr = m_pGrassTexture->LockRect(
		0,          // 최상위 밉맵 체인을 잠근다.
		&lockedRect,// 잠긴 데이터
		0,          // 텍스쳐 이미지 전체를 잠근다.
		0);         // 플래그

	if(FAILED(hr))
		return FALSE;

	DWORD* imageData = (DWORD*)lockedRect.pBits;
	for(INT i = 0; i < (int)textureDesc.Height; i++)
	{
		for(INT j = 0; j < (INT)textureDesc.Width; j++)
		{
			INT index = i * lockedRect.Pitch / 4 + j;

			// get current color of quad
			D3DXCOLOR c( imageData[index] );

			// shade current quad
			c *= CalcShade(i, j, _pLightDir);;

			// save shaded color
			imageData[index] = (D3DCOLOR)c;
		}
	}

	m_pGrassTexture->UnlockRect(0);

	return true;
}

FLOAT LmTerrain::GetHeight( FLOAT _x, FLOAT _z )
{
	// 지형의 시작점을 원점으로 이동하는 변환으로
	// xz - 평면 상에서 이동한다.
	_x = ((FLOAT)m_iWidth / 2.0f) + _x;
	_z = ((FLOAT)m_iDepth / 2.0f) - _z;

	// 셀간격을 1로 만드는 변환으로 변환의 배율을 낮춘다.
	// m_iTileSize * 1 / m_iTileSize = 1이므로
	_x /= (FLOAT)m_iTileSize;
	_z /= (FLOAT)m_iTileSize;

	// 현재 셀의 행과 열을 알아낼 수 있다.
	FLOAT col = floorf(_x);
	FLOAT row = floorf(_z);

	//  A   B
	//  *---*
	//  | / |
	//  *---*  
	//  C   D

	FLOAT A = (FLOAT)GetHeightEntry((INT)row,   (INT)col);
	FLOAT B = (FLOAT)GetHeightEntry((INT)row,   (INT)col+1);
	FLOAT C = (FLOAT)GetHeightEntry((INT)row+1, (INT)col);
	FLOAT D = (FLOAT)GetHeightEntry((INT)row+1, (INT)col+1);

	// 셀 상단 버텍스를 원점으로 이동
	FLOAT dx = _x - col;
	FLOAT dz = _z - row;

	FLOAT height = 0.0f;

	if(dz < 1.0f - dx) // 위쪽 삼각형 ABC이면
	{
		FLOAT uy = B - A; // A -> B
		FLOAT vy = C - A; // A -> C
		height = A+ Lerp(0.0f, uy, dx) + Lerp(0.0f, vy, dz);
	}

	else
	{
		FLOAT uy = C - D; // D -> C
		FLOAT vy = B - D; // D -> B
		height = D + Lerp(0.0f, uy, 1.0f - dx) + Lerp(0.0f, vy, 1.0f - dz);
	}

	return height;
}

FLOAT LmTerrain::Lerp( FLOAT _a, FLOAT _b, FLOAT _t )
{
	return _a * (1 - _t) + _b * _t;
}

VOID LmTerrain::Render( LPDIRECT3DDEVICE9 _pDevice )
{
	if( _pDevice )
	{
		D3DXMatrixIdentity(&m_matWorld);

		// 겹치는 것을 방지 하기 위해 약간 띄운다.
		D3DXMatrixTranslation( &m_matWorld, 0.0f, -2.0f, 0.0f );
		_pDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

		_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(LmTVertex));
		_pDevice->SetFVF(LmTVertex::FVF);
		_pDevice->SetIndices(m_pIB);

		switch(m_TexType)
		{
		default:
		case TERRAIN_GRASS: _pDevice->SetTexture(0, m_pGrassTexture); break;
		case TERRAIN_DESERT: _pDevice->SetTexture(0, m_pDesertTexture); break;
		case TERRAIN_SNOW: _pDevice->SetTexture(0, m_pSnowTexture); break;
		}

		// turn off lighting since we're lighting it ourselves
		_pDevice->SetRenderState(D3DRS_LIGHTING, false);

		_pDevice->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			0,
			0,
			m_iVertexNum,
			0,
			m_iTriangleNum);

		_pDevice->SetRenderState(D3DRS_LIGHTING, true);

		//if( m_bTriStrip )
		//{
		//	_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		//	_pDevice->DrawIndexedPrimitive(
		//		D3DPT_TRIANGLELIST,
		//		0,
		//		0,
		//		m_iVertexNum,
		//		0,
		//		m_iTriangleNum);

		//	_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		//}
	}
}

//////////////////////////////////////////////////////////////////////////

LmTerrainManager::LmTerrainManager()
{
}

LmTerrainManager::~LmTerrainManager()
{
	SAFE_DELETE(m_pTerrain);
}

BOOL LmTerrainManager::Init(LPDIRECT3DDEVICE9 _pDevice)
{
	
	WCHAR strPath[MAX_PATH];
	HRESULT hr = DXUTFindDXSDKMediaFileCch( strPath, MAX_PATH, L"Data\\terrain.raw" );
	if( FAILED(hr) )
	{
		return DXTRACE_ERR( L"DXUTFindDXSDKMediaFileCch", hr );
	}

	m_pTerrain = new LmTerrain(_pDevice, strPath, 256, 256, 2, 0.5f);

	m_pTerrain->LoadDefaultTexture();
	//D3DXVECTOR3 lightDir(0.0f, 1.0f, 0.0f);
	//m_pTerrain->CreateTexture(&lightDir);

	return TRUE;
}

VOID LmTerrainManager::Render( LPDIRECT3DDEVICE9 _pDevice )
{
	m_pTerrain->Render(_pDevice);
}

LmTerrainManager* LmTerrainManager::GetInstance()
{
	static LmTerrainManager g_TerrainManager;

	return &g_TerrainManager;
}

VOID LmTerrainManager::SetTerrainType( UINT _TexType )
{
	m_pTerrain->SetTexType(_TexType);
}
