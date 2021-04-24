#include "dxstdafx.h"
#include "LmAnimate.h"

//////////////////////////////////////////////////////////////////////////
LmAnimate::LmAnimate()
{
	m_pFrameRoot = NULL;
	m_pAnimController = NULL;
	m_pBoneMatrices = NULL;
	m_NumBoneMatricesMax = 0;
}

LmAnimate::~LmAnimate()
{
	ReleaseAttributeTable( m_pFrameRoot );
	delete[] m_pBoneMatrices;

	LmAllocateHierarchy Alloc;
	D3DXFrameDestroy( m_pFrameRoot, &Alloc );
	SAFE_RELEASE( m_pAnimController );
}

HRESULT LmAnimate::Init( LPDIRECT3DDEVICE9 _pDevice, LPCWSTR _pXFileName )
{	
	wstring wstrXFile = L"Data\\";
	wstrXFile += _pXFileName;
	WCHAR strPath[MAX_PATH];
	HRESULT hr = DXUTFindDXSDKMediaFileCch( strPath, MAX_PATH, wstrXFile.c_str() );
	if( FAILED(hr) )
	{
		return DXTRACE_ERR( L"DXUTFindDXSDKMediaFileCch", hr );
	}

	LmAllocateHierarchy Alloc;
	V_RETURN( D3DXLoadMeshHierarchyFromX( strPath, D3DXMESH_MANAGED, _pDevice,
		&Alloc, NULL, &m_pFrameRoot, &m_pAnimController ) );
	
	UINT num = m_pAnimController->GetNumAnimationSets();

	LPD3DXANIMATIONSET pAnimSet;
	m_pAnimController->GetAnimationSetByName("Walk",&pAnimSet);
	
	
	
	V_RETURN( SetupBoneMatrixPointers( m_pFrameRoot ) );

	// Obtain the behavior flags
	D3DDEVICE_CREATION_PARAMETERS cp;
	_pDevice->GetCreationParameters( &cp );
	m_dwBehaviorFlags = cp.BehaviorFlags;

	return S_OK;
}

VOID LmAnimate::Update( FLOAT _fTimeDelta, D3DXMATRIX _matWorld )
{
	UpdateFrameMatrices( m_pFrameRoot, &_matWorld );
}

VOID LmAnimate::ReleaseAttributeTable( LPD3DXFRAME pFrameBase )
{
	D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
	D3DXMESHCONTAINER_DERIVED *pMeshContainer;

	pMeshContainer = (D3DXMESHCONTAINER_DERIVED *)pFrame->pMeshContainer;

	while( pMeshContainer != NULL )
	{
		delete[] pMeshContainer->pAttributeTable;

		pMeshContainer = (D3DXMESHCONTAINER_DERIVED *)pMeshContainer->pNextMeshContainer;
	}

	if (pFrame->pFrameSibling != NULL)
	{
		ReleaseAttributeTable(pFrame->pFrameSibling);
	}

	if (pFrame->pFrameFirstChild != NULL)
	{
		ReleaseAttributeTable(pFrame->pFrameFirstChild);
	}
}

VOID LmAnimate::UpdateFrameMatrices( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix )
{
	D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;

	if (pParentMatrix != NULL)
		D3DXMatrixMultiply(&pFrame->CombinedTransformationMatrix, &pFrame->TransformationMatrix, pParentMatrix);
	else
		pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;

	if (pFrame->pFrameSibling != NULL)
	{
		UpdateFrameMatrices(pFrame->pFrameSibling, pParentMatrix);
	}

	if (pFrame->pFrameFirstChild != NULL)
	{
		UpdateFrameMatrices(pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix);
	}
}

HRESULT LmAnimate::SetupBoneMatrixPointers( LPD3DXFRAME pFrame )
{
	HRESULT hr;

	if (pFrame->pMeshContainer != NULL)
	{
		hr = SetupBoneMatrixPointersOnMesh(pFrame->pMeshContainer);
		if (FAILED(hr))
			return hr;
	}

	if (pFrame->pFrameSibling != NULL)
	{
		hr = SetupBoneMatrixPointers(pFrame->pFrameSibling);
		if (FAILED(hr))
			return hr;
	}

	if (pFrame->pFrameFirstChild != NULL)
	{
		hr = SetupBoneMatrixPointers(pFrame->pFrameFirstChild);
		if (FAILED(hr))
			return hr;
	}

	return S_OK;
}

HRESULT LmAnimate::SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pMeshContainerBase )
{
	UINT iBone, cBones;
	D3DXFRAME_DERIVED *pFrame;

	D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

	// if there is a skinmesh, then setup the bone matrices
	if (pMeshContainer->pSkinInfo != NULL)
	{
		cBones = pMeshContainer->pSkinInfo->GetNumBones();

		pMeshContainer->ppBoneMatrixPtrs = new D3DXMATRIX*[cBones];
		if (pMeshContainer->ppBoneMatrixPtrs == NULL)
			return E_OUTOFMEMORY;

		for (iBone = 0; iBone < cBones; iBone++)
		{
			pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind( m_pFrameRoot, pMeshContainer->pSkinInfo->GetBoneName(iBone) );
			if (pFrame == NULL)
				return E_FAIL;

			pMeshContainer->ppBoneMatrixPtrs[iBone] = &pFrame->CombinedTransformationMatrix;
		}
	}

	return S_OK;
}

VOID LmAnimate::Render( LPDIRECT3DDEVICE9 _pDevice )
{
	DrawFrame( _pDevice, m_pFrameRoot );
}

VOID LmAnimate::DrawFrame( IDirect3DDevice9 *pd3dDevice, LPD3DXFRAME pFrame )
{
	LPD3DXMESHCONTAINER pMeshContainer;

	pMeshContainer = pFrame->pMeshContainer;
	while (pMeshContainer != NULL)
	{
		DrawMeshContainer( pd3dDevice, pMeshContainer, pFrame );

		pMeshContainer = pMeshContainer->pNextMeshContainer;
	}

	if (pFrame->pFrameSibling != NULL)
	{
		DrawFrame( pd3dDevice, pFrame->pFrameSibling);
	}

	if(pFrame->pFrameFirstChild != NULL)
	{
		DrawFrame( pd3dDevice, pFrame->pFrameFirstChild );
	}
}

VOID LmAnimate::DrawMeshContainer( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase )
{
	HRESULT hr;
	D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
	D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
	UINT iMaterial;
	D3DXMATRIXA16 matTemp;
	D3DCAPS9 d3dCaps;
	pd3dDevice->GetDeviceCaps( &d3dCaps );

	V( pd3dDevice->SetTransform(D3DTS_WORLD, &pFrame->CombinedTransformationMatrix) );

	for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
	{
		V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[iMaterial].MatD3D ) );
		V( pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[iMaterial] ) );
		V( pMeshContainer->MeshData.pMesh->DrawSubset(iMaterial) );
	}
}

VOID LmAnimate::ResetTime()
{
	if( m_pAnimController == NULL )
		return;

	m_pAnimController->SetTrackPosition(0,0);
}

BOOL LmAnimate::AdvanceTime( FLOAT _fTime )
{
	HRESULT hr = FALSE;

	if( m_pAnimController == NULL )
		return FALSE;

	hr = m_pAnimController->AdvanceTime(_fTime, NULL);
	if(FAILED(hr))
		return FALSE;

	return TRUE;
}