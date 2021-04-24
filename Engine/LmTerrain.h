//
//
//  @ Project : 3D 카툰 메이커
//  @ File Name : LmScene.h
//  @ Date : 2007-10-23
//  @ Author : 신동인
//
//


#if !defined(_LMTERRAIN_H)
#define _LMTERRAIN_H

#include "LmCartoonMaker.h"

class LmTerrain{
public:
	LmTerrain(
		LPDIRECT3DDEVICE9 _pDevice, LPCWSTR _pcwsFile, 
		INT _iVertPerRow, INT _iVertPerCol, 
		INT _iTileSize, // 타일 간격
		FLOAT _fHeightScale // 높이 배율
		);

	virtual ~LmTerrain();

	BOOL LoadDefaultTexture();
	BOOL CreateTexture(D3DXVECTOR3* _pLightDir);
	FLOAT GetHeight(FLOAT _x, FLOAT _z);
	VOID Render(LPDIRECT3DDEVICE9 _pDevice);
	VOID SetTriLine(BOOL _bTriStrip){ m_bTriStrip = _bTriStrip; }
	VOID SetTexType(UINT _texType){ m_TexType = _texType; }
private:
	BOOL LoadRawFile(LPCWSTR _pcwsFile);
	BOOL CalcVertex(VOID);
	BOOL CalcIndex(VOID);
	BOOL LightTerrain(D3DXVECTOR3* _pLightDir);
	FLOAT CalcShade(INT _iTileRow, INT _iTileCol, D3DXVECTOR3* pLightDir);

	INT  GetHeightEntry(INT _iRow, INT _iCol);
	VOID GetHeightEntry(INT _iRow, INT _iCol, INT _Value);

	FLOAT Lerp(FLOAT _a, FLOAT _b, FLOAT _t);

	LPDIRECT3DDEVICE9       m_pDevice;
	LPDIRECT3DTEXTURE9		m_pGrassTexture;
	LPDIRECT3DTEXTURE9		m_pDesertTexture;
	LPDIRECT3DTEXTURE9		m_pSnowTexture;
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;
	D3DXMATRIX				m_matWorld;	// 월드 매트릭스

	vector<INT> m_vecHeight;	// 높이 정보

	INT m_iVertPerRow;	// 가로 점의 개수
	INT m_iVertPerCol;	// 세로 점의 개수
	INT m_iTileSize;	// 타일의 크기
	INT m_iTilePerRow;	// 가로 타일의 개수
	INT m_iTilePerCol;	// 세로 타일의 개수
	INT m_iWidth;		// 넓이
	INT m_iDepth;		// 깊이
	INT m_iVertexNum;	// 버텍스 개수
	INT m_iTriangleNum;	// 삼각형 개수
	FLOAT m_fHeightScale;
	BOOL m_bTriStrip;
	UINT m_TexType;



	struct LmTVertex
	{
		LmTVertex(){}
		LmTVertex(float x, float y, float z, float u, float v){
			_x = x; _y = y; _z = z; _u = u; _v = v;
		}
		float _x, _y, _z;
		float _u, _v;

		static const DWORD FVF;
	};
};

class LmTerrainManager{
public:
	virtual ~LmTerrainManager();
	static LmTerrainManager* GetInstance();

	BOOL Init(LPDIRECT3DDEVICE9 _pDevice);
	VOID Render(LPDIRECT3DDEVICE9 _pDevice);
	VOID SetTerrainType(UINT _TexType);
private:
	LmTerrainManager();

	LmTerrain* m_pTerrain;
};

#define TheTerrain LmTerrainManager::GetInstance()
#endif // _LMTERRAIN_H