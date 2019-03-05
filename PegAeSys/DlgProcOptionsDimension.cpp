#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "DlgProcSetLength.h"

//#include "dde.h"
//#include "ddeGItms.h"

double DlgProcDistance_dDistance;

BOOL CALLBACK DlgProcOptionsTabDimension(HWND hDlg, UINT nMsg, WPARAM, LPARAM)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	char szBuf3[32];

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			DlgProcDistance_dDistance = app.GetDimLen();
			DlgBoxSetItemUnitsText(hDlg, IDC_DISTANCE, DlgProcDistance_dDistance);
			DlgBoxSetItemDouble(hDlg, IDC_ANG, app.GetDimAngZ());
			return TRUE;
		}
		case WM_DIALOG_FOCUS:
		{
			switch(pDoc->GetOptionsFocus())
			{
				case IDC_DISTANCE:
					SendDlgItemMessage(hDlg, IDC_DISTANCE, EM_SETSEL, 0, -1);
					SetFocus(::GetDlgItem(hDlg, IDC_DISTANCE));
					break;
				case IDC_ANG:
					SendDlgItemMessage(hDlg, IDC_ANG, EM_SETSEL, 0, -1);
					SetFocus(::GetDlgItem(hDlg, IDC_ANG));
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
			DlgProcDistance_dDistance = DlgBoxGetItemUnitsText(hDlg, IDC_DISTANCE);
			app.SetDimLen(DlgProcDistance_dDistance);
//			dde::PostAdvise(dde::DimLenInfo);
			app.StatusLineDisplay(DimLen);

			::GetDlgItemText(hDlg, IDC_ANG, szBuf3, sizeof(szBuf3));
			app.SetDimAngZ(atof(szBuf3));
//			dde::PostAdvise(dde::DimAngZInfo);

			return TRUE;
		}
	}
	return FALSE; 
}