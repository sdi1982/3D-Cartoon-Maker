// 3CMakerDoc.h : CMy3CMakerDoc Ŭ������ �������̽�
//


#pragma once

class LmUIMotionThread;
class LmUITimeLineView;
class CMy3CMakerDoc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	CMy3CMakerDoc();
	DECLARE_DYNCREATE(CMy3CMakerDoc)

// Ư���Դϴ�.
public:

//	LmUIMotionThread			*m_pMotionThread;		//Ÿ�Ӷ��� �� ��ü UI�� �����ϴ� Thread��.
	
// �۾��Դϴ�.
public:
//	LmUITimeLineView			*m_pTimeLineView;

// �������Դϴ�.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// �����Դϴ�.
public:
	virtual ~CMy3CMakerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
};


