//
//
//  @ Project : 3D 카툰 메이커
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
	// 자기가 오리지날이면 데이터를 지운다.
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
	if(!m_pMesh) // 메쉬가 로드되지 않았다면 X파일을 읽어서 로드한다.
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

//  현재 LmThing과 동일한 정보를 가진 복사본을 리턴한다.
LmThing* LmThing::Clone(LPCWSTR _pcwsClone) 
{
	LmThing* pCloneThing = new LmThing(_pcwsClone); // 객체를 생성
	
	if(pCloneThing == NULL)
		return NULL;

	// 원본의 정보를 그대로 복사해줘야 한다. =연산자 오보라이드로 구현
	*pCloneThing = *this;

	return pCloneThing;
}

LmThing& LmThing::operator=( const LmThing& _rhs )
{
	// 자기 자신이랑 같다면 그냥 리턴
	if (this != &_rhs)
	{
		m_wstrXFile = _rhs.m_wstrXFile;
		m_matWorld = _rhs.m_matWorld;

		m_pMesh = _rhs.m_pMesh;	// 메쉬를 공유한다.

		// 재질을 복사한다. 
		// - 재질도 공유하게 할려면 동적할당을 하고 포인터를 공유한다.<미구현>
		m_vecMtrl = _rhs.m_vecMtrl;

		// 텍스쳐 포인터를 복사한다. - 텍스쳐를 공유한다.
		m_vecTexture = _rhs.m_vecTexture;

		m_vCenter = _rhs.m_vCenter;	// 경계구 중심
		m_fRadius = _rhs.m_fRadius;	// 경계구 반지름

		m_bClone = TRUE;
	}

	return *this;
}

// XFile로 부터 렌더링 데이터를 로드한다. 
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

	// 재질을 추출하고 텍스쳐를 읽어들인다.
	if(mtrlBuffer != NULL && numMtrls != 0)
	{
		D3DXMATERIAL* mtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();

		for(int i = 0; i < (int)numMtrls; i++)
		{
			// Mat3D 속성은 로드될 때 ambient 값을 가지지 않으므로 지금 설정한다.
			mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;

			// i번째 재질을 저장한다.
			m_vecMtrl.push_back(mtrls[i].MatD3D);

			// i번째 재질에 연결된 텍스쳐가 있는지 확인한다.
			WCHAR strPath[MAX_PATH];
			WCHAR wszTexBuf[MAX_PATH];
			if(mtrls[i].pTextureFilename != NULL)
			{
				// 텍스쳐를 읽어들인다.
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

				// 읽어들인 텍스쳐를 저장한다.
				m_vecTexture.push_back(texture);
			}
			else
			{
				// i번째 텍스쳐가 없다.
				m_vecTexture.push_back(NULL);
			}
		}
	}

	SAFE_RELEASE(mtrlBuffer);

	// 버텍스 버퍼를 잠그고, 경계구를 생성한다.
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

	// 버텍스 법선을 생성한다.

	LPD3DXMESH   pMeshTemp;	// 복사에 사용할 임시 메쉬

	// 메쉬에 법선이 있는지 확인한다.
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

	// 메쉬 최적화
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
	// 타일 크기만큼의 배율을 곱해줘야 한다.

	m_fPosX = _fPosX;
	m_fPosZ = _fPosZ;
	return TRUE;
}

VOID LmThing::Rotate()
{
	// 매트릭스를 회전시킨다
	// 중점으로 이동시켜서 회전한 다음 원래 위치로 보낸다.

	m_fAngle += (D3DX_PI / 2.0f);
}
