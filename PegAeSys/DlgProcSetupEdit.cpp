
//This is mostly Terry Smith code from 2000.
//Previously ProcEditOps.cpp
//Matt Keslin Added the vernier switch command uses case statement rather than if
//case WM_DIALOG_FOCUS: was added - not sure why 
//variable nMsg used instead of anMsg
//case IDCANCEL is omitted

#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "DlgProcEditOps.h"

BOOL CALLBACK DlgProcSetupTabEdit(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			DlgBoxSetItemDouble(hDlg, IDC_EDIT_OP_SIZ_X, dlgproceditops::dXScale);
			DlgBoxSetItemDouble(hDlg, IDC_EDIT_OP_SIZ_Y, dlgproceditops::dYScale);
			DlgBoxSetItemDouble(hDlg, IDC_EDIT_OP_SIZ_Z, dlgproceditops::dZScale);

			DlgBoxSetItemDouble(hDlg, IDC_EDIT_OP_ROT_X, dlgproceditops::dXRotAng);
			DlgBoxSetItemDouble(hDlg, IDC_EDIT_OP_ROT_Y, dlgproceditops::dYRotAng);
			DlgBoxSetItemDouble(hDlg, IDC_EDIT_OP_ROT_Z, dlgproceditops::dZRotAng);

			DlgBoxSetItemDouble(hDlg, IDC_VERNIER_SIZE, dlgproceditops::dVernierSize);
			DlgBoxSetItemDouble(hDlg, IDC_VERNIER_ROT, dlgproceditops::dVernierRot);


			if (dlgproceditops::dXMirrorScale < 0.)
				::CheckDlgButton(hDlg, IDC_EDIT_OP_MIR_X, 1);
			else if (dlgproceditops::dYMirrorScale < 0.)
				::CheckDlgButton(hDlg, IDC_EDIT_OP_MIR_Y, 1);
			else
				::CheckDlgButton(hDlg, IDC_EDIT_OP_MIR_X, 1); // NLF changed from _Z to _X 012418

			return TRUE;
		}
		case WM_DIALOG_FOCUS:
		{
			switch(pDoc->GetOptionsFocus())
			{
				case IDC_EDIT_OP_SIZING:
					SetFocus(::GetDlgItem(hDlg, IDC_EDIT_OP_SIZING));
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
				case IDC_EDIT_OP_SIZING:
					::CheckRadioButton(hDlg, IDC_EDIT_OP_SIZING, IDC_EDIT_OP_MIRRORING, IDC_EDIT_OP_SIZING);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_SIZ_X), TRUE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_SIZ_Y), TRUE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_SIZ_Z), TRUE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_ROT_X), FALSE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_ROT_Y), FALSE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_ROT_Z), FALSE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_MIR_X), FALSE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_MIR_Y), FALSE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_MIR_Z), FALSE);
					return (TRUE);

				case IDC_EDIT_OP_ROTATION:
					::CheckRadioButton(hDlg, IDC_EDIT_OP_SIZING, IDC_EDIT_OP_MIRRORING, IDC_EDIT_OP_ROTATION);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_SIZ_X), FALSE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_SIZ_Y), FALSE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_SIZ_Z), FALSE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_ROT_X), TRUE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_ROT_Y), TRUE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_ROT_Z), TRUE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_MIR_X), FALSE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_MIR_Y), FALSE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_MIR_Z), FALSE);
					return (TRUE);

				case IDC_EDIT_OP_MIRRORING:
					::CheckRadioButton(hDlg, IDC_EDIT_OP_SIZING, IDC_EDIT_OP_ROTATION, IDC_EDIT_OP_MIRRORING);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_SIZ_X), FALSE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_SIZ_Y), FALSE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_SIZ_Z), FALSE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_ROT_X), FALSE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_ROT_Y), FALSE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_ROT_Z), FALSE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_MIR_X), TRUE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_MIR_Y), TRUE);
					::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_MIR_Z), TRUE); 
					return (TRUE);
									
				case IDC_EDIT_OP_MIR_X:
					::CheckDlgButton(hDlg, IDC_EDIT_OP_MIR_Y, 0);
					::CheckDlgButton(hDlg, IDC_EDIT_OP_MIR_Z, 0);
					return (TRUE);

				case IDC_EDIT_OP_MIR_Y:
					::CheckDlgButton(hDlg, IDC_EDIT_OP_MIR_X, 0);
					::CheckDlgButton(hDlg, IDC_EDIT_OP_MIR_Z, 0);
					return (TRUE);

				case IDC_EDIT_OP_MIR_Z:
					::CheckDlgButton(hDlg, IDC_EDIT_OP_MIR_X, 0);
					::CheckDlgButton(hDlg, IDC_EDIT_OP_MIR_Y, 0);
					return (TRUE);
			}
			return TRUE;
		}
		case WM_DIALOG_OK:
		{
			dlgproceditops::dXScale = DlgBoxGetItemDouble(hDlg, IDC_EDIT_OP_SIZ_X);
			dlgproceditops::dYScale = DlgBoxGetItemDouble(hDlg, IDC_EDIT_OP_SIZ_Y);
			dlgproceditops::dZScale = DlgBoxGetItemDouble(hDlg, IDC_EDIT_OP_SIZ_Z);
			
			dlgproceditops::dXRotAng = DlgBoxGetItemDouble(hDlg, IDC_EDIT_OP_ROT_X);
			dlgproceditops::dYRotAng = DlgBoxGetItemDouble(hDlg, IDC_EDIT_OP_ROT_Y);
			dlgproceditops::dZRotAng = DlgBoxGetItemDouble(hDlg, IDC_EDIT_OP_ROT_Z);

			dlgproceditops::dVernierSize = DlgBoxGetItemDouble(hDlg, IDC_VERNIER_SIZE);
			dlgproceditops::dVernierRot = DlgBoxGetItemDouble(hDlg, IDC_VERNIER_ROT);

			// make sure value is between 0 and 1
			dlgproceditops::dVernierSize = fabs(dlgproceditops::dVernierSize - (int)dlgproceditops::dVernierSize);
			dlgproceditops::dVernierRot = fabs(dlgproceditops::dVernierRot - (int)dlgproceditops::dVernierRot);
			
			if (IsDlgButtonChecked(hDlg, IDC_EDIT_OP_MIR_X))
				dlgproceditops::SetMirrorScale(- 1., 1., 1.);
			else if (IsDlgButtonChecked(hDlg, IDC_EDIT_OP_MIR_Y))
				dlgproceditops::SetMirrorScale(1., - 1., 1.);
			else
//NLF			dlgproceditops::SetMirrorScale(1., 1., -1.);
				dlgproceditops::SetMirrorScale(1., 1., 1.);

			return TRUE;
		}
	}
	return FALSE; 
}
