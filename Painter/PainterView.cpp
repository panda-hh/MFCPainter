// PainterView.cpp: CPainterView 클래스의 구현
//
#pragma once
#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Painter.h"
#endif

#include "PainterDoc.h"
#include "PainterView.h"
#include "MainFrm.h"
#include "windows.h"
#include "Windows.h"
#include "iostream"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPainterView

IMPLEMENT_DYNCREATE(CPainterView, CView)

BEGIN_MESSAGE_MAP(CPainterView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEHWHEEL()
	ON_WM_SIZING()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CPainterView 생성/소멸

CPainterView::CPainterView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_bMouseDragging = false;
	m_bStartDrawing = false;
	m_bPanning = false;
	m_bViewResize = true;
	m_f64Scale = 1.0;
	m_f64Limitzoom = 1.0;
}

CPainterView::~CPainterView()
{
}

BOOL CPainterView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

CDoublePoint CPainterView::ConvertRealToScreen(const CDoublePoint& dpt)//그리는거
{
	return CDoublePoint(dpt.m_f64X * m_f64Scale + m_ptOffset.m_f64X, dpt.m_f64Y * m_f64Scale + m_ptOffset.m_f64Y);
}

CDoublePoint CPainterView::ConvertScreenToReal(const CDoublePoint& dpt, double* pF64Scale)//저장하는거
{
	if(!pF64Scale)
		return CDoublePoint((dpt.m_f64X - m_ptOffset.m_f64X) / m_f64Scale, (dpt.m_f64Y - m_ptOffset.m_f64Y) / m_f64Scale);
	else
		return CDoublePoint((dpt.m_f64Y - m_ptOffset.m_f64X) / *pF64Scale, (dpt.m_f64X - m_ptOffset.m_f64Y) / *pF64Scale);
}

void CPainterView::OnDraw(CDC* pDC)
{
	do
	{
		CPaintDC dc(this);
		CRect rtrect;

		GetClientRect(&rtrect);

		CDC dcMemDC;
		CBitmap bmpOrigin;
		CBitmap* pBmpOld = nullptr;

		dcMemDC.CreateCompatibleDC(pDC);
		bmpOrigin.CreateCompatibleBitmap(pDC, rtrect.Width(), rtrect.Height());
		pBmpOld = (CBitmap*)dcMemDC.SelectObject(&bmpOrigin);
		dcMemDC.PatBlt(0, 0, rtrect.Width(), rtrect.Height(), WHITENESS);

		for (auto& iter : m_vctLinePoints)
		{
			if (iter.size() > 1)
			{
				for (int32_t i = 0; i < iter.size() - 2; i++)
				{
					CDoublePoint ptScreenStart = ConvertRealToScreen(iter[i]);
					CDoublePoint ptScreenEnd = ConvertRealToScreen(iter[i + 1]);

					if ((ptScreenStart.m_f64X < 0 && ptScreenEnd.m_f64X < 0) ||
						(ptScreenStart.m_f64X > rtrect.Width() && ptScreenEnd.m_f64X > rtrect.Width()) ||
						(ptScreenStart.m_f64Y < 0 && ptScreenEnd.m_f64Y < 0) ||
						(ptScreenStart.m_f64Y > rtrect.Height() && ptScreenEnd.m_f64Y > rtrect.Height()))//둘다 밖에 있을 경우
						continue;

					bool bStartOut = (ptScreenStart.m_f64X < 0 || ptScreenStart.m_f64X > rtrect.Width() ||
						ptScreenStart.m_f64Y < 0 || ptScreenStart.m_f64Y > rtrect.Height());
					bool bEndOut = (ptScreenEnd.m_f64X < 0 || ptScreenEnd.m_f64X > rtrect.Width() ||
						ptScreenEnd.m_f64Y < 0 || ptScreenEnd.m_f64Y > rtrect.Height());

					if (bStartOut || bEndOut)//둘중에 하나라도 밖에 있는 경우
					{
						double f64Slope = (ptScreenEnd.m_f64Y - ptScreenStart.m_f64Y) / (ptScreenEnd.m_f64X - ptScreenStart.m_f64X);
						double f64Intercept = ptScreenStart.m_f64Y - f64Slope * ptScreenStart.m_f64X;
						double f64SlopeverX = (ptScreenEnd.m_f64X - ptScreenStart.m_f64X) / (ptScreenEnd.m_f64Y - ptScreenStart.m_f64Y) ;
						double f64InterceptverX = ptScreenStart.m_f64X - f64SlopeverX * ptScreenStart.m_f64Y;

						if (bStartOut)//처음이 나가있는 경우
						{
							if (ptScreenStart.m_f64X < 0)//왼쪽으로 나가있는 경우
							{
								ptScreenStart.m_f64X = 0;
								ptScreenStart.m_f64Y = f64Intercept;
							}
							else if (ptScreenStart.m_f64X > rtrect.Width())//오르쪽으로 나가있는경우
							{
								ptScreenStart.m_f64X = rtrect.Width();
								ptScreenStart.m_f64Y = f64Slope * rtrect.Width() + f64Intercept;
							}
							else if (ptScreenStart.m_f64Y < 0)//위로 나가있는 경우
							{
								ptScreenStart.m_f64Y = 0;
								ptScreenStart.m_f64X = f64InterceptverX;
							}
							else if (ptScreenStart.m_f64Y > rtrect.Height())//아래로 나가있는 경우
							{
								ptScreenStart.m_f64Y = rtrect.Height();
								ptScreenStart.m_f64X = rtrect.Height()*f64SlopeverX+f64InterceptverX;
							}
						}

						if (bEndOut)//끝점이 나가있는 경우
						{
							if (ptScreenEnd.m_f64X < 0)//왼쪽으로 나가있는 경우
							{
								ptScreenEnd.m_f64X = 0;
								ptScreenEnd.m_f64Y = f64Intercept;
							}
							else if (ptScreenEnd.m_f64X > rtrect.Width())//오르쪽으로 나가있는경우
							{
								ptScreenEnd.m_f64X = rtrect.Width();
								ptScreenEnd.m_f64Y = f64Slope * rtrect.Width() + f64Intercept;
							}
							else if (ptScreenEnd.m_f64Y < 0)//위로 나가있는 경우
							{
								ptScreenEnd.m_f64Y = 0;
								ptScreenEnd.m_f64X = f64InterceptverX;
							}
							else if (ptScreenEnd.m_f64Y > rtrect.Height())//아래로 나가있는 경우
							{
								ptScreenEnd.m_f64Y = rtrect.Height();
								ptScreenEnd.m_f64X = rtrect.Height()*f64SlopeverX+f64InterceptverX;
							}

						}
					}

					dcMemDC.MoveTo((int32_t)ptScreenStart.m_f64X, (int32_t)ptScreenStart.m_f64Y);
					dcMemDC.LineTo((int32_t)ptScreenEnd.m_f64X, (int32_t)ptScreenEnd.m_f64Y);
				}
			}
		}

		pDC->BitBlt(0, 0, rtrect.Width(), rtrect.Height(), &dcMemDC, 0, 0, SRCCOPY);

		dcMemDC.SelectObject(pBmpOld);
		dcMemDC.DeleteDC();
		bmpOrigin.DeleteObject();
	}
	while(false);
}

void CPainterView::OnLButtonDown(UINT nFlags, CPoint point)
{
	do
	{
		m_bMouseDragging = true;

		m_vctLinePoints.emplace_back();

		if(!m_bStartDrawing)
		{
			CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());

			if(pMainFrame)
			{
				CString strcurrentTitle;

				pMainFrame->GetWindowText(strcurrentTitle);
				strcurrentTitle = _T("*") + strcurrentTitle;
				pMainFrame->SetWindowText(strcurrentTitle);
			}

			m_bStartDrawing = true;
		}

	}
	while(false);

	CView::OnLButtonDown(nFlags, point);
}

void CPainterView::OnLButtonUp(UINT nFlags, CPoint point)
{
	do
	{
		m_bMouseDragging = false;

		if(GetCapture() == this)
		{
			ReleaseCapture();
			Invalidate(false);
		}
	}
	while(false);

}

void CPainterView::OnMouseMove(UINT nFlags, CPoint point)
{
	do
	{
		if(m_bPanning)
		{
			SetCapture();

			m_ptOffset.m_f64X -= m_ptStartoffset.x - point.x;
			m_ptOffset.m_f64Y -= m_ptStartoffset.y - point.y;
			m_ptStartoffset = point;
		}
		else if(m_bMouseDragging)
		{
			SetCapture();
			m_vctLinePoints.rbegin()->emplace_back((point.x - m_ptOffset.m_f64X) / m_f64Scale, (point.y - m_ptOffset.m_f64Y) / m_f64Scale);
		}

		Invalidate(false);
	}
	while(false);
}

void CPainterView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	do
	{
		m_ptStartoffset = point;
		m_bPanning = true;
	}
	while(false);

	CView::OnMButtonDown(nFlags, point);
}

void CPainterView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	ReleaseCapture();
	m_bPanning = false;

	CView::OnMButtonUp(nFlags, point);
}

BOOL CPainterView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	do
	{
		CPoint ptClient(pt);
		ScreenToClient(&ptClient);

		double m_f64ZoomFactor = m_f64Scale;

		if(zDelta > 0 && m_f64Limitzoom < 100000)
		{
			m_f64ZoomFactor *= 1.5;
			m_f64Limitzoom *= 1.5;
		}
		else if(zDelta < 0 && 0.00001 < m_f64Limitzoom)
		{
			m_f64ZoomFactor *= 0.75;
			m_f64Limitzoom *= 0.75;
		}

		if(m_f64Limitzoom > 100000)
		{
			m_f64ZoomFactor = 100000;
			m_f64Limitzoom = 100000;
		}
		else if(m_f64Limitzoom < 0.00001)
		{
			m_f64ZoomFactor = 0.00001;
			m_f64Limitzoom = 0.00001;
		}

		CDoublePoint dpt1 = ConvertScreenToReal(CDoublePoint(ptClient.x, ptClient.y));
		double f64Dx = (dpt1.m_f64X * m_f64ZoomFactor + m_ptOffset.m_f64X) - (dpt1.m_f64X * m_f64Scale + m_ptOffset.m_f64X);
		double f64Dy = (dpt1.m_f64Y * m_f64ZoomFactor + m_ptOffset.m_f64Y) - (dpt1.m_f64Y * m_f64Scale + m_ptOffset.m_f64Y);
		m_f64Scale = m_f64ZoomFactor;
		m_ptOffset.m_f64X -= f64Dx;
		m_ptOffset.m_f64Y -= f64Dy;

		Invalidate(false);
	}
	while(false);

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

// CPainterView 인쇄
BOOL CPainterView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CPainterView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CPainterView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

// CPainterView 진단
#ifdef _DEBUG
void CPainterView::AssertValid() const
{
	CView::AssertValid();
}

void CPainterView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPainterDoc* CPainterView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPainterDoc)));
	return (CPainterDoc*)m_pDocument;
}
#endif //_DEBUG

void CPainterView::OnSize(UINT nType, int cx, int cy)
{
	do
	{
		CView::OnSize(nType, cx, cy);

		CRect rtrec;

		GetWindowRect(&rtrec);

		if(m_bViewResize)
		{
			m_f64WindowChangex = rtrec.Width();
			m_f64WindowChangey = rtrec.Height();
			m_bViewResize = false;
		}

		double f64Windowwidth = rtrec.Width();
		double f64Windowheight = rtrec.Height();
		m_ptOffset.m_f64X += (cx - m_f64WindowChangex) / 2;
		m_ptOffset.m_f64Y += (cy - m_f64WindowChangey) / 2;
		m_f64WindowChangex = cx;
		m_f64WindowChangey = cy;
	}
	while(false);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
