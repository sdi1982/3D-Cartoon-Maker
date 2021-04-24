#if !defined(_LMCHARACTER_H)
#define _LMCHARACTER_H


#include "LmThing.h"
#include "LmAnimate.h"
#include "LmTalkBox.h"

class LmChar : public LmThing {
public:
	// 생성 관련
	LmChar(LPCWSTR _pcwsName);
	LmChar(LPCWSTR _pcwsName, LPCWSTR _pcwsXFile);
	virtual ~LmChar();

	// 복사, 대입연산자
	LmChar(const LmChar& _rhs);
	LmChar& operator=(LmChar& _rhs);

	LmChar* Clone(LPCWSTR _pcwsClone);

	BOOL Init(LPDIRECT3DDEVICE9 _pDevice );
	VOID Update(FLOAT _fTimeDelta);
	VOID SetCurrFrame(UINT _uFrame);
	VOID Render(LPDIRECT3DDEVICE9 _pDevice);
	VOID Setup(UINT _uScreenWidth, UINT _uScreenHeight);
	VOID Cleanup();
	VOID SetFrameSize(UINT _uFrameSize){ m_listInfoAct.resize(_uFrameSize+1); }

	BOOL Place(FLOAT _fStepX, FLOAT _fStepZ);	// 타일의 위치를 설정한다.
	VOID Rotate();

	VOID TurnLeft(UINT _uStartFrame);
	VOID TurnRight(UINT _uStartFrame);
	VOID Walk(UINT _uStartFrame);
	VOID ShakeHand(UINT _uStartFrame);
	VOID Bow(UINT _uStartFrame);
	VOID Turnning(UINT _uStartFrame);
	VOID Jump(UINT _uStartFrame);
	BOOL DeleteAction();
	BOOL CanWalk();

	VOID InitTimeLine();

	BOOL InsertTalkBox( LPCWSTR _pcwsMessage, UINT _uStartFrame, UINT _uFrameSize, D3DXCOLOR _dxColor);
	BOOL ModifyTalkBox( UINT _uOldStartFrame, LPCWSTR _pcwsMessage, UINT _uStartFrame, UINT _uFrameSize, 
						D3DXCOLOR _dxColor);
	BOOL RemoveTalkBox( UINT _uOldStartFrame );

	D3DXCOLOR GetTalkBoxColor(UINT _uStartFrame);
private:
	vector<LmInfoAct>  m_listInfoAct;			// 액션 정보 리스트
	vector<LmKeyAct>   m_listKeyAct;			// 액션 키 리스트
	vector<LmKeyTalk>   m_listKeyTalkBox;			// 말풍선 리스트

	UINT	m_uLastActOffset;

	LmTalkBox* m_pTalkBox;
	LmAnimate* m_pAnimate;
	LPDIRECT3DDEVICE9 m_pDevice;
};


#endif // _LMCHARACTER_H