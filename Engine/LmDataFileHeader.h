#if !defined(_LMDATAFILEHEADER_H)
#define _LMDATAFILEHEADER_H

//모든 파일 기록의 기본은 처음과 끝을 필두로 한다. 
//이 클래스는 파일 헤더 4K에 쓰여질 내용이다. 

class LmDataFileHeader
{
public:
	LmDataFileHeader(void) { ClearHeader(); }
	~LmDataFileHeader(void) { }

//멤버 함수
public:
	VOID SetSceneViewStart(UINT _uFilePoint) { m_uSceneViewStart = _uFilePoint; }
	VOID SetSceneViewEnd(UINT _uFilePoint) { m_uSceneViewEnd = _uFilePoint; }

	VOID SetSoundLowDataStart(UINT _uFilePoint) { m_uSoundLowDataStart = _uFilePoint; }
	VOID SetSoundLowDataEnd(UINT _uFilePoint) { m_uSoundLowDataEnd = _uFilePoint; }

	VOID SetSceneListStart(UINT _uFilePoint) { m_uSceneListStart = _uFilePoint; }
	VOID SetSceneListEnd(UINT _uFilePoint) { m_uSceneListEnd = _uFilePoint; }

	UINT GetSceneViewStart() { return m_uSceneViewStart; }
	UINT GetSceneViewEnd() { return m_uSceneViewEnd; }

	UINT GetSoundLowDataStart() { return m_uSoundLowDataStart; }
	UINT GetSoundLowDataEnd() { return m_uSoundLowDataEnd; }

	UINT GetSceneListStart() { return m_uSceneListStart; }
	UINT GetSceneListEnd() { return m_uSceneListEnd; }


	VOID ClearHeader();
	VOID SaveHeader(CFile &_File);
	VOID OpenHeader(CFile &_File);

//멤버 변수
public:
	UINT m_uSceneViewStart;
	UINT m_uSceneViewEnd;

	UINT m_uSceneListStart;
	UINT m_uSceneListEnd;

	UINT m_uSoundLowDataStart;
	UINT m_uSoundLowDataEnd;

};
#endif
