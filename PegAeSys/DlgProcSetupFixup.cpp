#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "SubProcFixup.h"
#include "UserAxis.h"

BOOL CALLBACK DlgProcSetupTabFixup(HWND hDlg, UINT nMsg, WPARAM, LPARAM)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			//fixup
			DlgBoxSetItemDouble(hDlg, IDC_FIX_SIZ, fixup::dFilletChamfer);
			DlgBoxSetItemDouble(hDlg, IDC_FIX_AX_TOL, fixup::dInfluenceAngle);

			return TRUE;
			break;
		}
		case WM_DIALOG_FOCUS:
		{
			switch(pDoc->GetOptionsFocus())
			{
				case IDC_FIX_AX_TOL:
					SendDlgItemMessage(hDlg, IDC_FIX_AX_TOL, EM_SETSEL, 0, -1);
					SetFocus(::GetDlgItem(hDlg, IDC_FIX_AX_TOL));
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
			// fixup
			fixup::dFilletChamfer = DlgBoxGetItemDouble(hDlg, IDC_FIX_SIZ);
			fixup::dInfluenceAngle = DlgBoxGetItemDouble(hDlg, IDC_FIX_AX_TOL);
			UserAxis::UserAxisSetInfAng(fixup::dInfluenceAngle);

			return TRUE;
			break;
		}
	}
	return (FALSE); 
}