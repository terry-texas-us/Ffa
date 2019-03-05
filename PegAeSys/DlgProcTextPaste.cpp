#include "stdafx.h"
#include "DlgProcTextWidth.h"

namespace textwidth
{
	int nWidth = 70;
}

BOOL CALLBACK DlgProcTextPaste(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
{
	switch (nMsg) 
	{
		case WM_INITDIALOG:
			::SetDlgItemInt(hDlg, IDC_WIDTHCOUNT, textwidth::nWidth, FALSE);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
				{
					textwidth::nWidth = ::GetDlgItemInt(hDlg, IDC_WIDTHCOUNT, 0, false);
					::EndDialog(hDlg, TRUE);
				}
				case IDCANCEL:
					::EndDialog(hDlg, FALSE);
			}
	}
	return FALSE;
}