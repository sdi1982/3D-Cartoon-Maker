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
					 INT _iTileSize, /* Ÿ�� ���� (¦�������Ѵ�!!!) */
					 FLOAT _fHeightScale /* ���� ���� */ )
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

	// �⺻�� �׸��� �ʴ´�.
	m_bTriStrip	= FALSE;

	m_TexType = TERRAIN_GRASS;

	// ���̸��� �ε��Ѵ�.
	if( !LoadRawFile(_pcwsFile) )
	{
		MessageBox(0, L"LoadRawFile - FAILED", 0, 0);
		PostQuitMessage(0);
	}

	// ���̸� Ȯ���Ѵ�.
	for(INT i = 0; i < (INT)m_vecHeight.size(); i++)
		m_vecHeight[i] = (INT)(m_vecHeight[i] * m_fHeightScale);

	// ���ؽ��� ����Ѵ�.
	if( !CalcVertex() )
	{
		MessageBox(0, L"CalcVertex - FAILED", 0, 0);
		PostQuitMessage(0);
	}

	// �ε����� ����Ѵ�.
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
	// �� ���͸� ���� ����
	vector<BYTE> buffer(m_iVertexNum);

	ifstream inFile(_pcwsFile, ios_base::binary);

	if(inFile == 0)
		return FALSE;

	inFile.read((char*)&buffer[0], (streamsize)buffer.size());
	inFile.close();

	// BYTE ���͸� INT ���ͷ� �����Ѵ�.
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

	// ���� ���۸� �����Ѵ�.
	hr = m_pDevice->CreateVertexBuffer( 
		m_iVertexNum * sizeof(LmTVertex),
		D3DUSAGE_WRITEONLY,
		LmTVertex::FVF,
		D3DPOOL_MANAGED,
		&m_pVB,
		NULL);

	if(FAILED(hr))
		return FALSE;

	// ���ؽ� ������ ������ ��ǥ
	INT StartX = -m_iWidth/2;
	INT StartZ = m_iDepth/2;

	// ���ؽ� ������ ��ĥ ��ǥ
	INT EndX = m_iWidth/2;
	INT EndZ = -m_iDepth/2;

	// ���ؽ����� ���� ���ؽ��� ������ �ؽ��� ��ǥ ũ�⸦ ����Ѵ�.

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
			// ��ø�� ���� ���� ��ġ�� ���� ���ؽ� ���ۿ�
			// ���� �������� �ùٸ� �ε����� ����Ѵ�.
			int index = i * m_iVertPerRow + j;
			pV[index] = LmTVertex(
				(FLOAT)x,
				(FLOAT)m_vecHeight[index],
				(FLOAT)z,
				(FLOAT)j * uIncSize,
				(FLOAT)i * vIncSize);
			j++; // ���� ��
		}
		i++; // ���� ��
	}

	m_pVB->Unlock();

	return TRUE;
}

BOOL LmTerrain::CalcIndex( VOID )
{
	HRESULT hr = FALSE;

	hr = m_pDevice->CreateIndexBuffer(
		m_iTriangleNum * 3 * sizeof(WORD), // �ﰢ�� �� 3���� �ε���
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIB,
		NULL);

	if(FAILED(hr))
		return FALSE;

	WORD* pI = 0; // �ε��� ������ ���� ������
	m_pIB->Lock(0, 0, (VOID**)&pI, 0);

	// �ϳ��� �簢���� �����ϴ� �ΰ��� �ﰢ���� ��Ÿ���� ���� 
	// 6���� �ε��� �׷��� ������
	INT index = 0;

	// �� �簢���� �ﰢ���� ����ϴ� ����
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
	// �ؽ����� �ֻ��� ǥ���� ������ ������� ä���, 
	// ǥ�鿡 ������ �߰��Ѵ�. D3DXFilterTexture�� �̿���
	// �ֻ��� ǥ�� �����͸� ������� �ٸ� �Ӹ����� ä���.

	HRESULT hr;

	INT iTexWidth = m_iTilePerRow;
	INT iTexHeight = m_iTilePerCol;


	// �� �ؽ��ĸ� �����.
	hr = D3DXCreateTexture(
		m_pDevice,
		iTexWidth, iTexHeight,
		D3DX_DEFAULT, // ������ �Ӹ� ü���� �����.
		0, // �̿� - ����
		D3DFMT_X8B8G8R8, // 32 ��Ʈ XRGB ����
		D3DPOOL_MANAGED,
		&m_pGrassTexture);

	if(FAILED(hr))
		return FALSE;

	// Ư�� ������ ���� ������ ���´�.
	D3DSURFACE_DESC TextureDesc;
	m_pGrassTexture->GetLevelDesc( 0, &TextureDesc); 

	// �츮�� �ڵ�� 32��Ʈ �ȼ��� �̿��� �ؽ��ĸ� ä��Ƿ�
	// �ùٸ� �������� Ȯ���Ѵ�.
	
	//if(TextureDesc.Format != D3DFMT_X8B8G8R8)
	//	return FALSE;

	D3DLOCKED_RECT LockedRect;

	m_pGrassTexture->LockRect(
		0,				// �ֻ��� ����
		&LockedRect,	// ��״� ������ ���� ������
		0,				// �ؽ��� ��ü�� ��ٴ�.
		0);

	// �ؽ��ĸ� ü���.
	DWORD* pImageData = (DWORD*)LockedRect.pBits;

	for(INT i = 0; i < iTexHeight; i++)
	{
		for(INT j = 0; j < iTexWidth; j++)
		{
			D3DXCOLOR color;

			// �簢���� ���� ��� ���̸� ���Ѵ�.
			FLOAT height = (FLOAT)GetHeightEntry(i, j) / m_fHeightScale;

			// �����Ǵ� �簢���� ���̿� ���� �ؼ� �÷��� �����Ѵ�.
			if( height < 42.5f ) color = WHITE;
			else if( height < 85.0f) color = PUREGREEN;
			else if( height < 127.5f) color = LIGHTGREEN;
			else if( height < 170.0f) color = WHITE;
			else if( height < 212.5f) color = DARKBROWN;
			else color = WHITE;

			// ��� �����͸� ä���. ��ġ�� 4�� ���������� Ȯ������.
			// ��ġ�� ����Ʈ�� �־����� DWORD�� 4����Ʈ�� �̷���� �ִ�.
			pImageData[i * LockedRect.Pitch / 4 + j] = (D3DXCOLOR)color;

		}
	}

	m_pGrassTexture->UnlockRect(0);

	// ������ ������ �����Ѵ�.

	if(!LightTerrain(_pLightDir))
	{
		MessageBox(0, L"LightTerrain() - FAILED",0, 0);
		return FALSE;
	}

	// �Ӹ��� ä���.
	hr = D3DXFilterTexture(
		m_pGrassTexture, 	// �Ӹ� ������ ä�� �ؽ���
		0,				// ����Ʈ �ȷ�Ʈ
		0,				// ���� ������ �������� �ֻ��� ������ �̿��Ѵ�.
		D3DX_DEFAULT);	// ����Ʈ ����
	
	if(FAILED(hr))
	{
		MessageBox(0, L"D3DXFilterTexture() - FAILED", 0, 0);
		return FALSE;
	}

	return TRUE;
}

FLOAT LmTerrain::CalcShade( INT _iTileRow, INT _iTileCol, D3DXVECTOR3* pLightDir )
{
	// �簢���� �� ���ؽ� ���̸� ��´�.
	FLOAT heightA = (FLOAT)GetHeightEntry(_iTileRow,_iTileCol);
	FLOAT heightB = (FLOAT)GetHeightEntry(_iTileRow,_iTileCol+1); 
	FLOAT heightC = (FLOAT)GetHeightEntry(_iTileRow+1 ,_iTileCol);

	// �簢���� �� ���͸� �����Ѵ�.
	D3DXVECTOR3 u((FLOAT)m_iTileSize, heightB - heightA, 0.0f);
	D3DXVECTOR3 v(0.0f, (FLOAT)(heightC - heightA), (FLOAT)(-m_iTileSize));

	// �簢���� �� ���� ������ �̿��� ������ ã�Ƴ���.
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

	// �䱸�Ǵ� ������ 32��Ʈ �ؽ��ķ� ä�쵵���Ѵ�.
	if( textureDesc.Format != D3DFMT_X8R8G8B8 )
		return FALSE;

	D3DLOCKED_RECT lockedRect;
	hr = m_pGrassTexture->LockRect(
		0,          // �ֻ��� �Ӹ� ü���� ��ٴ�.
		&lockedRect,// ��� ������
		0,          // �ؽ��� �̹��� ��ü�� ��ٴ�.
		0);         // �÷���

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
	// ������ �������� �������� �̵��ϴ� ��ȯ����
	// xz - ��� �󿡼� �̵��Ѵ�.
	_x = ((FLOAT)m_iWidth / 2.0f) + _x;
	_z = ((FLOAT)m_iDepth / 2.0f) - _z;

	// �������� 1�� ����� ��ȯ���� ��ȯ�� ������ �����.
	// m_iTileSize * 1 / m_iTileSize = 1�̹Ƿ�
	_x /= (FLOAT)m_iTileSize;
	_z /= (FLOAT)m_iTileSize;

	// ���� ���� ��� ���� �˾Ƴ� �� �ִ�.
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

	// �� ��� ���ؽ��� �������� �̵�
	FLOAT dx = _x - col;
	FLOAT dz = _z - row;

	FLOAT height = 0.0f;

	if(dz < 1.0f - dx) // ���� �ﰢ�� ABC�̸�
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

		// ��ġ�� ���� ���� �ϱ� ���� �ణ ����.
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
