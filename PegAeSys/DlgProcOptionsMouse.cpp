#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

//extern char	szLeftMouseDown[60];
//extern char	szRightMouseDown[60]; 
//extern char	szLeftMouseUp[60];
//extern char	szRightMouseUp[60];

namespace mouse
{
	char szLeftMouseDown[60]  = "";
	char szRightMouseDown[60] = ""; 
	char szLeftMouseUp[60]	= "{13}"; //default return
	char szRightMouseUp[60] = "{27}"; //default escape
}

BOOL CALLBACK DlgProcOptionsTabMouse(HWND hDlg, UINT nMsg, WPARAM, LPARAM)
{
//	CPegDoc *pDoc = CPegDoc::GetDoc();

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			SetDlgItemText(hDlg, IDC_LEFT_DOWN, mouse::szLeftMouseDown);						  
			SetDlgItemText(hDlg, IDC_LEFT_UP, mouse::szLeftMouseUp);
			SetDlgItemText(hDlg, IDC_RIGHT_DOWN, mouse::szRightMouseDown);
			SetDlgItemText(hDlg, IDC_RIGHT_UP, mouse::szRightMouseUp);
			
			return (TRUE);
		}
		case WM_DIALOG_FOCUS:
		{
			SetFocus(GetWindow(hDlg,GW_CHILD));
			return TRUE;
		}
		case WM_DIALOG_OK:
		{
			::GetDlgItemText(hDlg, IDC_LEFT_DOWN, mouse::szLeftMouseDown, sizeof(mouse::szLeftMouseDown));
			::GetDlgItemText(hDlg, IDC_LEFT_UP, mouse::szLeftMouseUp, sizeof(mouse::szLeftMouseUp));
			::GetDlgItemText(hDlg, IDC_RIGHT_DOWN, mouse::szRightMouseDown, sizeof(mouse::szRightMouseDown));
			::GetDlgItemText(hDlg, IDC_RIGHT_UP, mouse::szRightMouseUp, sizeof(mouse::szRightMouseUp));

			return TRUE;
		}
	}
	return (FALSE); 
}