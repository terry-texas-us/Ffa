#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

BOOL CALLBACK DlgProcProfile(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
{
//	CPegDoc *pDoc = CPegDoc::GetDoc();

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			return (TRUE);
		}
		case WM_DIALOG_FOCUS:
		{
			SetFocus(GetWindow(hDlg,GW_CHILD));
			return TRUE;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{
				case IDC_CUSTOM1_UPDATE:
					app.ProfileWrite(PROF_CUSTOM1);
					break;
				case IDC_CUSTOM2_UPDATE:
					app.ProfileWrite(PROF_CUSTOM2);
					break;
//				case IDC_CUSTOM1_RENAME:
//					break;
//				case IDC_CUSTOM2_RENAME:
//					break;
			}

			return TRUE;
		}
		case WM_DIALOG_OK:
		{

			return TRUE;
		}
	}
	return (FALSE); 
}