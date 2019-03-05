#include "stdafx.h"

#include "PegAEsys.h"

BOOL CALLBACK DlgProcOptionsTabRegistry(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
{
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
			if(HIWORD(wParam) == BN_CLICKED)
			{
				if(LOWORD(wParam) == IDC_CLEAR_REG)
				{
					LRESULT lResult  = ::RegDeleteKey(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\Solo");
					LRESULT lResult2 = ::RegDeleteKey(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\MRU");

					if((lResult == ERROR_SUCCESS) || (lResult2 == ERROR_SUCCESS))
						MessageBox(0, "Registry Cleaned", "Operation Complete", 0);

					// update menu buttons
					::RemoveMenu(app.GetMenu(), ID_FILE_MRU1, MF_BYCOMMAND);
					::RemoveMenu(app.GetMenu(), ID_FILE_MRU2, MF_BYCOMMAND);
					::RemoveMenu(app.GetMenu(), ID_FILE_MRU3, MF_BYCOMMAND);
					::RemoveMenu(app.GetMenu(), ID_FILE_MRU4, MF_BYCOMMAND);
					::RemoveMenu(app.GetMenu(), ID_FILE_MRU5, MF_BYCOMMAND);
				}
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