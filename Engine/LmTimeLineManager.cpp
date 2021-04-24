#include "StdAfx.h"
#include "LmTimeLineManager.h"
#include "../3CMaker/resource.h"
#include "../3CMaker/LmUIMainToolFormView.h"
#include "../3CMaker/LmUIMainToolDlg.h"
#include "LmSceneManager.h"
#include "LmScene.h"


//////////////////////////////////////////////////////////////////////////
//DEBUG용 함수
VOID trace_list(LayerList & inList)
{
	LayerListItr itr;
	for(itr=inList.begin() ; itr!= inList.end(); itr++)
	{
		TRACE(L"Layer Name : %s\r\n", itr->m_wstrLayerName.c_str());
	}
}
//////////////////////////////////////////////////////////////////////////


LmTimeLineManager::LmTimeLineManager(VOID)
{
	m_nFrmPerSec = 20;			//초당 프레임 수 기본 20 Frame!
	m_nCurCaret = 0;			//현재 커서의 위치 - 1부터 시작!!
	m_nFrameLength = 1000;		//프레임 길이 -- Scene에 따라 다름
	m_uFireWorksLength = 40;	//불꽃놀이 길이 40 Frame
}

LmTimeLineManager::~LmTimeLineManager(VOID)
{
}

wstring LmTimeLineManager::AddLayer( LPCWSTR _pcwsLayerName, LmKIND_OF_LAYER _enumKindofLayer )
{
	LmLayer NewLayer;

	////Layer 초기화
	NewLayer.m_enumKindofLayer = _enumKindofLayer;
	NewLayer.m_wstrLayerName = GetNewLayerName(_pcwsLayerName);
	
	m_listLayer.push_back(NewLayer);

	DrawTimeLineView();

	return NewLayer.m_wstrLayerName;
}

wstring LmTimeLineManager::InsertLayer( LayerListItr _itrLayer,LPCWSTR _pcwsLayerName, LmKIND_OF_LAYER _enumKindofLayer )
{

	LmLayer NewLayer;

	//insert는 iterator가 가리키는 element의 바로 앞에 대입하므로 
	//한칸 뒤로 가야함.
	_itrLayer++;

	//Layer 초기화
	NewLayer.m_enumKindofLayer = _enumKindofLayer;
	NewLayer.m_wstrLayerName = GetNewLayerName(_pcwsLayerName);

	m_listLayer.insert( _itrLayer,NewLayer);

	DrawTimeLineView();

	return NewLayer.m_wstrLayerName;

}


LayerListItr LmTimeLineManager::FindForLayerName(LPCWSTR _pcwsLayerName)
{
	LayerListItr itrLayerFinder = m_listLayer.begin();
	LayerListItr itrLayerEnd = m_listLayer.end();

	for(; itrLayerFinder != itrLayerEnd ; itrLayerFinder++)
	{
		wstring wstrListLayerName = itrLayerFinder->m_wstrLayerName;
		wstring wstrInputLayerName = _pcwsLayerName;

		//찾았으면 리턴

		if( wstrListLayerName == wstrInputLayerName )
			return itrLayerFinder;
	}

	//못찾았으면 itrLayerEnd를 리턴
	return itrLayerFinder;

}


wstring LmTimeLineManager::GetNewLayerName( LPCWSTR _pcwsLayerName )
{
	INT nCnt=0;
	WCHAR wcsKeyNameBuffer[512];
	LayerListItr itrFindKey;

	//입력받는 키 값이 같은지 확인한다. 
	//같은 키값이 있으면
	//레이어 이름 마지막에 번호를 붙인다.
	
	wcsncpy_s(wcsKeyNameBuffer, _pcwsLayerName, wcslen(_pcwsLayerName));
	itrFindKey = FindForLayerName(wcsKeyNameBuffer);
	while(itrFindKey != m_listLayer.end())
	{	
		nCnt++;
		memset(wcsKeyNameBuffer, 0, sizeof(wcsKeyNameBuffer));
		StringCbPrintfW(wcsKeyNameBuffer, sizeof(wcsKeyNameBuffer), L"%s(%d)", _pcwsLayerName, nCnt);
		itrFindKey = FindForLayerName(wcsKeyNameBuffer);
	} 

	wstring wstrResult = wcsKeyNameBuffer;
	return wstrResult;
}

INT LmTimeLineManager::RemoveLayer( LPCWSTR _pcwsLayerName )
{
	LayerListItr itrFindKey = FindForLayerName(_pcwsLayerName);

	//키 값을 찾았는가?
	//찾았으면 지워라.
	if(itrFindKey != m_listLayer.end() )
	{
		m_listLayer.erase(itrFindKey);
		
		DrawTimeLineView();

		return 0;
	}
	return -1; //에러 있음 - 못찾았음.

}

LmUITimeLineView* LmTimeLineManager::GetTimeLineView()
{
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
	LmUITimeLineView* pTimeLineView = (LmUITimeLineView*) ( pMainFrame->m_wndSplitRow.GetPane(1, 0) );
	return pTimeLineView;
}

VOID LmTimeLineManager::DrawTimeLineView()
{
	LmUITimeLineView* pTimeLineView = GetTimeLineView();
	pTimeLineView->ReFreshVertScroll();
	pTimeLineView->Invalidate();
}

VOID LmTimeLineManager::ModifyLayerName( LPCWSTR _pcwsLayerName, LPCWSTR _pcwsNewLayerName )
{
	LayerListItr itrFinder = FindForLayerName(_pcwsLayerName);
	itrFinder->m_wstrLayerName = GetNewLayerName(_pcwsNewLayerName);
}

VOID LmTimeLineManager::InsertSoundSection( LPCWSTR _pcwsSoundName, UINT _uStartFrm, DWORD _dwSoundLength, DWORD _dwFadeIn, DWORD _dwFadeOut )
{
	//사운드는 기본적으로 사운드 이름과 시작 프레임이 있다. 
	//중요한 것은 제귀적인 알고리즘을 구현하는 것이다.
	//우선 시작 프레임에서 몇 프레임이 소요되는지를 알아야 한다. 
	
	//입력을 하다가 최대 프레임을 초과 하는 경우 그만큼 확장하지 않고 데이터 값을 저장해 둔다.
	//타임라인뷰에서 그릴때 자동으로 잘라주기 때문임.

	//사운드가 몇 프레임인지 알아낸다.
	UINT nSoundFrmLength = ChangeMsecToFrame(_dwSoundLength);
	UINT uEndFrm = _uStartFrm + nSoundFrmLength;

	InsertSoundSectionProcess(_pcwsSoundName, _uStartFrm, uEndFrm, _dwFadeIn, _dwFadeOut);

	// 현재 선택된 씬이 없으면 실패
	LmScene* pNowScene = TheSceneManager->GetNowScene();
	if(pNowScene == NULL)
		return;

	pNowScene->InsertSoundSection(_pcwsSoundName, _uStartFrm, uEndFrm, _dwFadeIn, _dwFadeOut);

	DrawTimeLineView();
	return;
}

VOID LmTimeLineManager::InsertSoundSectionProcess( LPCWSTR _pcwsSoundName, UINT _uStartFrm, UINT _uEndFrm, DWORD _dwFadeIn, DWORD _dwFadeOut )
{
	//사운드는 기본적으로 사운드 이름과 시작 프레임이 있다. 
	//중요한 것은 제귀적인 알고리즘을 구현하는 것이다.
	//우선 시작 프레임에서 몇 프레임이 소요되는지를 알아야 한다. 

	//입력을 하다가 최대 프레임을 초과 하는 경우 그만큼 확장하지 않고 데이터 값을 저장해 둔다.
	//타임라인뷰에서 그릴때 자동으로 잘라주기 때문임.

	//사운드가 몇 프레임인지 알아낸다.

	LayerListItr itrLastSoundlayer = m_listLayer.end();
	LayerListItr itrLayer = m_listLayer.begin();
	LayerListItr itrLayerEnd = m_listLayer.end();

	BOOL isInserted = FALSE;
	for (; itrLayer != itrLayerEnd ; itrLayer++)
	{
		//사운드 레이어 인지 확인
		if(itrLayer->GetKindOfLayer() != SOUND_LAYER)
			continue;

		//Sound Section을 Insert해 본다.
		INT nInsertResult = itrLayer->InsertSection(_pcwsSoundName, _uStartFrm, _uEndFrm, _dwFadeIn, _dwFadeOut);
		if(!nInsertResult)
		{
			isInserted = TRUE;
			break;
		}

		//IsThereArea==0 일 경우에는 다음 사운드 레이어를 찾는다.
		//마지막 사운드 레이어를 찾기 위해
		itrLastSoundlayer = itrLayer;
	}

	//충분한 공간을 가진 사운드 레이어가 없다면, 사운드 레이어를 하나 만들고 Sound Section을 하나 넣는다.
	if(!isInserted)
	{
		wstring wsLayerName = L"Sound Layer";
		wstring wsInsertedName = InsertLayer(itrLastSoundlayer, wsLayerName.c_str(), SOUND_LAYER);
		LayerListItr itrFinder =  FindForLayerName(wsInsertedName.c_str());
		itrFinder->InsertSection( _pcwsSoundName, _uStartFrm, _uEndFrm, _dwFadeIn , _dwFadeOut);
	}

	DrawTimeLineView();
	return;
}




UINT LmTimeLineManager::ChangeMsecToFrame( DWORD _dwSoundLength )
{
	//밀리초 단위의 시간을 초로 변환
	UINT nFrameSec = _dwSoundLength /1000;

	//초를 초당 프레임 수로 나눈다.
	UINT nFramelength = nFrameSec * m_nFrmPerSec;

	//나머지가 있으면 1을 증가시켜준다.
	if(nFrameSec  % m_nFrmPerSec)
		nFramelength++;

	return nFramelength;
}

INT LmTimeLineManager::RemoveSoundSection(LPCWSTR _pcwsSoundName)
{
	//Sound Name을 Key값으로 모든 사운드 레이어에서 찾는다.
	LayerListItr itrLastSoundlayer = m_listLayer.end();
	LayerListItr itrLayer = m_listLayer.begin();
	LayerListItr itrLayerEnd = m_listLayer.end();

	wstring wsSoundName = _pcwsSoundName;

	for(; itrLayer != itrLayerEnd ; itrLayer++)
	{
		//사운드 레이어만 검사
		if(itrLayer->m_enumKindofLayer != SOUND_LAYER)
			continue;

		//각 사운드 레이어에서 이름으로 Remove를 시도하고 Remove가 성공했으면 빠져나간다.
		INT nRemoveResult = itrLayer->RemoveSection(_pcwsSoundName);
		
		if(!nRemoveResult)
		{
			//만일 사운드레이어에 사운드가 없고, "Sound Layer"가 아니면 그 레이어를 지워버린다. 
			INT nSoundSectionCount = (INT)itrLayer->m_mapSection.size();
			if( (!nSoundSectionCount) && 
				 (itrLayer->m_wstrLayerName != L"Sound Layer"))
				RemoveLayer(itrLayer->m_wstrLayerName.c_str());
			DrawTimeLineView();

			// 현재 선택된 씬이 없으면 -1을 리턴
			LmScene* pNowScene = TheSceneManager->GetNowScene();
			if(pNowScene == NULL)
				return -1;

			pNowScene->RemoveSoundSection(_pcwsSoundName);

			return 0;
		}
	}

	//없으면 -1을 반환한다.
	DrawTimeLineView();
	return -1;
}

INT LmTimeLineManager::ModifySoundSection( LPCWSTR _pcwsSoundName, LPCWSTR _pcwsNewSoundName, UINT _uStartFrm, DWORD _dwSoundLength, DWORD _dwFadeIn, DWORD _dwFadeOut )
{
	//기존의 등록된 사운드를 지우고 다시 등록한다.
	if(RemoveSoundSection(_pcwsSoundName)<0)
		return -1;
	InsertSoundSection(_pcwsNewSoundName, _uStartFrm, _dwSoundLength, _dwFadeIn, _dwFadeOut);

	DrawTimeLineView();
	return 0;
}

LmTimeLineManager* LmTimeLineManager::GetInstance()
{
	static LmTimeLineManager TimeLineManager;

	return &TimeLineManager;
}


UINT LmTimeLineManager::GetCurCaret()
{
	return m_nCurCaret;
}

INT LmTimeLineManager::GetFrameLength()
{
	return m_nFrameLength;
}

INT LmTimeLineManager::GetFramePerSec()
{
	return m_nFrmPerSec;
}

BOOL LmTimeLineManager::SetCurCaret( INT _iCurCaret )
{
	if(_iCurCaret > m_nFrameLength)
		return FALSE;

	m_nCurCaret = _iCurCaret;

	return TRUE;
}

VOID LmTimeLineManager::SetFrameLength( INT _iFrameLength )
{
	m_nFrameLength = _iFrameLength;
	INT nLayerCnt = GetListSize();

	GetTimeLineView()->ReSizeFrameLength( nLayerCnt,  _iFrameLength );
}

VOID LmTimeLineManager::SetFramePerSec( INT _iFrmPerSec )
{
	m_nFrmPerSec = _iFrmPerSec;

	//모든 레이어에 있는 Secton들의 길이를 다시 갱신 해 줘야 한다. 
	//그래서 모든 씬의 초당 프레임 수는 10 Frame 이다. 

}

VOID LmTimeLineManager::InitTimeLIne()
{
	////////////////////////////////////////////////////////////////////////////
	//// 타임라인 초기화
	//// 여기에다가 TheCartoonMaker에게 모든 Things의 정보를 받아야 할텐데..
	// [동인] 여기에 필요한 Layer 밑 Section을 넣어야 함.
	m_listLayer.clear();
	SetCurCaret(1);
	AddLayer(L"Sound Layer", SOUND_LAYER);
	AddLayer(L"Snow Layer", SNOW_LAYER);
	AddLayer(L"Fireworks Layer", FIREWORKS_LAYER);
	AddLayer(L"Camera Layer", CAMERA_LAYER);
}

INT LmTimeLineManager::GetListSize()
{
	return (INT)m_listLayer.size();
}

LayerListItr LmTimeLineManager::GetListBeginItr()
{
	return m_listLayer.begin();
}

LayerListItr LmTimeLineManager::GetListEndItr()
{
	return m_listLayer.end();
}

wstring LmTimeLineManager::InsertCharacterLayer( LPCWSTR _pcwsCharacterName )
{
	wstring wstrCharacterName = AddLayer(_pcwsCharacterName, CHARACTER_LAYER);
	wstring wstrTalkboxName = GetCharacterTalkBoxName( wstrCharacterName.c_str());
	wstring TalkBoxName = AddLayer(wstrTalkboxName.c_str(), TALKBOX_LAYER);

	//DrawTimeLineView();
	return wstrCharacterName;
}

LayerListItr LmTimeLineManager::FindForLayerNameStrStr( LPCWSTR _pcwsLayerName, LmKIND_OF_LAYER _KindofLayer )
{
	LayerListItr itrLayerFinder = m_listLayer.begin();
	LayerListItr itrLayerEnd = m_listLayer.end();

	for(; itrLayerFinder != itrLayerEnd ; itrLayerFinder++)
	{
		wstring wstrListLayerName = itrLayerFinder->m_wstrLayerName;
		wstring wstrInputLayerName = _pcwsLayerName;

		//찾았으면 리턴
		if(wcsstr(wstrListLayerName.c_str(), wstrInputLayerName.c_str()) 
			&& (itrLayerFinder->m_enumKindofLayer == _KindofLayer) )
			return itrLayerFinder;
	}

	//못찾았으면 itrLayerEnd를 리턴
	return itrLayerFinder;
}

INT LmTimeLineManager::RemoveCharacterLayer( LPCWSTR _pcwsCharacterName )
{
	//캐릭터 레이어를 지운다.
	RemoveLayer(_pcwsCharacterName);
	wstring wstrTalkboxName = GetCharacterTalkBoxName( _pcwsCharacterName );

	//만일 TalkBox를 못찼았을 경우 Character 이름을 포함하고 있는 TalkBox를 삭제한다.
	if(RemoveLayer(wstrTalkboxName.c_str())<0) 
	{
		LayerListItr itrFinder = FindForLayerNameStrStr( wstrTalkboxName.c_str(), TALKBOX_LAYER);
		if(itrFinder != m_listLayer.end())
			RemoveLayer(itrFinder->m_wstrLayerName.c_str());
		else
			//그래도 TalkBox를 찾지 못했을 경우 -1리턴
			return -1;
	}
	//DrawTimeLineView();
	return 0;
}

wstring LmTimeLineManager::GetCharacterTalkBoxName( LPCWSTR _pcwsCharacterName )
{
	//캐릭터 이름을 입력하면 해당 캐릭터가 가지는 TalkBox Layer를 반환한다.
	WCHAR wcsNameBuff[512];
	swprintf_s(wcsNameBuff, 512,L"%s_TalkBox",_pcwsCharacterName);
	wstring wstrResult = wcsNameBuff;
	return wstrResult;
}

INT LmTimeLineManager::ModifyCharacterLayerName( LPCWSTR _pcwsLayerName, LPCWSTR _pcwsNewLayerName )
{
	wstring wstrTalkBoxName = GetCharacterTalkBoxName(_pcwsLayerName);
	wstring wstrTalkBoxNewName = GetCharacterTalkBoxName(_pcwsNewLayerName);

	ModifyLayerName(_pcwsLayerName, _pcwsNewLayerName);
	ModifyLayerName(wstrTalkBoxName.c_str(), wstrTalkBoxNewName.c_str());

	DrawTimeLineView();
	return 0;
}

INT LmTimeLineManager::InsertTalkBoxSection( LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uTimeLength)
{
	wstring wstrTalkBoxName = GetCharacterTalkBoxName(_pcwsCharacterName);
	return InsertLayerSection(wstrTalkBoxName.c_str(), L"", _uStartFrm, _uTimeLength);
}

INT LmTimeLineManager::InsertTalkBoxSection( LPCWSTR _pcwsCharacterName, LmSection _Section )
{
	wstring wstrTalkBoxName = GetCharacterTalkBoxName(_pcwsCharacterName);
	return InsertLayerSection(wstrTalkBoxName.c_str(), _Section);
}

INT LmTimeLineManager::RemoveTalkBoxSection( LPCWSTR _pcwsCharacterName, UINT _uStartFrm)
{
	wstring wstrTalkBoxName = GetCharacterTalkBoxName(_pcwsCharacterName);
	return RemoveLayerSection(wstrTalkBoxName.c_str(), _uStartFrm);
}

INT LmTimeLineManager::ModifyTalkBoxSection( LPCWSTR _pcwsCharacterName, UINT _uStartFrm,  UINT _uNewStartFrm, UINT _uNewTimeLength)
{
	wstring wstrTalkBoxName = GetCharacterTalkBoxName(_pcwsCharacterName);
	LayerListItr itrLayerFinder = FindForLayerName(wstrTalkBoxName.c_str());
	SectionMapItr itrSecFinder = itrLayerFinder->m_mapSection.find(_uStartFrm);
	LmSection secBk = itrSecFinder->second;

	//기존의 말풍선을 지운다.
	if(RemoveTalkBoxSection(_pcwsCharacterName, _uStartFrm)<0)
		return -1;

	//새로운 말풍선의 Section 공간 확보가 실패 했을때 다시 이전의 모습으로 돌아간다.
	if(InsertTalkBoxSection(_pcwsCharacterName, _uNewStartFrm, _uNewTimeLength)<0)
	{
		InsertTalkBoxSection(_pcwsCharacterName, secBk);
		return -2;
	}
	return 0;
}

INT LmTimeLineManager::InsertLayerSection( LPCWSTR _pcwsLayerName, LPCWSTR _pcwsSectionName, UINT _uStartFrm, UINT _uTimeLength )
{
	LayerListItr itrFinder = FindForLayerName(_pcwsLayerName);

	//Layer를 못찾으면 -1 리턴!
	if(itrFinder==m_listLayer.end())
		return -1;

	INT nEndFrm = _uStartFrm + _uTimeLength - 1;
	
	//타임라인 길이를 넘어가면 -3 리턴
	if(nEndFrm > m_nFrameLength)
		return -3;

	INT nResult = itrFinder->InsertSection(_pcwsSectionName, _uStartFrm, nEndFrm);

	//Section을 넣을 공간이 없을 경우 -1을 리턴한다. 
	if(nResult < 0)
		return -2;

	DrawTimeLineView();
	return 0;
}

INT LmTimeLineManager::InsertLayerSection( LPCWSTR _pcwsLayerName, LmSection _Section )
{
	LayerListItr itrFinder = FindForLayerName(_pcwsLayerName);

	//Layer를 못찾으면 -1 리턴!
	if(itrFinder==m_listLayer.end())
		return -1;

	//타임라인 길이를 넘어가면 -3 리턴
	if(_Section.m_uEndFrm > (UINT)m_nFrameLength)
		return -3;


	INT nResult = itrFinder->InsertSection(_Section);

	//Section을 넣을 공간이 없을 경우 -1을 리턴한다. 
	if(nResult < 0)
		return -2;

	DrawTimeLineView();
	return 0;
}

INT LmTimeLineManager::InsertLayerSection( LPCWSTR _pcwsLayerName, UINT _uStartFrm, UINT _uTimeLength )
{
	LayerListItr itrFinder = FindForLayerName(_pcwsLayerName);

	//Layer를 못찾으면 -1 리턴!
	if(itrFinder==m_listLayer.end())
		return -1;

	INT nEndFrm = _uStartFrm + _uTimeLength - 1;

	//타임라인 길이를 넘어가면 -3 리턴
	if(nEndFrm > m_nFrameLength)
		return -3;

	INT nResult = itrFinder->InsertSection(L"", _uStartFrm, nEndFrm);

	//Section을 넣을 공간이 없을 경우 -1을 리턴한다. 
	if(nResult < 0)
		return -2;

	DrawTimeLineView();
	return 0;
}

INT LmTimeLineManager::RemoveLayerSection( LPCWSTR _pcwsLayerName, LPCWSTR _pcwsSectionName )
{
	LayerListItr itrFinder = FindForLayerName(_pcwsLayerName);

	//TalkBox를 못찾으면 -1 리턴!
	if(itrFinder==m_listLayer.end())
		return -1;

	INT nResult = itrFinder->RemoveSection(_pcwsSectionName);

	//말풍선 이름을 찾지 못해 삭제하지 못했을 경우 -1을 리턴한다.
	if(nResult < 0)
		return -1;

	DrawTimeLineView();
	return 0;
}

INT LmTimeLineManager::RemoveLayerSection( LPCWSTR _pcwsLayerName, UINT _uStartFrm )
{
	LayerListItr itrFinder = FindForLayerName(_pcwsLayerName);

	//레이어를 못찾으면 -1 리턴!
	if(itrFinder==m_listLayer.end())
		return -1;

	INT nKeyFrame = GetSectionFirstFrame(_pcwsLayerName, _uStartFrm);
	INT nResult = itrFinder->RemoveSection(nKeyFrame);

	// Section을 찾지 못해 삭제하지 못했을 경우 -1을 리턴한다.
	if(nResult < 0)
		return -1;

	DrawTimeLineView();
	return 0;
}
INT LmTimeLineManager::InsertActionSection( LPCWSTR _pcwsCharacterName,  UINT _uStartFrm, UINT _uTimeLength )
{
	return InsertLayerSection(_pcwsCharacterName, L"", _uStartFrm, _uTimeLength);
}

INT LmTimeLineManager::InsertActionSection( LPCWSTR _pcwsCharacterName, LmSection _Section )
{
	return InsertLayerSection(_pcwsCharacterName, _Section);
}

INT LmTimeLineManager::RemoveActionSection( LPCWSTR _pcwsCharacterName,  UINT _uStartFrm )
{
	return RemoveLayerSection(_pcwsCharacterName, _uStartFrm);
}

INT LmTimeLineManager::ModifyActionSection( LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uNewStartFrm, UINT _uNewTimeLength)
{
	LayerListItr itrLayerFinder = FindForLayerName(_pcwsCharacterName);
	SectionMapItr itrSecFinder = itrLayerFinder->m_mapSection.find(_uStartFrm);
	LmSection secBk = itrSecFinder->second;

	//기존의 액션을 지운다.
	if(RemoveActionSection(_pcwsCharacterName, _uStartFrm)<0)
		return -1;

	//새로운 Action Section 공간 확보가 실패 했을때 다시 이전의 모습으로 돌아간다.
	if(InsertActionSection(_pcwsCharacterName, _uNewStartFrm, _uNewTimeLength)<0)
	{
		InsertActionSection(_pcwsCharacterName, secBk);
		return -2;
	}

	DrawTimeLineView();
	return 0;
}

INT LmTimeLineManager::InsertCameraSection(UINT _uPosFrm)
{
	return InsertLayerSection(L"Camera Layer", L"", _uPosFrm, 1);
}

INT LmTimeLineManager::InsertCameraSection( LmSection _Section )
{
	return InsertLayerSection(L"Camera Layer",_Section);
}

INT LmTimeLineManager::RemoveCameraSection(UINT _uPosFrm)
{
	return RemoveLayerSection(L"Camera Layer",_uPosFrm);
}

INT LmTimeLineManager::ModifyCameraSection(UINT _uPosFrm , UINT _uNewPosFrm)
{
	LayerListItr itrLayerFinder = FindForLayerName(L"Camera Layer");
	SectionMapItr itrSecFinder = itrLayerFinder->m_mapSection.find(_uPosFrm);
	LmSection secBk = itrSecFinder->second;

	//기존의 카메라 Section을 지운다.
	if(RemoveCameraSection(_uPosFrm)<0)
		return -1;

	//새로운 카메라 Section 공간 확보가 실패 했을때 다시 이전의 모습으로 돌아간다.
	if(InsertCameraSection( _uNewPosFrm)<0)
	{
		InsertCameraSection(secBk);
		return -2;
	}

	DrawTimeLineView();
	return 0;
}

LmUIMotionThread* LmTimeLineManager::GetMotionThread()
{
	return GetTimeLineView()->m_pMotionThread;	
}

SectionVector LmTimeLineManager::GetNowSoundSection( UINT _uFrame, UINT * _uResultCount )
{
	//Sound Name을 Key값으로 모든 사운드 레이어에서 찾는다.
	LayerListItr itrLastSoundlayer = m_listLayer.end();
	LayerListItr itrLayer = m_listLayer.begin();
	LayerListItr itrLayerEnd = m_listLayer.end();

	//반환할 벡터를 생성하고 초기화 한다.
	SectionVector vResult;
	vResult.clear();

	for(; itrLayer != itrLayerEnd ; itrLayer++)
	{
		//사운드 레이어만 검사
		if(itrLayer->m_enumKindofLayer != SOUND_LAYER)
			continue;

		//각 사운드 레이어에 현재 프레임에서 연주(?)를 시작하는 사운드 레이어가 있는지 찾는다.
		SectionMapItr itrSection= itrLayer->m_mapSection.find(_uFrame);

		//현 프레임에서 시작되는 사운드 Section이 있는가?
		if( itrSection != itrLayer->m_mapSection.end() )
		{
			//현 프레임에서 시작되는 사운드 Section이 있으면 vector에 주렁주렁 매달아 놓는다. 
			vResult.push_back(itrSection->second);
		}
	}
	//벡터의 수를 따로 넘겨줌.
	(*_uResultCount) = (INT)vResult.size();
	return vResult;
}

SectionVector LmTimeLineManager::GetOnStartSoundSection( UINT _uFrame, UINT * _uResultCount )
{
	//Sound Name을 Key값으로 모든 사운드 레이어에서 찾는다.
	LayerListItr itrLastSoundlayer = m_listLayer.end();
	LayerListItr itrLayer = m_listLayer.begin();
	LayerListItr itrLayerEnd = m_listLayer.end();

	//반환할 벡터를 생성하고 초기화 한다.
	SectionVector vResult;
	vResult.clear();

	for(; itrLayer != itrLayerEnd ; itrLayer++)
	{
		//사운드 레이어만 검사
		if(itrLayer->m_enumKindofLayer != SOUND_LAYER)
			continue;

		//각 사운드 레이어에 현재 프레임에서 연주(?)를 시작하는 사운드 레이어가 있는지 찾는다.
		//SectionMapItr itrSection= itrLayer->m_mapSection.find(_uFrame);

		INT nResult = GetSectionFirstFrame( itrLayer->m_wstrLayerName.c_str(), _uFrame );

		//현 프레임에 포함된 사운드 Section이 있는가?
		//현 프레임에 포함된 사운드 Section이 있으면 vector에 주렁주렁 매달아 놓는다. 
		if(nResult<0)
			continue;

		SectionMapItr itrSection= itrLayer->m_mapSection.find(nResult);
		vResult.push_back(itrSection->second);
	}
	//벡터의 수를 따로 넘겨줌.
	(*_uResultCount) = (INT)vResult.size();
	return vResult;
}


INT LmTimeLineManager::GetSoundLayerCount()
{
	//Sound Name을 Key값으로 모든 사운드 레이어에서 찾는다.
	LayerListItr itrLastSoundlayer = m_listLayer.end();
	LayerListItr itrLayer = m_listLayer.begin();
	LayerListItr itrLayerEnd = m_listLayer.end();

	INT nResult = 0;
	for(; itrLayer != itrLayerEnd ; itrLayer++)
	{
		//사운드 레이어를 카운트 한다. 
		if(itrLayer->m_enumKindofLayer == SOUND_LAYER)
			nResult++;
	}
	//벡터의 수를 따로 넘겨줌.
	return nResult;
}

INT LmTimeLineManager::InsertFireWorksSection(UINT _uPosFrm)
{
	return InsertLayerSection(L"Fireworks Layer", L"", _uPosFrm, m_uFireWorksLength);
}

INT LmTimeLineManager::InsertFireWorksSection( LmSection _Section )
{
	return InsertLayerSection(L"Fireworks Layer",_Section);
}
INT LmTimeLineManager::RemoveFireWorksSection(UINT _uPosFrm)
{
	return RemoveLayerSection(L"Fireworks Layer",_uPosFrm);
}

INT LmTimeLineManager::ModifyFireWorksSection(UINT _uPosFrm , UINT _uNewPosFrm)
{
	LayerListItr itrLayerFinder = FindForLayerName(L"Fireworks Layer");
	SectionMapItr itrSecFinder = itrLayerFinder->m_mapSection.find(_uPosFrm);
	LmSection secBk = itrSecFinder->second;

	//기존의 불꽃놀이 Section을 지운다.
	if(RemoveFireWorksSection(_uPosFrm)<0)
		return -1;

	//새로운 불꽃놀이 Section 공간 확보가 실패 했을때 다시 이전의 모습으로 돌아간다.
	if(InsertFireWorksSection( _uNewPosFrm)<0)
	{
		InsertFireWorksSection(secBk);
		return -2;
	}

	DrawTimeLineView();
	return 0;
}


INT LmTimeLineManager::InsertSnowSection( LPCWSTR _pcwsSnowName,  UINT _uStartFrm, UINT _uTimeLength )
{
	return InsertLayerSection(L"Snow Layer", _pcwsSnowName, _uStartFrm, _uTimeLength);
}

INT LmTimeLineManager::InsertSnowSection( LmSection _Section )
{
	return InsertLayerSection(L"Snow Layer", _Section);
}

INT LmTimeLineManager::RemoveSnowSection( UINT _uStartFrm )
{
	return RemoveLayerSection(L"Snow Layer", _uStartFrm);
}

INT LmTimeLineManager::ModifySnowSection(UINT _uStartFrm, LPCWSTR _pcwsNewSnowName, UINT _uNewStartFrm, UINT _uNewTimeLength)
{
	LayerListItr itrLayerFinder = FindForLayerName(L"Snow Layer");
	SectionMapItr itrSecFinder = itrLayerFinder->m_mapSection.find(_uStartFrm);
	LmSection secBk = itrSecFinder->second;

	//기존의 눈내리기 효과를 지운다.
	if(RemoveSnowSection(_uStartFrm)<0)
		return -1;

	//새로운 말풍선의 Section 공간 확보가 실패 했을때 다시 이전의 모습으로 돌아간다.
	if(InsertSnowSection(_pcwsNewSnowName, _uNewStartFrm, _uNewTimeLength)<0)
	{
		InsertSnowSection(secBk);
		return -2;
	}

	DrawTimeLineView();
	return 0;
}

INT LmTimeLineManager::CheckLayerSectionArea( LPCWSTR _pcwsLayerName, UINT _uStartFrm, UINT _uEndFrm )
{
	LayerListItr itrLayerFinder = FindForLayerName(_pcwsLayerName);
	
	if(itrLayerFinder == m_listLayer.end())
		//레이어가 아예 존재 하지도 않을때
		return -2;

	//타임라인 길이를 넘어가면 -3 리턴
	if(_uEndFrm > (UINT)m_nFrameLength)
		return -3;

	return itrLayerFinder->CheckSectionArea(_uStartFrm, _uEndFrm);
}

INT LmTimeLineManager::CheckTalkBoxSectionArea( LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uEndFrm )
{
	wstring wstrTalkBoxName = GetCharacterTalkBoxName(_pcwsCharacterName);
	return CheckLayerSectionArea( wstrTalkBoxName.c_str(), _uStartFrm, _uEndFrm );
}

INT LmTimeLineManager::CheckTalkBoxSectionArea( LPCWSTR _pcwsCharacterName, UINT _uOldStartFrm, UINT _uStartFrm, UINT _uEndFrm )
{
	wstring wstrTalkBoxName = GetCharacterTalkBoxName(_pcwsCharacterName);
	LayerListItr itrLayerFinder = FindForLayerName(wstrTalkBoxName.c_str());
	SectionMapItr itrSecFinder = itrLayerFinder->m_mapSection.find(_uOldStartFrm);

	//기존의 말풍선 백업
	LmSection secBk = itrSecFinder->second;

	//기존의 말풍선을 지운다. - 이렇게 지우는 이유는 수정할 때 수정을 해야할 Section과 겹쳐지면 기존의 함수로 검사했을때
	//							반환값이 안된다고 리턴되기 때문에 수정해야할 Section을 지우고 검사한다.
	if(RemoveTalkBoxSection(_pcwsCharacterName, _uOldStartFrm)<0)
		return -1;

	//말풍선을 대입할 자리가 있는가?
	INT nResult = CheckLayerSectionArea( wstrTalkBoxName.c_str(), _uStartFrm, _uEndFrm );

	//다시 지운 말풍선을 대입한다.
	InsertTalkBoxSection(_pcwsCharacterName, secBk);

	return nResult;
}
INT LmTimeLineManager::CheckSnowSectionArea( UINT _uStartFrm, UINT _uEndFrm )
{
	return CheckLayerSectionArea( L"Snow Layer", _uStartFrm, _uEndFrm );
}

INT LmTimeLineManager::CheckSnowSectionArea( UINT _uOldStartFrm, UINT _uStartFrm, UINT _uEndFrm )
{
	LayerListItr itrLayerFinder = FindForLayerName(L"Snow Layer");
	SectionMapItr itrSecFinder = itrLayerFinder->m_mapSection.find(_uOldStartFrm);
	
	//기존의 눈 내리기 효과 백업
	LmSection secBk = itrSecFinder->second;
	
	//기존의 눈 내리기 효과를 지운다. - 이렇게 지우는 이유는 수정할 때 수정을 해야할 Section과 겹쳐지면 기존의 함수로 검사했을때
	//									반환값이 안된다고 리턴되기 때문에 수정해야할 Section을 지우고 검사한다.
	if(RemoveSnowSection(_uOldStartFrm)<0)
		return -1;

	//눈 내리기 효과를 대입할 자리가 있는가?
	INT nResult = CheckLayerSectionArea( L"Snow Layer", _uStartFrm, _uEndFrm );
	
	InsertSnowSection(secBk);

	return nResult;		
}

INT LmTimeLineManager::GetSectionFirstFrame( LPCWSTR _pcwsLayerName, UINT _uFrame )
{
	LayerListItr itrLayerFinder = FindForLayerName(_pcwsLayerName);
	SectionMapItr itrSecFinder = itrLayerFinder->m_mapSection.begin();
	SectionMapItr itrSecEnd = itrLayerFinder->m_mapSection.end();
	
	for( ;itrSecFinder != itrSecEnd ; itrSecFinder++)
	{
		UINT uSecStart = itrSecFinder->second.m_uStartFrm;
		UINT uSecEnd = itrSecFinder->second.m_uEndFrm;

		if(  (uSecStart <= _uFrame) && (uSecEnd >= _uFrame)  )
			return uSecStart;
	}

	//못찾으면 -1을 리턴
	return -1;
}

INT LmTimeLineManager::PushActionSection( LPCWSTR _pcwsCharacterName, UINT _uTimeLength )
{
	INT nStartFrm = GetSPFrame(_pcwsCharacterName);
	//잘못될 이유가 없지..
	return InsertActionSection(_pcwsCharacterName, nStartFrm, _uTimeLength);
}

//마지막 Section에 1을 더하면 Stack Pointer가 된다. 
INT LmTimeLineManager::GetSPFrame( LPCWSTR _pcwsLayerName )
{
	LayerListItr itrLayerFinder = FindForLayerName(_pcwsLayerName);
	SectionMapItr itrSection = itrLayerFinder->m_mapSection.end();

	INT nSize = (INT)itrLayerFinder->m_mapSection.size();

	//section이 하나도 없는 경우 0을 리턴한다.
	if(!nSize)
		return 1;

	itrSection--;

	return (itrSection->second.m_uEndFrm) + 1;

}

INT LmTimeLineManager::PopActionSection( LPCWSTR _pcwsCharacterName , LmSection * _pSection)
{
	LayerListItr itrLayerFinder = FindForLayerName(_pcwsCharacterName);
	SectionMapItr itrSection = itrLayerFinder->m_mapSection.end();

	INT nSize = (INT)itrLayerFinder->m_mapSection.size();

	if(!nSize)
		return -1;

	itrSection--;
	
	(*_pSection) = itrSection->second;

	RemoveActionSection(_pcwsCharacterName, itrSection->second.m_uStartFrm);

	return 0;
}

LmUIMainToolDlg* LmTimeLineManager::GetMainToolDlg()
{
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
	LmUIMainToolFormView* pMainToolFormView = (LmUIMainToolFormView*) ( pMainFrame->m_wndSplitUpCol.GetPane(0, 1) );
	LmUIMainToolDlg* pMainToolDlg  =  &(pMainToolFormView->m_MainToolDlg);

	return pMainToolDlg;
}

VOID LmTimeLineManager::RefreshUIMainToolList()
{
	LmUIMainToolDlg	*pMainToolDlg  =GetMainToolDlg();	

	//모든 메인 툴의 리스트들을 초기화!
	pMainToolDlg->InitToolList();

	//눈내리기 레이어의 모든 Section에 있는 내용을 눈내리기 효과 리스트에 담아준다. 
	LayerListItr itrLayerFinder = FindForLayerName(L"Snow Layer");
	SectionMapItr itrSection = itrLayerFinder->m_mapSection.begin();
	SectionMapItr itrSectionEnd = itrLayerFinder->m_mapSection.end();

	for(; itrSection != itrSectionEnd ; itrSection++)
	{
		pMainToolDlg->InsertSnow(itrSection->second.m_uStartFrm,
			itrSection->second.m_uEndFrm,
			itrSection->second.m_wstrSectionName.c_str());
	}

	//사운드 레이어를 모두 순회 하면서 모든 Section에 있는 내용을 리스트에 담아준다.
	//Sound Name을 Key값으로 모든 사운드 레이어에서 찾는다.
	LayerListItr itrLayer = m_listLayer.begin();
	LayerListItr itrLayerEnd = m_listLayer.end();

	for(; itrLayer != itrLayerEnd ; itrLayer++)
	{
		//사운드 레이어만 검사
		if(itrLayer->m_enumKindofLayer != SOUND_LAYER)
			continue;

		SectionMapItr itrSection = itrLayer->m_mapSection.begin();
		SectionMapItr itrSectionEnd = itrLayer->m_mapSection.end();

		for(; itrSection != itrSectionEnd ; itrSection++)
		{
			pMainToolDlg->InsertSound(itrSection->second.m_wstrSectionName.c_str(),
				itrSection->second.m_uStartFrm);
		}

	}

	//말풍선 레이어를 모두 순회하면서 모든 Section에 있는 내용을 리스트에 담아준다. 
	itrLayer = m_listLayer.begin();
	itrLayerEnd = m_listLayer.end();

	for(; itrLayer != itrLayerEnd ; itrLayer++)
	{
		//말풍선 레이어만 검사
		if(itrLayer->m_enumKindofLayer != TALKBOX_LAYER)
			continue;

		SectionMapItr itrSection = itrLayer->m_mapSection.begin();
		SectionMapItr itrSectionEnd = itrLayer->m_mapSection.end();

		for(; itrSection != itrSectionEnd ; itrSection++)
		{
			CString strNameTemp;
			CString strCharacterName;
			strNameTemp.Format(L"%s",itrLayer->m_wstrLayerName.c_str());
			INT nTalkBoxLayerName = strNameTemp.GetLength();
			strCharacterName = strNameTemp.Left( nTalkBoxLayerName - 8 );

			pMainToolDlg->InsertTalkBox(strCharacterName.GetBuffer(0),
				itrSection->second.m_uStartFrm,
				itrSection->second.m_uEndFrm,
				itrSection->second.m_wstrSectionName.c_str(),
				L"말풍선1");  // [말풍선] 여기에서 내부데이터를 읽어와야한다.
		}
	}
}

VOID LmTimeLineManager::ReloadSoundSection( LPCWSTR _pcwsSoundName, UINT _uStartFrm, UINT _uEndFrm, DWORD _dwFadeIn, DWORD _dwFadeOut )
{
	InsertSoundSectionProcess(_pcwsSoundName, _uStartFrm, _uEndFrm, _dwFadeIn, _dwFadeOut);
	return;

}