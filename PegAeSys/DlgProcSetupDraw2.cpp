#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "SubProcDraw2.h"

BOOL CALLBACK DlgProcSetupTabDraw2(HWND hDlg, UINT nMsg, WPARAM, LPARAM)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			DlgBoxSetItemUnitsText(hDlg, IDC_DRAW2_WIDTH, draw2::DlgProcDraw2_dDistance);
			return TRUE;
		}
		case WM_DIALOG_FOCUS:
		{
			switch(pDoc->GetOptionsFocus())
			{
				case IDC_DRAW2_WIDTH:
					SendDlgItemMessage(hDlg, IDC_DRAW2_WIDTH, EM_SETSEL, 0, -1);
					SetFocus(::GetDlgItem(hDlg, IDC_DRAW2_WIDTH));
					break;
				default:
					SetFocus(GetWindow(hDlg,GW_CHILD));
					break;
			}

			return TRUE;
		}	
		case WM_DIALOG_OK:
		{
			draw2::DlgProcDraw2_dDistance = DlgBoxGetItemUnitsText(hDlg, IDC_DRAW2_WIDTH);
			return TRUE;
		}
	}
	return FALSE; 
}