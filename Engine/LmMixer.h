#ifndef _LmMixer_H_
#define _LmMixer_H_

#define MAXIMUM_ERR_MSG		255			// ���� �޽��� �ִ� ũ��
#define BIT_PER_SAMPLE		16
#define SAMPLE_RATE_OUTPUT	22050
#define CHANNEL				2

#include <Windows.h>
#include <mmsystem.h>

class LmMixer{
	struct MIXING_BUFFER{
		BYTE			*pbuffer;
		HMMIO			hmmio;
		DWORD			data_chunk_size;
		DWORD			sum_read_size;
		DWORD			readsize;
		DWORD			inbuffer_slot;
		DWORD			myframe;
		DWORD			fade_in;
		DWORD			fade_out;
		WAVEFORMATEX	wfx;		
	};

public:
	LmMixer();
	~LmMixer();

	// �ʱ�ȭ �Լ�
	BOOL			Init(int nLayer, int FramePerSec, LPTSTR szOutputFileName);
	// ���� �߰�
	BOOL			AddFile(LPCTSTR lpszFileName);

	// �а� �ͽ�
	BOOL			Read();	
	BOOL			Mixing();

	// ���� �ݱ�.
	BOOL			CloseWaveFile();
	

	// ������ ���� �޽��� ����
	LPCTSTR			GetLastError() const { return m_szErrMsg; }


private:
	DWORD			m_cFrame;

	// ����
	INT				m_BufferSize;
	INT				m_nLayer;
	MIXING_BUFFER	*m_pInputBuffer;
	MIXING_BUFFER	*m_pWriteBuffer;
	BYTE			*m_pTempBuffer;
	INT				m_nBuffer;
	INT				m_nBufferSlot;		// ���� ���� ���� ��
	INT				m_cBufferSlot;		// ���� ���� ���� ī����
	DWORD			m_nSampleInBuffer;	// ���� ���ο� ���� ���� ��

	
	TCHAR			m_szErrMsg[MAXIMUM_ERR_MSG];

	
	// wave ûũ ���� ����
	MMCKINFO		m_WriteRIFF;
	MMCKINFO		m_Writefmt;
	MMCKINFO		m_WriteData;

	// bit �� sample rate�� �ٸ� wave ���� �о�� �� ũ�� ����
	INT				GetReadSize(int nLayer);

	// ���� �ٸ� ������ �ϳ��� ����.. 22050, 16, stereo
	BOOL			FormatConverter(int nLayer, int ReadSize);
	
	// nLayer��ȣ�� ������ �Է� ���۸� �ʱ�ȭ
	BOOL			ResetInputBuffer(int nLayer);
	
	// ������ wfx ������ �˻�.
	BOOL			CheckFileFormat(int nLayer);
	BOOL			CheckWfxFormat(WAVEFORMATEX *pWfx);

	DWORD			FillBuffer(INT cBufferSlot, INT LayerNumber);
	DWORD			SlotToOffset(INT SlotNumber);

	// ���� ��ȯ
	BOOL			SampleRateConverter(BYTE *pDst, BYTE *pSrc, int Size, int SrcSampleRate);
	BOOL			BitPerSampleConverter(BYTE *pDst, BYTE *pSrc, int Size, int SrcBitPerSample);
	BOOL			ChannelConverter(BYTE *pDst, BYTE *pSrc, int Size, int SrcChannel);

	
	BOOL			Write();
	
	// ���� ������ ���� ����
	BOOL			CreateWaveFile(LPTSTR szOutputFileName);	
	// ����ִ� �Է� ���� ��ȣ�� ����
	int				GetEmptyLayer();


	// ���� �޽��� ����
	VOID			SetErrorMsg(LPCTSTR lpszErrMsg);




};


#endif //_LmMixer_H_
