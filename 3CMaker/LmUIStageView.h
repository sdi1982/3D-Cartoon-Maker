#pragma once


// CStageView 뷰입니다.
class LmUISceneView;
class LmUIStageView : public CView
{
	DECLARE_DYNCREATE(LmUIStageView)

//멤버함수
protected:
	LmUIStageView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~LmUIStageView();
	LmUISceneView* GetSceneView();


public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.	
	

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif


//멤버변수
private: 

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt); // 줌인 줌아웃
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);	// 카메라 시작
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);	// 카메라 끝
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);	// 카메라 설정
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


