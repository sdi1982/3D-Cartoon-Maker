//
//
//  @ Project : 3D 카툰 메이커
//  @ File Name : LmCamera.h
//  @ Date : 2007-10-07
//  @ Author : 신동인
//
//


#if !defined(_LMCAMERA_H)
#define _LMCAMERA_H


//
//
//  @ class : 카메라 클래스
//  @ brief : D3D화면의 시점을 보여주는 카메라를 제어하는 클래스
//  @ Date : 2007-10-12
//  @ Author : 신동인
//
//

class LmCamera {
public:
	~LmCamera();
	static LmCamera* GetInstance();

	VOID Setup(UINT _uWidth, UINT _uHeight);
	VOID Render(LPDIRECT3DDEVICE9 _pDevice);

	FLOAT GetDist() CONST;
	VOID AddDist(FLOAT _fD);

	CONST D3DXVECTOR3* GetEyePt() CONST;
	VOID SetEyePt(FLOAT _fX, FLOAT _fY, FLOAT _fZ);
	VOID ResetEyePt();
	
	CONST D3DXMATRIX* GetWorldMatrix() CONST;
	CONST D3DXMATRIX* GetProjMatrix() CONST;
	CONST D3DXMATRIX* GetViewMatrix() CONST;

	VOID Update(FLOAT _fTimeDelta);
	BOOL HandleMessages(HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );

	VOID EnableMouse(BOOL _bEnableMouse){ m_bEnableMouse = _bEnableMouse; }


	CONST UINT GetScreenWidth(){return m_uScreenWidth;}
	CONST UINT GetScreenHeight(){return m_uScreenHeight;}
private:
	LmCamera();
	CModelViewerCamera* m_pCamera;

	BOOL m_bEnableMouse;

	UINT m_uScreenWidth, m_uScreenHeight;
};

#define TheCamera LmCamera::GetInstance()
#endif  //_LMCAMERA_H
