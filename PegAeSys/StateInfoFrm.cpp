#include "stdafx.h"

#include "StateInfoFrm.h"
#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

BOOL MakeWindowTransparent(HWND hWnd, unsigned char factor)
{
	/* First, see if we can get the API call we need. If we've tried
	once, we don't need to try again. */
	if(!initialized)
	{
		HMODULE hDLL = LoadLibrary("user32");
		pSetLayeredWindowAttributes = (PSLWA) GetProcAddress(hDLL, "SetLayeredWindowAttributes");
		initialized = TRUE;
	}

	if(pSetLayeredWindowAttributes == NULL) 
		return FALSE;

	/* Windows need to be layered to be made transparent. This is done
	by modifying the extended style bits to contain WS_EX_LAYARED. */
	SetLastError(0);

	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

	if(GetLastError())
		return FALSE;

	/* Now, we need to set the 'layered window attributes'. This is where the alpha values get set. */
//	return pSetLayeredWindowAttributes(hWnd, RGB(255,255,255), factor, /*LWA_COLORKEY|*/LWA_ALPHA);
	return pSetLayeredWindowAttributes(hWnd, RGB(255,255,255), factor, LWA_ALPHA);
}

BOOL MakeWindowTransparent2(HWND hWnd, COLORREF color)
{
	/* First, see if we can get the API call we need. If we've tried
	once, we don't need to try again. */
	if(!initialized)
	{
		HMODULE hDLL = LoadLibrary("user32");
		pSetLayeredWindowAttributes = (PSLWA) GetProcAddress(hDLL, "SetLayeredWindowAttributes");
		initialized = TRUE;
	}

	if(pSetLayeredWindowAttributes == NULL) 
		return FALSE;

	/* Windows need to be layered to be made transparent. This is done
	by modifying the extended style bits to contain WS_EX_LAYARED. */
	SetLastError(0);

	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

	if(GetLastError())
		return FALSE;

	/* Now, we need to set the 'layered window attributes'. This is where the alpha values get set. */
//	return pSetLayeredWindowAttributes(hWnd, RGB(255,255,255), factor, /*LWA_COLORKEY|*/LWA_ALPHA);
//	return pSetLayeredWindowAttributes(hWnd, RGB(255,255,255), factor, LWA_ALPHA);
	return pSetLayeredWindowAttributes(hWnd, color, 255, LWA_COLORKEY);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CInfoFrm::InvalidateRect() 
{
	return ::InvalidateRect(m_hWnd, NULL, TRUE);
}

void CInfoFrm::SetInfo(int x, int y, int width, int height)
{
	m_nWidth	= width;
	m_nHeight	= height;
	m_nX		= x;
	m_nY		= y;
}

void CInfoFrm::ToggleState()
{
	if(m_bVisible)
		ShowWindow(m_hWnd, SW_HIDE);
	else
		ShowWindow(m_hWnd, SW_SHOW);

	m_bVisible = !m_bVisible;
}

void DlgProcPlotPreviewInit(HWND hDlg);
void DlgProcPlotPreviewTranslatePens(CString strFile);
void DlgProcPlotPreviewUsePenWidths(CString strFile);

void CInfoFrm::Refresh()
{
	DlgProcPlotPreviewInit(m_hWnd);

	CString strSearch = printStates.GetTranslateFile();
	DlgProcPlotPreviewTranslatePens(strSearch);

	strSearch = printStates.GetPenWidthsFile();
	DlgProcPlotPreviewUsePenWidths(strSearch);
}

void CInfoFrm::SetPosition(int x, int y)
{
	m_nX = x;
	m_nY = y;
}

void CInfoFrm::OnMove(HWND hwnd, int, int)
{
	POINT point;
	point.x = m_nX;
	point.y = m_nY;

	ClientToScreen(app.GetSafeHwnd(), &point);
	MoveWindow(hwnd, point.x, point.y, m_nWidth, m_nHeight, TRUE);
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// CInfoStatusFrm
////////////////////////////////////////////////////////////////////////////////////////////
CInfoStatusFrm::CInfoStatusFrm(int x, int y, int width, int height)
{
	SetInfo(x, y, width, height);
}

void CInfoStatusFrm::OnPaint(HWND hwnd)
{
	CRect rcUpdRect;
	if(::GetUpdateRect(hwnd, &rcUpdRect, TRUE) != 0) 
		app.StatusLineDisplay();

	::ValidateRect(hwnd, 0);
}

void CInfoStatusFrm::OnSize(HWND, UINT, int, int)
{

}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// CInfoModeFrm
////////////////////////////////////////////////////////////////////////////////////////////
CInfoModeFrm::CInfoModeFrm()
{
	SetInfo(0, 70, 550, 18);
}

CInfoModeFrm::CInfoModeFrm(int x, int y, int width, int height)
{
	SetInfo(x, y, width, height);
}

void CInfoModeFrm::OnPaint(HWND hwnd)
{
	CRect rcUpdRect;
	if(::GetUpdateRect(hwnd, &rcUpdRect, TRUE) != 0) 
		app.ModeLineDisplay();

	::ValidateRect(hwnd, 0);
}

void CInfoModeFrm::OnSize(HWND hwnd, UINT, int, int)
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

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// CInfoOdomFrm
////////////////////////////////////////////////////////////////////////////////////////////
CInfoOdomFrm::CInfoOdomFrm(int x, int y, int width, int height)
{
	SetInfo(x, y, width, height);
}

void CInfoOdomFrm::OnPaint(HWND hwnd)
{
	CPegView *pView = CPegView::GetActiveView();

	CRect rcUpdRect;
	if(::GetUpdateRect(hwnd, &rcUpdRect, TRUE) != 0) 
		pView->DisplayOdometer();

   ::ValidateRect(hwnd, 0);
}

void CInfoOdomFrm::OnSize(HWND hwnd, UINT, int, int)
{
	RECT cRect;
	::GetClientRect(app.GetSafeHwnd(), &cRect);
	
	POINT point;
	point.x = cRect.right - m_nWidth;
	point.y = cRect.top;

	m_nX = point.x;
	m_nY = point.y;
	ClientToScreen(app.GetSafeHwnd(), &point);
	MoveWindow(hwnd, point.x, point.y, m_nWidth, m_nHeight, TRUE);
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// State Information
////////////////////////////////////////////////////////////////////////////////////////////
//CInfoStatusFrm statusframe(0, 0, 538, 15);
CInfoStatusFrm statusframe(0, 0, 566, 15);

LRESULT CALLBACK WndProcStateInfo(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg) 
	{
		HANDLE_MSG(hwnd, WM_PAINT, statusframe.OnPaint);
		HANDLE_MSG(hwnd, WM_MOVE, statusframe.OnMove);
		HANDLE_MSG(hwnd, WM_SIZE, statusframe.OnSize);
	}

	return (::DefWindowProc(hwnd, nMsg, wParam, lParam));
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//// Mode Information
//////////////////////////////////////////////////////////////////////////////////////////////
CInfoModeFrm modeframe(0, 635, 935, 15);

LRESULT CALLBACK WndProcModeInfo(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg) 
	{
		HANDLE_MSG(hwnd, WM_PAINT, modeframe.OnPaint);
		HANDLE_MSG(hwnd, WM_MOVE, modeframe.OnMove);
		HANDLE_MSG(hwnd, WM_SIZE, modeframe.OnSize);
	}

	return (::DefWindowProc(hwnd, nMsg, wParam, lParam));
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//// Odometer Information
//////////////////////////////////////////////////////////////////////////////////////////////
CInfoOdomFrm odomframe(200, 50, 92, 119);

LRESULT CALLBACK WndProcOdometer(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg) 
	{
		HANDLE_MSG(hwnd, WM_PAINT, odomframe.OnPaint);
		HANDLE_MSG(hwnd, WM_MOVE, odomframe.OnMove);
		HANDLE_MSG(hwnd, WM_SIZE, odomframe.OnSize);
	}

	return (::DefWindowProc(hwnd, nMsg, wParam, lParam));
}