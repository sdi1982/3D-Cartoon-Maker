//
//
//  @ Project : 3D ī�� ����Ŀ
//  @ File Name : LmThing.cpp
//  @ Date : 2007-10-11
//  @ Author : 
//
//

#include "dxstdafx.h"
#include "LmThing.h"
#include "LmModelManager.h"


LmThing::LmThing(LPCWSTR _pcwsName) : LmModel(_pcwsName)
{
	m_fPosX = m_fPosZ = 0;
	m_pMesh = NULL;
	m_vecMtrl.clear();
	m_vecTexture.clear();
}

LmThing::LmThing(LPCWSTR _pcwsName, LPCWSTR _pcwsXFile) : LmModel(_pcwsName, _pcwsXFile)
{
	m_fPosX = m_fPosZ = 0;

	m_pMesh = NULL;
	m_vecMtrl.clear();
	m_vecTexture.clear();
}

LmThing::~LmThing()
{
	// �ڱⰡ ���������̸� �����͸� �����.
	if(!m_bClone)
	{
		for(int i = 0; i < (int)m_vecTexture.size(); i++)
			SAFE_RELEASE(m_vecTexture[i]);
		SAFE_RELEASE( m_pMesh );
	}

	m_vecTexture.clear();
	m_vecMtrl.clear();
}

VOID LmThing::Update( FLOAT _fTimeDelta )
{	
	D3DXMATRIX matRot;
	D3DXMatrixIdentity(&matRot);
	D3DXMatrixRotationY(&matRot, m_fAngle);

	D3DXMATRIX matTran;
	D3DXMatrixIdentity(&matTran);
	D3DXMatrixTranslation( &matTran, m_fPosX, 0.0f, m_fPosZ );

	D3DXMatrixMultiply(&m_matWorld, &matRot, &matTran);
}

VOID LmThing::Render(LPDIRECT3DDEVICE9 _pDevice)
{
	if(!m_pMesh) // �޽��� �ε���� �ʾҴٸ� X������ �о �ε��Ѵ�.
	{
		LoadData(_pDevice);
	}


	_pDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

	for(int i = 0; i < (int)m_vecMtrl.size(); i++)
	{
		_pDevice->SetMaterial(&m_vecMtrl[i]);
		_pDevice->SetTexture(0,m_vecTexture[i]);
		m_pMesh->DrawSubset(i);
	}
}

//  ���� LmThing�� ������ ������ ���� ���纻�� �����Ѵ�.
LmThing* LmThing::Clone(LPCWSTR _pcwsClone) 
{
	LmThing* pCloneThing = new LmThing(_pcwsClone); // ��ü�� ����
	
	if(pCloneThing == NULL)
		return NULL;

	// ������ ������ �״�� ��������� �Ѵ�. =������ �������̵�� ����
	*pCloneThing = *this;

	return pCloneThing;
}

LmThing& LmThing::operator=( const LmThing& _rhs )
{
	// �ڱ� �ڽ��̶� ���ٸ� �׳� ����
	if (this != &_rhs)
	{
		m_wstrXFile = _rhs.m_wstrXFile;
		m_matWorld = _rhs.m_matWorld;

		m_pMesh = _rhs.m_pMesh;	// �޽��� �����Ѵ�.

		// ������ �����Ѵ�. 
		// - ������ �����ϰ� �ҷ��� �����Ҵ��� �ϰ� �����͸� �����Ѵ�.<�̱���>
		m_vecMtrl = _rhs.m_vecMtrl;

		// �ؽ��� �����͸� �����Ѵ�. - �ؽ��ĸ� �����Ѵ�.
		m_vecTexture = _rhs.m_vecTexture;

		m_vCenter = _rhs.m_vCenter;	// ��豸 �߽�
		m_fRadius = _rhs.m_fRadius;	// ��豸 ������

		m_bClone = TRUE;
	}

	return *this;
}

// XFile�� ���� ������ �����͸� �ε��Ѵ�. 
BOOL LmThing::LoadData(LPDIRECT3DDEVICE9 _pDevice)
{
	LPD3DXBUFFER adjBuffer = NULL;
	LPD3DXBUFFER mtrlBuffer = NULL;
	DWORD numMtrls = 0;


	wstring wstrXFile = L"Data\\";
	wstrXFile += m_wstrXFile + L".x";

	WCHAR strPath[MAX_PATH];
	HRESULT hr = DXUTFindDXSDKMediaFileCch( strPath, MAX_PATH, wstrXFile.c_str() );
	if( FAILED(hr) )
	{
		return DXTRACE_ERR( L"DXUTFindDXSDKMediaFileCch", hr );
	}

	hr = D3DXLoadMeshFromX(
		strPath,
		D3DXMESH_MANAGED,
		_pDevice,
		&adjBuffer,
		&mtrlBuffer,
		0,
		&numMtrls,
		&m_pMesh
		);

	if(FAILED(hr))
	{
		MessageBox(0,L"D3DXLoadMeshFromX() - FAILED", 0, 0);
		return FALSE;
	} 

	// ������ �����ϰ� �ؽ��ĸ� �о���δ�.
	if(mtrlBuffer != NULL && numMtrls != 0)
	{
		D3DXMATERIAL* mtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();

		for(int i = 0; i < (int)numMtrls; i++)
		{
			// Mat3D �Ӽ��� �ε�� �� ambient ���� ������ �����Ƿ� ���� �����Ѵ�.
			mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;

			// i��° ������ �����Ѵ�.
			m_vecMtrl.push_back(mtrls[i].MatD3D);

			// i��° ������ ����� �ؽ��İ� �ִ��� Ȯ���Ѵ�.
			WCHAR strPath[MAX_PATH];
			WCHAR wszTexBuf[MAX_PATH];
			if(mtrls[i].pTextureFilename != NULL)
			{
				// �ؽ��ĸ� �о���δ�.
				MultiByteToWideChar( CP_ACP, 0, mtrls[i].pTextureFilename, -1, wszTexBuf, MAX_PATH );
				wszTexBuf[MAX_PATH - 1] = L'\0';
				wstring wstrTex = L"Data\\"; 
				wstrTex += wszTexBuf;
				DXUTFindDXSDKMediaFileCch( strPath, MAX_PATH, wstrTex.c_str() );

				LPDIRECT3DTEXTURE9 texture = NULL;
				D3DXCreateTextureFromFile(
					_pDevice,
					strPath,
					&texture
					);

				// �о���� �ؽ��ĸ� �����Ѵ�.
				m_vecTexture.push_back(texture);
			}
			else
			{
				// i��° �ؽ��İ� ����.
				m_vecTexture.push_back(NULL);
			}
		}
	}

	SAFE_RELEASE(mtrlBuffer);

	// ���ؽ� ���۸� ��װ�, ��豸�� �����Ѵ�.
	LPDIRECT3DVERTEXBUFFER9 pMeshVB = NULL;
	void*        pVertices;

	hr = m_pMesh->GetVertexBuffer( &pMeshVB );
	if( SUCCEEDED(hr) )
	{
		hr = pMeshVB->Lock( 0, 0, &pVertices, D3DLOCK_NOSYSLOCK );
		if( SUCCEEDED(hr) )
		{
			D3DXComputeBoundingSphere( (D3DXVECTOR3*)pVertices, m_pMesh->GetNumVertices(),
				D3DXGetFVFVertexSize( m_pMesh->GetFVF() ),
				&m_vCenter, &m_fRadius );
			pMeshVB->Unlock();
		}
		pMeshVB->Release();
	} 
	else
	{
		SAFE_RELEASE( m_pMesh );

		MessageBox(0,L"GetVertexBuffer() - FAILED", 0, 0);
		return FALSE;
	}

	// ���ؽ� ������ �����Ѵ�.

	LPD3DXMESH   pMeshTemp;	// ���翡 ����� �ӽ� �޽�

	// �޽��� ������ �ִ��� Ȯ���Ѵ�.
	bool bNormalsInFile = ( m_pMesh->GetFVF() & D3DFVF_NORMAL ) != 0;

	if( !bNormalsInFile )
	{
		hr = m_pMesh->CloneMeshFVF( m_pMesh->GetOptions(), m_pMesh->GetFVF() | D3DFVF_NORMAL, 
			_pDevice, &pMeshTemp );
		if (FAILED(hr))
		{
			SAFE_RELEASE( m_pMesh );

			MessageBox(0,L"CloneMeshFVF() - FAILED", 0, 0);
			return FALSE;
		}

		m_pMesh->Release();
		m_pMesh = pMeshTemp;

		D3DXComputeNormals( m_pMesh, NULL );
	}

	// �޽� ����ȭ
	hr = m_pMesh->OptimizeInplace(		
		D3DXMESHOPT_ATTRSORT |
		D3DXMESHOPT_COMPACT  |
		D3DXMESHOPT_VERTEXCACHE,
		(DWORD*)adjBuffer->GetBufferPointer(),
		0, 0, 0);

	SAFE_RELEASE(adjBuffer);

	if(FAILED(hr))
	{
		MessageBox(0, L"OptimizeInplace() - FAILED", 0, 0);
		return FALSE;
	}

	return TRUE;
}


BOOL LmThing::Place( FLOAT _fPosX, FLOAT _fPosZ )
{
	// Ÿ�� ũ�⸸ŭ�� ������ ������� �Ѵ�.

	m_fPosX = _fPosX;
	m_fPosZ = _fPosZ;
	return TRUE;
}

VOID LmThing::Rotate()
{
	// ��Ʈ������ ȸ����Ų��
	// �������� �̵����Ѽ� ȸ���� ���� ���� ��ġ�� ������.

	m_fAngle += (D3DX_PI / 2.0f);
}
