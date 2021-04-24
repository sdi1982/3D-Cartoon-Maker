//
//
//  @ Project : 3D ī�� ����Ŀ
//  @ File Name : LmCamera.h
//  @ Date : 2007-10-07
//  @ Author : �ŵ���
//
//


#if !defined(_LMCAMERA_H)
#define _LMCAMERA_H


//
//
//  @ class : ī�޶� Ŭ����
//  @ brief : D3Dȭ���� ������ �����ִ� ī�޶� �����ϴ� Ŭ����
//  @ Date : 2007-10-12
//  @ Author : �ŵ���
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
