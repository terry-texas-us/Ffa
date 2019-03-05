#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysView.h"

#include "Scroll.h"

BOOL CALLBACK DlgProcOptionsTabScrollbars(HWND hDlg, UINT nMsg, WPARAM, LPARAM)
{
	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			SetDlgItemInt(hDlg, IDC_SCROLL_HSIZE, scroll::nHScrollThumbSize, false);						  
			SetDlgItemInt(hDlg, IDC_SCROLL_HSENS, scroll::nHScrollSens, false);
			SetDlgItemInt(hDlg, IDC_SCROLL_VSIZE, scroll::nVScrollThumbSize, false);
			SetDlgItemInt(hDlg, IDC_SCROLL_VSENS, scroll::nVScrollSens, false);
			
			return (TRUE);
		}
		case WM_DIALOG_FOCUS:
		{
			SetFocus(GetWindow(hDlg,GW_CHILD));
			return TRUE;
		}
		case WM_DIALOG_OK:
		{
			scroll::nHScrollThumbSize = ::GetDlgItemInt(hDlg, IDC_SCROLL_HSIZE, 0, false);						  
			scroll::nHScrollSens = ::GetDlgItemInt(hDlg, IDC_SCROLL_HSENS, 0, false);
			scroll::nVScrollThumbSize = ::GetDlgItemInt(hDlg, IDC_SCROLL_VSIZE, 0, false);
			scroll::nVScrollSens = ::GetDlgItemInt(hDlg, IDC_SCROLL_VSENS, 0, false);
			CPegView::GetActiveView()->UpdateScroll();

			return TRUE;
			break;
		}
	}
	return (FALSE); 
}