#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "UserAxis.h"

//double	UserAxis_dInfAng2;
//double	UserAxis_dAxOffAng2;

BOOL CALLBACK DlgProcOptionsTabUserAxis(HWND hDlg, UINT nMsg, WPARAM, LPARAM)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			DlgBoxSetItemDouble(hDlg, IDC_USR_AX_INF_ANG, UserAxis::UserAxis_dInfAng);
			DlgBoxSetItemDouble(hDlg, IDC_USR_AX_Z_OFF_ANG, UserAxis::UserAxis_dAxOffAng);
			return TRUE;
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
		}
		case WM_DIALOG_OK:
		{
			UserAxis::UserAxis_dInfAng = DlgBoxGetItemDouble(hDlg, IDC_USR_AX_INF_ANG);
			UserAxis::UserAxis_dAxOffAng = DlgBoxGetItemDouble(hDlg, IDC_USR_AX_Z_OFF_ANG);

			CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);

			return TRUE;
		}
	}
	return (FALSE); 
}