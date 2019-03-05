#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

BOOL CALLBACK DlgProcProjAxonometric(HWND hDlg, UINT nMsg, WPARAM, LPARAM)
{
//	CPegDoc *pDoc = CPegDoc::GetDoc();

	CVec	vDirection;
	CVec	vVwUp;
	CPnt	ptEye;
	CPnt	ptTarget;

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			vDirection = view::GetDirection();
			DlgBoxSetItemDouble(hDlg, IDC_VPN_X, vDirection[0]);
			DlgBoxSetItemDouble(hDlg, IDC_VPN_Y, vDirection[1]);
			DlgBoxSetItemDouble(hDlg, IDC_VPN_Z, vDirection[2]);
			
			vVwUp = view::GetVwUp();
			DlgBoxSetItemDouble(hDlg, IDC_VUP_X, vVwUp[0]);
			DlgBoxSetItemDouble(hDlg, IDC_VUP_Y, vVwUp[1]);
			DlgBoxSetItemDouble(hDlg, IDC_VUP_Z, vVwUp[2]);

			ptTarget = view::GetTarget();
			DlgBoxSetItemDouble(hDlg, IDC_TARGET_X, ptTarget[0]);
			DlgBoxSetItemDouble(hDlg, IDC_TARGET_Y, ptTarget[1]);
			DlgBoxSetItemDouble(hDlg, IDC_TARGET_Z, ptTarget[2]);
			
			ptEye = view::GetEye();
			DlgBoxSetItemDouble(hDlg, IDC_PRP_X, ptEye[0]);
			DlgBoxSetItemDouble(hDlg, IDC_PRP_Y, ptEye[1]);
			DlgBoxSetItemDouble(hDlg, IDC_PRP_Z, ptEye[2]);
			
			return (TRUE);
		}
		case WM_DIALOG_FOCUS:
		{
			SetFocus(GetWindow(hDlg,GW_CHILD));
			return TRUE;
		}
		case WM_DIALOG_OK:
		{
			ptEye[0] = DlgBoxGetItemDouble(hDlg, IDC_PRP_X);
			ptEye[1] = DlgBoxGetItemDouble(hDlg, IDC_PRP_Y);
			ptEye[2] = DlgBoxGetItemDouble(hDlg, IDC_PRP_Z);
			
			view::SetEye(ptEye);
			
			ptTarget[0] = DlgBoxGetItemDouble(hDlg, IDC_TARGET_X);
			ptTarget[1] = DlgBoxGetItemDouble(hDlg, IDC_TARGET_Y);
			ptTarget[2] = DlgBoxGetItemDouble(hDlg, IDC_TARGET_Z);
			
			view::SetTarget(ptTarget);

			vDirection = ptTarget - ptEye;
			view::SetDirection(vDirection);
			
			vVwUp[0] = DlgBoxGetItemDouble(hDlg, IDC_VUP_X);
			vVwUp[1] = DlgBoxGetItemDouble(hDlg, IDC_VUP_Y);
			vVwUp[2] = DlgBoxGetItemDouble(hDlg, IDC_VUP_Z);

			//vVwUp = ZDIR ^ vDirection;
			vVwUp.Normalize();

			if (vVwUp.IsNull())
				vVwUp = YDIR;

			view::SetVwUp(vVwUp);
			view::SetCenteredWnd();
			view::SetPerspectiveEnabled(false);

			return TRUE;
		}
	}
	return (FALSE); 
}


//#include "stdafx.h"
//
//
//BOOL CALLBACK DlgProcProjAxonometric(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM  )
//{
//	CVec	vDirection;
//	CVec	vVwUp;
//	
//	CPnt	ptEye;
//	CPnt	ptTarget;
//
//	switch (anMsg) 
//	{
//		case WM_INITDIALOG:
//			vDirection = view::GetDirection();
//			DlgBoxSetItemDouble(hDlg, IDC_VPN_X, vDirection[0]);
//			DlgBoxSetItemDouble(hDlg, IDC_VPN_Y, vDirection[1]);
//			DlgBoxSetItemDouble(hDlg, IDC_VPN_Z, vDirection[2]);
//			
//			vVwUp = view::GetVwUp();
//			DlgBoxSetItemDouble(hDlg, IDC_VUP_X, vVwUp[0]);
//			DlgBoxSetItemDouble(hDlg, IDC_VUP_Y, vVwUp[1]);
//			DlgBoxSetItemDouble(hDlg, IDC_VUP_Z, vVwUp[2]);
//
//			ptTarget = view::GetTarget();
//			DlgBoxSetItemDouble(hDlg, IDC_TARGET_X, ptTarget[0]);
//			DlgBoxSetItemDouble(hDlg, IDC_TARGET_Y, ptTarget[1]);
//			DlgBoxSetItemDouble(hDlg, IDC_TARGET_Z, ptTarget[2]);
//			
//			ptEye = view::GetEye();
//			DlgBoxSetItemDouble(hDlg, IDC_PRP_X, ptEye[0]);
//			DlgBoxSetItemDouble(hDlg, IDC_PRP_Y, ptEye[1]);
//			DlgBoxSetItemDouble(hDlg, IDC_PRP_Z, ptEye[2]);
//						
//			return (TRUE);
//
//		case WM_COMMAND:
//			switch (LOWORD(wParam)) 
//			{
//				case IDOK:
//					ptEye[0] = DlgBoxGetItemDouble(hDlg, IDC_PRP_X);
//					ptEye[1] = DlgBoxGetItemDouble(hDlg, IDC_PRP_Y);
//					ptEye[2] = DlgBoxGetItemDouble(hDlg, IDC_PRP_Z);
//					
//					view::SetEye(ptEye);
//					
//					ptTarget[0] = DlgBoxGetItemDouble(hDlg, IDC_TARGET_X);
//					ptTarget[1] = DlgBoxGetItemDouble(hDlg, IDC_TARGET_Y);
//					ptTarget[2] = DlgBoxGetItemDouble(hDlg, IDC_TARGET_Z);
//					
//					view::SetTarget(ptTarget);
//
//					vDirection = ptTarget - ptEye;
//					view::SetDirection(vDirection);
//					
//					vVwUp[0] = DlgBoxGetItemDouble(hDlg, IDC_VUP_X);
//					vVwUp[1] = DlgBoxGetItemDouble(hDlg, IDC_VUP_Y);
//					vVwUp[2] = DlgBoxGetItemDouble(hDlg, IDC_VUP_Z);
//
//					//vVwUp = ZDIR ^ vDirection;
//					vVwUp.Normalize();
//
//					if (vVwUp.IsNull())
//						vVwUp = YDIR;
//
//					view::SetVwUp(vVwUp);
//					view::SetCenteredWnd();
//					view::SetPerspectiveEnabled(false);
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