#include "stdafx.h"
#include "resource.h"


BOOL CALLBACK DlgProcAbout(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
{
	HWND aboutDate = GetDlgItem(hDlg, IDC_ABB_DATE1);
	HWND aboutBuild = GetDlgItem(hDlg, IDC_ABB_BUILD1);
	CString ver;
#ifndef DEV_BRANCH
	ver += "BETA";
	ver += " ";
#endif
	ver += "4.08.03";
#ifdef DEV_BRANCH
	ver += "\n";
	ver += DEV_BRANCH;
#endif
	CString date = "Build Date\n";
//	date = _DATE_;

	switch (nMsg) 
	{
		case WM_INITDIALOG:
			SetWindowText(aboutDate, date);
			SetWindowText(aboutBuild, ver);

			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
				case IDCANCEL:
					::EndDialog(hDlg, TRUE);
			}
	}
	return FALSE;
}