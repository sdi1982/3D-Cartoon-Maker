//
//
//  @ Project : 3D ī�� ����Ŀ
//  @ File Name : LmModel.cpp
//  @ Date : 2007-10-11
//  @ Author : �ŵ���
//
//

#include "dxstdafx.h"
#include "LmModel.h"
#include "LmModelManager.h"


LmModel::LmModel(LPCWSTR _pcwsName) 
{
	m_wstrXFile = L"";
	m_wstrName = _pcwsName;
	D3DXMatrixIdentity(&m_matWorld);


	m_vCenter.x = 0.0f;
	m_vCenter.y = 0.0f;
	m_vCenter.z = 0.0f;
	m_fRadius = 0.0f;
	m_fAngle = 0.0f;
	m_bClone = FALSE;
}

LmModel::LmModel(LPCWSTR _pcwsName, LPCWSTR _pcwsXFile )
{
	m_wstrXFile = _pcwsXFile;
	m_wstrName = _pcwsName;
	D3DXMatrixIdentity(&m_matWorld);

	m_vCenter.x = 0.0f;
	m_vCenter.y = 0.0f;
	m_vCenter.z = 0.0f;
	m_fRadius = 0.0f;
	m_fAngle = 0.0f;
	m_bClone = FALSE;
}

LmModel::~LmModel() 
{

}

VOID LmModel::SetXFile( LPCWSTR _pcwsXFile )
{
	m_wstrXFile = _pcwsXFile;
}