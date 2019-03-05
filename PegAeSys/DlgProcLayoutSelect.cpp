#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include <deque>
using std::deque;

namespace AcadMode
{
	extern deque<CString> strLayouts;
	extern CString strSelectedLayout;
	extern bool bVectorize;
	extern deque<CString> strXREFs;
}

BOOL CALLBACK DlgProcLayoutSelect(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
{
	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			if(AcadMode::bVectorize)
				::CheckRadioButton(hDlg, IDC_PRIM_VECTORIZE, IDC_PRIM_DATA, IDC_PRIM_VECTORIZE);
			else
				::CheckRadioButton(hDlg, IDC_PRIM_VECTORIZE, IDC_PRIM_DATA, IDC_PRIM_DATA);

			for(int i = 0; i < (int)AcadMode::strLayouts.size(); i++)
				::SendDlgItemMessage(hDlg, IDC_LAYOUT, LB_ADDSTRING, 0, (LPARAM) (LPCSTR) AcadMode::strLayouts[i]);

			//::SendDlgItemMessage(hDlg, IDC_LAYOUT, LB_SETCURSEL, 0, 0L); 

			// select "model"
			char szModel[] = "model";
			::SendDlgItemMessage(hDlg, IDC_LAYOUT, LB_SELECTSTRING, (WPARAM)-1, (LPARAM)szModel); 
	
			return (TRUE);
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{
				case IDOK:
				{
					if(::IsDlgButtonChecked(hDlg, IDC_PRIM_VECTORIZE))
						AcadMode::bVectorize = true;
					else
						AcadMode::bVectorize = false;

					LRESULT lResult = ::SendDlgItemMessage(hDlg, IDC_LAYOUT, LB_GETCURSEL, 0, 0);
					if(lResult != CB_ERR)
					{
						char szBuf2[MAX_PATH];
						::SendDlgItemMessage(hDlg, IDC_LAYOUT,  LB_GETTEXT, lResult, (LPARAM)szBuf2);
						AcadMode::strSelectedLayout = szBuf2;
					}

					::EndDialog(hDlg, IDOK);
					return (TRUE);
				}
				case IDCANCEL:
					::EndDialog(hDlg, IDCANCEL);
					return (FALSE);
			}
		}
	}
	return (FALSE); 
}