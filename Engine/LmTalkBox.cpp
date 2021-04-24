#include "dxstdafx.h"
#include "LmTalkBox.h"
#include "LmCamera.h"

LmTalkBox::LmTalkBox()
{
	m_pDevice = NULL;
	m_pFont = NULL;
	m_pTextSprite = NULL;
	//m_pBoxSprite = NULL;
	//m_pBoxTexture = NULL;
	m_bShow = FALSE;
	m_wstrMsg = L"에러같은데요";
	m_dxColor = D3DCOLOR_XRGB(0,0,0);
}

LmTalkBox::~LmTalkBox()
{
	Cleanup();
	SAFE_RELEASE( m_pFont );
}

VOID LmTalkBox::Update()
{
	D3DXMATRIX matScale;
	D3DXMatrixIdentity(&matScale);
	D3DXMatrixScaling(&matScale, 1.0f, 1.0f,1.0f);

	D3DXMATRIX matTrans;
	D3DXMatrixIdentity(&matTrans);

	D3DXMatrixTranslation(&matTrans, 
		TheCamera->GetScreenWidth()/2.0f * (0.95f + m_fPosXF), 
		TheCamera->GetScreenHeight()/2.0f * (0.7f - m_fPosYF), 
		0.0f);

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixMultiply(&matWorld,&matScale,&matTrans);

	if(m_pTextSprite)
		m_pTextSprite->SetTransform(&matWorld);
}

VOID LmTalkBox::Render( LPDIRECT3DDEVICE9 _pDevice )
{	
	if(!m_bShow)
		return;

	if(_pDevice)
	{		
		RenderText();

		//m_pBoxSprite->Begin(D3DXSPRITE_ALPHABLEND);
		//m_pBoxSprite->Draw(m_pBoxTexture, NULL, NULL, NULL, 0xFFFFFFFF );
		//m_pBoxSprite->End();
	}
}

VOID LmTalkBox::Cleanup()
{
	if( m_pFont )
		m_pFont->OnLostDevice();

	SAFE_RELEASE( m_pTextSprite );
	SAFE_RELEASE( m_pFont );
}

BOOL LmTalkBox::Setup()
{
	HRESULT hr;

	if(m_pDevice)
	{
		// 폰트 다시 생성
		hr = D3DXCreateFont( m_pDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
			OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
			L"Arial", &m_pFont );
		if(FAILED(hr))
			return FALSE;

		// 폰트 
		if( m_pFont )
		{
			hr = m_pFont->OnResetDevice();
			if(FAILED(hr))
				return FALSE;
		}

		// 폰트 스프라이트
		hr = D3DXCreateSprite( m_pDevice, &m_pTextSprite );
		if(FAILED(hr))
			return FALSE;

		//// 박스 스프라이트
		//V(D3DXCreateSprite(m_pDevice, &m_pBoxSprite));

		//// 박스 텍스쳐
		//wstring wstrFile = L"Data\\talkbox.bmp";

		//WCHAR strPath[MAX_PATH];
		//V(DXUTFindDXSDKMediaFileCch( strPath, MAX_PATH, wstrFile.c_str()));
		//V(D3DXCreateTextureFromFile(m_pDevice, strPath , &m_pBoxTexture));

	}

	return TRUE;
}

BOOL LmTalkBox::Init( LPDIRECT3DDEVICE9 _pDevice )
{
	HRESULT hr;

	if(!_pDevice)
		m_pDevice = _pDevice;

	// 폰트 
	hr = D3DXCreateFont( _pDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
		L"Arial", &m_pFont );
	if(FAILED(hr))
		return FALSE;


	if( m_pFont )
		V_RETURN( m_pFont->OnResetDevice() );

	// 퐅트 스프라이트
	hr = D3DXCreateSprite( _pDevice, &m_pTextSprite );
	if(FAILED(hr))
		return FALSE;

	//// 박스 스프라이트
	//V(D3DXCreateSprite(_pDevice, &m_pBoxSprite));

	//// 박스 텍스쳐
	//wstring wstrFile = L"Data\\talkbox.bmp";

	//WCHAR strPath[MAX_PATH];
	//V(DXUTFindDXSDKMediaFileCch( strPath, MAX_PATH, wstrFile.c_str()));
	//V(D3DXCreateTextureFromFile(_pDevice, strPath , &m_pBoxTexture));

	return TRUE;
}


void LmTalkBox::RenderText()
{
	if(m_pFont && m_pTextSprite)
	{
		CDXUTTextHelper txtHelper( m_pFont, m_pTextSprite, 10 );

		// Output statistics
		txtHelper.Begin();
		txtHelper.SetInsertionPos( 0, 0 );
		//txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
		//txtHelper.DrawTextLine( DXUTGetFrameStats() );
		//txtHelper.DrawTextLine( DXUTGetDeviceStats() );

		txtHelper.SetForegroundColor( m_dxColor );
		txtHelper.DrawTextLine( m_wstrMsg.c_str() );

		txtHelper.End();
	}
}

VOID LmTalkBox::SetPosF( FLOAT _fPosXF, FLOAT _fPosYF )
{
	m_fPosXF = _fPosXF;
	m_fPosYF = _fPosYF;
}