#pragma once
#include "LmAllocateHierarchy.h"

struct D3DXFRAME_DERIVED: public D3DXFRAME
{
	D3DXMATRIXA16        CombinedTransformationMatrix;
};

struct D3DXMESHCONTAINER_DERIVED: public D3DXMESHCONTAINER
{
	LPDIRECT3DTEXTURE9*  ppTextures;       // array of textures, entries are NULL if no texture specified    

	// SkinMesh info             
	LPD3DXMESH           pOrigMesh;
	LPD3DXATTRIBUTERANGE pAttributeTable;
	DWORD                NumAttributeGroups; 
	DWORD                NumInfl;
	LPD3DXBUFFER         pBoneCombinationBuf;
	D3DXMATRIX**         ppBoneMatrixPtrs;
	D3DXMATRIX*          pBoneOffsetMatrices;
	DWORD                NumPaletteEntries;
	bool                 UseSoftwareVP;
	DWORD                iAttributeSW;     // used to denote the split between SW and HW if necessary for non-indexed skinning
};

class LmAnimate
{
public:
	LmAnimate();
	~LmAnimate();
	HRESULT Init(LPDIRECT3DDEVICE9 _pDevice, LPCWSTR _pXFileName);

	VOID Update(FLOAT _fTimeDelta, D3DXMATRIX _matWorld);
	VOID Render(LPDIRECT3DDEVICE9 _pDevice);
	BOOL AdvanceTime(FLOAT _fTime);
	VOID ResetTime();
private:
	VOID UpdateFrameMatrices( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix );
	VOID DrawFrame( IDirect3DDevice9 *pd3dDevice, LPD3DXFRAME pFrame );
	VOID DrawMeshContainer( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase );

	HRESULT SetupBoneMatrixPointers( LPD3DXFRAME pFrame );
	HRESULT SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pMeshContainerBase );
	VOID ReleaseAttributeTable( LPD3DXFRAME pFrameBase );

	LPD3DXFRAME					m_pFrameRoot;
	ID3DXAnimationController*	m_pAnimController;
	D3DXMATRIXA16*				m_pBoneMatrices;
	UINT						m_NumBoneMatricesMax;
	DWORD						m_dwBehaviorFlags;      // Behavior flags of the 3D device
};
