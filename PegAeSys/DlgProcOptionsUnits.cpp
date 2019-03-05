#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

void DlgProcOptionsTabUnitsInit(HWND hDlg);
void DlgProcOptionsTabUnitsOK(HWND hDlg);

BOOL CALLBACK DlgProcOptionsTabUnits(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
{
	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			DlgProcOptionsTabUnitsInit(hDlg);	
			return (TRUE);
		}
		case WM_DIALOG_FOCUS:
		{
			SetFocus(GetWindow(hDlg,GW_CHILD));
//			SetFocus(::GetDlgItem(hDlg, IDC_PRECISION));
			return TRUE;
		}
		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDC_METRIC:
				{
					::EnableWindow(::GetDlgItem(hDlg, IDC_METRIC_UNITS2), true);
					break;
				}
				case IDC_ARCHITECTURAL:
				case IDC_ENGINEERING:
				case IDC_FEET:
				case IDC_INCHES:
				{
					::EnableWindow(::GetDlgItem(hDlg, IDC_METRIC_UNITS2), false);
					break;
				}
			}
			return TRUE;
		}
		case WM_DIALOG_OK:
		{
			DlgProcOptionsTabUnitsOK(hDlg);
			return TRUE;
		}
	}
	return (FALSE); 
}

void DlgProcOptionsTabUnitsInit(HWND hDlg)
{
	CString strMetricUnits[] = {"Meters", "Millimeters", "Centimeters", "Decimeters", "Kilometers"};
	
	int	iCtrlId = Min(IDC_ARCHITECTURAL + app.GetUnits(), IDC_METRIC);
			
	::CheckRadioButton(hDlg, IDC_ARCHITECTURAL, IDC_METRIC, iCtrlId);
			
	SetDlgItemInt(hDlg, IDC_PRECISION, app.GetUnitsPrec(), FALSE);

	int i;
	for (i = 0; i < sizeof(strMetricUnits) / sizeof(strMetricUnits[0]); i++)
		::SendDlgItemMessage(hDlg, IDC_METRIC_UNITS2, (UINT)CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)strMetricUnits[i]);
//		::SendDlgItemMessage(hDlg, IDC_METRIC_UNITS, LB_ADDSTRING, 0, (LPARAM) (LPCSTR) strMetricUnits[i]);

	::SendDlgItemMessage(hDlg, IDC_METRIC_UNITS2, CB_SELECTSTRING, (WPARAM) -1, (LPARAM)(LPSTR)"Meters"); 

	if (iCtrlId == IDC_METRIC)
	{
		i = app.GetUnits() - Meters;
//		::SendDlgItemMessage(hDlg, IDC_METRIC_UNITS, LB_SETCURSEL, (WPARAM) i, 0L);
		::SendDlgItemMessage(hDlg, IDC_METRIC_UNITS2, CB_SETCURSEL, (WPARAM) i, 0L); 
	}
	else
		::SendDlgItemMessage(hDlg, IDC_METRIC_UNITS2, CB_SELECTSTRING, (WPARAM) -1, (LPARAM)(LPSTR)"Meters"); 

	// enable/disable pen widths file select
	if(::IsDlgButtonChecked(hDlg, IDC_METRIC))
		::EnableWindow(::GetDlgItem(hDlg, IDC_METRIC_UNITS2), true);
	else
		::EnableWindow(::GetDlgItem(hDlg, IDC_METRIC_UNITS2), false);
}

void DlgProcOptionsTabUnitsOK(HWND hDlg)
{
	if (IsDlgButtonChecked(hDlg, IDC_ARCHITECTURAL))
		app.SetUnits(Architectural);
	else if (IsDlgButtonChecked(hDlg, IDC_ENGINEERING))
		app.SetUnits(Engineering);
	else if (IsDlgButtonChecked(hDlg, IDC_FEET))
		app.SetUnits(Feet);
	else if (IsDlgButtonChecked(hDlg, IDC_INCHES))
		app.SetUnits(Inches);
	else
	{	// get units from metric list
		switch(::SendDlgItemMessage(hDlg, IDC_METRIC_UNITS2, CB_GETCURSEL, 0, 0L))
		{
			case 0:
				app.SetUnits(Meters);
				break;
			case 1:
				app.SetUnits(Millimeters);
				break;
			case 2:
				app.SetUnits(Centimeters);
				break;
			case 3:
				app.SetUnits(Decimeters);
				break;
			default:
				app.SetUnits(Kilometers);
		}
	}						
	BOOL bTranslated;
	int iPrec = GetDlgItemInt(hDlg, IDC_PRECISION, &bTranslated, FALSE);
	if (bTranslated)
		app.SetUnitsPrec(iPrec);
}