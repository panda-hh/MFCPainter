
// PainterView.h: CPainterView 클래스의 인터페이스
//
#pragma once

#include "vector"
#include "DoublePoint.h"
#include "string"


class CPainterView : public CView
{

protected: // serialization에서만 만들어집니다.
	CPainterView() noexcept;
	DECLARE_DYNCREATE(CPainterView)

// 특성입니다.
public:
	CPainterDoc* GetDocument() const;

// 작업입니다.
public:
	

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	
	
// 구현입니다.
public:     
	virtual ~CPainterView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	std::vector <std::vector<CDoublePoint>> m_vctLinePoints;
	CDoublePoint m_ptOffset;
	CPoint m_ptStartoffset;
	bool m_bMouseDragging;
	bool m_bStartDrawing;
	bool m_bPanning;
	bool m_bViewResize;
	double m_f64Scale;
	double m_f64Limitzoom;
	double m_f64WindowChangex ;
	double m_f64WindowChangey ;
	double m_f64Windowwidth;
	double m_f64Windowheight ;

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	CDoublePoint ConvertRealToScreen(const CDoublePoint& dpt);
	CDoublePoint ConvertScreenToReal(const CDoublePoint& dpt, double* pF64Scale = nullptr);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // PainterView.cpp의 디버그 버전
inline CPainterDoc* CPainterView::GetDocument() const
   { return reinterpret_cast<CPainterDoc*>(m_pDocument); }
#endif

