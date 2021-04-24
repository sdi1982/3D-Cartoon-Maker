//
//
//  @ Project : 3D ī�� ����Ŀ
//  @ File Name : LmScene.cpp
//  @ Date : 2007-10-11
//  @ Author : �ŵ���
//
//

#include "dxstdafx.h"
#include "LmScene.h"
#include "LmModelManager.h"
#include "LmEffectManager.h"
#include "LmTerrain.h"
#include "LmTimeLineManager.h"

LmScene::LmScene()
{

}

LmScene::LmScene(LPCWSTR _pcwsName, UINT _uFrameSize = 600, UINT _uFPS = 10, UINT _uTerTex = TERRAIN_GRASS) 
{
	m_wstrName = _pcwsName;

	m_pBG = NULL;
	m_mapThing.clear();
	m_mapChar.clear();
	m_listInfoSound.clear();

	m_uFrameSize = _uFrameSize;
	m_uFPS = _uFPS;
	m_uTerrainType = _uTerTex;
	m_listInfoCamera.resize(_uFrameSize+1);
	m_listInfoFirework.resize(_uFrameSize+1);
	m_uOldFrame = 0;
}

LmScene::~LmScene() 
{
	ClearCharAndThing();
}

// ���� ���� ���纻�� �����Ѵ�. 
LmScene* LmScene::Clone(LPCWSTR _pcwsClone)
{
	LmScene* pCloneScene = new LmScene(_pcwsClone, m_uFrameSize, m_uFPS);

	if(pCloneScene == NULL)
		return NULL;

	 *pCloneScene = *this;

	return pCloneScene;
}
LmScene& LmScene::operator=( LmScene& _rhs )
{
	// �ڱ� �ڽ��̶� ���ٸ� �׳� ����
	
	if(this != &_rhs)
	{

		m_uFrameSize = _rhs.m_uFrameSize; 
		m_uFPS = _rhs.m_uFPS;		
		m_uTerrainType = _rhs.m_uTerrainType;
		m_uOldFrame = _rhs.m_uOldFrame;

		// ���� Ÿ�� ����
		m_uTerrainType = _rhs.m_uTerrainType;

		// ��� ����
		m_pBG = _rhs.m_pBG;

		ClearCharAndThing();

		// ĳ���� ����
		map<wstring,LmChar*>::iterator itrChar;
		for(itrChar = _rhs.m_mapChar.begin(); itrChar != _rhs.m_mapChar.end(); itrChar++)
		{
			CopyChar((*itrChar).second->GetXFileName(),
				(*itrChar).second->GetPosX(),
				(*itrChar).second->GetPosZ());
		}

		// ���� ����
		map<wstring,LmThing*>::iterator itrThing;
		for(itrThing = _rhs.m_mapThing.begin(); itrThing != _rhs.m_mapThing.end(); itrThing++)
		{
			PlaceThing((*itrThing).second->GetXFileName(),
				(*itrThing).second->GetPosX(),
				(*itrThing).second->GetPosZ());
		}

		// ī�޶� ���� ����
		m_listInfoCamera.clear();
		vector<LmInfoCamera>::iterator itrInfoCam;
		for(itrInfoCam = _rhs.m_listInfoCamera.begin(); itrInfoCam != _rhs.m_listInfoCamera.end();
			itrInfoCam++)
		{
			m_listInfoCamera.push_back(*itrInfoCam);
		}

		// ī�޶� Ű ����
		m_listKeySnow.clear();
		vector<UINT>::iterator itrKeyCam;
		for(itrKeyCam = _rhs.m_listKeyCamera.begin(); itrKeyCam != _rhs.m_listKeyCamera.end();
			itrKeyCam++)
		{
			m_listKeyCamera.push_back(*itrKeyCam);
		}

		// �������� Ű ����
		m_listKeySnow.clear();
		vector<LmKeySnow>::iterator itrKeySnow;
		for(itrKeySnow = _rhs.m_listKeySnow.begin(); itrKeySnow != _rhs.m_listKeySnow.end();
			itrKeySnow++)
		{
			m_listKeySnow.push_back(*itrKeySnow);
		}

		// �Ҳɳ��� ���� ����
		m_listInfoFirework.clear();
		vector<LmInfoFirework>::iterator itrInfoFirework;
		for(itrInfoFirework = _rhs.m_listInfoFirework.begin(); 
			itrInfoFirework != _rhs.m_listInfoFirework.end();
			itrInfoFirework++)
		{
			m_listInfoFirework.push_back(*itrInfoFirework);
		}

		// �Ҳɳ��� Ű ����
		m_listKeyFirework.clear();
		vector<UINT>::iterator itrKeyFirework;
		for(itrKeyFirework = _rhs.m_listKeyFirework.begin();
			itrKeyFirework != _rhs.m_listKeyFirework.end();
			itrKeyFirework++)
		{
			m_listKeyFirework.push_back(*itrKeyFirework);
		}

		// ���� ����
		m_listInfoSound.clear();
		vector<LmSection>::iterator itrSound;
		for(itrSound = _rhs.m_listInfoSound.begin();
			itrSound != _rhs.m_listInfoSound.end() ;
			itrSound++)
		{
			m_listInfoSound.push_back(*itrSound);
		}

	}

	return *this;
}

BOOL LmScene::PlaceBG( LPCWSTR _pcwsBG )
{
	// �� �Ŵ������� ����� �������� ������ ���� ���� ������� �����Ѵ�.
	m_pBG = TheModelManager->GetBG(_pcwsBG);

	if(m_pBG == NULL)
		return FALSE;

	return TRUE;
}

BOOL LmScene::PlaceThing( LPCWSTR _pcwsThing, FLOAT _fStepX, FLOAT _fStepZ )
{	
	// ����� ������ �̸����� �����̸� �ڿ� #���ڸ� ���δ�. ��� ������ ���ڸ� ���δ�.
	// (��������� ��ü�� ��û���� ��������� �ʴ� �̻� ������ ����..)
	WCHAR wNum[8];
	static int ThingCount = 1;
	_itow_s(ThingCount,wNum, 8 ,10);

	wstring wstrClone = _pcwsThing;
	wstrClone =  wstrClone + L"#" + wNum;
	LPCWSTR pcwsClone = wstrClone.c_str();

	// �ٸ� ��ü�� ���� �̸��� �ִ��� Ȯ���Ѵ�.
	if(CheckThingName(pcwsClone))
		return FALSE;

	// ��ü�� ĳ���Ͱ� �� ��ġ�� �ִ��� Ȯ���Ѵ�.
	if(CheckPos(_fStepX, _fStepZ))
		return FALSE;

	m_mapThing[pcwsClone] = TheModelManager->GetThing(_pcwsThing)->Clone(pcwsClone);
	m_mapThing[pcwsClone]->Place(_fStepX, _fStepZ);

	ThingCount++;

	return TRUE;
}

BOOL LmScene::PlaceChar( LPCWSTR _pcwsChar, FLOAT _fStepX, FLOAT _fStepZ )
{
	// ����� ĳ������ �̸����� �����̸� �ڿ� #���ڸ� ���δ�. ��� ������ ���ڸ� ���δ�.
	// (��������� ĳ���Ͱ� ��û���� ��������� �ʴ� �̻� ������ ����..)
	WCHAR wNum[8];
	static int CharCount = 1;
	_itow_s(CharCount,wNum, 8 ,10);

	wstring wstrClone = _pcwsChar;
	wstrClone = wstrClone + L"#" + wNum;
	LPCWSTR pcwsClone = wstrClone.c_str();

	// �ٸ� ĳ���Ͱ� ���� �̸��� �ִ��� Ȯ���Ѵ�.
	if(CheckCharName(pcwsClone))
		return FALSE;

	// ��ü�� ĳ���Ͱ� �� ��ġ�� �ִ��� Ȯ���Ѵ�.
	if(CheckPos(_fStepX, _fStepZ))
		return FALSE;

	// Ÿ�� ���ο� ĳ���� ���̾ �߰��Ѵ�.
	TheTimeLine->InsertCharacterLayer(pcwsClone);

	m_mapChar[pcwsClone] = TheModelManager->GetChar(_pcwsChar)->Clone(pcwsClone);
	m_mapChar[pcwsClone]->SetFrameSize(m_uFrameSize);
	m_mapChar[pcwsClone]->Place(_fStepX, _fStepZ);

	CharCount++;
	return TRUE;
}

BOOL LmScene::CopyChar( LPCWSTR _pcwsChar, FLOAT _fStepX, FLOAT _fStepZ )
{
	// ����� ĳ������ �̸����� �����̸� �ڿ� #���ڸ� ���δ�. ��� ������ ���ڸ� ���δ�.
	// (��������� ĳ���Ͱ� ��û���� ��������� �ʴ� �̻� ������ ����..)
	WCHAR wNum[8];
	static int CharCount = 1;
	_itow_s(CharCount,wNum, 8 ,10);

	wstring wstrClone = _pcwsChar;
	wstrClone = wstrClone + L"#" + wNum;
	LPCWSTR pcwsClone = wstrClone.c_str();


	m_mapChar[pcwsClone] = TheModelManager->GetChar(_pcwsChar)->Clone(pcwsClone);
	m_mapChar[pcwsClone]->SetFrameSize(m_uFrameSize);
	m_mapChar[pcwsClone]->Place(_fStepX, _fStepZ);

	CharCount++;
	return TRUE;
}

BOOL LmScene::DeleteThing( LPCWSTR _pcwsName )
{
	// ���纻�� �ڱ⸸�� ������ �����
	LmThing* pThing = m_mapThing[_pcwsName];

	m_mapThing.erase(_pcwsName);
	delete pThing;

	return TRUE;
}

BOOL LmScene::DeleteChar( LPCWSTR _pcwsName )
{
	// ���纻�� �ڱ⸸�� ������ �����
	LmChar* pChar = m_mapChar[_pcwsName];

	m_mapChar.erase(_pcwsName);
	delete pChar;

	return TRUE;
}

LmChar* LmScene::GetChar(LPCWSTR _pcwsName) 
{
	if(!CheckCharName(_pcwsName))
		return NULL;

	return m_mapChar[_pcwsName];
}

LmThing* LmScene::GetThing(LPCWSTR _pcwsName) 
{
	if(!CheckThingName(_pcwsName))
		return NULL;

	return m_mapThing[_pcwsName];
}

VOID LmScene::SetCurrFrame( UINT _uFrame )
{
	// ���� �������� ������ Ŭ���ϴ� ���� ���´�.
	if(m_uOldFrame == _uFrame)
		return;

	m_uOldFrame = _uFrame;

	// ���� �������� ī�޶� ��ǥ ����
	LmInfoCamera Camera = GetCameraInfo(_uFrame);
	TheCamera->SetEyePt(Camera.vEye.x, Camera.vEye.y, Camera.vEye.z);

	// ���� �������� ĳ���� ����
	map<wstring,LmChar*>::iterator itrChar;
	for(itrChar = m_mapChar.begin(); itrChar != m_mapChar.end(); itrChar++)
	{
		if((*itrChar).second != NULL)
			(*itrChar).second->SetCurrFrame(_uFrame);
	}


	//���� �����ӿ� ����Ʈ�� ���� ���� Ȯ��
	if(IsInSnowKey(_uFrame))
	{
		TheEffectManager->ShowSnow(TRUE);
		TheEffectManager->SetSnowTime(1.0f);
	}
	else
		TheEffectManager->ShowSnow(FALSE);

	if(m_listInfoFirework[_uFrame].bShow)
	{
		TheEffectManager->ShowFirework(TRUE);
		TheEffectManager->SetFireworkTime(m_listInfoFirework[_uFrame].fElapsedTime);
	}
	else
		TheEffectManager->ShowFirework(FALSE);

}

VOID LmScene::Update( FLOAT _fTimeDelta )
{
	if(m_pBG)
		m_pBG->Update(_fTimeDelta);

	map<wstring,LmThing*>::iterator itrThing;
	for(itrThing = m_mapThing.begin(); itrThing != m_mapThing.end(); itrThing++)
	{
		if((*itrThing).second != NULL)
			(*itrThing).second->Update(_fTimeDelta);
	}

	map<wstring,LmChar*>::iterator itrChar;
	for(itrChar = m_mapChar.begin(); itrChar != m_mapChar.end(); itrChar++)
	{
		if((*itrChar).second != NULL)
			(*itrChar).second->Update(_fTimeDelta);
	}
}

VOID LmScene::Render( LPDIRECT3DDEVICE9 _pDevice )
{
	// ����� �׸���.
	if(!m_pDevice)
		m_pDevice = _pDevice;
	else
	{
		if(m_pBG)
			m_pBG->Render(_pDevice);

		// ������ �׸���.
		map<wstring,LmThing*>::iterator itrThing;
		for(itrThing = m_mapThing.begin(); itrThing != m_mapThing.end(); itrThing++)
		{
			if((*itrThing).second != NULL)
				(*itrThing).second->Render(_pDevice);
		}

		map<wstring,LmChar*>::iterator itrChar;
		for(itrChar = m_mapChar.begin(); itrChar != m_mapChar.end(); itrChar++)
		{
			if((*itrChar).second != NULL)
				(*itrChar).second->Render(_pDevice);
		}
	}
}

// ���� ��ü �̸��� �ִٸ� TRUE�� �����Ѵ�.
BOOL LmScene::CheckThingName( LPCWSTR _pcwsThing )
{
	map<wstring,LmThing*>::iterator itr;
	for(itr  = m_mapThing.begin(); itr != m_mapThing.end(); itr++)
	{
		if(!wcscmp((*itr).second->GetName(), _pcwsThing))
			return TRUE;
	}

	return FALSE;
}

// ���� ĳ���� �̸��� �ִٸ� TRUE�� �����Ѵ�.
BOOL LmScene::CheckCharName( LPCWSTR _pcwsChar )
{
	map<wstring,LmChar*>::iterator itr;
	for(itr  = m_mapChar.begin(); itr != m_mapChar.end(); itr++)
	{
		if(!wcscmp((*itr).second->GetName(), _pcwsChar))
			return TRUE;
	}

	return FALSE;
}

BOOL LmScene::CheckPos(FLOAT _fStepX, FLOAT _fStepZ)
{
	map<wstring,LmThing*>::iterator itrThing;
	for(itrThing  = m_mapThing.begin(); itrThing != m_mapThing.end(); itrThing++)
	{
		if(((*itrThing).second->GetPosX() == _fStepX) &&
			((*itrThing).second->GetPosZ() == _fStepZ))
			return TRUE;
	}

	map<wstring,LmChar*>::iterator itrChar;
	for(itrChar  = m_mapChar.begin(); itrChar != m_mapChar.end(); itrChar++)
	{
		if(((*itrChar).second->GetPosX() == _fStepX) &&
			((*itrChar).second->GetPosZ() == _fStepZ))
			return TRUE;
	}

	return FALSE;
}

LPCWSTR LmScene::GetSamePosThing(FLOAT _fStepX, FLOAT _fStepZ)
{
	map<wstring,LmThing*>::iterator result;
	for(result  = m_mapThing.begin(); result != m_mapThing.end(); result++)
	{
		if(((*result).second->GetPosX() == _fStepX) &&
			((*result).second->GetPosZ() == _fStepZ))
			return (*result).second->GetName();
	}

	return L"";
}

LPCWSTR LmScene::GetSamePosChar(FLOAT _fStepX, FLOAT _fStepZ)
{
	map<wstring,LmChar*>::iterator result;
	for(result  = m_mapChar.begin(); result != m_mapChar.end(); result++)
	{
		if(((*result).second->GetPosX() == _fStepX) &&
			((*result).second->GetPosZ() == _fStepZ))
			return (*result).second->GetName();
	}

	return L"";
}

LmInfoCamera LmScene::GetCameraInfo( UINT _uFrame )
{
	return m_listInfoCamera[_uFrame];
}

BOOL LmScene::InsertCameraKey( UINT _uFrameNo, LmInfoCamera _CameraEye )
{
	m_listInfoCamera[_uFrameNo] = _CameraEye;

	// Ű���� �̹� �ִٸ� ���� �������� �ʴ´�.
	vector<UINT>::iterator itr;
	itr = find(m_listKeyCamera.begin(), m_listKeyCamera.end(), _uFrameNo);

	if(itr == m_listKeyCamera.end())
	{
		// Ű�� �����ϰ� �����Ѵ�.
		m_listKeyCamera.push_back(_uFrameNo);
		sort(m_listKeyCamera.begin(), m_listKeyCamera.end());
	}

	// Ű�� ������ �Ǹ� ���� Ű���� �ִ��� Ȯ���ϰ� �����Ѵ�.
	INT preFrameNo = -1, postFrameNo = -1;

	// ���� Ű���� ã�´�.
	preFrameNo = FindPreKey(_uFrameNo);

	// ���� Ű �������� ���� Ű������ �����Ѵ�.
	if(preFrameNo != -1)
		LerpFrame(preFrameNo, _uFrameNo);


	// ���� Ű ���� ã�´�.
	postFrameNo = FindPostKey(_uFrameNo);

	// ���� Ű �������� ���� Ű������ �����Ѵ�.
	if(postFrameNo != -1)
		LerpFrame(_uFrameNo, postFrameNo);

	return TRUE;
}

BOOL LmScene::DeleteBG()
{
	if(m_pBG)
	{
		m_pBG = NULL;
		return TRUE;
	}

	return FALSE;
}


VOID LmScene::Setup(UINT _uScreenWidth, UINT _uScreenHeight)
{
	map<wstring,LmChar*>::iterator result;
	for(result  = m_mapChar.begin(); result != m_mapChar.end(); result++)
	{
		result->second->Setup(_uScreenWidth, _uScreenHeight);
	}
}

VOID LmScene::Cleanup()
{
	map<wstring,LmChar*>::iterator result;
	for(result  = m_mapChar.begin(); result != m_mapChar.end(); result++)
	{
		result->second->Cleanup();
	}
}

BOOL LmScene::RemoveCameraKey( UINT _uFrameNo )
{
	// ������ ��ġ�� ī�޶� �������� Ű �������� �ƴϸ� �����Ѵ�.
	//if(m_listInfoCamera[_uFrameNo].bKey == FALSE)
	//	return FALSE;

	// Ű ����Ʈ�� Ű ���� ����� �����Ѵ�.
	vector<UINT>::iterator itr;
	itr = find(m_listKeyCamera.begin(), m_listKeyCamera.end(), _uFrameNo);

	// Ű ����Ʈ�� Ű�� ���ٸ� ����
	if(itr == m_listKeyCamera.end())
		return FALSE;

	// Ű�� �����.
	m_listKeyCamera.erase(itr);

	INT iPreCameraKeyNo, iPostCameraKeyNo;
	iPreCameraKeyNo = FindPreKey(_uFrameNo);
	iPostCameraKeyNo = FindPostKey(_uFrameNo);

	// ���� ��ġ�� �����ӿ��� �հ� ���� Ű �������� ã�Ƽ� �׻��̸� �����Ѵ�.
	// �Ѵ� ������ �׳� �ڱ⸸ ����
	if((iPreCameraKeyNo == -1)&&(iPostCameraKeyNo == -1))
		m_listInfoCamera[_uFrameNo].Reset();

	else if((iPreCameraKeyNo != -1)&&(iPostCameraKeyNo != -1))
	{
		LerpFrame(iPreCameraKeyNo, iPostCameraKeyNo);
		return TRUE;
	}

	// ���� ������ �ޱ����� �����Ѵ�.
	else if(iPreCameraKeyNo == -1)
	{
		ResetFrame(_uFrameNo, iPostCameraKeyNo);
		return TRUE;
	}

	// �ڰ� ������ �ձ����� �����Ѵ�.
	else if(iPostCameraKeyNo == -1)
	{
		ResetFrame(iPreCameraKeyNo, _uFrameNo);
		return TRUE;
	}

	return TRUE;
}

INT LmScene::FindPostKey(UINT _uFrameNo)
{
	vector<UINT>::iterator itr;
	for( itr = m_listKeyCamera.begin(); itr != m_listKeyCamera.end(); itr++)
	{
		if(*itr == _uFrameNo)
		{
			itr++;
			if(itr != m_listKeyCamera.end())
				return *itr;

			return -1;
		}
	}

	return -1;
}

INT LmScene::FindPreKey(UINT _uFrameNo)
{
	vector<UINT>::reverse_iterator ritr;
	for( ritr = m_listKeyCamera.rbegin(); ritr != m_listKeyCamera.rend(); ritr++)
	{
		if(*ritr == _uFrameNo)
		{
			ritr++;
			if(ritr != m_listKeyCamera.rend())
				return *ritr;

			return -1;
		}
	}

	return -1;
}

VOID LmScene::LerpFrame( UINT _uPreFrameNo, UINT _uPostFrameNo )
{
	if(_uPreFrameNo != -1)
	{
		for( UINT i = _uPreFrameNo+1; i < _uPostFrameNo; i++)
		{
			FLOAT child = (FLOAT)(i - _uPreFrameNo);
			FLOAT mother = (FLOAT)(_uPostFrameNo - _uPreFrameNo + 1);
			FLOAT t = child/mother;

			D3DXVec3Lerp(&m_listInfoCamera[i].vEye, 
				&m_listInfoCamera[_uPreFrameNo].vEye, 
				&m_listInfoCamera[_uPostFrameNo].vEye,
				t);
		}
	}
}

VOID LmScene::ResetFrame( UINT _uPreFrameNo, UINT _uPostFrameNo )
{
	if(_uPreFrameNo != -1)
	{
		for( UINT i = _uPreFrameNo+1; i < _uPostFrameNo; i++)
		{
			m_listInfoCamera[_uPreFrameNo].Reset();
		}
	}
}

VOID LmScene::InitTimeLine()
{
	// ���� ������ �ٽ÷ε��Ѵ�.
	TheTerrain->SetTerrainType(m_uTerrainType);

	// ���� Ÿ�Ӷ����� �ʱ�ȭ�Ѵ�.
	TheTimeLine->SetFrameLength(m_uFrameSize);
	TheTimeLine->InitTimeLIne();
	// ���� �����Ϳ� �°� Ÿ�Ӷ����� ������ �����Ѵ�.

	// ���� ����
	vector<LmSection>::iterator itrSound;
	for(itrSound = m_listInfoSound.begin(); itrSound != m_listInfoSound.end() ; itrSound++)
	{
		TheTimeLine->ReloadSoundSection(itrSound->m_wstrSectionName.c_str() , 
												itrSound->m_uStartFrm, itrSound->m_uEndFrm, 
												itrSound->m_dwFadeIn, itrSound->m_dwFadeOut);
	}

	// ī�޶� ���̾�
	vector<UINT>::iterator itrCam;
	for( itrCam = m_listKeyCamera.begin(); itrCam != m_listKeyCamera.end(); itrCam++)
		TheTimeLine->InsertCameraSection(*itrCam);
	
	// ĳ���� �׼� ���̾�
	map<wstring,LmChar*>::iterator itrChar;
	for(itrChar = m_mapChar.begin(); itrChar != m_mapChar.end(); itrChar++)
	{
		//ĳ���͸� �߰��Ѵ�.
		TheTimeLine->InsertCharacterLayer((*itrChar).second->GetName());

		// ĳ������ �׼ǰ� ��ǳ�� ���̾�<�̱���>�� �߰��Ѵ�.
		(*itrChar).second->InitTimeLine();
	}

	// �������� ȿ�� ���̾�
	vector<LmKeySnow>::iterator itrSnow;
	for(itrSnow = m_listKeySnow.begin(); itrSnow != m_listKeySnow.end(); itrSnow++)
		TheTimeLine->InsertSnowSection(L"Snow Section",(*itrSnow).uStartFrame,(*itrSnow).uEndFrame);


	// �Ҳɳ��� ȿ�� ���̾�
	vector<UINT>::iterator itr;
	for( itr = m_listKeyFirework.begin(); itr != m_listKeyFirework.end(); itr++)
		TheTimeLine->InsertFireWorksSection(*itr);
	
	// [����] Ÿ�Ӷ��ο� ��� ������ ����, UI�� �ִ� ListCtrl���� Ÿ�Ӷ��ο� �ٰ��Ͽ� �Է��Ѵ�.
	TheTimeLine->RefreshUIMainToolList();
}

BOOL LmScene::InsertAction( LPCWSTR _pcwsChar, ACT _uActType, UINT _uStartFrame )
{
	LmChar* pChar = GetChar(_pcwsChar);
	switch(_uActType)
	{
		case ACT_TURN_LEFT: pChar->TurnLeft(_uStartFrame); break;
		case ACT_TURN_RIGHT: pChar->TurnRight(_uStartFrame); break;
		case ACT_WALK: pChar->Walk(_uStartFrame); break;
		case ACT_SHAKE_HAND: pChar->ShakeHand(_uStartFrame); break;
		case ACT_BOW: pChar->Bow(_uStartFrame); break;
		case ACT_JUMP: pChar->Jump(_uStartFrame); break;
		case ACT_TURNNING: pChar->Turnning(_uStartFrame); break;
		default: return FALSE;
	}

	return TRUE;
}

BOOL LmScene::RemoveAction( UINT _uFrameNo )
{
	return FALSE;
}

BOOL LmScene::InsertSnowKey(UINT _uStartFrameNo, UINT _uEndFrameNo)
{
	LmKeySnow SnowKey;

	SnowKey.uStartFrame = _uStartFrameNo;
	SnowKey.uEndFrame = _uEndFrameNo;

	m_listKeySnow.push_back(SnowKey);

	return TRUE;
}

BOOL LmScene::RemoveSnowKey(UINT _uStartFrameNo)
{
	vector<LmKeySnow>::iterator itr;

	for(itr = m_listKeySnow.begin(); itr != m_listKeySnow.end(); itr++)
	{
		if((*itr).uStartFrame == _uStartFrameNo)
		{
			m_listKeySnow.erase(itr);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL LmScene::IsInSnowKey( UINT _uFrame )
{
	vector<LmKeySnow>::iterator itr;
	for(itr = m_listKeySnow.begin(); itr != m_listKeySnow.end(); itr++)
	{
		if(((*itr).uStartFrame <= _uFrame) && (_uFrame <= (*itr).uEndFrame))
			return TRUE;
	}

	return FALSE;
}

BOOL LmScene::InsertFireworkKey( UINT _uStartFrameNo, UINT _uEndFrameNo )
{
	m_listKeyFirework.push_back(_uStartFrameNo);

	vector<LmInfoFirework>::iterator itr;

	INT iCount = 0;
	for(INT i = (INT)_uStartFrameNo; i <= (INT)_uEndFrameNo; i++)
	{
		m_listInfoFirework[i].bShow = TRUE;
		m_listInfoFirework[i].fElapsedTime = 0.3f + (FLOAT)iCount * 0.2f;
		iCount++;
	}

	return TRUE;
}

BOOL LmScene::RemoveFireworkKey( UINT _uStartFrameNo )
{
	vector<UINT>::iterator itrFirework;
	itrFirework = find(m_listKeyFirework.begin(),m_listKeyFirework.end(),_uStartFrameNo);

	if(itrFirework == m_listKeyFirework.end())
		return FALSE;

	m_listKeyFirework.erase(itrFirework);

	vector<LmInfoFirework>::iterator itr;

	for(INT i = (INT)_uStartFrameNo; i < (INT)(_uStartFrameNo + FIREWORK_FRAME_SIZE); i++)
		m_listInfoFirework[i].Reset();

	return FALSE;

}

VOID LmScene::InsertSoundSection( LPCWSTR _pcwsSoundName, UINT _uStartFrm,  UINT _uEndFrm,DWORD _dwFadeIn, DWORD _dwFadeOut )
{
	LmSection NewSection(_pcwsSoundName, _uStartFrm, _uEndFrm, _dwFadeIn, _dwFadeOut);
	m_listInfoSound.push_back(NewSection);
}

INT LmScene::RemoveSoundSection( LPCWSTR _pcwsSoundName )
{
	vector<LmSection>::iterator itr;
	for(itr = m_listInfoSound.begin(); itr != m_listInfoSound.end() ; itr++)
	{
		if( !wcscmp(_pcwsSoundName, itr->m_wstrSectionName.c_str()) )
		{
			m_listInfoSound.erase(itr);
			return 0;
		}
	}
	//��ã����!
	return -1;
}

UINT LmScene::GetFrameSize()
{
	return m_uFrameSize;
}

void LmScene::ClearCharAndThing()
{
	map<wstring,LmChar*>::iterator itrChar;
	for(itrChar = m_mapChar.begin(); itrChar != m_mapChar.end(); itrChar++)
	{
		SAFE_DELETE(m_mapChar[(*itrChar).first]);
	}
	m_mapChar.clear();

	map<wstring,LmThing*>::iterator itrThing;
	for(itrThing = m_mapThing.begin(); itrThing != m_mapThing.end(); itrThing++)
	{
		SAFE_DELETE(m_mapThing[(*itrThing).first]);
	}
	m_mapThing.clear();
	return;
}

VOID LmScene::SaveScene( CFile& _File )
{
	// ������ �ϴ� ������� �ε��� �ؾ� �Ѵٴ� ����� ���� ����!

	// ī�޶� ���� ����
	UINT uInfoCameraSize = (UINT)m_listInfoCamera.size();
	_File.Write(&uInfoCameraSize, sizeof(UINT));

	vector<LmInfoCamera>::iterator itrInfoCam;
	for(itrInfoCam = m_listInfoCamera.begin(); itrInfoCam != m_listInfoCamera.end();
		itrInfoCam++)
	{
		LmInfoCamera Node = (*itrInfoCam);
		_File.Write(&Node, sizeof(LmInfoCamera));
	}

	// ī�޶� Ű ����
	UINT uKeyCameraSize = (UINT)m_listKeyCamera.size();
	_File.Write(&uKeyCameraSize, sizeof(UINT));
	vector<UINT>::iterator itrKeyCam;
	for(itrKeyCam = m_listKeyCamera.begin(); itrKeyCam != m_listKeyCamera.end();
		itrKeyCam++)
	{
		UINT Node = *itrKeyCam;
		_File.Write(&Node, sizeof(UINT));
	}

	// �������� Ű ����
	UINT uKeySnow = (UINT)m_listKeySnow.size();
	_File.Write(&uKeySnow, sizeof(UINT));

	vector<LmKeySnow>::iterator itrKeySnow;
	for(itrKeySnow = m_listKeySnow.begin(); itrKeySnow != m_listKeySnow.end();
		itrKeySnow++)
	{	
		LmKeySnow Node = *itrKeySnow;
		_File.Write(&Node, sizeof(LmKeySnow));
	}

	// �Ҳɳ��� ���� ����
	UINT uInfoFirework = (UINT)m_listInfoFirework.size();
	_File.Write(&uInfoFirework, sizeof(UINT));

	vector<LmInfoFirework>::iterator itrInfoFirework;
	for(itrInfoFirework = m_listInfoFirework.begin(); 
		itrInfoFirework != m_listInfoFirework.end();
		itrInfoFirework++)
	{
		LmInfoFirework Node = *itrInfoFirework;
		_File.Write(&Node, sizeof(LmInfoFirework));
	}

	////////////////////////////////////////////////////////////////////////// �̰�.. ������ ó�� �ض�!!
	// �Ҳɳ��� Ű ����
	UINT uKeyFirework= (UINT)m_listKeyFirework.size();
	_File.Write(&uKeyFirework, sizeof(UINT));

	vector<UINT>::iterator itrKeyFirework;
	for(itrKeyFirework = m_listKeyFirework.begin();
		itrKeyFirework != m_listKeyFirework.end();
		itrKeyFirework++)
	{
		UINT Node = *itrKeyFirework;
		_File.Write(&Node, sizeof(UINT));
	}

	// ���� ����
	UINT uInfoSound = (UINT)m_listInfoSound.size();
	_File.Write(&uInfoSound, sizeof(UINT));

	vector<LmSection>::iterator itrSound;
	for(itrSound = m_listInfoSound.begin();
		itrSound != m_listInfoSound.end() ;
		itrSound++)
	{
		//Section�̸� ����
		itrSound->SaveSection(_File);
	}

	//////////////////////////////////////////////////////////////////////////
	// �ؿ� �͵� �����ؾ���!!!!
	//////////////////////////////////////////////////////////////////////////

	/*
	m_uFrameSize = _rhs.m_uFrameSize; 
	m_uFPS = _rhs.m_uFPS;		
	m_uTerrainType = _rhs.m_uTerrainType;
	m_uOldFrame = _rhs.m_uOldFrame;

	// ���� Ÿ�� ����
	m_uTerrainType = _rhs.m_uTerrainType;

	// ��� ����
	m_pBG = _rhs.m_pBG;

	ClearCharAndThing();

	// ĳ���� ����
	map<wstring,LmChar*>::iterator itrChar;
	for(itrChar = _rhs.m_mapChar.begin(); itrChar != _rhs.m_mapChar.end(); itrChar++)
	{
		CopyChar((*itrChar).second->GetXFileName(),
			(*itrChar).second->GetPosX(),
			(*itrChar).second->GetPosZ());
	}

	// ���� ����
	map<wstring,LmThing*>::iterator itrThing;
	for(itrThing = _rhs.m_mapThing.begin(); itrThing != _rhs.m_mapThing.end(); itrThing++)
	{
		PlaceThing((*itrThing).second->GetXFileName(),
			(*itrThing).second->GetPosX(),
			(*itrThing).second->GetPosZ());
	}

*/

}

VOID LmScene::LoadScene( CFile& _File )
{
	// ������ �ϴ� ������� �ε��� �ؾ� �Ѵٴ� ����� ���� ����!

	UINT nCnt = 0;

	// ī�޶� ���� �ε�
	UINT uInfoCameraSize;
	_File.Read(&uInfoCameraSize, sizeof(UINT));
	m_listInfoCamera.clear();
	for(nCnt = 0 ; nCnt < uInfoCameraSize ; nCnt++)
	{
		LmInfoCamera newNode;
		_File.Read(&newNode, sizeof(LmInfoCamera));
		m_listInfoCamera.push_back(newNode);
	}

	// ī�޶� Ű �ε�
	UINT uKeyCameraSize;
	_File.Read(&uKeyCameraSize, sizeof(UINT));
	m_listKeyCamera.clear();
	for(nCnt = 0 ; nCnt < uKeyCameraSize ; nCnt++)
	{
		UINT newNode;
		_File.Read(&newNode, sizeof(UINT));
		m_listKeyCamera.push_back(newNode);

	}

	// �������� Ű �ε�
	UINT uKeySnow;
	_File.Read(&uKeySnow, sizeof(UINT));
	m_listKeySnow.clear();
	for(nCnt = 0 ; nCnt < uKeySnow ; nCnt++)
	{
		LmKeySnow newNode;
		_File.Read(&newNode, sizeof(LmKeySnow));
		m_listKeySnow.push_back(newNode);
	}

	// �Ҳɳ��� ���� �ε�
	UINT uInfoFirework;
	_File.Read(&uInfoFirework, sizeof(UINT));
	m_listInfoFirework.clear();
	for(nCnt = 0 ; nCnt < uInfoFirework ; nCnt++)
	{
		LmInfoFirework newNode;
		_File.Read(&newNode, sizeof(LmInfoFirework));
		m_listInfoFirework.push_back(newNode);
	}

	// �Ҳɳ��� Ű �ε�
	UINT uKeyFirework;
	_File.Read(&uKeyFirework, sizeof(UINT));
	m_listKeyFirework.clear();
	for(nCnt = 0 ; nCnt < uKeyFirework ; nCnt++)
	{
		UINT newNode;
		_File.Read(&newNode, sizeof(UINT));
		m_listKeyFirework.push_back(newNode);
	}

	// ���� ����
	UINT uInfoSound;
	_File.Read(&uInfoSound, sizeof(UINT));
	m_listInfoSound.clear();
	for(nCnt = 0 ; nCnt < uInfoSound ; nCnt++)
	{
		LmSection newNode;
		newNode.LoadSection(_File);
		m_listInfoSound.push_back(newNode);
	}


	//////////////////////////////////////////////////////////////////////////
	// �ؿ� �͵� �����ؾ���!!!!
	//////////////////////////////////////////////////////////////////////////

	

	
	// ��� ����
	m_pBG = m_pBG;
/*
	ClearCharAndThing();

	// ĳ���� ����
	map<wstring,LmChar*>::iterator itrChar;
	for(itrChar = _rhs.m_mapChar.begin(); itrChar != _rhs.m_mapChar.end(); itrChar++)
	{
	CopyChar((*itrChar).second->GetXFileName(),
	(*itrChar).second->GetPosX(),
	(*itrChar).second->GetPosZ());
	}

	// ���� ����
	map<wstring,LmThing*>::iterator itrThing;
	for(itrThing = _rhs.m_mapThing.begin(); itrThing != _rhs.m_mapThing.end(); itrThing++)
	{
	PlaceThing((*itrThing).second->GetXFileName(),
	(*itrThing).second->GetPosX(),
	(*itrThing).second->GetPosZ());
	}

	*/

}

VOID LmScene::GetNumberInfo( LmSceneNumberInfo * _pSceneInfo )
{
	_pSceneInfo->m_uFPS = m_uFPS;
	_pSceneInfo->m_uFrameSize = m_uFrameSize;
	_pSceneInfo->m_uOldFrame = m_uOldFrame;
	_pSceneInfo->m_uTerrainType = m_uTerrainType;
}

VOID LmScene::SetNumberInfo( LmSceneNumberInfo & _rSceneInfo )
{
	m_uFPS = _rSceneInfo.m_uFPS;
	m_uFrameSize = _rSceneInfo.m_uFrameSize;
	m_uOldFrame = _rSceneInfo.m_uOldFrame;
	m_uTerrainType = _rSceneInfo.m_uTerrainType;
}