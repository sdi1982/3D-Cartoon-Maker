
#if !defined(_LMPICKBOARD_H)
#define _LMPICKBOARD_H

#define FLOOR_DEPTH 50
#define FLOOR_WIDTH 50


class LmModel;

class LmPickBoard{

public:
	virtual ~LmPickBoard();
	static LmPickBoard* GetInstance();

	BOOL Init(LPDIRECT3DDEVICE9 _pDevice, INT _iVertPerRow, INT _iVertPerCol, INT _iTileSize);

	// 현재 포인팅된 색을 칠한다.
	VOID SetCurPlane(FLOAT _fX, FLOAT _fY);

	// 바둑판을 그린다.
	VOID Update(FLOAT _fTimeDelta);
	VOID Render(LPDIRECT3DDEVICE9 _pDevice);

	FLOAT GetTileX();
	FLOAT GetTileZ();

	INT GetWidth(){ return m_iWidth; }
	INT GetDepth(){ return m_iDepth; }
	INT GetTileSize(){ return m_iTileSize; }
	VOID Show(BOOL _bShow);
	BOOL IsShow(){return m_bShow; }
	VOID SetFloorPt( FLOAT _fX, FLOAT _fY );
	VOID SetInter(BOOL _bInter){ m_bInter = _bInter; }
	BOOL IsInter(){ return m_bInter; }

	VOID SetDragThing(LmThing* _pThing){ m_pDragThing = _pThing; }
	LmThing* GetDragThing(){ return m_pDragThing; }

	VOID SetDragChar(LmChar* _pChar){ m_pDragChar = _pChar; }
	LmChar* GetDragChar(){ return m_pDragChar; }

private:
	LmPickBoard();

	BOOL CalcVertex( LPDIRECT3DDEVICE9 _pDevice );
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DVERTEXBUFFER9 m_pTileVB;
	D3DXMATRIX m_matWorld;		// 월드 매트릭스

	INT m_iVertPerRow;	// 가로 점의 개수
	INT m_iVertPerCol;	// 세로 점의 개수
	INT m_iTileSize;	// 타일의 크기
	INT m_iTilePerRow;	// 가로 타일의 개수
	INT m_iTilePerCol;	// 세로 타일의 개수
	INT m_iWidth;		// 넓이
	INT m_iDepth;		// 깊이
	INT m_iVertexNum;	// 버텍스 개수
	INT m_iTriangleNum;	// 삼각형 개수

	BOOL m_bShow;
	FLOAT m_fFloorX, m_fFloorZ;
	BOOL m_bInter;

	LmThing* m_pDragThing;	// 현재 드래그 중인 물체
	LmChar* m_pDragChar;	// 현재 드래그 중인 캐릭터

	struct LmBVertex
	{
		LmBVertex(){}
		LmBVertex(FLOAT _x, FLOAT _y, FLOAT _z, D3DCOLOR _dxColor){
			fX = _x; fY = _y; fZ = _z; dxColor = _dxColor;
		}
		FLOAT fX, fY, fZ;
		D3DCOLOR dxColor;

		static CONST DWORD FVF;
	};
};

#define ThePickBoard LmPickBoard::GetInstance()

#endif  //_LMPICKBOARD_H