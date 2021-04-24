#include "StdAfx.h"
#include "LmTimeLineManager.h"
#include "../3CMaker/resource.h"
#include "../3CMaker/LmUIMainToolFormView.h"
#include "../3CMaker/LmUIMainToolDlg.h"
#include "LmSceneManager.h"
#include "LmScene.h"


//////////////////////////////////////////////////////////////////////////
//DEBUG�� �Լ�
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
	m_nFrmPerSec = 20;			//�ʴ� ������ �� �⺻ 20 Frame!
	m_nCurCaret = 0;			//���� Ŀ���� ��ġ - 1���� ����!!
	m_nFrameLength = 1000;		//������ ���� -- Scene�� ���� �ٸ�
	m_uFireWorksLength = 40;	//�Ҳɳ��� ���� 40 Frame
}

LmTimeLineManager::~LmTimeLineManager(VOID)
{
}

wstring LmTimeLineManager::AddLayer( LPCWSTR _pcwsLayerName, LmKIND_OF_LAYER _enumKindofLayer )
{
	LmLayer NewLayer;

	////Layer �ʱ�ȭ
	NewLayer.m_enumKindofLayer = _enumKindofLayer;
	NewLayer.m_wstrLayerName = GetNewLayerName(_pcwsLayerName);
	
	m_listLayer.push_back(NewLayer);

	DrawTimeLineView();

	return NewLayer.m_wstrLayerName;
}

wstring LmTimeLineManager::InsertLayer( LayerListItr _itrLayer,LPCWSTR _pcwsLayerName, LmKIND_OF_LAYER _enumKindofLayer )
{

	LmLayer NewLayer;

	//insert�� iterator�� ����Ű�� element�� �ٷ� �տ� �����ϹǷ� 
	//��ĭ �ڷ� ������.
	_itrLayer++;

	//Layer �ʱ�ȭ
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

		//ã������ ����

		if( wstrListLayerName == wstrInputLayerName )
			return itrLayerFinder;
	}

	//��ã������ itrLayerEnd�� ����
	return itrLayerFinder;

}


wstring LmTimeLineManager::GetNewLayerName( LPCWSTR _pcwsLayerName )
{
	INT nCnt=0;
	WCHAR wcsKeyNameBuffer[512];
	LayerListItr itrFindKey;

	//�Է¹޴� Ű ���� ������ Ȯ���Ѵ�. 
	//���� Ű���� ������
	//���̾� �̸� �������� ��ȣ�� ���δ�.
	
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

	//Ű ���� ã�Ҵ°�?
	//ã������ ������.
	if(itrFindKey != m_listLayer.end() )
	{
		m_listLayer.erase(itrFindKey);
		
		DrawTimeLineView();

		return 0;
	}
	return -1; //���� ���� - ��ã����.

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
	//����� �⺻������ ���� �̸��� ���� �������� �ִ�. 
	//�߿��� ���� �������� �˰����� �����ϴ� ���̴�.
	//�켱 ���� �����ӿ��� �� �������� �ҿ�Ǵ����� �˾ƾ� �Ѵ�. 
	
	//�Է��� �ϴٰ� �ִ� �������� �ʰ� �ϴ� ��� �׸�ŭ Ȯ������ �ʰ� ������ ���� ������ �д�.
	//Ÿ�Ӷ��κ信�� �׸��� �ڵ����� �߶��ֱ� ������.

	//���尡 �� ���������� �˾Ƴ���.
	UINT nSoundFrmLength = ChangeMsecToFrame(_dwSoundLength);
	UINT uEndFrm = _uStartFrm + nSoundFrmLength;

	InsertSoundSectionProcess(_pcwsSoundName, _uStartFrm, uEndFrm, _dwFadeIn, _dwFadeOut);

	// ���� ���õ� ���� ������ ����
	LmScene* pNowScene = TheSceneManager->GetNowScene();
	if(pNowScene == NULL)
		return;

	pNowScene->InsertSoundSection(_pcwsSoundName, _uStartFrm, uEndFrm, _dwFadeIn, _dwFadeOut);

	DrawTimeLineView();
	return;
}

VOID LmTimeLineManager::InsertSoundSectionProcess( LPCWSTR _pcwsSoundName, UINT _uStartFrm, UINT _uEndFrm, DWORD _dwFadeIn, DWORD _dwFadeOut )
{
	//����� �⺻������ ���� �̸��� ���� �������� �ִ�. 
	//�߿��� ���� �������� �˰����� �����ϴ� ���̴�.
	//�켱 ���� �����ӿ��� �� �������� �ҿ�Ǵ����� �˾ƾ� �Ѵ�. 

	//�Է��� �ϴٰ� �ִ� �������� �ʰ� �ϴ� ��� �׸�ŭ Ȯ������ �ʰ� ������ ���� ������ �д�.
	//Ÿ�Ӷ��κ信�� �׸��� �ڵ����� �߶��ֱ� ������.

	//���尡 �� ���������� �˾Ƴ���.

	LayerListItr itrLastSoundlayer = m_listLayer.end();
	LayerListItr itrLayer = m_listLayer.begin();
	LayerListItr itrLayerEnd = m_listLayer.end();

	BOOL isInserted = FALSE;
	for (; itrLayer != itrLayerEnd ; itrLayer++)
	{
		//���� ���̾� ���� Ȯ��
		if(itrLayer->GetKindOfLayer() != SOUND_LAYER)
			continue;

		//Sound Section�� Insert�� ����.
		INT nInsertResult = itrLayer->InsertSection(_pcwsSoundName, _uStartFrm, _uEndFrm, _dwFadeIn, _dwFadeOut);
		if(!nInsertResult)
		{
			isInserted = TRUE;
			break;
		}

		//IsThereArea==0 �� ��쿡�� ���� ���� ���̾ ã�´�.
		//������ ���� ���̾ ã�� ����
		itrLastSoundlayer = itrLayer;
	}

	//����� ������ ���� ���� ���̾ ���ٸ�, ���� ���̾ �ϳ� ����� Sound Section�� �ϳ� �ִ´�.
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
	//�и��� ������ �ð��� �ʷ� ��ȯ
	UINT nFrameSec = _dwSoundLength /1000;

	//�ʸ� �ʴ� ������ ���� ������.
	UINT nFramelength = nFrameSec * m_nFrmPerSec;

	//�������� ������ 1�� ���������ش�.
	if(nFrameSec  % m_nFrmPerSec)
		nFramelength++;

	return nFramelength;
}

INT LmTimeLineManager::RemoveSoundSection(LPCWSTR _pcwsSoundName)
{
	//Sound Name�� Key������ ��� ���� ���̾�� ã�´�.
	LayerListItr itrLastSoundlayer = m_listLayer.end();
	LayerListItr itrLayer = m_listLayer.begin();
	LayerListItr itrLayerEnd = m_listLayer.end();

	wstring wsSoundName = _pcwsSoundName;

	for(; itrLayer != itrLayerEnd ; itrLayer++)
	{
		//���� ���̾ �˻�
		if(itrLayer->m_enumKindofLayer != SOUND_LAYER)
			continue;

		//�� ���� ���̾�� �̸����� Remove�� �õ��ϰ� Remove�� ���������� ����������.
		INT nRemoveResult = itrLayer->RemoveSection(_pcwsSoundName);
		
		if(!nRemoveResult)
		{
			//���� ���巹�̾ ���尡 ����, "Sound Layer"�� �ƴϸ� �� ���̾ ����������. 
			INT nSoundSectionCount = (INT)itrLayer->m_mapSection.size();
			if( (!nSoundSectionCount) && 
				 (itrLayer->m_wstrLayerName != L"Sound Layer"))
				RemoveLayer(itrLayer->m_wstrLayerName.c_str());
			DrawTimeLineView();

			// ���� ���õ� ���� ������ -1�� ����
			LmScene* pNowScene = TheSceneManager->GetNowScene();
			if(pNowScene == NULL)
				return -1;

			pNowScene->RemoveSoundSection(_pcwsSoundName);

			return 0;
		}
	}

	//������ -1�� ��ȯ�Ѵ�.
	DrawTimeLineView();
	return -1;
}

INT LmTimeLineManager::ModifySoundSection( LPCWSTR _pcwsSoundName, LPCWSTR _pcwsNewSoundName, UINT _uStartFrm, DWORD _dwSoundLength, DWORD _dwFadeIn, DWORD _dwFadeOut )
{
	//������ ��ϵ� ���带 ����� �ٽ� ����Ѵ�.
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

	//��� ���̾ �ִ� Secton���� ���̸� �ٽ� ���� �� ��� �Ѵ�. 
	//�׷��� ��� ���� �ʴ� ������ ���� 10 Frame �̴�. 

}

VOID LmTimeLineManager::InitTimeLIne()
{
	////////////////////////////////////////////////////////////////////////////
	//// Ÿ�Ӷ��� �ʱ�ȭ
	//// ���⿡�ٰ� TheCartoonMaker���� ��� Things�� ������ �޾ƾ� ���ٵ�..
	// [����] ���⿡ �ʿ��� Layer �� Section�� �־�� ��.
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

		//ã������ ����
		if(wcsstr(wstrListLayerName.c_str(), wstrInputLayerName.c_str()) 
			&& (itrLayerFinder->m_enumKindofLayer == _KindofLayer) )
			return itrLayerFinder;
	}

	//��ã������ itrLayerEnd�� ����
	return itrLayerFinder;
}

INT LmTimeLineManager::RemoveCharacterLayer( LPCWSTR _pcwsCharacterName )
{
	//ĳ���� ���̾ �����.
	RemoveLayer(_pcwsCharacterName);
	wstring wstrTalkboxName = GetCharacterTalkBoxName( _pcwsCharacterName );

	//���� TalkBox�� ��á���� ��� Character �̸��� �����ϰ� �ִ� TalkBox�� �����Ѵ�.
	if(RemoveLayer(wstrTalkboxName.c_str())<0) 
	{
		LayerListItr itrFinder = FindForLayerNameStrStr( wstrTalkboxName.c_str(), TALKBOX_LAYER);
		if(itrFinder != m_listLayer.end())
			RemoveLayer(itrFinder->m_wstrLayerName.c_str());
		else
			//�׷��� TalkBox�� ã�� ������ ��� -1����
			return -1;
	}
	//DrawTimeLineView();
	return 0;
}

wstring LmTimeLineManager::GetCharacterTalkBoxName( LPCWSTR _pcwsCharacterName )
{
	//ĳ���� �̸��� �Է��ϸ� �ش� ĳ���Ͱ� ������ TalkBox Layer�� ��ȯ�Ѵ�.
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

	//������ ��ǳ���� �����.
	if(RemoveTalkBoxSection(_pcwsCharacterName, _uStartFrm)<0)
		return -1;

	//���ο� ��ǳ���� Section ���� Ȯ���� ���� ������ �ٽ� ������ ������� ���ư���.
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

	//Layer�� ��ã���� -1 ����!
	if(itrFinder==m_listLayer.end())
		return -1;

	INT nEndFrm = _uStartFrm + _uTimeLength - 1;
	
	//Ÿ�Ӷ��� ���̸� �Ѿ�� -3 ����
	if(nEndFrm > m_nFrameLength)
		return -3;

	INT nResult = itrFinder->InsertSection(_pcwsSectionName, _uStartFrm, nEndFrm);

	//Section�� ���� ������ ���� ��� -1�� �����Ѵ�. 
	if(nResult < 0)
		return -2;

	DrawTimeLineView();
	return 0;
}

INT LmTimeLineManager::InsertLayerSection( LPCWSTR _pcwsLayerName, LmSection _Section )
{
	LayerListItr itrFinder = FindForLayerName(_pcwsLayerName);

	//Layer�� ��ã���� -1 ����!
	if(itrFinder==m_listLayer.end())
		return -1;

	//Ÿ�Ӷ��� ���̸� �Ѿ�� -3 ����
	if(_Section.m_uEndFrm > (UINT)m_nFrameLength)
		return -3;


	INT nResult = itrFinder->InsertSection(_Section);

	//Section�� ���� ������ ���� ��� -1�� �����Ѵ�. 
	if(nResult < 0)
		return -2;

	DrawTimeLineView();
	return 0;
}

INT LmTimeLineManager::InsertLayerSection( LPCWSTR _pcwsLayerName, UINT _uStartFrm, UINT _uTimeLength )
{
	LayerListItr itrFinder = FindForLayerName(_pcwsLayerName);

	//Layer�� ��ã���� -1 ����!
	if(itrFinder==m_listLayer.end())
		return -1;

	INT nEndFrm = _uStartFrm + _uTimeLength - 1;

	//Ÿ�Ӷ��� ���̸� �Ѿ�� -3 ����
	if(nEndFrm > m_nFrameLength)
		return -3;

	INT nResult = itrFinder->InsertSection(L"", _uStartFrm, nEndFrm);

	//Section�� ���� ������ ���� ��� -1�� �����Ѵ�. 
	if(nResult < 0)
		return -2;

	DrawTimeLineView();
	return 0;
}

INT LmTimeLineManager::RemoveLayerSection( LPCWSTR _pcwsLayerName, LPCWSTR _pcwsSectionName )
{
	LayerListItr itrFinder = FindForLayerName(_pcwsLayerName);

	//TalkBox�� ��ã���� -1 ����!
	if(itrFinder==m_listLayer.end())
		return -1;

	INT nResult = itrFinder->RemoveSection(_pcwsSectionName);

	//��ǳ�� �̸��� ã�� ���� �������� ������ ��� -1�� �����Ѵ�.
	if(nResult < 0)
		return -1;

	DrawTimeLineView();
	return 0;
}

INT LmTimeLineManager::RemoveLayerSection( LPCWSTR _pcwsLayerName, UINT _uStartFrm )
{
	LayerListItr itrFinder = FindForLayerName(_pcwsLayerName);

	//���̾ ��ã���� -1 ����!
	if(itrFinder==m_listLayer.end())
		return -1;

	INT nKeyFrame = GetSectionFirstFrame(_pcwsLayerName, _uStartFrm);
	INT nResult = itrFinder->RemoveSection(nKeyFrame);

	// Section�� ã�� ���� �������� ������ ��� -1�� �����Ѵ�.
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

	//������ �׼��� �����.
	if(RemoveActionSection(_pcwsCharacterName, _uStartFrm)<0)
		return -1;

	//���ο� Action Section ���� Ȯ���� ���� ������ �ٽ� ������ ������� ���ư���.
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

	//������ ī�޶� Section�� �����.
	if(RemoveCameraSection(_uPosFrm)<0)
		return -1;

	//���ο� ī�޶� Section ���� Ȯ���� ���� ������ �ٽ� ������ ������� ���ư���.
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
	//Sound Name�� Key������ ��� ���� ���̾�� ã�´�.
	LayerListItr itrLastSoundlayer = m_listLayer.end();
	LayerListItr itrLayer = m_listLayer.begin();
	LayerListItr itrLayerEnd = m_listLayer.end();

	//��ȯ�� ���͸� �����ϰ� �ʱ�ȭ �Ѵ�.
	SectionVector vResult;
	vResult.clear();

	for(; itrLayer != itrLayerEnd ; itrLayer++)
	{
		//���� ���̾ �˻�
		if(itrLayer->m_enumKindofLayer != SOUND_LAYER)
			continue;

		//�� ���� ���̾ ���� �����ӿ��� ����(?)�� �����ϴ� ���� ���̾ �ִ��� ã�´�.
		SectionMapItr itrSection= itrLayer->m_mapSection.find(_uFrame);

		//�� �����ӿ��� ���۵Ǵ� ���� Section�� �ִ°�?
		if( itrSection != itrLayer->m_mapSection.end() )
		{
			//�� �����ӿ��� ���۵Ǵ� ���� Section�� ������ vector�� �ַ��ַ� �Ŵ޾� ���´�. 
			vResult.push_back(itrSection->second);
		}
	}
	//������ ���� ���� �Ѱ���.
	(*_uResultCount) = (INT)vResult.size();
	return vResult;
}

SectionVector LmTimeLineManager::GetOnStartSoundSection( UINT _uFrame, UINT * _uResultCount )
{
	//Sound Name�� Key������ ��� ���� ���̾�� ã�´�.
	LayerListItr itrLastSoundlayer = m_listLayer.end();
	LayerListItr itrLayer = m_listLayer.begin();
	LayerListItr itrLayerEnd = m_listLayer.end();

	//��ȯ�� ���͸� �����ϰ� �ʱ�ȭ �Ѵ�.
	SectionVector vResult;
	vResult.clear();

	for(; itrLayer != itrLayerEnd ; itrLayer++)
	{
		//���� ���̾ �˻�
		if(itrLayer->m_enumKindofLayer != SOUND_LAYER)
			continue;

		//�� ���� ���̾ ���� �����ӿ��� ����(?)�� �����ϴ� ���� ���̾ �ִ��� ã�´�.
		//SectionMapItr itrSection= itrLayer->m_mapSection.find(_uFrame);

		INT nResult = GetSectionFirstFrame( itrLayer->m_wstrLayerName.c_str(), _uFrame );

		//�� �����ӿ� ���Ե� ���� Section�� �ִ°�?
		//�� �����ӿ� ���Ե� ���� Section�� ������ vector�� �ַ��ַ� �Ŵ޾� ���´�. 
		if(nResult<0)
			continue;

		SectionMapItr itrSection= itrLayer->m_mapSection.find(nResult);
		vResult.push_back(itrSection->second);
	}
	//������ ���� ���� �Ѱ���.
	(*_uResultCount) = (INT)vResult.size();
	return vResult;
}


INT LmTimeLineManager::GetSoundLayerCount()
{
	//Sound Name�� Key������ ��� ���� ���̾�� ã�´�.
	LayerListItr itrLastSoundlayer = m_listLayer.end();
	LayerListItr itrLayer = m_listLayer.begin();
	LayerListItr itrLayerEnd = m_listLayer.end();

	INT nResult = 0;
	for(; itrLayer != itrLayerEnd ; itrLayer++)
	{
		//���� ���̾ ī��Ʈ �Ѵ�. 
		if(itrLayer->m_enumKindofLayer == SOUND_LAYER)
			nResult++;
	}
	//������ ���� ���� �Ѱ���.
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

	//������ �Ҳɳ��� Section�� �����.
	if(RemoveFireWorksSection(_uPosFrm)<0)
		return -1;

	//���ο� �Ҳɳ��� Section ���� Ȯ���� ���� ������ �ٽ� ������ ������� ���ư���.
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

	//������ �������� ȿ���� �����.
	if(RemoveSnowSection(_uStartFrm)<0)
		return -1;

	//���ο� ��ǳ���� Section ���� Ȯ���� ���� ������ �ٽ� ������ ������� ���ư���.
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
		//���̾ �ƿ� ���� ������ ������
		return -2;

	//Ÿ�Ӷ��� ���̸� �Ѿ�� -3 ����
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

	//������ ��ǳ�� ���
	LmSection secBk = itrSecFinder->second;

	//������ ��ǳ���� �����. - �̷��� ����� ������ ������ �� ������ �ؾ��� Section�� �������� ������ �Լ��� �˻�������
	//							��ȯ���� �ȵȴٰ� ���ϵǱ� ������ �����ؾ��� Section�� ����� �˻��Ѵ�.
	if(RemoveTalkBoxSection(_pcwsCharacterName, _uOldStartFrm)<0)
		return -1;

	//��ǳ���� ������ �ڸ��� �ִ°�?
	INT nResult = CheckLayerSectionArea( wstrTalkBoxName.c_str(), _uStartFrm, _uEndFrm );

	//�ٽ� ���� ��ǳ���� �����Ѵ�.
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
	
	//������ �� ������ ȿ�� ���
	LmSection secBk = itrSecFinder->second;
	
	//������ �� ������ ȿ���� �����. - �̷��� ����� ������ ������ �� ������ �ؾ��� Section�� �������� ������ �Լ��� �˻�������
	//									��ȯ���� �ȵȴٰ� ���ϵǱ� ������ �����ؾ��� Section�� ����� �˻��Ѵ�.
	if(RemoveSnowSection(_uOldStartFrm)<0)
		return -1;

	//�� ������ ȿ���� ������ �ڸ��� �ִ°�?
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

	//��ã���� -1�� ����
	return -1;
}

INT LmTimeLineManager::PushActionSection( LPCWSTR _pcwsCharacterName, UINT _uTimeLength )
{
	INT nStartFrm = GetSPFrame(_pcwsCharacterName);
	//�߸��� ������ ����..
	return InsertActionSection(_pcwsCharacterName, nStartFrm, _uTimeLength);
}

//������ Section�� 1�� ���ϸ� Stack Pointer�� �ȴ�. 
INT LmTimeLineManager::GetSPFrame( LPCWSTR _pcwsLayerName )
{
	LayerListItr itrLayerFinder = FindForLayerName(_pcwsLayerName);
	SectionMapItr itrSection = itrLayerFinder->m_mapSection.end();

	INT nSize = (INT)itrLayerFinder->m_mapSection.size();

	//section�� �ϳ��� ���� ��� 0�� �����Ѵ�.
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

	//��� ���� ���� ����Ʈ���� �ʱ�ȭ!
	pMainToolDlg->InitToolList();

	//�������� ���̾��� ��� Section�� �ִ� ������ �������� ȿ�� ����Ʈ�� ����ش�. 
	LayerListItr itrLayerFinder = FindForLayerName(L"Snow Layer");
	SectionMapItr itrSection = itrLayerFinder->m_mapSection.begin();
	SectionMapItr itrSectionEnd = itrLayerFinder->m_mapSection.end();

	for(; itrSection != itrSectionEnd ; itrSection++)
	{
		pMainToolDlg->InsertSnow(itrSection->second.m_uStartFrm,
			itrSection->second.m_uEndFrm,
			itrSection->second.m_wstrSectionName.c_str());
	}

	//���� ���̾ ��� ��ȸ �ϸ鼭 ��� Section�� �ִ� ������ ����Ʈ�� ����ش�.
	//Sound Name�� Key������ ��� ���� ���̾�� ã�´�.
	LayerListItr itrLayer = m_listLayer.begin();
	LayerListItr itrLayerEnd = m_listLayer.end();

	for(; itrLayer != itrLayerEnd ; itrLayer++)
	{
		//���� ���̾ �˻�
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

	//��ǳ�� ���̾ ��� ��ȸ�ϸ鼭 ��� Section�� �ִ� ������ ����Ʈ�� ����ش�. 
	itrLayer = m_listLayer.begin();
	itrLayerEnd = m_listLayer.end();

	for(; itrLayer != itrLayerEnd ; itrLayer++)
	{
		//��ǳ�� ���̾ �˻�
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
				L"��ǳ��1");  // [��ǳ��] ���⿡�� ���ε����͸� �о�;��Ѵ�.
		}
	}
}

VOID LmTimeLineManager::ReloadSoundSection( LPCWSTR _pcwsSoundName, UINT _uStartFrm, UINT _uEndFrm, DWORD _dwFadeIn, DWORD _dwFadeOut )
{
	InsertSoundSectionProcess(_pcwsSoundName, _uStartFrm, _uEndFrm, _dwFadeIn, _dwFadeOut);
	return;

}