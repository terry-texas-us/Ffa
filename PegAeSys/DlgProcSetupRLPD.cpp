#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "SubProcRLPD.h"
#include "UnitsString.h"

void DlgProcRLPDOptionsInit(HWND);

BOOL CALLBACK DlgProcSetupTabRLPD(HWND hDlg, UINT nMsg, WPARAM, LPARAM)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	char szBuf[32];

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			rlpd::dSecDiam = rlpd::dDiam[2];
			DlgProcRLPDOptionsInit(hDlg);	
			return (TRUE);
		}
		case WM_DIALOG_FOCUS:
		{
			switch(pDoc->GetOptionsFocus())
			{
				case IDC_DIAMETER:
					SendDlgItemMessage(hDlg, IDC_DIAMETER, EM_SETSEL, 0, -1);
					SetFocus(::GetDlgItem(hDlg, IDC_DIAMETER));
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
			rlpd::dInsideRadiusFactor = DlgBoxGetItemDouble(hDlg, IDC_RADIUS_FACTOR2);
			::GetDlgItemText(hDlg, IDC_DIAMETER, szBuf, sizeof(szBuf));
			rlpd::dSecDiam = UnitsString_ParseLength(max(app.GetUnits(), Inches), szBuf);
			rlpd::bGenCenterline = (IsDlgButtonChecked(hDlg, IDC_CENTERLINE) ? true : false);
			rlpd::eJust0 = rlpd::Center0;
			if (IsDlgButtonChecked(hDlg, IDC_LEFT2)) 
				rlpd::eJust0 = rlpd::Left0;
			else if (IsDlgButtonChecked(hDlg, IDC_RIGHT2)) 
				rlpd::eJust0 = rlpd::Right0;
			rlpd::dDiam[2]= Max(0., rlpd::dSecDiam);

			return TRUE;
			break;
		}
	}
	return (FALSE); 
}

//rlpd options init
void DlgProcRLPDOptionsInit(HWND hDlg)
{
	EUnits eUnits = app.GetUnits();
	app.SetUnits(max(eUnits, Inches));
	DlgBoxSetItemUnitsText(hDlg, IDC_DIAMETER, rlpd::dSecDiam);
	app.SetUnits(eUnits);
	DlgBoxSetItemDouble(hDlg, IDC_RADIUS_FACTOR2, rlpd::dInsideRadiusFactor);
	
	::CheckRadioButton(hDlg, IDC_LEFT2, IDC_RIGHT2, IDC_CENTER2 + rlpd::eJust0);

	if (rlpd::bGenCenterline) {::SendDlgItemMessage(hDlg, IDC_CENTERLINE, BM_SETCHECK, 1, 0L);}
}