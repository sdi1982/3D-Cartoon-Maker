#include "LmParticleSystem.h"

class LmSnowParticle : public LmParticleSystem
{
public:
	LmSnowParticle(LmBoundBox* _pBoundBox, INT _numParticle);

	VOID ResetParticle(LmPartiAttr* attr);
	VOID Update(FLOAT _fTimeDelta);
};

class LmFireworkParticle : public LmParticleSystem
{
public:
	LmFireworkParticle(D3DXVECTOR3* _vOrigin, INT _iNumParticles);
	void ResetParticle(LmPartiAttr* attr);
	void Update(FLOAT _fTimeDelta);
	void PreRender();
	void PostRender();
};

class LmEffectManager
{
public:
	virtual ~LmEffectManager();
	static LmEffectManager* GetInstance();

	BOOL Init(LPDIRECT3DDEVICE9 _pDevice);
	VOID Render(LPDIRECT3DDEVICE9 _pDevice);
	VOID SetEffectNo(UINT _uEffectNo){ m_uEffectNo = _uEffectNo; }
	VOID Cleanup();
	BOOL Setup();
	VOID SetSnowTime(FLOAT _fTimeDelta);
	VOID SetFireworkTime(FLOAT _fTimeDelta);

	VOID ShowSnow(BOOL _bShowSnow){m_bShowSnow = _bShowSnow;}
	VOID ShowFirework(BOOL _bShowFirework){m_bShowFirework = _bShowFirework;}
private:
	LmEffectManager();

	LmSnowParticle* m_pSnow;
	LmFireworkParticle* m_pExp;
	UINT m_uEffectNo;
private:
	BOOL m_bShowSnow;
	BOOL m_bShowFirework;
	LPDIRECT3DDEVICE9 m_pDevice;

	wstring m_wstrSnow;
	wstring m_wstrFirework;
};
#define TheEffectManager LmEffectManager::GetInstance()
