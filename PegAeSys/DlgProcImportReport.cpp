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

BOOL CALLBACK DlgProcImportReport(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
{
	switch (nMsg) 
	{
		case WM_INITDIALOG:
		{
			for(int i = 0; i < (int)AcadMode::strXREFs.size(); i++)
				::SendDlgItemMessage(hDlg, IDC_XREFLIST, LB_ADDSTRING, 0, (LPARAM) (LPCSTR) AcadMode::strXREFs[i]);

			return TRUE;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDOK:
				case IDCANCEL:
					::EndDialog(hDlg, TRUE);
			}
		}
	}
	return FALSE;
}