#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

BOOL CALLBACK DlgProcFileClean(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
{
	switch(nMsg)
	{
		case WM_INITDIALOG:
			return (TRUE);
			break;
		case WM_COMMAND:
			CPegDoc* pDoc = CPegDoc::GetDoc();

			switch(LOWORD(wParam)) 
			{
				case IDC_CLEAN_ALL:
				{
					UINT bState = ::IsDlgButtonChecked(hDlg, IDC_CLEAN_ALL);
					if(bState == BST_CHECKED)
					{
						::CheckDlgButton(hDlg, IDC_CLEAN_STYLES, BST_CHECKED);
						::CheckDlgButton(hDlg, IDC_CLEAN_BLOCKS, BST_CHECKED);
						::CheckDlgButton(hDlg, IDC_CLEAN_LAYERS, BST_CHECKED);
						::CheckDlgButton(hDlg, IDC_CLEAN_NOTES, BST_CHECKED);
						::CheckDlgButton(hDlg, IDC_CLEAN_SEGS, BST_CHECKED);
						::CheckDlgButton(hDlg, IDC_CLEAN_DUPL, BST_CHECKED);

						::CheckDlgButton(hDlg, IDC_FLAT_IRON, BST_CHECKED);
					}
					else if(bState == BST_UNCHECKED)
					{
						::CheckDlgButton(hDlg, IDC_CLEAN_STYLES, BST_UNCHECKED);
						::CheckDlgButton(hDlg, IDC_CLEAN_BLOCKS, BST_UNCHECKED);
						::CheckDlgButton(hDlg, IDC_CLEAN_LAYERS, BST_UNCHECKED);
						::CheckDlgButton(hDlg, IDC_CLEAN_NOTES, BST_UNCHECKED);
						::CheckDlgButton(hDlg, IDC_CLEAN_SEGS, BST_UNCHECKED);
						::CheckDlgButton(hDlg, IDC_CLEAN_DUPL, BST_UNCHECKED);

						::CheckDlgButton(hDlg, IDC_FLAT_IRON, BST_UNCHECKED);
					}
					break;
				}
				case IDOK:
				{
					UINT bState;
					int lRes = -1;
					char szBuf[16];

					HCURSOR hOldCursor = ::SetCursor(LoadCursor(0, IDC_WAIT));

					bState = ::IsDlgButtonChecked(hDlg, IDC_CLEAN_DUPL);
					if(bState == BST_CHECKED) 
					{
						lRes = pDoc->RemoveDuplicateSegments();
						SetWindowText(::GetDlgItem(hDlg, IDC_REM_DUP_SEG), itoa(lRes, szBuf, 10));
					}

					bState = ::IsDlgButtonChecked(hDlg, IDC_CLEAN_STYLES);
					if(bState == BST_CHECKED)
					{
						lRes = pDoc->PenStylesRemoveUnused();
						SetWindowText(::GetDlgItem(hDlg, IDC_REM_STY), itoa(lRes, szBuf, 10));
					}

					bState = ::IsDlgButtonChecked(hDlg, IDC_CLEAN_BLOCKS);
					if(bState == BST_CHECKED)
					{
						lRes = pDoc->BlksRemoveUnused();
						SetWindowText(::GetDlgItem(hDlg, IDC_REM_BLK), itoa(lRes, szBuf, 10));
					}

					bState = ::IsDlgButtonChecked(hDlg, IDC_CLEAN_LAYERS);
					if(bState == BST_CHECKED)
					{
						lRes = pDoc->LayersRemoveEmpty();
						SetWindowText(::GetDlgItem(hDlg, IDC_REM_LAY), itoa(lRes, szBuf, 10));
					}

					bState = ::IsDlgButtonChecked(hDlg, IDC_CLEAN_NOTES);
					if(bState == BST_CHECKED)
					{
						lRes = pDoc->RemoveEmptyNotes();
						SetWindowText(::GetDlgItem(hDlg, IDC_REM_NOT), itoa(lRes, szBuf, 10));
					}

					bState = ::IsDlgButtonChecked(hDlg, IDC_CLEAN_SEGS);
					if(bState == BST_CHECKED) 
					{
						lRes = pDoc->RemoveEmptySegs();
						SetWindowText(::GetDlgItem(hDlg, IDC_REM_EMP_SEG), itoa(lRes, szBuf, 10));
					}

					bState = ::IsDlgButtonChecked(hDlg, IDC_FLAT_IRON);
					if(bState == BST_CHECKED) 
					{
						pDoc->OnIronDrawing();
						SetWindowText(::GetDlgItem(hDlg, IDC_IRON_STATUS), "Complete");
					}

					::SetCursor(hOldCursor);

					break;
				}
				case IDCANCEL:
					::EndDialog(hDlg, IDCANCEL);
					return (FALSE);
			}
	}
	return (FALSE); 
}