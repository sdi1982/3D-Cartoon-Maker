#pragma once;

// Thing ��ü�� LmThingManager::CreateThing()�� ���ؼ��� ������ �� �ִ�.
class LmThing : public LmModel {
public:
	LmThing(LPCWSTR _pcwsName);
	LmThing(LPCWSTR _pcwsName, LPCWSTR _pcwsXFile);
	virtual ~LmThing();

	LmThing(const LmThing& _rhs);
	LmThing& operator=(const LmThing& _rhs);

	LmThing* Clone(LPCWSTR _pcwsClone);	// Thing ��ü�� ���纻�� �����ؼ� �����͸� �����Ѵ�.
	VOID Update(FLOAT _fTimeDelta);
	VOID Render(LPDIRECT3DDEVICE9 _pDevice);	// ��ü�� ȭ�鿡 �׸���.
	VOID Rotate();
	BOOL Place( FLOAT _fPosX, FLOAT _fPosZ );

protected:
	BOOL LoadData(LPDIRECT3DDEVICE9 _pDevice); // XFile�� ���� ������ �����͸� �ε��Ѵ�.
	LPD3DXMESH m_pMesh;							// �޽�
	vector<D3DMATERIAL9> m_vecMtrl;				// ����
	vector<LPDIRECT3DTEXTURE9> m_vecTexture;	// �ؽ���
};

// LmBackGround ��ü�� LmBGManager::CreateBG()�� ���ؼ��� ������ �� �ִ�.
class LmBackGround : public LmThing {
public:
	LmBackGround(LPCWSTR _pcwsName);
	LmBackGround(LPCWSTR _pcwsName, LPCWSTR _pcwsXFile);
	~LmBackGround();
};
