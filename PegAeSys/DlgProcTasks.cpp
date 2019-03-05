#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "DlgProcSetup.h"

BOOL CALLBACK DlgProcSetup(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			InitWindowText(hDlg, "Tasks");
			InitTasks(hDlg);
			DlgProcSetupSetPage(hDlg, pDoc->GetOptionsPage());
			return FALSE;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{
				case ID_APPLY:
					OkTasks(hDlg);
					return TRUE;
				case IDOK:
					OkTasks(hDlg);
					::EndDialog(hDlg, IDOK);
					return TRUE;
				case IDCANCEL:
					::EndDialog(hDlg, IDCANCEL);
					return FALSE;
			}
			return TRUE;
		}
		case WM_NOTIFY:
		{
			if(wParam == IDC_TREE2)
			{
				if(((LPNMHDR)lParam)->code == TVN_SELCHANGED)
				{
					NMTREEVIEW *pnmtv = (LPNMTREEVIEW)lParam;
					TVITEM item = pnmtv->itemNew;
					DlgProcSetupSetPage(hDlg, item.lParam);
				}
			}
			else
				return FALSE;

			return TRUE;
		}
	}
	return FALSE; 
}