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
// XInput ���� ����/�Լ�
// �̰��� XInput �����е忡�� ������ ó���� ����ϱ� �ణ �� ���ϰ� �����
// 
// ��� ��:
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

	// ��ġ �Ӽ�
    XINPUT_CAPABILITIES caps;
    bool    bConnected; // If the controller is currently connected
    bool    bInserted;  // If the controller was inserted this frame
    bool    bRemoved;   // If the controller was removed this frame

	// ���� ��ƽ�� ���� ����[-1,+1]�� �����ȴ�.
    float   fThumbRX;
    float   fThumbRY;
    float   fThumbLX;
    float   fThumbLY;

	// �� �����ӿ��� � ��ư�� ���������� ����Ѵ�.
	// �̰��� ���� ��ư�� ������ ù��° �������� �����Ѵ�
    WORD    wPressedButtons;
    bool    bPressedLeftTrigger;
    bool    bPressedRightTrigger;

	// ��ư�� ������ ����
    WORD    wLastButtons;
    bool    bLastLeftTrigger;
    bool    bLastRightTrigger;
};

HRESULT DXUTGetGamepadState( DWORD dwPort, DXUT_GAMEPAD* pGamePad, bool bThumbstickDeadZone = true, bool bSnapThumbstickToCardinals = true );
HRESULT DXUTStopRumbleOnAllControllers();
void    DXUTEnableXInput( bool bEnable );


//--------------------------------------------------------------------------------------
// ���� �迭
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
    TYPE* m_pData;      // �������� ���� �迭
    int m_nSize;        // ����� ũ��
    int m_nMaxSize;     // �Ҵ� �ִ�

    HRESULT SetSizeInternal( int nNewMaxSize );  // �̹����� ctor �̳� dtor�� ȣ������ �ʴ´�.
};


//--------------------------------------------------------------------------------------
// Ÿ�̸� ������ �����Ѵ�.
// ���� �ν��Ͻ��� ȹ���ϱ����� DXUTGetGlobalTimer()�� ����Ѵ�.
//--------------------------------------------------------------------------------------
class CDXUTTimer
{
public:
    CDXUTTimer();

    void Reset(); // Ÿ�̸Ӹ� �����Ѵ�.
    void Start(); // Ÿ�̸Ӹ� �����Ѵ�.
    void Stop();  // Ÿ�̸Ӹ� �����.(Ȥ�� �Ͻ�����)
    void Advance(); // 0.1���� advance the timer by 0.1 seconds
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

	// ������ ��ȭ��Ű������ �Լ�
    void Reset(); 
    void SetTranslationRadius( FLOAT fRadiusTranslation ) { m_fRadiusTranslation = fRadiusTranslation; }
    void SetWindow( INT nWidth, INT nHeight, FLOAT fRadius = 0.9f ) { m_nWidth = nWidth; m_nHeight = nHeight; m_fRadius = fRadius; m_vCenter = D3DXVECTOR2(m_nWidth/2.0f,m_nHeight/2.0f); }
    void SetOffset( INT nX, INT nY ) { m_Offset.x = nX; m_Offset.y = nY; }

	// Ŭ���̾�Ʈ�� ���� �̰��� ȣ���ϰ� ���ο� ȸ�� ����� �б� ���� GetRotationMatrix()�� ����Ѵ�.
    void OnBegin( int nX, int nY );  // ȸ���� �����Ѵ�.(���� ���콺 ��ġ�� ����Ѵ�)
    void OnMove( int nX, int nY );   // ȸ���� ����Ѵ�.(���� ���콺 ��ġ�� ����Ѵ�)
    void OnEnd();                    // ȸ���� ������.

	// �׷��� ������, ����, �߽�, ������ ��ư�� �ڵ����� �����ϱ� ���� �̰��� ȣ���Ѵ�.
    LRESULT     HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// ���¸� ȹ��/���� �Լ�
    const D3DXMATRIX* GetRotationMatrix()                   { return D3DXMatrixRotationQuaternion(&m_mRotation, &m_qNow); };
    const D3DXMATRIX* GetTranslationMatrix() const          { return &m_mTranslation; }
    const D3DXMATRIX* GetTranslationDeltaMatrix() const     { return &m_mTranslationDelta; }
    bool        IsBeingDragged() const                      { return m_bDrag; }
    D3DXQUATERNION GetQuatNow() const                       { return m_qNow; }
    void        SetQuatNow( D3DXQUATERNION q ) { m_qNow = q; }

    static D3DXQUATERNION QuatFromBallPoints( const D3DXVECTOR3 &vFrom, const D3DXVECTOR3 &vTo );


protected:
    D3DXMATRIXA16  m_mRotation;         // ��ũ�� �߽� ���
    D3DXMATRIXA16  m_mTranslation;      // ��ũ�� ��ġ ���
    D3DXMATRIXA16  m_mTranslationDelta; // ��ũ�� ��ġ ���

    POINT          m_Offset;   // ������ ������, �������� �»�𼭸�
    INT            m_nWidth;   // ��ũ���� ������ ����
    INT            m_nHeight;  // ��ũ���� ������ ����
    D3DXVECTOR2    m_vCenter;  // ��ũ���� �߽�
    FLOAT          m_fRadius;  // ��ũ�� ��ǥ�� ��ũ�� ������
    FLOAT          m_fRadiusTranslation; // ��ǥ�� �̵��ϱ����� ��ũ���� ������

    D3DXQUATERNION m_qDown;             // ���콺 ��ư Ŭ�� �� 4����
    D3DXQUATERNION m_qNow;              // ���� �巡���� ���յ� 4���� 
    bool           m_bDrag;             // ������ ��ũ ���� �巡�� �ϰ� �ִ°�

    POINT          m_ptLastMouse;       // ������ ���콺 ��ġ ����Ʈ
    D3DXVECTOR3    m_vDownPt;           // ȸ�� ��ũ�� ���� ����Ʈ
    D3DXVECTOR3    m_vCurrentPt;        // ȸ�� ��ũ�� ���� ����Ʈ

    D3DXVECTOR3    ScreenToVector( float fScreenPtX, float fScreenPtY );
};


//--------------------------------------------------------------------------------------
// WM_KEYDOWN Ű ������ ���� CCamera���� ���ȴ�.
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
// �̵��� ȸ���ϴ� �ܼ��� �⺻ ī�޶� Ŭ����.
// �� �⺻ Ŭ������ ��ӵ� Ŭ������ ����ϱ� ���� ���콺�� Ű���� �Է��� ����ϰ�,
// ���� ���¸� �����Ѵ�
//--------------------------------------------------------------------------------------
class CBaseCamera
{
public:
    CBaseCamera();

	// Ŭ���̾�Ʈ�� ���� �̰͵��� ȣ��Ǿ� ���ο� ����� �б����� Get*Matrix()�� ����Ѵ�.
    virtual LRESULT HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual void    FrameMove( FLOAT fElapsedTime ) = 0;

	// ī�޶� ����� �ٲٴ� �Լ�
    virtual void Reset(); 
    virtual void SetViewParams( D3DXVECTOR3* pvEyePt, D3DXVECTOR3* pvLookatPt );
    virtual void SetProjParams( FLOAT fFOV, FLOAT fAspect, FLOAT fNearPlane, FLOAT fFarPlane );

	// �ൿ�� �ٲٴ� �Լ�
    virtual void SetDragRect( RECT &rc ) { m_rcDrag = rc; }
    void SetInvertPitch( bool bInvertPitch ) { m_bInvertPitch = bInvertPitch; }
    void SetDrag( bool bMovementDrag, FLOAT fTotalDragTimeToZero = 0.25f ) { m_bMovementDrag = bMovementDrag; m_fTotalDragTimeToZero = fTotalDragTimeToZero; }
    void SetEnableYAxisMovement( bool bEnableYAxisMovement ) { m_bEnableYAxisMovement = bEnableYAxisMovement; }
    void SetEnablePositionMovement( bool bEnablePositionMovement ) { m_bEnablePositionMovement = bEnablePositionMovement; }
    void SetClipToBoundary( bool bClipToBoundary, D3DXVECTOR3* pvMinBoundary, D3DXVECTOR3* pvMaxBoundary ) { m_bClipToBoundary = bClipToBoundary; if( pvMinBoundary ) m_vMinBoundary = *pvMinBoundary; if( pvMaxBoundary ) m_vMaxBoundary = *pvMaxBoundary; }
    void SetScalers( FLOAT fRotationScaler = 0.01f, FLOAT fMoveScaler = 5.0f )  { m_fRotationScaler = fRotationScaler; m_fMoveScaler = fMoveScaler; }
    void SetNumberOfFramesToSmoothMouseData( int nFrames ) { if( nFrames > 0 ) m_fFramesToSmoothMouseData = (float)nFrames; }

	// ���¸� ������� �Լ�
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
	// WM_KEYDOWN�� D3DUtil_CameraKeys enum���� �����ϱ� ���� �Լ���
    virtual D3DUtil_CameraKeys MapKey( UINT nKey );    
    bool IsKeyDown( BYTE key ) const { return( (key & KEY_IS_DOWN_MASK) == KEY_IS_DOWN_MASK ); }
    bool WasKeyDown( BYTE key ) const { return( (key & KEY_WAS_DOWN_MASK) == KEY_WAS_DOWN_MASK ); }

    void ConstrainToBoundary( D3DXVECTOR3* pV );
    void UpdateVelocity( float fElapsedTime );
    void GetInput( bool bGetKeyboardInput, bool bGetMouseInput, bool bGetGamepadInput, bool bResetCursorAfterMove );

    D3DXMATRIX            m_mView;              // �� ���
    D3DXMATRIX            m_mProj;              // �������� ���

    DXUT_GAMEPAD          m_GamePad[DXUT_MAX_CONTROLLERS]; // XInput ��Ʈ�ѷ� ����
    D3DXVECTOR3           m_vGamePadLeftThumb;
    D3DXVECTOR3           m_vGamePadRightThumb;
    double                m_GamePadLastActive[DXUT_MAX_CONTROLLERS];

    int                   m_cKeysDown;            // ������ ī�޶� Ű�� ��
    BYTE                  m_aKeys[CAM_MAX_KEYS];  // �Է��� ���� - KEY_WAS_DOWN_MASK|KEY_IS_DOWN_MASK
    D3DXVECTOR3           m_vKeyboardDirection;   // Ű���� �Է��� ���� ����
    POINT                 m_ptLastMousePosition;  // ���콺 Ŀ���� ������ ���� ��ġ
    bool                  m_bMouseLButtonDown;    // ���� ��ư�� �������ٸ� True
    bool                  m_bMouseMButtonDown;    // ��� ��ư�� �������ٸ� True
    bool                  m_bMouseRButtonDown;    // ������ ��ư�� �������ٸ� True
    int                   m_nCurrentButtonMask;   // � ��ư�� �ҷ������� ����ũ
    int                   m_nMouseWheelDelta;     // ��� �� ��ũ�� ��ġ (+/-) 
    D3DXVECTOR2           m_vMouseDelta;          // �� �������� �ε巴�� ������ ���콺���� ��Ÿ
    float                 m_fFramesToSmoothMouseData; // ���콺 ������ �ε巴�� ������ �������� ��

    D3DXVECTOR3           m_vDefaultEye;          // �⺻ ī�޶� eye ��ġ
    D3DXVECTOR3           m_vDefaultLookAt;       // �⺻ LookAt ��ġ
    D3DXVECTOR3           m_vEye;                 // ī�޶� eye ��ġ
    D3DXVECTOR3           m_vLookAt;              // LookAt ��ġ
    float                 m_fCameraYawAngle;      // ī�޶��� Yaw ����
    float                 m_fCameraPitchAngle;    // ī�޶��� Pitch ����

    RECT                  m_rcDrag;               // �巡���ؼ� ������ �簢��
    D3DXVECTOR3           m_vVelocity;            // ī�޶��� �ӵ�
    bool                  m_bMovementDrag;        // ���� True�̸�, ī�޶��� �̵��� �����ϱ����� ������ ���̴�. �׷��� ������ ������ �����δ�.
    D3DXVECTOR3           m_vVelocityDrag;        // �巡�� ���� �ӵ�
    FLOAT                 m_fDragTimer;           // �巡�׸� �����ϱ� ���� ī��Ʈ�ٿ� Ÿ�̸�
    FLOAT                 m_fTotalDragTimeToZero; // �ӵ��� �ְ��� 0���� �Ǵ� �ð�
    D3DXVECTOR2           m_vRotVelocity;         // ī�޶��� �ӵ�

    float                 m_fFOV;                 // �þ�(Field of view)
    float                 m_fAspect;              // Aspect ratio
    float                 m_fNearPlane;           // Near plane
    float                 m_fFarPlane;            // Far plane 

    float                 m_fRotationScaler;      // ȸ�� ��Į��
    float                 m_fMoveScaler;          // �̵� ��Į��

    bool                  m_bInvertPitch;         // Pitch���� �����ϴ�.
    bool                  m_bEnablePositionMovement; // ���� true�̸� ����ڴ� �ɸ���� ���� �̵��� �� �ִ�.
    bool                  m_bEnableYAxisMovement; // ���� true�̸�, ī�޶�� y�࿡�� �̵��� �� �ִ�.

    bool                  m_bClipToBoundary;      // ���� true�̸�, ī�޶�� �׵θ��� Ŭ������ ���̴�.
    D3DXVECTOR3           m_vMinBoundary;         // Ŭ�� �׵θ��� �ּ� ����Ʈ
    D3DXVECTOR3           m_vMaxBoundary;         // Ŭ�� �׵θ��� �ִ� ����Ʈ
};


//--------------------------------------------------------------------------------------
// �̵��� ȸ�����ϴ� �ܼ��� ����Ī ī�޶� Ŭ����
// yaw, pitch�� �����ϳ� roll�� �ȵȴ�. Ű����� ���콺 �Է¿� �����ϱ� ���� 
// WM_KEYDOWN�� GetCursorPos() ����ϰ� �Է¿� ����� �� ��ĸ� ������Ʈ�Ѵ�.
//--------------------------------------------------------------------------------------
class CFirstPersonCamera : public CBaseCamera
{
public:
    CFirstPersonCamera();

	// Ŭ���̾�Ʈ�κ��� �̰��� ȣ���Ͽ� ���ο� ����� �б����� Get*Matrix()�� ����Ѵ�.
    virtual void FrameMove( FLOAT fElapsedTime );

	// ������ ��ȭ��Ű�� �Լ�
    void SetRotateButtons( bool bLeft, bool bMiddle, bool bRight, bool bRotateWithoutButtonDown = false );

    void SetResetCursorAfterMove( bool bResetCursorAfterMove ) { m_bResetCursorAfterMove = bResetCursorAfterMove; }

    // ���¸� ��� �Լ�
    D3DXMATRIX*  GetWorldMatrix()            { return &m_mCameraWorld; }

    const D3DXVECTOR3* GetWorldRight() const { return (D3DXVECTOR3*)&m_mCameraWorld._11; } 
    const D3DXVECTOR3* GetWorldUp() const    { return (D3DXVECTOR3*)&m_mCameraWorld._21; }
    const D3DXVECTOR3* GetWorldAhead() const { return (D3DXVECTOR3*)&m_mCameraWorld._31; }
    const D3DXVECTOR3* GetEyePt() const      { return (D3DXVECTOR3*)&m_mCameraWorld._41; }

protected:
    D3DXMATRIX m_mCameraWorld;       // ī�޶��� ���� ���(�� ����� �����)

    int        m_nActiveButtonMask;  // ��ư�� ȸ���� �������� �����ϴ� ����ũ
    bool       m_bRotateWithoutButtonDown;

    bool       m_bResetCursorAfterMove;// ���� true�̸�, Ŭ������ Ŀ�� ��ġ�� ������ ���̴�, �׷��� Ŀ���� �׻� �̵��� ������ �ִ�.
};


//--------------------------------------------------------------------------------------
// ��ü ������ ȸ���ϴ� ������ �� �� ī�޶� Ŭ����
//--------------------------------------------------------------------------------------
class CModelViewerCamera : public CBaseCamera
{
public:
    CModelViewerCamera();

	// Ŭ���̾�Ʈ�� ���� �̰��� ȣ���Ͽ� ���ο� ����� �б����� Get*Matrix()�� ����Ѵ�.
    virtual LRESULT HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual void FrameMove( FLOAT fElapsedTime );

   
	// ������ �����ϱ� ���� �Լ���
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

    // ���¸� ������� �Լ���
    const D3DXMATRIX* GetWorldMatrix() const { return &m_mWorld; }
    void SetWorldMatrix( D3DXMATRIX &mWorld ) { m_mWorld = mWorld; m_bDragSinceLastUpdate = true; }

protected:
    CD3DArcBall  m_WorldArcBall;
    CD3DArcBall  m_ViewArcBall;
    D3DXVECTOR3  m_vModelCenter;
    D3DXMATRIX   m_mModelLastRot;        // ���� ������ ��ũ�� ȸ�� ���
    D3DXMATRIX   m_mModelRot;            // ���� ȸ�� ���
    D3DXMATRIX   m_mWorld;               // ���� ���� ���

    int          m_nRotateModelButtonMask;
    int          m_nZoomButtonMask;
    int          m_nRotateCameraButtonMask;

    bool         m_bAttachCameraToModel;
    bool         m_bLimitPitch;
    float        m_fRadius;              // ī�޶�κ��� �𵨰��� �Ÿ�
    float        m_fDefaultRadius;       // ī�޶�κ��� �𵨰��� �⺻ �Ÿ�
    float        m_fMinRadius;           // �ּ� ������
    float        m_fMaxRadius;           // �ִ� ������
    bool         m_bDragSinceLastUpdate; // �ֱ��� FrameMove�� ȣ��� �� ���� ���콺 �巡�װ� �Ͼ�ٸ� true

    D3DXMATRIX   m_mCameraRotLast;

};


//--------------------------------------------------------------------------------------
// �ؽ�Ʈ�� �׷��� �� ���Ե� ��ġ�� �����Ѵ�
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
// �������� ������ ��� �����ϰ� ID3DXLine�� ����Ͽ� �װ͵��� �׸���.
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
// ��ũ���� ���� ����Ǿ� ������ ������ ȸ���ϴ� ����� �޽ÿ� ����, ���콺�� 
// �̺�Ʈ�� �����Ѵ�.
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
// ���� �׵��� REF ��ġ�� ���Ḧ ���Ѵٸ� ����ڿ��� ��û�ϴ� ������ �ڵ尡 �����ȴ�.
//--------------------------------------------------------------------------------------
void DXUTDisplaySwitchingToREFWarning();

//--------------------------------------------------------------------------------------
// ���� ��ġ���� �˻����� �̵�� ������ ã�⸦ �õ��Ѵ�.
//--------------------------------------------------------------------------------------
HRESULT DXUTFindDXSDKMediaFileCch( WCHAR* strDestPath, int cchDest, LPCWSTR strFilename );
HRESULT DXUTSetMediaSearchPath( LPCWSTR strPath );
LPCWSTR DXUTGetMediaSearchPath();


//--------------------------------------------------------------------------------------
// �־��� D3DFORMAT�� ���� ���ڿ��� �����Ѵ�.
// bWithPrefix�� ���ڿ��� "D3DFMT_"�� �����ؾ��ϴ��� �����Ѵ�.
//--------------------------------------------------------------------------------------
LPCWSTR DXUTD3DFormatToString( D3DFORMAT format, bool bWithPrefix );


//--------------------------------------------------------------------------------------
// ť����� ���� �������� �� ����� �����Ѵ�.
//--------------------------------------------------------------------------------------
D3DXMATRIX DXUTGetCubeMapViewMatrix( DWORD dwFace );


//--------------------------------------------------------------------------------------
// ���α׷��� ����� �Ŀ� �̵�� ���� UI�� ������ ���� �Լ�
//--------------------------------------------------------------------------------------
bool DXUTReLaunchMediaCenter();


//--------------------------------------------------------------------------------------
// ����� ��� ����
// ����� ������ ������ ���� dxerr.h�� ����
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

// �̷��� ��ũ�ε��� dxerr�� �� �ſ� ����ϴ�. ������ �̰��� Ư���� ����� HRESULT�� 
// ������ �޼��� �ڽ��� �����ִ� DXUT�� ���� ���ǵȴ�.

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
// Direct3D9 ���� ��ŷ ���� -- API�� ǥ������ ���Ѵٸ� ����� ���и� �ϴ� 
// top-level D3D9 API���� ȣ���Ѵ�. 
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
// �������ϸ�/���� ����
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
// ����迭�� ����
//--------------------------------------------------------------------------------------

// �̹����� ctor �̳� dtor�� ȣ������ �ʴ´�.
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
