#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "StateInfoFrm.h"
#include "DlgProcPrintPreview.h"

#include "DlgProcPlotPreview.h"
extern CPlotFrm plotframe;

//////////////////////////////////////////////////////////////////////////////////////////////////

CStateFrm stateframe(0, 100, 200, 300);
CStateFrm::CStateFrm(int x, int y, int width, int height) { SetInfo(x, y, width, height); }
void CStateFrm::OnPaint(HWND) {}

//////////////////////////////////////////////////////////////////////////////////////////////////

void DlgProcStatePreviewLoad(HWND hDlg, WPARAM wParam, int nIndex);

BOOL CALLBACK DlgProcStatePreview(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
{
	static int nSelect = IDC_PLOT_1;

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			// for print 1
			::CheckRadioButton(hDlg, IDC_PLOT_1, IDC_PLOT_4, nSelect);
			printStates.SetCurrentState(0);
			return (TRUE);
		}
		case WM_SIZE:
		{
			RECT cRect;
			::GetClientRect(app.GetSafeHwnd(), &cRect);

			POINT point;
			point.x = cRect.right - 94;
			point.y = cRect.bottom - 361;

			SetWindowPos(hDlg, HWND_TOPMOST, point.x, point.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			return (TRUE);
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{
				case IDC_PLOT_BUTTON:
				{
					if(HIWORD(wParam) == BN_CLICKED)
					{
						plotframe.ToggleState();
						::SetFocus(app.GetSafeHwnd());
					}
					return (TRUE);
				}
				case IDC_PLOT_1:
				{
					DlgProcStatePreviewLoad(hDlg, wParam, 0);
					return (TRUE);
				}
				case IDC_PLOT_2:
				{
					DlgProcStatePreviewLoad(hDlg, wParam, 1);
					return (TRUE);
				}
				case IDC_PLOT_3:
				{
					DlgProcStatePreviewLoad(hDlg, wParam, 2);
					return (TRUE);
				}
				case IDC_PLOT_4:
				{
					DlgProcStatePreviewLoad(hDlg, wParam, 3);
					return (TRUE);
				}
			}
			break;
		}
	}
	return (FALSE); 
}

void DlgProcStatePreviewLoad(HWND hDlg, WPARAM wParam, int nIndex)
{
	printStates.SetCurrentState(nIndex);
	if(nIndex < 2)
		::SetWindowText(::GetDlgItem(hDlg, IDC_PLOT_BUTTON), "Print Mode");
	else
		::SetWindowText(::GetDlgItem(hDlg, IDC_PLOT_BUTTON), "Plot Mode");
	plotframe.Refresh();

	if(HIWORD(wParam) == BN_CLICKED)
		::SetFocus(app.GetSafeHwnd());

	CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);
}