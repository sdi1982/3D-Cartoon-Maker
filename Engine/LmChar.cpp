//
//
//  @ Project : 3D ī�� ����Ŀ
//  @ File Name : LmCharacter.cpp
//  @ Date : 2007-10-12
//  @ Author : �ŵ���
//
//

#include "dxstdafx.h"
#include "LmChar.h"
#include "LmCamera.h"

LmChar::LmChar(LPCWSTR _pcwsName) : LmThing(_pcwsName)
{
	m_pDevice = NULL;
	m_pAnimate = NULL;
	m_uLastActOffset = 0;
	m_listInfoAct.resize(1);
	m_pTalkBox = NULL;
}

LmChar::LmChar(LPCWSTR _pcwsName, LPCWSTR _pcwsXFile) : LmThing(_pcwsName, _pcwsXFile)
{
	m_pDevice = NULL;
	m_pAnimate = NULL;
	m_uLastActOffset = 0;
	m_listInfoAct.resize(1);
	m_pTalkBox = NULL;
}

LmChar::~LmChar()
{
	SAFE_DELETE(m_pAnimate);
	SAFE_DELETE(m_pTalkBox);
}

LmChar* LmChar::Clone( LPCWSTR _pcwsClone )
{
	LmChar* pCloneChar = new LmChar(_pcwsClone); // ��ü�� ����

	if(pCloneChar == NULL)
		return NULL;

	// ������ ������ �״�� ��������� �Ѵ�. =������ �������̵�� ����
	*pCloneChar = *this;

	return pCloneChar;
}

LmChar& LmChar::operator=( LmChar& _rhs )
{
	// �ڱ� �ڽ��̶� ���ٸ� �׳� ����
	if(this != &_rhs)
	{
		m_wstrXFile = _rhs.m_wstrXFile;
		m_matWorld = _rhs.m_matWorld;
		m_vCenter = _rhs.m_vCenter;	// ��豸 �߽�
		m_fRadius = _rhs.m_fRadius;	// ��豸 ������
		m_bClone = TRUE;


		// �׼� ���� ����Ʈ
		vector<LmInfoAct>::iterator itrInfoAct;
		for(itrInfoAct = _rhs.m_listInfoAct.begin();
			itrInfoAct != _rhs.m_listInfoAct.end(); 
			itrInfoAct++)
		{
			m_listInfoAct.push_back(*itrInfoAct);
		}
		
		// �׼� Ű ����Ʈ
		vector<LmKeyAct>::iterator itrKeyAct;
		for(itrKeyAct = _rhs.m_listKeyAct.begin();
			itrKeyAct != _rhs.m_listKeyAct.end(); 
			itrKeyAct++)
		{
			m_listKeyAct.push_back(*itrKeyAct);
		}

		// �׼� ������
		m_uLastActOffset = _rhs.m_uLastActOffset;
	
		// ��ǳ�� ����Ʈ
		vector<LmKeyTalk>::iterator itrTalk;
		for(itrTalk = _rhs.m_listKeyTalkBox.begin();
			itrTalk != _rhs.m_listKeyTalkBox.end(); 
			itrTalk++)
		{
			m_listKeyTalkBox.push_back(*itrTalk);
		}

	}

	return *this;
}

VOID LmChar::SetCurrFrame( UINT _uFrame )
{
	// �׼� ���źκ�
	switch(m_listInfoAct[_uFrame].uActType)
	{
	case ACT_NONE:
		break;
	case ACT_TURN_LEFT:
		m_fAngle = m_listInfoAct[_uFrame].fAngle;
		m_fPosX = m_listInfoAct[_uFrame].vPos.x;
		m_fPosZ = m_listInfoAct[_uFrame].vPos.z;
		break;
	case ACT_TURN_RIGHT:
		m_fAngle = m_listInfoAct[_uFrame].fAngle;
		m_fPosX = m_listInfoAct[_uFrame].vPos.x;
		m_fPosZ = m_listInfoAct[_uFrame].vPos.z;
		break;
	case ACT_WALK:
		m_fAngle = m_listInfoAct[_uFrame].fAngle;
		m_fPosX = m_listInfoAct[_uFrame].vPos.x;
		m_fPosZ = m_listInfoAct[_uFrame].vPos.z;
		m_pAnimate->ResetTime();
		m_pAnimate->AdvanceTime(m_listInfoAct[_uFrame].fActTime);
		break;
	case ACT_SHAKE_HAND:
	case ACT_BOW:
	case ACT_JUMP:
	case ACT_TURNNING:
		m_pAnimate->ResetTime();
		m_pAnimate->AdvanceTime(m_listInfoAct[_uFrame].fActTime);
		break;
	}

	// ĳ���� ��ǳ�� ���� �κ�
	vector<LmKeyTalk>::iterator itrTalk;
	for(itrTalk = m_listKeyTalkBox.begin(); itrTalk != m_listKeyTalkBox.end(); itrTalk++)
	{
		if((itrTalk->uStartFrame <= _uFrame) &&
			(_uFrame <= itrTalk->uEndFrame))
		{
			m_pTalkBox->Show(TRUE);
			m_pTalkBox->SetMsg(itrTalk->wstrMsg.c_str());
			m_pTalkBox->SetColor(itrTalk->dxColor);

			return;
		}
	}

	m_pTalkBox->Show(FALSE);
}

VOID LmChar::Update( FLOAT _fTimeDelta )
{
	if(m_pAnimate)
	{
		D3DXMATRIX matRot;
		D3DXMatrixIdentity(&matRot);
		D3DXMatrixRotationY(&matRot, m_fAngle);

		D3DXMATRIX matTran;
		D3DXMatrixIdentity(&matTran);
		D3DXMatrixTranslation( &matTran, m_fPosX, 0.0f, m_fPosZ );

		D3DXMatrixMultiply(&m_matWorld, &matRot, &matTran);

		//////////////////////////////////////////////////////////////////////////
		//�ִϸ��̼� ó��
		D3DXMATRIX matCamera = *TheCamera->GetWorldMatrix();
		m_pAnimate->Update(_fTimeDelta, matCamera * m_matWorld);


		//////////////////////////////////////////////////////////////////////////
		// ��ǳ�� - ĳ������ ���� ��ǥ���� ī�޶� �並 ���ؼ� ȭ����� x,y���� ���غ���.
		D3DXMATRIX mat2D;
		D3DXMATRIX matView = *TheCamera->GetViewMatrix();
		D3DXMatrixMultiply(&mat2D, &m_matWorld, &matView);
		

		D3DXMatrixMultiply(&mat2D, &mat2D, TheCamera->GetProjMatrix());

		WCHAR buff[128];
		//swprintf_s(buff, 128, L"x:%f y:%f z:%f\n", mat2D._41, mat2D._42, mat2D._43);
		//OutputDebugStr(buff);
		swprintf_s(buff, 128, L"x:%f y:%f \n", mat2D._41/mat2D._43, mat2D._42/mat2D._43 );
		OutputDebugStr(buff);
		
		if(m_pTalkBox)
		{
			m_pTalkBox->SetPosF(mat2D._41/mat2D._43, mat2D._42/mat2D._43);
			m_pTalkBox->Update();
		}

		//////////////////////////////////////////////////////////////////////////
	}
}

VOID LmChar::Setup(UINT _uScreenWidth, UINT _uScreenHeight)
{
	if(m_pTalkBox)
		m_pTalkBox->Setup();
}

VOID LmChar::Cleanup()
{
	if(m_pTalkBox)
		m_pTalkBox->Cleanup();
}

VOID LmChar::Render( LPDIRECT3DDEVICE9 _pDevice )
{
	if(_pDevice)
	{
		if(!m_pAnimate) // �޽��� �ε���� �ʾҴٸ� X������ �о �ε��Ѵ�.
		{
			Init(_pDevice);
		}

		else
		{
			_pDevice->SetRenderState(D3DRS_LIGHTING, false);
			_pDevice->SetTransform(D3DTS_WORLD, &m_matWorld);
			m_pAnimate->Render(_pDevice);
			_pDevice->SetRenderState(D3DRS_LIGHTING, true);
		}

		if(!m_pTalkBox)
		{
			m_pTalkBox = new LmTalkBox;
			m_pTalkBox->Init(_pDevice);
		}

		// ��ǳ���� ����Ѵ�.
		else
			m_pTalkBox->Render(_pDevice);
	}
}

BOOL LmChar::Init(LPDIRECT3DDEVICE9 _pDevice )
{
	HRESULT hr;
	m_pDevice = _pDevice;
	m_pAnimate = new LmAnimate();

	wstring wstrXFile = m_wstrXFile + L".x";
	hr = m_pAnimate->Init(_pDevice,wstrXFile.c_str());
	

	if(FAILED(hr))
		return FALSE;

	return TRUE;
}

BOOL LmChar::Place(FLOAT _fStepX, FLOAT _fStepZ)
{
	m_fPosX = _fStepX;
	m_fPosZ = _fStepZ;

	m_listInfoAct[0].vPos.x = m_fPosX;
	m_listInfoAct[0].vPos.y = 0.0f;
	m_listInfoAct[0].vPos.z = m_fPosZ;
	return TRUE;
}


VOID LmChar::Rotate()
{
	m_fAngle += (D3DX_PI / 2.0f);
	m_listInfoAct[0].fAngle = m_fAngle;

	m_listInfoAct[0].iDir++;
	if(m_listInfoAct[0].iDir > 3)
		m_listInfoAct[0].iDir = 0;
}

// ��ȸ�� 20 ������
VOID LmChar::TurnLeft(UINT _uStartFrame)
{
	LmKeyAct ActKey(_uStartFrame, ACT_TURN_LEFT);
	m_listKeyAct.push_back(ActKey);

	int iCount = 1;

	INT i;
	for(i = (INT)_uStartFrame; i < (INT)(_uStartFrame + TURN_LEFT_FRAME_SIZE) - 1; i++)
	{
		// �׼� Ÿ��
		m_listInfoAct[i].uActType = ACT_TURN_LEFT;

		// ����
		m_listInfoAct[i].fAngle = m_listInfoAct[m_uLastActOffset].fAngle - iCount * (FLOAT)(D3DX_PI/2.0f/TURN_LEFT_FRAME_SIZE);
		if(m_listInfoAct[i].fAngle < 0.0f )
			m_listInfoAct[i].fAngle += D3DX_PI*2.0f;
	
		// ����
		m_listInfoAct[i].iDir = m_listInfoAct[m_uLastActOffset].iDir;

		// ��ġ
		m_listInfoAct[i].vPos = m_listInfoAct[m_uLastActOffset].vPos;
		iCount++;
	}

	// ������ ������ ó��

	// �׼� Ÿ��
	m_listInfoAct[i].uActType = ACT_TURN_LEFT;

	// ����
	m_listInfoAct[i].fAngle = m_listInfoAct[m_uLastActOffset].fAngle - D3DX_PI/2.0f;
	if(m_listInfoAct[i].fAngle < 0.0f )
		m_listInfoAct[i].fAngle += D3DX_PI*2.0f;

	// ���� ����
	m_listInfoAct[i].iDir = m_listInfoAct[m_uLastActOffset].iDir;
	m_listInfoAct[i].iDir--;
	if(m_listInfoAct[i].iDir < 0)
		m_listInfoAct[i].iDir = 3;

	// ��ġ
	m_listInfoAct[i].vPos = m_listInfoAct[m_uLastActOffset].vPos;


	// ������ �׼� ��ġ ����
	m_uLastActOffset += TURN_LEFT_FRAME_SIZE;
}

// ��ȸ�� 20 ������
VOID LmChar::TurnRight(UINT _uStartFrame)
{
	LmKeyAct ActKey(_uStartFrame, ACT_TURN_RIGHT);
	m_listKeyAct.push_back(ActKey);

	int iCount = 1;
	INT i;
	for(i = (INT)_uStartFrame; i < (INT)(_uStartFrame + TURN_LEFT_FRAME_SIZE) - 1; i++)
	{
		// �׼� Ÿ��
		m_listInfoAct[i].uActType = ACT_TURN_RIGHT;

		// ����
		m_listInfoAct[i].fAngle = m_listInfoAct[m_uLastActOffset].fAngle + iCount * (FLOAT)(D3DX_PI/2.0f/TURN_RIGHT_FRAME_SIZE);
		if(m_listInfoAct[i].fAngle > D3DX_PI*2.0f )
			m_listInfoAct[i].fAngle -= D3DX_PI*2.0f;

		// ����
		m_listInfoAct[i].iDir = m_listInfoAct[m_uLastActOffset].iDir;

		// ��ġ
		m_listInfoAct[i].vPos = m_listInfoAct[m_uLastActOffset].vPos;
		iCount++;
	}

	// ������ ������ ó��

	// �׼� Ÿ��
	m_listInfoAct[i].uActType = ACT_TURN_RIGHT;

	// ����
	m_listInfoAct[i].fAngle = m_listInfoAct[m_uLastActOffset].fAngle + D3DX_PI/2.0f;
	if(m_listInfoAct[i].fAngle > D3DX_PI*2.0f )
		m_listInfoAct[i].fAngle -= D3DX_PI*2.0f;

	// ���� ����
	m_listInfoAct[i].iDir = m_listInfoAct[m_uLastActOffset].iDir;
	m_listInfoAct[i].iDir++;
	if(m_listInfoAct[i].iDir > 3)
		m_listInfoAct[i].iDir = 0;

	// ��ġ
	m_listInfoAct[i].vPos = m_listInfoAct[m_uLastActOffset].vPos;


	// ������ �׼� ��ġ ����
	m_uLastActOffset += TURN_RIGHT_FRAME_SIZE;
}

// �ȱ� 40 ������
VOID LmChar::Walk(UINT _uStartFrame)
{	
	LmKeyAct ActKey(_uStartFrame, ACT_WALK);
	m_listKeyAct.push_back(ActKey);

	INT iCount = 1;
	switch(m_listInfoAct[m_uLastActOffset].iDir)
	{
	case 0: // ��
		for(INT i = (INT)_uStartFrame; i < (INT)(_uStartFrame + WALK_FRAME_SIZE); i++)
		{
			// �׼� Ÿ��
			m_listInfoAct[i].uActType = ACT_WALK;

			// ��ġ
			m_listInfoAct[i].vPos.x	= m_listInfoAct[m_uLastActOffset].vPos.x;
			m_listInfoAct[i].vPos.y = m_listInfoAct[m_uLastActOffset].vPos.y;

			// ������ ������ ó�� - ����
			if(i == (INT)(_uStartFrame + WALK_FRAME_SIZE -1))
				m_listInfoAct[i].vPos.z = m_listInfoAct[m_uLastActOffset].vPos.z - (FLOAT)ThePickBoard->GetTileSize();
			else
			{
				m_listInfoAct[i].vPos.z = m_listInfoAct[m_uLastActOffset].vPos.z 
				- iCount * (FLOAT)(ThePickBoard->GetTileSize())/(FLOAT)WALK_FRAME_SIZE;
			}

			// ����
			m_listInfoAct[i].fAngle = m_listInfoAct[m_uLastActOffset].fAngle;
			
			// ����
			m_listInfoAct[i].iDir = m_listInfoAct[m_uLastActOffset].iDir;
			
			// �ִϸ��̼� Ÿ��
			m_listInfoAct[i].fActTime = 12.0f +iCount * 3.0f/(FLOAT)WALK_FRAME_SIZE;

			iCount++;
		}
		break;
	case 1: // ����
		for(INT i = (INT)_uStartFrame; i < (INT)(_uStartFrame + WALK_FRAME_SIZE); i++)
		{
			// �׼� Ÿ��
			m_listInfoAct[i].uActType = ACT_WALK;

			// ��ġ
			if(i == (INT)(_uStartFrame + WALK_FRAME_SIZE -1)) // ������ ������ ó��
				m_listInfoAct[i].vPos.x = m_listInfoAct[m_uLastActOffset].vPos.x - (FLOAT)ThePickBoard->GetTileSize();
			else 
			{
				m_listInfoAct[i].vPos.x = m_listInfoAct[m_uLastActOffset].vPos.x 
					- iCount * (FLOAT)(ThePickBoard->GetTileSize())/(FLOAT)WALK_FRAME_SIZE;
			}
			m_listInfoAct[i].vPos.y = m_listInfoAct[m_uLastActOffset].vPos.y;
			m_listInfoAct[i].vPos.z	= m_listInfoAct[m_uLastActOffset].vPos.z;

			// ����
			m_listInfoAct[i].fAngle = m_listInfoAct[m_uLastActOffset].fAngle;

			// ����
			m_listInfoAct[i].iDir = m_listInfoAct[m_uLastActOffset].iDir;

			// �ִϸ��̼� Ÿ��
			m_listInfoAct[i].fActTime = 12.0f +iCount * 3.0f/(FLOAT)WALK_FRAME_SIZE;

			iCount++;
		}
		break;

	case 2: // ��
		for(INT i = (INT)_uStartFrame; i < (INT)(_uStartFrame + WALK_FRAME_SIZE); i++)
		{
			// �׼� Ÿ��
			m_listInfoAct[i].uActType = ACT_WALK;

			// ��ġ
			m_listInfoAct[i].vPos.x	= m_listInfoAct[m_uLastActOffset].vPos.x;
			m_listInfoAct[i].vPos.y = m_listInfoAct[m_uLastActOffset].vPos.y;

			// ������ ������ ó�� - ����
			if(i == (INT)(_uStartFrame + WALK_FRAME_SIZE -1)) // ������ ������ ó��
				m_listInfoAct[i].vPos.z = m_listInfoAct[m_uLastActOffset].vPos.z + (FLOAT)ThePickBoard->GetTileSize();
			else 
			{
				m_listInfoAct[i].vPos.z = m_listInfoAct[m_uLastActOffset].vPos.z 
					+ iCount * (FLOAT)(ThePickBoard->GetTileSize())/(FLOAT)WALK_FRAME_SIZE;
			}

			// ����
			m_listInfoAct[i].fAngle = m_listInfoAct[m_uLastActOffset].fAngle;

			// ����
			m_listInfoAct[i].iDir = m_listInfoAct[m_uLastActOffset].iDir;

			// �ִϸ��̼� Ÿ��
			m_listInfoAct[i].fActTime = 12.0f +iCount * 3.0f/(FLOAT)WALK_FRAME_SIZE;

			iCount++;
		}
		break;
	case 3: // ������
		for(INT i = (INT)_uStartFrame; i < (INT)(_uStartFrame + WALK_FRAME_SIZE); i++)
		{
			// �׼� Ÿ��
			m_listInfoAct[i].uActType = ACT_WALK;

			// ��ġ
			if(i == (INT)(_uStartFrame + WALK_FRAME_SIZE -1)) // ������ ������ ó��
				m_listInfoAct[i].vPos.x = m_listInfoAct[m_uLastActOffset].vPos.x + (FLOAT)ThePickBoard->GetTileSize();
			else 
			{
				m_listInfoAct[i].vPos.x = m_listInfoAct[m_uLastActOffset].vPos.x 
					+ iCount * (FLOAT)(ThePickBoard->GetTileSize())/(FLOAT)WALK_FRAME_SIZE;
			}
			m_listInfoAct[i].vPos.y = m_listInfoAct[m_uLastActOffset].vPos.y;
			m_listInfoAct[i].vPos.z	= m_listInfoAct[m_uLastActOffset].vPos.z;

			// ����
			m_listInfoAct[i].fAngle = m_listInfoAct[m_uLastActOffset].fAngle;

			// ����
			m_listInfoAct[i].iDir = m_listInfoAct[m_uLastActOffset].iDir;

			// �ִϸ��̼� Ÿ��
			m_listInfoAct[i].fActTime = 12.0f +iCount * 3.0f/(FLOAT)WALK_FRAME_SIZE;

			iCount++;
		}
		break;
	}

	m_uLastActOffset += WALK_FRAME_SIZE;
}

// ������ 40 ������
VOID LmChar::ShakeHand(UINT _uStartFrame)
{
	LmKeyAct ActKey(_uStartFrame, ACT_SHAKE_HAND);
	m_listKeyAct.push_back(ActKey);

	INT iCount = 1;
	for(INT i = (INT)_uStartFrame; i < (INT)(_uStartFrame + SHAKE_HAND_FRAME_SIZE); i++)
	{
		// �׼� Ÿ��
		m_listInfoAct[i].uActType = ACT_SHAKE_HAND;

		// �ִϸ��̼� Ÿ��
		m_listInfoAct[i].fActTime = iCount * 3.0f/(FLOAT)SHAKE_HAND_FRAME_SIZE;

		// ���� �����͸� ���� //

		// ����
		m_listInfoAct[i].fAngle = m_listInfoAct[m_uLastActOffset].fAngle;

		// ����
		m_listInfoAct[i].iDir = m_listInfoAct[m_uLastActOffset].iDir;

		// ��ġ
		m_listInfoAct[i].vPos = m_listInfoAct[m_uLastActOffset].vPos;

		iCount++;
	}

	m_uLastActOffset += SHAKE_HAND_FRAME_SIZE;
}

// �λ��ϱ� 40 ������
VOID LmChar::Bow(UINT _uStartFrame)
{
	LmKeyAct ActKey(_uStartFrame, ACT_BOW);
	m_listKeyAct.push_back(ActKey);

	INT iCount = 1;
	for(INT i = (INT)_uStartFrame; i < (INT)(_uStartFrame + BOW_FRAME_SIZE); i++)
	{
		// �׼� Ÿ��
		m_listInfoAct[i].uActType = ACT_BOW;

		// �ִϸ��̼� Ÿ��
		m_listInfoAct[i].fActTime = 3.0f + iCount * 3.0f/(FLOAT)BOW_FRAME_SIZE;

		// ���� �����͸� ���� //

		// ����
		m_listInfoAct[i].fAngle = m_listInfoAct[m_uLastActOffset].fAngle;

		// ����
		m_listInfoAct[i].iDir = m_listInfoAct[m_uLastActOffset].iDir;

		// ��ġ
		m_listInfoAct[i].vPos = m_listInfoAct[m_uLastActOffset].vPos;

		iCount++;
	}

	m_uLastActOffset += BOW_FRAME_SIZE;
}

// ���߱� 40 ������
VOID LmChar::Turnning(UINT _uStartFrame)
{
	LmKeyAct ActKey(_uStartFrame, ACT_TURNNING);
	m_listKeyAct.push_back(ActKey);

	m_listInfoAct[(INT)_uStartFrame] = m_listInfoAct[m_uLastActOffset];

	INT iCount = 1;
	for(INT i = (INT)_uStartFrame; i < (INT)(_uStartFrame + DANCE_FRAME_SIZE); i++)
	{
		// �׼� Ÿ��
		m_listInfoAct[i].uActType = ACT_TURNNING;

		// �ִϸ��̼� Ÿ��
		m_listInfoAct[i].fActTime = 6.0f + iCount * 3.0f/(FLOAT)DANCE_FRAME_SIZE;

		// ���� �����͸� ���� //

		// ����
		m_listInfoAct[i].fAngle = m_listInfoAct[m_uLastActOffset].fAngle;

		// ����
		m_listInfoAct[i].iDir = m_listInfoAct[m_uLastActOffset].iDir;

		// ��ġ
		m_listInfoAct[i].vPos = m_listInfoAct[m_uLastActOffset].vPos;

		iCount++;
	}

	m_uLastActOffset += DANCE_FRAME_SIZE;
}

// ���ڸ��ٱ� 40 ������
VOID LmChar::Jump(UINT _uStartFrame)
{
	LmKeyAct ActKey(_uStartFrame, ACT_JUMP);
	m_listKeyAct.push_back(ActKey);

	m_listInfoAct[(INT)_uStartFrame] = m_listInfoAct[m_uLastActOffset];

	INT iCount = 1;
	for(INT i = (INT)_uStartFrame; i < (INT)(_uStartFrame + JUMP_FRAME_SIZE); i++)
	{
		// �׼� Ÿ��
		m_listInfoAct[i].uActType = ACT_JUMP;

		// �ִϸ��̼� Ÿ��
		m_listInfoAct[i].fActTime = 9.0f + iCount * 3.0f/(FLOAT)JUMP_FRAME_SIZE;

		// ���� �����͸� ���� //

		// ����
		m_listInfoAct[i].fAngle = m_listInfoAct[m_uLastActOffset].fAngle;

		// ����
		m_listInfoAct[i].iDir = m_listInfoAct[m_uLastActOffset].iDir;

		// ��ġ
		m_listInfoAct[i].vPos = m_listInfoAct[m_uLastActOffset].vPos;

		iCount++;
	}

	m_uLastActOffset += JUMP_FRAME_SIZE;
}

BOOL LmChar::DeleteAction()
{

	if(m_uLastActOffset == 0)
		return FALSE;

	switch(m_listInfoAct[m_uLastActOffset].uActType)
	{
	case ACT_TURN_LEFT:
		for(INT i = m_uLastActOffset; i > (INT)(m_uLastActOffset - TURN_LEFT_FRAME_SIZE); i--)
			m_listInfoAct[i].Reset();
		m_uLastActOffset -= TURN_LEFT_FRAME_SIZE;
		break;
	case ACT_TURN_RIGHT:
		for(INT i = m_uLastActOffset; i > (INT)(m_uLastActOffset - TURN_RIGHT_FRAME_SIZE); i--)
			m_listInfoAct[i].Reset();
		m_uLastActOffset -= TURN_RIGHT_FRAME_SIZE;
		break;
	case ACT_WALK:
		for(INT i = m_uLastActOffset; i > (INT)(m_uLastActOffset - WALK_FRAME_SIZE); i--)
			m_listInfoAct[i].Reset();
		m_uLastActOffset -= WALK_FRAME_SIZE;
		break;
	case ACT_SHAKE_HAND:
		for(INT i = m_uLastActOffset; i > (INT)(m_uLastActOffset - SHAKE_HAND_FRAME_SIZE); i--)
			m_listInfoAct[i].Reset();
		m_uLastActOffset -= SHAKE_HAND_FRAME_SIZE;
		break;
	case ACT_BOW:
		for(INT i = m_uLastActOffset; i > (INT)(m_uLastActOffset - BOW_FRAME_SIZE); i--)
			m_listInfoAct[i].Reset();
		m_uLastActOffset -= BOW_FRAME_SIZE;
		break;
	case ACT_TURNNING:
		for(INT i = m_uLastActOffset; i > (INT)(m_uLastActOffset - TURNNING_FRAME_SIZE); i--)
			m_listInfoAct[i].Reset();
		m_uLastActOffset -= TURNNING_FRAME_SIZE;
		break;
	case ACT_JUMP:
		for(INT i = m_uLastActOffset; i > (INT)(m_uLastActOffset - JUMP_FRAME_SIZE); i--)
			m_listInfoAct[i].Reset();
		m_uLastActOffset -= JUMP_FRAME_SIZE;
		break;
	}

	// �׼� ���̾� Ű ���� ã�Ƽ� �����.
	vector<LmKeyAct>::iterator itr = m_listKeyAct.begin();
	for(size_t i = 0; i < m_listKeyAct.size(); i++)
	{
		if(m_listKeyAct[i].uStartFrame == (m_uLastActOffset+1))
			m_listKeyAct.erase(itr);
		else
			itr++;
	}

	return TRUE;
}


BOOL LmChar::CanWalk()
{
	FLOAT fLastPosX = m_listInfoAct[m_uLastActOffset].vPos.x;
	FLOAT fLastPosZ = m_listInfoAct[m_uLastActOffset].vPos.z;
	INT iLastDir = m_listInfoAct[m_uLastActOffset].iDir;

	INT iSide = (ThePickBoard->GetDepth() - ThePickBoard->GetTileSize()) /2;
	if( (( fLastPosX == iSide) && (iLastDir == 3))	||
		(( fLastPosX == -iSide) && (iLastDir == 1)) ||
		(( fLastPosZ == iSide) && (iLastDir == 2))	||
		(( fLastPosZ == -iSide) && (iLastDir == 0))	)
		return FALSE;

	return TRUE;
}

VOID LmChar::InitTimeLine()
{
	// �׼� ���̾�
	vector<LmKeyAct>::iterator itr;
	for(itr = m_listKeyAct.begin(); itr != m_listKeyAct.end();itr++)
	{
		switch((*itr).uActType)
		{
		case ACT_TURN_LEFT:
			TheTimeLine->InsertActionSection(GetName(), (*itr).uStartFrame, TURN_LEFT_FRAME_SIZE);
			break;
		case ACT_TURN_RIGHT:
			TheTimeLine->InsertActionSection(GetName(), (*itr).uStartFrame, TURN_RIGHT_FRAME_SIZE);
			break;
		case ACT_WALK:
			TheTimeLine->InsertActionSection(GetName(), (*itr).uStartFrame, WALK_FRAME_SIZE);
			break;
		case ACT_SHAKE_HAND:
			TheTimeLine->InsertActionSection(GetName(), (*itr).uStartFrame, SHAKE_HAND_FRAME_SIZE);
			break;
		case ACT_BOW:
			TheTimeLine->InsertActionSection(GetName(), (*itr).uStartFrame, BOW_FRAME_SIZE);
			break;
		case ACT_TURNNING:
			TheTimeLine->InsertActionSection(GetName(), (*itr).uStartFrame, TURNNING_FRAME_SIZE);
			break;
		case ACT_JUMP:
			TheTimeLine->InsertActionSection(GetName(), (*itr).uStartFrame, JUMP_FRAME_SIZE);
			break;

		}
	}


	// ��ǳ�� ���̾�
	vector<LmKeyTalk>::iterator itrTalk;
	for(itrTalk = m_listKeyTalkBox.begin(); itrTalk != m_listKeyTalkBox.end();itrTalk++)
	{
		TheTimeLine->InsertTalkBoxSection(GetName(), itrTalk->uStartFrame, itrTalk->uEndFrame -itrTalk->uStartFrame + 1);
	}
}

BOOL LmChar::InsertTalkBox( LPCWSTR _pcwsMessage, UINT _uStartFrame, UINT _uFrameSize, D3DXCOLOR _dxColor )
{
	LmKeyTalk TalkKey;

	TalkKey.wstrMsg = _pcwsMessage;
	TalkKey.dxColor = _dxColor;
	TalkKey.uStartFrame = _uStartFrame;
	TalkKey.uEndFrame = _uStartFrame + _uFrameSize  - 1;
	TalkKey.uShape = 0;

	m_listKeyTalkBox.push_back(TalkKey);

	return TRUE;
}

D3DXCOLOR LmChar::GetTalkBoxColor( UINT _uStartFrame )
{
	for(size_t i = 0; i < m_listKeyTalkBox.size(); i++)
	{
		if( m_listKeyTalkBox[i].uStartFrame == _uStartFrame)
			return m_listKeyTalkBox[i].dxColor;
	}

	return D3DXCOLOR(0, 0, 0, 0);
}
BOOL LmChar::ModifyTalkBox( UINT _uOldStartFrame, LPCWSTR _pcwsMessage, UINT _uStartFrame, UINT _uFrameSize, 
					D3DXCOLOR _dxColor)
{
	if(!RemoveTalkBox(_uOldStartFrame))
		return FALSE;
	if(!InsertTalkBox(_pcwsMessage, _uStartFrame, _uFrameSize, _dxColor))
		return FALSE;

	return TRUE;
}

BOOL LmChar::RemoveTalkBox( UINT _uOldStartFrame )
{
	vector<LmKeyTalk>::iterator itr = m_listKeyTalkBox.begin();

	for(size_t i = 0; i < m_listKeyTalkBox.size(); i++)
	{
		if(m_listKeyTalkBox[i].uStartFrame == _uOldStartFrame)
			break;
		itr++;
	}

	if(itr == m_listKeyTalkBox.end())
		return FALSE;

	m_listKeyTalkBox.erase(itr);
	return TRUE;
}
