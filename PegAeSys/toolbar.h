//#pragma once
//
//#define POPUP_STYLES   (WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_CAPTION)// | WS_SYSMENU | WS_THICKFRAME)
//#define POPUP_EXSTYLES (WS_EX_TOOLWINDOW | WS_EX_WINDOWEDGE)
//#define CHILD_STYLES   (WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS)
//#define CHILD_EXSTYLES (0)
//
//HWND hwndList[2];
//
//LRESULT HANDLE_NCACTIVATE(HWND hwndMain, HWND hwnd, WPARAM wParam, LPARAM lParam)
//{
//	//static HWND hwndList[MAX_DOCK_WINDOWS+1];
//	int i, nNumWnds;
//
//	HWND   hParam = (HWND)lParam;
//
//	BOOL   fKeepActive = wParam;
//	BOOL   fSyncOthers = TRUE;
//
//	nNumWnds = 2;//GetPopupList(hwndMain, hwndList, MAX_DOCK_WINDOWS+1, TRUE);
//
//	// UNDOCUMENTED FEATURE:
//	// if the other window being activated/deactivated (i.e. NOT this one)
//	// is one of our popups, then go (or stay) active, otherwise.
//	for(i = 0; i < nNumWnds; i++)
//	{
//		if(hParam == hwndList[i])
//		{
//			fKeepActive = TRUE;
//			fSyncOthers = FALSE;
//			break;
//		}
//	}
//
//	// If this message was sent by the synchronise-loop (below)
//	// then exit normally
//	if(hParam == (HWND)-1)
//	{
//		return DefWindowProc(hwnd, WM_NCACTIVATE, fKeepActive, 0);
//	}
//
//	// This window is about to change (inactive/active).
//	// Sync all other popups to the same state 
//	if(fSyncOthers == TRUE)
//	{
//		for(i = 0; i < nNumWnds; i++)
//		{
//			//DO NOT send this message to ourselves!!!!
//			if(hwndList[i] != hwnd && hwndList[i] != hParam)
//				SendMessage(hwndList[i], WM_NCACTIVATE, fKeepActive, (LONG)-1);
//		}
//	}
//
//	return DefWindowProc(hwnd, WM_NCACTIVATE, fKeepActive, lParam);
//}
//
//LRESULT HANDLE_ENABLE(HWND hwndMain, HWND hwnd, WPARAM wParam, LPARAM lParam)
//{
////	static HWND hwndList[MAX_DOCK_WINDOWS+1];
//	int i, nNumWnds;
//
//	HWND hParam = (HWND)lParam;
//		
//	nNumWnds = 2;//GetPopupList(hwndMain, hwndList, MAX_DOCK_WINDOWS+1, FALSE);
//
//	for(i = 0; i < nNumWnds; i++)
//	{
//		if(hwndList[i] != hwnd)
//		{
//			EnableWindow(hwndList[i], wParam);
//		}
//	}
//
//	//just do the default
//	return DefWindowProc(hwnd, WM_ENABLE, wParam, lParam);
//}
//
//static LRESULT CALLBACK DockWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
////	int    i;
//
//	switch(msg)
//	{
//	case WM_NCCREATE:
//		
//		// Add this one to list of dock-windows. 
////		if(nNumDockWnds < MAX_DOCK_WINDOWS)
/////		{
////			hDockList[nNumDockWnds++] = hwnd;
//			return TRUE;
////		}
////		else
////		{
////			return FALSE;
////		}
//
//	case WM_NCDESTROY:
//		
//		// remove this window from the list of dock-windows
////		for(i = 0; i < nNumDockWnds; i++)
////		{
////			if(hDockList[i] == hwnd)
////			{
////				for( ; i < nNumDockWnds - 1; i++)
////				{
////					hDockList[i] = hDockList[i+1];
////				}
////
////				nNumDockWnds--;
////				break;
////			}
////		}
//
//		return 0;
//
//	case WM_NCACTIVATE:
//		return HANDLE_NCACTIVATE(app.GetSafeHwnd(), hwnd, wParam, lParam);
//
//	case WM_DESTROY:
//		return 0;
//
//	case WM_CLOSE:
//		DestroyWindow(hwnd);
//		return 0;
//	}
//
//	return DefWindowProc(hwnd, msg, wParam, lParam);
//}
//
//void TogglePopupStyle(HWND hwnd)
//{
//	DWORD dwStyle   = GetWindowLong(hwnd, GWL_STYLE);
//	DWORD dwExStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
//	
//	if(dwStyle & WS_CHILD)
//	{
//		SetWindowLong(hwnd, GWL_STYLE,   (dwStyle   & ~CHILD_STYLES)   | POPUP_STYLES);
//		SetWindowLong(hwnd, GWL_EXSTYLE, (dwExStyle & ~CHILD_EXSTYLES) | POPUP_EXSTYLES);
//		SetParent(hwnd, NULL);
//	}
//	else
//	{
//		SetWindowLong(hwnd, GWL_STYLE,   (dwStyle   & ~POPUP_STYLES)   | CHILD_STYLES);
//		SetWindowLong(hwnd, GWL_EXSTYLE, (dwExStyle & ~POPUP_EXSTYLES) | CHILD_EXSTYLES);
//		SetParent(hwnd, app.GetSafeHwnd());//GetOwner(hwnd));
//	}
//
//	// Send the window a WM_NCCALCSIZE message, because the
//	// frame-style has changed.
//	SetWindowPos(hwnd, 0, 0, 0, 0, 0,   SWP_NOMOVE   | SWP_NOSIZE     |
//										SWP_NOZORDER | SWP_NOACTIVATE |
//										SWP_FRAMECHANGED);
//}
//
//static void Send_WM_SIZE(HWND hwnd)
//{
//	RECT rect;
//
//	GetClientRect(hwnd, &rect);
//	SendMessage(hwnd, WM_SIZE,  SIZE_RESTORED, MAKELPARAM(rect.right-rect.left,rect.bottom-rect.top));
//}
//
////
////	Toggle the specified dock-window between docked/floating status
////
//void DockWnd_ToggleDockingMode(HWND hwnd)
//{
//	ShowWindow(hwnd, SW_HIDE);
//
//	TogglePopupStyle(hwnd);
//
//	// Get parent to reposition if docked
//	Send_WM_SIZE(app.GetSafeHwnd());
//
//	if(app.IsToolbarDocked())
//		SetWindowPos(hwnd, HWND_TOP, 200, 200, 360, 52, 0);
//
//	ShowWindow(hwnd, SW_SHOW);
//}