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
// ���� ���ۿ� �ִ� ������ �ͽ��Ͽ� ���� ���ۿ� ����
//------------------------------------------------------------------------------
BOOL LmMixer::Mixing()
{
	// ���� ���� ������ ������ ��ġ�� ������ �ͽ� �ϱ� ���ؼ�.
	m_cFrame++;
	if (m_nBufferSlot - 1 > m_cBufferSlot){
		m_cBufferSlot++;
		return TRUE;
	}

	// �ͽ� ���ۿ� ������ ä���
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
	// ���� ���� �˻�
	if (LayerNumber < 0 || LayerNumber > m_nBuffer){
		SetErrorMsg(_T("FillBuffer error"));
		return 0;
	}

	if (cBufferSlot < 0 || cBufferSlot > m_nBufferSlot){
		SetErrorMsg(_T("FillBuffer error"));
		return 0;
	}

	// ���ۿ��� ä���� �������� ����. �о���� ���ü� ����.
	DWORD StartOffset, ReadSample, ReadSize;
	StartOffset = SlotToOffset(cBufferSlot);
	ReadSample = (m_BufferSize - StartOffset) / CHANNEL / (BIT_PER_SAMPLE / 8);		

	if (m_pInputBuffer[LayerNumber].wfx.nSamplesPerSec == 44100){
		ReadSample = ReadSample * 2;
	}

	if (m_pInputBuffer[LayerNumber].wfx.nSamplesPerSec == 11025){
		ReadSample = ReadSample / 2;
	}

	// ���Ͽ��� �ӽ� ���۷� ������ ���� �� ��ŭ ����.
	ReadSize = mmioRead(m_pInputBuffer[LayerNumber].hmmio, (HPSTR)m_pTempBuffer, ReadSample * m_pInputBuffer[LayerNumber].wfx.nBlockAlign);
	
	// ������ ���κб��� �� �о����� ����Ʈ�� �˻�
	m_pInputBuffer[LayerNumber].sum_read_size += ReadSize;	
	if (m_pInputBuffer[LayerNumber].data_chunk_size <= m_pInputBuffer[LayerNumber].sum_read_size){
		ResetInputBuffer(LayerNumber);
	}

	// ���� ��ȯ
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
// �ͽ� ���ۿ� ������ �ֱ�.
//------------------------------------------------------------------------------
BOOL LmMixer::Read()
{
	// �� ���̾��� ���ϸ��� ���� ����Ʈ���� �����Ͽ� �о�� �� �� ����Ʈ���� ���ԵǸ� ���� ����.

	MMRESULT		hr;
	int				ReadSize;
	int				Size;
	
	for (int i = 0; i < m_nLayer; i++){
		
		
		// ���ۿ� �Ҵ�� ������ ���� ���. ������ �о ���ۿ� ä��.
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

			//���ۿ� �Ҵ�� ������ ��� �о��� ��� ������ ����.
			if (m_pInputBuffer[i].data_chunk_size <= m_pInputBuffer[i].sum_read_size){				
				hr = mmioClose(m_pInputBuffer[i].hmmio, NULL);
				m_pInputBuffer[i].hmmio = NULL;
				m_pInputBuffer[i].sum_read_size = 0;
			}

		}else{
			// ���ۿ� �Ҵ�� ������ ���� ���. 0���� ä��.
			memset(m_pInputBuffer[i].pbuffer, 0, m_BufferSize);
		}
	}

	return TRUE;
}



//------------------------------------------------------------------------------
// m_pWriteBuffer�� �ִ� ������ ���Ϸ� ����.
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
// Wave ����� �°� ���� ����. (��� ����������, �����ʹ� Write �Լ����� ����)
//------------------------------------------------------------------------------
BOOL LmMixer::CreateWaveFile(LPTSTR szOutputFileName)
{
	if (!m_pWriteBuffer){
		SetErrorMsg(_T("Write buffer is not exist"));
		return FALSE;
	}

	// wave ���� ����
	WAVEFORMATEX		wfx;
	wfx.cbSize = 0;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = CHANNEL;
	wfx.wBitsPerSample = BIT_PER_SAMPLE;
	wfx.nAvgBytesPerSec = BIT_PER_SAMPLE / 8 * SAMPLE_RATE_OUTPUT * CHANNEL;
	wfx.nSamplesPerSec = SAMPLE_RATE_OUTPUT;
	wfx.nBlockAlign = BIT_PER_SAMPLE / 8 * CHANNEL;

	// ���� ����
	MMRESULT			hr;
	m_pWriteBuffer->hmmio = 
		mmioOpen(szOutputFileName, NULL, MMIO_CREATE | MMIO_WRITE | MMIO_EXCLUSIVE | MMIO_ALLOCBUF);

	memset(&m_WriteRIFF, 0, sizeof(MMCKINFO));
	memset(&m_Writefmt, 0, sizeof(MMCKINFO));
	memset(&m_WriteData, 0, sizeof(MMCKINFO));

	
	// �� ûũ ���� �� ����
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
// ���� �ݱ�
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
// �ʱ�ȭ. �ͽ� ���� �Ҵ�.
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
	// ���� ���� ���� �� ���
	m_nBufferSlot = FramePerSec / 5;

	// ���̾� ���� ���� ������ ��� 0.2�� 2ä�� 16��Ʈ
	m_nLayer = nLayer;
	m_nSampleInBuffer = SAMPLE_RATE_OUTPUT / 10 * 2;
	m_BufferSize = m_nSampleInBuffer * CHANNEL * BIT_PER_SAMPLE / 8;			// 0.2�� ũ���� ����

	// �ͽ� ���� �Ҵ�. ������ ���� = ���̾�� * ���۳��� ���� ��
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


	// ���� ���� ����.
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

	// �ִ� 44100hz �� �Ҹ����� �����ϱ� ����.. �ӽ� ���۴� 2���..
	m_pTempBuffer = new BYTE[m_BufferSize * 2];

	// �ͽ̵� wave�� ����� ���� ����
	if (!CreateWaveFile(szOutputFileName)){
		SetErrorMsg(_T("Create wave file error."));
		return FALSE;
	}
	
	return TRUE;
}



//------------------------------------------------------------------------------
// ���� �޽��� ����.
//------------------------------------------------------------------------------
VOID LmMixer::SetErrorMsg(LPCTSTR lpszErrMsg)
{
	_tcscpy_s(m_szErrMsg, lpszErrMsg);//, MAXIMUM_ERR_MSG);
}



//------------------------------------------------------------------------------
// ����ִ� ���� ��ȣ�� ã��.
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
// �ش� ��ȣ�� ���۸� �ʱ�ȭ
//------------------------------------------------------------------------------
BOOL LmMixer::ResetInputBuffer(int nLayer)
{
	if (!m_pInputBuffer){
		SetErrorMsg(_T("before init"));
		return FALSE;
	}

	// �ڵ��� ������ ����. �̹� ���� �ִ��� �� �ݾƵ� �������.
	if (m_pInputBuffer[nLayer].hmmio){
		mmioClose(m_pInputBuffer[nLayer].hmmio, NULL);
	}

	m_pInputBuffer[nLayer].hmmio = NULL;

	// �ٸ� �κе� 0���� �ʱ�ȭ�� ���ٱ� ����?

	return TRUE;
}



//------------------------------------------------------------------------------
// �ͽ� �ؾ��� ���ο� ������ �߰���.
// ����ִ� ���̾ ã�� ������ �����ϰ� �ڵ��� ���.
//------------------------------------------------------------------------------
BOOL LmMixer::AddFile(LPCTSTR lpszFileName)
{
	MMCKINFO		ckInRIFF, ckIn;
	PCMWAVEFORMAT	pcmWaveFormat;
	MMRESULT		hr;
	int				ReadSize;
	HMMIO			*phmmio;

	int				EmptyLayer;

	// ��� �ִ� ���۸� ã��
	EmptyLayer = GetEmptyLayer();
	if (EmptyLayer < 0){
		SetErrorMsg(_T("Buffer is full"));
		return FALSE;
	}

	// ������ ����
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

	// wave ���� ���
	if (ReadSize = mmioRead(*phmmio, (HPSTR)&pcmWaveFormat, sizeof(pcmWaveFormat))
		!= sizeof(pcmWaveFormat)){
			mmioClose(*phmmio, NULL);
			*phmmio = NULL;
			return FALSE;
	}

	memcpy(&m_pInputBuffer[EmptyLayer].wfx, &pcmWaveFormat, sizeof(pcmWaveFormat));
	m_pInputBuffer[EmptyLayer].wfx.cbSize = 0;		// �߰� ���� ũ�� 0

	// ���� ûũ�� �̵�
	if (mmioAscend(*phmmio, &ckIn, 0)){
		mmioClose(*phmmio, NULL);
		*phmmio = NULL;
		SetErrorMsg(_T("Ascend Chunk err"));
		return FALSE;
	}

	// data ûũ�� �̵�
	ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(*phmmio, &ckIn, &ckInRIFF, MMIO_FINDCHUNK)){
		mmioClose(*phmmio, NULL);
		*phmmio = NULL;
		SetErrorMsg(_T("data Chunk err"));
		return FALSE;
	}

	// ������, ���� ����.
	m_pInputBuffer[EmptyLayer].myframe = m_cFrame;
	m_pInputBuffer[EmptyLayer].inbuffer_slot = m_cFrame % m_nBufferSlot;

	// �о���� ������ ����Ʈ �� ����.
	m_pInputBuffer[EmptyLayer].data_chunk_size = ckIn.cksize;
	m_pInputBuffer[EmptyLayer].sum_read_size = 0;

	m_pInputBuffer[EmptyLayer].readsize = GetReadSize(EmptyLayer);


	// �������� �ʴ� ���� �ϰ��.
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
// �پ��� ���ø��� �� ���ô� ��Ʈ���� ���� �о���� ����Ʈ�� ����
//
// nLayer			:	�Է� ���� ���̾� ��ȣ
// ����				:	����Ʈ ��
//------------------------------------------------------------------------------
int LmMixer::GetReadSize(int nLayer)
{
	if (!m_pInputBuffer){
		SetErrorMsg(_T("�Ѥ�"));
		return FALSE;
	}
	return 
		m_BufferSize * m_pInputBuffer[nLayer].wfx.nChannels / CHANNEL
		* m_pInputBuffer[nLayer].wfx.nSamplesPerSec / SAMPLE_RATE_OUTPUT
		* m_pInputBuffer[nLayer].wfx.wBitsPerSample / BIT_PER_SAMPLE;	
}



//------------------------------------------------------------------------------
// ���� ��ȯ 22050hz 16bit 2channel �� ��ȯ.
// ��ȯ ���� �߿�!
//
// nLayer		:	�Է� ���� ���̾� ��ȣ
// ReadSize		:	m_pTempBuffer�� ��ȿ�� ������ ũ��(���Ͽ��� ���� ������ ũ��)
// ����			:	TRUE/FALSE
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

	// bit ��ȯ
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


	// ä�� ��ȯ
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


	// ���ø��� ��ȯ
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
// 1���� ä���� 2���� ä�η� ��ȯ
// �Է��� ������ �ѻ��ô� 16��Ʈ.
//
// pDst				:	����� �޸� �ּ�
// pSrc				:	��ȯ�� ���� �޸� �ּ�
// Size				:	���� ũ��
// SrcSampleRate	:	���� ä�� ��
// ����				:	TRUE/FALSE
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

	// ä���� 1���̸�.. 2ä�η� ��ȯ
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
// 22050hz ���� ���ø� ���� ��ȯ
// ���� �Է� pcm�� 11025, 44100�� ������
// �Է� �Ǵ� pcm�� 2ä�� 16��Ʈ
//
// pDst				:	����� �޸� �ּ�
// pSrc				:	��ȯ�� ���� �޸� �ּ�
// Size				:	���� ũ��
// SrcSampleRate	:	���� ���ø���
// ����				:	TRUE/FALSE
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
// 8bit wave�� 16bit�� ��ȯ
// ���� �Է� ���ô� ��Ʈ���� 8��Ʈ�� ����.
//
// pDst				:	����� �޸� �ּ�
// pSrc				:	��ȯ�� ���� �޸� �ּ�
// Size				:	���� ũ��
// SrcSampleRate	:	���� ���ô� ��Ʈ��
// ����				:	TRUE/FALSE
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
// SamplePerSec, BitPerSample, Channel �˻�
// ���� ���� ���
// SamplePerSec		:	11025, 22050, 44100
// BitPerSample		:	8, 16
// Channel			:	1, 2
//
// nLayer			:	�Է� ���� ���̾� ��ȣ
// ����				:	TRUE/FALSE
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