#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

BOOL CALLBACK DlgProcFileArchive(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			CLayer *pLayerZero = pDoc->LayersGetAt(pDoc->LayersLookup("0"));
			if(pLayerZero != 0)
			{
				if(pLayerZero->GetCount() > 0)
					::CheckDlgButton(hDlg, IDC_MOVE_ZERO, BST_CHECKED);
				else
				{
					::EnableWindow(::GetDlgItem(hDlg, IDC_MOVE_ZERO), false);
					::EnableWindow(::GetDlgItem(hDlg, IDC_MOVE_LAYERS), false);
					::EnableWindow(::GetDlgItem(hDlg, IDC_ARCHIVE_STATIC), false);
					::EnableWindow(::GetDlgItem(hDlg, IDC_ARCHIVE_STATIC2), false);
				}
			}
			::CheckDlgButton(hDlg, IDC_FUSE_ALL, BST_CHECKED);

			HWND hwndLayers = ::GetDlgItem(hDlg, IDC_MOVE_LAYERS);

			// populate field with all layers
			for(int i = 0; i < pDoc->LayersGetSize(); i++)
			{
				CLayer *pLayer = pDoc->LayersGetAt(i);

				if(pLayer->IsInternal())
				{
					CString LayName = pLayer->GetName();
					SendMessage(hwndLayers, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)LayName.GetBuffer());
				}
			}

			SendMessage(hwndLayers, CB_DELETESTRING, 0, 0);
			SendMessage(hwndLayers, CB_SETCURSEL, 0, 0);

			return (TRUE);
			break;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{
				case IDC_MOVE_ZERO:
				{
					UINT bState = ::IsDlgButtonChecked(hDlg, IDC_MOVE_ZERO);

					if(bState == BST_CHECKED)
					{
						::EnableWindow(::GetDlgItem(hDlg, IDC_MOVE_LAYERS), true);
						::EnableWindow(::GetDlgItem(hDlg, IDC_ARCHIVE_STATIC), true);
						::EnableWindow(::GetDlgItem(hDlg, IDC_ARCHIVE_STATIC2), true);
					}
					else if(bState == BST_UNCHECKED)
					{
						::EnableWindow(::GetDlgItem(hDlg, IDC_MOVE_LAYERS), false);
						::EnableWindow(::GetDlgItem(hDlg, IDC_ARCHIVE_STATIC), false);
						::EnableWindow(::GetDlgItem(hDlg, IDC_ARCHIVE_STATIC2), false);
					}
					
					break;
				}
				case IDOK:
				{
					UINT bState;
					UINT bStateFuseAll;
					CString strLayers;

					HCURSOR hOldCursor = ::SetCursor(LoadCursor(0, IDC_WAIT));

					bStateFuseAll = ::IsDlgButtonChecked(hDlg, IDC_FUSE_ALL);
					if(bStateFuseAll == BST_CHECKED) 
					{
						pDoc->TracingFuseAll(strLayers);
					}

					bState = ::IsDlgButtonChecked(hDlg, IDC_MOVE_ZERO);
					if(bState == BST_CHECKED)
					{
						char szBuffer[64];
						CLayer* pLayer;

						HWND hWndCtrl = ::GetDlgItem(hDlg, IDC_MOVE_LAYERS);
						SendMessage(hWndCtrl, WM_GETTEXT, (WPARAM)64, (LPARAM)szBuffer);
						int nLayerIndex = pDoc->LayersLookup(szBuffer);
						if(nLayerIndex == -1)
						{
							pLayer = new CLayer(szBuffer);
							pDoc->LayersAdd(pLayer);
						}
						else
							pLayer = pDoc->LayersGetAt(pDoc->LayersLookup(szBuffer));

						CLayer *pLayerZero = pDoc->LayersGetAt(pDoc->LayersLookup("0"));
						if(pLayerZero != 0 && pLayer != 0)
						{
							POSITION pos = pLayerZero->GetHeadPosition();
							while(pos != 0)
							{
								CSeg *pSeg = pLayerZero->GetNext(pos);
								pLayer->AddTail(pSeg);
								pLayerZero->Remove(pSeg);
							}
						}
					}

					// saves the file
					pDoc->OnFileExportArchive();

					// set working directory so melting files puts them in the original directory
					bStateFuseAll = ::IsDlgButtonChecked(hDlg, IDC_FUSE_ALL);
					if(bStateFuseAll == BST_CHECKED) 
					{
						CString strPathName = pDoc->GetPathName();
						CString strWDirectory = strPathName.Left(strPathName.ReverseFind('\\'));
						SetCurrentDirectory(strWDirectory);
						pDoc->LayerMeltAllWithoutSaving(strLayers);
					}

					::SetCursor(hOldCursor);

					::EndDialog(hDlg, IDOK);
					return TRUE;
					break;
				}
				case IDCANCEL:
					::EndDialog(hDlg, IDCANCEL);
					return FALSE;
			}
		}
	}
	return (FALSE); 
}