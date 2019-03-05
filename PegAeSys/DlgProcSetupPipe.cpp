#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "SubProcPipe.h"

BOOL CALLBACK DlgProcSetupTabPipe(HWND hDlg, UINT nMsg, WPARAM, LPARAM)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			DlgBoxSetItemDouble(hDlg, IDC_TIC_SIZE, pipe::dTicSize);
			DlgBoxSetItemDouble(hDlg, IDC_RISEDROP_RADIUS, pipe::dCSize);
			return (TRUE);
			break;
		}
		case WM_DIALOG_FOCUS:
		{
			switch(pDoc->GetOptionsFocus())
			{
				case IDC_TIC_SIZE:
					SendDlgItemMessage(hDlg, IDC_TIC_SIZE, EM_SETSEL, 0, -1);
					SetFocus(::GetDlgItem(hDlg, IDC_TIC_SIZE));
					break;
				default:
					SetFocus(GetWindow(hDlg,GW_CHILD));
					break;
			}

			return TRUE;
			break;
		}
		case WM_DIALOG_OK:
		{
			pipe::dTicSize = DlgBoxGetItemDouble(hDlg, IDC_TIC_SIZE);
			pipe::dCSize = DlgBoxGetItemDouble(hDlg, IDC_RISEDROP_RADIUS);
			return TRUE;
			break;
		}
	}
	return (FALSE); 
}