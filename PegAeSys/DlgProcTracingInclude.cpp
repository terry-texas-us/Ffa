#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "Preview.h"

UINT CALLBACK OFNHookProcTracingInclude(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	static bool bPreview = true;

	switch(uiMsg) 
	{
		case WM_INITDIALOG:
		{
			if(bPreview)
				::CheckDlgButton(hDlg, IDC_TRACING_PREVIEW, BST_CHECKED);

//			SendMessage(GetParent(hDlg), CDM_HIDECONTROL, (WPARAM)IDOK, (LPARAM)0);
//			SendMessage(GetParent(hDlg), CDM_HIDECONTROL, (WPARAM)IDCANCEL, (LPARAM)0);
			WndProcPreviewClear(hDlg);
			return (TRUE);
		}
		case WM_NOTIFY:
		{
			LPOFNOTIFY lpofn = (LPOFNOTIFY) lParam;
			if(lpofn->hdr.code == CDN_FOLDERCHANGE)
			{
				WndProcPreviewClear(hDlg);
			}
			else if(lpofn->hdr.code == CDN_SELCHANGE)
			{
				char psz[MAX_PATH];
				CommDlg_OpenSave_GetFilePath(GetParent(hDlg), psz, MAX_PATH);
				
				CFileStatus	fs;
				if(CFile::GetStatus(psz, fs))
				{
					if(!bPreview) return TRUE;

					CLayer* pLayer = pDoc->LayersGet(psz);
					
					if(pLayer != 0)
						WndProcPreviewUpdate(hDlg, pLayer);
					else
					{
						pLayer = new CLayer;
						
						pDoc->TracingLoadLayer(psz, pLayer);
						WndProcPreviewUpdate(hDlg, pLayer);
					
						pLayer->RemoveSegs();
						delete pLayer;
					}
				}
			}
			return (TRUE);
		}
		case WM_COMMAND:
		{
			char szFilePath[MAX_PATH];
			CommDlg_OpenSave_GetFilePath(GetParent(hDlg), szFilePath, MAX_PATH);
			
			CString strFileName(szFilePath);
			
			CFileStatus	fs;
			if(!CFile::GetStatus(strFileName, fs))
			{
				msgWarning(IDS_MSG_FILE_NOT_FOUND, strFileName);
				return (TRUE);
			}

			switch(LOWORD(wParam)) 
			{
				case IDC_TRACING_PREVIEW:
				{
					if(HIWORD(wParam) == BN_CLICKED)
					{
						bPreview = !bPreview;

						if(!bPreview)
							WndProcPreviewClear(hDlg);
						else
						{
							CLayer* pLayer = pDoc->LayersGet(strFileName);
					
							if (pLayer != 0)
								WndProcPreviewUpdate(hDlg, pLayer);
							else
							{
								pLayer = new CLayer;
								
								pDoc->TracingLoadLayer(strFileName, pLayer);
								WndProcPreviewUpdate(hDlg, pLayer);
							
								pLayer->RemoveSegs();
								delete pLayer;
							}
						}
					}
					return TRUE;
				}
				/////////////////////////////////////////////////////////////////////
				case IDC_APPEND:
				{
					CLayer*	pLayer = pDoc->WorkLayerGet();
					pDoc->TracingLoadLayer(strFileName, pLayer);
					pDoc->UpdateAllViews(NULL, CPegDoc::HINT_LAYER_SAFE, pLayer);										
					return (TRUE);
				}
				case IDC_TRAMAP:
				{
					if (pDoc->TracingMap(szFilePath))
					{
						CString strName = szFilePath;
						CLayer* pLayer = pDoc->LayersGet(strName);
						pLayer->SetStateFlg(CLayer::STATE_FLG_RESIDENT);
						
						strName = strName.Right(strName.GetLength() - strName.ReverseFind('\\'));
						strName.TrimLeft('\\');
						pLayer->SetName(strName);

						PostMessage(GetParent(hDlg), WM_COMMAND, MAKEWPARAM(IDC_TRAMAP_RETURN, 0), 0);
					}
					return TRUE;
				}
				case IDC_TRAP:
				{
					CLayer* pLayer = new CLayer();
					pDoc->TracingLoadLayer(szFilePath, pLayer);

					trapsegs.RemoveAll();
					trapsegs.AddTail(pLayer);
					DoEditTrapCopy();
					trapsegs.RemoveAll();
					
					pLayer->RemoveSegs();
					delete pLayer;

					// mak - added so that the dialog for pasting is displayed immediately
					pDoc->OnEditTrapPaste();
					
					return (TRUE);
				}
				case IDC_TRAVIEW:
				{
					if(pDoc->TracingView(szFilePath))
					{
						CString strName = szFilePath;
						CLayer* pLayer = pDoc->LayersGet(strName);
						pLayer->SetStateFlg(CLayer::STATE_FLG_RESIDENT);
						
						strName = strName.Right(strName.GetLength() - strName.ReverseFind('\\'));
						strName.TrimLeft('\\');
						pLayer->SetName(strName);
					}
					return (TRUE);
				}
				case IDC_TRACING_SOLO:
				{
					// create new instance of pegaesys
					SECURITY_ATTRIBUTES shouldInherit;
					shouldInherit.nLength = sizeof(SECURITY_ATTRIBUTES);
					shouldInherit.lpSecurityDescriptor = NULL;
					shouldInherit.bInheritHandle = TRUE;

					// create new process
					STARTUPINFO info;
					GetStartupInfo(&info);
					PROCESS_INFORMATION pi;

					// get pathname for executable
					TCHAR szPath[MAX_PATH];
					GetModuleFileName(app.GetInstance(), szPath, MAX_PATH);
					CString strExePath = szPath;

					CreateProcess(strExePath,strFileName.GetBuffer(),&shouldInherit,&shouldInherit,true,0,0,0,&info,&pi);

					CloseHandle(&pi);
					return TRUE;
				}
				//////////////////////////////////////////////////////////////////
				case IDCANCEL:
					PostMessage(GetParent(hDlg), WM_COMMAND, MAKEWPARAM(IDCANCEL, 0), 0);
					return FALSE;
			}
			return TRUE;
		}		
	}
	return (FALSE); 		// Message for default dialog handlers
}