#pragma once

#include "LmDefine.h"

class LmTalkBox
{
public:
	LmTalkBox();
	virtual ~LmTalkBox();

	BOOL Init(LPDIRECT3DDEVICE9 _pDevice);
	VOID Cleanup();
	BOOL Setup();
	VOID Update();
	VOID Render(LPDIRECT3DDEVICE9 _pDevice);

	VOID SetPosF(FLOAT _fPosXF, FLOAT _fPosYF);
	VOID SetMsg(LPCWSTR _pcwsMsg){ m_wstrMsg = _pcwsMsg; }
	VOID SetColor(D3DXCOLOR _dxColor){ m_dxColor = _dxColor; }
	VOID Show(BOOL _bShow){ m_bShow = _bShow; }
	BOOL IsShow(){return m_bShow; }

private:
	VOID RenderText();

	BOOL m_bShow;
	wstring m_wstrMsg;

	TALKBOX_TYPE m_BoxType;

	// 텍스트용 스프라이트
	ID3DXFont*              m_pFont;
	ID3DXSprite*            m_pTextSprite;

	// 박스용 스프라이트
	//ID3DXSprite*            m_pBoxSprite;
	//LPDIRECT3DTEXTURE9      m_pBoxTexture;

	LPDIRECT3DDEVICE9		m_pDevice;

	FLOAT m_fPosXF;
	FLOAT m_fPosYF;
	D3DXCOLOR m_dxColor;
};