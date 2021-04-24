//
//
//  @ Project : 3D 카툰 메이커
//  @ File Name : LmSection.h
//  @ Date : 2007-12-13
//  @ Author : 안진왕
//
//

#if !defined(_LMTIMELINEMANAGER_H)
#define _LMTIMELINEMANAGER_H


#include "LmLayer.h"
#include "LmSection.h"
#include "../3CMaker/MainFrm.h"
#include "../3CMaker/LmUITimeLineView.h"


#include<list>
#include<algorithm>

typedef vector<LmSection> SectionVector;
typedef SectionVector::iterator SectionVectorItr;
class LmUIMainToolDlg;
class LmTimeLineManager
{
//멤버 함수
public:
  	static LmTimeLineManager* GetInstance();

	//타임라인뷰 외부 인터페이스. 
	VOID				InitTimeLIne();		//타임라인 초기화 멤버 함수.

	INT					GetListSize();		//Layer List의 크기 리턴
	LayerListItr		GetListBeginItr();	//Layer List의 시작 지점 리턴
	LayerListItr		GetListEndItr();	//Layer List의 끝 지점 리턴

	UINT				GetCurCaret();		//현재 Caret 값을 얻어온다.
	INT					GetFrameLength();	//총 프레임 수를 얻어옴.
	INT					GetFramePerSec();	//초당 프레임 수를 얻어옴.

	BOOL				SetCurCaret(INT _iCurCaret);		//현재 Caret 값 세팅.
	VOID				SetFrameLength(INT _iFrameLength);	//총 프레임 수 세팅.
	VOID				SetFramePerSec(INT _iFrmPerSec);	//초당 프레임 수 세팅.

	//Sound 외부 인터페이스
	VOID			InsertSoundSection(LPCWSTR _pcwsSoundName, UINT _uStartFrm, DWORD _dwSoundLength, DWORD _dwFadeIn, DWORD _dwFadeOut);			//사운드 Section 삽입
	VOID			InsertSoundSectionProcess( LPCWSTR _pcwsSoundName, UINT _uStartFrm, UINT _uEndFrm, DWORD _dwFadeIn, DWORD _dwFadeOut);
	VOID			ReloadSoundSection(LPCWSTR _pcwsSoundName, UINT _uStartFrm, UINT _uEndFrm, DWORD _dwFadeIn, DWORD _dwFadeOut);				//사운드 Section 리로드
	
	INT				RemoveSoundSection(LPCWSTR _pcwsSoundName);													//사운드 Section 삭제
	INT				ModifySoundSection(LPCWSTR _pcwsSoundName, LPCWSTR _pcwsNewSoundName,
										UINT _uStartFrm, DWORD _dwSoundLength, DWORD _dwFadeIn, DWORD _dwFadeOut);									//사운드 Section 수정
	SectionVector	GetNowSoundSection(UINT _uFrame, UINT * _uResultCount);										//현재 프레임에서 재생이 시작되어야 할 사운드 Section을 넘겨 줌.
	SectionVector	GetOnStartSoundSection( UINT _uFrame, UINT * _uResultCount );
	INT				GetSoundLayerCount();																		//사운드 레이어의 갯수를 가르쳐 준다. 


	//Character 외부 인터페이스
	wstring			InsertCharacterLayer(LPCWSTR _pcwsCharacterName);								//캐릭터 입력
	INT				RemoveCharacterLayer(LPCWSTR _pcwsCharacterName);								//캐릭터 삭제
	INT				ModifyCharacterLayerName(LPCWSTR _pcwsLayerName, LPCWSTR _pcwsNewLayerName);	//캐릭터 이름 변경
				
	//Character TalkBox 외부 인터페이스
	INT				InsertTalkBoxSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uTimeLength);								//말풍선 Section 입력
	INT				InsertTalkBoxSection(LPCWSTR _pcwsCharacterName, LmSection _Section);												//말풍선 Section 입력
	INT				RemoveTalkBoxSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm);													//말풍선 Section 삭제
	INT				ModifyTalkBoxSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm,  UINT _uNewStartFrm, UINT _uNewTimeLength);		//말풍선 Section 수정
	INT				CheckTalkBoxSectionArea(LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uEndFrm);								//말풍선이 들어갈 수 있는지 확인
	INT				CheckTalkBoxSectionArea(LPCWSTR _pcwsCharacterName, UINT _uOldStartFrm, UINT _uStartFrm, UINT _uEndFrm);			//수정하기에서 말풍선이 들어갈 수 있는지 확인

	//Character Action 외부 인터페이스
	INT				InsertActionSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uTimeLength);								//캐릭터 Action Section 입력
	INT				InsertActionSection(LPCWSTR _pcwsCharacterName, LmSection _Section);												//캐릭터 Action Section 입력
	INT				RemoveActionSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm);													//캐릭터 Action Section 삭제
	INT				ModifyActionSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uNewStartFrm, UINT _uNewTimeLength); 		//캐릭터 Action Section 수정
	
	INT				PushActionSection(LPCWSTR _pcwsCharacterName, UINT _uTimeLength);		//캐릭터 액션을 Push 한다.
	INT				PopActionSection(LPCWSTR _pcwsCharacterName, LmSection * _pSection);	//캐릭터 액션을 Pop 한다.
										
	//Camera Section 외부 인터페이스
	INT				InsertCameraSection(UINT _uPosFrm);						//카메라 Section 삽입
	INT				InsertCameraSection(LmSection _Section);				//카메라 Section 삽입
	INT				RemoveCameraSection(UINT _uPosFrm);						//카메라 Section 삭제
	INT				ModifyCameraSection(UINT _uPosFrm, UINT _uNewPosFrm);	//카메라 Section 수정

	//FireWorks Section 외부 인터페이스
	INT				InsertFireWorksSection(UINT _uPosFrm);						//불꽃놀이 Section 삽입
	INT				InsertFireWorksSection(LmSection _Section);					//불꽃놀이 Section 삽입
	INT				RemoveFireWorksSection(UINT _uPosFrm);						//불꽃놀이 Section 삭제
	INT				ModifyFireWorksSection(UINT _uPosFrm, UINT _uNewPosFrm);	//불꽃놀이 Section 수정

	//눈내리기 Snow 외부 인터페이스
	INT				InsertSnowSection(LPCWSTR _pcwsSnowName, UINT _uStartFrm, UINT _uTimeLength);								//눈내리기 Section 입력
	INT				InsertSnowSection(LmSection _Section);																		//눈내리기 Section 입력
	INT				RemoveSnowSection(UINT _uStartFrm);																			//눈내리기 Section 삭제
	INT				ModifySnowSection(UINT _uStartFrm, LPCWSTR _pcwsNewSnowName, UINT _uNewStartFrm, UINT _uNewTimeLength); 	//눈내리기 Section 수정
	INT				CheckSnowSectionArea(UINT _uStartFrm, UINT _uEndFrm);														//눈내리기 Section을 입력할 수 있는지 확인
	INT				CheckSnowSectionArea(UINT _uOldStartFrm, UINT _uStartFrm, UINT _uEndFrm);									//수정하기 에서 눈내리기 Section을 입력할 수 있는지 확인

	//공개된 공용함수
	INT			GetSectionFirstFrame(LPCWSTR _pcwsLayerName,  UINT _uFrame);  //레이어 이름에 Section을 넣을 수 있는지 확인
	INT			GetSPFrame(LPCWSTR _pcwsLayerName);

	//UI MainTool List 초기화 및 리로드
	VOID		RefreshUIMainToolList();

	LmUIMotionThread* GetMotionThread();


//멤버 변수
public:
	LayerList				m_listLayer;			//레이어 리스트
	INT						m_nFrmPerSec;			//초당 프레임 수
	UINT					m_nCurCaret;			//현재 커서의 위치 - 1부터 시작!!
	INT						m_nFrameLength;			//프레임 길이 -- Scene에 따라 다름
	UINT					m_uFireWorksLength;		//불꽃놀이 길이.

	//내부 멤버 함수
private:
	LmTimeLineManager(VOID);
	~LmTimeLineManager(VOID);

	//Layer 관련 내부 함수
	wstring		AddLayer(LPCWSTR _pcwsLayerName, LmKIND_OF_LAYER _enumKindofLayer);		//레이어 추가
	wstring		InsertLayer(LayerListItr _itrLayer,LPCWSTR _pcwsLayerName,
							LmKIND_OF_LAYER _enumKindofLayer);							//레이어 삽입
	INT			RemoveLayer(LPCWSTR _pcwsLayerName);									//레이어 삭제
	VOID        ModifyLayerName(LPCWSTR _pcwsLayerName, LPCWSTR _pcwsNewLayerName);		//레이어 이름 변경
	
	INT			InsertLayerSection(LPCWSTR _pcwsLayerName,  LPCWSTR _pcwsSectionName, 
									UINT _uStartFrm, UINT _uTimeLength);						//레이어 이름에 Section 삽입
	INT			InsertLayerSection(LPCWSTR _pcwsLayerName, UINT _uStartFrm, UINT _uTimeLength); //레이어 이름에 Section 삽입(Section이름 없음)
	INT			InsertLayerSection(LPCWSTR _pcwsLayerName, LmSection _Section );
	
	INT			RemoveLayerSection( LPCWSTR _pcwsLayerName, LPCWSTR _pcwsSectionName );	//레이어 이름에 Section 이름을 이용한 삭제
	INT			RemoveLayerSection( LPCWSTR _pcwsLayerName, UINT _uStartFrm );			//레이어 이름에 Section 시작 Section을 이용한 삭제

	INT			CheckLayerSectionArea(LPCWSTR _pcwsLayerName,  UINT _uStartFrm, UINT _uEndFrm );  //레이어 이름에 Section을 넣을 수 있는지 확인
	
	
	wstring		GetNewLayerName(LPCWSTR _pcwsLayerName);								//자동으로 번호를 붙여준다.
	wstring		GetCharacterTalkBoxName(LPCWSTR _pcwsCharacterName);					//캐릭터의 TalkBox Layer 이름을 얻어옴

	//Sound Section 관련 내부 함수
	UINT		ChangeMsecToFrame(DWORD _dwSoundLength);						//사운드 밀리초를 프레임으로 변경

	//타임라인 컨트롤 관련 함수
	LayerListItr		FindForLayerName(LPCWSTR _pcwsLayerName);				//레이어를 이름으로 검색한다.
	LayerListItr		FindForLayerNameStrStr(LPCWSTR _pcwsLayerName, 
												LmKIND_OF_LAYER _KindofLayer);	//이름이 포함된 레이어를 찾는다.
	
	LmUITimeLineView*	GetTimeLineView();										//타임라인 뷰의 주소를 얻어온다.
	LmUIMainToolDlg*	GetMainToolDlg();										//메인 툴 다이얼로그 주소를 얻어온다.
	VOID				DrawTimeLineView();										//스크롤을 다시 설정한 후 타임라인 뷰를 다시 그려준다.  
	
};
#define TheTimeLine LmTimeLineManager::GetInstance()

#endif  //_LMTIMELINEMANAGER_H