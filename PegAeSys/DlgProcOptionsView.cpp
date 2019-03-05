#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

//#include "dde.h"
//#include "ddeGItms.h"

BOOL CALLBACK DlgProcOptionsTabView(HWND hDlg, UINT nMsg, WPARAM, LPARAM)
{
	CPegView* pView = CPegView::GetActiveView();

	CString str;
	char* pStr;
	double dAngle;
	char szBuf[32];

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			DlgBoxSetItemDouble(hDlg, IDC_NEAR, view::GetNearClipDistance());
			DlgBoxSetItemDouble(hDlg, IDC_FAR, view::GetFarClipDistance());
			DlgBoxSetItemDouble(hDlg, IDC_ANGLE, Vec_Angle_xy(view::GetVwUp()) / RADIAN);

			CString str;
			str.Format("%2.2f", view::GetEye()[0]);	
			SetWindowText(::GetDlgItem(hDlg, IDC_STATIC_EYEX), str.GetBuffer());

			str.Format("%2.2f", view::GetEye()[1]);
			SetWindowText(::GetDlgItem(hDlg, IDC_STATIC_EYEY), str.GetBuffer());

			str.Format("%2.2f", view::GetEye()[2]);
			SetWindowText(::GetDlgItem(hDlg, IDC_STATIC_EYEZ), str.GetBuffer());

			str.Format("%2.2f", view::GetTarget()[0]);
			SetWindowText(::GetDlgItem(hDlg, IDC_STATIC_TARGX), str.GetBuffer());

			str.Format("%2.2f", view::GetTarget()[1]);
			SetWindowText(::GetDlgItem(hDlg, IDC_STATIC_TARGY), str.GetBuffer());

			str.Format("%2.2f", view::GetTarget()[2]);
			SetWindowText(::GetDlgItem(hDlg, IDC_STATIC_TARGZ), str.GetBuffer());
			
			return (TRUE);
		}
		case WM_DIALOG_FOCUS:
		{
			SetFocus(GetWindow(hDlg,GW_CHILD));
			return TRUE;
		}
		case WM_DIALOG_OK:
		{
			pStr = str.GetBuffer(64);

			::GetDlgItemText(hDlg, IDC_NEAR, szBuf, sizeof(szBuf));
			view::SetNearClipDistance(atof(szBuf));
			::GetDlgItemText(hDlg, IDC_FAR, szBuf, sizeof(szBuf));
			view::SetFarClipDistance(atof(szBuf));
			::GetDlgItemText(hDlg, IDC_ANGLE, pStr, 64);
			str.ReleaseBuffer();

			if (str.CompareNoCase("EngAngZ") == 0)
				dAngle = 90. + app.GetEngAngZ() / RADIAN;
			else if (str.CompareNoCase("DimAngZ") == 0)
				dAngle = 90. + app.GetDimAngZ();
			else
				dAngle = atof(str);
		
			view::SetVwUp(CVec(cos(dAngle * RADIAN), sin(dAngle * RADIAN), 0.));
			pView->InvalidateRect(NULL, TRUE);

			return TRUE;
		}
	}
	return (FALSE); 
}