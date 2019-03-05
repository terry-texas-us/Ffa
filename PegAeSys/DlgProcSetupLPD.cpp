#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "UnitsString.h"
#include "SubProcLPD.h"

void DlgProcLPDOptionsInit(HWND);

BOOL CALLBACK DlgProcSetupTabLPD(HWND hDlg, UINT nMsg, WPARAM, LPARAM)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	char szBuf2[32];

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			DlgProcLPDOptionsInit(hDlg);
			
			return (TRUE);
			break;
		}
		case WM_DIALOG_FOCUS:
		{
			switch(pDoc->GetOptionsFocus())
			{
				case IDC_RADIUS_FACTOR:
					SendDlgItemMessage(hDlg, IDC_RADIUS_FACTOR, EM_SETSEL, 0, -1);
					SetFocus(::GetDlgItem(hDlg, IDC_RADIUS_FACTOR));
					break;
				case IDC_WIDTH:
					SendDlgItemMessage(hDlg, IDC_WIDTH, EM_SETSEL, 0, -1);
					SetFocus(::GetDlgItem(hDlg, IDC_WIDTH));
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
			lpd::dInsRadElFac = DlgBoxGetItemDouble(hDlg, IDC_RADIUS_FACTOR);
			::GetDlgItemText(hDlg, IDC_WIDTH, szBuf2, sizeof(szBuf2));
			lpd::dSecWid = UnitsString_ParseLength(max(app.GetUnits(), Inches), szBuf2);
			::GetDlgItemText(hDlg, IDC_DEPTH, szBuf2, sizeof(szBuf2));
			lpd::dSecDep = UnitsString_ParseLength(max(app.GetUnits(), Inches), szBuf2);
			lpd::bGenTurnVanes = (IsDlgButtonChecked(hDlg, IDC_GEN_VANES) ? true : false);
			lpd::eElbow = (IsDlgButtonChecked(hDlg, IDC_MITERED) ? Mittered : Radial);
			lpd::eJust = lpd::Center;
			if (IsDlgButtonChecked(hDlg, IDC_LEFT)) 
				lpd::eJust = lpd::Left;
			else if (IsDlgButtonChecked(hDlg, IDC_RIGHT)) 
				lpd::eJust = lpd::Right;

			return TRUE;
			break;
		}
	}
	return (FALSE); 
}

void DlgProcLPDOptionsInit(HWND hDlg)
{
	EUnits eUnits = app.GetUnits();

	app.SetUnits(max(eUnits, Inches));

	DlgBoxSetItemUnitsText(hDlg, IDC_WIDTH, lpd::dSecWid);
	DlgBoxSetItemUnitsText(hDlg, IDC_DEPTH, lpd::dSecDep);

	app.SetUnits(eUnits);

	DlgBoxSetItemDouble(hDlg, IDC_RADIUS_FACTOR, lpd::dInsRadElFac);
	
	::CheckRadioButton(hDlg, IDC_LEFT, IDC_RIGHT, IDC_CENTER + lpd::eJust);
						
	if (lpd::bGenTurnVanes)
		::SendDlgItemMessage(hDlg, IDC_GEN_VANES, BM_SETCHECK, 1, 0L);
			
	::SendDlgItemMessage(hDlg, IDC_MITERED + lpd::eElbow, BM_SETCHECK, 1, 0L);
}