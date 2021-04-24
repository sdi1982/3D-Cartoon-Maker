//
//
//  @ Project : 3D ī�� ����Ŀ
//  @ File Name : LmModel.h
//  @ Date : 2007-10-11
//  @ Author : �ŵ���
//
//


#if !defined(_LMMODEL_H)
#define _LMMODEL_H

class LmModelManager;

class LmModel {
public:
	LmModel(LPCWSTR _pcwsName); 
	LmModel(LPCWSTR _pcwsName, LPCWSTR _pcwsXFile);
	virtual ~LmModel();

	// ����� ������ ���´�.
	LmModel(const LmModel& _rhs);
	LmModel& operator=(const LmModel& _rhs);
	
	virtual VOID Update(FLOAT _fTimeDelta) = 0;
	virtual VOID Render(LPDIRECT3DDEVICE9 _pDevice) = 0;
	
	LPCWSTR GetName(){ return m_wstrName.c_str(); }
	LPCWSTR GetXFileName(){ return m_wstrXFile.c_str(); }
	FLOAT GetPosX() { return m_fPosX; }
	FLOAT GetPosZ() { return m_fPosZ; }
	virtual BOOL Place(FLOAT _fPosX, FLOAT _fPosZ) = 0;	// ��ü�� ��ġ�� �̵���Ų��.
	virtual VOID Rotate() = 0;								// ��ü�� 90���� ȸ����Ų��.

protected:	
	BOOL LoadData(LPDIRECT3DDEVICE9 _pDevice); // XFile�� ���� ������ �����͸� �ε��Ѵ�.
	VOID SetXFile(LPCWSTR _pcwsXFile);	// X���� �̸��� �����Ѵ�.

	wstring m_wstrXFile;				// �ε��� X���� ��
	wstring m_wstrName;					// ������ �� �̸�
	D3DXMATRIX m_matWorld;				// ���� ��Ʈ����

	D3DXVECTOR3 m_vCenter;				// ��豸 �߽�	// ��ǥ x. y
	FLOAT m_fRadius;					// ��豸 ������

	BOOL m_bClone;						// �ڱⰡ ���纻�ΰ�?

	FLOAT m_fPosX, m_fPosZ;
	FLOAT m_fAngle;
};

#endif  //_LMMODEL_H
