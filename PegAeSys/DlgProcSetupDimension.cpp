#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "SubProcAnnotate.h"

BOOL CALLBACK DlgProcSetupTabDimension(HWND hDlg, UINT nMsg, WPARAM, LPARAM)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	// annotate
	char *szArrTyp[] = {"Open", "Closed", "Mark", "Hash"};
	int i;

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			// annotate
			//SetDlgItemText(hDlg, IDC_ANN_DEF_TXT, annotate::szDefText);
			//DlgBoxSetItemDouble(hDlg, IDC_ANN_GAP_SPACE_FAC, annotate::dGapSpacing);
			//DlgBoxSetItemDouble(hDlg, IDC_ANN_HOOK_RAD, annotate::dHookRadius);
			
			for (i = 0; i < sizeof(szArrTyp) / sizeof(szArrTyp[0]); i++)
				::SendDlgItemMessage(hDlg, IDC_ANN_ARR_TYP2, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) szArrTyp[i]);

			::SendDlgItemMessage(hDlg, IDC_ANN_ARR_TYP2, CB_SETCURSEL, (WPARAM) (annotate::nArrowType - 1), 0L);

			DlgBoxSetItemDouble(hDlg, IDC_ANN_ARR_SIZ, annotate::dArrowSize);
			//DlgBoxSetItemDouble(hDlg, IDC_ANN_BUB_RAD, annotate::dBubbleRadius);
			//SetDlgItemInt(hDlg, IDC_ANN_BUB_FACETS, annotate::nBubbleFacets, FALSE);

			DlgBoxSetItemDouble(hDlg, IDC_DIM_CONST, annotate::dConstraint);
			
			return TRUE;
			break;
		}
		case WM_DIALOG_FOCUS:
		{
			switch(pDoc->GetOptionsFocus())
			{
				case IDC_DIM_CONST:
					SetFocus(::GetDlgItem(hDlg, IDC_DIM_CONST));
					break;
				default:
					SetFocus(GetWindow(hDlg,GW_CHILD));
					break;
			}

			return TRUE;
			break;
		}
		case WM_DIALOG_OK:
		{
			// annotate
			//::GetDlgItemText(hDlg, IDC_ANN_DEF_TXT, annotate::szDefText, sizeof(annotate::szDefText));

			//annotate::dGapSpacing = DlgBoxGetItemDouble(hDlg, IDC_ANN_GAP_SPACE_FAC);
			//annotate::dHookRadius = DlgBoxGetItemDouble(hDlg, IDC_ANN_HOOK_RAD);

			annotate::nArrowType = (int) ::SendDlgItemMessage(hDlg, IDC_ANN_ARR_TYP2, CB_GETCURSEL, 0, 0L) + 1;

			annotate::dArrowSize = DlgBoxGetItemDouble(hDlg, IDC_ANN_ARR_SIZ);
			//annotate::dBubbleRadius = DlgBoxGetItemDouble(hDlg, IDC_ANN_BUB_RAD);
			//annotate::nBubbleFacets = (WORD) GetDlgItemInt(hDlg, IDC_ANN_BUB_FACETS, 0, FALSE);

			annotate::dConstraint = DlgBoxGetItemDouble(hDlg, IDC_DIM_CONST);

			return TRUE;
			break;
		}
	}
	return FALSE; 
}