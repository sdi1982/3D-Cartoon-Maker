//--------------------------------------------------------------------------------------
// File: DXUTgui.h
//
// Desc: 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#ifndef DXUT_GUI_H
#define DXUT_GUI_H

#include <usp10.h>
#include <dimm.h>


//--------------------------------------------------------------------------------------
// 정의와 매크로
//--------------------------------------------------------------------------------------
#define EVENT_BUTTON_CLICKED                0x0101
#define EVENT_COMBOBOX_SELECTION_CHANGED    0x0201
#define EVENT_RADIOBUTTON_CHANGED           0x0301
#define EVENT_CHECKBOX_CHANGED              0x0401
#define EVENT_SLIDER_VALUE_CHANGED          0x0501
#define EVENT_EDITBOX_STRING                0x0601

// 사용자 입력에 의해 리스트박스의 내용이 변할 때, EVENT_EDITBOX_CHANGE가 보내진다.
#define EVENT_EDITBOX_CHANGE                0x0602
#define EVENT_LISTBOX_ITEM_DBLCLK           0x0701
// 리스트 박스의 단일 선택으로 선택이 변할때, EVENT_LISTBOX_SELECTION가 발생한다.
#define EVENT_LISTBOX_SELECTION             0x0702
#define EVENT_LISTBOX_SELECTION_END         0x0703


//--------------------------------------------------------------------------------------
// 전방 선언
//--------------------------------------------------------------------------------------
class CDXUTDialogResourceManager;
class CDXUTControl;
class CDXUTButton;
class CDXUTStatic;
class CDXUTCheckBox;
class CDXUTRadioButton;
class CDXUTComboBox;
class CDXUTSlider;
class CDXUTEditBox;
class CDXUTIMEEditBox;
class CDXUTListBox;
class CDXUTScrollBar;
class CDXUTElement;
struct DXUTElementHolder;
struct DXUTTextureNode;
struct DXUTFontNode;
typedef VOID (CALLBACK *PCALLBACKDXUTGUIEVENT) ( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );


//--------------------------------------------------------------------------------------
// pre-defined 제어 타입 열거
//--------------------------------------------------------------------------------------
enum DXUT_CONTROL_TYPE 
{ 
    DXUT_CONTROL_BUTTON, 
    DXUT_CONTROL_STATIC, 
    DXUT_CONTROL_CHECKBOX,
    DXUT_CONTROL_RADIOBUTTON,
    DXUT_CONTROL_COMBOBOX,
    DXUT_CONTROL_SLIDER,
    DXUT_CONTROL_EDITBOX,
    DXUT_CONTROL_IMEEDITBOX,
    DXUT_CONTROL_LISTBOX,
    DXUT_CONTROL_SCROLLBAR,
};

enum DXUT_CONTROL_STATE
{
    DXUT_STATE_NORMAL = 0,
    DXUT_STATE_DISABLED,
    DXUT_STATE_HIDDEN,
    DXUT_STATE_FOCUS,
    DXUT_STATE_MOUSEOVER,
    DXUT_STATE_PRESSED,
};

#define MAX_CONTROL_STATES 6

struct DXUTBlendColor
{
    void Init( D3DCOLOR defaultColor, D3DCOLOR disabledColor = D3DCOLOR_ARGB(200, 128, 128, 128), D3DCOLOR hiddenColor = 0 );
    void Blend( UINT iState, float fElapsedTime, float fRate = 0.7f );
    
    D3DCOLOR  States[ MAX_CONTROL_STATES ]; // 모든 가능한 제어 상태를 위해 색을 합친다.
    D3DXCOLOR Current;
};


//-----------------------------------------------------------------------------
// 서브 컨트롤의 조정이 가능한 모든 표시를 포함한다.
//-----------------------------------------------------------------------------
class CDXUTElement
{
public:
    void SetTexture( UINT iTexture, RECT* prcTexture, D3DCOLOR defaultTextureColor = D3DCOLOR_ARGB(255, 255, 255, 255) );
    void SetFont( UINT iFont, D3DCOLOR defaultFontColor = D3DCOLOR_ARGB(255, 255, 255, 255), DWORD dwTextFormat = DT_CENTER | DT_VCENTER );
    
    void Refresh();
    
    UINT iTexture;          // 이 요소의 텍스쳐 인덱스
    UINT iFont;             // 이 요소의 폰트 인덱스
    DWORD dwTextFormat;     // DrawText 포멧 인자

    RECT rcTexture;         // 조합된 텍스쳐에서 이 요소의 경계 사각형
    
    DXUTBlendColor TextureColor;
    DXUTBlendColor FontColor;
};


//-----------------------------------------------------------------------------
// 모든 컨트롤은 입력과 표시를 제어할 다이얼로그에 할당되어야 한다.
//-----------------------------------------------------------------------------
class CDXUTDialog
{
    friend class CDXUTDialogResourceManager;

public:
    CDXUTDialog();
    ~CDXUTDialog();

	// 지금 콜할 필요가 있다.
    void Init( CDXUTDialogResourceManager* pManager, bool bRegisterDialog = true );
    void Init( CDXUTDialogResourceManager* pManager, bool bRegisterDialog, LPCWSTR pszControlTextureFilename );
    void Init( CDXUTDialogResourceManager* pManager, bool bRegisterDialog, LPCWSTR szControlTextureResourceName, HMODULE hControlTextureResourceModule );

	// 윈도우 메세지 핸들러
    bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// 컨트롤 생성
    HRESULT AddStatic( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault=false, CDXUTStatic** ppCreated=NULL );
    HRESULT AddButton( int ID, LPCWSTR strText, int x, int y, int width, int height, UINT nHotkey=0, bool bIsDefault=false, CDXUTButton** ppCreated=NULL );
    HRESULT AddCheckBox( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bChecked=false, UINT nHotkey=0, bool bIsDefault=false, CDXUTCheckBox** ppCreated=NULL );
    HRESULT AddRadioButton( int ID, UINT nButtonGroup, LPCWSTR strText, int x, int y, int width, int height, bool bChecked=false, UINT nHotkey=0, bool bIsDefault=false, CDXUTRadioButton** ppCreated=NULL );
    HRESULT AddComboBox( int ID, int x, int y, int width, int height, UINT nHotKey=0, bool bIsDefault=false, CDXUTComboBox** ppCreated=NULL );
    HRESULT AddSlider( int ID, int x, int y, int width, int height, int min=0, int max=100, int value=50, bool bIsDefault=false, CDXUTSlider** ppCreated=NULL );
    HRESULT AddEditBox( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault=false, CDXUTEditBox** ppCreated=NULL );
    HRESULT AddIMEEditBox( int ID, LPCWSTR strText, int x, int y, int width, int height, bool bIsDefault=false, CDXUTIMEEditBox** ppCreated=NULL );
    HRESULT AddListBox( int ID, int x, int y, int width, int height, DWORD dwStyle=0, CDXUTListBox** ppCreated=NULL );
    HRESULT AddControl( CDXUTControl* pControl );
    HRESULT InitControl( CDXUTControl* pControl );

	// 컨트롤  복구
    CDXUTStatic*      GetStatic( int ID ) { return (CDXUTStatic*) GetControl( ID, DXUT_CONTROL_STATIC ); }
    CDXUTButton*      GetButton( int ID ) { return (CDXUTButton*) GetControl( ID, DXUT_CONTROL_BUTTON ); }
    CDXUTCheckBox*    GetCheckBox( int ID ) { return (CDXUTCheckBox*) GetControl( ID, DXUT_CONTROL_CHECKBOX ); }
    CDXUTRadioButton* GetRadioButton( int ID ) { return (CDXUTRadioButton*) GetControl( ID, DXUT_CONTROL_RADIOBUTTON ); }
    CDXUTComboBox*    GetComboBox( int ID ) { return (CDXUTComboBox*) GetControl( ID, DXUT_CONTROL_COMBOBOX ); }
    CDXUTSlider*      GetSlider( int ID ) { return (CDXUTSlider*) GetControl( ID, DXUT_CONTROL_SLIDER ); }
    CDXUTEditBox*     GetEditBox( int ID ) { return (CDXUTEditBox*) GetControl( ID, DXUT_CONTROL_EDITBOX ); }
    CDXUTIMEEditBox*  GetIMEEditBox( int ID ) { return (CDXUTIMEEditBox*) GetControl( ID, DXUT_CONTROL_IMEEDITBOX ); }
    CDXUTListBox*     GetListBox( int ID ) { return (CDXUTListBox*) GetControl( ID, DXUT_CONTROL_LISTBOX ); }

    CDXUTControl* GetControl( int ID );
    CDXUTControl* GetControl( int ID, UINT nControlType );
    CDXUTControl* GetControlAtPoint( POINT pt );

    bool GetControlEnabled( int ID );
    void SetControlEnabled( int ID, bool bEnabled );

    void ClearRadioButtonGroup( UINT nGroup );
    void ClearComboBox( int ID );

	// 새로운 컨트롤을 추가할 때 사용된 기본 표시할 요소를 접근한다.
    HRESULT       SetDefaultElement( UINT nControlType, UINT iElement, CDXUTElement* pElement );
    CDXUTElement* GetDefaultElement( UINT nControlType, UINT iElement );

	// 컨트롤에의해 불려지는 함수들
    void SendEvent( UINT nEvent, bool bTriggeredByUser, CDXUTControl* pControl );
    void RequestFocus( CDXUTControl* pControl );

	// 렌더링 도우미
    HRESULT DrawRect( RECT* pRect, D3DCOLOR color );
    HRESULT DrawPolyLine( POINT* apPoints, UINT nNumPoints, D3DCOLOR color );
    HRESULT DrawSprite( CDXUTElement* pElement, RECT* prcDest );
    HRESULT CalcTextRect( LPCWSTR strText, CDXUTElement* pElement, RECT* prcDest, int nCount = -1 );
    HRESULT DrawText( LPCWSTR strText, CDXUTElement* pElement, RECT* prcDest, bool bShadow = false, int nCount = -1 );

    // 속성
    bool GetVisible() { return m_bVisible; }
    void SetVisible( bool bVisible ) { m_bVisible = bVisible; }
    bool GetMinimized() { return m_bMinimized; }
    void SetMinimized( bool bMinimized ) { m_bMinimized = bMinimized; }
    void SetBackgroundColors( D3DCOLOR colorAllCorners ) { SetBackgroundColors( colorAllCorners, colorAllCorners, colorAllCorners, colorAllCorners ); }
    void SetBackgroundColors( D3DCOLOR colorTopLeft, D3DCOLOR colorTopRight, D3DCOLOR colorBottomLeft, D3DCOLOR colorBottomRight );
    void EnableCaption( bool bEnable ) { m_bCaption = bEnable; }
    int GetCaptionHeight() const { return m_nCaptionHeight; }
    void SetCaptionHeight( int nHeight ) { m_nCaptionHeight = nHeight; }
    void SetCaptionText( const WCHAR *pwszText ) { StringCchCopy( m_wszCaption, sizeof(m_wszCaption)/sizeof(m_wszCaption[0]), pwszText); }
    void GetLocation( POINT &Pt ) const { Pt.x = m_x; Pt.y = m_y; }
    void SetLocation( int x, int y ) { m_x = x; m_y = y; }
    void SetSize( int width, int height ) { m_width = width; m_height = height;  }
    int GetWidth() { return m_width; }
    int GetHeight() { return m_height; }

    static void SetRefreshTime( float fTime ){ s_fTimeRefresh = fTime; }

    static CDXUTControl* GetNextControl( CDXUTControl* pControl );
    static CDXUTControl* GetPrevControl( CDXUTControl* pControl );

    void RemoveControl( int ID );
    void RemoveAllControls();

	// 컨트롤 이벤트의 app에게 통지하기 위해 사용될 콜백을 설정한다.
    void SetCallback( PCALLBACKDXUTGUIEVENT pCallback, void* pUserContext = NULL );
    void EnableNonUserEvents( bool bEnable ) { m_bNonUserEvents = bEnable; }
    void EnableKeyboardInput( bool bEnable ) { m_bKeyboardInput = bEnable; }
    void EnableMouseInput( bool bEnable ) { m_bMouseInput = bEnable; }
    bool IsKeyboardInputEnabled() const { return m_bKeyboardInput; }

	// 장치 상태 통지
    void Refresh();
    HRESULT OnRender( float fElapsedTime );

	// 공유 자원 접근. 모든 컨트롤 간의 폰트 색인과 텍스쳐가 공유된다.
    HRESULT       SetFont( UINT index, LPCWSTR strFaceName, LONG height, LONG weight );
    DXUTFontNode* GetFont( UINT index );

    HRESULT          SetTexture( UINT index, LPCWSTR strFilename );
    HRESULT          SetTexture( UINT index, LPCWSTR strResourceName, HMODULE hResourceModule );
    DXUTTextureNode* GetTexture( UINT index );

    CDXUTDialogResourceManager* GetManager() { return m_pManager; }

    static void ClearFocus();
    void FocusDefaultControl();

    bool m_bNonUserEvents;
    bool m_bKeyboardInput;
    bool m_bMouseInput;



private:
    int m_nDefaultControlID;

    static double s_fTimeRefresh;
    double m_fTimeLastRefresh;

	// 기본 요소를 초기화한다.
    void InitDefaultElements();

	// 윈도우 메세지 핸들러
    void OnMouseMove( POINT pt );
    void OnMouseUp( POINT pt );

    void SetNextDialog( CDXUTDialog* pNextDialog );

    // 컨트롤 이벤트
    bool OnCycleFocus( bool bForward );

    static CDXUTControl* s_pControlFocus;        // The control which has focus
    static CDXUTControl* s_pControlPressed;      // The control currently pressed

    CDXUTControl* m_pControlMouseOver;           // The control which is hovered over

    bool m_bVisible;
    bool m_bCaption;
    bool m_bMinimized;
    bool m_bDrag;
    WCHAR m_wszCaption[256];

    int m_x;
    int m_y;
    int m_width;
    int m_height;
    int m_nCaptionHeight;

    D3DCOLOR m_colorTopLeft;
    D3DCOLOR m_colorTopRight;
    D3DCOLOR m_colorBottomLeft;
    D3DCOLOR m_colorBottomRight;

    CDXUTDialogResourceManager* m_pManager;
    PCALLBACKDXUTGUIEVENT m_pCallbackEvent;
    void* m_pCallbackEventUserContext;

    CGrowableArray< int > m_Textures;   // Index into m_TextureCache;
    CGrowableArray< int > m_Fonts;      // Index into m_FontCache;

    CGrowableArray< CDXUTControl* > m_Controls;
    CGrowableArray< DXUTElementHolder* > m_DefaultElements;

    CDXUTElement m_CapElement;  // Element for the caption

    CDXUTDialog* m_pNextDialog;
    CDXUTDialog* m_pPrevDialog;
};


//--------------------------------------------------------------------------------------
// 공유 자원을 위한 구조체
//--------------------------------------------------------------------------------------
struct DXUTTextureNode
{
    bool bFileSource;  // True if this texture is loaded from a file. False if from resource.
    HMODULE hResourceModule;
    int nResourceID;   // Resource ID. If 0, string-based ID is used and stored in strFilename.
    WCHAR strFilename[MAX_PATH];
    IDirect3DTexture9* pTexture;
    DWORD dwWidth;
    DWORD dwHeight;
};

struct DXUTFontNode
{
    WCHAR strFace[MAX_PATH];
    ID3DXFont* pFont;
    LONG  nHeight;
    LONG  nWeight;
};


//-----------------------------------------------------------------------------
// 다이얼로그의 공유자원을 관리한다.
//-----------------------------------------------------------------------------
class CDXUTDialogResourceManager
{
public:
    CDXUTDialogResourceManager();
    ~CDXUTDialogResourceManager();

    HRESULT     OnCreateDevice( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT     OnResetDevice();
    void        OnLostDevice();
    void        OnDestroyDevice();
    bool        MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    
    int AddFont( LPCWSTR strFaceName, LONG height, LONG weight );
    int AddTexture( LPCWSTR strFilename );
    int AddTexture( LPCWSTR strResourceName, HMODULE hResourceModule );

    DXUTFontNode*     GetFontNode( int iIndex )     { return m_FontCache.GetAt( iIndex ); };
    DXUTTextureNode*  GetTextureNode( int iIndex )  { return m_TextureCache.GetAt( iIndex ); };
    IDirect3DDevice9* GetD3DDevice()                { return m_pd3dDevice; }

    bool RegisterDialog( CDXUTDialog *pDialog );
    void UnregisterDialog( CDXUTDialog *pDialog );
    void EnableKeyboardInputForAllDialogs();

	// 모든 다이얼로그 사이에서 공유된다.
    IDirect3DStateBlock9* m_pStateBlock;
    ID3DXSprite*          m_pSprite;          // Sprite used for drawing

    CGrowableArray< CDXUTDialog* > m_Dialogs;            // Dialogs registered

protected:
    CGrowableArray< DXUTTextureNode* > m_TextureCache;   // Shared textures
    CGrowableArray< DXUTFontNode* > m_FontCache;         // Shared fonts

    IDirect3DDevice9* m_pd3dDevice;

	// 자원 생성 도우미
    HRESULT CreateFont( UINT index );
    HRESULT CreateTexture( UINT index );
};


//-----------------------------------------------------------------------------
// 컨트롤의 기본 클래스
//-----------------------------------------------------------------------------
class CDXUTControl
{
public:
    CDXUTControl( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTControl();

    virtual HRESULT OnInit() { return S_OK; }
    virtual void Refresh();
    virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime ) { };

	// 윈도우 메세지 핸들러
    virtual bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam ) { return false; }

    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam ) { return false; }
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam ) { return false; }

    virtual bool CanHaveFocus() { return false; }
    virtual void OnFocusIn() { m_bHasFocus = true; }
    virtual void OnFocusOut() { m_bHasFocus = false; }
    virtual void OnMouseEnter() { m_bMouseOver = true; }
    virtual void OnMouseLeave() { m_bMouseOver = false; }
    virtual void OnHotkey() {}

    virtual BOOL ContainsPoint( POINT pt ) { return PtInRect( &m_rcBoundingBox, pt ); }

    virtual void SetEnabled( bool bEnabled ) { m_bEnabled = bEnabled; }
    virtual bool GetEnabled() { return m_bEnabled; }
    virtual void SetVisible( bool bVisible ) { m_bVisible = bVisible; }
    virtual bool GetVisible() { return m_bVisible; }

    UINT GetType() const { return m_Type; }

    int  GetID() const { return m_ID; }
    void SetID( int ID ) { m_ID = ID; }

    void SetLocation( int x, int y ) { m_x = x; m_y = y; UpdateRects(); }
    void SetSize( int width, int height ) { m_width = width; m_height = height; UpdateRects(); }

    void SetHotkey( UINT nHotkey ) { m_nHotkey = nHotkey; }
    UINT GetHotkey() { return m_nHotkey; }

    void SetUserData( void *pUserData ) { m_pUserData = pUserData; }
    void *GetUserData() const { return m_pUserData; }

    virtual void SetTextColor( D3DCOLOR Color );
    CDXUTElement* GetElement( UINT iElement ) { return m_Elements.GetAt( iElement ); }
    HRESULT SetElement( UINT iElement, CDXUTElement* pElement);

    bool m_bVisible;                // 보임/숨김 플래그
    bool m_bMouseOver;              // 컨트롤 위에 마우스 포인트가 잇다.
    bool m_bHasFocus;               // 컨트롤이 입력 포커스를 가지고 있다.
    bool m_bIsDefault;              // 기본 컨트롤이다.

	// 크기, 배율, 위치 변수
    int m_x, m_y;
    int m_width, m_height;

	// 컨테이너가 이러한 변수를 설정한다.
    CDXUTDialog* m_pDialog;    // 부모 컨테이너
    UINT m_Index;              // 컨트롤 리스트가 가지는 인덱스
    
    CGrowableArray< CDXUTElement* > m_Elements;  // 모든 표시 요소들

protected:
    virtual void UpdateRects();

    int  m_ID;                 // ID 번호
    DXUT_CONTROL_TYPE m_Type;  // 컨트롤 타입, 생성자에서 한번 설정된다.
    UINT m_nHotkey;            // 컨트롤의 단축기의 가상 키 코드
    void *m_pUserData;         // 사용자에 의해 설정된 컨트롤과 관련된 데이터
    
    bool m_bEnabled;           // 활성/비활성 플래그
    
    RECT m_rcBoundingBox;      // 컨트롤 활성 영역을 정의한 사각형
};


//-----------------------------------------------------------------------------
// 주어진 컨트롤 타입을 위한 표시 정보를 가진다.
//-----------------------------------------------------------------------------
struct DXUTElementHolder
{
    UINT nControlType;
    UINT iElement;

    CDXUTElement Element;
};


//-----------------------------------------------------------------------------
// 정적 컨트롤
//-----------------------------------------------------------------------------
class CDXUTStatic : public CDXUTControl
{
public:
    CDXUTStatic( CDXUTDialog *pDialog = NULL );

    virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
    virtual BOOL ContainsPoint( POINT pt ) { return false; }

    HRESULT GetTextCopy( LPWSTR strDest, UINT bufferCount );
    LPCWSTR GetText() { return m_strText; }
    HRESULT SetText( LPCWSTR strText );


protected:
    WCHAR m_strText[MAX_PATH];      // Window text  
};


//-----------------------------------------------------------------------------
// 버튼 컨트롤
//-----------------------------------------------------------------------------
class CDXUTButton : public CDXUTStatic
{
public:
    CDXUTButton( CDXUTDialog *pDialog = NULL );

    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual void OnHotkey() { if( m_pDialog->IsKeyboardInputEnabled() ) m_pDialog->RequestFocus( this ); m_pDialog->SendEvent( EVENT_BUTTON_CLICKED, true, this ); }
    
    virtual BOOL ContainsPoint( POINT pt ) { return PtInRect( &m_rcBoundingBox, pt ); }
    virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }

    virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );

protected:
    bool m_bPressed;
};


//-----------------------------------------------------------------------------
// 체크박스 컨트롤
//-----------------------------------------------------------------------------
class CDXUTCheckBox : public CDXUTButton
{
public:
    CDXUTCheckBox( CDXUTDialog *pDialog = NULL );

    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual void OnHotkey() { if( m_pDialog->IsKeyboardInputEnabled() ) m_pDialog->RequestFocus( this ); SetCheckedInternal( !m_bChecked, true ); }

    virtual BOOL ContainsPoint( POINT pt ); 
    virtual void UpdateRects(); 

    virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );

    bool GetChecked() { return m_bChecked; }
    void SetChecked( bool bChecked ) { SetCheckedInternal( bChecked, false ); }
    
protected:
    virtual void SetCheckedInternal( bool bChecked, bool bFromInput );

    bool m_bChecked;
    RECT m_rcButton;
    RECT m_rcText;
};


//-----------------------------------------------------------------------------
// 라디오버튼 컨트롤
//-----------------------------------------------------------------------------
class CDXUTRadioButton : public CDXUTCheckBox
{
public:
    CDXUTRadioButton( CDXUTDialog *pDialog = NULL );

    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual void OnHotkey() { if( m_pDialog->IsKeyboardInputEnabled() ) m_pDialog->RequestFocus( this ); SetCheckedInternal( true, true, true ); }
    
    void SetChecked( bool bChecked, bool bClearGroup=true ) { SetCheckedInternal( bChecked, bClearGroup, false ); }
    void SetButtonGroup( UINT nButtonGroup ) { m_nButtonGroup = nButtonGroup; }
    UINT GetButtonGroup() { return m_nButtonGroup; }
    
protected:
    virtual void SetCheckedInternal( bool bChecked, bool bClearGroup, bool bFromInput );
    UINT m_nButtonGroup;
};


//-----------------------------------------------------------------------------
// 스크롤바 컨트롤
//-----------------------------------------------------------------------------
class CDXUTScrollBar : public CDXUTControl
{
public:
    CDXUTScrollBar( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTScrollBar();

    virtual bool    HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool    HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual bool    MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

    virtual void    Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
    virtual void    UpdateRects();

    void SetTrackRange( int nStart, int nEnd );
    int GetTrackPos() { return m_nPosition; }
    void SetTrackPos( int nPosition ) { m_nPosition = nPosition; Cap(); UpdateThumbRect(); }
    int GetPageSize() { return m_nPageSize; }
    void SetPageSize( int nPageSize ) { m_nPageSize = nPageSize; Cap(); UpdateThumbRect(); }

    void Scroll( int nDelta );    // Scroll by nDelta items (plus or minus)
    void ShowItem( int nIndex );  // Ensure that item nIndex is displayed, scroll if necessary

protected:
    // ARROWSTATE		화살표버튼의 상태를 의미한다.
    // CLEAR            화살표가 눌려지지 않았다.
    // CLICKED_UP       위쪽 화살표가 클릭되었다.
    // CLICKED_DOWN     아래쪽 화살표가 클릭되었다.
    // HELD_UP          지연된 시간만큼 위쪽 화살표가 유지된다.
    // HELD_DOWN        지연된 시간만큼 아래쪽 화살표가 유지된다.
    enum ARROWSTATE { CLEAR, CLICKED_UP, CLICKED_DOWN, HELD_UP, HELD_DOWN };

    void UpdateThumbRect();
    void Cap();  // 경계에서 위치를 잘라낸다. 일반 범위에 유지되는 것을 보증한다.

    bool m_bShowThumb;
    bool m_bDrag;
    RECT m_rcUpButton;
    RECT m_rcDownButton;
    RECT m_rcTrack;
    RECT m_rcThumb;
    int m_nPosition;  // Position of the first displayed item
    int m_nPageSize;  // How many items are displayable in one page
    int m_nStart;     // First item
    int m_nEnd;       // The index after the last item
    POINT m_LastMouse;// Last mouse position
    ARROWSTATE m_Arrow; // State of the arrows
    double m_dArrowTS;  // Timestamp of last arrow event.
};


//-----------------------------------------------------------------------------
// 리스트박스 컨트롤
//-----------------------------------------------------------------------------
struct DXUTListBoxItem
{
    WCHAR strText[256];
    void*  pData;

    RECT  rcActive;
    bool  bSelected;
};

class CDXUTListBox : public CDXUTControl
{
public:
    CDXUTListBox( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTListBox();

    virtual HRESULT OnInit() { return m_pDialog->InitControl( &m_ScrollBar ); }
    virtual bool    CanHaveFocus() { return (m_bVisible && m_bEnabled); }
    virtual bool    HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool    HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual bool    MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

    virtual void    Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
    virtual void    UpdateRects();

    DWORD GetStyle() const { return m_dwStyle; }
    int GetSize() const { return m_Items.GetSize(); }
    void SetStyle( DWORD dwStyle ) { m_dwStyle = dwStyle; }
    int  GetScrollBarWidth() const { return m_nSBWidth; }
    void SetScrollBarWidth( int nWidth ) { m_nSBWidth = nWidth; UpdateRects(); }
    void SetBorder( int nBorder, int nMargin ) { m_nBorder = nBorder; m_nMargin = nMargin; }
    HRESULT AddItem( const WCHAR *wszText, void *pData );
    HRESULT InsertItem( int nIndex, const WCHAR *wszText, void *pData );
    void RemoveItem( int nIndex );
    void RemoveItemByText( WCHAR *wszText );
    void RemoveItemByData( void *pData );
    void RemoveAllItems();

    DXUTListBoxItem *GetItem( int nIndex );
    int GetSelectedIndex( int nPreviousSelected = -1 );
    DXUTListBoxItem *GetSelectedItem( int nPreviousSelected = -1 ) { return GetItem( GetSelectedIndex( nPreviousSelected ) ); }
    void SelectItem( int nNewIndex );

    enum STYLE { MULTISELECTION = 1 };

protected:
    RECT m_rcText;      // Text rendering bound
    RECT m_rcSelection; // Selection box bound
    CDXUTScrollBar m_ScrollBar;
    int m_nSBWidth;
    int m_nBorder;
    int m_nMargin;
    int m_nTextHeight;  // Height of a single line of text
    DWORD m_dwStyle;    // List box style
    int m_nSelected;    // Index of the selected item for single selection list box
    int m_nSelStart;    // Index of the item where selection starts (for handling multi-selection)
    bool m_bDrag;       // Whether the user is dragging the mouse to select

    CGrowableArray< DXUTListBoxItem* > m_Items;
};


//-----------------------------------------------------------------------------
// 콤보박스 컨트롤
//-----------------------------------------------------------------------------
struct DXUTComboBoxItem
{
    WCHAR strText[256];
    void*  pData;

    RECT  rcActive;
    bool  bVisible;
};


class CDXUTComboBox : public CDXUTButton
{
public:
    CDXUTComboBox( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTComboBox();
    
    virtual void SetTextColor( D3DCOLOR Color );
    virtual HRESULT OnInit() { return m_pDialog->InitControl( &m_ScrollBar ); }

    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual void OnHotkey();

    virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
    virtual void OnFocusOut();
    virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );

    virtual void UpdateRects(); 

    HRESULT AddItem( const WCHAR* strText, void* pData );
    void    RemoveAllItems();
    void    RemoveItem( UINT index );
    bool    ContainsItem( const WCHAR* strText, UINT iStart=0 );
    int     FindItem( const WCHAR* strText, UINT iStart=0 );
    void*   GetItemData( const WCHAR* strText );
    void*   GetItemData( int nIndex );
    void    SetDropHeight( UINT nHeight ) { m_nDropHeight = nHeight; UpdateRects(); }
    int     GetScrollBarWidth() const { return m_nSBWidth; }
    void    SetScrollBarWidth( int nWidth ) { m_nSBWidth = nWidth; UpdateRects(); }

    void*   GetSelectedData();
    DXUTComboBoxItem* GetSelectedItem();

    UINT    GetNumItems() { return m_Items.GetSize(); }
    DXUTComboBoxItem* GetItem( UINT index ) { return m_Items.GetAt( index ); }

    HRESULT SetSelectedByIndex( UINT index );
    HRESULT SetSelectedByText( const WCHAR* strText );
    HRESULT SetSelectedByData( void* pData );  

protected:
    int     m_iSelected;
    int     m_iFocused;
    int     m_nDropHeight;
    CDXUTScrollBar m_ScrollBar;
    int     m_nSBWidth;

    bool    m_bOpened;

    RECT m_rcText;
    RECT m_rcButton;
    RECT m_rcDropdown;
    RECT m_rcDropdownText;

    
    CGrowableArray< DXUTComboBoxItem* > m_Items;
};


//-----------------------------------------------------------------------------
// 슬라이더 컨트롤
//-----------------------------------------------------------------------------
class CDXUTSlider : public CDXUTControl
{
public:
    CDXUTSlider( CDXUTDialog *pDialog = NULL );

    virtual BOOL ContainsPoint( POINT pt ); 
    virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    
    virtual void UpdateRects(); 

    virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );

    void SetValue( int nValue ) { SetValueInternal( nValue, false ); }
    int  GetValue() const { return m_nValue; };

    void GetRange( int &nMin, int &nMax ) const { nMin = m_nMin; nMax = m_nMax; }
    void SetRange( int nMin, int nMax );

protected:
    void SetValueInternal( int nValue, bool bFromInput );
    int  ValueFromPos( int x ); 

    int m_nValue;

    int m_nMin;
    int m_nMax;

    int m_nDragX;      // 드래그를 시작한 마우스 위치
    int m_nDragOffset; // 버튼의 중심으로 부터 드래그 오프셋
    int m_nButtonX;

    bool m_bPressed;
    RECT m_rcButton;
};


//-----------------------------------------------------------------------------
// 에디트 컨트롤의 CUniBuffer 클래스
//-----------------------------------------------------------------------------
class CUniBuffer
{
public:
    CUniBuffer( int nInitialSize = 1 );
    ~CUniBuffer();

    static void Initialize();
    static void Uninitialize();

    int  GetBufferSize() { return m_nBufferSize; }
    bool SetBufferSize( int nSize );
    int  GetTextSize()  { return lstrlenW( m_pwszBuffer ); }
    const WCHAR* GetBuffer() { return m_pwszBuffer; }
    const WCHAR& operator[]( int n ) const { return m_pwszBuffer[n]; }
    WCHAR& operator[]( int n );
    DXUTFontNode* GetFontNode() { return m_pFontNode; }
    void SetFontNode( DXUTFontNode *pFontNode ) { m_pFontNode = pFontNode; }
    void Clear();

    bool InsertChar( int nIndex, WCHAR wChar ); // Inserts the char at specified index. If nIndex == -1, insert to the end.
    bool RemoveChar( int nIndex );  // Removes the char at specified index. If nIndex == -1, remove the last char.
    bool InsertString( int nIndex, const WCHAR *pStr, int nCount = -1 );  // Inserts the first nCount characters of the string pStr at specified index.  If nCount == -1, the entire string is inserted. If nIndex == -1, insert to the end.
    bool SetText( LPCWSTR wszText );

    // Uniscribe(윈도우에서 제공해주는 유니코드용? 텍스트 파서, DX에서 Rect를 넣고 Text출력을 하면 줄바꿈 처리가 되는게 내부에서 UniScribe를 사용한다.)
    HRESULT CPtoX( int nCP, BOOL bTrail, int *pX );
    HRESULT XtoCP( int nX, int *pCP, int *pnTrail );
    void GetPriorItemPos( int nCP, int *pPrior );
    void GetNextItemPos( int nCP, int *pPrior );

private:
    HRESULT Analyse();      // Uniscribe -- Analyse() 버퍼의 문자열을 분석한다.

    WCHAR* m_pwszBuffer;    // 텍스트를 가지는 버퍼
    int    m_nBufferSize;   // 문자들중에서 할당된 버퍼의 크기

    // Uniscribe 명세
    DXUTFontNode* m_pFontNode;          // 버퍼를 사용하는 폰트를 위한 폰트 노드
    bool m_bAnalyseRequired;            // True이면 마지막 분석을 한후 문자열이 변경되었다.
    SCRIPT_STRING_ANALYSIS m_Analysis;  // 현재 문자열의 분석기

private:
	// Uniscribe API의 빈 구현
    static HRESULT WINAPI Dummy_ScriptApplyDigitSubstitution( const SCRIPT_DIGITSUBSTITUTE*, SCRIPT_CONTROL*, SCRIPT_STATE* ) { return E_NOTIMPL; }
    static HRESULT WINAPI Dummy_ScriptStringAnalyse( HDC, const void *, int, int, int, DWORD, int, SCRIPT_CONTROL*, SCRIPT_STATE*, const int*, SCRIPT_TABDEF*, const BYTE*, SCRIPT_STRING_ANALYSIS* ) { return E_NOTIMPL; }
    static HRESULT WINAPI Dummy_ScriptStringCPtoX( SCRIPT_STRING_ANALYSIS, int, BOOL, int* ) { return E_NOTIMPL; }
    static HRESULT WINAPI Dummy_ScriptStringXtoCP( SCRIPT_STRING_ANALYSIS, int, int*, int* ) { return E_NOTIMPL; }
    static HRESULT WINAPI Dummy_ScriptStringFree( SCRIPT_STRING_ANALYSIS* ) { return E_NOTIMPL; }
    static const SCRIPT_LOGATTR* WINAPI Dummy_ScriptString_pLogAttr( SCRIPT_STRING_ANALYSIS ) { return NULL; }
    static const int* WINAPI Dummy_ScriptString_pcOutChars( SCRIPT_STRING_ANALYSIS ) { return NULL; }

    // 함수 포인터
    static HRESULT (WINAPI *_ScriptApplyDigitSubstitution)( const SCRIPT_DIGITSUBSTITUTE*, SCRIPT_CONTROL*, SCRIPT_STATE* );
    static HRESULT (WINAPI *_ScriptStringAnalyse)( HDC, const void *, int, int, int, DWORD, int, SCRIPT_CONTROL*, SCRIPT_STATE*, const int*, SCRIPT_TABDEF*, const BYTE*, SCRIPT_STRING_ANALYSIS* );
    static HRESULT (WINAPI *_ScriptStringCPtoX)( SCRIPT_STRING_ANALYSIS, int, BOOL, int* );
    static HRESULT (WINAPI *_ScriptStringXtoCP)( SCRIPT_STRING_ANALYSIS, int, int*, int* );
    static HRESULT (WINAPI *_ScriptStringFree)( SCRIPT_STRING_ANALYSIS* );
    static const SCRIPT_LOGATTR* (WINAPI *_ScriptString_pLogAttr)( SCRIPT_STRING_ANALYSIS );
    static const int* (WINAPI *_ScriptString_pcOutChars)( SCRIPT_STRING_ANALYSIS );

    static HINSTANCE s_hDll;  // Uniscribe DLL 핸들

};

//-----------------------------------------------------------------------------
// 에디트박스 컨트롤
//-----------------------------------------------------------------------------
class CDXUTEditBox : public CDXUTControl
{
public:
    CDXUTEditBox( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTEditBox();

    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual void UpdateRects();
    virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
    virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
    virtual void OnFocusIn();

    void SetText( LPCWSTR wszText, bool bSelected = false );
    LPCWSTR GetText() { return m_Buffer.GetBuffer(); }
    int GetTextLength() { return m_Buffer.GetTextSize(); }  // 문자들중 NULL을 제외한 텍스트 길이를 리턴한다.
    HRESULT GetTextCopy( LPWSTR strDest, UINT bufferCount );
    void ClearText();
    virtual void SetTextColor( D3DCOLOR Color ) { m_TextColor = Color; }  // 텍스트 컬러
    void SetSelectedTextColor( D3DCOLOR Color ) { m_SelTextColor = Color; }  // 선택된 텍스트 컬러
    void SetSelectedBackColor( D3DCOLOR Color ) { m_SelBkColor = Color; }  // 선택된 배경 컬러
    void SetCaretColor( D3DCOLOR Color ) { m_CaretColor = Color; }  // 캐럿 컬러
    void SetBorderWidth( int nBorder ) { m_nBorder = nBorder; UpdateRects(); }  // 윈도우의 테두리
    void SetSpacing( int nSpacing ) { m_nSpacing = nSpacing; UpdateRects(); }
    void ParseFloatArray( float *pNumbers, int nCount );
    void SetTextFloatArray( const float *pNumbers, int nCount );

protected:
    void PlaceCaret( int nCP );
    void DeleteSelectionText();
    void ResetCaretBlink();
    void CopyToClipboard();
    void PasteFromClipboard();

    CUniBuffer m_Buffer;     // 텍스트를 가지는 버퍼
    int      m_nBorder;      // 윈도우 테두리
    int      m_nSpacing;     // 텍스트와 테두리의 변 사이의 공간
    RECT     m_rcText;       // 텍스트 경계 사각형
    RECT     m_rcRender[9];  // 요소들을 랜더링하기 위한 편리한 사각형
    double   m_dfBlink;      // 캣럿 깜빡임 속도(millisec)
    double   m_dfLastBlink;  // 캐럿 깜빡임의 최근 타임 스탬프
    bool     m_bCaretOn;     // 캐럿이 현재 보이는지 나타내는 플래그
    int      m_nCaret;       // 문자들중에서 캐럿 위치
    bool     m_bInsertMode;  // 만약 True이면, 컨트롤은 삽입 모드이고 아니면 덥쳐쓰기 모드이다.
    int      m_nSelStart;    // 선택의 시작 위치. 캐럿은 끝을 표시한다.
    int      m_nFirstVisible;// 에디트 컨트롤에서 처음 보여지는 문자열
    D3DCOLOR m_TextColor;    // 텍스트 색
    D3DCOLOR m_SelTextColor; // 선택된 텍스트 색
    D3DCOLOR m_SelBkColor;   // 선택된 배경 색
    D3DCOLOR m_CaretColor;   // 캐럿 색

    // 마우스 명세
    bool m_bMouseDrag;       // True이면 처리중에 드래그를 나타낸다.

    // Static
    static bool s_bHideCaret;   // 만약 True이면, 우리는 캐럿을 랜더링하지 않는다.
};


//-----------------------------------------------------------------------------
// IME가 가능한 에디트박스 컨트롤
//-----------------------------------------------------------------------------
#define MAX_CANDLIST 10
#define MAX_COMPSTRING_SIZE 256

class CDXUTIMEEditBox : public CDXUTEditBox
{
protected:
	// IMM32 API의 빈 구현
    static INPUTCONTEXT* WINAPI Dummy_ImmLockIMC( HIMC ) { return NULL; }
    static BOOL WINAPI Dummy_ImmUnlockIMC( HIMC ) { return FALSE; }
    static LPVOID WINAPI Dummy_ImmLockIMCC( HIMCC ) { return NULL; }
    static BOOL WINAPI Dummy_ImmUnlockIMCC( HIMCC ) { return FALSE; }
    static BOOL WINAPI Dummy_ImmDisableTextFrameService( DWORD ) { return TRUE; }
    static LONG WINAPI Dummy_ImmGetCompositionStringW( HIMC, DWORD, LPVOID, DWORD ) { return IMM_ERROR_GENERAL; }
    static DWORD WINAPI Dummy_ImmGetCandidateListW( HIMC, DWORD, LPCANDIDATELIST, DWORD ) { return 0; }
    static HIMC WINAPI Dummy_ImmGetContext( HWND ) { return NULL; }
    static BOOL WINAPI Dummy_ImmReleaseContext( HWND, HIMC ) { return FALSE; }
    static HIMC WINAPI Dummy_ImmAssociateContext( HWND, HIMC ) { return NULL; }
    static BOOL WINAPI Dummy_ImmGetOpenStatus( HIMC ) { return 0; }
    static BOOL WINAPI Dummy_ImmSetOpenStatus( HIMC, BOOL ) { return 0; }
    static BOOL WINAPI Dummy_ImmGetConversionStatus( HIMC, LPDWORD, LPDWORD ) { return 0; }
    static HWND WINAPI Dummy_ImmGetDefaultIMEWnd( HWND ) { return NULL; }
    static UINT WINAPI Dummy_ImmGetIMEFileNameA( HKL, LPSTR, UINT ) { return 0; }
    static UINT WINAPI Dummy_ImmGetVirtualKey( HWND ) { return 0; }
    static BOOL WINAPI Dummy_ImmNotifyIME( HIMC, DWORD, DWORD, DWORD ) { return FALSE; }
    static BOOL WINAPI Dummy_ImmSetConversionStatus( HIMC, DWORD, DWORD ) { return FALSE; }
    static BOOL WINAPI Dummy_ImmSimulateHotKey( HWND, DWORD ) { return FALSE; }
    static BOOL WINAPI Dummy_ImmIsIME( HKL ) { return FALSE; }

    // 전통적인 중국의 IME
    static UINT WINAPI Dummy_GetReadingString( HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT ) { return 0; }
    static BOOL WINAPI Dummy_ShowReadingWindow( HIMC, BOOL ) { return FALSE; }

    // 버전 라이브러리 임포트
    static BOOL APIENTRY Dummy_VerQueryValueA( const LPVOID, LPSTR, LPVOID *, PUINT ) { return 0; }
    static BOOL APIENTRY Dummy_GetFileVersionInfoA( LPSTR, DWORD, DWORD, LPVOID ) { return 0; }
    static DWORD APIENTRY Dummy_GetFileVersionInfoSizeA( LPSTR, LPDWORD ) { return 0; }

    // 함수 포인터: IMM32
    static INPUTCONTEXT* (WINAPI * _ImmLockIMC)( HIMC );
    static BOOL (WINAPI * _ImmUnlockIMC)( HIMC );
    static LPVOID (WINAPI * _ImmLockIMCC)( HIMCC );
    static BOOL (WINAPI * _ImmUnlockIMCC)( HIMCC );
    static BOOL (WINAPI * _ImmDisableTextFrameService)( DWORD );
    static LONG (WINAPI * _ImmGetCompositionStringW)( HIMC, DWORD, LPVOID, DWORD );
    static DWORD (WINAPI * _ImmGetCandidateListW)( HIMC, DWORD, LPCANDIDATELIST, DWORD );
    static HIMC (WINAPI * _ImmGetContext)( HWND );
    static BOOL (WINAPI * _ImmReleaseContext)( HWND, HIMC );
    static HIMC (WINAPI * _ImmAssociateContext)( HWND, HIMC );
    static BOOL (WINAPI * _ImmGetOpenStatus)( HIMC );
    static BOOL (WINAPI * _ImmSetOpenStatus)( HIMC, BOOL );
    static BOOL (WINAPI * _ImmGetConversionStatus)( HIMC, LPDWORD, LPDWORD );
    static HWND (WINAPI * _ImmGetDefaultIMEWnd)( HWND );
    static UINT (WINAPI * _ImmGetIMEFileNameA)( HKL, LPSTR, UINT );
    static UINT (WINAPI * _ImmGetVirtualKey)( HWND );
    static BOOL (WINAPI * _ImmNotifyIME)( HIMC, DWORD, DWORD, DWORD );
    static BOOL (WINAPI * _ImmSetConversionStatus)( HIMC, DWORD, DWORD );
    static BOOL (WINAPI * _ImmSimulateHotKey)( HWND, DWORD );
    static BOOL (WINAPI * _ImmIsIME)( HKL );

    // 함수 포인터:전통적인 중국의 IME
    static UINT (WINAPI * _GetReadingString)( HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT );
    static BOOL (WINAPI * _ShowReadingWindow)( HIMC, BOOL );

    // 함수 포인터: 버전 라이브러리 임포트
    static BOOL (APIENTRY * _VerQueryValueA)( const LPVOID, LPSTR, LPVOID *, PUINT );
    static BOOL (APIENTRY * _GetFileVersionInfoA)( LPSTR, DWORD, DWORD, LPVOID );
    static DWORD (APIENTRY * _GetFileVersionInfoSizeA)( LPSTR, LPDWORD );

public:
    CDXUTIMEEditBox( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTIMEEditBox();

    static void Initialize();
    static void Uninitialize();

    static  HRESULT StaticOnCreateDevice();
    static  bool StaticMsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

    static  void EnableImeSystem( bool bEnable );

    virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
    virtual bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual void UpdateRects();
    virtual void OnFocusIn();
    virtual void OnFocusOut();

    void PumpMessage();

    virtual void RenderCandidateReadingWindow( IDirect3DDevice9* pd3dDevice, float fElapsedTime, bool bReading );
    virtual void RenderComposition( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
    virtual void RenderIndicator( IDirect3DDevice9* pd3dDevice, float fElapsedTime );

protected:
    static WORD GetLanguage() { return LOWORD( s_hklCurrent ); }
    static WORD GetPrimaryLanguage() { return PRIMARYLANGID( LOWORD( s_hklCurrent ) ); }
    static WORD GetSubLanguage() { return SUBLANGID( LOWORD( s_hklCurrent ) ); }
    static void SendKey( BYTE nVirtKey );
    static DWORD GetImeId( UINT uIndex = 0 );
    static void CheckInputLocale();
    static void CheckToggleState();
    static void SetupImeApi();
    static void ResetCompositionString();
    void TruncateCompString( bool bUseBackSpace = true, int iNewStrLen = 0 );
    void FinalizeString( bool bSend );
    static void GetReadingWindowOrientation( DWORD dwId );
    static void GetPrivateReadingString();

    void SendCompString();

protected:
    enum { INDICATOR_NON_IME, INDICATOR_CHS, INDICATOR_CHT, INDICATOR_KOREAN, INDICATOR_JAPANESE };
    enum IMESTATE { IMEUI_STATE_OFF, IMEUI_STATE_ON, IMEUI_STATE_ENGLISH };

    struct CCandList
    {
        WCHAR awszCandidate[MAX_CANDLIST][256];
        CUniBuffer HoriCand; // 후보 리스트 문자열(수평 후보 윈도우 중)
        int   nFirstSelected; // 수평 후보 윈도우 중에서 선택된 첫번째 문자의 위치
        int   nHoriSelectedLen; // 수평후보들중에서 선택된 문자열의 길이
        DWORD dwCount;       // 후보 리스트중에서 유효한 요소의 수
        DWORD dwSelection;   // 페이지 상위에 관련된 현재 선택된 후보 요소
        DWORD dwPageSize;
        int   nReadingError; // 에러 문자의 인덱스
        bool  bShowWindow;   // 후보 리스트 윈도우가 보여지는가?
        RECT  rcCandidate;   // 후보 사각형은 랜더링되기 전에 계산되고 각 시간 채워진다.
    };

    struct CInputLocale
    {
        HKL   m_hKL;            // 키보드 레이아웃
        WCHAR m_wszLangAbb[3];  // 언어 약어
        WCHAR m_wszLang[64];    // 지역화된(Localized) 언어명
    };

	// 응용프로그램 - wide data
    static HINSTANCE s_hDllImm32;         // IMM32 DLL 핸들
    static HINSTANCE s_hDllVer;           // 버전 DLL 핸들
    static HIMC      s_hImcDef;           // 기본 입력 컨텍스트

    static HKL     s_hklCurrent;          // 프로세스의 현재 키보드 레이아웃
    static bool    s_bVerticalCand;       // 후보가 수직으로 나열되었나?
    static LPWSTR  s_wszCurrIndicator;    // 현재 입력 지역에 일치하는 문자열의 지시자의 포인터
    static WCHAR   s_aszIndicator[5][3];  // 현재 입력 지역을 나타내기위해 그리는 문자열
    static bool    s_bInsertOnType;       // 키가 눌려지자 마자 캐릭터가 삽입한다.(Korean behavior)
    static HINSTANCE s_hDllIme;           // 현재 IME 모듈의 인스턴스 핸들
    static IMESTATE  s_ImeState;          // IME 전역 상태
    static bool    s_bEnableImeSystem;    // IME 시스템이 활성화되었나?
    static POINT   s_ptCompString;        // 조합 문자열 위치. 모든 프레임이 갱신되었다.
    static int     s_nCompCaret;          // 조합된 문자열의 캐럿 위치.
    static int     s_nFirstTargetConv;    // 조합 문자열의 문자 변경할 첫번재 목표의 인덱스. 만약 없다면 -1
    static CUniBuffer s_CompString;       // 조합 문자열을 가지는 버퍼(우리는 버퍼의 길이를 수정한다.)
    static BYTE    s_abCompStringAttr[MAX_COMPSTRING_SIZE];
    static DWORD   s_adwCompStringClause[MAX_COMPSTRING_SIZE];
    static WCHAR   s_wszReadingString[32];// 수평으로 읽기만 위한 윈도우로 사용된다.(왜?)
    static CCandList s_CandList;          // 후보리스트와 관련된 데이터
    static bool    s_bShowReadingWindow;  // 읽기 윈도우가 보여지는지 의미한다.
    static bool    s_bHorizontalReading;  // 읽기 윈도우가 수평인지 수직인지 의미한다.
    static bool    s_bChineseIME;
    static CGrowableArray< CInputLocale > s_Locale; // Array of loaded keyboard layout on system

    // 다양한 IME 요소의 색
    D3DCOLOR       m_ReadingColor;        // 읽는 문자열 색
    D3DCOLOR       m_ReadingWinColor;     // 읽는 윈도우 색
    D3DCOLOR       m_ReadingSelColor;     // 읽는 문자열 중에서 선택된 문자
    D3DCOLOR       m_ReadingSelBkColor;   // 읽는 문자열 중에서 선택된 문자의 배경 색
    D3DCOLOR       m_CandidateColor;      // 후보 문자열 색
    D3DCOLOR       m_CandidateWinColor;   // 후보 윈도우 색
    D3DCOLOR       m_CandidateSelColor;   // 선택된 후보 문자열 색
    D3DCOLOR       m_CandidateSelBkColor; // 선택된 후보 배경색
    D3DCOLOR       m_CompColor;           // 조합 문자열 색
    D3DCOLOR       m_CompWinColor;        // 조합 문자열 윈도우 색
    D3DCOLOR       m_CompCaretColor;      // 조합 문자열 캐럿 색
    D3DCOLOR       m_CompTargetColor;     // 목표 조합 문자열가 변경한 색
    D3DCOLOR       m_CompTargetBkColor;   // 목표 조합 문자열가 변경한 배경
    D3DCOLOR       m_CompTargetNonColor;  // 목표 조합 문자열가 변경 안된 색
    D3DCOLOR       m_CompTargetNonBkColor;// 목표 조합 문자열가 변경 안된 배경
    D3DCOLOR       m_IndicatorImeColor;   // IME를 위한 텍스트 색 지시자
    D3DCOLOR       m_IndicatorEngColor;   // 영어 텍스트 색 지시자
    D3DCOLOR       m_IndicatorBkColor;    // 텍스트 배경색 지시자

    // 에디트 컨트롤 명세 데이터
    int            m_nIndicatorWidth;     // 심볼을 표시하는 넓이
    RECT           m_rcIndicator;         // 버튼을 그리기위한 사각형

#if defined(DEBUG) || defined(_DEBUG)
    static bool    m_bIMEStaticMsgProcCalled;
#endif
};



#endif // DXUT_GUI_H