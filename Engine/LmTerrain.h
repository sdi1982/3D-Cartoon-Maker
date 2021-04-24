//
//
//  @ Project : 3D ī�� ����Ŀ
//  @ File Name : LmScene.h
//  @ Date : 2007-10-23
//  @ Author : �ŵ���
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
		INT _iTileSize, // Ÿ�� ����
		FLOAT _fHeightScale // ���� ����
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
	D3DXMATRIX				m_matWorld;	// ���� ��Ʈ����

	vector<INT> m_vecHeight;	// ���� ����

	INT m_iVertPerRow;	// ���� ���� ����
	INT m_iVertPerCol;	// ���� ���� ����
	INT m_iTileSize;	// Ÿ���� ũ��
	INT m_iTilePerRow;	// ���� Ÿ���� ����
	INT m_iTilePerCol;	// ���� Ÿ���� ����
	INT m_iWidth;		// ����
	INT m_iDepth;		// ����
	INT m_iVertexNum;	// ���ؽ� ����
	INT m_iTriangleNum;	// �ﰢ�� ����
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