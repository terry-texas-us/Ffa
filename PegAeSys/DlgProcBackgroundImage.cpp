#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "DlgProcBackgroundImage.h"

CBackgroundImage backgroundImage(0, 100, 200, 300);
CBackgroundImage::CBackgroundImage(int x, int y, int width, int height) { SetInfo(x, y, width, height); }
void CBackgroundImage::OnPaint(HWND) {}

BOOL CALLBACK DlgProcBackgroundImage(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
{
//	CPegDoc *pDoc = CPegDoc::GetDoc();
	CPegView *pView = CPegView::GetActiveView();

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			SendMessage(::GetDlgItem(hDlg, IDC_TRANS_SLIDER), TBM_SETRANGE, TRUE, MAKELPARAM(0, 255));
			SendMessage(::GetDlgItem(hDlg, IDC_TRANS_SLIDER), TBM_SETTIC, 0, 0);
			SendMessage(::GetDlgItem(hDlg, IDC_TRANS_SLIDER), TBM_SETTIC, 0, 63);
			SendMessage(::GetDlgItem(hDlg, IDC_TRANS_SLIDER), TBM_SETTIC, 0, 127);
			SendMessage(::GetDlgItem(hDlg, IDC_TRANS_SLIDER), TBM_SETTIC, 0, 191);
			SendMessage(::GetDlgItem(hDlg, IDC_TRANS_SLIDER), TBM_SETTIC, 0, 255);
			SendMessage(::GetDlgItem(hDlg, IDC_TRANS_SLIDER), TBM_SETPOS, TRUE, pView->GetBackgroundTrans());

			return (TRUE);
		}
		case WM_SIZE:
		{
			RECT cRect;
			::GetClientRect(app.GetSafeHwnd(), &cRect);

			POINT point;
			point.x = cRect.right - 181;
			point.y = cRect.bottom - 361;

			SetWindowPos(hDlg, HWND_TOPMOST, point.x, point.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

			return (TRUE);
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{
				case IDC_SHOW_HIDE:
				{
					pView->ShowBackgroundImage(!pView->IsBackgroundImageOn());
					break;
				}
				case ID_REMOVE_IMAGE:
				{
					pView->OnBackgroundImageRemove();
					break;
				}
				case IDC_MOVE_POINT:
				{
					if(::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_PASTE_POSITION), app.GetSafeHwnd(), DlgProcSetPasteLoc) > 0)
					{
						backgroundImage.SetPoint(app.CursorPosGet());
						pView->InvalidateRect(0, TRUE);
					}
					break;
				}
			}

			break;
		}
		case WM_HSCROLL:
		{
			if(LOWORD(wParam) == TB_ENDTRACK)
			{
				LRESULT lResult = SendMessage(::GetDlgItem(hDlg, IDC_TRANS_SLIDER), TBM_GETPOS, 0, 0);
				pView->SetBackgroundTrans(lResult);
				::SetFocus(app.GetSafeHwnd());
				pView->InvalidateRect(0, TRUE);
			}

			return (TRUE);
		}
	}
	return (FALSE); 
}