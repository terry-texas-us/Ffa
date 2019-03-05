#include "stdafx.h"

#include "DlgProcViewZoom.h"

CPoint	pnt;
bool	bPtInRect = FALSE;

void WndProcKeyPlanOnDraw(HWND);
void WndProcKeyPlanOnMouseMove(HWND, UINT, LPARAM);
void WndProcKeyPlanOnNewRatio(HWND, LPARAM);

LRESULT CALLBACK WndProcKeyPlan(HWND hwnd, UINT nMsg, UINT nParam, LPARAM lParam)
{
	switch (nMsg)
	{
		case WM_USER_ON_NEW_RATIO:
			WndProcKeyPlanOnNewRatio(hwnd, lParam);
			break;
					
		case WM_PAINT:
			WndProcKeyPlanOnDraw(hwnd);
			return (FALSE);

		case WM_LBUTTONDOWN:
			SetFocus(hwnd);
			pnt.x = LOWORD(lParam); pnt.y = HIWORD(lParam);
			bPtInRect = ::PtInRect(&CDlgViewZoom::m_rcWnd, pnt) != 0;
			return (FALSE);
		
		case WM_MOUSEMOVE:
			WndProcKeyPlanOnMouseMove(hwnd, nParam, lParam);
			return (FALSE);
	}
	return DefWindowProc(hwnd, nMsg, nParam, lParam);
}

void WndProcKeyPlanOnDraw(HWND hwnd)
{
	PAINTSTRUCT ps;
	
	CDC dc;
	dc.Attach(::BeginPaint(hwnd, &ps));

	CDC dcMem;
	dcMem.CreateCompatibleDC(NULL);
				
	dcMem.SelectObject(CBitmap::FromHandle(CDlgViewZoom::m_hbmKeyplan));
	BITMAP bitmap; 
	::GetObject(CDlgViewZoom::m_hbmKeyplan, sizeof(BITMAP), (LPSTR) &bitmap);

	dc.BitBlt(0, 0, bitmap.bmWidth, bitmap.bmHeight, &dcMem, 0, 0, SRCCOPY);
				
	CBrush* pBrush = (CBrush*) dc.SelectStockObject(NULL_BRUSH);
	dc.Rectangle(0, 0, bitmap.bmWidth, bitmap.bmHeight);

	CPnt ptTarget = view::GetTarget();
	double dUMin = ptTarget[0] + view::GetUMin();
	double dUMax = ptTarget[0] + view::GetUMax();
	double dVMin = ptTarget[1] + view::GetVMin();
	double dVMax = ptTarget[1] + view::GetVMax();
	double dUMinOverview = ptTarget[0] + vOver.GetUMin();
	double dVMinOverview = ptTarget[1] + vOver.GetVMin();
	
	CRect rc;
	rc.left = Round((dUMin - dUMinOverview) / vOver.GetUExt() * bitmap.bmWidth);
	rc.right = Round((dUMax - dUMinOverview) / vOver.GetUExt() * bitmap.bmWidth);
	rc.top = Round((1. - (dVMax - dVMinOverview) / vOver.GetVExt()) * bitmap.bmHeight);
	rc.bottom = Round((1. - (dVMin - dVMinOverview) / vOver.GetVExt()) * bitmap.bmHeight);
				
	int iDrawMode = dc.SetROP2(R2_XORPEN);

	// Show current window as light gray rectangle with no outline
	dc.SelectStockObject(LTGRAY_BRUSH);
	CPen* pPen = (CPen*) dc.SelectStockObject(NULL_PEN);
	dc.Rectangle(rc);
				
	// Show defining window as hollow rectangle with dark gray outline
	dc.SelectStockObject(NULL_BRUSH);
	CPen penGray(PS_SOLID, 2, RGB(0x80, 0x80, 0x80));
	dc.SelectObject(&penGray);
	dc.Rectangle(CDlgViewZoom::m_rcWnd);
				
	// Restore device context
	dc.SelectObject(pPen);
	dc.SelectObject(pBrush);
	dc.SetROP2(iDrawMode);
	
	dc.Detach();

	::EndPaint(hwnd, &ps);
}

void WndProcKeyPlanOnMouseMove(HWND hwnd, UINT nParam, LPARAM lParam)
{
	if (LOWORD(nParam) == MK_LBUTTON)
	{			
		CPoint pntCur;
				
		pntCur.x = LOWORD(lParam); 
		pntCur.y = HIWORD(lParam);
				
		CDC dcKeyPlan;
		dcKeyPlan.Attach(::GetDC(hwnd));
		int iDrawMode = dcKeyPlan.SetROP2(R2_XORPEN);
		
		// Show defining window as hollow rectangle with dark gray outline
		CBrush* pBrush = (CBrush*) dcKeyPlan.SelectStockObject(NULL_BRUSH);
		CPen penGray(PS_SOLID, 2, RGB(0x80, 0x80, 0x80));
		CPen* pPen = dcKeyPlan.SelectObject(&penGray);
		dcKeyPlan.Rectangle(CDlgViewZoom::m_rcWnd);
			
		if (bPtInRect)
		{
			::OffsetRect(&CDlgViewZoom::m_rcWnd, (pntCur.x - pnt.x), (pntCur.y - pnt.y));
		}
		else
		{
			if (pntCur.x > CDlgViewZoom::m_rcWnd.right)
				CDlgViewZoom::m_rcWnd.right += (pntCur.x - pnt.x);
			else if (pntCur.x < CDlgViewZoom::m_rcWnd.left)
				CDlgViewZoom::m_rcWnd.left += (pntCur.x - pnt.x);
			if (pntCur.y > CDlgViewZoom::m_rcWnd.bottom)
				CDlgViewZoom::m_rcWnd.bottom += (pntCur.y - pnt.y);
			else if (pntCur.y < CDlgViewZoom::m_rcWnd.top)
				CDlgViewZoom::m_rcWnd.top += (pntCur.y - pnt.y);
			::SendMessage(::GetParent(hwnd), WM_COMMAND, (WPARAM) ::GetWindowLong(hwnd, GWL_ID), (LPARAM) hwnd);
		}
		pnt = pntCur;
		dcKeyPlan.Rectangle(CDlgViewZoom::m_rcWnd);
		
		dcKeyPlan.SelectObject(pPen);
		dcKeyPlan.SelectObject(pBrush);
		dcKeyPlan.SetROP2(iDrawMode);
		
		dcKeyPlan.Detach();
	}			
}			

void WndProcKeyPlanOnNewRatio(HWND hwnd, LPARAM lParam)
{
	double dRatio = *(double*) (LPDWORD) lParam;
				
	CPnt ptTarget = view::GetTarget();
	
	double dUExt = viewport::GetWidthInInches() / dRatio;
	double dUMin = ptTarget[0] - (dUExt * .5);
	double dUMax = dUMin + dUExt;
	double dVExt = viewport::GetHeightInInches() / dRatio;
	double dVMin = ptTarget[1] - (dVExt * .5);
	double dVMax = dVMin + dVExt;
				
	CDC dcKeyPlan;
	dcKeyPlan.Attach(::GetDC(hwnd));
	int iDrawMode = dcKeyPlan.SetROP2(R2_XORPEN);

	CBrush* pBrush = (CBrush*) dcKeyPlan.SelectStockObject(NULL_BRUSH);
	CPen penGray(PS_SOLID, 2, RGB(0x80, 0x80, 0x80));
	CPen* pPen = dcKeyPlan.SelectObject(&penGray);
	dcKeyPlan.Rectangle(CDlgViewZoom::m_rcWnd);
				
	CDC dcMem;
	dcMem.CreateCompatibleDC(NULL);
				
	dcMem.SelectObject(CBitmap::FromHandle(CDlgViewZoom::m_hbmKeyplan));
	BITMAP bitmap; 
	::GetObject(CDlgViewZoom::m_hbmKeyplan, sizeof(BITMAP), (LPSTR) &bitmap);
	
	double dUMinOverview = ptTarget[0] + vOver.GetUMin();
	double dVMinOverview = ptTarget[1] + vOver.GetVMin();

	CDlgViewZoom::m_rcWnd.left = Round((dUMin - dUMinOverview) / vOver.GetUExt() * bitmap.bmWidth);
	CDlgViewZoom::m_rcWnd.top = Round((1. - (dVMax - dVMinOverview) / vOver.GetVExt()) * bitmap.bmHeight);
	CDlgViewZoom::m_rcWnd.right = Round((dUMax - dUMinOverview) / vOver.GetUExt() * bitmap.bmWidth);
	CDlgViewZoom::m_rcWnd.bottom = Round((1. - (dVMin - dVMinOverview) / vOver.GetVExt()) * bitmap.bmHeight);
			
	dcKeyPlan.Rectangle(CDlgViewZoom::m_rcWnd);
	dcKeyPlan.SelectObject(pPen);
	dcKeyPlan.SelectObject(pBrush);
	
	dcKeyPlan.SetROP2(iDrawMode);
	dcKeyPlan.Detach();
}