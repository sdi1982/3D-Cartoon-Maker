#pragma once
#include "LmScene.h"
#include "LmDefine.h"


struct LmParticle
{
	D3DXVECTOR3 vPos;
	D3DCOLOR	dxColor;
	FLOAT		fSize;
	static const DWORD FVF;
};


struct LmPartiAttr
{
	D3DXVECTOR3 vPos;	// ��ġ
	D3DXVECTOR3 vVel;	// �ӵ�
	D3DXVECTOR3 vAcc;	// ���ӵ�
	FLOAT	fLifeTime;
	FLOAT	fAge;
	D3DCOLOR	dxColor;
	D3DCOLOR	dxColorFade;
	BOOL	bAlive;		
};


class LmParticleSystem
{
public:
	LmParticleSystem();
	virtual ~LmParticleSystem();

	// �ʱ�ȭ
	virtual BOOL Init(LPDIRECT3DDEVICE9 _pDevice, LPCWSTR _pcwsTexFileName);

	virtual BOOL Setup();
	// ��ƼŬ �Ӽ��� ����
	virtual VOID Reset();
	// ��ƼŬ�� �Ӽ��� ����
	virtual VOID ResetParticle(LmPartiAttr* _pAttr) = 0;
	// ��ƼŬ�� �߰�
	virtual VOID AddParticle();
	// ��ƼŬ�� ����
	virtual VOID Update(FLOAT _timeDelta) = 0;

	virtual VOID Cleanup();
	virtual VOID PreRender(); // �ʱ� ���� ���¸� �����Ѵ�.
	virtual VOID Render(LPDIRECT3DDEVICE9 _pDevice);
	virtual VOID PostRender(); // ���� ���¸� �����Ѵ�.

	BOOL IsEmpty();	// ���� ��ƼŬ�� ���ٸ� TRUE
	BOOL IsDead();	// ��ƼŬ�� ��� ������ TRUE - ���������� �����ɷ� ǥ��

protected:
	// m_listParticle�� �˻��Ͽ� ���� ��ƼŬ�� �����Ѵ�.
	virtual VOID RemoveDeadParticle();
	DWORD	FtoDw(FLOAT _fNum);
	FLOAT	GetRandomFloat(float _fLowBound, FLOAT _fHighBound);
	VOID	GetRandomVector(D3DXVECTOR3* _pvOut, D3DXVECTOR3* _vMin, D3DXVECTOR3* _vMax);

	LPDIRECT3DDEVICE9 m_pDevice;
	wstring m_wstrTexFileName;
	D3DXVECTOR3 m_vOrigin;			// �ý��ۿ��� ��ƼŬ�� ���۵Ǵ� ��
	LmBoundBox m_BoundBox;			// ��ƼŬ �̵� ����
	FLOAT	m_fEmitRate;			// ��ƼŬ �߰� ����. �� �� ��ƼŬ �� 
	FLOAT	m_fSize;				// ��ƼŬ�� ũ��
	LPDIRECT3DTEXTURE9 m_pTexture;	
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	list<LmPartiAttr>		m_listParticle;	// ��ƼŬ�� �Ӽ� ����Ʈ
	INT					m_iMaxParticle;	// �ִ� ��ƼŬ ��

	DWORD	m_dwVBSize;		// ���ؽ� ������ �ִ� ��ƼŬ ��
	DWORD	m_dwVBOffset;
	DWORD	m_dwVBBatchSize;
};
