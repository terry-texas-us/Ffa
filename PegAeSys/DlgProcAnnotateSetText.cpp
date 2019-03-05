#include "stdafx.h"

#include "SubProcAnnotate.h"

//extern CProfileManager profManager;

BOOL CALLBACK DlgProcAnnotateEnterText(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM)
{
//	SPrefAnnotate prefAnnotate = profManager.GetAnnotatePrefs();

	switch (anMsg)
	{
		case WM_INITDIALOG:
//			SetDlgItemText(hDlg, IDC_TEXT, prefAnnotate.GetText());
			SetDlgItemText(hDlg, IDC_TEXT, annotate::szCurTxt);
			return (TRUE);
	
		case WM_COMMAND:
			switch (LOWORD(wParam)) 
			{
				case IDOK:
					::GetDlgItemText(hDlg, IDC_TEXT, annotate::szCurTxt, sizeof(annotate::szCurTxt));
//					char szBuf[64];
//					::GetDlgItemText(hDlg, IDC_TEXT, szBuf, sizeof(szBuf));
//					prefAnnotate.SetText(szBuf);
//					profManager.SetAnnotatePrefs(prefAnnotate);
			
				case IDCANCEL:
					::EndDialog(hDlg, TRUE);
					return (TRUE);
			}
	}
	return (FALSE); 		
}