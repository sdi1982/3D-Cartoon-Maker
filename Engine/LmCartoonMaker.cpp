//
//
//  @ Project : 3D 카툰 메이커
//  @ File Name : LmMainFrame.cpp
//  @ Date : 2007-10-11
//  @ Author : 신동인
//
//

#include "dxstdafx.h"
#include "LmCartoonMaker.h"
#include "LmScene.h"
#include "LmTerrain.h"
#include "LmPickBoard.h"

#include "LmModelManager.h"
#include "LmCamera.h"
#include "LmEffectManager.h"

LmCartoonMaker* LmCartoonMaker::GetInstance()
{
	static LmCartoonMaker g_MainFrame;

	return &g_MainFrame;
}

LmCartoonMaker::LmCartoonMaker()
{
	// 디버깅시 런타임 메모리 릭 체크를 활성화한다.
	#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif

	// 메모리 릭 디버깅용
	// _CrtSetBreakAlloc(번지);
	//_CrtSetBreakAlloc(4242);
	//_CrtSetBreakAlloc(4220);
	//_CrtSetBreakAlloc(5489);

	m_pEffect = NULL;
	m_pGotThing = NULL;

	m_pTitleSprite = NULL;
	m_pTitleTexture = NULL;

	m_enumStateOfUI = INIT_STATE;
	m_enumStateOfUIBefore = INIT_STATE;
	m_TimerStop = FALSE;

	m_uMakeWidth = 360;
	m_uMakeHeight = 240;	
}

LmCartoonMaker::~LmCartoonMaker()
{
}


BOOL LmCartoonMaker::CameraMsgHandler(HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam )
{
	BOOL hr = FALSE;

	// 동영상 제작중이면 마우스를 트래킹하지 않는다.
	if(m_bMakeMode == TRUE)
		return hr;

	// 현재 씬이 없으면 카메라를 처리하지 않는다.
	if(TheSceneManager->GetNowScene() == NULL)
		return hr;

	// 카메라에 메세지를 전달한다.
	hr = TheCamera->HandleMessages(_hWnd, _uMsg, _wParam, _lParam);
	return TRUE;
}

HRESULT LmCartoonMaker::InitD3D( HWND _hWnd, UINT _uWidth, UINT _uHeight, D3DDEVTYPE _DeviceType )
{
	HRESULT hr = 0;
	// Step 1: Create the IDirect3D9 object.
	IDirect3D9* d3d9 = 0;
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if( !d3d9 )
	{
		::MessageBox(0, L"Direct3DCreate9() - FAILED", 0, 0);
		return E_FAIL;
	}

	// Step 2: Check for hardware vp.
	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, _DeviceType, &caps);
	int vp = 0;
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// Step 3: Fill out the D3DPRESENT_PARAMETERS structure.
	m_PresentParameter.BackBufferWidth = _uWidth;
	m_PresentParameter.BackBufferHeight = _uHeight;
	m_PresentParameter.BackBufferFormat = D3DFMT_A8R8G8B8;
	m_PresentParameter.BackBufferCount = 1;
	m_PresentParameter.MultiSampleType = D3DMULTISAMPLE_NONE;
	m_PresentParameter.MultiSampleQuality = 0;
	m_PresentParameter.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_PresentParameter.hDeviceWindow = _hWnd;
	m_PresentParameter.Windowed = true;
	m_PresentParameter.EnableAutoDepthStencil = true;
	m_PresentParameter.AutoDepthStencilFormat = D3DFMT_D24S8;
	m_PresentParameter.Flags = 0;
	m_PresentParameter.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_PresentParameter.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Step 4: Create the device.
	hr = d3d9->CreateDevice(
		D3DADAPTER_DEFAULT, // primary adapter
		_DeviceType, // device type
		_hWnd, // window associated with device
		vp, // vertex processing
		&m_PresentParameter, // present parameters
		&m_pDevice); // return created device
	if( FAILED(hr) )
	{
		// try again using a safer configuration.
		m_PresentParameter.AutoDepthStencilFormat = D3DFMT_D16;
		hr = d3d9->CreateDevice(
			D3DADAPTER_DEFAULT,
			_DeviceType,
			_hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&m_PresentParameter,
			&m_pDevice);
		if( FAILED(hr) )
		{
			d3d9->Release(); // done with d3d9 object
			::MessageBox(0, L"CreateDevice() - FAILED", 0, 0);
			return hr;
		}
	}
	d3d9->Release(); // done with d3d9 object

	TheTerrain->Init(m_pDevice);
	ThePickBoard->Init(m_pDevice, 21, 21, 10);
	TheEffectManager->Init(m_pDevice);
	TheEffectManager->SetEffectNo(3);

	// 사운드 매니저 초기화
	TheSoundManager->SetHwnd(m_hMainWnd);
	return S_OK;
}

HRESULT LmCartoonMaker::SetupD3D( UINT _uWidth, UINT _uHeight )
{
	if( m_pDevice )
	{
		m_pDevice->SetRenderState( D3DRS_ZENABLE, TRUE ); // Z버퍼를 사용한다.
		m_pDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff ); // 환경광을 켠다.

		TheCamera->Setup(_uWidth, _uHeight);

		// 모델 매니저 객체는 재할당을 안해줘도 된다.
		//TheModelManager->Setup(_uWidth, _uHeight); 
		TheSceneManager->Setup(_uWidth, _uHeight);

		TheEffectManager->Setup();
	}

	return S_OK;
}

HRESULT LmCartoonMaker::CleanupD3D()
{
	TheModelManager->Cleanup();
	TheSceneManager->Cleanup();
	TheEffectManager->Cleanup();

	SAFE_RELEASE(m_pTitleTexture);
	SAFE_RELEASE(m_pTitleSprite);
	return S_OK;
}

HRESULT LmCartoonMaker::UpdateD3D( FLOAT _fTimeDelta )
{
	if( m_pDevice )
	{
		TheCamera->Update(0.0f);
		TheSceneManager->Update(0.0f);
		ThePickBoard->Update(0.0f);
	}
	return S_OK;
}

HRESULT LmCartoonMaker::RenderD3D()
{
	if( m_pDevice )
	{
		HRESULT hr;

		// 렌터링 타겟과 Z버퍼를 초기화한다.
		V( m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER , D3DCOLOR_XRGB(65, 137, 200), 1.0f, 0) );

		// 씬을 렌더링한다.
		if( SUCCEEDED( m_pDevice->BeginScene() ) )
		{

			if(TheSceneManager->GetNowScene() == NULL)
			{
				RenderTitle();
			}
			else
			{
				TheCamera->Render(m_pDevice); // 카메라를 투영행렬을 설정한다.
				TheSceneManager->Render(m_pDevice);
				ThePickBoard->Render(m_pDevice);
				TheTerrain->Render(m_pDevice);
				TheEffectManager->Render(m_pDevice);
			}

			V( m_pDevice->EndScene() );
		}

		m_pDevice->Present(0, 0, 0 , 0);
	}

	return S_OK;
}

HRESULT LmCartoonMaker::ResizeD3D( UINT _cx, UINT _cy )
{
	if( m_pDevice )
	{
		HRESULT hr = 0;
		// On a resize we must change the dimensions of the
		// back buffers to match the new window size.
		m_PresentParameter.BackBufferWidth = _cx;
		m_PresentParameter.BackBufferHeight = _cy;
		// We are about to call Reset, free any resources
		// that need to be freed prior to a Reset.
		hr = CleanupD3D();
		if(FAILED(hr))
		{
			MessageBox(NULL, L"CleanupD3D() - Failed", L"Error", 0);
			return S_FALSE;
		}
		// Reset the flipping chain with the new window dimensions.
		// Note that all device states are reset to the default
		// after this call.
		hr = m_pDevice->Reset(&m_PresentParameter);
		if(FAILED(hr))
		{
			MessageBox(NULL, L"Reset() - Failed", L"Error", 0);
			return S_FALSE;
		}
		// Reinitialize resource and device states since we
		// Reset everything.
		hr = SetupD3D(_cx, _cy);

		if(FAILED(hr))
		{
			MessageBox(NULL, L"SetupD3D() - Failed", L"Error",0);
			return S_FALSE;
		}
	}

	return S_OK;
}

BOOL LmCartoonMaker::Read3cmFile()
{
	CStdioFile file;

	if(file.Open(L"3cm.txt",CFile::modeRead) == 0)
	{
		MessageBox(NULL, L"3cm.txt를 열 수 없습니다.",L"에러", 0);
		return FALSE;
	}

	CString str;
	
	int mode = 0;
	
	// RTF-8로 저장된 걸 읽으면 처음에 쓰레기값이 들어가있어서
	// 텍스트파일의 첫줄은 띄워서 쓰레기값을 읽어서 버리고
	// 다음부터 읽어야 한다. <버그 - 우선순위:낮음>

	while(file.ReadString(str))
	{
		if( str.CompareNoCase(L"[terrain]") == 0)
		{
			mode = 1;
			continue;
		}

		else if( str.CompareNoCase(L"[background]") == 0)
		{
			mode = 2;
			continue;
		}

		else if( str.CompareNoCase(L"[thing]") == 0)
		{
			mode = 3;
			continue;
		}
		else if( str.CompareNoCase(L"[character]") == 0)
		{
			mode = 4;
			continue;
		}
		else if(str.Compare(L"") == 0)
			continue;

		switch(mode)
		{
		case 0: break;
		case 1: /* 지형 객체 등록 <미구현> */break;
		case 2: TheModelManager->RegisterBG(str.GetBuffer(0)); break;
		case 3: TheModelManager->RegisterThing(str.GetBuffer(0)); break;
		case 4: TheModelManager->RegisterChar(str.GetBuffer(0)); break;
		}
	};

	file.Close();	

	return TRUE;
}

HRESULT LmCartoonMaker::DestroyD3D()
{
	CleanupD3D();

	m_pDevice->Release();
	return S_OK;
}

VOID LmCartoonMaker::TrackingMouse( INT _iMouseX, INT _iMouseY, UINT _uHeight, UINT _uWidth )
{
	POINT TilePos = {0, 0};

	// 동영상 제작중이면 마우스를 트래킹하지 않는다.
	if(m_bMakeMode == TRUE)
		return;

	// 현재 씬이 없으면 마우스 피킹을 처리하지 않는다.
	if(TheSceneManager->GetNowScene() == NULL)
		return;

	// 픽킹보드가 안보이면 타일의 위치를 구할 수가 없다.
	//if(!ThePickBoard->IsShow())
	//	return;

	D3DXVECTOR3 vPickRayDir;
	D3DXVECTOR3 vPickRayOrig;
	CONST D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetBackBufferSurfaceDesc();
	CONST D3DXMATRIX *pmatProj = TheCamera->GetProjMatrix();

	//POINT ptCursor;
	//GetCursorPos( &ptCursor );
	//ScreenToClient( DXUTGetHWND(), &ptCursor );

	// 화면에서 피킹 광선의 벡터를 계산한다.
	D3DXVECTOR3 v;
	//v.x =  ( ( ( 2.0f * ptCursor.x ) / pd3dsdBackBuffer->Width  ) - 1 ) / pmatProj->_11;
	//v.y = -( ( ( 2.0f * ptCursor.y ) / pd3dsdBackBuffer->Height ) - 1 ) / pmatProj->_22;
	v.x =  ( ( ( 2.0f * _iMouseX ) / _uWidth  ) - 1 ) / pmatProj->_11;
	v.y = -( ( ( 2.0f * _iMouseY ) / _uHeight ) - 1 ) / pmatProj->_22;

	v.z =  1.0f;

	// 뷰 행렬의 역행렬을 구한다.
	CONST D3DXMATRIX* matView = TheCamera->GetViewMatrix();
	CONST D3DXMATRIX* matWorld = TheCamera->GetWorldMatrix();
	D3DXMATRIX mWorldView = *matWorld * *matView;
	D3DXMATRIX m;
	D3DXMatrixInverse( &m, NULL, &mWorldView );

	// 피킹 광선을 3차원 공간으로 변환한다.
	vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
	vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
	vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
	vPickRayOrig.x = m._41;
	vPickRayOrig.y = m._42;
	vPickRayOrig.z = m._43;


	//p1------p2
	//|       |
	//|       |
	//|       |
	//p0------p3

	FLOAT fWidth = (FLOAT)ThePickBoard->GetWidth();
	FLOAT fDepth = (FLOAT)ThePickBoard->GetDepth();

	D3DXVECTOR3 p0(-fWidth/2, 0.0f, -fDepth/2);
	D3DXVECTOR3 p1(-fWidth/2, 0.0f,  fDepth/2);
	D3DXVECTOR3 p2( fWidth/2, 0.0f,  fDepth/2);
	D3DXVECTOR3 p3( fWidth/2, 0.0f,  -fDepth/2);

	FLOAT U, V, D;
	
	if(D3DXIntersectTri(&p1,&p2, &p0, &vPickRayOrig, &vPickRayDir, &U, &V, &D))
	{

		FLOAT X = p1.x + (p2.x - p1.x)*U;
		FLOAT Z = p1.z + (p0.z - p1.z)*V;
		ThePickBoard->SetInter(TRUE);
		ThePickBoard->SetFloorPt(X, Z);
		return;
	}

	else if(D3DXIntersectTri(&p3,&p0,&p2, &vPickRayOrig, &vPickRayDir, &U, &V, &D))
	{
		FLOAT X = p3.x + (p0.x - p3.x)*U;
		FLOAT Z = p3.z + (p2.z - p3.z)*V;
		ThePickBoard->SetInter(TRUE);
		ThePickBoard->SetFloorPt(X, Z);
		return;
	}

	ThePickBoard->SetInter(FALSE);
}

BOOL LmCartoonMaker::CreateScene( LPCWSTR _pcwsScene, UINT _uFrameSize, UINT _uFPS, UINT _uTerTex )
{
	return TheSceneManager->CreateScene(_pcwsScene, _uFrameSize, _uFPS, _uTerTex);
}

VOID LmCartoonMaker::SelectNowScene( LPCWSTR _pcwsScene )
{
	TheSceneManager->SelectNowScene(_pcwsScene);
}

BOOL LmCartoonMaker::DeleteScene( LPCWSTR _pcwsScene ) /* 씬 삭제 */
{
	if(!TheSceneManager->DeleteScene(_pcwsScene))
		return FALSE;

	return TRUE;
}

VOID LmCartoonMaker::ShowPickBoard( BOOL _bShow )
{
	ThePickBoard->Show(_bShow);
	// 갱신된 화면을 보여주기 위해 렌더링함수를 호출한다.
	RenderD3D();
}

BOOL LmCartoonMaker::PlaceBGNowScean( LPCWSTR _pcwsBG )
{
	return TheSceneManager->GetNowScene()->PlaceBG(_pcwsBG);
}

BOOL LmCartoonMaker::SelectThing( LPCWSTR _pcwsThing )
{
	LmThing* pThing = TheModelManager->GetThing(_pcwsThing);
	ThePickBoard->SetDragThing(pThing);
	return TRUE;
}

BOOL LmCartoonMaker::PlaceSelectedThing()
{
	// 현재 피킹보드가 없으면 실패
	if(!ThePickBoard->IsShow())
		return FALSE;

	LmThing* pThing = ThePickBoard->GetDragThing();

	// 선택된 물체가 없으면 실패
	if(pThing)
	{
		// 다른물체가 있는 경우 FALSE 리턴
		BOOL bResult = TheSceneManager->GetNowScene()->PlaceThing(pThing->GetName(), 
			ThePickBoard->GetTileX(), ThePickBoard->GetTileZ());

		if(bResult)
		{
			ThePickBoard->SetDragThing(NULL);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL LmCartoonMaker::GetThing()
{
	if(!ThePickBoard->IsShow())
		return FALSE;

	// 현재 드래그중인 물체가 있으면 다른 물체를 선택할 수 없다.
	if(ThePickBoard->GetDragThing())
		return FALSE;

	// 현재 피킹보드 내부에 마우스 포인터가 없으면 실패
	if(!ThePickBoard->IsInter())
		return FALSE;

	// 현재 위치의 물체의 이름을 얻어온다.
	wstring strGetThing = TheSceneManager->GetNowScene()->GetSamePosThing(
		ThePickBoard->GetTileX(), ThePickBoard->GetTileZ());

	// 현재 위치에 물체가 없으면 선택된 물체를 없앤다.
	if(strGetThing == L"")
		return FALSE;

	m_pGotThing = TheSceneManager->GetNowScene()->GetThing(
		strGetThing.c_str());

	return TRUE;
}

BOOL LmCartoonMaker::RotateGotThing()
{
	// 선택한 물체가 없으면 최소한다.
	if(!m_pGotThing)
		return FALSE;

	m_pGotThing->Rotate();

	return TRUE;
}

BOOL LmCartoonMaker::CancelGotThing()
{
	if(m_pGotThing)
	{
		m_pGotThing = NULL;
		return TRUE;
	}

	return FALSE;
}

BOOL LmCartoonMaker::DeleteGotThing()
{
	if(m_pGotThing)
	{
		TheSceneManager->GetNowScene()->DeleteThing(m_pGotThing->GetName());
		return TRUE;
	}

	return FALSE;
}

CONST D3DXVECTOR3* LmCartoonMaker::GetCameraEyePt() CONST
{
	return TheCamera->GetEyePt();
}

VOID LmCartoonMaker::SetCameraEyePt( FLOAT _fX, FLOAT _fY, FLOAT _fZ )
{
	TheCamera->SetEyePt(_fX, _fY, _fZ);
}

//------------------------------------------------------------------------------
// Sound 관련
//------------------------------------------------------------------------------
BOOL LmCartoonMaker::InsertSound(LPCWSTR _pcwFileName, LPCWSTR _pcwSoundID)
{
	CString SoundID_SceneName;
	SoundID_SceneName = _pcwSoundID;
	SoundID_SceneName += "/";
	SoundID_SceneName += TheCartoonMaker->GetNowSceneName();

	if (!TheSoundManager->Register(_pcwFileName, SoundID_SceneName)){
		return FALSE;
	}

	LmSound *pSound = TheSoundManager->GetSound(SoundID_SceneName);
	
	if (pSound){
		if (!pSound->Init()){
			TheSoundManager->Unregister(SoundID_SceneName);
			return FALSE;
		}

		return TRUE;
	}
	
	return FALSE;		
}

BOOL LmCartoonMaker::RemoveSound(LPCWSTR _pcwSoundID)
{
	CString SoundID_SceneName;
	SoundID_SceneName = _pcwSoundID;
	SoundID_SceneName += "/";
	SoundID_SceneName += TheCartoonMaker->GetNowSceneName();

	return TheSoundManager->Unregister(SoundID_SceneName);
}

BOOL LmCartoonMaker::SoundIDCheck(LPCWSTR _pcwSoundID)
{
	CString SoundID_SceneName;
	SoundID_SceneName = _pcwSoundID;
	SoundID_SceneName += "/";
	SoundID_SceneName += TheCartoonMaker->GetNowSceneName();

	if (TheSoundManager->GetSound(SoundID_SceneName))
		return FALSE;
	
	return TRUE;
}

BOOL  LmCartoonMaker::SoundPlay(LPCWSTR _pcwSoundID)
{
	CString SoundID_SceneName;
	SoundID_SceneName = _pcwSoundID;
	SoundID_SceneName += "/";
	SoundID_SceneName += TheCartoonMaker->GetNowSceneName();

	LmSound *pSound;
	pSound = TheSoundManager->GetSound(SoundID_SceneName);

	if (pSound)
		return pSound->Play();

	return FALSE;
}

BOOL LmCartoonMaker::SetVolume(LPCWSTR _pcwSoundID, UINT Volume)
{
	CString SoundID_SceneName;
	SoundID_SceneName = _pcwSoundID;
	SoundID_SceneName += "/";
	SoundID_SceneName += TheCartoonMaker->GetNowSceneName();

	LmSound *pSound;
	pSound = TheSoundManager->GetSound(SoundID_SceneName);

	if (pSound)
		return pSound->SetVolume(Volume);

	return FALSE;
}

UINT LmCartoonMaker::GetVolume(LPCWSTR _pcwSoundID)
{
	CString SoundID_SceneName;
	SoundID_SceneName = _pcwSoundID;
	SoundID_SceneName += "/";
	SoundID_SceneName += TheCartoonMaker->GetNowSceneName();

	LmSound *pSound;
	pSound = TheSoundManager->GetSound(SoundID_SceneName);

	if (pSound)
		return pSound->GetVolume();

	return 0;
}

BOOL LmCartoonMaker::Seek(LPCWSTR _pcwSoundID, DWORD dwMSec)
{
	CString SoundID_SceneName;
	SoundID_SceneName = _pcwSoundID;
	SoundID_SceneName += "/";
	SoundID_SceneName += TheCartoonMaker->GetNowSceneName();

	LmSound *pSound;
	pSound = TheSoundManager->GetSound(SoundID_SceneName);

	if (pSound)
		return pSound->Seek(dwMSec);

	return FALSE;
}

VOID LmCartoonMaker::Pause()
{

}

VOID LmCartoonMaker::FadeIn(LPCWSTR _pcwSoundID, DWORD dwTime)
{
	CString SoundID_SceneName;
	SoundID_SceneName = _pcwSoundID;
	SoundID_SceneName += "/";
	SoundID_SceneName += TheCartoonMaker->GetNowSceneName();

	LmSound *pSound;
	pSound = TheSoundManager->GetSound(SoundID_SceneName);

	if (pSound)
		pSound->FadeIn(dwTime);
}

VOID LmCartoonMaker::FadeOut(LPCWSTR _pcwSoundID, DWORD dwTime)
{
	CString SoundID_SceneName;
	SoundID_SceneName = _pcwSoundID;
	SoundID_SceneName += "/";
	SoundID_SceneName += TheCartoonMaker->GetNowSceneName();

	LmSound *pSound;
	pSound = TheSoundManager->GetSound(SoundID_SceneName);

	if (pSound)
		pSound->FadeOut(dwTime);
}

DWORD LmCartoonMaker::GetCurrentPos(LPCWSTR _pcwSoundID)
{
	CString SoundID_SceneName;
	SoundID_SceneName = _pcwSoundID;
	SoundID_SceneName += "/";
	SoundID_SceneName += TheCartoonMaker->GetNowSceneName();

	LmSound *pSound;
	pSound = TheSoundManager->GetSound(SoundID_SceneName);

	if (pSound)
		return pSound->GetCurrentTime();

	return 0;
}

DWORD LmCartoonMaker::GetTotalTime(LPCWSTR _pcwSoundID)
{
	CString SoundID_SceneName;
	SoundID_SceneName = _pcwSoundID;
	SoundID_SceneName += "/";
	SoundID_SceneName += TheCartoonMaker->GetNowSceneName();

	LmSound *pSound;
	pSound = TheSoundManager->GetSound(SoundID_SceneName);

	if (pSound)
		return pSound->GetTotalTime();

	return 0;
}

BOOL LmCartoonMaker::IsPlay(LPCWSTR _pcwSoundID)
{
	CString SoundID_SceneName;
	SoundID_SceneName = _pcwSoundID;
	SoundID_SceneName += "/";
	SoundID_SceneName += TheCartoonMaker->GetNowSceneName();

	LmSound *pSound;
	pSound = TheSoundManager->GetSound(SoundID_SceneName);

	if (pSound)
		return pSound->IsPlay();

	return FALSE;
}

LPCWSTR LmCartoonMaker::GetSoundFileName(LPCWSTR _pcwSoundID)
{
	CString SoundID_SceneName;
	SoundID_SceneName = _pcwSoundID;
	SoundID_SceneName += "/";
	SoundID_SceneName += TheCartoonMaker->GetNowSceneName();

	LmSound *pSound;
	pSound = TheSoundManager->GetSound(SoundID_SceneName);

	if (pSound)
		return pSound->GetFileName();

	return NULL;

}

LPCWSTR LmCartoonMaker::GetLastErrorSound(LPCWSTR _pcwSoundID)
{
	CString SoundID_SceneName;
	SoundID_SceneName = _pcwSoundID;
	SoundID_SceneName += "/";
	SoundID_SceneName += TheCartoonMaker->GetNowSceneName();

	LmSound *pSound;
	pSound = TheSoundManager->GetSound(SoundID_SceneName);

	if (pSound)
		return pSound->GetLastError();

	return NULL;
}

FLOAT LmCartoonMaker::GetCameraDist()
{
	return TheCamera->GetDist();
}

VOID LmCartoonMaker::ResetCameraEyePt()
{
	TheCamera->ResetEyePt();
}

INT LmCartoonMaker::GetCurFrame()
{
	return TheTimeLine->GetCurCaret();
}

INT LmCartoonMaker::GetFrameLength()
{
	return TheTimeLine->GetFrameLength();
}

INT LmCartoonMaker::GetFramePerSec()
{
	return TheTimeLine->GetFramePerSec();
}

VOID LmCartoonMaker::SetCurFrame( INT _iCurCaret )
{
	if(!TheTimeLine->SetCurCaret(_iCurCaret))
		return;

	// 현재 씬의 프레임을 설정한다.
	LmScene* pNowScene = TheSceneManager->GetNowScene();
	if( pNowScene == NULL)
		return;

	pNowScene->SetCurrFrame((UINT)_iCurCaret);
}

VOID LmCartoonMaker::SetFrameLength( INT _iFrameLength )
{
	TheTimeLine->SetFrameLength(_iFrameLength);
}

VOID LmCartoonMaker::SetFramePerSec( INT _iFrmPerSec )
{
	TheTimeLine->SetFramePerSec(_iFrmPerSec);
}

INT LmCartoonMaker::GetListLayerSize()
{
	return TheTimeLine->GetListSize();
}

LayerListItr LmCartoonMaker::GetListLayerBeginItr()
{
	return TheTimeLine->GetListBeginItr();
}

LayerListItr LmCartoonMaker::GetListLayerEndItr()
{
	return TheTimeLine->GetListEndItr();
}

VOID LmCartoonMaker::InitTimeLine()
{
	TheTimeLine->InitTimeLIne();
}

VOID LmCartoonMaker::AddCameraDist( FLOAT _fD )
{
	TheCamera->AddDist(_fD);
}

BOOL LmCartoonMaker::ScreenToFile( LPCWSTR szFileName )
{
	HRESULT				hr;	
	LPDIRECT3DSURFACE9	pSurface;


	hr = m_pDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface);
	if (FAILED(hr))
		return FALSE;

	hr = D3DXSaveSurfaceToFile(szFileName, D3DXIFF_BMP, pSurface, NULL, NULL);
	if (FAILED(hr))
	{
		pSurface->Release();
		return FALSE;
	}

	pSurface->Release();	

	return TRUE;
}

BOOL LmCartoonMaker::ScreenToMemory( LPD3DXBUFFER lpd3dxbuffer )
{
	HRESULT				hr;

	LPDIRECT3DSURFACE9	pSurface;
	hr = m_pDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface);
	if (FAILED(hr))
		return FALSE;

	hr = D3DXSaveSurfaceToFileInMemory(&lpd3dxbuffer, D3DXIFF_BMP, pSurface, NULL, NULL);
	if (FAILED(hr))
	{
		pSurface->Release();
		return FALSE;
	}

	pSurface->Release();	

	return TRUE;
}

VOID LmCartoonMaker::InsertCameraKey()
{
	UINT uFrameNo = TheTimeLine->GetCurCaret();

	LmScene* pNowScene = TheSceneManager->GetNowScene();
	if(pNowScene == NULL)
		return;

	LmInfoCamera CameraKey;

	CameraKey.bKey = TRUE;
	CameraKey.vEye = *GetCameraEyePt();

	pNowScene->InsertCameraKey(uFrameNo, CameraKey);
	TheTimeLine->InsertCameraSection(uFrameNo);
}

VOID LmCartoonMaker::InsertSoundSection( LPCWSTR _pcwsSoundName, UINT _uStartFrm, DWORD _dwSoundLength, DWORD _dwFadeIn, DWORD _dwFadeOut )
{
	TheTimeLine->InsertSoundSection(_pcwsSoundName, _uStartFrm, _dwSoundLength, _dwFadeIn, _dwFadeOut);
}

INT LmCartoonMaker::RemoveSoundSection( LPCWSTR _pcwsSoundName )
{
	return TheTimeLine->RemoveSoundSection(_pcwsSoundName);
}

INT LmCartoonMaker::ModifySoundSection( LPCWSTR _pcwsSoundName, LPCWSTR _pcwsNewSoundName, UINT _uStartFrm, DWORD _dwSoundLength, DWORD _dwFadeIn, DWORD _dwFadeOut )
{
	return TheTimeLine->ModifySoundSection(_pcwsSoundName, _pcwsNewSoundName, _uStartFrm, _dwSoundLength, _dwFadeIn, _dwFadeOut);
}

wstring LmCartoonMaker::InsertCharacterLayer( LPCWSTR _pcwsCharacterName )
{
	return TheTimeLine->InsertCharacterLayer(_pcwsCharacterName);
}

INT LmCartoonMaker::RemoveCharacterLayer( LPCWSTR _pcwsCharacterName )
{
	return TheTimeLine->RemoveCharacterLayer(_pcwsCharacterName);
}

INT LmCartoonMaker::ModifyCharacterLayerName( LPCWSTR _pcwsLayerName, LPCWSTR _pcwsNewLayerName )
{
	return TheTimeLine->ModifyCharacterLayerName(_pcwsLayerName, _pcwsNewLayerName);
}


VOID LmCartoonMaker::DeleteCameraKey()
{
	UINT uFrameNo = TheTimeLine->GetCurCaret();

	if(TheSceneManager->GetNowScene()->RemoveCameraKey(uFrameNo) == TRUE)
		TheTimeLine->RemoveCameraSection(uFrameNo);
}

INT LmCartoonMaker::InsertTalkBoxSection( LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uTimeLength )
{
	return TheTimeLine->InsertTalkBoxSection(_pcwsCharacterName, _uStartFrm, _uTimeLength );
}

INT LmCartoonMaker::InsertTalkBoxSection(LPCWSTR _pcwsCharacterName, LmSection _Secton)
{
	return TheTimeLine->InsertTalkBoxSection(_pcwsCharacterName, _Secton);
}

INT LmCartoonMaker::RemoveTalkBoxSection( LPCWSTR _pcwsCharacterName, UINT _uStartFrm )
{
	return TheTimeLine->RemoveTalkBoxSection(_pcwsCharacterName, _uStartFrm);
}

INT LmCartoonMaker::ModifyTalkBoxSection( LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uNewStartFrm, UINT _uNewTimeLength )
{
	return TheTimeLine->ModifyTalkBoxSection(_pcwsCharacterName, _uStartFrm, _uNewStartFrm, _uNewTimeLength) ;
}

INT LmCartoonMaker::InsertActionSection( LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uTimeLength )
{
	return TheTimeLine->InsertActionSection( _pcwsCharacterName, _uStartFrm, _uTimeLength );
}

INT LmCartoonMaker::InsertActionSection( LPCWSTR _pcwsCharacterName, LmSection _Secton )
{
	return TheTimeLine->InsertActionSection(_pcwsCharacterName, _Secton);
}

INT LmCartoonMaker::RemoveActionSection( LPCWSTR _pcwsCharacterName, UINT _uStartFrm )
{
	return TheTimeLine->RemoveActionSection(_pcwsCharacterName, _uStartFrm);
}

INT LmCartoonMaker::ModifyActionSection( LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uNewStartFrm, UINT _uNewTimeLength )
{
	return TheTimeLine->ModifyActionSection(_pcwsCharacterName, _uStartFrm, _uNewTimeLength, _uNewTimeLength);
}

INT LmCartoonMaker::InsertCameraSection( UINT _uPosFrm )
{
	return TheTimeLine->InsertCameraSection(_uPosFrm);
}

INT LmCartoonMaker::InsertCameraSection( LmSection _Secton )
{
	return TheTimeLine->InsertCameraSection(_Secton);
}

INT LmCartoonMaker::RemoveCameraSection( UINT _uPosFrm )
{
	return TheTimeLine->RemoveCameraSection(_uPosFrm);
}

INT LmCartoonMaker::ModifyCameraSection( UINT _uPosFrm, UINT _uNewPosFrm )
{
	return TheTimeLine->ModifyCameraSection(_uPosFrm, _uNewPosFrm);
}

IPicture* LmCartoonMaker::ScreenToMemory()
{
	HRESULT				hr;

	D3DDISPLAYMODE		mode;
	hr = m_pDevice->GetDisplayMode(0, &mode);
	if (FAILED(hr))
		return NULL;

	LPDIRECT3DSURFACE9	pSurface;

	hr = m_pDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface);

	if (FAILED(hr))
	{
		pSurface->Release();
		return NULL;
	}

	D3DSURFACE_DESC		suffaceDesc;

	hr = pSurface->GetDesc(&suffaceDesc);
	if (FAILED(hr))
		return NULL;

	CString strFileName;
	CString strResDirectory;
	GetResourceDirectory(strResDirectory);

	strFileName.Format(L"%s\\NewScene.bmp",strResDirectory.GetBuffer(0));

	hr = D3DXSaveSurfaceToFile(strFileName.GetBuffer(0), D3DXIFF_BMP, pSurface, NULL, NULL);	

	IPicture * pPic; //사진을 위한 클래스

	//OLE 객체형식으로 새로운 사진 객체를 만들고 초기화 시킴
	OleLoadPicturePath(strFileName.GetBuffer(0), NULL, NULL, NULL, IID_IPicture,(LPVOID*)&pPic);

	//IPicture 객체가 제대로 생성이 되었는가?
	if (pPic==NULL) 
	{
		//객체가 제대로 생성되지 않았음.
		return NULL;
	}

	//저장된 파일을 삭제한다.
	DeleteFile(strFileName.GetBuffer(0));

	pSurface->Release();

	return pPic;

}

//////////////////////////////////////////////////////////////////////////
// 캐릭터 관련 메소드

BOOL LmCartoonMaker::SelectChar( LPCWSTR _pcwsChar )
{
	LmChar* pChar = TheModelManager->GetChar(_pcwsChar);
	ThePickBoard->SetDragChar(pChar);

	return TRUE;
}

BOOL LmCartoonMaker::PlaceSelectedChar()
{
	if(!ThePickBoard->IsShow())
		return FALSE;

	LmChar* pChar = ThePickBoard->GetDragChar();

	// 현재 선택된 캐릭터가 없으면 실패
	if(pChar)
	{
		// 다른캐릭터가 있는 경우 FALSE 리턴
		BOOL bResult = TheSceneManager->GetNowScene()->PlaceChar(pChar->GetName(), 
			ThePickBoard->GetTileX(), ThePickBoard->GetTileZ());

		if(bResult)
		{
			ThePickBoard->SetDragChar(NULL);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL LmCartoonMaker::GetChar()
{
	LmScene* pNowScene = TheSceneManager->GetNowScene();
	if(pNowScene == NULL)
		return FALSE;

	// 현재 드래그중인 물체가 있으면 다른 물체를 선택할 수 없다.
	if(ThePickBoard->GetDragChar())
		return FALSE;

	// 현재 위치의 캐릭터의 이름을 얻어온다.
	wstring strGetChar = pNowScene->GetSamePosChar(
		ThePickBoard->GetTileX(), 
		ThePickBoard->GetTileZ());

	// 현재 위치에 캐릭터가 없으면 선택된 캐릭터를 없앤다.
	if(strGetChar == L"")
		return FALSE;

	m_pGotChar = TheSceneManager->GetNowScene()->GetChar(
		strGetChar.c_str());

	return TRUE;
}

BOOL LmCartoonMaker::RotateGotChar()
{
	// 선택한 캐릭터가 없으면 최소한다.
	if(!m_pGotChar)
		return FALSE;

	m_pGotChar->Rotate();

	return TRUE;
}

BOOL LmCartoonMaker::CancelGotChar()
{
	if(m_pGotChar)
	{
		m_pGotChar = NULL;
		return TRUE;
	}

	return FALSE;
}

BOOL LmCartoonMaker::DeleteGotChar()
{
	if(m_pGotChar)
	{
		if(TheTimeLine->RemoveCharacterLayer(m_pGotChar->GetName()) < 0)
			return FALSE;
		TheSceneManager->GetNowScene()->DeleteChar(m_pGotChar->GetName());
		return TRUE;
	}

	return FALSE;
}


VOID LmCartoonMaker::RenderTitle()
{
	HRESULT hr;

	if(m_pTitleSprite == NULL)
	{
		V(D3DXCreateSprite(m_pDevice,&m_pTitleSprite));

		wstring wstrFile = L"Data\\title.bmp";

		WCHAR strPath[MAX_PATH];
		V(DXUTFindDXSDKMediaFileCch( strPath, MAX_PATH, wstrFile.c_str()));
		V(D3DXCreateTextureFromFile(m_pDevice, strPath , &m_pTitleTexture));
	}
	if(m_pTitleSprite)
	{		
		D3DXVECTOR3 center;
		center.x = 256.0f;
		center.y = 256.0f;
		center.z = 0.0f;
		
		D3DXVECTOR3 pos;
		pos.x = (FLOAT)m_uVIewWidth/2;
		pos.y = (FLOAT)m_uViewHeight/2;
		pos.z = 0.0f;

		m_pTitleSprite->Begin(D3DXSPRITE_ALPHABLEND);
		m_pTitleSprite->Draw(m_pTitleTexture, NULL, &center, &pos, 0xFFFFFFFF);
		m_pTitleSprite->End();
	}
}

BOOL LmCartoonMaker::DeleteBGNowScene()
{
	LmScene* pNowScene = TheSceneManager->GetNowScene();
	if(pNowScene == NULL)
		return FALSE;
		
	pNowScene->DeleteBG();

	return TRUE;
}

VOID LmCartoonMaker::SetMakeSize(ULONG _uWidth, ULONG _uHeight)
{
	m_uMakeWidth = _uWidth;
	m_uMakeHeight = _uHeight;
}

LmUIMotionThread* LmCartoonMaker::GetMotionThread()
{
	return TheTimeLine->GetMotionThread();
}

SectionVector LmCartoonMaker::GetNowSoundSection( UINT _uFrame, UINT * _uResultCount )
{
	return TheTimeLine->GetNowSoundSection(_uFrame, _uResultCount);
}

INT LmCartoonMaker::GetSoundLayerCount()
{
	return TheTimeLine->GetSoundLayerCount();
}

INT LmCartoonMaker::InsertFireWorksSection( UINT _uPosFrm )
{
	return TheTimeLine->InsertFireWorksSection(_uPosFrm);
}

INT LmCartoonMaker::InsertFireWorksSection( LmSection _Section )
{
	return TheTimeLine->InsertFireWorksSection(_Section);
}

INT LmCartoonMaker::RemoveFireWorksSection( UINT _uPosFrm )
{
	return TheTimeLine->RemoveFireWorksSection(_uPosFrm);
}

INT LmCartoonMaker::ModifyFireWorksSection( UINT _uPosFrm, UINT _uNewPosFrm )
{
	return TheTimeLine->ModifyFireWorksSection(_uPosFrm, _uNewPosFrm);
}

INT LmCartoonMaker::InsertSnowSection( LPCWSTR _pcwsSnowName, UINT _uStartFrm, UINT _uTimeLength )
{
	return TheTimeLine->InsertSnowSection(_pcwsSnowName, _uStartFrm, _uTimeLength);
}

INT LmCartoonMaker::InsertSnowSection( LmSection _Section )
{
	return TheTimeLine->InsertSnowSection(_Section);
}

INT LmCartoonMaker::RemoveSnowSection( UINT _uStartFrm )
{
	return TheTimeLine->RemoveSnowSection(_uStartFrm);
}

INT LmCartoonMaker::ModifySnowSection( UINT _uStartFrm, LPCWSTR _pcwsNewSnowName, UINT _uNewStartFrm, UINT _uNewTimeLength )
{
	return TheTimeLine->ModifySnowSection(_uStartFrm, _pcwsNewSnowName, _uNewStartFrm, _uNewTimeLength);
}

INT LmCartoonMaker::CheckTalkBoxSectionArea( LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uEndFrm )
{
	return TheTimeLine->CheckTalkBoxSectionArea(_pcwsCharacterName, _uStartFrm, _uEndFrm);
}

INT LmCartoonMaker::CheckTalkBoxSectionArea( LPCWSTR _pcwsCharacterName, UINT _uOldStartFrm, UINT _uStartFrm, UINT _uEndFrm )
{
	return TheTimeLine->CheckTalkBoxSectionArea(  _pcwsCharacterName,  _uOldStartFrm,  _uStartFrm,  _uEndFrm );
}
INT LmCartoonMaker::CheckSnowSectionArea( UINT _uStartFrm, UINT _uEndFrm )
{
	return TheTimeLine->CheckSnowSectionArea(_uStartFrm, _uEndFrm);
}

INT LmCartoonMaker::CheckSnowSectionArea( UINT _uOldStartFrm, UINT _uStartFrm, UINT _uEndFrm )
{
	return TheTimeLine->CheckSnowSectionArea(  _uOldStartFrm,  _uStartFrm,  _uEndFrm );
}

VOID LmCartoonMaker::SetMakeMode(BOOL _bMakingMode )
{
	m_bMakeMode = _bMakingMode;

	if(m_bMakeMode == TRUE)
	{
		SetRenderTimerOff();
		ResizeD3D(m_uMakeWidth, m_uMakeHeight);
		RenderOnce();
	}
	else
	{
		ResizeD3D(m_uVIewWidth, m_uViewHeight);
		SetRenderTimerOn();
	}
}

BOOL LmCartoonMaker::InsertAction( ACT _uActionType )
{
	// 현재 선택된 씬이 없으면 실패
	LmScene* pNowScene = TheSceneManager->GetNowScene();
	if(pNowScene == NULL)
		return FALSE;

	// 현재 선택된 캐릭터가 없으면 실패
	if(m_pGotChar == NULL)
		return FALSE;

	UINT uStartFrame = TheTimeLine->GetSPFrame(m_pGotChar->GetName());

	// 캐릭터가 이동할 경우 보드를 벗어나는지 체크한다.
	INT iSide = 0;
	INT iResult;
	// TimeLine UI 처리
	switch(_uActionType)
	{
	case ACT_TURN_LEFT:
	case ACT_TURN_RIGHT:
		iResult = TheTimeLine->PushActionSection(m_pGotChar->GetName(), 20);
		break;
	case ACT_WALK:
		if(!m_pGotChar->CanWalk())
			return FALSE;
	case ACT_SHAKE_HAND:
	case ACT_BOW:
	case ACT_JUMP:
	case ACT_TURNNING:
		iResult = TheTimeLine->PushActionSection(m_pGotChar->GetName(), 40);
		break;
	default:
		return FALSE;
	}

	if(iResult < 0)
		return FALSE;

	return pNowScene->InsertAction(m_pGotChar->GetName(), _uActionType, uStartFrame);
}

BOOL LmCartoonMaker::RemoveAction()
{
	// 현재 선택된 씬이 없으면 실패
	LmScene* pNowScene = TheSceneManager->GetNowScene();
	if(pNowScene == NULL)
		return FALSE;

	// 현재 선택된 캐릭터가 없으면 실패
	if(m_pGotChar == NULL)
		return FALSE;

	if(TheTimeLine->GetSPFrame(m_pGotChar->GetName()) == 1)
		return FALSE;

	LmSection lmSection;
	INT iResult = TheTimeLine->PopActionSection(m_pGotChar->GetName(), &lmSection);
	if(iResult != 0)
		return FALSE;

	return m_pGotChar->DeleteAction();
}

VOID LmCartoonMaker::SetStateOfUIBefore()
{
	m_enumStateOfUIBefore = m_enumStateOfUI;
}

LmSTATE_OF_UI LmCartoonMaker::GetStateOfUI()
{
	return m_enumStateOfUI;
}

VOID LmCartoonMaker::SetStateOfUI( LmSTATE_OF_UI _enumState )
{
	m_enumStateOfUI = _enumState;
}

VOID LmCartoonMaker::RenderOnce()
{
	if(m_bMakeMode)
	{
		// 카메라 시점 좌표 갱신
		static D3DXVECTOR3 vOldEye(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 vEye = *TheCartoonMaker->GetCameraEyePt();

		TheCartoonMaker->UpdateD3D(0.0f);
		TheCartoonMaker->RenderD3D();
	}
}

BOOL LmCartoonMaker::InsertSnowEffect( LPCWSTR _pcwsEffectName, UINT _uStartFrame, UINT _uEndFrame )
{
	LmScene* pNowScene = TheSceneManager->GetNowScene();
	if(pNowScene == NULL)
		return FALSE;

	if(TheTimeLine->InsertSnowSection(_pcwsEffectName, _uStartFrame, _uEndFrame) < 0)
		return FALSE;

	return pNowScene->InsertSnowKey(_uStartFrame, _uEndFrame);
}

BOOL LmCartoonMaker::RemoveSnowEffect( UINT _uStartFrame )
{
	LmScene* pNowScene = TheSceneManager->GetNowScene();
	if(pNowScene == NULL)
		return FALSE;

	if(TheTimeLine->RemoveSnowSection(_uStartFrame) < 0)
		return FALSE;

	return pNowScene->RemoveSnowKey(_uStartFrame);
}

BOOL LmCartoonMaker::ModifySnowEffect( UINT _uOldStartFrame, LPCWSTR _pcwsEffectName, UINT _uStartFrame, UINT _uEndFrame )
{
	LmScene* pNowScene = TheSceneManager->GetNowScene();
	if(pNowScene == NULL)
		return FALSE;

	if(TheCartoonMaker->ModifySnowSection(
		_uOldStartFrame, _pcwsEffectName, _uStartFrame, _uEndFrame) < 0)
		return FALSE;

	if(!pNowScene->RemoveSnowKey(_uOldStartFrame))
		return FALSE;

	if(!pNowScene->InsertSnowKey(_uStartFrame,_uEndFrame))
		return FALSE;

	return TRUE;
}

BOOL LmCartoonMaker::InsertFireworkEffect( UINT _uStartFrame )
{	
	LmScene* pNowScene = TheSceneManager->GetNowScene();
	if(pNowScene == NULL)
		return FALSE;

	if(TheTimeLine->InsertFireWorksSection(_uStartFrame) < 0)
		return FALSE;

	return pNowScene->InsertFireworkKey(_uStartFrame, _uStartFrame + FIREWORK_FRAME_SIZE -1);
}

BOOL LmCartoonMaker::RemoveFireworkEffect( UINT _uStartFrame )
{
	LmScene* pNowScene = TheSceneManager->GetNowScene();
	if(pNowScene == NULL)
		return FALSE;

	if(TheTimeLine->RemoveFireWorksSection(_uStartFrame) < 0)
		return FALSE;

	return pNowScene->RemoveFireworkKey(_uStartFrame);

}

BOOL LmCartoonMaker::CopyScene()
{
	LmScene* pNowScene = TheSceneManager->GetNowScene();
	if(pNowScene == NULL)
		return FALSE;

	return TheSceneManager->CopyScene(pNowScene->GetName());
}

BOOL LmCartoonMaker::CutScene()
{
	LmScene* pNowScene = TheSceneManager->GetNowScene();
	if(pNowScene == NULL)
		return FALSE;

	return TheSceneManager->CutScene(pNowScene->GetName());
}

BOOL LmCartoonMaker::PasteScene( LPCWSTR _pcwsScene )
{
	return TheSceneManager->PasteScene(_pcwsScene);

}

// [진우]
DWORD LmCartoonMaker::GetTotalFrame()
{
	return TheSceneManager->GetTotalFrame();
}


// [진우]
// wstring를 메모리 블럭에 저장해줌.
// 사용후 메모리 해제 필요.

BYTE* LmCartoonMaker::StringToMemory(std::wstring str, DWORD *dwSize)
{	

	BYTE *pBuf;
	*dwSize = (DWORD)str.length() * 2 + 2;
	pBuf = new BYTE[*dwSize];

	if (!pBuf)
		return NULL;

	memcpy(pBuf, str.c_str(), *dwSize);

	return pBuf;	
}


LPCWSTR LmCartoonMaker::GetCurrFileName()
{
	return m_wstrCurrFileName.c_str();
}

VOID LmCartoonMaker::SetCurrFileName( LPCWSTR _pcwsFileName )
{
	wstring wstrFileName = _pcwsFileName;
	m_wstrCurrFileName = wstrFileName;
}

VOID LmCartoonMaker::InitFileOpen()
{
	m_dataFileHeader.ClearHeader();
	m_wstrCurrFileName.clear();
}

VOID LmCartoonMaker::InitFileHeader()
{
	m_dataFileHeader.ClearHeader();
}

VOID LmCartoonMaker::SetSceneViewStart( UINT _uFilePoint )
{
	m_dataFileHeader.SetSceneViewStart(_uFilePoint);
}

VOID LmCartoonMaker::SetSceneViewEnd( UINT _uFilePoint )
{
	m_dataFileHeader.SetSceneViewEnd(_uFilePoint);
}

VOID LmCartoonMaker::SaveFileHeader( CFile &_File )
{
	_File.Seek(0, CFile::begin);
	m_dataFileHeader.SaveHeader(_File);
}

VOID LmCartoonMaker::OpenFileHeader( CFile &_File )
{
	_File.Seek(0, CFile::begin);
	m_dataFileHeader.OpenHeader(_File);
	
	//실제 파일이 저장된 곳으로 이동
	_File.Seek(FILE_HEADER_SIZE, CFile::begin);
}

// [진우]
// 현재 선택된 씬의 이름을 리턴
LPCWSTR LmCartoonMaker::GetNowSceneName()
{
	LmScene * pScene = TheSceneManager->GetNowScene();

	if (!pScene)
		return NULL;

	return pScene->GetName();
}

// [진우]
// 씬복사시 사운드 복사 기능
BOOL LmCartoonMaker::CopySound(LPCTSTR lpszDstName, LPCTSTR lpszSrcName)
{
	return TheSoundManager->CopySound(lpszDstName, lpszSrcName);	

}


VOID LmCartoonMaker::SaveSoundLowData( CFile &_File )
{
	UINT uFilePos = (UINT)_File.GetPosition();
	m_dataFileHeader.SetSoundLowDataStart(uFilePos);

	DWORD dwSize = 0;
	BYTE * pByteSoundLowData = TheSoundManager->SaveSound(&dwSize);
	_File.Write(&dwSize, sizeof(DWORD));
	_File.Write(pByteSoundLowData , dwSize);

	uFilePos = (UINT)_File.GetPosition();
	m_dataFileHeader.SetSoundLowDataEnd(uFilePos);

	//메모리 해제
	delete pByteSoundLowData;
}

BOOL LmCartoonMaker::LoadSound(BYTE *pData, DWORD dwSize)
{
	return TheSoundManager->LoadSound(pData, dwSize);
}

BOOL LmCartoonMaker::DeleteSceneSound(LPCWSTR _pcwSceneName)
{
	TheSoundManager->DeleteSceneSound(_pcwSceneName);
	return TRUE;
}
HANDLE* LmCartoonMaker::LmEvent(INT EventNum)
{
	static HANDLE g_hEvent[LM_MAXIMUM_EVENT];

	return &g_hEvent[EventNum];
}

DWORD LmCartoonMaker::GetEmptyEvent()
{		
	for (INT i = 0; i < LM_MAXIMUM_EVENT; i++){
		if (!(*LmEvent(i))){
			*LmEvent(i) = CreateEvent(NULL, FALSE, TRUE, NULL);
			return i;
		}
			
	}
	return NULL;
}


BOOL LmCartoonMaker::LmCloseEvent(INT EventNum)
{
	if (EventNum < 0 || EventNum >= LM_MAXIMUM_EVENT){
		return FALSE;
	}

	if (*LmEvent(EventNum)){
		CloseHandle(*LmEvent(EventNum));
		return TRUE;
	}

	return FALSE;
}


BOOL LmCartoonMaker::InsertTalkBox( LPCWSTR _pcwsChar, LPCWSTR _pcwsMessage, UINT _uStartFrame, UINT _uFrameSize, COLORREF _colorMsg )
{
	LmScene* pNowScene = TheSceneManager->GetNowScene();
	if(!pNowScene) 
		return FALSE;
	LmChar* pChar = pNowScene->GetChar(_pcwsChar);
	if(!pChar) 
		return FALSE;

	D3DXCOLOR dxColor = D3DCOLOR_XRGB(GetRValue(_colorMsg), GetGValue(_colorMsg), GetBValue(_colorMsg));

	if(!m_pGotChar->InsertTalkBox(_pcwsMessage, _uStartFrame, _uFrameSize, dxColor))
		return FALSE;

	if(TheTimeLine->InsertTalkBoxSection(pChar->GetName(), _uStartFrame , _uFrameSize) < 0)
		return FALSE;

	return TRUE;
}

BOOL LmCartoonMaker::InsertTalkBox( LPCWSTR _pcwsMessage, UINT _uStartFrame, UINT _uFrameSize, COLORREF _colorMsg )
{
	if(!m_pGotChar) 
		return FALSE;

	D3DXCOLOR dxColor = D3DCOLOR_XRGB(GetRValue(_colorMsg), GetGValue(_colorMsg), GetBValue(_colorMsg));
	
	if(TheTimeLine->InsertTalkBoxSection(m_pGotChar->GetName(), _uStartFrame , _uFrameSize)<0)
		return FALSE;

	if(!m_pGotChar->InsertTalkBox(_pcwsMessage, _uStartFrame, _uFrameSize, dxColor))
		return FALSE;

	return TRUE;
}

BOOL LmCartoonMaker::ModifyTalkBox( LPCWSTR _pcwsChar, LPCWSTR _pcwsMessage, UINT _uOldStartFrame, 
					UINT _uStartFrame, UINT _uFrameSize,  COLORREF _colorMsg)
{
	LmScene* pNowScene = TheSceneManager->GetNowScene();
	if(!pNowScene) 
		return FALSE;
	LmChar* pChar = pNowScene->GetChar(_pcwsChar);
	if(!pChar) 
		return FALSE;

	if(!RemoveTalkBox(pChar->GetName(), _uOldStartFrame))
		return FALSE;

	if(!InsertTalkBox(pChar->GetName(), _uStartFrame, _uFrameSize, _colorMsg))
		return FALSE;

	return TRUE;
}

BOOL LmCartoonMaker::RemoveTalkBox( LPCWSTR _pcwsChar, UINT _uStartFrame )
{
	LmScene* pNowScene = TheSceneManager->GetNowScene();
	if(!pNowScene) 
		return FALSE;
	LmChar* pChar = pNowScene->GetChar(_pcwsChar);
	if(!pChar) 
		return FALSE;

	if(TheTimeLine->RemoveTalkBoxSection(pChar->GetName(), _uStartFrame) < 0)
		return FALSE;

	if(!pChar->RemoveTalkBox(_uStartFrame))
		return FALSE;

	return TRUE;
}

BOOL LmCartoonMaker::GetTalkBoxColor( LPCWSTR _pcwsChar, UINT _uStartFrame, COLORREF& _refColor )
{
	LmScene* pNowScene = TheSceneManager->GetNowScene();
	if(!pNowScene) 
		return FALSE;
	LmChar* pChar = pNowScene->GetChar(_pcwsChar);
	if(!pChar) 
		return FALSE;

	D3DXCOLOR dxColor = pChar->GetTalkBoxColor(_uStartFrame);

	_refColor = RGB(dxColor.r * 255, dxColor.g * 255, dxColor.b * 255);

	return TRUE;
}


VOID LmCartoonMaker::SetSceneListStart(UINT _uFilePoint)
{
	m_dataFileHeader.SetSceneListStart(_uFilePoint);
}

VOID LmCartoonMaker::SetSceneListEnd(UINT _uFilePoint)
{
	m_dataFileHeader.SetSceneListEnd(_uFilePoint);
}

VOID LmCartoonMaker::LoadSoundLowData( CFile &_File )
{
	UINT uFilePos = m_dataFileHeader.GetSoundLowDataStart();
	_File.Seek(uFilePos, CFile::begin);

	DWORD dwSize = 0;
	_File.Read(&dwSize, sizeof(DWORD));
	
	BYTE* pByteSoundLowData = new BYTE[dwSize];
	_File.Read(pByteSoundLowData , dwSize);

	TheSoundManager->LoadSound(pByteSoundLowData, dwSize);

	//메모리 해제
	delete pByteSoundLowData;
}
