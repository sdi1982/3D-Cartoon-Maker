//--------------------------------------------------------------------------------------
// File: DXUTMisc.h
//
// Helper functions for Direct3D programming.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef DXUT_MISC_H
#define DXUT_MISC_H

#ifndef INT_MAX
#define INT_MAX       2147483647    /* maximum (signed) int value */
#endif
#ifndef UINT_MAX      
#define UINT_MAX      0xffffffff    /* maximum unsigned int value */
#endif


//--------------------------------------------------------------------------------------
// XInput 헬퍼 상태/함수
// 이것은 XInput 게임패드에서 데이터 처리를 사용하기 약간 더 편하게 만든다
// 
// 사용 예:
//
//      DXUT_GAMEPAD gamepad[4];
//      for( DWORD iPort=0; iPort<DXUT_MAX_CONTROLLERS; iPort++ )
//          DXUTGetGamepadState( iPort, gamepad[iPort] );
//
//--------------------------------------------------------------------------------------

#define DXUT_MAX_CONTROLLERS 4  // XInput handles up to 4 controllers 

struct DXUT_GAMEPAD
{
    // From XINPUT_GAMEPAD
    WORD    wButtons;
    BYTE    bLeftTrigger;
    BYTE    bRightTrigger;
    SHORT   sThumbLX;
    SHORT   sThumbLY;
    SHORT   sThumbRX;
    SHORT   sThumbRY;

	// 장치 속성
    XINPUT_CAPABILITIES caps;
    bool    bConnected; // If the controller is currently connected
    bool    bInserted;  // If the controller was inserted this frame
    bool    bRemoved;   // If the controller was removed this frame

	// 엄지 스틱의 값은 범위[-1,+1]로 가공된다.
    float   fThumbRX;
    float   fThumbRY;
    float   fThumbLX;
    float   fThumbLY;

	// 이 프레임에서 어떤 버튼이 눌려졌는지 기록한다.
	// 이것은 단지 버튼이 눌려진 첫번째 프레임을 설정한다
    WORD    wPressedButtons;
    bool    bPressedLeftTrigger;
    bool    bPressedRightTrigger;

	// 버튼의 마지막 상태
    WORD    wLastButtons;
    bool    bLastLeftTrigger;
    bool    bLastRightTrigger;
};

HRESULT DXUTGetGamepadState( DWORD dwPort, DXUT_GAMEPAD* pGamePad, bool bThumbstickDeadZone = true, bool bSnapThumbstickToCardinals = true );
HRESULT DXUTStopRumbleOnAllControllers();
void    DXUTEnableXInput( bool bEnable );


//--------------------------------------------------------------------------------------
// 성장 배열
//--------------------------------------------------------------------------------------
template< typename TYPE >
class CGrowableArray
{
public:
    CGrowableArray()  { m_pData = NULL; m_nSize = 0; m_nMaxSize = 0; }
    CGrowableArray( const CGrowableArray<TYPE>& a ) { for( int i=0; i < a.m_nSize; i++ ) Add( a.m_pData[i] ); }
    ~CGrowableArray() { RemoveAll(); }

    const TYPE& operator[]( int nIndex ) const { return GetAt( nIndex ); }
    TYPE& operator[]( int nIndex ) { return GetAt( nIndex ); }
   
    CGrowableArray& operator=( const CGrowableArray<TYPE>& a ) { if( this == &a ) return *this; RemoveAll(); for( int i=0; i < a.m_nSize; i++ ) Add( a.m_pData[i] ); return *this; }

    HRESULT SetSize( int nNewMaxSize );
    HRESULT Add( const TYPE& value );
    HRESULT Insert( int nIndex, const TYPE& value );
    HRESULT SetAt( int nIndex, const TYPE& value );
    TYPE&   GetAt( int nIndex ) { assert( nIndex >= 0 && nIndex < m_nSize ); return m_pData[nIndex]; }
    int     GetSize() const { return m_nSize; }
    TYPE*   GetData() { return m_pData; }
    bool    Contains( const TYPE& value ){ return ( -1 != IndexOf( value ) ); }

    int     IndexOf( const TYPE& value ) { return ( m_nSize > 0 ) ? IndexOf( value, 0, m_nSize ) : -1; }
    int     IndexOf( const TYPE& value, int iStart ) { return IndexOf( value, iStart, m_nSize - iStart ); }
    int     IndexOf( const TYPE& value, int nIndex, int nNumElements );

    int     LastIndexOf( const TYPE& value ) { return ( m_nSize > 0 ) ? LastIndexOf( value, m_nSize-1, m_nSize ) : -1; }
    int     LastIndexOf( const TYPE& value, int nIndex ) { return LastIndexOf( value, nIndex, nIndex+1 ); }
    int     LastIndexOf( const TYPE& value, int nIndex, int nNumElements );

    HRESULT Remove( int nIndex );
    void    RemoveAll() { SetSize(0); }

protected:
    TYPE* m_pData;      // 데이터의 실제 배열
    int m_nSize;        // 요소의 크기
    int m_nMaxSize;     // 할당 최대

    HRESULT SetSizeInternal( int nNewMaxSize );  // 이버전은 ctor 이나 dtor을 호출하지 않는다.
};


//--------------------------------------------------------------------------------------
// 타이머 동작을 수행한다.
// 전역 인스턴스를 획득하기위해 DXUTGetGlobalTimer()을 사용한다.
//--------------------------------------------------------------------------------------
class CDXUTTimer
{
public:
    CDXUTTimer();

    void Reset(); // 타이머를 리셋한다.
    void Start(); // 타이머를 시작한다.
    void Stop();  // 타이머를 멈춘다.(혹은 일시정지)
    void Advance(); // 0.1초의 advance the timer by 0.1 seconds
    double GetAbsoluteTime(); // get the absolute system time
    double GetTime(); // get the current time
    double GetElapsedTime(); // get the time that elapsed between Get*ElapsedTime() calls
    void GetTimeValues( double* pfTime, double* pfAbsoluteTime, float* pfElapsedTime ); // get all time values at once
    bool IsStopped(); // returns true if timer stopped

    // Limit the current thread to one processor (the current one). This ensures that timing code runs
    // on only one processor, and will not suffer any ill effects from power management.
    void LimitThreadAffinityToCurrentProc();

protected:
    LARGE_INTEGER GetAdjustedCurrentTime();

    bool m_bTimerStopped;
    LONGLONG m_llQPFTicksPerSec;
    
    LONGLONG m_llStopTime;
    LONGLONG m_llLastElapsedTime;
    LONGLONG m_llBaseTime;
};

CDXUTTimer* DXUTGetGlobalTimer();


//-----------------------------------------------------------------------------
// Resource cache for textures, fonts, meshs, and effects.  
// Use DXUTGetGlobalResourceCache() to access the global cache
//-----------------------------------------------------------------------------

enum DXUTCACHE_SOURCELOCATION { DXUTCACHE_LOCATION_FILE, DXUTCACHE_LOCATION_RESOURCE };

struct DXUTCache_Texture
{
    DXUTCACHE_SOURCELOCATION Location;
    WCHAR wszSource[MAX_PATH];
    HMODULE hSrcModule;
    UINT Width;
    UINT Height;
    UINT Depth;
    UINT MipLevels;
    DWORD Usage;
    D3DFORMAT Format;
    D3DPOOL Pool;
    D3DRESOURCETYPE Type;
    IDirect3DBaseTexture9 *pTexture;
};

struct DXUTCache_Font : public D3DXFONT_DESC
{
    ID3DXFont *pFont;
};

struct DXUTCache_Effect
{
    DXUTCACHE_SOURCELOCATION Location;
    WCHAR wszSource[MAX_PATH];
    HMODULE hSrcModule;
    DWORD dwFlags;
    ID3DXEffect *pEffect;
};


class CDXUTResourceCache
{
public:
    ~CDXUTResourceCache();

    HRESULT CreateTextureFromFile( LPDIRECT3DDEVICE9 pDevice, LPCWSTR pSrcFile, LPDIRECT3DTEXTURE9 *ppTexture );
    HRESULT CreateTextureFromFileEx( LPDIRECT3DDEVICE9 pDevice, LPCWSTR pSrcFile, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO *pSrcInfo, PALETTEENTRY *pPalette, LPDIRECT3DTEXTURE9 *ppTexture );
    HRESULT CreateTextureFromResource( LPDIRECT3DDEVICE9 pDevice, HMODULE hSrcModule, LPCWSTR pSrcResource, LPDIRECT3DTEXTURE9 *ppTexture );
    HRESULT CreateTextureFromResourceEx( LPDIRECT3DDEVICE9 pDevice, HMODULE hSrcModule, LPCWSTR pSrcResource, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO *pSrcInfo, PALETTEENTRY *pPalette, LPDIRECT3DTEXTURE9 *ppTexture );
    HRESULT CreateCubeTextureFromFile( LPDIRECT3DDEVICE9 pDevice, LPCWSTR pSrcFile, LPDIRECT3DCUBETEXTURE9 *ppCubeTexture );
    HRESULT CreateCubeTextureFromFileEx( LPDIRECT3DDEVICE9 pDevice, LPCWSTR pSrcFile, UINT Size, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO *pSrcInfo, PALETTEENTRY *pPalette, LPDIRECT3DCUBETEXTURE9 *ppCubeTexture );
    HRESULT CreateCubeTextureFromResource( LPDIRECT3DDEVICE9 pDevice, HMODULE hSrcModule, LPCWSTR pSrcResource, LPDIRECT3DCUBETEXTURE9 *ppCubeTexture );
    HRESULT CreateCubeTextureFromResourceEx( LPDIRECT3DDEVICE9 pDevice, HMODULE hSrcModule, LPCWSTR pSrcResource, UINT Size, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO *pSrcInfo, PALETTEENTRY *pPalette, LPDIRECT3DCUBETEXTURE9 *ppCubeTexture );
    HRESULT CreateVolumeTextureFromFile( LPDIRECT3DDEVICE9 pDevice, LPCWSTR pSrcFile, LPDIRECT3DVOLUMETEXTURE9 *ppVolumeTexture );
    HRESULT CreateVolumeTextureFromFileEx( LPDIRECT3DDEVICE9 pDevice, LPCWSTR pSrcFile, UINT Width, UINT Height, UINT Depth, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO *pSrcInfo, PALETTEENTRY *pPalette, LPDIRECT3DVOLUMETEXTURE9 *ppTexture );
    HRESULT CreateVolumeTextureFromResource( LPDIRECT3DDEVICE9 pDevice, HMODULE hSrcModule, LPCWSTR pSrcResource, LPDIRECT3DVOLUMETEXTURE9 *ppVolumeTexture );
    HRESULT CreateVolumeTextureFromResourceEx( LPDIRECT3DDEVICE9 pDevice, HMODULE hSrcModule, LPCWSTR pSrcResource, UINT Width, UINT Height, UINT Depth, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO *pSrcInfo, PALETTEENTRY *pPalette, LPDIRECT3DVOLUMETEXTURE9 *ppVolumeTexture );
    HRESULT CreateFont( LPDIRECT3DDEVICE9 pDevice, UINT Height, UINT Width, UINT Weight, UINT MipLevels, BOOL Italic, DWORD CharSet, DWORD OutputPrecision, DWORD Quality, DWORD PitchAndFamily, LPCWSTR pFacename, LPD3DXFONT *ppFont );
    HRESULT CreateFontIndirect( LPDIRECT3DDEVICE9 pDevice, CONST D3DXFONT_DESC *pDesc, LPD3DXFONT *ppFont );
    HRESULT CreateEffectFromFile( LPDIRECT3DDEVICE9 pDevice, LPCWSTR pSrcFile, const D3DXMACRO *pDefines, LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT *ppEffect, LPD3DXBUFFER *ppCompilationErrors );
    HRESULT CreateEffectFromResource( LPDIRECT3DDEVICE9 pDevice, HMODULE hSrcModule, LPCWSTR pSrcResource, const D3DXMACRO *pDefines, LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT *ppEffect, LPD3DXBUFFER *ppCompilationErrors );

public:
    HRESULT OnCreateDevice( IDirect3DDevice9 *pd3dDevice );
    HRESULT OnResetDevice( IDirect3DDevice9 *pd3dDevice );
    HRESULT OnLostDevice();
    HRESULT OnDestroyDevice();

protected:
    friend CDXUTResourceCache& DXUTGetGlobalResourceCache();
    friend HRESULT DXUTInitialize3DEnvironment();
    friend HRESULT DXUTReset3DEnvironment();
    friend void DXUTCleanup3DEnvironment( bool bReleaseSettings );

    CDXUTResourceCache() { }

    CGrowableArray< DXUTCache_Texture > m_TextureCache;
    CGrowableArray< DXUTCache_Effect > m_EffectCache;
    CGrowableArray< DXUTCache_Font > m_FontCache;
};

CDXUTResourceCache& DXUTGetGlobalResourceCache();


//--------------------------------------------------------------------------------------
class CD3DArcBall
{
public:
    CD3DArcBall();

	// 행위를 변화시키기위한 함수
    void Reset(); 
    void SetTranslationRadius( FLOAT fRadiusTranslation ) { m_fRadiusTranslation = fRadiusTranslation; }
    void SetWindow( INT nWidth, INT nHeight, FLOAT fRadius = 0.9f ) { m_nWidth = nWidth; m_nHeight = nHeight; m_fRadius = fRadius; m_vCenter = D3DXVECTOR2(m_nWidth/2.0f,m_nHeight/2.0f); }
    void SetOffset( INT nX, INT nY ) { m_Offset.x = nX; m_Offset.y = nY; }

	// 클라이언트로 부터 이것을 호출하고 새로운 회전 행렬을 읽기 위해 GetRotationMatrix()을 사용한다.
    void OnBegin( int nX, int nY );  // 회전을 시작한다.(현재 마우스 위치를 통과한다)
    void OnMove( int nX, int nY );   // 회전을 계속한다.(현재 마우스 위치를 통과한다)
    void OnEnd();                    // 회전을 끝낸다.

	// 그렇지 않으면, 왼쪽, 중심, 오른쪽 버튼을 자동으로 제어하기 위해 이것을 호출한다.
    LRESULT     HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// 상태를 획득/설정 함수
    const D3DXMATRIX* GetRotationMatrix()                   { return D3DXMatrixRotationQuaternion(&m_mRotation, &m_qNow); };
    const D3DXMATRIX* GetTranslationMatrix() const          { return &m_mTranslation; }
    const D3DXMATRIX* GetTranslationDeltaMatrix() const     { return &m_mTranslationDelta; }
    bool        IsBeingDragged() const                      { return m_bDrag; }
    D3DXQUATERNION GetQuatNow() const                       { return m_qNow; }
    void        SetQuatNow( D3DXQUATERNION q ) { m_qNow = q; }

    static D3DXQUATERNION QuatFromBallPoints( const D3DXVECTOR3 &vFrom, const D3DXVECTOR3 &vTo );


protected:
    D3DXMATRIXA16  m_mRotation;         // 아크볼 중심 행렬
    D3DXMATRIXA16  m_mTranslation;      // 아크볼 위치 행렬
    D3DXMATRIXA16  m_mTranslationDelta; // 아크볼 위치 행렬

    POINT          m_Offset;   // 윈도우 오프셋, 윈도우의 좌상모서리
    INT            m_nWidth;   // 아크볼의 윈도우 넓이
    INT            m_nHeight;  // 아크볼의 윈도우 높이
    D3DXVECTOR2    m_vCenter;  // 아크볼의 중심
    FLOAT          m_fRadius;  // 스크린 좌표의 아크볼 반지름
    FLOAT          m_fRadiusTranslation; // 목표를 이동하기위한 아크볼의 반지름

    D3DXQUATERNION m_qDown;             // 마우스 버튼 클릭 전 4원수
    D3DXQUATERNION m_qNow;              // 현재 드래그의 결합된 4원수 
    bool           m_bDrag;             // 유저가 아크 볼을 드래그 하고 있는가

    POINT          m_ptLastMouse;       // 마지막 마우스 위치 포인트
    D3DXVECTOR3    m_vDownPt;           // 회전 아크의 시작 포인트
    D3DXVECTOR3    m_vCurrentPt;        // 회전 아크의 현재 포인트

    D3DXVECTOR3    ScreenToVector( float fScreenPtX, float fScreenPtY );
};


//--------------------------------------------------------------------------------------
// WM_KEYDOWN 키 맵핑을 위해 CCamera에서 사용된다.
//--------------------------------------------------------------------------------------
enum D3DUtil_CameraKeys
{
    CAM_STRAFE_LEFT = 0,
    CAM_STRAFE_RIGHT,
    CAM_MOVE_FORWARD,
    CAM_MOVE_BACKWARD,
    CAM_MOVE_UP,
    CAM_MOVE_DOWN,
    CAM_RESET,
    CAM_CONTROLDOWN,
    CAM_MAX_KEYS,
    CAM_UNKNOWN = 0xFF
};

#define KEY_WAS_DOWN_MASK 0x80
#define KEY_IS_DOWN_MASK  0x01

#define MOUSE_LEFT_BUTTON   0x01
#define MOUSE_MIDDLE_BUTTON 0x02
#define MOUSE_RIGHT_BUTTON  0x04
#define MOUSE_WHEEL         0x08


//--------------------------------------------------------------------------------------
// 이동과 회전하는 단순한 기본 카메라 클래스.
// 이 기본 클래스는 상속된 클래스가 사용하기 위해 마우스와 키보드 입력을 기록하고,
// 공통 상태를 유지한다
//--------------------------------------------------------------------------------------
class CBaseCamera
{
public:
    CBaseCamera();

	// 클라이언트로 부터 이것들이 호출되어 새로운 행렬을 읽기위해 Get*Matrix()를 사용한다.
    virtual LRESULT HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual void    FrameMove( FLOAT fElapsedTime ) = 0;

	// 카메라 행렬을 바꾸는 함수
    virtual void Reset(); 
    virtual void SetViewParams( D3DXVECTOR3* pvEyePt, D3DXVECTOR3* pvLookatPt );
    virtual void SetProjParams( FLOAT fFOV, FLOAT fAspect, FLOAT fNearPlane, FLOAT fFarPlane );

	// 행동을 바꾸는 함수
    virtual void SetDragRect( RECT &rc ) { m_rcDrag = rc; }
    void SetInvertPitch( bool bInvertPitch ) { m_bInvertPitch = bInvertPitch; }
    void SetDrag( bool bMovementDrag, FLOAT fTotalDragTimeToZero = 0.25f ) { m_bMovementDrag = bMovementDrag; m_fTotalDragTimeToZero = fTotalDragTimeToZero; }
    void SetEnableYAxisMovement( bool bEnableYAxisMovement ) { m_bEnableYAxisMovement = bEnableYAxisMovement; }
    void SetEnablePositionMovement( bool bEnablePositionMovement ) { m_bEnablePositionMovement = bEnablePositionMovement; }
    void SetClipToBoundary( bool bClipToBoundary, D3DXVECTOR3* pvMinBoundary, D3DXVECTOR3* pvMaxBoundary ) { m_bClipToBoundary = bClipToBoundary; if( pvMinBoundary ) m_vMinBoundary = *pvMinBoundary; if( pvMaxBoundary ) m_vMaxBoundary = *pvMaxBoundary; }
    void SetScalers( FLOAT fRotationScaler = 0.01f, FLOAT fMoveScaler = 5.0f )  { m_fRotationScaler = fRotationScaler; m_fMoveScaler = fMoveScaler; }
    void SetNumberOfFramesToSmoothMouseData( int nFrames ) { if( nFrames > 0 ) m_fFramesToSmoothMouseData = (float)nFrames; }

	// 상태를 얻기위한 함수
    const D3DXMATRIX*  GetViewMatrix() const { return &m_mView; }
    const D3DXMATRIX*  GetProjMatrix() const { return &m_mProj; }
    const D3DXVECTOR3* GetEyePt() const      { return &m_vEye; }
    const D3DXVECTOR3* GetLookAtPt() const   { return &m_vLookAt; }
    float GetNearClip() const { return m_fNearPlane; }
    float GetFarClip() const { return m_fFarPlane; }

    bool IsBeingDragged() const         { return (m_bMouseLButtonDown || m_bMouseMButtonDown || m_bMouseRButtonDown); }
    bool IsMouseLButtonDown() const     { return m_bMouseLButtonDown; } 
    bool IsMouseMButtonDown() const     { return m_bMouseMButtonDown; } 
    bool IsMouseRButtonDown() const     { return m_bMouseRButtonDown; } 

protected:
	// WM_KEYDOWN를 D3DUtil_CameraKeys enum으로 맵핑하기 위한 함수들
    virtual D3DUtil_CameraKeys MapKey( UINT nKey );    
    bool IsKeyDown( BYTE key ) const { return( (key & KEY_IS_DOWN_MASK) == KEY_IS_DOWN_MASK ); }
    bool WasKeyDown( BYTE key ) const { return( (key & KEY_WAS_DOWN_MASK) == KEY_WAS_DOWN_MASK ); }

    void ConstrainToBoundary( D3DXVECTOR3* pV );
    void UpdateVelocity( float fElapsedTime );
    void GetInput( bool bGetKeyboardInput, bool bGetMouseInput, bool bGetGamepadInput, bool bResetCursorAfterMove );

    D3DXMATRIX            m_mView;              // 뷰 행렬
    D3DXMATRIX            m_mProj;              // 프로젝션 행렬

    DXUT_GAMEPAD          m_GamePad[DXUT_MAX_CONTROLLERS]; // XInput 컨트롤러 상태
    D3DXVECTOR3           m_vGamePadLeftThumb;
    D3DXVECTOR3           m_vGamePadRightThumb;
    double                m_GamePadLastActive[DXUT_MAX_CONTROLLERS];

    int                   m_cKeysDown;            // 눌려진 카메라 키의 수
    BYTE                  m_aKeys[CAM_MAX_KEYS];  // 입력의 상태 - KEY_WAS_DOWN_MASK|KEY_IS_DOWN_MASK
    D3DXVECTOR3           m_vKeyboardDirection;   // 키보드 입력의 방향 벡터
    POINT                 m_ptLastMousePosition;  // 마우스 커서의 마지막 절대 위치
    bool                  m_bMouseLButtonDown;    // 왼쪽 버튼이 눌려졌다면 True
    bool                  m_bMouseMButtonDown;    // 가운데 버튼이 눌려졌다면 True
    bool                  m_bMouseRButtonDown;    // 오른쪽 버튼이 눌려졌다면 True
    int                   m_nCurrentButtonMask;   // 어떤 버튼이 불려졌는지 마스크
    int                   m_nMouseWheelDelta;     // 가운데 휠 스크롤 수치 (+/-) 
    D3DXVECTOR2           m_vMouseDelta;          // 몇 프레임을 부드럽게 지나는 마우스관련 델타
    float                 m_fFramesToSmoothMouseData; // 마우스 데이터 부드럽게 지나는 프레임의 수

    D3DXVECTOR3           m_vDefaultEye;          // 기본 카메라 eye 위치
    D3DXVECTOR3           m_vDefaultLookAt;       // 기본 LookAt 위치
    D3DXVECTOR3           m_vEye;                 // 카메라 eye 위치
    D3DXVECTOR3           m_vLookAt;              // LookAt 위치
    float                 m_fCameraYawAngle;      // 카메라의 Yaw 각도
    float                 m_fCameraPitchAngle;    // 카메라의 Pitch 각도

    RECT                  m_rcDrag;               // 드래그해서 시작한 사각형
    D3DXVECTOR3           m_vVelocity;            // 카메라의 속도
    bool                  m_bMovementDrag;        // 만약 True이면, 카메라의 이동은 정지하기위해 느려질 것이다. 그렇지 않으면 순간에 움직인다.
    D3DXVECTOR3           m_vVelocityDrag;        // 드래그 힘의 속도
    FLOAT                 m_fDragTimer;           // 드래그를 적용하기 위한 카운트다운 타이머
    FLOAT                 m_fTotalDragTimeToZero; // 속도가 최고에서 0으로 되는 시간
    D3DXVECTOR2           m_vRotVelocity;         // 카메라의 속도

    float                 m_fFOV;                 // 시야(Field of view)
    float                 m_fAspect;              // Aspect ratio
    float                 m_fNearPlane;           // Near plane
    float                 m_fFarPlane;            // Far plane 

    float                 m_fRotationScaler;      // 회전 스칼라
    float                 m_fMoveScaler;          // 이동 스칼라

    bool                  m_bInvertPitch;         // Pitch축을 뒤집니다.
    bool                  m_bEnablePositionMovement; // 만약 true이면 사용자는 케마라와 모델을 이동할 수 있다.
    bool                  m_bEnableYAxisMovement; // 만약 true이면, 카메라는 y축에서 이동할 수 있다.

    bool                  m_bClipToBoundary;      // 만약 true이면, 카메라는 테두리로 클리핑할 것이다.
    D3DXVECTOR3           m_vMinBoundary;         // 클립 테두리의 최소 포인트
    D3DXVECTOR3           m_vMaxBoundary;         // 클립 테두리의 최대 포인트
};


//--------------------------------------------------------------------------------------
// 이동과 회전을하는 단순한 일인칭 카메라 클래스
// yaw, pitch는 가능하나 roll은 안된다. 키보드와 마우스 입력에 응답하기 위해 
// WM_KEYDOWN와 GetCursorPos() 사용하고 입력에 기반한 뷰 행렬를 업데이트한다.
//--------------------------------------------------------------------------------------
class CFirstPersonCamera : public CBaseCamera
{
public:
    CFirstPersonCamera();

	// 클라이언트로부터 이것을 호출하여 새로운 행렬을 읽기위해 Get*Matrix()를 사용한다.
    virtual void FrameMove( FLOAT fElapsedTime );

	// 행위를 변화시키는 함수
    void SetRotateButtons( bool bLeft, bool bMiddle, bool bRight, bool bRotateWithoutButtonDown = false );

    void SetResetCursorAfterMove( bool bResetCursorAfterMove ) { m_bResetCursorAfterMove = bResetCursorAfterMove; }

    // 상태를 얻는 함수
    D3DXMATRIX*  GetWorldMatrix()            { return &m_mCameraWorld; }

    const D3DXVECTOR3* GetWorldRight() const { return (D3DXVECTOR3*)&m_mCameraWorld._11; } 
    const D3DXVECTOR3* GetWorldUp() const    { return (D3DXVECTOR3*)&m_mCameraWorld._21; }
    const D3DXVECTOR3* GetWorldAhead() const { return (D3DXVECTOR3*)&m_mCameraWorld._31; }
    const D3DXVECTOR3* GetEyePt() const      { return (D3DXVECTOR3*)&m_mCameraWorld._41; }

protected:
    D3DXMATRIX m_mCameraWorld;       // 카메라의 월드 행렬(뷰 행렬의 역행렬)

    int        m_nActiveButtonMask;  // 버튼이 회전이 가능한지 결정하는 마스크
    bool       m_bRotateWithoutButtonDown;

    bool       m_bResetCursorAfterMove;// 만약 true이면, 클래스는 커서 위치를 리셋할 것이다, 그래서 커서는 항상 이동할 공간이 있다.
};


//--------------------------------------------------------------------------------------
// 물체 주위를 회전하는 간단한 모델 뷰 카메라 클래스
//--------------------------------------------------------------------------------------
class CModelViewerCamera : public CBaseCamera
{
public:
    CModelViewerCamera();

	// 클라이언트로 부터 이것을 호출하여 새로운 행렬을 읽기위해 Get*Matrix()를 사용한다.
    virtual LRESULT HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual void FrameMove( FLOAT fElapsedTime );

   
	// 행위를 변경하기 위한 함수들
    virtual void SetDragRect( RECT &rc );
    void Reset(); 
    void SetViewParams( D3DXVECTOR3* pvEyePt, D3DXVECTOR3* pvLookatPt );
    void SetButtonMasks( int nRotateModelButtonMask = MOUSE_LEFT_BUTTON, int nZoomButtonMask = MOUSE_WHEEL, int nRotateCameraButtonMask = MOUSE_RIGHT_BUTTON ) { m_nRotateModelButtonMask = nRotateModelButtonMask, m_nZoomButtonMask = nZoomButtonMask; m_nRotateCameraButtonMask = nRotateCameraButtonMask; }
    
    void SetAttachCameraToModel( bool bEnable = false ) { m_bAttachCameraToModel = bEnable; }
    void SetWindow( int nWidth, int nHeight, float fArcballRadius=0.9f ) { m_WorldArcBall.SetWindow( nWidth, nHeight, fArcballRadius ); m_ViewArcBall.SetWindow( nWidth, nHeight, fArcballRadius ); }
    void SetRadius( float fDefaultRadius=5.0f, float fMinRadius=1.0f, float fMaxRadius=FLT_MAX  ) { m_fDefaultRadius = m_fRadius = fDefaultRadius; m_fMinRadius = fMinRadius; m_fMaxRadius = fMaxRadius; m_bDragSinceLastUpdate = true; }
    void SetModelCenter( D3DXVECTOR3 vModelCenter ) { m_vModelCenter = vModelCenter; }
    void SetLimitPitch( bool bLimitPitch ) { m_bLimitPitch = bLimitPitch; }
    void SetViewQuat( D3DXQUATERNION q ) { m_ViewArcBall.SetQuatNow( q ); m_bDragSinceLastUpdate = true; }
    void SetWorldQuat( D3DXQUATERNION q ) { m_WorldArcBall.SetQuatNow( q ); m_bDragSinceLastUpdate = true; }

    // 상태를 얻기위한 함수들
    const D3DXMATRIX* GetWorldMatrix() const { return &m_mWorld; }
    void SetWorldMatrix( D3DXMATRIX &mWorld ) { m_mWorld = mWorld; m_bDragSinceLastUpdate = true; }

protected:
    CD3DArcBall  m_WorldArcBall;
    CD3DArcBall  m_ViewArcBall;
    D3DXVECTOR3  m_vModelCenter;
    D3DXMATRIX   m_mModelLastRot;        // 모델의 마지막 아크볼 회전 행렬
    D3DXMATRIX   m_mModelRot;            // 모델의 회전 행렬
    D3DXMATRIX   m_mWorld;               // 모델의 월드 행렬

    int          m_nRotateModelButtonMask;
    int          m_nZoomButtonMask;
    int          m_nRotateCameraButtonMask;

    bool         m_bAttachCameraToModel;
    bool         m_bLimitPitch;
    float        m_fRadius;              // 카메라로부터 모델과의 거리
    float        m_fDefaultRadius;       // 카메라로부터 모델과의 기본 거리
    float        m_fMinRadius;           // 최소 반지름
    float        m_fMaxRadius;           // 최대 반지름
    bool         m_bDragSinceLastUpdate; // 최근의 FrameMove가 호출된 후 부터 마우스 드래그가 일어났다면 true

    D3DXMATRIX   m_mCameraRotLast;

};


//--------------------------------------------------------------------------------------
// 텍스트가 그려질 때 삽입될 위치를 관리한다
//--------------------------------------------------------------------------------------
class CDXUTTextHelper
{
public:
    CDXUTTextHelper( ID3DXFont* pFont, ID3DXSprite* pSprite, int nLineHeight );

    void SetInsertionPos( int x, int y ) { m_pt.x = x; m_pt.y = y; }
    void SetForegroundColor( D3DXCOLOR clr ) { m_clr = clr; }

    void Begin();
    HRESULT DrawFormattedTextLine( const WCHAR* strMsg, ... );
    HRESULT DrawTextLine( const WCHAR* strMsg );
    HRESULT DrawFormattedTextLine( RECT &rc, DWORD dwFlags, const WCHAR* strMsg, ... );
    HRESULT DrawTextLine( RECT &rc, DWORD dwFlags, const WCHAR* strMsg );
    void End();

protected:
    ID3DXFont*   m_pFont;
    ID3DXSprite* m_pSprite;
    D3DXCOLOR    m_clr;
    POINT        m_pt;
    int          m_nLineHeight;
};


//--------------------------------------------------------------------------------------
// 영구적인 라인의 목록 관리하고 ID3DXLine을 사용하여 그것들을 그린다.
//--------------------------------------------------------------------------------------
class CDXUTLineManager
{
public:
    CDXUTLineManager();
    ~CDXUTLineManager();

    HRESULT OnCreatedDevice( IDirect3DDevice9* pd3dDevice );
    HRESULT OnResetDevice();
    HRESULT OnRender();
    HRESULT OnLostDevice();
    HRESULT OnDeletedDevice();

    HRESULT AddLine( int* pnLineID, D3DXVECTOR2* pVertexList, DWORD dwVertexListCount, D3DCOLOR Color, float fWidth, float fScaleRatio, bool bAntiAlias );
    HRESULT AddRect( int* pnLineID, RECT rc, D3DCOLOR Color, float fWidth, float fScaleRatio, bool bAntiAlias );
    HRESULT RemoveLine( int nLineID );
    HRESULT RemoveAllLines();

protected:
    struct LINE_NODE
    {
        int      nLineID;
        D3DCOLOR Color;
        float    fWidth;
        bool     bAntiAlias;
        float    fScaleRatio;
        D3DXVECTOR2* pVertexList;
        DWORD    dwVertexListCount;
    };

    CGrowableArray<LINE_NODE*> m_LinesList;
    IDirect3DDevice9* m_pd3dDevice;
    ID3DXLine* m_pD3DXLine;
};


//--------------------------------------------------------------------------------------
// 아크볼에 의해 제어되어 반지름 주위를 회전하는 양방향 메시와 방향, 마우스의 
// 이벤트를 관리한다.
//--------------------------------------------------------------------------------------
class CDXUTDirectionWidget
{
public:
    CDXUTDirectionWidget();

    static HRESULT StaticOnCreateDevice( IDirect3DDevice9* pd3dDevice );
    HRESULT OnResetDevice( const D3DSURFACE_DESC* pBackBufferSurfaceDesc );
    HRESULT OnRender( D3DXCOLOR color, const D3DXMATRIX* pmView, const D3DXMATRIX* pmProj, const D3DXVECTOR3* pEyePt );
    LRESULT HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    static void StaticOnLostDevice();
    static void StaticOnDestroyDevice();

    D3DXVECTOR3 GetLightDirection()         { return m_vCurrentDir; };
    void        SetLightDirection( D3DXVECTOR3 vDir ) { m_vDefaultDir = m_vCurrentDir = vDir; };
    void        SetButtonMask( int nRotate = MOUSE_RIGHT_BUTTON ) { m_nRotateMask = nRotate; }

    float GetRadius()                 { return m_fRadius; };
    void  SetRadius( float fRadius )  { m_fRadius = fRadius; };

    bool  IsBeingDragged() { return m_ArcBall.IsBeingDragged(); };

protected:
    HRESULT UpdateLightDir();

    D3DXMATRIXA16  m_mRot;
    D3DXMATRIXA16  m_mRotSnapshot;
    static IDirect3DDevice9* s_pd3dDevice;
    static ID3DXEffect* s_pEffect;       
    static ID3DXMesh*   s_pMesh;    

    float          m_fRadius;
    int            m_nRotateMask;
    CD3DArcBall    m_ArcBall;
    D3DXVECTOR3    m_vDefaultDir;
    D3DXVECTOR3    m_vCurrentDir;
    D3DXMATRIX     m_mView;
};


//--------------------------------------------------------------------------------------
// 만약 그들이 REF 장치나 종료를 원한다면 사용자에게 요청하는 예제의 코드가 공유된다.
//--------------------------------------------------------------------------------------
void DXUTDisplaySwitchingToREFWarning();

//--------------------------------------------------------------------------------------
// 공통 위치에서 검색으로 미디어 파일을 찾기를 시도한다.
//--------------------------------------------------------------------------------------
HRESULT DXUTFindDXSDKMediaFileCch( WCHAR* strDestPath, int cchDest, LPCWSTR strFilename );
HRESULT DXUTSetMediaSearchPath( LPCWSTR strPath );
LPCWSTR DXUTGetMediaSearchPath();


//--------------------------------------------------------------------------------------
// 주어진 D3DFORMAT을 위한 문자열을 리턴한다.
// bWithPrefix는 문자열이 "D3DFMT_"을 포함해야하는지 결정한다.
//--------------------------------------------------------------------------------------
LPCWSTR DXUTD3DFormatToString( D3DFORMAT format, bool bWithPrefix );


//--------------------------------------------------------------------------------------
// 큐브맵의 면을 랜더링할 뷰 행렬을 리턴한다.
//--------------------------------------------------------------------------------------
D3DXMATRIX DXUTGetCubeMapViewMatrix( DWORD dwFace );


//--------------------------------------------------------------------------------------
// 프로그램이 종료된 후에 미디어 센터 UI를 실행할 헬퍼 함수
//--------------------------------------------------------------------------------------
bool DXUTReLaunchMediaCenter();


//--------------------------------------------------------------------------------------
// 디버그 출력 지원
// 디버그 프린팅 지원을 위해 dxerr.h을 보라
//--------------------------------------------------------------------------------------
void DXUTOutputDebugStringW( LPCWSTR strMsg, ... );
void DXUTOutputDebugStringA( LPCSTR strMsg, ... );
HRESULT WINAPI DXUTTrace( const CHAR* strFile, DWORD dwLine, HRESULT hr, const WCHAR* strMsg, bool bPopMsgBox );
void DXUTTraceDecl( D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE] );
WCHAR* DXUTTraceD3DDECLUSAGEtoString( BYTE u );
WCHAR* DXUTTraceD3DDECLMETHODtoString( BYTE m );
WCHAR* DXUTTraceD3DDECLTYPEtoString( BYTE t );

#ifdef UNICODE
#define DXUTOutputDebugString DXUTOutputDebugStringW
#else
#define DXUTOutputDebugString DXUTOutputDebugStringA
#endif

// 이러한 매크로들은 dxerr들 과 매우 비슷하다. 하지만 이것은 특별한 경우의 HRESULT는 
// 더나은 메세지 박스를 보여주는 DXUT에 의해 정의된다.

#if defined(DEBUG) || defined(_DEBUG)
#define DXUT_ERR(str,hr)           DXUTTrace( __FILE__, (DWORD)__LINE__, hr, str, false )
#define DXUT_ERR_MSGBOX(str,hr)    DXUTTrace( __FILE__, (DWORD)__LINE__, hr, str, true )
#define DXUTTRACE                  DXUTOutputDebugString
#else
#define DXUT_ERR(str,hr)           (hr)
#define DXUT_ERR_MSGBOX(str,hr)    (hr)
#define DXUTTRACE                  (__noop)
#endif


//--------------------------------------------------------------------------------------
// Direct3D9 동적 링킹 지원 -- API가 표현하지 못한다면 우아한 실패를 하는 
// top-level D3D9 API들을 호출한다. 
//--------------------------------------------------------------------------------------

IDirect3D9 * WINAPI DXUT_Dynamic_Direct3DCreate9(UINT SDKVersion);
int WINAPI DXUT_Dynamic_D3DPERF_BeginEvent( D3DCOLOR col, LPCWSTR wszName );
int WINAPI DXUT_Dynamic_D3DPERF_EndEvent( void );
void WINAPI DXUT_Dynamic_D3DPERF_SetMarker( D3DCOLOR col, LPCWSTR wszName );
void WINAPI DXUT_Dynamic_D3DPERF_SetRegion( D3DCOLOR col, LPCWSTR wszName );
BOOL WINAPI DXUT_Dynamic_D3DPERF_QueryRepeatFrame( void );
void WINAPI DXUT_Dynamic_D3DPERF_SetOptions( DWORD dwOptions );
DWORD WINAPI DXUT_Dynamic_D3DPERF_GetStatus( void );


//--------------------------------------------------------------------------------------
// 프로파일링/계측 지원
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Some D3DPERF APIs take a color that can be used when displaying user events in 
// performance analysis tools.  The following constants are provided for your 
// convenience, but you can use any colors you like.
//--------------------------------------------------------------------------------------
const D3DCOLOR DXUT_PERFEVENTCOLOR  = D3DCOLOR_XRGB(200,100,100);
const D3DCOLOR DXUT_PERFEVENTCOLOR2 = D3DCOLOR_XRGB(100,200,100);
const D3DCOLOR DXUT_PERFEVENTCOLOR3 = D3DCOLOR_XRGB(100,100,200);

//--------------------------------------------------------------------------------------
// The following macros provide a convenient way for your code to call the D3DPERF 
// functions only when PROFILE is defined.  If PROFILE is not defined (as for the final 
// release version of a program), these macros evaluate to nothing, so no detailed event
// information is embedded in your shipping program.  It is recommended that you create
// and use three build configurations for your projects:
//     Debug (nonoptimized code, asserts active, PROFILE defined to assist debugging)
//     Profile (optimized code, asserts disabled, PROFILE defined to assist optimization)
//     Release (optimized code, asserts disabled, PROFILE not defined)
//--------------------------------------------------------------------------------------
#ifdef PROFILE
// PROFILE is defined, so these macros call the D3DPERF functions
#define DXUT_BeginPerfEvent( color, pstrMessage )   DXUT_Dynamic_D3DPERF_BeginEvent( color, pstrMessage )
#define DXUT_EndPerfEvent()                         DXUT_Dynamic_D3DPERF_EndEvent()
#define DXUT_SetPerfMarker( color, pstrMessage )    DXUT_Dynamic_D3DPERF_SetMarker( color, pstrMessage )
#else
// PROFILE is not defined, so these macros do nothing
#define DXUT_BeginPerfEvent( color, pstrMessage )   (__noop)
#define DXUT_EndPerfEvent()                         (__noop)
#define DXUT_SetPerfMarker( color, pstrMessage )    (__noop)
#endif

//--------------------------------------------------------------------------------------
// CDXUTPerfEventGenerator is a helper class that makes it easy to attach begin and end
// events to a block of code.  Simply define a CDXUTPerfEventGenerator variable anywhere 
// in a block of code, and the class's constructor will call DXUT_BeginPerfEvent when 
// the block of code begins, and the class's destructor will call DXUT_EndPerfEvent when 
// the block ends.
//--------------------------------------------------------------------------------------
class CDXUTPerfEventGenerator
{
public:
    CDXUTPerfEventGenerator( D3DCOLOR color, LPCWSTR pstrMessage ) { DXUT_BeginPerfEvent( color, pstrMessage ); }
    ~CDXUTPerfEventGenerator( void ) { DXUT_EndPerfEvent(); }
};


//--------------------------------------------------------------------------------------
// Multimon handling to support OSes with or without multimon API support.  
// Purposely avoiding the use of multimon.h so DXUT.lib doesn't require 
// COMPILE_MULTIMON_STUBS and cause complication with MFC or other users of multimon.h
//--------------------------------------------------------------------------------------
#ifndef MONITOR_DEFAULTTOPRIMARY
    #define MONITORINFOF_PRIMARY        0x00000001
    #define MONITOR_DEFAULTTONULL       0x00000000
    #define MONITOR_DEFAULTTOPRIMARY    0x00000001
    #define MONITOR_DEFAULTTONEAREST    0x00000002
    typedef struct tagMONITORINFO
    {
        DWORD   cbSize;
        RECT    rcMonitor;
        RECT    rcWork;
        DWORD   dwFlags;
    } MONITORINFO, *LPMONITORINFO;
    typedef struct tagMONITORINFOEXW : public tagMONITORINFO
    {
        WCHAR       szDevice[CCHDEVICENAME];
    } MONITORINFOEXW, *LPMONITORINFOEXW;
    typedef MONITORINFOEXW MONITORINFOEX;
    typedef LPMONITORINFOEXW LPMONITORINFOEX;
#endif

HMONITOR DXUTMonitorFromWindow( HWND hWnd, DWORD dwFlags );
BOOL     DXUTGetMonitorInfo( HMONITOR hMonitor, LPMONITORINFO lpMonitorInfo );
void     DXUTGetDesktopResolution( UINT AdapterOrdinal, UINT* pWidth, UINT* pHeight );


//--------------------------------------------------------------------------------------
// 성장배열의 구현
//--------------------------------------------------------------------------------------

// 이버전은 ctor 이나 dtor을 호출하지 않는다.
template< typename TYPE >
HRESULT CGrowableArray<TYPE>::SetSizeInternal( int nNewMaxSize )
{
    if( nNewMaxSize < 0 )
    {
        assert( false );
        return E_INVALIDARG;
    }

    if( nNewMaxSize == 0 )
    {
        // Shrink to 0 size & cleanup
        if( m_pData )
        {
            free( m_pData );
            m_pData = NULL;
        }

        m_nMaxSize = 0;
        m_nSize = 0;
    }
    else if( m_pData == NULL || nNewMaxSize > m_nMaxSize )
    {
        // Grow array
        int nGrowBy = ( m_nMaxSize == 0 ) ? 16 : m_nMaxSize;

        // Limit nGrowBy to keep m_nMaxSize less than INT_MAX
        if( (UINT)m_nMaxSize + (UINT)nGrowBy > (UINT)INT_MAX )
            nGrowBy = INT_MAX - m_nMaxSize;

        nNewMaxSize = __max( nNewMaxSize, m_nMaxSize + nGrowBy );

        // Verify that (nNewMaxSize * sizeof(TYPE)) is not greater than UINT_MAX or the realloc will overrun
        if( sizeof(TYPE) > UINT_MAX / (UINT)nNewMaxSize )
            return E_INVALIDARG;

        TYPE* pDataNew = (TYPE*) realloc( m_pData, nNewMaxSize * sizeof(TYPE) );
        if( pDataNew == NULL )
            return E_OUTOFMEMORY;

        m_pData = pDataNew;
        m_nMaxSize = nNewMaxSize;
    }

    return S_OK;
}


//--------------------------------------------------------------------------------------
template< typename TYPE >
HRESULT CGrowableArray<TYPE>::SetSize( int nNewMaxSize )
{
    int nOldSize = m_nSize;

    if( nOldSize > nNewMaxSize )
    {
        // Removing elements. Call dtor.

        for( int i = nNewMaxSize; i < nOldSize; ++i )
            m_pData[i].~TYPE();
    }

    // Adjust buffer.  Note that there's no need to check for error
    // since if it happens, nOldSize == nNewMaxSize will be true.)
    HRESULT hr = SetSizeInternal( nNewMaxSize );

    if( nOldSize < nNewMaxSize )
    {
        // Adding elements. Call ctor.

        for( int i = nOldSize; i < nNewMaxSize; ++i )
            ::new (&m_pData[i]) TYPE;
    }

    return hr;
}


//--------------------------------------------------------------------------------------
template< typename TYPE >
HRESULT CGrowableArray<TYPE>::Add( const TYPE& value )
{
    HRESULT hr;
    if( FAILED( hr = SetSizeInternal( m_nSize + 1 ) ) )
        return hr;

    // Construct the new element
    ::new (&m_pData[m_nSize]) TYPE;

    // Assign
    m_pData[m_nSize] = value;
    ++m_nSize;

    return S_OK;
}


//--------------------------------------------------------------------------------------
template< typename TYPE >
HRESULT CGrowableArray<TYPE>::Insert( int nIndex, const TYPE& value )
{
    HRESULT hr;

    // Validate index
    if( nIndex < 0 || 
        nIndex > m_nSize )
    {
        assert( false );
        return E_INVALIDARG;
    }

    // Prepare the buffer
    if( FAILED( hr = SetSizeInternal( m_nSize + 1 ) ) )
        return hr;

    // Shift the array
    MoveMemory( &m_pData[nIndex+1], &m_pData[nIndex], sizeof(TYPE) * (m_nSize - nIndex) );

    // Construct the new element
    ::new (&m_pData[nIndex]) TYPE;

    // Set the value and increase the size
    m_pData[nIndex] = value;
    ++m_nSize;

    return S_OK;
}


//--------------------------------------------------------------------------------------
template< typename TYPE >
HRESULT CGrowableArray<TYPE>::SetAt( int nIndex, const TYPE& value )
{
    // Validate arguments
    if( nIndex < 0 ||
        nIndex >= m_nSize )
    {
        assert( false );
        return E_INVALIDARG;
    }

    m_pData[nIndex] = value;
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Searches for the specified value and returns the index of the first occurrence
// within the section of the data array that extends from iStart and contains the 
// specified number of elements. Returns -1 if value is not found within the given 
// section.
//--------------------------------------------------------------------------------------
template< typename TYPE >
int CGrowableArray<TYPE>::IndexOf( const TYPE& value, int iStart, int nNumElements )
{
    // Validate arguments
    if( iStart < 0 || 
        iStart >= m_nSize ||
        nNumElements < 0 ||
        iStart + nNumElements > m_nSize )
    {
        assert( false );
        return -1;
    }

    // Search
    for( int i = iStart; i < (iStart + nNumElements); i++ )
    {
        if( value == m_pData[i] )
            return i;
    }

    // Not found
    return -1;
}


//--------------------------------------------------------------------------------------
// Searches for the specified value and returns the index of the last occurrence
// within the section of the data array that contains the specified number of elements
// and ends at iEnd. Returns -1 if value is not found within the given section.
//--------------------------------------------------------------------------------------
template< typename TYPE >
int CGrowableArray<TYPE>::LastIndexOf( const TYPE& value, int iEnd, int nNumElements )
{
    // Validate arguments
    if( iEnd < 0 || 
        iEnd >= m_nSize ||
        nNumElements < 0 ||
        iEnd - nNumElements < 0 )
    {
        assert( false );
        return -1;
    }

    // Search
    for( int i = iEnd; i > (iEnd - nNumElements); i-- )
    {
        if( value == m_pData[i] )
            return i;
    }

    // Not found
    return -1;
}



//--------------------------------------------------------------------------------------
template< typename TYPE >
HRESULT CGrowableArray<TYPE>::Remove( int nIndex )
{
    if( nIndex < 0 || 
        nIndex >= m_nSize )
    {
        assert( false );
        return E_INVALIDARG;
    }

    // Destruct the element to be removed
    m_pData[nIndex].~TYPE();

    // Compact the array and decrease the size
    MoveMemory( &m_pData[nIndex], &m_pData[nIndex+1], sizeof(TYPE) * (m_nSize - (nIndex+1)) );
    --m_nSize;

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Creates a REF or NULLREF device and returns that device.  The caller should call
// Release() when done with the device.
//--------------------------------------------------------------------------------------
IDirect3DDevice9* DXUTCreateRefDevice( HWND hWnd, bool bNullRef = true );


#endif
