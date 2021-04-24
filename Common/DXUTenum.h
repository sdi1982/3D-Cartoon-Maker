//--------------------------------------------------------------------------------------
// File: DXUTEnum.h
//
// Enumerates D3D adapters, devices, modes, etc.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#ifndef DXUT_ENUM_H
#define DXUT_ENUM_H

//--------------------------------------------------------------------------------------
// 전방 선언
//--------------------------------------------------------------------------------------
class CD3DEnumAdapterInfo;
class CD3DEnumDeviceInfo;
struct CD3DEnumDeviceSettingsCombo;
struct CD3DEnumDSMSConflict;


//--------------------------------------------------------------------------------------
// 사용 가능한 Direct3D 아답터, 장치, 모드, 등을 열거한다.
// 전역 인스턴스에 접근하기위해 DXUTGetEnumeration()를 사용한다.
//--------------------------------------------------------------------------------------
class CD3DEnumeration
{
public:
	// 이것들은 Enumerate()을 호출하기 전에 불려져야한다.
    //
	// 장치 선택과 장치 설정 다이얼로그에 영향을 주는 열거 객체들의 내용을 
	// 제어하기위해 이것들의 호출과 IsDeviceAcceptable를 사용한다.
    void SetRequirePostPixelShaderBlending( bool bRequire ) { m_bRequirePostPixelShaderBlending = bRequire; }
    void SetResolutionMinMax( UINT nMinWidth, UINT nMinHeight, UINT nMaxWidth, UINT nMaxHeight );  
    void SetRefreshMinMax( UINT nMin, UINT nMax );
    void SetMultisampleQualityMax( UINT nMax );    
    void GetPossibleVertexProcessingList( bool* pbSoftwareVP, bool* pbHardwareVP, bool* pbPureHarewareVP, bool* pbMixedVP );
    void SetPossibleVertexProcessingList( bool bSoftwareVP, bool bHardwareVP, bool bPureHarewareVP, bool bMixedVP );
    CGrowableArray<D3DFORMAT>* GetPossibleDepthStencilFormatList();   
    CGrowableArray<D3DMULTISAMPLE_TYPE>* GetPossibleMultisampleTypeList();   
    CGrowableArray<UINT>* GetPossiblePresentIntervalList();
    void ResetPossibleDepthStencilFormats();
    void ResetPossibleMultisampleTypeList();
    void ResetPossiblePresentIntervalList();

	// 사용가능한 D3D 아답터, 장치, 모드 등을 열거하기위해 Enumerate()를 호출한다.
    HRESULT Enumerate( IDirect3D9* pD3D = NULL,
                       LPDXUTCALLBACKISDEVICEACCEPTABLE IsDeviceAcceptableFunc = NULL,
                       void* pIsDeviceAcceptableFuncUserContext = NULL );

	// Enumerate()를 호출한 뒤에 이것들을 호출한다.
    CGrowableArray<CD3DEnumAdapterInfo*>*   GetAdapterInfoList();  
    CD3DEnumAdapterInfo*                    GetAdapterInfo( UINT AdapterOrdinal );  
    CD3DEnumDeviceInfo*                     GetDeviceInfo( UINT AdapterOrdinal, D3DDEVTYPE DeviceType );    
    CD3DEnumDeviceSettingsCombo*            GetDeviceSettingsCombo( DXUTDeviceSettings* pDeviceSettings ) { return GetDeviceSettingsCombo( pDeviceSettings->AdapterOrdinal, pDeviceSettings->DeviceType, pDeviceSettings->AdapterFormat, pDeviceSettings->pp.BackBufferFormat, pDeviceSettings->pp.Windowed ); }
    CD3DEnumDeviceSettingsCombo*            GetDeviceSettingsCombo( UINT AdapterOrdinal, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL Windowed );  

    ~CD3DEnumeration();

private:
    friend CD3DEnumeration* DXUTGetEnumeration(); 

	// 전역 인스턴스에 접근하기위해 DXUTGetEnumeration()를 사용한다.
    CD3DEnumeration();

    IDirect3D9* m_pD3D;                                    
    LPDXUTCALLBACKISDEVICEACCEPTABLE m_IsDeviceAcceptableFunc;
    void* m_pIsDeviceAcceptableFuncUserContext;
    bool m_bRequirePostPixelShaderBlending;
    CGrowableArray<D3DFORMAT> m_DepthStecilPossibleList;
    CGrowableArray<D3DMULTISAMPLE_TYPE> m_MultiSampleTypeList;
    CGrowableArray<UINT> m_PresentIntervalList;

    bool m_bSoftwareVP;
    bool m_bHardwareVP;
    bool m_bPureHarewareVP;
    bool m_bMixedVP;

    UINT m_nMinWidth;
    UINT m_nMaxWidth;
    UINT m_nMinHeight;
    UINT m_nMaxHeight;
    UINT m_nRefreshMin;
    UINT m_nRefreshMax;
    UINT m_nMultisampleQualityMax;

	// 유일한 아답터순서를 가진 CD3DEnumAdapterInfo*의 배열
    CGrowableArray<CD3DEnumAdapterInfo*> m_AdapterInfoList;  

    HRESULT EnumerateDevices( CD3DEnumAdapterInfo* pAdapterInfo, CGrowableArray<D3DFORMAT>* pAdapterFormatList );
    HRESULT EnumerateDeviceCombos( CD3DEnumAdapterInfo* pAdapterInfo, CD3DEnumDeviceInfo* pDeviceInfo, CGrowableArray<D3DFORMAT>* pAdapterFormatList );
    void BuildDepthStencilFormatList( CD3DEnumDeviceSettingsCombo* pDeviceCombo );
    void BuildMultiSampleTypeList( CD3DEnumDeviceSettingsCombo* pDeviceCombo );
    void BuildDSMSConflictList( CD3DEnumDeviceSettingsCombo* pDeviceCombo );
    void BuildPresentIntervalList( CD3DEnumDeviceInfo* pDeviceInfo, CD3DEnumDeviceSettingsCombo* pDeviceCombo );
    void ClearAdapterInfoList();
};

CD3DEnumeration* DXUTGetEnumeration(); 


//--------------------------------------------------------------------------------------
// 클래스는 시스템에 설치된 유일한 아답터의 순서를 가진 아답터를 설명한다.
//--------------------------------------------------------------------------------------
class CD3DEnumAdapterInfo
{
public:
    ~CD3DEnumAdapterInfo();

    UINT AdapterOrdinal;
    D3DADAPTER_IDENTIFIER9 AdapterIdentifier;
    WCHAR szUniqueDescription[256];

    CGrowableArray<D3DDISPLAYMODE> displayModeList; // Array of supported D3DDISPLAYMODEs
    CGrowableArray<CD3DEnumDeviceInfo*> deviceInfoList; // Array of CD3DEnumDeviceInfo* with unique supported DeviceTypes
};


//--------------------------------------------------------------------------------------
// 클래스는 유일한 지원가능한 장치 타입을 포함한 Direct3D 설명한다.
//--------------------------------------------------------------------------------------
class CD3DEnumDeviceInfo
{
public:
    ~CD3DEnumDeviceInfo();

    UINT AdapterOrdinal;
    D3DDEVTYPE DeviceType;
    D3DCAPS9 Caps;

	// 아답터포멧, 백버퍼포멧과 윈도우의 집합을 가지는 
	// CD3DEnumDeviceSettingsCombo* 목록
    CGrowableArray<CD3DEnumDeviceSettingsCombo*> deviceSettingsComboList; 
};


//--------------------------------------------------------------------------------------
// 구조체는 장치 아답터 포멧, 백버퍼 포멧, 특별한 Direct3D 장치에 적합한 윈도우와 app의 
// 고유한 조합을 가지는 장치 설정을 나타낸다.
//--------------------------------------------------------------------------------------
struct CD3DEnumDeviceSettingsCombo
{
    UINT AdapterOrdinal;
    D3DDEVTYPE DeviceType;
    D3DFORMAT AdapterFormat;
    D3DFORMAT BackBufferFormat;
    BOOL Windowed;

    CGrowableArray<D3DFORMAT> depthStencilFormatList; // List of D3DFORMATs
    CGrowableArray<D3DMULTISAMPLE_TYPE> multiSampleTypeList; // List of D3DMULTISAMPLE_TYPEs
    CGrowableArray<DWORD> multiSampleQualityList; // List of number of quality levels for each multisample type
    CGrowableArray<UINT> presentIntervalList; // List of D3DPRESENT flags
    CGrowableArray<CD3DEnumDSMSConflict> DSMSConflictList; // List of CD3DEnumDSMSConflict

    CD3DEnumAdapterInfo* pAdapterInfo;
    CD3DEnumDeviceInfo* pDeviceInfo;
};


//--------------------------------------------------------------------------------------
// 깊이/스탠실 버퍼 포멧은 멀티샘플 타입과 호환성이 없다.
//--------------------------------------------------------------------------------------
struct CD3DEnumDSMSConflict
{
    D3DFORMAT DSFormat;
    D3DMULTISAMPLE_TYPE MSType;
};


#endif
