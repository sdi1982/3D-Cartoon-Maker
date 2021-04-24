#pragma once


// CStageView ���Դϴ�.
class LmUISceneView;
class LmUIStageView : public CView
{
	DECLARE_DYNCREATE(LmUIStageView)

//����Լ�
protected:
	LmUIStageView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~LmUIStageView();
	LmUISceneView* GetSceneView();


public:
	virtual void OnDraw(CDC* pDC);      // �� �並 �׸��� ���� �����ǵǾ����ϴ�.	
	

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif


//�������
private: 

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt); // ���� �ܾƿ�
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);	// ī�޶� ����
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);	// ī�޶� ��
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);	// ī�޶� ����
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


