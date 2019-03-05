#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

void DlgProcProjPerspectiveDoOK(HWND);
void DlgProcProjPerspectiveInit(HWND);

BOOL CALLBACK DlgProcProjPerspective(HWND hDlg, UINT nMsg, WPARAM, LPARAM)
{
//	CPegDoc *pDoc = CPegDoc::GetDoc();

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			DlgProcProjPerspectiveInit(hDlg);
			return (TRUE);
		}
		case WM_DIALOG_FOCUS:
		{
			SetFocus(GetWindow(hDlg,GW_CHILD));
			return TRUE;
		}
		case WM_DIALOG_OK:
		{
			DlgProcProjPerspectiveDoOK(hDlg);
			return TRUE;
		}
	}
	return (FALSE); 
}

//#include "stdafx.h"
//
//void DlgProcProjPerspectiveDoOK(HWND);
//void DlgProcProjPerspectiveInit(HWND);
//
//BOOL CALLBACK DlgProcProjPerspective(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM  )
//{
//	switch (anMsg) 
//	{
//		case WM_INITDIALOG:
//			DlgProcProjPerspectiveInit(hDlg);
//			return (TRUE);
//
//		case WM_COMMAND:		// message: received a command
//			switch (LOWORD(wParam)) 
//			{
//				case IDOK:
//					DlgProcProjPerspectiveDoOK(hDlg);
//
//					::EndDialog(hDlg, TRUE);
//					return (TRUE);
//				
//				case IDCANCEL:
//					::EndDialog(hDlg, FALSE);
//					return (FALSE);
//			}
//	}
//	return (FALSE); 											
//}

void DlgProcProjPerspectiveInit(HWND hDlg)
{
	CVec vDirection = view::GetDirection();
	
	double dDist = view::GetLensLength();

	DlgBoxSetItemDouble(hDlg, IDC_VIEW_DIS_TO_TARGET, dDist);
}

void DlgProcProjPerspectiveDoOK(HWND hDlg)
{
	double dDis = DlgBoxGetItemUnitsText(hDlg, IDC_VIEW_DIS_TO_TARGET);
					
	view::SetLensLength(dDis);
	view::SetEye(view::GetDirection());
	view::SetPerspectiveEnabled(true);
}			