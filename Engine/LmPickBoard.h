
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

	// ���� �����õ� ���� ĥ�Ѵ�.
	VOID SetCurPlane(FLOAT _fX, FLOAT _fY);

	// �ٵ����� �׸���.
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
	D3DXMATRIX m_matWorld;		// ���� ��Ʈ����

	INT m_iVertPerRow;	// ���� ���� ����
	INT m_iVertPerCol;	// ���� ���� ����
	INT m_iTileSize;	// Ÿ���� ũ��
	INT m_iTilePerRow;	// ���� Ÿ���� ����
	INT m_iTilePerCol;	// ���� Ÿ���� ����
	INT m_iWidth;		// ����
	INT m_iDepth;		// ����
	INT m_iVertexNum;	// ���ؽ� ����
	INT m_iTriangleNum;	// �ﰢ�� ����

	BOOL m_bShow;
	FLOAT m_fFloorX, m_fFloorZ;
	BOOL m_bInter;

	LmThing* m_pDragThing;	// ���� �巡�� ���� ��ü
	LmChar* m_pDragChar;	// ���� �巡�� ���� ĳ����

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