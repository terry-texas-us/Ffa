#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysView.h"

#include "Grid.h"
using namespace grid;

BOOL CALLBACK DlgProcOptionsTabConstraints(HWND hDlg, UINT nMsg, WPARAM, LPARAM)
{
	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			DlgBoxSetItemUnitsText(hDlg, IDC_USR_GRID_X_INT, dXSnapInt);
			DlgBoxSetItemUnitsText(hDlg, IDC_USR_GRID_Y_INT, dYSnapInt);
			DlgBoxSetItemUnitsText(hDlg, IDC_USR_GRID_Z_INT, dZSnapInt);
			
			DlgBoxSetItemUnitsText(hDlg, IDC_GRID_DOT_INT_X, dXDotInt);
			DlgBoxSetItemUnitsText(hDlg, IDC_GRID_DOT_INT_Y, dYDotInt);
			DlgBoxSetItemUnitsText(hDlg, IDC_GRID_DOT_INT_Z, dZDotInt);
			
			DlgBoxSetItemUnitsText(hDlg, IDC_GRID_LN_INT_X, dXLnInt);
			DlgBoxSetItemUnitsText(hDlg, IDC_GRID_LN_INT_Y, dYLnInt);
			DlgBoxSetItemUnitsText(hDlg, IDC_GRID_LN_INT_Z, dZLnInt);
			
			::CheckDlgButton(hDlg, IDC_GRID_SNAP_ON, IsSnapEnabled());
			::CheckDlgButton(hDlg, IDC_GRID_PTS_DIS_ON, GetDotsState());
			::CheckDlgButton(hDlg, IDC_GRID_LNS_DIS_ON, GetLinesState());

			return TRUE;
		}
		case WM_DIALOG_FOCUS:
		{
			SetFocus(GetWindow(hDlg,GW_CHILD));
			return TRUE;
		}
		case WM_DIALOG_OK:
		{
			dXSnapInt = DlgBoxGetItemUnitsText(hDlg, IDC_USR_GRID_X_INT);
			dYSnapInt = DlgBoxGetItemUnitsText(hDlg, IDC_USR_GRID_Y_INT);
			dZSnapInt = DlgBoxGetItemUnitsText(hDlg, IDC_USR_GRID_Z_INT);
			
			dXDotInt = DlgBoxGetItemUnitsText(hDlg, IDC_GRID_DOT_INT_X);
			dYDotInt = DlgBoxGetItemUnitsText(hDlg, IDC_GRID_DOT_INT_Y);
			dZDotInt = DlgBoxGetItemUnitsText(hDlg, IDC_GRID_DOT_INT_Z);
			
			dXLnInt = DlgBoxGetItemUnitsText(hDlg, IDC_GRID_LN_INT_X);
			dYLnInt = DlgBoxGetItemUnitsText(hDlg, IDC_GRID_LN_INT_Y);
			dZLnInt = DlgBoxGetItemUnitsText(hDlg, IDC_GRID_LN_INT_Z);
			
			SetSnapState(IsDlgButtonChecked(hDlg, IDC_GRID_SNAP_ON) == BST_CHECKED);
			SetDotsState(IsDlgButtonChecked(hDlg, IDC_GRID_PTS_DIS_ON) == BST_CHECKED);
			SetLinesState(IsDlgButtonChecked(hDlg, IDC_GRID_LNS_DIS_ON) == BST_CHECKED);

			CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);

			return TRUE;
		}
	}
	return (FALSE); 
}