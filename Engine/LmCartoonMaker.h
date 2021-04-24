//
//
//
//  @ Project : 3D ī�� ����Ŀ
//  @ File Name : LmMainFrame.h
//  @ Date : 2007-10-11
//  @ Author : �ŵ���
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
// ���� ����
#include "LmSoundManager.h"
#include "LmSound.h"

//���� ���� ����
#include "LmDataFileHeader.h"

using namespace std;

enum LmSTATE_OF_UI {INIT_STATE,TIMELINE_CHANGED,SCENE_CHANGED, WIZARD1, WIZARD2, WIZARD3, WIZARD4, 
					TALKBOX_TAB, CAMERA_TAB, SNOW_TAB, FIREWORKS_TAB, SOUND_TAB,SCENE_CREATED};

//���� ����� ũ��
#define FILE_HEADER_SIZE 4096
#define LM_MAXIMUM_EVENT 64

//
//
//  @ class : ���� ������ Ŭ����
//  @ brief : 3D ī�� ����Ŀ�� ���α׷� ��ü�� �����ϴ� Ŭ����
// 
//  �ٸ� ������ü�� ����Ͽ� ������� �������̽��� ���� ������ �Ѵ�.
//
//  @ Date : 2007-10-10
//  @ Author : �ŵ���
//
//
class LmCartoonMaker {
public:	
	virtual ~LmCartoonMaker();
	static LmCartoonMaker* GetInstance();

	/* D3D �ʱ�ȭ */
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

	// 3cm.txt ������ �о ���ҽ��� ����Ѵ�.
	BOOL Read3cmFile();
	
	/* ����� ����Ʈ ��� ���� �Լ�*/
	wstring GetListBGFirst(){ return TheModelManager->GetLIstBGFirst(); }
	wstring GetListBGNext(){ return TheModelManager->GetLIstBGNext(); }	
	wstring GetListThingFirst(){ return TheModelManager->GetLIstThingFirst(); }
	wstring GetListThingNext(){ return TheModelManager->GetLIstThingNext(); }	
	wstring GetListCharFirst(){ return TheModelManager->GetLIstCharFirst(); }
	wstring GetListCharNext(){ return TheModelManager->GetLIstCharNext(); }	

	VOID SetMakeSize(ULONG _uWidth, ULONG _uHeight);
	VOID SetMakeMode(BOOL _bMakingMode);

	//////////////////////////////////////////////////////////////////////////

	/* ����� �������̽� - �������̽� */

	//���� ����� �������̽� ����
	LmSTATE_OF_UI GetStateOfUI();
	VOID SetStateOfUI(LmSTATE_OF_UI _enumState);
	VOID SetStateOfUIBefore();
	//////////////////////////////////////////////////////////////////////////

	// ����
	BOOL CreateNewFile(LPCWSTR _pcwsFile);		// ���θ����
	BOOL OpenFile(LPCWSTR _pcwsFile);				// ����
	BOOL SaveFile(LPCWSTR _pcwsFile);				// ����
	//////////////////////////////////////////////////////////////////////////

	// ��
	BOOL CreateScene(LPCWSTR _pcwsScene, UINT _uFrameSize, UINT _uFPS, UINT _uTerTex);			// �� �����
	BOOL DeleteScene(LPCWSTR _pcwsScene);			// �� ����
	BOOL CopyScene();			// �� ����
	BOOL CutScene();			// �� �߶󳻱�
	BOOL PasteScene(LPCWSTR _pcwsScene);			// �� �ٿ��ֱ�

	VOID SelectNowScene(LPCWSTR _pcwsScene);	// ���� ������ �����ϱ�
	DWORD GetTotalFrame();

	VOID ShowPickBoard(BOOL _bShow);
	//////////////////////////////////////////////////////////////////////////

	// ī�޶�
	VOID InsertCameraKey();	// ���� �����ӿ� ī�޶� ��ǥ�� �Է��Ѵ�.
	VOID DeleteCameraKey();

	CONST D3DXVECTOR3* GetCameraEyePt() CONST;
	VOID ResetCameraEyePt();			// �⺻ ī�޶� ��ġ�� �̵�
	VOID SetCameraEyePt(FLOAT _fX, FLOAT _fY, FLOAT _fZ); // ī�޶� ���� ��ġ �Է�
	FLOAT GetCameraDist();
	VOID AddCameraDist(FLOAT _fD);
	VOID EnableCameraMouse(BOOL _bEnable){ TheCamera->EnableMouse(_bEnable); }

	//////////////////////////////////////////////////////////////////////////

	// ���
	BOOL PlaceBGNowScean(LPCWSTR _pcwsBG);					// �ǿ��� ��漱���ϸ� �ٷ� ��ġ�� �ȴ�.
	BOOL DeleteBGNowScene();
	//////////////////////////////////////////////////////////////////////////

	// ��ü
	BOOL SelectThing(LPCWSTR _pcwsThing);		// �ǿ��� ������ �����Ѵ�.
	BOOL PlaceSelectedThing();					// ���������� ���õ� ������ ��ġ�Ѵ�.
	BOOL GetThing();							// ������������ ���� �����ϱ�
	BOOL RotateGotThing();						// ������ ��ü�� ȸ���ϱ�
	BOOL CancelGotThing();						// ���� ������ ��ü�� ������ ����Ѵ�.
	BOOL DeleteGotThing();						// ������ ��ü�� ������ �����.
	//////////////////////////////////////////////////////////////////////////

	// ĳ���� �κ�
	BOOL SelectChar(LPCWSTR _pcwsChar);			// ĳ���� �����ϱ� - �ܿ��� ĳ���͸� �����Ѵ�.
	BOOL PlaceSelectedChar();					// ���������� ĳ���͸� ��ġ�Ѵ�.
	BOOL GetChar();								// ������������ ĳ���� �����ϱ�
	BOOL RotateGotChar();
	BOOL CancelGotChar();						// ���� ������ ��ü�� ������ ����Ѵ�.
	BOOL DeleteGotChar();						// ������ ��ü�� ������ �����.


	BOOL InsertMove(LPCWSTR _pcwsChar, UINT _uTileX, UINT _uTileY);	// �̵� ����
	BOOL RemoveMove(LPCWSTR _pcwsChar);								// �̵� ����

	BOOL InsertAction(ACT _uActionType);			// �׼� ����
	BOOL RemoveAction();							// �׼� ����
	
	// ��ǳ��
	// ��ǳ���� �����Ѵ�. - ���õ� ĳ������ ���� �����ӿ���  ũ�⸸ŭ ���� ���δ�.
	BOOL InsertTalkBox( LPCWSTR _pcwsMessage, UINT _uStartFrame, UINT _uFrameSize, COLORREF _colorMsg );
	BOOL InsertTalkBox( LPCWSTR _pcwsChar, LPCWSTR _pcwsMessage, UINT _uStartFrame, UINT _uFrameSize, COLORREF _colorMsg );
	BOOL ModifyTalkBox( LPCWSTR _pcwsChar, LPCWSTR _pcwsMessage, UINT _uOldStartFrame, UINT _uStartFrame, UINT _uFrameSize, COLORREF _colorMsg);
	BOOL RemoveTalkBox( LPCWSTR _pcwsChar, UINT _uStartFrame );
	//////////////////////////////////////////////////////////////////////////

	// ����Ʈ

	BOOL InsertSnowEffect(LPCWSTR _pcwsEffectName, UINT _uStartFrame, UINT _uEndFrame);
	BOOL RemoveSnowEffect(UINT _uStartFrame);
	BOOL ModifySnowEffect(UINT _uOldStartFrame, LPCWSTR _pcwsEffectName, UINT _uStartFrame, UINT _uEndFrame);


	BOOL InsertFireworkEffect(UINT _uStartFrame);
	BOOL RemoveFireworkEffect(UINT _uStartFrame);

	// ����
	BOOL InsertSound(LPCWSTR _pcwFileName, LPCWSTR _pcwSoundID);	// ���� ����
	BOOL RemoveSound(LPCWSTR _pcwSoundID);	// ���� ����
	BOOL SoundIDCheck(LPCWSTR _pcwSoundID);	// ��� ������ ID���� �˻�
	BOOL CopySound(LPCTSTR lpszDstName, LPCTSTR lpszSrcName);
	BYTE *SaveSound(DWORD *dwSize);
	BOOL LoadSound(BYTE *pData, DWORD dwSize);
	BOOL DeleteSceneSound(LPCWSTR _pcwSceneName);
	
	BOOL SoundPlay(LPCWSTR _pcwSoundID);		// ���� ���
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

	// Ÿ�Ӷ���

	VOID InitTimeLine();		//Ÿ�Ӷ��� �ʱ�ȭ ��� �Լ�.

	BOOL PlayTL();				// �÷���
	BOOL PauseTL();				// ����
	BOOL PrevFrameTL();			// ���� ����������
	BOOL NextFrameTL();			// ���� ����������
	BOOL GoStartTL();			// ���� ����������
	BOOL GoEndTL();				// �� ����������
	LmUIMotionThread* GetMotionThread();	// ��� �����带 ���´�.

	//BOOL SetTotalFrameTL();		// ������ �� ���� ����

	INT	GetCurFrame();		//���� Caret ���� ���´�.
	INT	GetFrameLength();	//�� ������ ���� ����.
	INT	GetFramePerSec();	//�ʴ� ������ ���� ����.

	VOID SetCurFrame(INT _iCurCaret);		//���� Caret �� ����.
	VOID SetFrameLength(INT _iFrameLength);	//�� ������ �� ����.
	VOID SetFramePerSec(INT _iFrmPerSec);	//�ʴ� ������ �� ����.

	INT GetListLayerSize();		//Layer List�� ũ�� ����
	LayerListItr GetListLayerBeginItr();	//Layer List�� ���� ���� ����
	LayerListItr GetListLayerEndItr();	//Layer List�� �� ���� ����

	// ���� �������̽�
	VOID InsertSoundSection(LPCWSTR _pcwsSoundName, UINT _uStartFrm, DWORD _dwSoundLength, DWORD _dwFadeIn, DWORD _dwFadeOut);			//���� Section ����
	INT	RemoveSoundSection(LPCWSTR _pcwsSoundName);																						//���� Section ����
	INT ModifySoundSection(LPCWSTR _pcwsSoundName, LPCWSTR _pcwsNewSoundName, 
							UINT _uStartFrm, DWORD _dwSoundLength, DWORD _dwFadeIn, DWORD _dwFadeOut);									//���� Section ����
	SectionVector	GetNowSoundSection(UINT _uFrame, UINT * _uResultCount);							//���� �����ӿ��� ����� ���۵Ǿ�� �� ���� Section�� �Ѱ� ��.
	INT				GetSoundLayerCount();															//���� ���̾��� ������ ������ �ش�. 

	//Character �������̽�
	wstring	InsertCharacterLayer(LPCWSTR _pcwsCharacterName);							//ĳ���� �Է�
	INT	RemoveCharacterLayer(LPCWSTR _pcwsCharacterName);								//ĳ���� ����
	INT	ModifyCharacterLayerName(LPCWSTR _pcwsLayerName, LPCWSTR _pcwsNewLayerName);	//ĳ���� �̸� ����

	//Character TalkBox �ܺ� �������̽�
	INT InsertTalkBoxSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uTimeLength);		//��ǳ�� Section �Է�
	INT InsertTalkBoxSection(LPCWSTR _pcwsCharacterName, LmSection _Secton);						//��ǳ�� Section �Է�
	INT RemoveTalkBoxSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm);							//��ǳ�� Section ����
	INT ModifyTalkBoxSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm,  UINT _uNewStartFrm, UINT _uNewTimeLength);		//��ǳ�� Section ����
	INT CheckTalkBoxSectionArea(LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uEndFrm);								//��ǳ���� �� �� �ִ��� Ȯ��
	INT CheckTalkBoxSectionArea(LPCWSTR _pcwsCharacterName, UINT _uOldStartFrm, UINT _uStartFrm, UINT _uEndFrm);			//�����ϱ⿡�� ��ǳ���� �� �� �ִ��� Ȯ��

	//Character Action �ܺ� �������̽�
	INT InsertActionSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uTimeLength);		//ĳ���� Action Section �Է�
	INT InsertActionSection(LPCWSTR _pcwsCharacterName, LmSection _Secton);							//ĳ���� Action Section �Է�
	INT RemoveActionSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm);							//ĳ���� Action Section ����
	INT ModifyActionSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uNewStartFrm, UINT _uNewTimeLength); 		//ĳ���� Action Section ����

	//Camera Section �ܺ� �������̽�
	INT	InsertCameraSection(UINT _uPosFrm);						//ī�޶� Section ����
	INT	InsertCameraSection(LmSection _Secton);					//ī�޶� Section ����
	INT	RemoveCameraSection(UINT _uPosFrm);						//ī�޶� Section ����
	INT	ModifyCameraSection(UINT _uPosFrm, UINT _uNewPosFrm);	//ī�޶� Section ����

	//FireWorks Section �ܺ� �������̽�
	INT	InsertFireWorksSection(UINT _uPosFrm);						//�Ҳɳ��� Section ����
	INT	InsertFireWorksSection(LmSection _Secton);					//�Ҳɳ��� Section ����
	INT	RemoveFireWorksSection(UINT _uPosFrm);						//�Ҳɳ��� Section ����
	INT	ModifyFireWorksSection(UINT _uPosFrm, UINT _uNewPosFrm);	//�Ҳɳ��� Section ����

	//�������� Snow �ܺ� �������̽�
	INT	InsertSnowSection(LPCWSTR _pcwsSnowName, UINT _uStartFrm, UINT _uTimeLength);								//�������� Snow Section �Է�
	INT	InsertSnowSection(LmSection _Section);																		//�������� Snow Section �Է�
	INT	RemoveSnowSection(UINT _uStartFrm);																			//�������� Snow Section ����
	INT	ModifySnowSection(UINT _uStartFrm, LPCWSTR _pcwsNewSnowName, UINT _uNewStartFrm, UINT _uNewTimeLength); 	//�������� Snow Section ����
	INT CheckSnowSectionArea(UINT _uStartFrm, UINT _uEndFrm);														//�������� Section�� �Է��� �� �ִ��� Ȯ��
	INT CheckSnowSectionArea(UINT _uOldStartFrm, UINT _uStartFrm, UINT _uEndFrm);									//�����ϱ� ���� �������� Section�� �Է��� �� �ִ��� Ȯ��

	VOID	SetHMainWnd(HWND _hMainWnd){ m_hMainWnd = _hMainWnd; }

	// [����] - �� �信 ������� ����Ҷ� ������ ĳ���� �־ �о� �� �� ����. 
	// �׷��� �������.
	IPicture* ScreenToMemory();

	BOOL ScreenToFile(LPCWSTR szFileName);
	BOOL ScreenToMemory(LPD3DXBUFFER lpd3dxbuffer);
	VOID SetScreenSize(UINT _uWidth, UINT _uHeight){ m_uVIewWidth = _uWidth; m_uViewHeight = _uHeight; }

	// [����] Ÿ�̸ӿ� ���� OnRender �Լ��� �۵� ���� ����. ������ ��� ����.
	VOID SetRenderTimerOff(){ m_TimerStop = TRUE; }
	VOID SetRenderTimerOn(){ m_TimerStop = FALSE; }
	BOOL IsTimerOn(){ return m_TimerStop?FALSE:TRUE; }

	// [����]
	BYTE* StringToMemory(std::wstring str, DWORD *dwSize);
	LPCWSTR GetNowSceneName();

	// ���� ó�� ���� ��� �Լ�
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
	ID3DXEffect*            m_pEffect;       // D3DX ����Ʈ �������̽�

	LmThing*				m_pGotThing;	// ������������ ��ŷ�� ��ü�� ������
	LmChar*					m_pGotChar;		// ������������ ��ŷ�� ĳ������ ������

	UINT m_uVIewWidth, m_uViewHeight;
	HWND m_hMainWnd;

	LPD3DXSPRITE m_pTitleSprite;
	LPDIRECT3DTEXTURE9 m_pTitleTexture;

	UINT m_uMakeWidth, m_uMakeHeight;		// ������ ����� �۾��ÿ� ũ�⸦ �����Ѵ�.
	BOOL m_bMakeMode;						// ������ ����� �۾��� ȭ������ ��ȯ�Ѵ�.

	LmSTATE_OF_UI m_enumStateOfUI;
	LmSTATE_OF_UI m_enumStateOfUIBefore;

	BOOL m_TimerStop;						// Ÿ�̸ӿ� ���� OnRender �Լ� �۵� ����.

	wstring					m_wstrCurrFileName;  //���� �۾����� ���� �̸�
	LmDataFileHeader		m_dataFileHeader;

};


// ���� ��ü�� �ν��Ͻ��� �����Ѵ�.
#define TheCartoonMaker LmCartoonMaker::GetInstance()

#endif  //_LMMAINFRAME_H
