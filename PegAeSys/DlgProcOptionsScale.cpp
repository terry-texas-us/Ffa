#include "stdafx.h"

#include "PegAEsys.h"

//#include "dde.h"
//#include "ddeGItms.h"

void DlgProcOptionsScaleInit(HWND hDlg);
void DlgProcOptionsScaleOK(HWND hDlg);

BOOL CALLBACK DlgProcOptionsTabScale(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
{
	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			DlgProcOptionsScaleInit(hDlg);		
			return (TRUE);
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{
				case IDC_SCALE_USER:
				{
					if(::IsDlgButtonChecked(hDlg, IDC_SCALE_USER))
					{
						::EnableWindow(::GetDlgItem(hDlg, IDC_SCALE), true);
						::EnableWindow(::GetDlgItem(hDlg, IDC_SCALE2), false);
					}
					else
					{
						::EnableWindow(::GetDlgItem(hDlg, IDC_SCALE), false);
						::EnableWindow(::GetDlgItem(hDlg, IDC_SCALE2), true);
					}
					break;
				}
			}

			return TRUE;
		}
		case WM_DIALOG_FOCUS:
		{
			SetFocus(GetWindow(hDlg,GW_CHILD));
			return TRUE;
		}
		case WM_DIALOG_OK:
		{
			DlgProcOptionsScaleOK(hDlg);	
			return TRUE;
		}
	}
	return (FALSE); 
}

void DlgProcOptionsScaleInit(HWND hDlg)
{
	char szBuf2[32];

	SetWindowLong(hDlg, GWL_USERDATA, (long)app.GetScale());
	sprintf(szBuf2, "%3.2f", app.GetScale());
	SetDlgItemText(hDlg, IDC_SCALE, szBuf2); 

	SendMessage(::GetDlgItem(hDlg, IDC_SCALE2), (UINT)CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)"384  1.0\" = 1\\32\"");
	SendMessage(::GetDlgItem(hDlg, IDC_SCALE2), (UINT)CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)"192  1.0\" = 1\\16\"");
	SendMessage(::GetDlgItem(hDlg, IDC_SCALE2), (UINT)CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)"144  1.0\" = 1\\12\"");
	SendMessage(::GetDlgItem(hDlg, IDC_SCALE2), (UINT)CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)"128  1.0\" = 3\\32\"");
	SendMessage(::GetDlgItem(hDlg, IDC_SCALE2), (UINT)CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)"120  1.0\" = 1\\10\"");
	SendMessage(::GetDlgItem(hDlg, IDC_SCALE2), (UINT)CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)" 96   1.0\" = 1\\8\"");
	SendMessage(::GetDlgItem(hDlg, IDC_SCALE2), (UINT)CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)" 64   1.0\" = 3\\16\"");
	SendMessage(::GetDlgItem(hDlg, IDC_SCALE2), (UINT)CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)" 48   1.0\" = 1\\4\"");
	SendMessage(::GetDlgItem(hDlg, IDC_SCALE2), (UINT)CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)" 32   1.0\" = 3\\8\"");
	SendMessage(::GetDlgItem(hDlg, IDC_SCALE2), (UINT)CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)" 24   1.0\" = 1\\2\"");
	SendMessage(::GetDlgItem(hDlg, IDC_SCALE2), (UINT)CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)" 16   1.0\" = 3\\4\"");
	SendMessage(::GetDlgItem(hDlg, IDC_SCALE2), (UINT)CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)"  8    1.0\" = 1.5\"");
	SendMessage(::GetDlgItem(hDlg, IDC_SCALE2), (UINT)CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)"  4    1.0\" = 3\"");
	SendMessage(::GetDlgItem(hDlg, IDC_SCALE2), (UINT)CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)"  4    Quarter");
	SendMessage(::GetDlgItem(hDlg, IDC_SCALE2), (UINT)CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)"  2    Half");
	SendMessage(::GetDlgItem(hDlg, IDC_SCALE2), (UINT)CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)"  1    Full");
	SendMessage(::GetDlgItem(hDlg, IDC_SCALE2), (UINT)CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)" .5    Double");

	// select default scale
	CString strSearch2 = " ";
	bool dropDown = true;

	switch((int)(app.GetScale() * 100))
	{
		case 38400:	strSearch2 = "384  1.0\" = 1\\32\""; break;
		case 19200:	strSearch2 = "192  1.0\" = 1\\16\""; break;
		case 14400:	strSearch2 = "144  1.0\" = 1\\12\""; break;
		case 12800:	strSearch2 = "128  1.0\" = 3\\32\""; break;
		case 12000:	strSearch2 = "120  1.0\" = 1\\10\""; break;
		case 9600:	strSearch2 = " 96   1.0\" = 1\\8\""; break;
		case 6400:	strSearch2 = " 64   1.0\" = 3\\16\""; break;
		case 4800:	strSearch2 = " 48   1.0\" = 1\\4\""; break;
		case 3200:	strSearch2 = " 32   1.0\" = 3\\8\""; break;
		case 2400:	strSearch2 = " 24   1.0\" = 1\\2\""; break;
		case 1600:	strSearch2 = " 16   1.0\" = 3\\4\""; break;
		case 800:	strSearch2 = "  8    1.0\" = 1.5\""; break;
		case 400:	strSearch2 = "  4    Quarter"; break;
		case 200:	strSearch2 = "  2    Half"; break;
		case 100:	strSearch2 = "  1    Full"; break;
		case 50:	strSearch2 = " .5    Double"; break;
		default:	dropDown = false;
	}

	if(dropDown)
	{
		int lResult = ::SendDlgItemMessage(hDlg, IDC_SCALE2, CB_SELECTSTRING, (WPARAM) -1, (LPARAM)(LPSTR)strSearch2.GetBuffer()); 
		if(lResult == CB_ERR)
			::SendDlgItemMessage(hDlg, IDC_SCALE2, CB_SETCURSEL, 0, 0);

		::CheckDlgButton(hDlg, IDC_SCALE_USER, BST_UNCHECKED);
		::EnableWindow(::GetDlgItem(hDlg, IDC_SCALE), false);
		::EnableWindow(::GetDlgItem(hDlg, IDC_SCALE2), true);
	}
	else
	{
		strSearch2 = "1\\8\" = 1.0\"    96";
		int lResult = ::SendDlgItemMessage(hDlg, IDC_SCALE2, CB_SELECTSTRING, (WPARAM) -1, (LPARAM)(LPSTR)strSearch2.GetBuffer()); 
		if(lResult == CB_ERR)
			::SendDlgItemMessage(hDlg, IDC_SCALE2, CB_SETCURSEL, 0, 0);

		::CheckDlgButton(hDlg, IDC_SCALE_USER, BST_CHECKED);
		::EnableWindow(::GetDlgItem(hDlg, IDC_SCALE), true);
		::EnableWindow(::GetDlgItem(hDlg, IDC_SCALE2), false);
	}
}

void DlgProcOptionsScaleOK(HWND hDlg)
{
	char szBuf2[32];

	double dScale = 0;
	if(::IsDlgButtonChecked(hDlg, IDC_SCALE_USER))
	{
		dScale = (double)::GetWindowLong(hDlg, GWL_USERDATA);
		::GetDlgItemText(hDlg, IDC_SCALE, szBuf2, sizeof(szBuf2));
		dScale = atof(szBuf2); 
		
	}
	else
	{
		LRESULT lResult = ::SendDlgItemMessage(hDlg, IDC_SCALE2, CB_GETCURSEL, 0, 0);
		if(lResult != CB_ERR)
		{
			switch(lResult)
			{
				case 0:			dScale = 384; break;
				case 1:			dScale = 192; break;
				case 2:			dScale = 144; break;
				case 3:			dScale = 128; break;
				case 4:			dScale = 120; break;
				case 5:
				default:		dScale = 96; break;
				case 6:			dScale = 64; break;
				case 7:			dScale = 48; break;
				case 8:			dScale = 32; break;
				case 9:			dScale = 24; break;
				case 10:		dScale = 16; break;
				case 11:		dScale = 8; break;
				case 12:		dScale = 4; break;
				case 13:		dScale = 4; break;
				case 14:		dScale = 2; break;
				case 15:		dScale = 1; break;
				case 16:		dScale = .5; break;
			}
		}
	}
	app.SetScale(dScale);
//			dde::PostAdvise(dde::ScaleInfo);
	app.StatusLineDisplay(Scale);
}