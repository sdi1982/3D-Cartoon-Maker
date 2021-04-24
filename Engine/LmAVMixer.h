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

	// ���� �ʱ�ȭ
	BOOL		InitAvi(LPCTSTR szFile, int nMenu);

	// ����� �Ҹ� �ͽ�
	BOOL		Mixing(LPCTSTR szFile);

	LPCTSTR		GetLastErrorMsg() const{ return m_szErrMsg; }

private:
	VOID		SetErrorMsg(LPCTSTR lpszErrMsg);
	VOID		InitStreams();
	VOID		FreeAvi();
	BOOL		InsertAVIFile(PAVIFILE pfile, LPCTSTR lpszFile);

	int                 m_npStream;                     // ��Ʈ���� ����

	PAVISTREAM				m_pAVIStream[MAXNUMSTREAMS];
	AVICOMPRESSOPTIONS		m_AVIOptions[MAXNUMSTREAMS];
	LPAVICOMPRESSOPTIONS	m_lpAVIOptions[MAXNUMSTREAMS];

	LPVOID m_lpAudio;

	PAVIFILE    m_pfile[2];

	TCHAR		m_szErrMsg[MAXMSG];			// ���� �޽���.
};



#endif //_LMAVMIXER_H_


