#include "stdafx.h"

#include "PegAEsys.h"

#include "SubProcPipe.h"

void DlgProcPipeSymbolInit(HWND);

BOOL CALLBACK DlgProcPipeSymbol(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM)
{
	
	switch (anMsg) 
	{
		case WM_INITDIALOG:
			DlgProcPipeSymbolInit(hDlg);
			return (TRUE);

		case WM_COMMAND:
			switch (LOWORD(wParam)) 
			{
				case IDOK:
					pipe::wCurSymId = WORD(::SendDlgItemMessage(hDlg, IDC_LIST, LB_GETCURSEL, 0, 0L));
					
				case IDCANCEL:
					::EndDialog(hDlg, TRUE);
					return (TRUE);
			}
			break;

		break;	
	}
	return (FALSE); 		
}

void DlgProcPipeSymbolInit(HWND hDlg)
{
	char szNames[512];
	char* pName;
	SecureZeroMemory(szNames, 512);
	::LoadString(app.GetInstance(), IDS_SUBPROC_PIPE_SYMBOL_NAMES, szNames, 256);
	
	pName = strtok(szNames, "\t");
	while (pName != 0)
	{		
		::SendDlgItemMessage(hDlg, IDC_LIST, LB_ADDSTRING, 0, (LPARAM) (LPCSTR) pName);
		pName = strtok(0, "\t");
	}
	::SendDlgItemMessage(hDlg, IDC_LIST, LB_SETCURSEL, pipe::wCurSymId, 0L);
}