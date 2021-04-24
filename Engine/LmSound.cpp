#include "stdafx.h"
#include "LmSound.h"

#include <process.h>

//------------------------------------------------------------------------------
// 소멸자
// 재생 정지 요청을 하고 재생이 완전히 정지 될 때 까지 기다렸다 소멸 수행.
//------------------------------------------------------------------------------
LmSound::~LmSound()
{
	m_State = SOUND_STOP;

	WaitForSingleObject(m_THREAD_ARG.hEvent, INFINITE);

	if (m_lpds)
		m_lpds->Release();

	if (m_hmmio)
		mmioClose(m_hmmio, NULL);

	if (m_pWaveFormatex)
		delete m_pWaveFormatex;
}


//------------------------------------------------------------------------------
// 생성자
// 멤버 변수 초기화
//------------------------------------------------------------------------------
LmSound::LmSound(LPCTSTR lpszFileName, HWND hwnd)
{
	ZeroMemory(m_szFileName, FILE_NAME_LEN);
	ZeroMemory(m_szErrMsg, MAXIMUM_ERR_MSG);
	ZeroMemory(&m_THREAD_ARG, sizeof(THREAD_ARG));	

	m_State			= SOUND_STOP;
	m_Volume		= 100;
	m_dwFadeIn		= 0;
	m_dwFadeOut		= 0;

	m_lpds			= NULL;
	m_lpdsBuffer	= NULL;
	m_pWaveFormatex	= NULL;
	m_dsBufferSize	= 0;

	_tcscpy_s(m_szFileName, lpszFileName);
	m_hwnd = hwnd;

	m_THREAD_ARG.pState = &m_State;	
}


//------------------------------------------------------------------------------
// 초기화(wave 정보 얻기, dsound 설정)
//------------------------------------------------------------------------------
BOOL LmSound::Init()
{
	if (m_lpdsBuffer){
		SetErrorMsg(_T("already initialized"));
		return FALSE;
	}

	if (!WaveInit())
		return FALSE;

	if (!DSoundInit())
		return FALSE;

	return TRUE;
}


//------------------------------------------------------------------------------
// Wave 초기화
// Wave 헤더에서 재생 정보 얻기
//------------------------------------------------------------------------------
BOOL LmSound::WaveInit()
{
	MMCKINFO		ckInRIFF, ckIn;
	PCMWAVEFORMAT	pcmWaveFormat;
	MMRESULT		hr;
	int				ReadSize;

	m_hmmio = mmioOpen(m_szFileName, NULL, MMIO_ALLOCBUF | MMIO_READ);

	if (!m_hmmio){
		SetErrorMsg(_T("Wave file open error"));
		return FALSE;
	}

	ckInRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	if ((hr = mmioDescend(m_hmmio, &ckInRIFF, NULL, MMIO_FINDRIFF))){
		mmioClose(m_hmmio, NULL);
		SetErrorMsg(_T("Not found WAVE chunk"));
		return FALSE;
	}

	ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (hr = mmioDescend(m_hmmio, &ckIn, &ckInRIFF, MMIO_FINDCHUNK)){
		mmioClose(m_hmmio, NULL);
		SetErrorMsg(_T("Not found fmt chunk"));
		return FALSE;
	}

	// wave 정보 얻기
	if (ReadSize = mmioRead(m_hmmio, (HPSTR)&pcmWaveFormat, sizeof(pcmWaveFormat))
		!= sizeof(pcmWaveFormat)){
			mmioClose(m_hmmio, NULL);
			return FALSE;
	}	
	m_pWaveFormatex = new WAVEFORMATEX;
	memcpy(m_pWaveFormatex, &pcmWaveFormat, sizeof(pcmWaveFormat));
	m_pWaveFormatex->cbSize = 0;		// 추가 정보 크기 0

	if (!CheckWfxFormat(m_pWaveFormatex)){
		mmioClose(m_hmmio, NULL);
		SetErrorMsg(_T("Not support"));
		return FALSE;
	}


	// 상위 청크로 이동
	if (mmioAscend(m_hmmio, &ckIn, 0)){
		mmioClose(m_hmmio, NULL);
		SetErrorMsg(_T("Ascend Chunk err"));
		return FALSE;
	}

	// data 청크로 이동
	ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(m_hmmio, &ckIn, &ckInRIFF, MMIO_FINDCHUNK)){
		mmioClose(m_hmmio, NULL);
		SetErrorMsg(_T("data Chunk err"));
		return FALSE;
	}

	// 재생 시간 구하기. ms 단위.
	m_dwDataCkSize = ckIn.cksize;
	m_THREAD_ARG.dwTotalByte = ckIn.cksize;
	m_THREAD_ARG.dwTotalTime = ckIn.cksize / m_pWaveFormatex->nBlockAlign;
	m_THREAD_ARG.dwTotalTime = (DWORD)(m_THREAD_ARG.dwTotalTime / ((double)m_pWaveFormatex->nSamplesPerSec / 1000));
	m_THREAD_ARG.pWaveFormatex = m_pWaveFormatex;

	return TRUE;
}


//------------------------------------------------------------------------------
// Direct sound 초기화
// 사운드 생성 및 초기화.
//------------------------------------------------------------------------------
BOOL LmSound::DSoundInit()
{
	HRESULT			hr;

	// 생성
	if (hr = DirectSoundCreate(NULL, &m_lpds, NULL) != DS_OK){
		SetErrorMsg(_T("Direct sound create error"));
		return FALSE;
	}

	// 협력 레벨 설정
	if (hr = m_lpds->SetCooperativeLevel(m_hwnd, DSSCL_NORMAL)){
		SetErrorMsg(_T("Direct sound cooperative level error"));
		return FALSE;
	}

	m_THREAD_ARG.dsBufferSize = m_pWaveFormatex->nAvgBytesPerSec;	
	m_THREAD_ARG.phmmio = m_hmmio;
	m_dsBufferSize = m_pWaveFormatex->nAvgBytesPerSec;


	return TRUE;
}


//------------------------------------------------------------------------------
// DSound 버퍼 생성
//------------------------------------------------------------------------------
BOOL LmSound::CreateDSBuffer()
{
	HRESULT			hr;
	DSBUFFERDESC	dsBufferDesc;

	// 버퍼 설정. 버퍼의 크기는 재생할 wave 파일을 1초 동안 담을 수 있는 크기.
	ZeroMemory(&dsBufferDesc, sizeof(DSBUFFERDESC));
	dsBufferDesc.dwSize = sizeof(DSBUFFERDESC);
	dsBufferDesc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPOSITIONNOTIFY;
	dsBufferDesc.dwBufferBytes = m_pWaveFormatex->nAvgBytesPerSec;
	dsBufferDesc.lpwfxFormat = m_pWaveFormatex;

	if (hr = m_lpds->CreateSoundBuffer(&dsBufferDesc, &m_lpdsBuffer, NULL)){
		SetErrorMsg(_T("Create sound buffer error"));
		return FALSE;
	}

	m_THREAD_ARG.pdsBuffer = m_lpdsBuffer;

	return TRUE;
}


//------------------------------------------------------------------------------
// DSound 버퍼 릴리즈
//------------------------------------------------------------------------------
BOOL LmSound::ReleaseDSBuffer()
{
	if (!m_lpdsBuffer){
		SetErrorMsg(_T("DSBuffer is not exist."));
		return FALSE;
	}

	m_lpdsBuffer->Release();
	
	return TRUE;
}


//------------------------------------------------------------------------------
// 재생
// 재생 버퍼, 이벤트 생성
//------------------------------------------------------------------------------
BOOL LmSound::Play()
{
	VOID	*pData;
	DWORD	DataSize;
	HRESULT	hr;
	//BOOL	bThread;

	if (m_State == SOUND_PLAY){
		SetErrorMsg(_T("Playing now"));
		return FALSE;
	}

	// 재생 동기화 관련 이벤트
	m_THREAD_ARG.hEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	ResetEvent(m_THREAD_ARG.hEvent);
	
	// 재생 시간 계산 관련 이벤트
	m_THREAD_ARG.hEventEndPos = CreateEvent(NULL, FALSE, TRUE, NULL);
	ResetEvent(m_THREAD_ARG.hEventEndPos);

	// 각종 포인터 들을 처음 위치로 이동.
	m_THREAD_ARG.dwCurrentTime = 0;
	m_THREAD_ARG.dwCurrentPosByte = 0;
	mmioSeek(m_hmmio, 44, SEEK_SET);

	// 버퍼 생성
	if (!CreateDSBuffer()){
		return FALSE;
	}


	// 버퍼에 데이터 채우기
	hr = m_lpdsBuffer->Lock(0, 0, &pData, &DataSize, NULL, NULL, DSBLOCK_ENTIREBUFFER);
	if (hr != DS_OK){
		SetErrorMsg(_T("Create buffer error"));
		return FALSE;
	}
	memset(pData, 0, DataSize);

	DWORD	ReadSize;
	if (!(ReadSize = mmioRead(m_hmmio, (HPSTR)pData, DataSize))){
		SetErrorMsg(_T("file read error"));
		return FALSE;
	}
	hr = m_lpdsBuffer->Unlock(pData, DataSize, NULL, NULL);	

	m_THREAD_ARG.dwCurrentPosByte += ReadSize;


	// 음량 설정
	SetVolume(m_Volume);

	// 버퍼 통지 이벤트 설정
	SetNotification(m_THREAD_ARG.hEventEndPos, m_dsBufferSize - 1);

	// 재생
	m_State = SOUND_PLAY;
	m_THREAD_ARG.ThreadState = SOUND_PLAY;
	hr = m_lpdsBuffer->Play(0, 0, DSBPLAY_LOOPING);

	// 재생 결과 검사
	if (hr != DS_OK){
		SetErrorMsg(_T("Play error"));
		return FALSE;
	}	


	// 버퍼 통지 스레드 시작
	_beginthread(NotifyThread, 0, &m_THREAD_ARG);
	
	// 데이터 채우는 스레드 시작
	_beginthread(StreamThread, 0, &m_THREAD_ARG);

	return TRUE;
}

//------------------------------------------------------------------------------
// 정지
// 정지 요청. 실제 정지 수행은 StreamThread에서 일어남.
//------------------------------------------------------------------------------
VOID LmSound::Stop()
{
	if (!m_lpdsBuffer)
		return;

	//m_lpdsBuffer->Stop();
	m_THREAD_ARG.ThreadState = SOUND_STOP;
}


//------------------------------------------------------------------------------
// 에러 메시지 설정
//------------------------------------------------------------------------------
VOID LmSound::SetErrorMsg(LPCTSTR lpszErrMsg)
{
	_tcscpy_s(m_szErrMsg, lpszErrMsg);//, MAXIMUM_ERR_MSG);	
}

UINT LmSound::GetVolume()
{
	return m_Volume;
}

BOOL LmSound::SetVolume(UINT Volume)
{	
	if (!m_lpdsBuffer){
		SetErrorMsg(_T("DSBuffer is not initialization"));
		return FALSE;
	}

	if (Volume < 0 || Volume > 100){
		SetErrorMsg(_T("Volume out of range"));
		return FALSE;
	}
	

	HRESULT			hr;
	m_Volume = Volume;
	hr = m_lpdsBuffer->SetVolume(-30*(100 - Volume));

	return TRUE;
}

// 비어있는 이벤트 번호를 찾음
int LmSound::GetEmptyEventNum()
{
	// 이 함수는 필요 없을 듯.

	return -1;
}


//------------------------------------------------------------------------------
// Fade in
// dwTime : Fade in 될 시간. 0이면 하지 않음.
//------------------------------------------------------------------------------
VOID LmSound::FadeIn(DWORD dwTime)
{
	m_dwFadeIn = dwTime;
	m_THREAD_ARG.dwFadeIn = dwTime;
}


//------------------------------------------------------------------------------
// Fade out
// dwTime : Fade out 시간. 0이면 하지 않음.
//------------------------------------------------------------------------------
VOID LmSound::FadeOut(DWORD dwTime)
{
	m_dwFadeOut = dwTime;
	m_THREAD_ARG.dwFadeOut = dwTime;
}


//------------------------------------------------------------------------------
// 현재 재생 시간을 리턴
//------------------------------------------------------------------------------
DWORD LmSound::GetCurrentTime()
{
	if (m_State != SOUND_PLAY)
		return 0;

	DWORD time;

	time = GetCurrentPosByte() / (m_pWaveFormatex->nBlockAlign * m_pWaveFormatex->nSamplesPerSec / 1000);

	if (GetTotalTime() < time)
		return GetTotalTime();
	else
		return time;
}


//------------------------------------------------------------------------------
// 전체 재생 시간을 리턴
//------------------------------------------------------------------------------
DWORD LmSound::GetTotalTime()
{
	return m_THREAD_ARG.dwTotalTime;
}


//------------------------------------------------------------------------------
// 원하는 위치로 재생 포인터를 이동
// dwMsec : 밀리 세컨드 단위의 시간.
//------------------------------------------------------------------------------
BOOL LmSound::Seek(DWORD dwMSec)
{
	
	if (m_dwDataCkSize < dwMSec * (m_pWaveFormatex->nAvgBytesPerSec / 1000)){
		SetErrorMsg(_T("Time over"));
		return FALSE;
	}

	m_THREAD_ARG.IsSeek = TRUE;
	m_THREAD_ARG.SeekPos = 44 + dwMSec * (m_pWaveFormatex->nAvgBytesPerSec / 1000);
	//mmioSeek(m_hmmio, m_THREAD_ARG.dwCurrentPosByte, SEEK_SET);
	

	return TRUE;
}


//------------------------------------------------------------------------------
// 현재 재생 위치를 바이트 단위로 리턴
//------------------------------------------------------------------------------
DWORD LmSound::GetCurrentPosByte()
{
	if (!m_lpdsBuffer)
		return 0;

	DWORD		pos;

	if (m_THREAD_ARG.ThreadState == SOUND_PLAY)
		m_lpdsBuffer->GetCurrentPosition(&pos, NULL);
	else
		pos = 0;
	
	return m_THREAD_ARG.dwBufferCycle * m_THREAD_ARG.dsBufferSize + pos;

}


//------------------------------------------------------------------------------
// 전체 재생 크기를 바이트 단위로 리턴
//------------------------------------------------------------------------------
DWORD LmSound::GetTotalByte()
{
	return m_dwDataCkSize;
}


//------------------------------------------------------------------------------
// 재생 여부를 리턴.
// TRUE 리턴시 현재 재생중을 의미.
//------------------------------------------------------------------------------
BOOL LmSound::IsPlay()
{
	if (m_State == SOUND_PLAY)
		return TRUE;
	else
		return FALSE;
}


//------------------------------------------------------------------------------
// 버퍼 통지 위치 설정
// hEvent : 이벤트 핸들
// dwOffset : 버퍼 내의 통지 위치 설정
//------------------------------------------------------------------------------
HRESULT LmSound::SetNotification(HANDLE hEvent, DWORD dwOffset)
{

	LPDIRECTSOUNDNOTIFY8 lpDsNotify; 
	DSBPOSITIONNOTIFY PositionNotify;
	HRESULT hr;


	if (SUCCEEDED(
		hr = m_lpdsBuffer->QueryInterface(IID_IDirectSoundNotify8,
		(LPVOID*) &lpDsNotify))) 
	{
		PositionNotify.dwOffset = dwOffset;
		PositionNotify.hEventNotify = hEvent;
		hr = lpDsNotify->SetNotificationPositions(1, &PositionNotify);
		lpDsNotify->Release();
	}
	
	return hr;
}

BOOL LmSound::CheckWfxFormat(WAVEFORMATEX *pWfx)
{
	if (pWfx->nSamplesPerSec != 44100 &&
		pWfx->nSamplesPerSec != 22050)
		return FALSE;

	if (pWfx->wBitsPerSample != 8 &&
		pWfx->wBitsPerSample != 16)
		return FALSE;

	if (pWfx->nChannels != 1 &&
		pWfx->nChannels != 2)
		return FALSE;

	return TRUE;
}

LPCTSTR LmSound::GetFileName()
{
	return m_szFileName;
}



//////////////////////////////////////////////////////////////////////////
// 이제 부터 static 함수
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// DSound 버퍼에 데이터를 채워 넣음
// lpdsBuffer : 버퍼
// pArg : 재생관련 변수 구조체의 포인터(THREAD_ARG)
// dwOffset : 어디서 부터 채울 것인지 버퍼 내의 오프셋.
// dwByte : 채울 바이트 크기
//------------------------------------------------------------------------------
DWORD LmSound::FillBuffer(LPDIRECTSOUNDBUFFER lpdsBuffer, VOID *pArg, DWORD dwOffset, DWORD dwByte, BOOL IsSeek, DWORD ReadContinue)
{
	THREAD_ARG			*arg = (THREAD_ARG*)pArg;
	HRESULT				hr;
	VOID				*pData;
	DWORD				DataSize;
	DWORD				ReadSize;
	DWORD				Pos;
	DWORD				dwPlayByte;
	
	if (IsSeek){
		dwByte = arg->dsBufferSize;
		arg->dwCurrentPosByte = arg->SeekPos;
		mmioSeek(arg->phmmio, arg->SeekPos, SEEK_SET);
	}

	// 버퍼 유실시 restore 해주는 코드 삽입해야함.
	hr = lpdsBuffer->Lock(dwOffset, dwByte, &pData, &DataSize, NULL, NULL, 0);
	
	// 버퍼에 무음 삽입
	memset(pData, 0, DataSize);
	ReadSize = mmioRead(arg->phmmio, (HPSTR)pData, DataSize);
	lpdsBuffer->Unlock(pData, DataSize, NULL, NULL);	
	arg->dwCurrentPosByte += ReadSize;

	if (IsSeek){
		// IsSeek 상태 일때는 dwOffset에는 0이 들어감.
		lpdsBuffer->SetCurrentPosition(dwOffset);
		arg->dwBufferCycle = arg->SeekPos / arg->dsBufferSize;
	}

	DWORD dwEndOffset;
	dwEndOffset = dwOffset + ReadSize;


	// 파일 끝까지 읽었을 때. 얼마나 더 재생 해야 하는지 계산.
	if (arg->dwTotalByte <= arg->dwCurrentPosByte){
		lpdsBuffer->GetCurrentPosition(&Pos, NULL);
		if (Pos < arg->PrePos){
			if (ReadContinue)
				dwPlayByte = arg->dsBufferSize - Pos + ReadSize;			
			else
				dwPlayByte = dwOffset - Pos + ReadSize;					
		}
		else{
			if (IsSeek){
				dwPlayByte = ReadSize - Pos;
			}
			else
				dwPlayByte = arg->dsBufferSize - Pos + dwOffset + ReadSize;
		}

		// dwPlayByte는 몇 바이트를 더 재생해야 하는지 나타냄.
		Sleep(dwPlayByte / (arg->pWaveFormatex->nBlockAlign * arg->pWaveFormatex->nSamplesPerSec / 1000));
		arg->IsSeek = FALSE;
		return 0;

	}

	arg->IsSeek = FALSE;

	return ReadSize;
}


//------------------------------------------------------------------------------
// 스트림 스레드
// 버퍼의 재생 위치를 검사하여 이미 재생된 부분에 새로운 데이터를 채워 넣음
// parg : 재생관련 변수 구조체의 포인터(THREAD_ARG)
//------------------------------------------------------------------------------
VOID LmSound::StreamThread(VOID *parg)
{
#define SLEEPTIME	50
	
	THREAD_ARG			*arg = (THREAD_ARG*)parg;
	LPDIRECTSOUNDBUFFER	lpdsBuffer = arg->pdsBuffer;
	HMMIO				hmmio = arg->phmmio;

	DWORD			nBufferSize = arg->dsBufferSize;
	DWORD			ReadSize;
	double			dwVolume;
	int				count = 0;
	
	// 재생중에 클래스 소멸 방지.
	ResetEvent(arg->hEvent);

	arg->PrePos = 0;
	
	if (arg->dwFadeIn){
		dwVolume = 0;
		lpdsBuffer->SetVolume(-30*(100 - (LONG)dwVolume));
	}

	DWORD StartTime;
	StartTime = ::GetTickCount();
	
	while(1){

		if (arg->ThreadState == SOUND_STOP){	// 정지 플래그 검사.
			lpdsBuffer->Stop();
			lpdsBuffer->Release();
			lpdsBuffer = NULL;
			ResetEvent(arg->hEvent);
			CloseHandle(arg->hEvent);
			ResetEvent(arg->hEventEndPos);
			CloseHandle(arg->hEventEndPos);
			arg->hEventEndPos = NULL;
			arg->hEvent = NULL;
			*arg->pState = SOUND_STOP;
			break;
		}

		Sleep(SLEEPTIME);
		arg->dwCurrentTime += ::GetTickCount() - StartTime;
	
		// Fade in
		if (arg->dwCurrentTime < arg->dwFadeIn){
			dwVolume += (double)100 / (arg->dwFadeIn / SLEEPTIME);
			lpdsBuffer->SetVolume(-30*(100 - (LONG)dwVolume));

			if ((arg->dwCurrentTime + SLEEPTIME + 5) > arg->dwFadeIn){
				dwVolume = 100;
				lpdsBuffer->SetVolume(-30*(100 - (LONG)dwVolume));
			}
		}

		// Fade out
		if (arg->dwTotalTime < arg->dwCurrentTime + arg->dwFadeOut && arg->dwFadeOut > 0){
			dwVolume -= (double)100 / (arg->dwFadeOut / SLEEPTIME);
			lpdsBuffer->SetVolume(-30*(100 - (LONG)dwVolume));
		}
		
		if (arg->IsSeek){
			count = 5;
			arg->PrePos = 0;
		}

		count++; 
		// 버퍼 채우기
		if (count == 6){			

			lpdsBuffer->GetCurrentPosition(&arg->NowPos, NULL);
			if (arg->IsSeek){			
				arg->PrePos = 0;
				arg->NowPos = 0;
			}
			
			if (arg->NowPos < arg->PrePos){
				if (ReadSize = FillBuffer(lpdsBuffer, parg, arg->PrePos, nBufferSize - arg->PrePos, FALSE, 0))
					FillBuffer(lpdsBuffer, parg, 0, arg->NowPos, FALSE, ReadSize);
				else{
					arg->pdsBuffer->Stop();
					arg->ThreadState = SOUND_STOP;
				}
			}else{

				if (!FillBuffer(lpdsBuffer, parg, arg->PrePos, arg->NowPos - arg->PrePos, arg->IsSeek, 0)){
					arg->pdsBuffer->Stop();
					arg->ThreadState = SOUND_STOP;
				}
			}
			arg->PrePos = arg->NowPos;			
			count = 0;
		}
	}

	SetEvent(arg->hEvent);
}

//------------------------------------------------------------------------------
// DSound의 버퍼를 몇번 반복해서 재생했는지 arg->dwBufferCycle에 저장.
// 버퍼의 끝 부분이 재생 될 때 신호상태가 되어 while 안의 내용이 수행됨.
// parg : 재생관련 변수 구조체의 포인터(THREAD_ARG)
//------------------------------------------------------------------------------
VOID LmSound::NotifyThread(VOID *parg)
{
	THREAD_ARG			*arg = (THREAD_ARG*)parg;

	arg->dwBufferCycle = 0;

	while(1){
		while(WaitForSingleObject(arg->hEventEndPos, 10) == WAIT_TIMEOUT){
			if (arg->ThreadState == SOUND_STOP)
				break;
		}

		if (arg->ThreadState == SOUND_STOP)
			break;		

		if (arg->dwCurrentPosByte >= arg->dwBufferCycle * arg->dsBufferSize){
			arg->dwBufferCycle++;
		}
	}
}