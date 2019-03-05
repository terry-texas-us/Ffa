#pragma once

// User defined message sent by CDlgViewZoom to WndProcKeyPlan when a new
// zoom ratio is set using an edit control
//		LPARAM is the new value of ratio
#define WM_USER_ON_NEW_RATIO WM_USER + 1

class CDlgViewZoom
{
	public:
		static HBITMAP	m_hbmKeyplan;
		static CRect	m_rcWnd;
};