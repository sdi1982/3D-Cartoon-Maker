#ifndef _LMAVMIXER_H_
#define _LMAVMIXER_H_

#define OPEN_FILE	0
#define MERGE_FILE	1
#define MAXMSG		255
#define MAXNUMSTREAMS   25
#define BUFSIZE 1024

#include <windowsx.h>
#include <vfw.h>

class LmAVMixer
{
public:
	LmAVMixer();
	~LmAVMixer();

	// 파일 초기화
	BOOL		InitAvi(LPCTSTR szFile, int nMenu);

	// 영상과 소리 믹싱
	BOOL		Mixing(LPCTSTR szFile);

	LPCTSTR		GetLastErrorMsg() const{ return m_szErrMsg; }

private:
	VOID		SetErrorMsg(LPCTSTR lpszErrMsg);
	VOID		InitStreams();
	VOID		FreeAvi();
	BOOL		InsertAVIFile(PAVIFILE pfile, LPCTSTR lpszFile);

	int                 m_npStream;                     // 스트림의 갯수

	PAVISTREAM				m_pAVIStream[MAXNUMSTREAMS];
	AVICOMPRESSOPTIONS		m_AVIOptions[MAXNUMSTREAMS];
	LPAVICOMPRESSOPTIONS	m_lpAVIOptions[MAXNUMSTREAMS];

	LPVOID m_lpAudio;

	PAVIFILE    m_pfile[2];

	TCHAR		m_szErrMsg[MAXMSG];			// 에러 메시지.
};



#endif //_LMAVMIXER_H_


