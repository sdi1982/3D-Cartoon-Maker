//
//
//
//  @ Project : 3D 카툰 메이커
//  @ File Name : LmMainFrame.h
//  @ Date : 2007-10-11
//  @ Author : 신동인
//
//


#if !defined(_LMMAINFRAME_H)
#define _LMMAINFRAME_H

//#include "LmDATA_FILE.h"
#include "LmDefine.h"
#include "LmSceneManager.h"
#include "LmModelManager.h"
#include "LmTimeLineManager.h"
#include "LmPickBoard.h"
#include "LmTalkBox.h"
// 사운드 관련
#include "LmSoundManager.h"
#include "LmSound.h"

//파일 저장 관련
#include "LmDataFileHeader.h"

using namespace std;

enum LmSTATE_OF_UI {INIT_STATE,TIMELINE_CHANGED,SCENE_CHANGED, WIZARD1, WIZARD2, WIZARD3, WIZARD4, 
					TALKBOX_TAB, CAMERA_TAB, SNOW_TAB, FIREWORKS_TAB, SOUND_TAB,SCENE_CREATED};

//파일 헤더의 크기
#define FILE_HEADER_SIZE 4096
#define LM_MAXIMUM_EVENT 64

//
//
//  @ class : 메인 프레임 클래스
//  @ brief : 3D 카툰 메이커의 프로그램 전체를 관리하는 클래스
// 
//  다른 전역객체를 사용하여 사용자의 인터페이스의 실제 구현을 한다.
//
//  @ Date : 2007-10-10
//  @ Author : 신동인
//
//
class LmCartoonMaker {
public:	
	virtual ~LmCartoonMaker();
	static LmCartoonMaker* GetInstance();

	/* D3D 초기화 */
	HRESULT InitD3D(HWND _hWnd, UINT _uWidth, UINT _uHeight, D3DDEVTYPE _DeviceType);
	HRESULT SetupD3D(UINT _uWidth, UINT _uHeight);
	HRESULT CleanupD3D();
	HRESULT UpdateD3D(FLOAT _fTimeDelta);
	HRESULT RenderD3D();
	HRESULT DestroyD3D();
	HRESULT ResizeD3D(UINT _cx, UINT _cy);

	VOID	RenderOnce();

	VOID TrackingMouse(INT _iMouseX, INT _iMouseY, UINT _uHeight, UINT _uWidth);
	BOOL CameraMsgHandler(HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );

	// 3cm.txt 파일을 읽어서 리소스를 등록한다.
	BOOL Read3cmFile();
	
	/* 등록한 리스트 목록 열거 함수*/
	wstring GetListBGFirst(){ return TheModelManager->GetLIstBGFirst(); }
	wstring GetListBGNext(){ return TheModelManager->GetLIstBGNext(); }	
	wstring GetListThingFirst(){ return TheModelManager->GetLIstThingFirst(); }
	wstring GetListThingNext(){ return TheModelManager->GetLIstThingNext(); }	
	wstring GetListCharFirst(){ return TheModelManager->GetLIstCharFirst(); }
	wstring GetListCharNext(){ return TheModelManager->GetLIstCharNext(); }	

	VOID SetMakeSize(ULONG _uWidth, ULONG _uHeight);
	VOID SetMakeMode(BOOL _bMakingMode);

	//////////////////////////////////////////////////////////////////////////

	/* 사용자 인터페이스 - 유스케이스 */

	//현재 사용자 인터페이스 상태
	LmSTATE_OF_UI GetStateOfUI();
	VOID SetStateOfUI(LmSTATE_OF_UI _enumState);
	VOID SetStateOfUIBefore();
	//////////////////////////////////////////////////////////////////////////

	// 파일
	BOOL CreateNewFile(LPCWSTR _pcwsFile);		// 새로만들기
	BOOL OpenFile(LPCWSTR _pcwsFile);				// 열기
	BOOL SaveFile(LPCWSTR _pcwsFile);				// 저장
	//////////////////////////////////////////////////////////////////////////

	// 씬
	BOOL CreateScene(LPCWSTR _pcwsScene, UINT _uFrameSize, UINT _uFPS, UINT _uTerTex);			// 씬 만들기
	BOOL DeleteScene(LPCWSTR _pcwsScene);			// 씬 삭제
	BOOL CopyScene();			// 씬 복제
	BOOL CutScene();			// 씬 잘라내기
	BOOL PasteScene(LPCWSTR _pcwsScene);			// 씬 붙여넣기

	VOID SelectNowScene(LPCWSTR _pcwsScene);	// 현재 씬으로 선택하기
	DWORD GetTotalFrame();

	VOID ShowPickBoard(BOOL _bShow);
	//////////////////////////////////////////////////////////////////////////

	// 카메라
	VOID InsertCameraKey();	// 현재 프레임에 카메라 좌표를 입력한다.
	VOID DeleteCameraKey();

	CONST D3DXVECTOR3* GetCameraEyePt() CONST;
	VOID ResetCameraEyePt();			// 기본 카메라 위치로 이동
	VOID SetCameraEyePt(FLOAT _fX, FLOAT _fY, FLOAT _fZ); // 카메라 절대 위치 입력
	FLOAT GetCameraDist();
	VOID AddCameraDist(FLOAT _fD);
	VOID EnableCameraMouse(BOOL _bEnable){ TheCamera->EnableMouse(_bEnable); }

	//////////////////////////////////////////////////////////////////////////

	// 배경
	BOOL PlaceBGNowScean(LPCWSTR _pcwsBG);					// 탭에서 배경선택하면 바로 배치가 된다.
	BOOL DeleteBGNowScene();
	//////////////////////////////////////////////////////////////////////////

	// 물체
	BOOL SelectThing(LPCWSTR _pcwsThing);		// 탭에서 물건을 선택한다.
	BOOL PlaceSelectedThing();					// 스테이지에 선택된 물건을 배치한다.
	BOOL GetThing();							// 스테이지에서 물건 선택하기
	BOOL RotateGotThing();						// 선택한 물체를 회전하기
	BOOL CancelGotThing();						// 현재 선택한 물체의 선택을 취소한다.
	BOOL DeleteGotThing();						// 선택한 물체를 씬에서 지운다.
	//////////////////////////////////////////////////////////////////////////

	// 캐릭터 부분
	BOOL SelectChar(LPCWSTR _pcwsChar);			// 캐릭터 선택하기 - 텝에서 캐릭터를 선택한다.
	BOOL PlaceSelectedChar();					// 스테이지에 캐릭터를 배치한다.
	BOOL GetChar();								// 스테이지에서 캐릭터 선택하기
	BOOL RotateGotChar();
	BOOL CancelGotChar();						// 현재 선택한 물체의 선택을 취소한다.
	BOOL DeleteGotChar();						// 선택한 물체를 씬에서 지운다.


	BOOL InsertMove(LPCWSTR _pcwsChar, UINT _uTileX, UINT _uTileY);	// 이동 삽입
	BOOL RemoveMove(LPCWSTR _pcwsChar);								// 이동 삭제

	BOOL InsertAction(ACT _uActionType);			// 액션 삽입
	BOOL RemoveAction();							// 액션 삭제
	
	// 말풍선
	// 말풍선을 삽입한다. - 선택된 캐릭터의 현재 프레임에서  크기만큼 동안 보인다.
	BOOL InsertTalkBox( LPCWSTR _pcwsMessage, UINT _uStartFrame, UINT _uFrameSize, COLORREF _colorMsg );
	BOOL InsertTalkBox( LPCWSTR _pcwsChar, LPCWSTR _pcwsMessage, UINT _uStartFrame, UINT _uFrameSize, COLORREF _colorMsg );
	BOOL ModifyTalkBox( LPCWSTR _pcwsChar, LPCWSTR _pcwsMessage, UINT _uOldStartFrame, UINT _uStartFrame, UINT _uFrameSize, COLORREF _colorMsg);
	BOOL RemoveTalkBox( LPCWSTR _pcwsChar, UINT _uStartFrame );
	//////////////////////////////////////////////////////////////////////////

	// 이펙트

	BOOL InsertSnowEffect(LPCWSTR _pcwsEffectName, UINT _uStartFrame, UINT _uEndFrame);
	BOOL RemoveSnowEffect(UINT _uStartFrame);
	BOOL ModifySnowEffect(UINT _uOldStartFrame, LPCWSTR _pcwsEffectName, UINT _uStartFrame, UINT _uEndFrame);


	BOOL InsertFireworkEffect(UINT _uStartFrame);
	BOOL RemoveFireworkEffect(UINT _uStartFrame);

	// 사운드
	BOOL InsertSound(LPCWSTR _pcwFileName, LPCWSTR _pcwSoundID);	// 사운드 삽입
	BOOL RemoveSound(LPCWSTR _pcwSoundID);	// 사운드 삭제
	BOOL SoundIDCheck(LPCWSTR _pcwSoundID);	// 사용 가능한 ID인지 검사
	BOOL CopySound(LPCTSTR lpszDstName, LPCTSTR lpszSrcName);
	BYTE *SaveSound(DWORD *dwSize);
	BOOL LoadSound(BYTE *pData, DWORD dwSize);
	BOOL DeleteSceneSound(LPCWSTR _pcwSceneName);
	
	BOOL SoundPlay(LPCWSTR _pcwSoundID);		// 사운드 재생
	BOOL SetVolume(LPCWSTR _pcwSoundID, UINT Volume);
	UINT GetVolume(LPCWSTR _pcwSoundID);
	BOOL Seek(LPCWSTR _pcwSoundID, DWORD dwMSec);
	VOID Pause();
	VOID FadeIn(LPCWSTR _pcwSoundID, DWORD dwTime);
	VOID FadeOut(LPCWSTR _pcwSoundID, DWORD dwTime);
	DWORD GetCurrentPos(LPCWSTR _pcwSoundID);
	DWORD GetTotalTime(LPCWSTR _pcwSoundID);
	BOOL IsPlay(LPCWSTR _pcwSoundID);
	LPCWSTR GetSoundFileName(LPCWSTR _pcwSoundID);
	LPCWSTR GetLastErrorSound(LPCWSTR _pcwSoundID);


	//////////////////////////////////////////////////////////////////////////

	// 타임라인

	VOID InitTimeLine();		//타임라인 초기화 멤버 함수.

	BOOL PlayTL();				// 플레이
	BOOL PauseTL();				// 멈춤
	BOOL PrevFrameTL();			// 이전 프레임으로
	BOOL NextFrameTL();			// 다음 프레임으로
	BOOL GoStartTL();			// 시작 프레임으로
	BOOL GoEndTL();				// 끝 프레임으로
	LmUIMotionThread* GetMotionThread();	// 모션 스레드를 얻어온다.

	//BOOL SetTotalFrameTL();		// 프레임 총 길이 설정

	INT	GetCurFrame();		//현재 Caret 값을 얻어온다.
	INT	GetFrameLength();	//총 프레임 수를 얻어옴.
	INT	GetFramePerSec();	//초당 프레임 수를 얻어옴.

	VOID SetCurFrame(INT _iCurCaret);		//현재 Caret 값 세팅.
	VOID SetFrameLength(INT _iFrameLength);	//총 프레임 수 세팅.
	VOID SetFramePerSec(INT _iFrmPerSec);	//초당 프레임 수 세팅.

	INT GetListLayerSize();		//Layer List의 크기 리턴
	LayerListItr GetListLayerBeginItr();	//Layer List의 시작 지점 리턴
	LayerListItr GetListLayerEndItr();	//Layer List의 끝 지점 리턴

	// 사운드 인터페이스
	VOID InsertSoundSection(LPCWSTR _pcwsSoundName, UINT _uStartFrm, DWORD _dwSoundLength, DWORD _dwFadeIn, DWORD _dwFadeOut);			//사운드 Section 삽입
	INT	RemoveSoundSection(LPCWSTR _pcwsSoundName);																						//사운드 Section 삭제
	INT ModifySoundSection(LPCWSTR _pcwsSoundName, LPCWSTR _pcwsNewSoundName, 
							UINT _uStartFrm, DWORD _dwSoundLength, DWORD _dwFadeIn, DWORD _dwFadeOut);									//사운드 Section 수정
	SectionVector	GetNowSoundSection(UINT _uFrame, UINT * _uResultCount);							//현재 프레임에서 재생이 시작되어야 할 사운드 Section을 넘겨 줌.
	INT				GetSoundLayerCount();															//사운드 레이어의 갯수를 가르쳐 준다. 

	//Character 인터페이스
	wstring	InsertCharacterLayer(LPCWSTR _pcwsCharacterName);							//캐릭터 입력
	INT	RemoveCharacterLayer(LPCWSTR _pcwsCharacterName);								//캐릭터 삭제
	INT	ModifyCharacterLayerName(LPCWSTR _pcwsLayerName, LPCWSTR _pcwsNewLayerName);	//캐릭터 이름 변경

	//Character TalkBox 외부 인터페이스
	INT InsertTalkBoxSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uTimeLength);		//말풍선 Section 입력
	INT InsertTalkBoxSection(LPCWSTR _pcwsCharacterName, LmSection _Secton);						//말풍선 Section 입력
	INT RemoveTalkBoxSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm);							//말풍선 Section 삭제
	INT ModifyTalkBoxSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm,  UINT _uNewStartFrm, UINT _uNewTimeLength);		//말풍선 Section 수정
	INT CheckTalkBoxSectionArea(LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uEndFrm);								//말풍선이 들어갈 수 있는지 확인
	INT CheckTalkBoxSectionArea(LPCWSTR _pcwsCharacterName, UINT _uOldStartFrm, UINT _uStartFrm, UINT _uEndFrm);			//수정하기에서 말풍선이 들어갈 수 있는지 확인

	//Character Action 외부 인터페이스
	INT InsertActionSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uTimeLength);		//캐릭터 Action Section 입력
	INT InsertActionSection(LPCWSTR _pcwsCharacterName, LmSection _Secton);							//캐릭터 Action Section 입력
	INT RemoveActionSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm);							//캐릭터 Action Section 삭제
	INT ModifyActionSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uNewStartFrm, UINT _uNewTimeLength); 		//캐릭터 Action Section 수정

	//Camera Section 외부 인터페이스
	INT	InsertCameraSection(UINT _uPosFrm);						//카메라 Section 삽입
	INT	InsertCameraSection(LmSection _Secton);					//카메라 Section 삽입
	INT	RemoveCameraSection(UINT _uPosFrm);						//카메라 Section 삭제
	INT	ModifyCameraSection(UINT _uPosFrm, UINT _uNewPosFrm);	//카메라 Section 수정

	//FireWorks Section 외부 인터페이스
	INT	InsertFireWorksSection(UINT _uPosFrm);						//불꽃놀이 Section 삽입
	INT	InsertFireWorksSection(LmSection _Secton);					//불꽃놀이 Section 삽입
	INT	RemoveFireWorksSection(UINT _uPosFrm);						//불꽃놀이 Section 삭제
	INT	ModifyFireWorksSection(UINT _uPosFrm, UINT _uNewPosFrm);	//불꽃놀이 Section 수정

	//눈내리기 Snow 외부 인터페이스
	INT	InsertSnowSection(LPCWSTR _pcwsSnowName, UINT _uStartFrm, UINT _uTimeLength);								//눈내리기 Snow Section 입력
	INT	InsertSnowSection(LmSection _Section);																		//눈내리기 Snow Section 입력
	INT	RemoveSnowSection(UINT _uStartFrm);																			//눈내리기 Snow Section 삭제
	INT	ModifySnowSection(UINT _uStartFrm, LPCWSTR _pcwsNewSnowName, UINT _uNewStartFrm, UINT _uNewTimeLength); 	//눈내리기 Snow Section 수정
	INT CheckSnowSectionArea(UINT _uStartFrm, UINT _uEndFrm);														//눈내리기 Section을 입력할 수 있는지 확인
	INT CheckSnowSectionArea(UINT _uOldStartFrm, UINT _uStartFrm, UINT _uEndFrm);									//수정하기 에서 눈내리기 Section을 입력할 수 있는지 확인

	VOID	SetHMainWnd(HWND _hMainWnd){ m_hMainWnd = _hMainWnd; }

	// [진왕] - 씬 뷰에 썸네일을 등록할때 파일이 캐쉬에 있어서 읽어 올 수 없음. 
	// 그래서 만들었음.
	IPicture* ScreenToMemory();

	BOOL ScreenToFile(LPCWSTR szFileName);
	BOOL ScreenToMemory(LPD3DXBUFFER lpd3dxbuffer);
	VOID SetScreenSize(UINT _uWidth, UINT _uHeight){ m_uVIewWidth = _uWidth; m_uViewHeight = _uHeight; }

	// [진우] 타이머에 의한 OnRender 함수의 작동 여부 설정. 동영상 출력 관련.
	VOID SetRenderTimerOff(){ m_TimerStop = TRUE; }
	VOID SetRenderTimerOn(){ m_TimerStop = FALSE; }
	BOOL IsTimerOn(){ return m_TimerStop?FALSE:TRUE; }

	// [진우]
	BYTE* StringToMemory(std::wstring str, DWORD *dwSize);
	LPCWSTR GetNowSceneName();

	// 파일 처리 관련 멤버 함수
	LPCWSTR GetCurrFileName();
	VOID SetCurrFileName(LPCWSTR _pcwsFileName );
	VOID InitFileOpen();
	VOID InitFileHeader();
	VOID SaveFileHeader(CFile &_File);
	VOID OpenFileHeader(CFile &_File);

	VOID SaveSoundLowData(CFile &_File);
	VOID SaveSceneList(CFile &_File) { return TheSceneManager->SaveSceneList(_File); }
	
	VOID LoadSoundLowData(CFile &_File);
	VOID LoadSceneList(CFile &_File) { return TheSceneManager->LoadSceneList(_File); }

	VOID SetSceneViewStart(UINT _uFilePoint); 
	VOID SetSceneViewEnd(UINT _uFilePoint);
	VOID SetSceneListStart(UINT _uFilePoint); 
	VOID SetSceneListEnd(UINT _uFilePoint);

	UINT GetSceneViewStart() { return m_dataFileHeader.GetSceneViewStart(); }
	UINT GetSceneViewEnd() { return m_dataFileHeader.GetSceneViewEnd(); }
	UINT GetSceneListStart() { return m_dataFileHeader.GetSceneListStart(); }
	UINT GetSceneListEnd() { return m_dataFileHeader.GetSceneListEnd(); }


	DWORD GetEmptyEvent();
	HANDLE* LmEvent(INT EventNum);
	BOOL LmCloseEvent(INT EventNum);


	BOOL GetTalkBoxColor(LPCWSTR _pcwsChar, UINT _uStartFrame, COLORREF& _refColor);
private:
	LmCartoonMaker();

	VOID RenderTitle();
	LPDIRECT3DDEVICE9		m_pDevice;
	D3DPRESENT_PARAMETERS	m_PresentParameter;
	ID3DXEffect*            m_pEffect;       // D3DX 이펙트 인터페이스

	LmThing*				m_pGotThing;	// 스테이지에서 피킹한 물체의 포인터
	LmChar*					m_pGotChar;		// 스테이지에서 피킹한 캐릭터의 포인터

	UINT m_uVIewWidth, m_uViewHeight;
	HWND m_hMainWnd;

	LPD3DXSPRITE m_pTitleSprite;
	LPDIRECT3DTEXTURE9 m_pTitleTexture;

	UINT m_uMakeWidth, m_uMakeHeight;		// 동영상 만들기 작업시에 크기를 지정한다.
	BOOL m_bMakeMode;						// 동영상 만들기 작업용 화면으로 전환한다.

	LmSTATE_OF_UI m_enumStateOfUI;
	LmSTATE_OF_UI m_enumStateOfUIBefore;

	BOOL m_TimerStop;						// 타이머에 의한 OnRender 함수 작동 여부.

	wstring					m_wstrCurrFileName;  //현재 작업중인 파일 이름
	LmDataFileHeader		m_dataFileHeader;

};


// 전역 객체의 인스턴스를 리턴한다.
#define TheCartoonMaker LmCartoonMaker::GetInstance()

#endif  //_LMMAINFRAME_H
