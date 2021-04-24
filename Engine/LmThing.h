#pragma once;

// Thing 객체는 LmThingManager::CreateThing()을 통해서만 생성할 수 있다.
class LmThing : public LmModel {
public:
	LmThing(LPCWSTR _pcwsName);
	LmThing(LPCWSTR _pcwsName, LPCWSTR _pcwsXFile);
	virtual ~LmThing();

	LmThing(const LmThing& _rhs);
	LmThing& operator=(const LmThing& _rhs);

	LmThing* Clone(LPCWSTR _pcwsClone);	// Thing 객체의 복사본을 생성해서 포인터를 리턴한다.
	VOID Update(FLOAT _fTimeDelta);
	VOID Render(LPDIRECT3DDEVICE9 _pDevice);	// 물체를 화면에 그린다.
	VOID Rotate();
	BOOL Place( FLOAT _fPosX, FLOAT _fPosZ );

protected:
	BOOL LoadData(LPDIRECT3DDEVICE9 _pDevice); // XFile로 부터 렌더링 데이터를 로드한다.
	LPD3DXMESH m_pMesh;							// 메쉬
	vector<D3DMATERIAL9> m_vecMtrl;				// 재질
	vector<LPDIRECT3DTEXTURE9> m_vecTexture;	// 텍스쳐
};

// LmBackGround 객체는 LmBGManager::CreateBG()를 통해서만 생성할 수 있다.
class LmBackGround : public LmThing {
public:
	LmBackGround(LPCWSTR _pcwsName);
	LmBackGround(LPCWSTR _pcwsName, LPCWSTR _pcwsXFile);
	~LmBackGround();
};
