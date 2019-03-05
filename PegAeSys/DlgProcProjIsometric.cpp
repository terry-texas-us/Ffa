#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

namespace isometric
{
	int iLeftRight = 0;
	int iFrontBack = 0; 
	int iAboveUnder = 0;
}

using namespace isometric;

BOOL CALLBACK DlgProcProjIsometric(HWND hDlg, UINT nMsg, WPARAM, LPARAM)
{
//	CPegDoc *pDoc = CPegDoc::GetDoc();

	CVec	vVwUp;
	CVec	vDirection;

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			::SendDlgItemMessage(hDlg, IDC_VIEW_ISO_LEFT + iLeftRight, BM_SETCHECK, 1, 0L);
			::SendDlgItemMessage(hDlg, IDC_VIEW_ISO_FRONT + iFrontBack, BM_SETCHECK, 1, 0L);
			::SendDlgItemMessage(hDlg, IDC_VIEW_ISO_ABOVE + iAboveUnder, BM_SETCHECK, 1, 0L);
			
			return (TRUE);
		}
		case WM_DIALOG_FOCUS:
		{
			SetFocus(GetWindow(hDlg,GW_CHILD));
			return TRUE;
		}
		case WM_DIALOG_OK:
		{
			iLeftRight = (IsDlgButtonChecked(hDlg, IDC_VIEW_ISO_LEFT) != 0) ? 0 : 1;
			vDirection[0] = .5773503;
			if (iLeftRight != 0)
				vDirection[0] = - .5773503;
			iFrontBack = (IsDlgButtonChecked(hDlg, IDC_VIEW_ISO_FRONT) != 0) ? 0 : 1;
			vDirection[1] = .5773503;
			if (iFrontBack != 0) 
				vDirection[1] = - .5773503;
			iAboveUnder = (IsDlgButtonChecked(hDlg, IDC_VIEW_ISO_ABOVE) != 0) ? 0 : 1;
			vDirection[2] = - .5773503;
			if (iAboveUnder != 0) 
				vDirection[2] = .5773503;
			
			view::SetEye(-vDirection);
			view::SetDirection(-vDirection);
			view::SetPerspectiveEnabled(false);
			
			vVwUp = vDirection ^ ZDIR;
			vVwUp = vVwUp ^ vDirection;
			vVwUp.Normalize();
			view::SetVwUp(vVwUp);
			view::SetCenteredWnd();

			return TRUE;
		}
	}
	return (FALSE); 
}