//
//
//  @ Project : 3D 카툰 메이커
//  @ File Name : LmScene.cpp
//  @ Date : 2007-10-11
//  @ Author : 신동인
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

// 현재 씬의 복사본을 리턴한다. 
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
	// 자기 자신이랑 같다면 그냥 리턴
	
	if(this != &_rhs)
	{

		m_uFrameSize = _rhs.m_uFrameSize; 
		m_uFPS = _rhs.m_uFPS;		
		m_uTerrainType = _rhs.m_uTerrainType;
		m_uOldFrame = _rhs.m_uOldFrame;

		// 지형 타입 복사
		m_uTerrainType = _rhs.m_uTerrainType;

		// 배경 복사
		m_pBG = _rhs.m_pBG;

		ClearCharAndThing();

		// 캐릭터 복사
		map<wstring,LmChar*>::iterator itrChar;
		for(itrChar = _rhs.m_mapChar.begin(); itrChar != _rhs.m_mapChar.end(); itrChar++)
		{
			CopyChar((*itrChar).second->GetXFileName(),
				(*itrChar).second->GetPosX(),
				(*itrChar).second->GetPosZ());
		}

		// 물건 복사
		map<wstring,LmThing*>::iterator itrThing;
		for(itrThing = _rhs.m_mapThing.begin(); itrThing != _rhs.m_mapThing.end(); itrThing++)
		{
			PlaceThing((*itrThing).second->GetXFileName(),
				(*itrThing).second->GetPosX(),
				(*itrThing).second->GetPosZ());
		}

		// 카메라 정보 복사
		m_listInfoCamera.clear();
		vector<LmInfoCamera>::iterator itrInfoCam;
		for(itrInfoCam = _rhs.m_listInfoCamera.begin(); itrInfoCam != _rhs.m_listInfoCamera.end();
			itrInfoCam++)
		{
			m_listInfoCamera.push_back(*itrInfoCam);
		}

		// 카메라 키 복사
		m_listKeySnow.clear();
		vector<UINT>::iterator itrKeyCam;
		for(itrKeyCam = _rhs.m_listKeyCamera.begin(); itrKeyCam != _rhs.m_listKeyCamera.end();
			itrKeyCam++)
		{
			m_listKeyCamera.push_back(*itrKeyCam);
		}

		// 눈내리기 키 복사
		m_listKeySnow.clear();
		vector<LmKeySnow>::iterator itrKeySnow;
		for(itrKeySnow = _rhs.m_listKeySnow.begin(); itrKeySnow != _rhs.m_listKeySnow.end();
			itrKeySnow++)
		{
			m_listKeySnow.push_back(*itrKeySnow);
		}

		// 불꽃놀이 정보 복사
		m_listInfoFirework.clear();
		vector<LmInfoFirework>::iterator itrInfoFirework;
		for(itrInfoFirework = _rhs.m_listInfoFirework.begin(); 
			itrInfoFirework != _rhs.m_listInfoFirework.end();
			itrInfoFirework++)
		{
			m_listInfoFirework.push_back(*itrInfoFirework);
		}

		// 불꽃놀이 키 복사
		m_listKeyFirework.clear();
		vector<UINT>::iterator itrKeyFirework;
		for(itrKeyFirework = _rhs.m_listKeyFirework.begin();
			itrKeyFirework != _rhs.m_listKeyFirework.end();
			itrKeyFirework++)
		{
			m_listKeyFirework.push_back(*itrKeyFirework);
		}

		// 사운드 정보
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
	// 모델 매니저에서 배경의 포인터을 가져와 현재 씬의 배경으로 설정한다.
	m_pBG = TheModelManager->GetBG(_pcwsBG);

	if(m_pBG == NULL)
		return FALSE;

	return TRUE;
}

BOOL LmScene::PlaceThing( LPCWSTR _pcwsThing, FLOAT _fStepX, FLOAT _fStepZ )
{	
	// 복사된 물건의 이름으로 원본이름 뒤에 #숫자를 붙인다. 계속 증가된 숫자를 붙인다.
	// (허술하지만 물체가 엄청나게 만들어지지 않는 이상 문제가 없다..)
	WCHAR wNum[8];
	static int ThingCount = 1;
	_itow_s(ThingCount,wNum, 8 ,10);

	wstring wstrClone = _pcwsThing;
	wstrClone =  wstrClone + L"#" + wNum;
	LPCWSTR pcwsClone = wstrClone.c_str();

	// 다른 물체가 같은 이름이 있는지 확인한다.
	if(CheckThingName(pcwsClone))
		return FALSE;

	// 물체나 캐릭터가 그 위치에 있는지 확인한다.
	if(CheckPos(_fStepX, _fStepZ))
		return FALSE;

	m_mapThing[pcwsClone] = TheModelManager->GetThing(_pcwsThing)->Clone(pcwsClone);
	m_mapThing[pcwsClone]->Place(_fStepX, _fStepZ);

	ThingCount++;

	return TRUE;
}

BOOL LmScene::PlaceChar( LPCWSTR _pcwsChar, FLOAT _fStepX, FLOAT _fStepZ )
{
	// 복사된 캐릭터의 이름으로 원본이름 뒤에 #숫자를 붙인다. 계속 증가된 숫자를 붙인다.
	// (허술하지만 캐릭터가 엄청나게 만들어지지 않는 이상 문제가 없다..)
	WCHAR wNum[8];
	static int CharCount = 1;
	_itow_s(CharCount,wNum, 8 ,10);

	wstring wstrClone = _pcwsChar;
	wstrClone = wstrClone + L"#" + wNum;
	LPCWSTR pcwsClone = wstrClone.c_str();

	// 다른 캐릭터가 같은 이름이 있는지 확인한다.
	if(CheckCharName(pcwsClone))
		return FALSE;

	// 물체나 캐릭터가 그 위치에 있는지 확인한다.
	if(CheckPos(_fStepX, _fStepZ))
		return FALSE;

	// 타임 라인에 캐릭터 레이어를 추가한다.
	TheTimeLine->InsertCharacterLayer(pcwsClone);

	m_mapChar[pcwsClone] = TheModelManager->GetChar(_pcwsChar)->Clone(pcwsClone);
	m_mapChar[pcwsClone]->SetFrameSize(m_uFrameSize);
	m_mapChar[pcwsClone]->Place(_fStepX, _fStepZ);

	CharCount++;
	return TRUE;
}

BOOL LmScene::CopyChar( LPCWSTR _pcwsChar, FLOAT _fStepX, FLOAT _fStepZ )
{
	// 복사된 캐릭터의 이름으로 원본이름 뒤에 #숫자를 붙인다. 계속 증가된 숫자를 붙인다.
	// (허술하지만 캐릭터가 엄청나게 만들어지지 않는 이상 문제가 없다..)
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
	// 복사본의 자기만의 변수를 지우고
	LmThing* pThing = m_mapThing[_pcwsName];

	m_mapThing.erase(_pcwsName);
	delete pThing;

	return TRUE;
}

BOOL LmScene::DeleteChar( LPCWSTR _pcwsName )
{
	// 복사본의 자기만의 변수를 지우고
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
	// 같은 프레임을 여러번 클릭하는 것을 막는다.
	if(m_uOldFrame == _uFrame)
		return;

	m_uOldFrame = _uFrame;

	// 현재 프레임의 카메라 좌표 설정
	LmInfoCamera Camera = GetCameraInfo(_uFrame);
	TheCamera->SetEyePt(Camera.vEye.x, Camera.vEye.y, Camera.vEye.z);

	// 현재 프레임의 캐릭터 설정
	map<wstring,LmChar*>::iterator itrChar;
	for(itrChar = m_mapChar.begin(); itrChar != m_mapChar.end(); itrChar++)
	{
		if((*itrChar).second != NULL)
			(*itrChar).second->SetCurrFrame(_uFrame);
	}


	//현재 프레임에 이펙트를 켤지 끌지 확인
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
	// 배경을 그린다.
	if(!m_pDevice)
		m_pDevice = _pDevice;
	else
	{
		if(m_pBG)
			m_pBG->Render(_pDevice);

		// 물건을 그린다.
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

// 같은 물체 이름이 있다면 TRUE를 리턴한다.
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

// 같은 캐릭터 이름이 있다면 TRUE를 리턴한다.
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

	// 키값이 이미 있다면 새로 삽입하지 않는다.
	vector<UINT>::iterator itr;
	itr = find(m_listKeyCamera.begin(), m_listKeyCamera.end(), _uFrameNo);

	if(itr == m_listKeyCamera.end())
	{
		// 키를 삽입하고 정렬한다.
		m_listKeyCamera.push_back(_uFrameNo);
		sort(m_listKeyCamera.begin(), m_listKeyCamera.end());
	}

	// 키가 삽입이 되면 이전 키값이 있는지 확인하고 보간한다.
	INT preFrameNo = -1, postFrameNo = -1;

	// 앞의 키값을 찾는다.
	preFrameNo = FindPreKey(_uFrameNo);

	// 앞의 키 다음에서 현재 키전까지 보간한다.
	if(preFrameNo != -1)
		LerpFrame(preFrameNo, _uFrameNo);


	// 뒤의 키 값을 찾는다.
	postFrameNo = FindPostKey(_uFrameNo);

	// 현재 키 다음에서 뒤의 키전까지 보간한다.
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
	// 지정한 위치의 카메라 프레임이 키 프레임이 아니면 실패한다.
	//if(m_listInfoCamera[_uFrameNo].bKey == FALSE)
	//	return FALSE;

	// 키 리스트의 키 값을 지우고 정렬한다.
	vector<UINT>::iterator itr;
	itr = find(m_listKeyCamera.begin(), m_listKeyCamera.end(), _uFrameNo);

	// 키 리스트에 키가 없다면 실패
	if(itr == m_listKeyCamera.end())
		return FALSE;

	// 키를 지운다.
	m_listKeyCamera.erase(itr);

	INT iPreCameraKeyNo, iPostCameraKeyNo;
	iPreCameraKeyNo = FindPreKey(_uFrameNo);
	iPostCameraKeyNo = FindPostKey(_uFrameNo);

	// 현재 위치의 프레임에서 앞과 뒤의 키 프레임을 찾아서 그사이를 보간한다.
	// 둘다 없으면 그냥 자기만 리셋
	if((iPreCameraKeyNo == -1)&&(iPostCameraKeyNo == -1))
		m_listInfoCamera[_uFrameNo].Reset();

	else if((iPreCameraKeyNo != -1)&&(iPostCameraKeyNo != -1))
	{
		LerpFrame(iPreCameraKeyNo, iPostCameraKeyNo);
		return TRUE;
	}

	// 앞이 없으면 뒷구간을 리셋한다.
	else if(iPreCameraKeyNo == -1)
	{
		ResetFrame(_uFrameNo, iPostCameraKeyNo);
		return TRUE;
	}

	// 뒤가 없으면 앞구간을 리셋한다.
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
	// 지형 정보를 다시로드한다.
	TheTerrain->SetTerrainType(m_uTerrainType);

	// 먼저 타임라인을 초기화한다.
	TheTimeLine->SetFrameLength(m_uFrameSize);
	TheTimeLine->InitTimeLIne();
	// 씬의 데이터에 맞게 타임라인의 섹션을 설정한다.

	// 사운드 정보
	vector<LmSection>::iterator itrSound;
	for(itrSound = m_listInfoSound.begin(); itrSound != m_listInfoSound.end() ; itrSound++)
	{
		TheTimeLine->ReloadSoundSection(itrSound->m_wstrSectionName.c_str() , 
												itrSound->m_uStartFrm, itrSound->m_uEndFrm, 
												itrSound->m_dwFadeIn, itrSound->m_dwFadeOut);
	}

	// 카메라 레이어
	vector<UINT>::iterator itrCam;
	for( itrCam = m_listKeyCamera.begin(); itrCam != m_listKeyCamera.end(); itrCam++)
		TheTimeLine->InsertCameraSection(*itrCam);
	
	// 캐릭터 액션 레이어
	map<wstring,LmChar*>::iterator itrChar;
	for(itrChar = m_mapChar.begin(); itrChar != m_mapChar.end(); itrChar++)
	{
		//캐릭터를 추가한다.
		TheTimeLine->InsertCharacterLayer((*itrChar).second->GetName());

		// 캐릭터의 액션과 말풍선 레이어<미구현>를 추가한다.
		(*itrChar).second->InitTimeLine();
	}

	// 눈내리기 효과 레이어
	vector<LmKeySnow>::iterator itrSnow;
	for(itrSnow = m_listKeySnow.begin(); itrSnow != m_listKeySnow.end(); itrSnow++)
		TheTimeLine->InsertSnowSection(L"Snow Section",(*itrSnow).uStartFrame,(*itrSnow).uEndFrame);


	// 불꽃놀이 효과 레이어
	vector<UINT>::iterator itr;
	for( itr = m_listKeyFirework.begin(); itr != m_listKeyFirework.end(); itr++)
		TheTimeLine->InsertFireWorksSection(*itr);
	
	// [진왕] 타임라인에 모든 정보가 들어가면, UI에 있는 ListCtrl들을 타임라인에 근거하여 입력한다.
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
	//못찾았음!
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
	// 저장을 하는 순서대로 로딩을 해야 한다는 사실을 잊지 말자!

	// 카메라 정보 저장
	UINT uInfoCameraSize = (UINT)m_listInfoCamera.size();
	_File.Write(&uInfoCameraSize, sizeof(UINT));

	vector<LmInfoCamera>::iterator itrInfoCam;
	for(itrInfoCam = m_listInfoCamera.begin(); itrInfoCam != m_listInfoCamera.end();
		itrInfoCam++)
	{
		LmInfoCamera Node = (*itrInfoCam);
		_File.Write(&Node, sizeof(LmInfoCamera));
	}

	// 카메라 키 저장
	UINT uKeyCameraSize = (UINT)m_listKeyCamera.size();
	_File.Write(&uKeyCameraSize, sizeof(UINT));
	vector<UINT>::iterator itrKeyCam;
	for(itrKeyCam = m_listKeyCamera.begin(); itrKeyCam != m_listKeyCamera.end();
		itrKeyCam++)
	{
		UINT Node = *itrKeyCam;
		_File.Write(&Node, sizeof(UINT));
	}

	// 눈내리기 키 저장
	UINT uKeySnow = (UINT)m_listKeySnow.size();
	_File.Write(&uKeySnow, sizeof(UINT));

	vector<LmKeySnow>::iterator itrKeySnow;
	for(itrKeySnow = m_listKeySnow.begin(); itrKeySnow != m_listKeySnow.end();
		itrKeySnow++)
	{	
		LmKeySnow Node = *itrKeySnow;
		_File.Write(&Node, sizeof(LmKeySnow));
	}

	// 불꽃놀이 정보 저장
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

	////////////////////////////////////////////////////////////////////////// 이거.. 위에꺼 처럼 해라!!
	// 불꽃놀이 키 저장
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

	// 사운드 정보
	UINT uInfoSound = (UINT)m_listInfoSound.size();
	_File.Write(&uInfoSound, sizeof(UINT));

	vector<LmSection>::iterator itrSound;
	for(itrSound = m_listInfoSound.begin();
		itrSound != m_listInfoSound.end() ;
		itrSound++)
	{
		//Section이름 저장
		itrSound->SaveSection(_File);
	}

	//////////////////////////////////////////////////////////////////////////
	// 밑에 것들 저장해야함!!!!
	//////////////////////////////////////////////////////////////////////////

	/*
	m_uFrameSize = _rhs.m_uFrameSize; 
	m_uFPS = _rhs.m_uFPS;		
	m_uTerrainType = _rhs.m_uTerrainType;
	m_uOldFrame = _rhs.m_uOldFrame;

	// 지형 타입 복사
	m_uTerrainType = _rhs.m_uTerrainType;

	// 배경 복사
	m_pBG = _rhs.m_pBG;

	ClearCharAndThing();

	// 캐릭터 복사
	map<wstring,LmChar*>::iterator itrChar;
	for(itrChar = _rhs.m_mapChar.begin(); itrChar != _rhs.m_mapChar.end(); itrChar++)
	{
		CopyChar((*itrChar).second->GetXFileName(),
			(*itrChar).second->GetPosX(),
			(*itrChar).second->GetPosZ());
	}

	// 물건 복사
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
	// 저장을 하는 순서대로 로딩을 해야 한다는 사실을 잊지 말자!

	UINT nCnt = 0;

	// 카메라 정보 로딩
	UINT uInfoCameraSize;
	_File.Read(&uInfoCameraSize, sizeof(UINT));
	m_listInfoCamera.clear();
	for(nCnt = 0 ; nCnt < uInfoCameraSize ; nCnt++)
	{
		LmInfoCamera newNode;
		_File.Read(&newNode, sizeof(LmInfoCamera));
		m_listInfoCamera.push_back(newNode);
	}

	// 카메라 키 로딩
	UINT uKeyCameraSize;
	_File.Read(&uKeyCameraSize, sizeof(UINT));
	m_listKeyCamera.clear();
	for(nCnt = 0 ; nCnt < uKeyCameraSize ; nCnt++)
	{
		UINT newNode;
		_File.Read(&newNode, sizeof(UINT));
		m_listKeyCamera.push_back(newNode);

	}

	// 눈내리기 키 로딩
	UINT uKeySnow;
	_File.Read(&uKeySnow, sizeof(UINT));
	m_listKeySnow.clear();
	for(nCnt = 0 ; nCnt < uKeySnow ; nCnt++)
	{
		LmKeySnow newNode;
		_File.Read(&newNode, sizeof(LmKeySnow));
		m_listKeySnow.push_back(newNode);
	}

	// 불꽃놀이 정보 로딩
	UINT uInfoFirework;
	_File.Read(&uInfoFirework, sizeof(UINT));
	m_listInfoFirework.clear();
	for(nCnt = 0 ; nCnt < uInfoFirework ; nCnt++)
	{
		LmInfoFirework newNode;
		_File.Read(&newNode, sizeof(LmInfoFirework));
		m_listInfoFirework.push_back(newNode);
	}

	// 불꽃놀이 키 로딩
	UINT uKeyFirework;
	_File.Read(&uKeyFirework, sizeof(UINT));
	m_listKeyFirework.clear();
	for(nCnt = 0 ; nCnt < uKeyFirework ; nCnt++)
	{
		UINT newNode;
		_File.Read(&newNode, sizeof(UINT));
		m_listKeyFirework.push_back(newNode);
	}

	// 사운드 정보
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
	// 밑에 것들 저장해야함!!!!
	//////////////////////////////////////////////////////////////////////////

	

	
	// 배경 복사
	m_pBG = m_pBG;
/*
	ClearCharAndThing();

	// 캐릭터 복사
	map<wstring,LmChar*>::iterator itrChar;
	for(itrChar = _rhs.m_mapChar.begin(); itrChar != _rhs.m_mapChar.end(); itrChar++)
	{
	CopyChar((*itrChar).second->GetXFileName(),
	(*itrChar).second->GetPosX(),
	(*itrChar).second->GetPosZ());
	}

	// 물건 복사
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