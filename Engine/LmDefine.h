#pragma once

// 유니코드 및 멀티바이트용 tstring 선언
#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif

struct LmBoundBox
{
	LmBoundBox();

	BOOL isPointInside(D3DXVECTOR3& _vP);

	D3DXVECTOR3 vMin;
	D3DXVECTOR3 vMax;
};

struct LmBoundSphere
{
	LmBoundSphere();

	D3DXVECTOR3 vCenter;
	FLOAT       fRadius;
};

struct LmRay
{
	D3DXVECTOR3 vOrigin;
	D3DXVECTOR3 vDirection;
};

const UINT TERRAIN_GRASS = 1;
const UINT TERRAIN_DESERT = 2;
const UINT TERRAIN_SNOW = 3;

// 타임라인에 저장할 카메라 정보
class LmInfoCamera
{
public:
	BOOL bKey;
	D3DXVECTOR3 vEye;
	LmInfoCamera(){ Reset(); }
	VOID Reset()
	{
		bKey = FALSE;
		vEye.x = 0.0f;
		vEye.y = 200.0f;
		vEye.z = -400.0f;
	}
};

class LmKeyTalk
{
public:
	wstring 	wstrMsg;	// 말하는 내용
	UINT		uStartFrame;
	UINT		uEndFrame;
	D3DXCOLOR	dxColor;		// 글자색
	UINT		uShape;		// 물풍선 모양
};

typedef enum{ACT_NONE = 0, ACT_TURN_LEFT, ACT_TURN_RIGHT, ACT_WALK, 
ACT_SHAKE_HAND, ACT_BOW, ACT_TURNNING, ACT_JUMP}ACT;

class LmInfoAct
{
public:
	LmInfoAct(){
		Reset();
	}

	VOID Reset(){
		uActType = ACT_NONE;
		fAngle = 0.0f;
		fActTime = 0.0f;
		vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		iDir = 0;
	}
	ACT uActType;
	FLOAT fAngle;
	INT  iDir;
	FLOAT fActTime;
	D3DXVECTOR3 vPos;
};

class LmKeyAct
{
public:
	LmKeyAct(UINT _uStartFrame, ACT _uActType){
		uStartFrame = _uStartFrame;
		uActType = _uActType;
	}
	UINT uStartFrame;
	ACT uActType;
};

const UINT TURN_LEFT_FRAME_SIZE = 20;
const UINT TURN_RIGHT_FRAME_SIZE = 20;
const UINT WALK_FRAME_SIZE = 40;
const UINT SHAKE_HAND_FRAME_SIZE = 40;
const UINT BOW_FRAME_SIZE = 40;
const UINT DANCE_FRAME_SIZE = 40;
const UINT JUMP_FRAME_SIZE = 40;
const UINT TURNNING_FRAME_SIZE = 40;

class LmKeySnow
{
public:
	LmKeySnow(){ Reset(); }
	VOID Reset()
	{
		uStartFrame = 0;
		uEndFrame = 0;
	}
	UINT uStartFrame;
	UINT uEndFrame;
};

const FLOAT SNOW_TIME_PER_FRAME = 0.1f;

class LmInfoFirework
{
public:
	LmInfoFirework(){ Reset(); }
	VOID Reset()
	{
		bShow = FALSE;
		fElapsedTime = 0.0f;
	}
	BOOL bShow;
	FLOAT fElapsedTime;
};

const UINT FIREWORK_FRAME_SIZE = 40;

typedef enum {TALKBOX_TYPE1 = 0, TALKBOX_TYPE2, TALKBOX_TYPE3, TALKBOX_TYPE4, TALKBOX_TYPE5}TALKBOX_TYPE;