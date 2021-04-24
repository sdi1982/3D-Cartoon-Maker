//
//
//  @ Project : 3D ī�� ����Ŀ
//  @ File Name : LmSection.h
//  @ Date : 2007-12-13
//  @ Author : ������
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
//��� �Լ�
public:
  	static LmTimeLineManager* GetInstance();

	//Ÿ�Ӷ��κ� �ܺ� �������̽�. 
	VOID				InitTimeLIne();		//Ÿ�Ӷ��� �ʱ�ȭ ��� �Լ�.

	INT					GetListSize();		//Layer List�� ũ�� ����
	LayerListItr		GetListBeginItr();	//Layer List�� ���� ���� ����
	LayerListItr		GetListEndItr();	//Layer List�� �� ���� ����

	UINT				GetCurCaret();		//���� Caret ���� ���´�.
	INT					GetFrameLength();	//�� ������ ���� ����.
	INT					GetFramePerSec();	//�ʴ� ������ ���� ����.

	BOOL				SetCurCaret(INT _iCurCaret);		//���� Caret �� ����.
	VOID				SetFrameLength(INT _iFrameLength);	//�� ������ �� ����.
	VOID				SetFramePerSec(INT _iFrmPerSec);	//�ʴ� ������ �� ����.

	//Sound �ܺ� �������̽�
	VOID			InsertSoundSection(LPCWSTR _pcwsSoundName, UINT _uStartFrm, DWORD _dwSoundLength, DWORD _dwFadeIn, DWORD _dwFadeOut);			//���� Section ����
	VOID			InsertSoundSectionProcess( LPCWSTR _pcwsSoundName, UINT _uStartFrm, UINT _uEndFrm, DWORD _dwFadeIn, DWORD _dwFadeOut);
	VOID			ReloadSoundSection(LPCWSTR _pcwsSoundName, UINT _uStartFrm, UINT _uEndFrm, DWORD _dwFadeIn, DWORD _dwFadeOut);				//���� Section ���ε�
	
	INT				RemoveSoundSection(LPCWSTR _pcwsSoundName);													//���� Section ����
	INT				ModifySoundSection(LPCWSTR _pcwsSoundName, LPCWSTR _pcwsNewSoundName,
										UINT _uStartFrm, DWORD _dwSoundLength, DWORD _dwFadeIn, DWORD _dwFadeOut);									//���� Section ����
	SectionVector	GetNowSoundSection(UINT _uFrame, UINT * _uResultCount);										//���� �����ӿ��� ����� ���۵Ǿ�� �� ���� Section�� �Ѱ� ��.
	SectionVector	GetOnStartSoundSection( UINT _uFrame, UINT * _uResultCount );
	INT				GetSoundLayerCount();																		//���� ���̾��� ������ ������ �ش�. 


	//Character �ܺ� �������̽�
	wstring			InsertCharacterLayer(LPCWSTR _pcwsCharacterName);								//ĳ���� �Է�
	INT				RemoveCharacterLayer(LPCWSTR _pcwsCharacterName);								//ĳ���� ����
	INT				ModifyCharacterLayerName(LPCWSTR _pcwsLayerName, LPCWSTR _pcwsNewLayerName);	//ĳ���� �̸� ����
				
	//Character TalkBox �ܺ� �������̽�
	INT				InsertTalkBoxSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uTimeLength);								//��ǳ�� Section �Է�
	INT				InsertTalkBoxSection(LPCWSTR _pcwsCharacterName, LmSection _Section);												//��ǳ�� Section �Է�
	INT				RemoveTalkBoxSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm);													//��ǳ�� Section ����
	INT				ModifyTalkBoxSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm,  UINT _uNewStartFrm, UINT _uNewTimeLength);		//��ǳ�� Section ����
	INT				CheckTalkBoxSectionArea(LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uEndFrm);								//��ǳ���� �� �� �ִ��� Ȯ��
	INT				CheckTalkBoxSectionArea(LPCWSTR _pcwsCharacterName, UINT _uOldStartFrm, UINT _uStartFrm, UINT _uEndFrm);			//�����ϱ⿡�� ��ǳ���� �� �� �ִ��� Ȯ��

	//Character Action �ܺ� �������̽�
	INT				InsertActionSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uTimeLength);								//ĳ���� Action Section �Է�
	INT				InsertActionSection(LPCWSTR _pcwsCharacterName, LmSection _Section);												//ĳ���� Action Section �Է�
	INT				RemoveActionSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm);													//ĳ���� Action Section ����
	INT				ModifyActionSection(LPCWSTR _pcwsCharacterName, UINT _uStartFrm, UINT _uNewStartFrm, UINT _uNewTimeLength); 		//ĳ���� Action Section ����
	
	INT				PushActionSection(LPCWSTR _pcwsCharacterName, UINT _uTimeLength);		//ĳ���� �׼��� Push �Ѵ�.
	INT				PopActionSection(LPCWSTR _pcwsCharacterName, LmSection * _pSection);	//ĳ���� �׼��� Pop �Ѵ�.
										
	//Camera Section �ܺ� �������̽�
	INT				InsertCameraSection(UINT _uPosFrm);						//ī�޶� Section ����
	INT				InsertCameraSection(LmSection _Section);				//ī�޶� Section ����
	INT				RemoveCameraSection(UINT _uPosFrm);						//ī�޶� Section ����
	INT				ModifyCameraSection(UINT _uPosFrm, UINT _uNewPosFrm);	//ī�޶� Section ����

	//FireWorks Section �ܺ� �������̽�
	INT				InsertFireWorksSection(UINT _uPosFrm);						//�Ҳɳ��� Section ����
	INT				InsertFireWorksSection(LmSection _Section);					//�Ҳɳ��� Section ����
	INT				RemoveFireWorksSection(UINT _uPosFrm);						//�Ҳɳ��� Section ����
	INT				ModifyFireWorksSection(UINT _uPosFrm, UINT _uNewPosFrm);	//�Ҳɳ��� Section ����

	//�������� Snow �ܺ� �������̽�
	INT				InsertSnowSection(LPCWSTR _pcwsSnowName, UINT _uStartFrm, UINT _uTimeLength);								//�������� Section �Է�
	INT				InsertSnowSection(LmSection _Section);																		//�������� Section �Է�
	INT				RemoveSnowSection(UINT _uStartFrm);																			//�������� Section ����
	INT				ModifySnowSection(UINT _uStartFrm, LPCWSTR _pcwsNewSnowName, UINT _uNewStartFrm, UINT _uNewTimeLength); 	//�������� Section ����
	INT				CheckSnowSectionArea(UINT _uStartFrm, UINT _uEndFrm);														//�������� Section�� �Է��� �� �ִ��� Ȯ��
	INT				CheckSnowSectionArea(UINT _uOldStartFrm, UINT _uStartFrm, UINT _uEndFrm);									//�����ϱ� ���� �������� Section�� �Է��� �� �ִ��� Ȯ��

	//������ �����Լ�
	INT			GetSectionFirstFrame(LPCWSTR _pcwsLayerName,  UINT _uFrame);  //���̾� �̸��� Section�� ���� �� �ִ��� Ȯ��
	INT			GetSPFrame(LPCWSTR _pcwsLayerName);

	//UI MainTool List �ʱ�ȭ �� ���ε�
	VOID		RefreshUIMainToolList();

	LmUIMotionThread* GetMotionThread();


//��� ����
public:
	LayerList				m_listLayer;			//���̾� ����Ʈ
	INT						m_nFrmPerSec;			//�ʴ� ������ ��
	UINT					m_nCurCaret;			//���� Ŀ���� ��ġ - 1���� ����!!
	INT						m_nFrameLength;			//������ ���� -- Scene�� ���� �ٸ�
	UINT					m_uFireWorksLength;		//�Ҳɳ��� ����.

	//���� ��� �Լ�
private:
	LmTimeLineManager(VOID);
	~LmTimeLineManager(VOID);

	//Layer ���� ���� �Լ�
	wstring		AddLayer(LPCWSTR _pcwsLayerName, LmKIND_OF_LAYER _enumKindofLayer);		//���̾� �߰�
	wstring		InsertLayer(LayerListItr _itrLayer,LPCWSTR _pcwsLayerName,
							LmKIND_OF_LAYER _enumKindofLayer);							//���̾� ����
	INT			RemoveLayer(LPCWSTR _pcwsLayerName);									//���̾� ����
	VOID        ModifyLayerName(LPCWSTR _pcwsLayerName, LPCWSTR _pcwsNewLayerName);		//���̾� �̸� ����
	
	INT			InsertLayerSection(LPCWSTR _pcwsLayerName,  LPCWSTR _pcwsSectionName, 
									UINT _uStartFrm, UINT _uTimeLength);						//���̾� �̸��� Section ����
	INT			InsertLayerSection(LPCWSTR _pcwsLayerName, UINT _uStartFrm, UINT _uTimeLength); //���̾� �̸��� Section ����(Section�̸� ����)
	INT			InsertLayerSection(LPCWSTR _pcwsLayerName, LmSection _Section );
	
	INT			RemoveLayerSection( LPCWSTR _pcwsLayerName, LPCWSTR _pcwsSectionName );	//���̾� �̸��� Section �̸��� �̿��� ����
	INT			RemoveLayerSection( LPCWSTR _pcwsLayerName, UINT _uStartFrm );			//���̾� �̸��� Section ���� Section�� �̿��� ����

	INT			CheckLayerSectionArea(LPCWSTR _pcwsLayerName,  UINT _uStartFrm, UINT _uEndFrm );  //���̾� �̸��� Section�� ���� �� �ִ��� Ȯ��
	
	
	wstring		GetNewLayerName(LPCWSTR _pcwsLayerName);								//�ڵ����� ��ȣ�� �ٿ��ش�.
	wstring		GetCharacterTalkBoxName(LPCWSTR _pcwsCharacterName);					//ĳ������ TalkBox Layer �̸��� ����

	//Sound Section ���� ���� �Լ�
	UINT		ChangeMsecToFrame(DWORD _dwSoundLength);						//���� �и��ʸ� ���������� ����

	//Ÿ�Ӷ��� ��Ʈ�� ���� �Լ�
	LayerListItr		FindForLayerName(LPCWSTR _pcwsLayerName);				//���̾ �̸����� �˻��Ѵ�.
	LayerListItr		FindForLayerNameStrStr(LPCWSTR _pcwsLayerName, 
												LmKIND_OF_LAYER _KindofLayer);	//�̸��� ���Ե� ���̾ ã�´�.
	
	LmUITimeLineView*	GetTimeLineView();										//Ÿ�Ӷ��� ���� �ּҸ� ���´�.
	LmUIMainToolDlg*	GetMainToolDlg();										//���� �� ���̾�α� �ּҸ� ���´�.
	VOID				DrawTimeLineView();										//��ũ���� �ٽ� ������ �� Ÿ�Ӷ��� �並 �ٽ� �׷��ش�.  
	
};
#define TheTimeLine LmTimeLineManager::GetInstance()

#endif  //_LMTIMELINEMANAGER_H