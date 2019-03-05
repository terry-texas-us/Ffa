#pragma once

namespace print
{
	extern bool		bUserAbort;
	extern HWND		hDlgPrint;

	// mak
	extern CString	strDeviceName;
	extern CString	strPortName;

	BOOL CALLBACK	AbortProc(HDC hDC, int nCode);
	BOOL CALLBACK	DlgProcAbort(HWND hDlg, UINT nMsg, WPARAM, LPARAM);
	void			FilePlot(WPARAM wParam);
	void			FilePrintCurrentView();
}