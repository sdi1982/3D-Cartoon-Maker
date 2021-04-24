// 3CMakerDoc.h : CMy3CMakerDoc 클래스의 인터페이스
//


#pragma once

class LmUIMotionThread;
class LmUITimeLineView;
class CMy3CMakerDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CMy3CMakerDoc();
	DECLARE_DYNCREATE(CMy3CMakerDoc)

// 특성입니다.
public:

//	LmUIMotionThread			*m_pMotionThread;		//타임라인 및 전체 UI를 관할하는 Thread임.
	
// 작업입니다.
public:
//	LmUITimeLineView			*m_pTimeLineView;

// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 구현입니다.
public:
	virtual ~CMy3CMakerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
};


