#ifndef _LmMixer_H_
#define _LmMixer_H_

#define MAXIMUM_ERR_MSG		255			// 에러 메시지 최대 크기
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

	// 초기화 함수
	BOOL			Init(int nLayer, int FramePerSec, LPTSTR szOutputFileName);
	// 파일 추가
	BOOL			AddFile(LPCTSTR lpszFileName);

	// 읽고 믹싱
	BOOL			Read();	
	BOOL			Mixing();

	// 파일 닫기.
	BOOL			CloseWaveFile();
	

	// 마지막 에러 메시지 리턴
	LPCTSTR			GetLastError() const { return m_szErrMsg; }


private:
	DWORD			m_cFrame;

	// 버퍼
	INT				m_BufferSize;
	INT				m_nLayer;
	MIXING_BUFFER	*m_pInputBuffer;
	MIXING_BUFFER	*m_pWriteBuffer;
	BYTE			*m_pTempBuffer;
	INT				m_nBuffer;
	INT				m_nBufferSlot;		// 버퍼 내부 슬롯 수
	INT				m_cBufferSlot;		// 버퍼 내부 슬롯 카운터
	DWORD			m_nSampleInBuffer;	// 버퍼 내부에 들어가는 샘플 수

	
	TCHAR			m_szErrMsg[MAXIMUM_ERR_MSG];

	
	// wave 청크 관련 변수
	MMCKINFO		m_WriteRIFF;
	MMCKINFO		m_Writefmt;
	MMCKINFO		m_WriteData;

	// bit 및 sample rate가 다른 wave 에서 읽어야 할 크기 리턴
	INT				GetReadSize(int nLayer);

	// 서로 다른 포멧을 하나로 통일.. 22050, 16, stereo
	BOOL			FormatConverter(int nLayer, int ReadSize);
	
	// nLayer번호를 가지는 입력 버퍼를 초기화
	BOOL			ResetInputBuffer(int nLayer);
	
	// 파일의 wfx 정보를 검사.
	BOOL			CheckFileFormat(int nLayer);
	BOOL			CheckWfxFormat(WAVEFORMATEX *pWfx);

	DWORD			FillBuffer(INT cBufferSlot, INT LayerNumber);
	DWORD			SlotToOffset(INT SlotNumber);

	// 포멧 변환
	BOOL			SampleRateConverter(BYTE *pDst, BYTE *pSrc, int Size, int SrcSampleRate);
	BOOL			BitPerSampleConverter(BYTE *pDst, BYTE *pSrc, int Size, int SrcBitPerSample);
	BOOL			ChannelConverter(BYTE *pDst, BYTE *pSrc, int Size, int SrcChannel);

	
	BOOL			Write();
	
	// 최종 생성될 파일 생성
	BOOL			CreateWaveFile(LPTSTR szOutputFileName);	
	// 비어있는 입력 버퍼 번호를 리턴
	int				GetEmptyLayer();


	// 에러 메시지 설정
	VOID			SetErrorMsg(LPCTSTR lpszErrMsg);




};


#endif //_LmMixer_H_
