#include "stdafx.h"

#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "SubProcDraw.h"
#include "SubProcFixup.h"
#include "UserAxis.h"

//#include "dde.h"
//#include "ddeGItms.h"

void DlgProcOptionsScaleInit(HWND hDlg);
void DlgProcOptionsScaleOK(HWND hDlg);

BOOL CALLBACK DlgProcSetupTabDraw(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			// drawing - arc
			if(draw::eArcType == draw::ARC_THREE_POINT)
				::CheckRadioButton(hDlg, IDC_ARC_3_POINT, IDC_ARC_CENTER_2_POINTS, IDC_ARC_3_POINT);
			else
				::CheckRadioButton(hDlg, IDC_ARC_3_POINT, IDC_ARC_CENTER_2_POINTS, IDC_ARC_CENTER_2_POINTS);

			// drawing - circle
			if(draw::eCircleType == draw::CIRCLE_CENTER_RADIUS)
				::CheckRadioButton(hDlg, IDC_CIRCLE_CR, IDC_CIRCLE_DIAM, IDC_CIRCLE_CR);
			else
				::CheckRadioButton(hDlg, IDC_CIRCLE_CR, IDC_CIRCLE_DIAM, IDC_CIRCLE_DIAM);

			// drawing - curve
			if(draw::eCurveType == draw::CURVE_SPLINE)
				::CheckRadioButton(hDlg, IDC_CURVE_SPLINE, IDC_CURVE_FITTED, IDC_CURVE_SPLINE);
			else
				::CheckRadioButton(hDlg, IDC_CURVE_SPLINE, IDC_CURVE_FITTED, IDC_CURVE_FITTED);

			// influence angle
			DlgBoxSetItemDouble(hDlg, IDC_INFL_ANGLE, fixup::dInfluenceAngle);

			// marker type
			SendMessage(::GetDlgItem(hDlg, IDC_TYPE_MARKER), (UINT)CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)"Pixel");
			SendMessage(::GetDlgItem(hDlg, IDC_TYPE_MARKER), (UINT)CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)"<None>");
			SendMessage(::GetDlgItem(hDlg, IDC_TYPE_MARKER), (UINT)CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)"Plus");
			SendMessage(::GetDlgItem(hDlg, IDC_TYPE_MARKER), (UINT)CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)"Cross");
			::SendDlgItemMessage(hDlg, IDC_TYPE_MARKER, CB_SETCURSEL, pstate.MarkStyle(), 0);

			// scale
			DlgProcOptionsScaleInit(hDlg);

			return (TRUE);
		}
		case WM_DIALOG_FOCUS:
		{
			switch(pDoc->GetOptionsFocus())
			{
				case IDC_ARC_3_POINT:
					SetFocus(::GetDlgItem(hDlg, IDC_ARC_3_POINT));
					break;
				case IDC_INFL_ANGLE:
					SendDlgItemMessage(hDlg, IDC_INFL_ANGLE, EM_SETSEL, 0, -1);
					SetFocus(::GetDlgItem(hDlg, IDC_INFL_ANGLE));
					break;
				default:
					SetFocus(GetWindow(hDlg,GW_CHILD));
					break;
			}
			return TRUE;
		}	
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{
				case IDC_SCALE_USER:
				{
					if(::IsDlgButtonChecked(hDlg, IDC_SCALE_USER))
					{
						::EnableWindow(::GetDlgItem(hDlg, IDC_SCALE), true);
						::EnableWindow(::GetDlgItem(hDlg, IDC_SCALE2), false);
					}
					else
					{
						::EnableWindow(::GetDlgItem(hDlg, IDC_SCALE), false);
						::EnableWindow(::GetDlgItem(hDlg, IDC_SCALE2), true);
					}
					break;
				}
			}
			return TRUE;
		}
		case WM_DIALOG_OK:
		{
			// scale
			DlgProcOptionsScaleOK(hDlg);

			// influence angle
			fixup::dInfluenceAngle = DlgBoxGetItemDouble(hDlg, IDC_INFL_ANGLE);
			UserAxis::UserAxisSetInfAng(fixup::dInfluenceAngle);

			// marker type
			LRESULT lResult2 = ::SendDlgItemMessage(hDlg, IDC_TYPE_MARKER, CB_GETCURSEL, 0, 0);
			if(lResult2 != CB_ERR)
				pstate.SetMarkStyle((short)lResult2);

			CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);

			// drawing arc
			if(::IsDlgButtonChecked(hDlg, IDC_ARC_3_POINT))
				draw::eArcType = draw::ARC_THREE_POINT;
			else
				draw::eArcType = draw::ARC_C2_POINT;

			// drawing circle
			if(::IsDlgButtonChecked(hDlg, IDC_CIRCLE_CR))
				draw::eCircleType = draw::CIRCLE_CENTER_RADIUS;
			else
				draw::eCircleType = draw::CIRCLE_DIAMETER;

			// drawing curve
			if(::IsDlgButtonChecked(hDlg, IDC_CURVE_SPLINE))
				draw::eCurveType = draw::CURVE_SPLINE;
			else
				draw::eCurveType = draw::CURVE_FITTED;

			return TRUE;
		} 
	}
	return FALSE; 
}