#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "OwnerDraw.h"

void SetupAppearance_DrawEntire(LPDRAWITEMSTRUCT lpDIS, int inflate);

void DlgProcAppearanceInit(HWND hDlg);
void DlgProcAppearanceOK(HWND hDlg);
//void DlgProcAppearanceFillColorCombos(HWND hDlg);

static bool bChanged = false;

BOOL CALLBACK DlgProcAppearance(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CPegView *pView = CPegView::GetActiveView();

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			bChanged = false;
			DlgProcAppearanceInit(hDlg);
			return (TRUE);
		}
		case WM_DIALOG_FOCUS:
		{
			SetFocus(GetWindow(hDlg,GW_CHILD));
			return TRUE;
		}
		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDC_DISP_ORIGIN:
				{
					UINT bState = ::IsDlgButtonChecked(hDlg, IDC_DISP_ORIGIN);
					bool bValue = (bState == BST_CHECKED) ? true : false;
					::EnableWindow(::GetDlgItem(hDlg, IDC_ORIGIN_COLOR1), bValue);
					::EnableWindow(::GetDlgItem(hDlg, IDC_ORIGIN_COLOR2), bValue);
					::EnableWindow(::GetDlgItem(hDlg, IDC_ORIGIN_STAT1), bValue);
					::EnableWindow(::GetDlgItem(hDlg, IDC_ORIGIN_STAT2), bValue);
					break;
				}
				case IDC_PENWIDTHS:
				{
					UINT bState = ::IsDlgButtonChecked(hDlg, IDC_PENWIDTHS);
					bool bValue = (bState == BST_CHECKED) ? true : false;
					::EnableWindow(::GetDlgItem(hDlg, IDC_PENWIDTHS_FILE), bValue);
					break;
				}
				case IDC_COLOR_FILE:
				{
					// pen colors file
					LRESULT lResult = ::SendDlgItemMessage(hDlg, IDC_COLOR_FILE, CB_GETCURSEL, 0, 0);
					if(lResult != CB_ERR)
					{
						char szBuf2[MAX_PATH];
						::SendDlgItemMessage(hDlg, IDC_COLOR_FILE, CB_GETLBTEXT, lResult, (LPARAM)szBuf2);
//						app.PenColorsSetFile(szBuf2);
						app.PenColorsLoadTemp(app.GetAppPath() + "\\Pens\\Colors\\" + szBuf2);
			//			DlgProcAppearanceFillColorCombos(hDlg);
					}

					bChanged = true;
					
					::SendDlgItemMessage(hDlg, IDC_BACK_COLOR, CB_SETCURSEL, app.BackgroundColorGet(), 0L);
					::SendDlgItemMessage(hDlg, IDC_ORIGIN_COLOR1, CB_SETCURSEL, pView->OriginGetColor1(), 0L);
					::SendDlgItemMessage(hDlg, IDC_ORIGIN_COLOR2, CB_SETCURSEL, pView->OriginGetColor2(), 0L);
				}
			}
			return TRUE;
		}
		case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT) lParam;
			
		//	if(lpDIS->CtlID == IDC_INFO_COLOR1)
			{
				if (lpDIS->itemID == - 1)					// Empty combo box
					OwnerDraw_Focus(lpDIS, 0);				// Draw only focus rectangle
				else
				{
					switch (lpDIS->itemAction)
					{
						case ODA_DRAWENTIRE:
							SetupAppearance_DrawEntire(lpDIS, 1);
							break;
						
						case ODA_SELECT:
							OwnerDraw_Select(lpDIS, 0);				  
							break;
						
						case ODA_FOCUS:
							OwnerDraw_Focus(lpDIS, 0);
							break;
					}

	//				penPaint = (PENCOLOR)lpDIS->itemID;
//					InvalidateRect(hDlg, &dimen, TRUE);
				}
			}
			return TRUE;
		} 
		case WM_DIALOG_OK:
		{
			DlgProcAppearanceOK(hDlg);
			return TRUE;
		}
	}
	return (FALSE); 
}

void DlgProcAppearanceInit(HWND hDlg)
{
	CPegView *pView = CPegView::GetActiveView();

	char szBuf[32];

	// display status information
	UINT uiValue = app.IsStateInfoOn() == true ? 1 : 0;
	::CheckDlgButton(hDlg, IDC_DISP_STATUS, uiValue);

	// display mode information
	uiValue = app.IsModeInfoOn() == true ? 1 : 0;
	::CheckDlgButton(hDlg, IDC_DISP_MODE, uiValue);

	// display odometer
	uiValue = pView->IsOdometerOn() == true ? 1 : 0;
	::CheckDlgButton(hDlg, IDC_DISP_ODOM, uiValue);

	// display scrollbars
	uiValue = pView->IsScrollbarsOn() == true ? 1 : 0;
	::CheckDlgButton(hDlg, IDC_DISP_SCROLL, uiValue);

	// display TTF
	uiValue = pView->IsTTFontsOn() == true ? 1 : 0;
	::CheckDlgButton(hDlg, IDC_DISP_TTF, uiValue);

	// display Origin Markers
	uiValue = pView->IsOriginMarkerOn() == true ? 1 : 0;
	::CheckDlgButton(hDlg, IDC_DISP_ORIGIN, uiValue);

	// origin color 1
	for(int i = 0; i < sizeof(crHotCols) / sizeof(COLORREF); i++)
		::SendDlgItemMessage(hDlg, IDC_ORIGIN_COLOR1, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) _itoa(i, szBuf, 10));
	::SendDlgItemMessage(hDlg, IDC_ORIGIN_COLOR1, CB_SETCURSEL, pView->OriginGetColor1(), 0L);

	// origin color 2
	for(int i = 0; i < sizeof(crHotCols) / sizeof(COLORREF); i++)
		::SendDlgItemMessage(hDlg, IDC_ORIGIN_COLOR2, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) _itoa(i, szBuf, 10));
	::SendDlgItemMessage(hDlg, IDC_ORIGIN_COLOR2, CB_SETCURSEL, pView->OriginGetColor2(), 0L);

	// transparency slider
	SendMessage(::GetDlgItem(hDlg, IDC_TRANS_SLIDER), TBM_SETRANGE, TRUE, MAKELPARAM(0, 255));
	SendMessage(::GetDlgItem(hDlg, IDC_TRANS_SLIDER), TBM_SETTIC, 0, app.GetTransFactor());
	SendMessage(::GetDlgItem(hDlg, IDC_TRANS_SLIDER), TBM_SETPOS, TRUE, app.GetTransFactor());

	// display pen widths
	uiValue = pView->IsPenWidthsOn() == true ? 1 : 0;
	::CheckDlgButton(hDlg, IDC_PENWIDTHS, uiValue);

	// pen widths file
	{
		CString s3 = app.GetAppPath();
		s3 += "Pens\\Widths\\*.txt";

		char strSearch[12] = "_readme.txt";

		int iResult = DlgDirListComboBox(hDlg, s3.GetBuffer(), IDC_PENWIDTHS_FILE, 0, DDL_READWRITE);
		if(iResult == 0)
			MessageBox(hDlg, "Pen Color files must be located in \"(ApplicationDirectory)\\Pens\\Widths\\\"", 0, 0);

		int lResult = ::SendDlgItemMessage(hDlg, IDC_PENWIDTHS_FILE, CB_FINDSTRING, (WPARAM) -1, (LPARAM)&strSearch); 
		if(lResult != CB_ERR)
			::SendDlgItemMessage(hDlg, IDC_PENWIDTHS_FILE, CB_DELETESTRING, (WPARAM)lResult, 0);

		// select pen widths file
		CString strSearch3 = pView->PenWdithsGetFile();
		lResult = ::SendDlgItemMessage(hDlg, IDC_PENWIDTHS_FILE, CB_SELECTSTRING, (WPARAM) -1, (LPARAM)(LPSTR)strSearch3.GetBuffer()); 
		if(lResult == CB_ERR)
			::SendDlgItemMessage(hDlg, IDC_PENWIDTHS_FILE, CB_SETCURSEL, 0, 0);
	}

	// pen colors file
	{
		CString s3 = app.GetAppPath();
		s3 += "Pens\\Colors\\*.txt";

		char strSearch[12] = "_readme.txt";

		int iResult = DlgDirListComboBox(hDlg, s3.GetBuffer(), IDC_COLOR_FILE, 0, DDL_READWRITE);
		if(iResult == 0)
			MessageBox(hDlg, "Pen Color files must be located in \"(ApplicationDirectory)\\Pens\\Colors\\\"", 0, 0);

		int lResult = ::SendDlgItemMessage(hDlg, IDC_COLOR_FILE, CB_FINDSTRING, (WPARAM) -1, (LPARAM)&strSearch); 
		if(lResult != CB_ERR)
			::SendDlgItemMessage(hDlg, IDC_COLOR_FILE, CB_DELETESTRING, (WPARAM)lResult, 0);

		// select pen color file
		CString strSearch2 = app.PenColorsGetFile();
		lResult = ::SendDlgItemMessage(hDlg, IDC_COLOR_FILE, CB_SELECTSTRING, (WPARAM) -1, (LPARAM)(LPSTR)strSearch2.GetBuffer()); 
		if(lResult == CB_ERR)
			::SendDlgItemMessage(hDlg, IDC_COLOR_FILE, CB_SETCURSEL, 0, 0);
	}

	// background color
	for(int i = 0; i < sizeof(crHotCols) / sizeof(COLORREF); i++)
		::SendDlgItemMessage(hDlg, IDC_BACK_COLOR, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) _itoa(i, szBuf, 10));
	::SendDlgItemMessage(hDlg, IDC_BACK_COLOR, CB_SETCURSEL, app.BackgroundColorGet(), 0L);

	// default view
	::SendDlgItemMessage(hDlg, IDC_DEFAULT_VIEW, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)"Unchanged");
	::SendDlgItemMessage(hDlg, IDC_DEFAULT_VIEW, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)"Overview");
//	::SendDlgItemMessage(hDlg, IDC_DEFAULT_VIEW, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)"Best");
	switch(pView->GetDefaultView())
	{
		case CPegView::VIEW_UNCHANGED:
			::SendDlgItemMessage(hDlg, IDC_DEFAULT_VIEW, CB_SETCURSEL, 0, 0L);
			break;
		case CPegView::VIEW_OVERVIEW:
			::SendDlgItemMessage(hDlg, IDC_DEFAULT_VIEW, CB_SETCURSEL, 1, 0L);
			break;
//		case CPegView::VIEW_BEST:
//			::SendDlgItemMessage(hDlg, IDC_DEFAULT_VIEW, CB_SETCURSEL, 2, 0L);
//			break;
		
	}

	// enable/disable origin colors
	if(::IsDlgButtonChecked(hDlg, IDC_DISP_ORIGIN))
	{
		::EnableWindow(::GetDlgItem(hDlg, IDC_ORIGIN_COLOR1), true);
		::EnableWindow(::GetDlgItem(hDlg, IDC_ORIGIN_COLOR2), true);
		::EnableWindow(::GetDlgItem(hDlg, IDC_ORIGIN_STAT1), true);
		::EnableWindow(::GetDlgItem(hDlg, IDC_ORIGIN_STAT2), true);
	}
	else
	{
		::EnableWindow(::GetDlgItem(hDlg, IDC_ORIGIN_COLOR1), false);
		::EnableWindow(::GetDlgItem(hDlg, IDC_ORIGIN_COLOR2), false);
		::EnableWindow(::GetDlgItem(hDlg, IDC_ORIGIN_STAT1), false);
		::EnableWindow(::GetDlgItem(hDlg, IDC_ORIGIN_STAT2), false);
	}

	// enable/disable pen widths file select
	if(::IsDlgButtonChecked(hDlg, IDC_PENWIDTHS))
		::EnableWindow(::GetDlgItem(hDlg, IDC_PENWIDTHS_FILE), true);
	else
		::EnableWindow(::GetDlgItem(hDlg, IDC_PENWIDTHS_FILE), false);
}

//void DlgProcAppearanceFillColorCombos(HWND hDlg)
//{
//	CPegView *pView = CPegView::GetActiveView();
//	char szBuf[32];
//
//	// origin color 1
//	for(int i = 0; i < sizeof(crHotCols) / sizeof(COLORREF); i++)
//		::SendDlgItemMessage(hDlg, IDC_ORIGIN_COLOR1, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) _itoa(i, szBuf, 10));
//	::SendDlgItemMessage(hDlg, IDC_ORIGIN_COLOR1, CB_SETCURSEL, pView->OriginGetColor1(), 0L);
//
//	// origin color 2
//	for(int i = 0; i < sizeof(crHotCols) / sizeof(COLORREF); i++)
//		::SendDlgItemMessage(hDlg, IDC_ORIGIN_COLOR2, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) _itoa(i, szBuf, 10));
//	::SendDlgItemMessage(hDlg, IDC_ORIGIN_COLOR2, CB_SETCURSEL, pView->OriginGetColor2(), 0L);
//
//	// background color
//	for(int i = 0; i < sizeof(crHotCols) / sizeof(COLORREF); i++)
//		::SendDlgItemMessage(hDlg, IDC_BACK_COLOR, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) _itoa(i, szBuf, 10));
//	::SendDlgItemMessage(hDlg, IDC_BACK_COLOR, CB_SETCURSEL, app.BackgroundColorGet(), 0L);
//}

void DlgProcAppearanceOK(HWND hDlg)
{
	CPegView *pView = CPegView::GetActiveView();

	// display status information
	UINT uiValue = ::IsDlgButtonChecked(hDlg, IDC_DISP_STATUS);
	app.StatusInfoEnable((uiValue==0) ? false : true);

	// display mode information
	uiValue = ::IsDlgButtonChecked(hDlg, IDC_DISP_MODE);
	app.ModeInfoEnable((uiValue==0) ? false : true);

	// display odometer
	uiValue = ::IsDlgButtonChecked(hDlg, IDC_DISP_ODOM);
	pView->OdometerEnable((uiValue==0) ? false : true);

	// display scrollbars
	uiValue = ::IsDlgButtonChecked(hDlg, IDC_DISP_SCROLL);
	pView->ScrollbarEnable((uiValue==0) ? false : true);

	// display TTF
	uiValue = ::IsDlgButtonChecked(hDlg, IDC_DISP_TTF);
	pView->TTFEnable((uiValue==0) ? false : true);

	// display Origin Markers
	uiValue = ::IsDlgButtonChecked(hDlg, IDC_DISP_ORIGIN);
	pView->OriginEnable((uiValue==0) ? false : true);

	// origin color 1
	LRESULT lResult = ::SendDlgItemMessage(hDlg, IDC_ORIGIN_COLOR1, CB_GETCURSEL, 0, 0L);
	pView->OriginSetColor1(lResult);

	// origin color 2
	lResult = ::SendDlgItemMessage(hDlg, IDC_ORIGIN_COLOR2, CB_GETCURSEL, 0, 0L);
	pView->OriginSetColor2(lResult);
	
	// transparency slider
	lResult = SendMessage(::GetDlgItem(hDlg, IDC_TRANS_SLIDER), TBM_GETPOS, 0, 0);
	app.SetTransFactor(lResult);

	// display pen widths
	uiValue = ::IsDlgButtonChecked(hDlg, IDC_PENWIDTHS);
	pView->PenWidthsEnable((uiValue==0) ? false : true);

	// pen widths file
	lResult = ::SendDlgItemMessage(hDlg, IDC_PENWIDTHS_FILE, CB_GETCURSEL, 0, 0);
	if(lResult != CB_ERR)
	{
		char szBuf2[MAX_PATH];
		::SendDlgItemMessage(hDlg, IDC_PENWIDTHS_FILE, CB_GETLBTEXT, lResult, (LPARAM)szBuf2);
		pView->PenWidthsSetFile(szBuf2);
		app.PenWidthsLoad(app.GetAppPath() + "\\Pens\\Widths\\" + szBuf2);
	}

	// pen colors file
	lResult = ::SendDlgItemMessage(hDlg, IDC_COLOR_FILE, CB_GETCURSEL, 0, 0);
	if(lResult != CB_ERR)
	{
		char szBuf2[MAX_PATH];
		::SendDlgItemMessage(hDlg, IDC_COLOR_FILE, CB_GETLBTEXT, lResult, (LPARAM)szBuf2);
		app.PenColorsSetFile(szBuf2);
		app.PenColorsLoad(app.GetAppPath() + "\\Pens\\Colors\\" + szBuf2);
	}

	// default view
	lResult = ::SendDlgItemMessage(hDlg, IDC_DEFAULT_VIEW, CB_GETCURSEL, 0, 0);
	if(lResult != CB_ERR)
	{
		switch(lResult)
		{
			case 0:	pView->SetDefaultView(CPegView::VIEW_UNCHANGED);	break;
			case 1:	pView->SetDefaultView(CPegView::VIEW_OVERVIEW);		break;
//			case 2:	pView->SetDefaultView(CPegView::VIEW_BEST);			break;
		}
	}

	// background color
	lResult = ::SendDlgItemMessage(hDlg, IDC_BACK_COLOR, CB_GETCURSEL, 0, 0L);
	app.BackgroundColorSet(lResult);
}

void SetupAppearance_DrawEntire(LPDRAWITEMSTRUCT lpDIS, int inflate)
{
	CDC dc;
	dc.Attach(lpDIS->hDC);

//	char szBuf[32];

	CRect rc;
	::CopyRect(&rc, &lpDIS->rcItem);

//	::SendMessage(lpDIS->hwndItem, CB_GETLBTEXT, lpDIS->itemID, (LPARAM) (LPCSTR) szBuf);

//	dc.SetTextColor(RGB(0, 0, 0));
//	dc.SetBkColor(RGB(255, 255, 255)), 	

//	dc.ExtTextOut(rc.right - 16, rc.top + 2, ETO_CLIPPED, &rc, szBuf, lstrlen(szBuf), 0);

	::InflateRect(&rc, inflate - 2, inflate - 2);
//	rc.right -= 24;
	
	if(!bChanged)
	{
		CBrush brush(crHotCols[lpDIS->itemID]);
		dc.FillRect(&rc, &brush);
	}
	else
	{
		CBrush brush(crHotColsTemp[lpDIS->itemID]);
		dc.FillRect(&rc, &brush);
	}

//	dc.FillRect(&rc, &brush);

	OwnerDraw_Select(lpDIS, inflate);
	OwnerDraw_Focus(lpDIS, inflate);
	
	dc.Detach();
}