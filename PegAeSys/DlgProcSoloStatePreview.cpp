#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "filejob.h"

#include "StateInfoFrm.h"
#include "DlgProcPrintPreview.h"

#include "DlgProcPlotPreview.h"

//////////////////////////////////////////////////////////////////////////////////////////////////

CSoloStateFrm soloStateFrame(0, 100, 200, 300);
CSoloStateFrm::CSoloStateFrm(int x, int y, int width, int height) { SetInfo(x, y, width, height); }
void CSoloStateFrm::OnPaint(HWND) {}

//////////////////////////////////////////////////////////////////////////////////////////////////

void DlgProcSoloStateRestore(HWND hDlg);

BOOL CALLBACK DlgProcSoloStatePreview(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();
//	CPegView *pView = CPegView::GetActiveView();

//	UINT bState;
//	bool bValue;

	static int nSelect = IDC_PLOT_1;

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			return (TRUE);
		}
		case WM_SIZE:
		{
			RECT cRect;
			::GetClientRect(app.GetSafeHwnd(), &cRect);

			POINT point;
			point.x = cRect.right - 190;
			point.y = cRect.bottom - 361;

			SetWindowPos(hDlg, HWND_TOPMOST, point.x, point.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

			return (TRUE);
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{
				case IDC_SOLO_SAVE:
				{
					switch(app.GetSoloType())
					{
						case SOLO_LAYER:
						{
							CLayer *pSoloLayer = pDoc->WorkLayerGet();
							if(pSoloLayer == 0) break;
							pDoc->WorkLayerSet(pDoc->LayersGet("0"));
							CString strLayerName = pSoloLayer->GetName().TrimLeft("solo:");
							CLayer *pOldLayer = pDoc->LayersGet(strLayerName);
							if(pOldLayer == 0) break;
							WORD wStateFlg = pOldLayer->GetStateFlgs();
							pDoc->LayersRemove(strLayerName);
							pSoloLayer->SetName(strLayerName);

							pSoloLayer->ClrStateFlg();
							pSoloLayer->SetStateFlg(wStateFlg);
							if(strLayerName == "0")
								pDoc->WorkLayerSet(pSoloLayer);
							break;
						}
						case SOLO_TRACING:
						{
							CLayer *pLayer = pDoc->WorkLayerGet();
							if(pLayer == 0) break;
							pDoc->WorkLayerSet(pDoc->LayersGet("0"));
							CString strLayerName = pLayer->GetName().TrimLeft("solo:");
							CLayer *pOldLayer = pDoc->LayersGetAt(pDoc->LayersLookup(strLayerName));
							if(pOldLayer == 0) break;

							WORD wTracingFlg = pOldLayer->GetTracingFlgs();
							WORD wStateFlg = pOldLayer->GetStateFlgs();
							pDoc->LayersRemove(strLayerName);

							bool bRetVal = false;

							WORD wFileType = app.GetFileTypeFromPath(strLayerName);
							if(pDoc->FileTypeIsTracing(wFileType))
							{
								if(wFileType == CPegDoc::FILE_TYPE_TRACING)
								{
									CFileTracing ft;
									if(ft.OpenForWrite(strLayerName))
									{
										ft.WriteHeader();
										ft.WriteSegs(pLayer);
										bRetVal = true;
										ft.Close();
									}
								}
								else
								{
									CFileJob fj;
									if(fj.OpenForWrite(strLayerName))
									{
										fj.WriteHeader();
										fj.WriteSegs(pLayer);
										bRetVal = true;
										fj.Close();
									}
								}
								if(bRetVal)
								{
									pLayer->ClrStateFlg();
									pLayer->SetStateFlg(CLayer::STATE_FLG_RESIDENT);
									
									pLayer->SetStateCold();
									
									pLayer->SetTracingFlg(wTracingFlg);
									pLayer->SetStateFlg(wStateFlg);
								
									pLayer->SetName(strLayerName);
								}
							}

							break;
						}
						case SOLO_BLOCK:
						{
							CLayer *pSoloLayer = pDoc->WorkLayerGet();
							if(pSoloLayer == 0) break;
							
							CString strBlockName = pSoloLayer->GetName().TrimLeft("solo:");

							pDoc->OnEditTrapWork();
		//					if (trapsegs.GetCount() == 0) break;
							
							CBlock* pBlock;	
							pBlock = new CBlock;
							
							POSITION pos = trapsegs.GetHeadPosition();
							while(pos != 0)
							{
								CSeg* pSeg = trapsegs.GetNext(pos);
								CSeg* pSeg2 = new CSeg(*pSeg);
								pBlock->AddTail(pSeg2);
								pSeg2->RemoveAll();
								delete pSeg2;
							}

							CBlock *pBlockOld;
							pDoc->BlksLookup(strBlockName, pBlockOld);
							pBlock->SetBasePt(pBlockOld->GetBasePt());
							pDoc->BlksSetAt(strBlockName, pBlock);

							pDoc->WorkLayerSet(pDoc->LayersGet("0"));
							pDoc->OnEditTrapQuit();
							pDoc->LayersRemove(pSoloLayer->GetName());
							break;
						}
					}

					DlgProcSoloStateRestore(hDlg);
					return (TRUE);
				}
				case IDC_SOLO_DISCARD:
				{
					switch(app.GetSoloType())
					{
						case SOLO_LAYER:
						{
							CLayer *pLayer = pDoc->WorkLayerGet();
							pDoc->WorkLayerSet(pDoc->LayersGet("0"));
							pDoc->LayersRemove(pLayer->GetName());
							break;
						}
						case SOLO_TRACING:
						{
							CLayer *pLayer = pDoc->WorkLayerGet();
							pDoc->WorkLayerSet(pDoc->LayersGet("0"));
							pDoc->LayersRemove(pLayer->GetName());
							break;
						}
						case SOLO_BLOCK:
						{
							CLayer *pLayer = pDoc->WorkLayerGet();
							pDoc->WorkLayerSet(pDoc->LayersGet("0"));
							pDoc->LayersRemove(pLayer->GetName());
							break;
						}
					}

					DlgProcSoloStateRestore(hDlg);
					return (TRUE);
				}
			}

			break;
		}
	}
	return (FALSE); 
}

extern CString strSoloHideLayers;

void DlgProcSoloStateRestore(HWND)// hDlg)
{
	app.SetPegState(STATE_NORMAL);

	::ShowWindow(soloStateFrame.GetHWND(), SW_HIDE);

	EnableMenuItem(app.GetMenu(), IDM_FILE_NEW, MF_ENABLED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_OPEN, MF_ENABLED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_SAVE, MF_ENABLED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_SAVEAS, MF_ENABLED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_CLOSE, MF_ENABLED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_MANAGE, MF_ENABLED);
	EnableMenuItem(app.GetMenu(), ID_FILE_PLOT, MF_ENABLED);
	EnableMenuItem(app.GetMenu(), ID_FILE_MRU1, MF_ENABLED);
	EnableMenuItem(app.GetMenu(), ID_FILE_MRU2, MF_ENABLED);
	EnableMenuItem(app.GetMenu(), ID_FILE_MRU3, MF_ENABLED);
	EnableMenuItem(app.GetMenu(), ID_FILE_MRU4, MF_ENABLED);
	EnableMenuItem(app.GetMenu(), ID_FILE_MRU5, MF_ENABLED);
	EnableMenuItem(app.GetMenu(), ID_FILE_RECOVER, MF_ENABLED);
	EnableMenuItem(app.GetMenu(), ID_VIEW_PRINTPREVIEW, MF_ENABLED);
	EnableMenuItem(app.GetMenu(), ID_EDIT_SEGTOWORK, MF_ENABLED);

	// import and export
	HMENU hSubMenu = ::GetSubMenu(app.GetMenu(), 0);
	EnableMenuItem(hSubMenu, 11, MF_BYPOSITION | MF_ENABLED);
	EnableMenuItem(hSubMenu, 12, MF_BYPOSITION | MF_ENABLED);

	// "remove segs from..."
	hSubMenu = ::GetSubMenu(app.GetMenu(), 1);
	EnableMenuItem(hSubMenu, 13, MF_BYPOSITION | MF_ENABLED);
		
	// clear all
	EnableMenuItem(app.GetMenu(), 9, MF_BYPOSITION | MF_ENABLED);

	CPegDoc::GetDoc()->LayersSoloUnHideAll(strSoloHideLayers);
	strSoloHideLayers.Empty();
}