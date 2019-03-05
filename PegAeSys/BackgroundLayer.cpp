#include "stdafx.h"

#include "StateInfoFrm.h"
#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

CBackgroundLayer::CBackgroundLayer(int x, int y, int width, int height)
{
	SetInfo(x, y, width, height);
}

void CBackgroundLayer::OnPaint(HWND hwnd)
{
	CPegView *pView = CPegView::GetActiveView();

	CRect rcUpdRect;
	if(::GetUpdateRect(hwnd, &rcUpdRect, TRUE) != 0) 
	{
//		CDC* pDC = pView->GetDC();
		CDC* pDC = GetDC();
		pView->BackgroundImageDisplay(pDC);
	}

   ::ValidateRect(hwnd, 0);
}

void CBackgroundLayer::OnSize(HWND hwnd, UINT, int, int)
{
	RECT cRect;
	::GetClientRect(app.GetSafeHwnd(), &cRect);
	
	POINT point;
	point.x = cRect.left;
	point.y = cRect.bottom - m_nHeight;

	m_nX = point.x;
	m_nY = point.y;
	m_nWidth = cRect.right;
	ClientToScreen(app.GetSafeHwnd(), &point);
	SetWindowPos(hwnd, NULL, point.x, point.y, m_nWidth, m_nHeight, NULL);
}

CBackgroundLayer BackgroundLayer(0, 0, 500, 500);

LRESULT CALLBACK WndProcBackgroundLayer(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg) 
	{
		HANDLE_MSG(hwnd, WM_PAINT, BackgroundLayer.OnPaint);
		HANDLE_MSG(hwnd, WM_MOVE, BackgroundLayer.OnMove);
		HANDLE_MSG(hwnd, WM_SIZE, BackgroundLayer.OnSize);
	}

	return (::DefWindowProc(hwnd, nMsg, wParam, lParam));
}