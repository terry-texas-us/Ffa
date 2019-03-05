#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

BOOL CALLBACK DlgProcProjOblique(HWND hDlg, UINT nMsg, WPARAM, LPARAM)
{
//	CPegDoc *pDoc = CPegDoc::GetDoc();

	double	dAng, dPhi, dTheta;
	CPnt ptProjRefPt;
	CVec vVwPlnNorm;
	CVec vVwUp;

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			vVwPlnNorm = view::GetDirection();
			dAng = acos(- vVwPlnNorm[2]) / RADIAN;
			DlgBoxSetItemDouble(hDlg, IDC_VIEW_VPN_Z_ANG, dAng);
			dAng = Vec_Angle_xy(- vVwPlnNorm) / RADIAN;
			DlgBoxSetItemDouble(hDlg, IDC_VIEW_VPN_X_ANG, dAng);
			dAng = acos(- ptProjRefPt[2]) / RADIAN;
			DlgBoxSetItemDouble(hDlg, IDC_VIEW_DOP_Z_ANG, dAng);
			dAng = Vec_Angle_xy(- (ptProjRefPt - ORIGIN)) / RADIAN;
			DlgBoxSetItemDouble(hDlg, IDC_VIEW_DOP_X_ANG, dAng);
			
			return (TRUE);
		}
		case WM_DIALOG_FOCUS:
		{
			SetFocus(GetWindow(hDlg,GW_CHILD));
			return TRUE;
		}
		case WM_DIALOG_OK:
		{
			dPhi = Arc(DlgBoxGetItemDouble(hDlg, IDC_VIEW_VPN_Z_ANG));
			dTheta = Arc(DlgBoxGetItemDouble(hDlg, IDC_VIEW_VPN_X_ANG));
						
			vVwPlnNorm = CVec(- sin(dPhi) * cos(dTheta), - sin(dPhi) * sin(dTheta), - cos(dPhi));
			
			dPhi = Arc(DlgBoxGetItemDouble(hDlg, IDC_VIEW_DOP_Z_ANG));
			dTheta = Arc(DlgBoxGetItemDouble(hDlg, IDC_VIEW_DOP_X_ANG));
			
			ptProjRefPt = CPnt(- sin(dPhi) * cos(dTheta), - sin(dPhi) * sin(dTheta), - cos(dPhi));
			
			if (fabs(dPhi) <= DBL_EPSILON) 
				vVwUp(- cos(dTheta), - sin(dTheta), 0.);
			else if (fabs(dPhi - PI) <= DBL_EPSILON) 
				vVwUp(cos(dTheta), sin(dTheta), 0.);
			else 
			{
				vVwUp = vVwPlnNorm ^ ZDIR;
				vVwUp = vVwUp ^ vVwPlnNorm;
				vVwUp.Normalize();
				if (dPhi > PI)
					vVwUp = - vVwUp;
			}
			view::SetDirection(- vVwPlnNorm);
			view::SetVwUp(vVwUp);
			view::SetPerspectiveEnabled(false);

			return TRUE;
		}
	}
	return (FALSE); 
}

//#include "stdafx.h"
//
//BOOL CALLBACK DlgProcProjOblique(HWND hDlg, UINT anMsg, WPARAM wParam,	LPARAM	)
//{
//	double	dAng, dPhi, dTheta;
//	CPnt ptProjRefPt;
//	CVec vVwPlnNorm;
//	CVec vVwUp;
//
//	switch (anMsg) 
//	{
//		case WM_INITDIALOG:
//			vVwPlnNorm = view::GetDirection();
//			dAng = acos(- vVwPlnNorm[2]) / RADIAN;
//			DlgBoxSetItemDouble(hDlg, IDC_VIEW_VPN_Z_ANG, dAng);
//			dAng = Vec_Angle_xy(- vVwPlnNorm) / RADIAN;
//			DlgBoxSetItemDouble(hDlg, IDC_VIEW_VPN_X_ANG, dAng);
//			dAng = acos(- ptProjRefPt[2]) / RADIAN;
//			DlgBoxSetItemDouble(hDlg, IDC_VIEW_DOP_Z_ANG, dAng);
//			dAng = Vec_Angle_xy(- (ptProjRefPt - ORIGIN)) / RADIAN;
//			DlgBoxSetItemDouble(hDlg, IDC_VIEW_DOP_X_ANG, dAng);
//			return (TRUE);
//
//		case WM_COMMAND:
//			switch (LOWORD(wParam)) 
//			{
//				case IDOK:
//					dPhi = Arc(DlgBoxGetItemDouble(hDlg, IDC_VIEW_VPN_Z_ANG));
//					dTheta = Arc(DlgBoxGetItemDouble(hDlg, IDC_VIEW_VPN_X_ANG));
//								
//					vVwPlnNorm = CVec(- sin(dPhi) * cos(dTheta), - sin(dPhi) * sin(dTheta), - cos(dPhi));
//					
//					dPhi = Arc(DlgBoxGetItemDouble(hDlg, IDC_VIEW_DOP_Z_ANG));
//					dTheta = Arc(DlgBoxGetItemDouble(hDlg, IDC_VIEW_DOP_X_ANG));
//					
//					ptProjRefPt = CPnt(- sin(dPhi) * cos(dTheta), - sin(dPhi) * sin(dTheta), - cos(dPhi));
//					
//					if (fabs(dPhi) <= DBL_EPSILON) 
//						vVwUp(- cos(dTheta), - sin(dTheta), 0.);
//					else if (fabs(dPhi - PI) <= DBL_EPSILON) 
//						vVwUp(cos(dTheta), sin(dTheta), 0.);
//					else 
//					{
//						vVwUp = vVwPlnNorm ^ ZDIR;
//						vVwUp = vVwUp ^ vVwPlnNorm;
//						vVwUp.Normalize();
//						if (dPhi > PI)
//							vVwUp = - vVwUp;
//					}
//					view::SetDirection(- vVwPlnNorm);
//					view::SetVwUp(vVwUp);
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