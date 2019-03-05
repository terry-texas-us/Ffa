#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

extern CExportState estate;

BOOL CALLBACK DlgProcExportACAD(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
{
	switch (nMsg) 
	{
		case WM_INITDIALOG:
		{
			if(estate.bVectorizeText)
				::CheckDlgButton(hDlg, IDC_VECTORIZETEXT, BST_CHECKED);

			DlgBoxSetItemDouble(hDlg, IDC_EDIT_X, estate.dXShift);
			DlgBoxSetItemDouble(hDlg, IDC_EDIT_Y, estate.dYShift);
			DlgBoxSetItemDouble(hDlg, IDC_EDIT_WIDTH, estate.dWidthFactor);

			return TRUE;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDOK:

					// Vectorize Text
					estate.bVectorizeText = (::IsDlgButtonChecked(hDlg, IDC_VECTORIZETEXT) == BST_CHECKED);

					estate.dXShift = DlgBoxGetItemDouble(hDlg, IDC_EDIT_X);
					estate.dYShift = DlgBoxGetItemDouble(hDlg, IDC_EDIT_Y);
					estate.dWidthFactor = DlgBoxGetItemDouble(hDlg, IDC_EDIT_WIDTH);

					::EndDialog(hDlg, TRUE);
					break;
				case IDCANCEL:
					::EndDialog(hDlg, FALSE);
					break;
			}
		}
	}
	return FALSE;
}