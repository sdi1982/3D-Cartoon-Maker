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
	D3DXVECTOR3 vPos;	// 위치
	D3DXVECTOR3 vVel;	// 속도
	D3DXVECTOR3 vAcc;	// 가속도
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

	// 초기화
	virtual BOOL Init(LPDIRECT3DDEVICE9 _pDevice, LPCWSTR _pcwsTexFileName);

	virtual BOOL Setup();
	// 파티클 속성을 리셋
	virtual VOID Reset();
	// 파티클의 속성을 리셋
	virtual VOID ResetParticle(LmPartiAttr* _pAttr) = 0;
	// 파티클을 추가
	virtual VOID AddParticle();
	// 파티클을 갱신
	virtual VOID Update(FLOAT _timeDelta) = 0;

	virtual VOID Cleanup();
	virtual VOID PreRender(); // 초기 렌더 상태를 지정한다.
	virtual VOID Render(LPDIRECT3DDEVICE9 _pDevice);
	virtual VOID PostRender(); // 렌더 상태를 복구한다.

	BOOL IsEmpty();	// 현재 파티클이 없다면 TRUE
	BOOL IsDead();	// 파티클이 모두 죽으면 TRUE - 존재하지만 죽은걸로 표시

protected:
	// m_listParticle을 검색하여 죽은 파티클을 제거한다.
	virtual VOID RemoveDeadParticle();
	DWORD	FtoDw(FLOAT _fNum);
	FLOAT	GetRandomFloat(float _fLowBound, FLOAT _fHighBound);
	VOID	GetRandomVector(D3DXVECTOR3* _pvOut, D3DXVECTOR3* _vMin, D3DXVECTOR3* _vMax);

	LPDIRECT3DDEVICE9 m_pDevice;
	wstring m_wstrTexFileName;
	D3DXVECTOR3 m_vOrigin;			// 시스템에서 파티클이 시작되는 곳
	LmBoundBox m_BoundBox;			// 파티클 이동 범위
	FLOAT	m_fEmitRate;			// 파티클 추가 비율. 초 당 파티클 수 
	FLOAT	m_fSize;				// 파티클의 크기
	LPDIRECT3DTEXTURE9 m_pTexture;	
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	list<LmPartiAttr>		m_listParticle;	// 파티클의 속성 리스트
	INT					m_iMaxParticle;	// 최대 파티클 수

	DWORD	m_dwVBSize;		// 버텍스 버퍼의 최대 파티클 수
	DWORD	m_dwVBOffset;
	DWORD	m_dwVBBatchSize;
};
