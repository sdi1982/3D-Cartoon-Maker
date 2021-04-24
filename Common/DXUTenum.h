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
// ���� ����
//--------------------------------------------------------------------------------------
class CD3DEnumAdapterInfo;
class CD3DEnumDeviceInfo;
struct CD3DEnumDeviceSettingsCombo;
struct CD3DEnumDSMSConflict;


//--------------------------------------------------------------------------------------
// ��� ������ Direct3D �ƴ���, ��ġ, ���, ���� �����Ѵ�.
// ���� �ν��Ͻ��� �����ϱ����� DXUTGetEnumeration()�� ����Ѵ�.
//--------------------------------------------------------------------------------------
class CD3DEnumeration
{
public:
	// �̰͵��� Enumerate()�� ȣ���ϱ� ���� �ҷ������Ѵ�.
    //
	// ��ġ ���ð� ��ġ ���� ���̾�α׿� ������ �ִ� ���� ��ü���� ������ 
	// �����ϱ����� �̰͵��� ȣ��� IsDeviceAcceptable�� ����Ѵ�.
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

	// ��밡���� D3D �ƴ���, ��ġ, ��� ���� �����ϱ����� Enumerate()�� ȣ���Ѵ�.
    HRESULT Enumerate( IDirect3D9* pD3D = NULL,
                       LPDXUTCALLBACKISDEVICEACCEPTABLE IsDeviceAcceptableFunc = NULL,
                       void* pIsDeviceAcceptableFuncUserContext = NULL );

	// Enumerate()�� ȣ���� �ڿ� �̰͵��� ȣ���Ѵ�.
    CGrowableArray<CD3DEnumAdapterInfo*>*   GetAdapterInfoList();  
    CD3DEnumAdapterInfo*                    GetAdapterInfo( UINT AdapterOrdinal );  
    CD3DEnumDeviceInfo*                     GetDeviceInfo( UINT AdapterOrdinal, D3DDEVTYPE DeviceType );    
    CD3DEnumDeviceSettingsCombo*            GetDeviceSettingsCombo( DXUTDeviceSettings* pDeviceSettings ) { return GetDeviceSettingsCombo( pDeviceSettings->AdapterOrdinal, pDeviceSettings->DeviceType, pDeviceSettings->AdapterFormat, pDeviceSettings->pp.BackBufferFormat, pDeviceSettings->pp.Windowed ); }
    CD3DEnumDeviceSettingsCombo*            GetDeviceSettingsCombo( UINT AdapterOrdinal, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL Windowed );  

    ~CD3DEnumeration();

private:
    friend CD3DEnumeration* DXUTGetEnumeration(); 

	// ���� �ν��Ͻ��� �����ϱ����� DXUTGetEnumeration()�� ����Ѵ�.
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

	// ������ �ƴ��ͼ����� ���� CD3DEnumAdapterInfo*�� �迭
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
// Ŭ������ �ý��ۿ� ��ġ�� ������ �ƴ����� ������ ���� �ƴ��͸� �����Ѵ�.
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
// Ŭ������ ������ ���������� ��ġ Ÿ���� ������ Direct3D �����Ѵ�.
//--------------------------------------------------------------------------------------
class CD3DEnumDeviceInfo
{
public:
    ~CD3DEnumDeviceInfo();

    UINT AdapterOrdinal;
    D3DDEVTYPE DeviceType;
    D3DCAPS9 Caps;

	// �ƴ�������, ���������� �������� ������ ������ 
	// CD3DEnumDeviceSettingsCombo* ���
    CGrowableArray<CD3DEnumDeviceSettingsCombo*> deviceSettingsComboList; 
};


//--------------------------------------------------------------------------------------
// ����ü�� ��ġ �ƴ��� ����, ����� ����, Ư���� Direct3D ��ġ�� ������ ������� app�� 
// ������ ������ ������ ��ġ ������ ��Ÿ����.
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
// ����/���Ľ� ���� ������ ��Ƽ���� Ÿ�԰� ȣȯ���� ����.
//--------------------------------------------------------------------------------------
struct CD3DEnumDSMSConflict
{
    D3DFORMAT DSFormat;
    D3DMULTISAMPLE_TYPE MSType;
};


#endif
