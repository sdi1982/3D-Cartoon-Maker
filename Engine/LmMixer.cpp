#include "stdafx.h"
#include "LmMixer.h"

LmMixer::LmMixer()
{
	m_pInputBuffer = NULL;
	m_pWriteBuffer = NULL;
	m_pTempBuffer = NULL;
	m_BufferSize = 0;
	m_nLayer = 0;
	m_cBufferSlot = 0;
	m_cFrame = 0;
	SetErrorMsg(_T(""));

}



LmMixer::~LmMixer()
{
	for (int i = 0; i < m_nBuffer; i++){
		if (m_pInputBuffer[i].pbuffer && m_pInputBuffer)
			delete[] m_pInputBuffer[i].pbuffer;
	}

	if (m_pInputBuffer)
		delete[] m_pInputBuffer;

	if (m_pWriteBuffer)
		delete m_pWriteBuffer;

	if (m_pTempBuffer)
		delete[] m_pTempBuffer;
}



//------------------------------------------------------------------------------
// 현재 버퍼에 있는 내용을 믹싱하여 쓰기 버퍼에 저장
//------------------------------------------------------------------------------
BOOL LmMixer::Mixing()
{
	// 버퍼 내부 슬롯의 마지막 위치에 왔을때 믹싱 하기 위해서.
	m_cFrame++;
	if (m_nBufferSlot - 1 > m_cBufferSlot){
		m_cBufferSlot++;
		return TRUE;
	}

	// 믹싱 버퍼에 내용을 채우고
	for (INT i = 0; i < m_nBuffer; i++){
		if (m_pInputBuffer[i].hmmio){
			FillBuffer(m_pInputBuffer[i].inbuffer_slot, i);
			m_pInputBuffer[i].inbuffer_slot = 0;						
		}
		else{
			memset(m_pInputBuffer[i].pbuffer, 0, m_BufferSize);
		}
	}

	
	INT			i, j;	

	memset(m_pWriteBuffer->pbuffer, 0, m_BufferSize);

	INT temp;
	for (i = 0; i < m_BufferSize; i+=2){
		temp = 0;
		for(j = 0; j < m_nBuffer; j++){
			temp += *((short*)(m_pInputBuffer[j].pbuffer + i));
			*((short*)(m_pWriteBuffer->pbuffer + i)) = temp / 2;
		}
	}

	Write();

	m_cBufferSlot = 0;

	return TRUE;		
}

DWORD LmMixer::FillBuffer(INT cBufferSlot, INT LayerNumber)
{
	// 인자 범위 검사
	if (LayerNumber < 0 || LayerNumber > m_nBuffer){
		SetErrorMsg(_T("FillBuffer error"));
		return 0;
	}

	if (cBufferSlot < 0 || cBufferSlot > m_nBufferSlot){
		SetErrorMsg(_T("FillBuffer error"));
		return 0;
	}

	// 버퍼에서 채워질 시작지점 설정. 읽어야할 샘플수 설정.
	DWORD StartOffset, ReadSample, ReadSize;
	StartOffset = SlotToOffset(cBufferSlot);
	ReadSample = (m_BufferSize - StartOffset) / CHANNEL / (BIT_PER_SAMPLE / 8);		

	if (m_pInputBuffer[LayerNumber].wfx.nSamplesPerSec == 44100){
		ReadSample = ReadSample * 2;
	}

	if (m_pInputBuffer[LayerNumber].wfx.nSamplesPerSec == 11025){
		ReadSample = ReadSample / 2;
	}

	// 파일에서 임시 버퍼로 지정된 샘플 수 만큼 읽음.
	ReadSize = mmioRead(m_pInputBuffer[LayerNumber].hmmio, (HPSTR)m_pTempBuffer, ReadSample * m_pInputBuffer[LayerNumber].wfx.nBlockAlign);
	
	// 파일의 끝부분까지 다 읽었는지 바이트수 검사
	m_pInputBuffer[LayerNumber].sum_read_size += ReadSize;	
	if (m_pInputBuffer[LayerNumber].data_chunk_size <= m_pInputBuffer[LayerNumber].sum_read_size){
		ResetInputBuffer(LayerNumber);
	}

	// 포멧 변환
	FormatConverter(LayerNumber, ReadSize);

	memcpy(m_pInputBuffer[LayerNumber].pbuffer + StartOffset, m_pTempBuffer, m_BufferSize - StartOffset);

	return 0;
}


DWORD LmMixer::SlotToOffset(INT SlotNumber)
{
	if (SlotNumber < 0 || SlotNumber > m_nBufferSlot){
		return 0;
	}

	INT SlotSize;
	SlotSize = m_BufferSize / m_nBufferSlot;

	return SlotNumber * SlotSize;
}


//------------------------------------------------------------------------------
// 믹싱 버퍼에 데이터 넣기.
//------------------------------------------------------------------------------
BOOL LmMixer::Read()
{
	// 각 레이어의 파일마다 읽은 바이트수를 누적하여 읽어야 할 총 바이트수와 같게되면 파일 닫음.

	MMRESULT		hr;
	int				ReadSize;
	int				Size;
	
	for (int i = 0; i < m_nLayer; i++){
		
		
		// 버퍼에 할당된 파일이 있을 경우. 파일을 읽어서 버퍼에 채움.
		if (m_pInputBuffer[i].hmmio){
			Size = m_pInputBuffer[i].readsize;
			memset(m_pTempBuffer, 0, m_BufferSize * 2);
			
			if (m_pInputBuffer[i].sum_read_size + Size > m_pInputBuffer[i].data_chunk_size){
				Size = m_pInputBuffer[i].data_chunk_size - m_pInputBuffer[i].sum_read_size;
			}

			ReadSize = mmioRead(m_pInputBuffer[i].hmmio, (HPSTR)m_pTempBuffer, Size);

			m_pInputBuffer[i].sum_read_size += ReadSize;
			FormatConverter(i, ReadSize);
			memset(m_pInputBuffer[i].pbuffer, 0, m_BufferSize);
			memcpy(m_pInputBuffer[i].pbuffer, m_pTempBuffer, m_BufferSize);

			//버퍼에 할당된 파일을 모두 읽었을 경우 파일을 닫음.
			if (m_pInputBuffer[i].data_chunk_size <= m_pInputBuffer[i].sum_read_size){				
				hr = mmioClose(m_pInputBuffer[i].hmmio, NULL);
				m_pInputBuffer[i].hmmio = NULL;
				m_pInputBuffer[i].sum_read_size = 0;
			}

		}else{
			// 버퍼에 할당된 파일이 없을 경우. 0으로 채움.
			memset(m_pInputBuffer[i].pbuffer, 0, m_BufferSize);
		}
	}

	return TRUE;
}



//------------------------------------------------------------------------------
// m_pWriteBuffer에 있는 내용을 파일로 쓰기.
//------------------------------------------------------------------------------
BOOL LmMixer::Write()
{	
	if(!m_pWriteBuffer->hmmio){
		SetErrorMsg(_T("initialization"));
		return FALSE;
	}
	
	if (!mmioWrite(m_pWriteBuffer->hmmio, (const char*)m_pWriteBuffer->pbuffer, m_BufferSize)){
		SetErrorMsg(_T("Wave file write error"));
		return FALSE;
	}

	return TRUE;
}



//------------------------------------------------------------------------------
// Wave 헤더에 맞게 파일 생성. (헤더 생성까지만, 데이터는 Write 함수에서 수행)
//------------------------------------------------------------------------------
BOOL LmMixer::CreateWaveFile(LPTSTR szOutputFileName)
{
	if (!m_pWriteBuffer){
		SetErrorMsg(_T("Write buffer is not exist"));
		return FALSE;
	}

	// wave 정보 설정
	WAVEFORMATEX		wfx;
	wfx.cbSize = 0;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = CHANNEL;
	wfx.wBitsPerSample = BIT_PER_SAMPLE;
	wfx.nAvgBytesPerSec = BIT_PER_SAMPLE / 8 * SAMPLE_RATE_OUTPUT * CHANNEL;
	wfx.nSamplesPerSec = SAMPLE_RATE_OUTPUT;
	wfx.nBlockAlign = BIT_PER_SAMPLE / 8 * CHANNEL;

	// 파일 생성
	MMRESULT			hr;
	m_pWriteBuffer->hmmio = 
		mmioOpen(szOutputFileName, NULL, MMIO_CREATE | MMIO_WRITE | MMIO_EXCLUSIVE | MMIO_ALLOCBUF);

	memset(&m_WriteRIFF, 0, sizeof(MMCKINFO));
	memset(&m_Writefmt, 0, sizeof(MMCKINFO));
	memset(&m_WriteData, 0, sizeof(MMCKINFO));

	
	// 각 청크 생성 및 쓰기
	m_WriteRIFF.fccType=mmioFOURCC('W','A','V','E');
	hr = mmioCreateChunk(m_pWriteBuffer->hmmio, &m_WriteRIFF, MMIO_CREATERIFF);

	if(hr != MMSYSERR_NOERROR)
	{
		SetErrorMsg(_T("Riff format writing error"));
		return FALSE;
	}

	m_Writefmt.ckid=mmioFOURCC('f','m','t',' ');
	m_Writefmt.cksize = sizeof(WAVEFORMATEX)-2;	
	hr = mmioCreateChunk(m_pWriteBuffer->hmmio, &m_Writefmt, 0);

	if(hr != MMSYSERR_NOERROR)
	{
		SetErrorMsg(_T("fmt format writing error"));
		return FALSE;
	}
	
	hr = mmioWrite(m_pWriteBuffer->hmmio, (const char*)&wfx, m_Writefmt.cksize);

	m_WriteData.ckid=mmioFOURCC('d','a','t','a');
	hr = mmioCreateChunk(m_pWriteBuffer->hmmio, &m_WriteData, 0);

	if(hr != MMSYSERR_NOERROR)
	{
		SetErrorMsg(_T("data format writing error"));
		return FALSE;
	}

	return TRUE;
}



//------------------------------------------------------------------------------
// 파일 닫기
//------------------------------------------------------------------------------
BOOL LmMixer::CloseWaveFile()
{
	MMRESULT			hr;
	if(m_pWriteBuffer->hmmio)
	{
		hr = mmioAscend(m_pWriteBuffer->hmmio, &m_WriteData, 0);
		hr = mmioAscend(m_pWriteBuffer->hmmio, &m_WriteRIFF, 0);
		hr = mmioClose(m_pWriteBuffer->hmmio, 0);
	}

	if (hr != MMSYSERR_NOERROR){
		SetErrorMsg(_T("Close file error"));
		return FALSE;
	}

	return TRUE;
}



//------------------------------------------------------------------------------
// 초기화. 믹싱 버퍼 할당.
//------------------------------------------------------------------------------
BOOL LmMixer::Init(int nLayer, int FramePerSec, LPTSTR szOutputFileName)
{
	if (FramePerSec != 5 && FramePerSec != 10 && FramePerSec != 20){
		SetErrorMsg(_T("Not support FPS."));
		return FALSE;
	}
	if (nLayer < 0){
		SetErrorMsg(_T("Invalid number of Layer"));
		return FALSE;
	}
	// 버퍼 내부 슬롯 수 계산
	m_nBufferSlot = FramePerSec / 5;

	// 레이어 수와 버퍼 사이즈 계산 0.2초 2채널 16비트
	m_nLayer = nLayer;
	m_nSampleInBuffer = SAMPLE_RATE_OUTPUT / 10 * 2;
	m_BufferSize = m_nSampleInBuffer * CHANNEL * BIT_PER_SAMPLE / 8;			// 0.2초 크기의 버퍼

	// 믹싱 버퍼 할당. 버퍼의 개수 = 레이어수 * 버퍼내부 슬롯 수
	m_nBuffer = m_nLayer * m_nBufferSlot;
	m_pInputBuffer = new MIXING_BUFFER[m_nBuffer];

	if (!m_pInputBuffer){
		SetErrorMsg(_T("Memory allocation failed"));
		return FALSE;
	}
	
	memset(m_pInputBuffer, 0, m_nBuffer * sizeof(MIXING_BUFFER));

	for (int i = 0; i < m_nBuffer; i++){
		m_pInputBuffer[i].pbuffer = new BYTE[m_BufferSize];		
		if (!m_pInputBuffer[i].pbuffer){
			SetErrorMsg(_T("Memory allocation failed(Input buffer)"));
			return FALSE;
		}
	}


	// 쓰기 버퍼 생성.
	m_pWriteBuffer = new MIXING_BUFFER;
	if (!m_pWriteBuffer){
		SetErrorMsg(_T("Memory allocation failed"));
		return FALSE;
	}
	
	memset(m_pWriteBuffer, 0, sizeof(MIXING_BUFFER));
	
	m_pWriteBuffer->pbuffer = new BYTE[m_BufferSize];
	if (!m_pWriteBuffer->pbuffer){
		SetErrorMsg(_T("Memory allocation failed(Write buffer)"));
		return FALSE;
	}

	// 최대 44100hz 의 소리까지 지원하기 위해.. 임시 버퍼는 2배로..
	m_pTempBuffer = new BYTE[m_BufferSize * 2];

	// 믹싱된 wave가 저장될 파일 생성
	if (!CreateWaveFile(szOutputFileName)){
		SetErrorMsg(_T("Create wave file error."));
		return FALSE;
	}
	
	return TRUE;
}



//------------------------------------------------------------------------------
// 에러 메시지 설정.
//------------------------------------------------------------------------------
VOID LmMixer::SetErrorMsg(LPCTSTR lpszErrMsg)
{
	_tcscpy_s(m_szErrMsg, lpszErrMsg);//, MAXIMUM_ERR_MSG);
}



//------------------------------------------------------------------------------
// 비어있는 버퍼 번호를 찾음.
//------------------------------------------------------------------------------
int LmMixer::GetEmptyLayer()
{
	for (int i = 0; i < m_nBuffer; i++){
		if (!m_pInputBuffer[i].hmmio)
			return i;
	}

	return -1;
}



//------------------------------------------------------------------------------
// 해당 번호의 버퍼를 초기화
//------------------------------------------------------------------------------
BOOL LmMixer::ResetInputBuffer(int nLayer)
{
	if (!m_pInputBuffer){
		SetErrorMsg(_T("before init"));
		return FALSE;
	}

	// 핸들이 있으면 닫음. 이미 닫혀 있더라도 또 닫아도 상관없음.
	if (m_pInputBuffer[nLayer].hmmio){
		mmioClose(m_pInputBuffer[nLayer].hmmio, NULL);
	}

	m_pInputBuffer[nLayer].hmmio = NULL;

	// 다른 부분도 0으로 초기화를 해줄까 말까?

	return TRUE;
}



//------------------------------------------------------------------------------
// 믹싱 해야할 새로운 파일을 추가함.
// 비어있는 레이어를 찾아 파일을 오픈하고 핸들을 등록.
//------------------------------------------------------------------------------
BOOL LmMixer::AddFile(LPCTSTR lpszFileName)
{
	MMCKINFO		ckInRIFF, ckIn;
	PCMWAVEFORMAT	pcmWaveFormat;
	MMRESULT		hr;
	int				ReadSize;
	HMMIO			*phmmio;

	int				EmptyLayer;

	// 비어 있는 버퍼를 찾고
	EmptyLayer = GetEmptyLayer();
	if (EmptyLayer < 0){
		SetErrorMsg(_T("Buffer is full"));
		return FALSE;
	}

	// 파일을 열고
	phmmio = &(m_pInputBuffer[EmptyLayer].hmmio);
	*phmmio = mmioOpen((LPTSTR)lpszFileName, NULL, MMIO_ALLOCBUF | MMIO_READ);

	if (!*phmmio){
		SetErrorMsg(_T("Wave file open error"));
		return FALSE;
	}

	ckInRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	if ((hr = mmioDescend(*phmmio, &ckInRIFF, NULL, MMIO_FINDRIFF))){
		mmioClose(*phmmio, NULL);
		*phmmio = NULL;
		SetErrorMsg(_T("Not found WAVE chunk"));
		return FALSE;
	}

	ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (hr = mmioDescend(*phmmio, &ckIn, &ckInRIFF, MMIO_FINDCHUNK)){
		mmioClose(*phmmio, NULL);
		*phmmio = NULL;
		SetErrorMsg(_T("Not found fmt chunk"));
		return FALSE;
	}

	// wave 정보 얻기
	if (ReadSize = mmioRead(*phmmio, (HPSTR)&pcmWaveFormat, sizeof(pcmWaveFormat))
		!= sizeof(pcmWaveFormat)){
			mmioClose(*phmmio, NULL);
			*phmmio = NULL;
			return FALSE;
	}

	memcpy(&m_pInputBuffer[EmptyLayer].wfx, &pcmWaveFormat, sizeof(pcmWaveFormat));
	m_pInputBuffer[EmptyLayer].wfx.cbSize = 0;		// 추가 정보 크기 0

	// 상위 청크로 이동
	if (mmioAscend(*phmmio, &ckIn, 0)){
		mmioClose(*phmmio, NULL);
		*phmmio = NULL;
		SetErrorMsg(_T("Ascend Chunk err"));
		return FALSE;
	}

	// data 청크로 이동
	ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(*phmmio, &ckIn, &ckInRIFF, MMIO_FINDCHUNK)){
		mmioClose(*phmmio, NULL);
		*phmmio = NULL;
		SetErrorMsg(_T("data Chunk err"));
		return FALSE;
	}

	// 프레임, 슬롯 설정.
	m_pInputBuffer[EmptyLayer].myframe = m_cFrame;
	m_pInputBuffer[EmptyLayer].inbuffer_slot = m_cFrame % m_nBufferSlot;

	// 읽어야할 데이터 바이트 수 설정.
	m_pInputBuffer[EmptyLayer].data_chunk_size = ckIn.cksize;
	m_pInputBuffer[EmptyLayer].sum_read_size = 0;

	m_pInputBuffer[EmptyLayer].readsize = GetReadSize(EmptyLayer);


	// 지원하지 않는 파일 일경우.
	if (!CheckFileFormat(EmptyLayer)){
		mmioClose(*phmmio, NULL);
		*phmmio = NULL;
		ResetInputBuffer(EmptyLayer);
		SetErrorMsg(_T("This wave file is not supported"));
		return FALSE;
	}

	
	return TRUE;

}



//------------------------------------------------------------------------------
// 다양한 샘플링율 및 샘플당 비트수에 따라 읽어야할 바이트수 리턴
//
// nLayer			:	입력 버퍼 레이어 번호
// 리턴				:	바이트 수
//------------------------------------------------------------------------------
int LmMixer::GetReadSize(int nLayer)
{
	if (!m_pInputBuffer){
		SetErrorMsg(_T("ㅡㅡ"));
		return FALSE;
	}
	return 
		m_BufferSize * m_pInputBuffer[nLayer].wfx.nChannels / CHANNEL
		* m_pInputBuffer[nLayer].wfx.nSamplesPerSec / SAMPLE_RATE_OUTPUT
		* m_pInputBuffer[nLayer].wfx.wBitsPerSample / BIT_PER_SAMPLE;	
}



//------------------------------------------------------------------------------
// 포멧 변환 22050hz 16bit 2channel 로 변환.
// 변환 순서 중요!
//
// nLayer		:	입력 버퍼 레이어 번호
// ReadSize		:	m_pTempBuffer에 유효한 데이터 크기(파일에서 읽은 데이터 크기)
// 리턴			:	TRUE/FALSE
//------------------------------------------------------------------------------
BOOL LmMixer::FormatConverter(int nLayer, int ReadSize)
{	
	if (!m_pTempBuffer){
		SetErrorMsg(_T("before init"));
		return FALSE;
	}

	if (ReadSize == 0)
		return TRUE;

	BYTE		*pTempBuffer = NULL;
	int			CurrentDataSize = ReadSize;

	// bit 변환
	if (m_pInputBuffer[nLayer].wfx.wBitsPerSample == 8){
		pTempBuffer = new BYTE[ReadSize * 2];
		memset(pTempBuffer, 0, ReadSize * 2);
		BitPerSampleConverter(pTempBuffer, m_pTempBuffer, ReadSize, m_pInputBuffer[nLayer].wfx.wBitsPerSample);
		memset(m_pTempBuffer, 0, m_BufferSize * 2);
		CurrentDataSize = ReadSize * 2;
		memcpy(m_pTempBuffer, pTempBuffer, CurrentDataSize);

		if (pTempBuffer){
			delete[] pTempBuffer;
			pTempBuffer = NULL;
		}
	}


	// 채널 변환
	if (m_pInputBuffer[nLayer].wfx.nChannels == 1){
		pTempBuffer = new BYTE[CurrentDataSize * 2];
		memset(pTempBuffer, 0, CurrentDataSize * 2);
		ChannelConverter(pTempBuffer, m_pTempBuffer, CurrentDataSize, 1);
		memset(m_pTempBuffer, 0, m_BufferSize * 2);
		CurrentDataSize = CurrentDataSize * 2;
		memcpy(m_pTempBuffer, pTempBuffer, CurrentDataSize);

		if (pTempBuffer){
			delete[] pTempBuffer;
			pTempBuffer = NULL;
		}
	}


	// 샘플링율 변환
	if (m_pInputBuffer[nLayer].wfx.nSamplesPerSec == 44100){
		pTempBuffer = new BYTE[CurrentDataSize / 2];
		memset(pTempBuffer, 0, CurrentDataSize / 2);
		SampleRateConverter(pTempBuffer, m_pTempBuffer, CurrentDataSize, 44100);
		memset(m_pTempBuffer, 0, m_BufferSize * 2);
		CurrentDataSize = CurrentDataSize / 2;
		memcpy(m_pTempBuffer, pTempBuffer, CurrentDataSize);

		if (pTempBuffer){
			delete[] pTempBuffer;
			pTempBuffer = NULL;
		}
	}
	
	if (m_pInputBuffer[nLayer].wfx.nSamplesPerSec == 11025){
		pTempBuffer = new BYTE[CurrentDataSize * 2];
		SampleRateConverter(pTempBuffer, m_pTempBuffer, CurrentDataSize, 11025);
		memset(m_pTempBuffer, 0, m_BufferSize * 2);
		CurrentDataSize = CurrentDataSize * 2;
		memcpy(m_pTempBuffer, pTempBuffer, CurrentDataSize);

		if (pTempBuffer){
			delete[] pTempBuffer;
			pTempBuffer = NULL;
		}
	}

		
	return TRUE;
}



//------------------------------------------------------------------------------
// 1개의 채널을 2개의 채널로 변환
// 입력은 무조건 한샘플당 16비트.
//
// pDst				:	저장될 메모리 주소
// pSrc				:	변환될 원본 메모리 주소
// Size				:	원본 크기
// SrcSampleRate	:	원본 채널 수
// 리턴				:	TRUE/FALSE
//------------------------------------------------------------------------------
BOOL LmMixer::ChannelConverter(BYTE *pDst, BYTE *pSrc, int Size, int SrcChannel)
{
	if (!pDst){
		SetErrorMsg(_T("ChannelConverter error"));
		return FALSE;
	}

	if (!pSrc){
		SetErrorMsg(_T("ChannelConverter error"));
		return FALSE;
	}

	// 채널이 1개이면.. 2채널로 변환
	if (SrcChannel == 1){
		for (int i = 0; i < Size / 2; i++){
			*((short*)pDst) = *((short*)pSrc + i);
			*((short*)pDst + 1) = *((short*)pSrc + i);
			pDst+=4;
		}

	}
	return FALSE;
}



//------------------------------------------------------------------------------
// 22050hz 으로 샘플링 율을 변환
// 현재 입력 pcm은 11025, 44100만 지원함
// 입력 되는 pcm은 2채널 16비트
//
// pDst				:	저장될 메모리 주소
// pSrc				:	변환될 원본 메모리 주소
// Size				:	원본 크기
// SrcSampleRate	:	원본 샘플링율
// 리턴				:	TRUE/FALSE
//------------------------------------------------------------------------------
BOOL LmMixer::SampleRateConverter(BYTE *pDst, BYTE *pSrc, int Size, int SrcSampleRate)
{
	if (!pDst){
		SetErrorMsg(_T("SampleRateConverter error"));
		return FALSE;
	}

	if (!pSrc){
		SetErrorMsg(_T("SampleRateConverter error"));
		return FALSE;
	}

	if (SrcSampleRate == 11025){
		for (int i = 0; i < Size; i+=4){
			if (i > 4350)
				i = i;

			*((UINT*)pDst) = *((UINT*)(pSrc + i));
			*((UINT*)pDst + 1) = *((UINT*)(pSrc + i));
			pDst+=8;
		}
	}	

	if (SrcSampleRate == 44100){
		/*for (int i = 0; i < Size; i+=8){
			*((UINT*)pDst) = *((UINT*)(pSrc + i));
			pDst+=4;
		}*/
		for (INT i = 0; i < Size/8; i++){
			*((UINT*)pDst) = *((UINT*)(pSrc + i * 8));
			pDst+=4;
		}
	}

	if (SrcSampleRate == 22050){

	}
	

	return FALSE;
}



//------------------------------------------------------------------------------
// 8bit wave를 16bit로 변환
// 현재 입력 샘플당 비트수는 8비트만 지원.
//
// pDst				:	저장될 메모리 주소
// pSrc				:	변환될 원본 메모리 주소
// Size				:	원본 크기
// SrcSampleRate	:	원본 샘플당 비트수
// 리턴				:	TRUE/FALSE
//------------------------------------------------------------------------------
BOOL LmMixer::BitPerSampleConverter(BYTE *pDst, BYTE *pSrc, int Size, int SrcBitPerSample)
{
	if (!pDst){
		SetErrorMsg(_T("BitPerSampleConverter error"));
		return FALSE;
	}

	if (!pSrc){
		SetErrorMsg(_T("BitPerSampleConverter error"));
		return FALSE;
	}

	if (SrcBitPerSample == 8){
		for (int i = 0; i < Size; i++){			
			*((short*)pDst + i) = (signed)(*(pSrc + i)) * 127;
		}
		return TRUE;
	}

	if (SrcBitPerSample == 16){
		return TRUE;
	}

	SetErrorMsg(_T("This Bit per sample is not supported"));
	return FALSE;
}



//------------------------------------------------------------------------------
// SamplePerSec, BitPerSample, Channel 검사
// 현재 지원 목록
// SamplePerSec		:	11025, 22050, 44100
// BitPerSample		:	8, 16
// Channel			:	1, 2
//
// nLayer			:	입력 버퍼 레이어 번호
// 리턴				:	TRUE/FALSE
//------------------------------------------------------------------------------
BOOL LmMixer::CheckFileFormat(int nLayer)
{
	if (!m_pInputBuffer){
		SetErrorMsg(_T("before init"));
		return FALSE;
	}

	//if (//m_pInputBuffer[nLayer].wfx.nSamplesPerSec != 11025 &&
	//	m_pInputBuffer[nLayer].wfx.nSamplesPerSec != 44100 &&
	//	m_pInputBuffer[nLayer].wfx.nSamplesPerSec != 22050){		
	//		//SetErrorMsg(_T("This sample rate is not supported"));
	//		return FALSE;
	//}

	//if (m_pInputBuffer[nLayer].wfx.wBitsPerSample != 8 &&
	//	m_pInputBuffer[nLayer].wfx.wBitsPerSample != 16){
	//		//SetErrorMsg(_T("This bit per sample is not supported"));
	//		return FALSE;
	//}

	//if (m_pInputBuffer[nLayer].wfx.nChannels != 1 &&
	//	m_pInputBuffer[nLayer].wfx.nChannels != 2){
	//		//SetErrorMsg(_T("This channel is not supported"));
	//		return FALSE;
	//}

	return CheckWfxFormat(&m_pInputBuffer[nLayer].wfx);

	//return TRUE;
}

BOOL LmMixer::CheckWfxFormat(WAVEFORMATEX *pWfx)
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