#include "stdafx.h"

//#include "PegAEsys.h"
////#include "PegAEsysDoc.h"
//
//#include "commctrl.h"
//
//#define PBM_SETMARQUEE (WM_USER+10)
//
//BOOL CALLBACK DlgProcProgress(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
//{
//	switch(nMsg)
//	{
//		case WM_INITDIALOG:
//		{
//			LONG lValue;
//			lValue = GetWindowLongPtr(::GetDlgItem(hDlg, IDD_PROGRESS), GWL_STYLE);
//			lValue |= PBM_SETMARQUEE;
//			SetWindowLongPtr(::GetDlgItem(hDlg, IDD_PROGRESS), GWL_STYLE, lValue);
//			SendMessage(::GetDlgItem(hDlg, IDD_PROGRESS), PBM_SETMARQUEE, true, 50);
//			return (TRUE);
//			break;
//		}
//		case WM_COMMAND:
//		{
//			switch (LOWORD(wParam)) 
//			{
//				case IDOK:
//					::EndDialog(hDlg, IDOK);
//					return (TRUE);
//				case IDCANCEL:
//					::EndDialog(hDlg, IDCANCEL);
//					return (FALSE);
//			}
//		}
//	}
//	return (FALSE); 
//}