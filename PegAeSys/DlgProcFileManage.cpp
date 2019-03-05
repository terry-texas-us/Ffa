#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

//#include "Block.h"
#include "DlgProcFileManage.h"
#include "Preview.h"
#include "FileJob.h"

#include "solo.h"

//#include "StateInfoFrm.h"
#include "DlgProcPrintPreview.h"
extern CSoloStateFrm soloStateFrame;
extern CSolo solo;

CString strSoloHideLayers;

// callback procedure for finding HWND from ProcessID
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
void DlgProcFileManageDoLayerColor(HWND hDlg, LPARAM lParam);
void DlgProcFileManageDoLayerStyle(HWND hDlg, LPARAM lParam);
UINT CALLBACK OFNHookProcTracingInclude(HWND, UINT, WPARAM, LPARAM);

WNDPROC wpOrigEditProc; 
bool bFirstTime = true;

HWND hLayerList = (HWND)-1;
HWND hTracingList = (HWND)-1;
HWND hBlockList = (HWND)-1;

BOOL CALLBACK WndProcSubclass(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	WORD wNotifyCode = HIWORD(wParam);

	//////////////////////////////////////////////////////////////
	//HDC				hdc;
	//PAINTSTRUCT		ps;
	//RECT			rect;
	//RECT			rect2;
	//POINT			pt;
	//
	//static BOOL		fMouseDown   = FALSE;
	//static BOOL		fButtonDown  = FALSE;
	///////////////////////////////////////////////////////////////////

	switch (nMsg) 
	{
	//	case WM_PAINT:
	//	
	//	if(wParam == 0)		hdc = BeginPaint(hDlg, &ps);
	//	else				hdc = (HDC)wParam;
	//	//
	//	//	Mask off the borders and draw ComboBox normally
	//	//
	//	GetClientRect(hDlg, &rect);

	//	InflateRect(&rect, -GetSystemMetrics(SM_CXEDGE), -GetSystemMetrics(SM_CYEDGE));

	//	rect.right -= GetSystemMetrics(SM_CXVSCROLL);

	//	IntersectClipRect(hdc, rect.left, rect.top, rect.right, rect.bottom);
	//	
	//	// Draw the ComboBox
	//	CallWindowProc(wpOrigEditProc, hDlg, nMsg, (WPARAM)hdc, lParam);

	//	//
	//	//	Now mask off inside and draw the borders
	//	//
	//	SelectClipRgn(hdc, NULL);
	//	rect.right += GetSystemMetrics(SM_CXVSCROLL);

	//	ExcludeClipRect(hdc, rect.left, rect.top, rect.right, rect.bottom);
	//	
	//	// draw borders
	//	GetClientRect(hDlg, &rect2);
	//	FillRect(hdc, &rect2, GetSysColorBrush(COLOR_3DSHADOW));

	//	// now draw the button
	//	SelectClipRgn(hdc, NULL);
	//	rect.left = rect.right - GetSystemMetrics(SM_CXVSCROLL);
	//	
	//	if(fButtonDown)
	//		DrawFrameControl(hdc, &rect, DFC_SCROLL, DFCS_SCROLLCOMBOBOX|DFCS_FLAT|DFCS_PUSHED);
	//		//FillRect(hdc, &rect, GetSysColorBrush(COLOR_3DDKSHADOW));
	//	else
	//		DrawFrameControl(hdc, &rect, DFC_SCROLL, DFCS_SCROLLCOMBOBOX|DFCS_FLAT);
	//		//FillRect(hdc, &rect, GetSysColorBrush(COLOR_3DFACE));

	//	if(wParam == 0)
	//		EndPaint(hDlg, &ps);

	//	return 0;

	//// check if mouse is within drop-arrow area, toggle
	//// a flag to say if the mouse is up/down. Then invalidate
	//// the window so it redraws to show the changes.
	//case WM_LBUTTONDBLCLK:
	//case WM_LBUTTONDOWN:

	//	pt.x = (short)LOWORD(lParam);
	//	pt.y = (short)HIWORD(lParam);

	//	GetClientRect(hDlg, &rect);

	//	InflateRect(&rect, -GetSystemMetrics(SM_CXEDGE), -GetSystemMetrics(SM_CYEDGE));
	//	rect.left = rect.right - GetSystemMetrics(SM_CXVSCROLL);

	//	if(PtInRect(&rect, pt))
	//	{
	//		// we *should* call SetCapture, but the ComboBox does it for us
	//		// SetCapture
	//		fMouseDown = TRUE;
	//		fButtonDown = TRUE;
	//		InvalidateRect(hDlg, 0, 0);
	//	}

	//	break;

	//// mouse has moved. Check to see if it is in/out of the drop-arrow
	//case WM_MOUSEMOVE:

	//	pt.x = (short)LOWORD(lParam);
	//	pt.y = (short)HIWORD(lParam);

	//	if(fMouseDown && (wParam & MK_LBUTTON))
	//	{
	//		GetClientRect(hDlg, &rect);

	//		InflateRect(&rect, -GetSystemMetrics(SM_CXEDGE), -GetSystemMetrics(SM_CYEDGE));
	//		rect.left = rect.right - GetSystemMetrics(SM_CXVSCROLL);

	//		if(fButtonDown != PtInRect(&rect, pt))
	//		{
	//			fButtonDown = PtInRect(&rect, pt);
	//			InvalidateRect(hDlg, 0, 0);
	//		}
	//	}

	//	break;

	//case WM_LBUTTONUP:

	//	if(fMouseDown)
	//	{
	//		// No need to call ReleaseCapture, the ComboBox does it for us
	//		// ReleaseCapture

	//		fMouseDown = FALSE;
	//		fButtonDown = FALSE;
	//		InvalidateRect(hDlg, 0, 0);
	//	}

	//	break;
	
//		case WM_GETDLGCODE:
////			if(lParam == VK_RETURN)
////				return DLGC_WANTMESSAGE;
////				MessageBox(0, "return", 0, 0);
//			return DLGC_WANTALLKEYS;
//		case WM_KEYDOWN:
//		{
//			if(wParam == VK_RETURN)
//			{
//				MessageBox(0, "return", 0, 0);
//				return FALSE;
//			}
//		}
		//case WM_NOTIFY:
		//{
		//	LPNMLVKEYDOWN pnkd = (LPNMLVKEYDOWN)lParam;

		//	if(pnkd->hdr.code == LVN_KEYDOWN)
		//	{
		//		MessageBox(0, "key down", 0, 0);
		//		return TRUE;
		//	}

		//	break;
		//}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case ID_LAYER_WASH:
				{
					HWND hCombo = ::GetDlgItem(hDlg, ID_LAYER_WASH);

					if((wNotifyCode == CBN_CLOSEUP))
						// || (wNotifyCode == CBN_SELCHANGE))//CBN_CLOSEUP)//CBN_SELENDOK)//CBN_CLOSEUP)
					{
						int nIndex = ::SendDlgItemMessage(hDlg, ID_LAYER_WASH, CB_GETCURSEL, 0, 0);
						if(nIndex < 0) return FALSE;
						::DestroyWindow(hCombo);
						int nStateControl = -1;
						switch(nIndex)
						{
							case 0: nStateControl = IDC_LAYWORK; break;
							case 1: nStateControl = IDC_LAYACTIVE; break;
							case 2: nStateControl = IDC_LAYSTATIC; break;
							case 3: nStateControl = IDC_LAYHIDDEN; break;
						}
						SendMessage(::GetParent(hDlg), WM_COMMAND, MAKEWPARAM(nStateControl, CBN_CLOSEUP), 0);
					}
					else if(wNotifyCode == CBN_SELENDCANCEL)
					{
						::PostMessage(hCombo, WM_CLOSE, 0, 0);
					}
					break;
				}
				case ID_TRACING_MVC:
				{
					HWND hCombo = ::GetDlgItem(hDlg, ID_TRACING_MVC);

					if(wNotifyCode == CBN_CLOSEUP)//CBN_SELCHANGE)//CBN_CLOSEUP)//CBN_SELENDOK)//CBN_CLOSEUP)
					{
						int nIndex = ::SendDlgItemMessage(hDlg, ID_TRACING_MVC, CB_GETCURSEL, 0, 0);
						::DestroyWindow(hCombo);
						int nStateControl = -1;
						switch(nIndex)
						{
							case 0: nStateControl = IDC_TRAMAP; break;
							case 1: nStateControl = IDC_TRAVIEW; break;
							case 2: nStateControl = IDC_TRACLOAK; break;
						}
						SendMessage(::GetParent(hDlg), WM_COMMAND, MAKEWPARAM(nStateControl, CBN_CLOSEUP), 0);
					}
					else if(wNotifyCode == CBN_SELENDCANCEL)
					{
						::PostMessage(hCombo, WM_CLOSE, 0, 0);
					}
					break;
				}
				case IDC_COLOR_ID:
				{
					HWND hCombo = ::GetDlgItem(hDlg, IDC_COLOR_ID);

					if(wNotifyCode == CBN_CLOSEUP)//CBN_SELCHANGE)//CBN_CLOSEUP)//CBN_SELENDOK)//CBN_CLOSEUP)
					{
						int nIndex = ::SendDlgItemMessage(hDlg, IDC_COLOR_ID, CB_GETCURSEL, 0, 0);
						::DestroyWindow(hCombo);
						::SendMessage(::GetParent(hDlg), WM_COMMAND, MAKEWPARAM(IDC_COLOR_ID, CBN_CLOSEUP), nIndex);
					}
					else if(wNotifyCode == CBN_SELENDCANCEL)
					{
						::PostMessage(hCombo, WM_CLOSE, 0, 0);
					}
					break;
				}
				case IDC_PENSTYLE:
				{
					HWND hCombo = ::GetDlgItem(hDlg, IDC_PENSTYLE);

					if(wNotifyCode == CBN_CLOSEUP)//CBN_SELCHANGE)//CBN_CLOSEUP)//CBN_SELENDOK)//CBN_CLOSEUP)
					{
						char strName[256];
						int nIndex = ::SendDlgItemMessage(hDlg, IDC_PENSTYLE, CB_GETCURSEL, 0, 0);
						if(nIndex != CB_ERR)
							::SendMessage(hCombo, CB_GETLBTEXT, (WPARAM) nIndex, (LPARAM) (LPCSTR) strName);
						::DestroyWindow(hCombo);
						::SendMessage(::GetParent(hDlg), WM_COMMAND, MAKEWPARAM(IDC_PENSTYLE, CBN_CLOSEUP), (LPARAM) (LPCSTR) strName);
					}
					else if(wNotifyCode == CBN_SELENDCANCEL)
					{
						::PostMessage(hCombo, WM_CLOSE, 0, 0);
					}
					break;
				}
			}
			break;
		}
	}
	return (::CallWindowProc(wpOrigEditProc, hDlg, nMsg, wParam, lParam));
}

struct FINDWINDOWHANDLESTRUCT
{
	HWND hWndFound;
	LPPROCESS_INFORMATION ProcessInfo;
};

char szLayerName[64];

namespace translate
{
	extern bool bOneLayer;
	extern CString strLayerName;
}

CString GetItemText(HWND hDlg, int iCtrlId)
{
	char szBuf[128] = {0};

	HWND hWndLay = ::GetDlgItem(hDlg, iCtrlId);
	int nIndex = ListView_GetSelectionMark(hWndLay);
	ListView_GetItemText(hWndLay, nIndex, 0, szBuf, 128);

	return szBuf;
}

CLayer* GetItemSelected(HWND hDlg, int iCtrlId, int& nIndex)
{
	char szBuf[128];

	HWND hWndLay = ::GetDlgItem(hDlg, iCtrlId);
	nIndex = ListView_GetSelectionMark(hWndLay);
	ListView_GetItemText(hWndLay, nIndex, 0, szBuf, 128);

	return CPegDoc::GetDoc()->LayersGet(szBuf);
}

#include <deque>
using std::deque;

deque<CString> GetItemsSelected(HWND hDlg, int iCtrlId)
{
//	CPegDoc *pDoc = CPegDoc::GetDoc();

	char szBuf[128];
	deque<CString> layers;

	HWND hWndLay = ::GetDlgItem(hDlg, iCtrlId);

	int nIndex = -1;

	do
	{
		nIndex = ListView_GetNextItem(hWndLay, nIndex, LVNI_SELECTED);
		if(nIndex > -1)
		{
			ListView_GetItemText(hWndLay, nIndex, 0, szBuf, 128);
			CString strName = szBuf;
			layers.push_back(strName);
		}
	}
	while(nIndex != -1);

	return layers;
}

void UpdateLayerItem(HWND hDlg, CLayer *pLayer)
{
	if(pLayer == 0)
		return;

	char szBuf4[256];
	HWND hWndLayer = ::GetDlgItem(hDlg, IDC_LAY2);

	if(pLayer->IsOff())
		strcpy(szBuf4, "Hidden");
	else if(pLayer->IsCold())
		strcpy(szBuf4, "Static");
	else if(pLayer->IsWarm())
		strcpy(szBuf4, "Active");
	else if(pLayer->IsHot())
		strcpy(szBuf4, "Work");

	//if(pLayer->IsHot())
	//	strcpy(szBuf4, "Work");
	//else if(pLayer->IsWarm())
	//	strcpy(szBuf4, "Active");
	//else if(pLayer->IsCold())
	//	strcpy(szBuf4, "Static");
	//else if(pLayer->IsOff())
	//	strcpy(szBuf4, "Hidden");

	LVFINDINFO lvFind;
	::ZeroMemory(&lvFind, sizeof(lvFind));	
	lvFind.flags = LVFI_STRING;
	lvFind.psz = pLayer->GetName();
	int nIndex2 = ListView_FindItem(hWndLayer, -1, &lvFind);

	CString strName = pLayer->GetName();
	ListView_SetItemText(hWndLayer, nIndex2, 0, (LPSTR)strName.GetBuffer());

	ListView_SetItemText(hWndLayer, nIndex2, 1, szBuf4);

	itoa(pLayer->GetCount(), szBuf4, 10);
	ListView_SetItemText(hWndLayer, nIndex2, 2, szBuf4);

	itoa(pLayer->PenColor(), szBuf4, 10);
	ListView_SetItemText(hWndLayer, nIndex2, 3, szBuf4);

	CPenStyle* pPenStyle = CPegDoc::GetDoc()->PenStylesGetAt(pLayer->PenStyle());
	strcpy(szBuf4, pPenStyle->GetName().GetBuffer());
	ListView_SetItemText(hWndLayer, nIndex2, 4, szBuf4);
}

void UpdateAllLayerItems(HWND hDlg)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	for (WORD w = 0; w < pDoc->LayersGetSize(); w++)
	{
		CLayer* pLayer = pDoc->LayersGetAt(w);

		if(pLayer->IsInternal())
			UpdateLayerItem(hDlg, pLayer);
	}
}

void InsertLayerItem(HWND hDlg, CLayer *pLayer)
{
	if(pLayer == 0)
		return;

	char szBuf4[256];
	HWND hWndLayer = ::GetDlgItem(hDlg, IDC_LAY2);

	LVITEM lvi;
	::ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT | LVIF_STATE;
	lvi.iItem = 0; 
	lvi.pszText = strcpy(szBuf4, (LPCSTR)pLayer->GetName());
	ListView_InsertItem(hWndLayer, &lvi);

	UpdateLayerItem(hDlg, pLayer);
}

void UpdateTracingItem(HWND hDlg, CLayer *pLayer)
{
	if(pLayer == 0)
		return;

	char szBuf4[256];
	HWND hWndLayer = ::GetDlgItem(hDlg, IDC_TRA2);

//	ListView_SetTextColor(hWndLayer, RGB(128, 128, 128));

	if(pLayer->IsMapped())
		strcpy(szBuf4, "Mapped");
	else if(pLayer->IsViewed())
		strcpy(szBuf4, "Viewed");
	else if(pLayer->IsCloaked())
		strcpy(szBuf4, "Cloaked");

	LVFINDINFO lvFind;
	::ZeroMemory(&lvFind, sizeof(lvFind));	
	lvFind.flags = LVFI_STRING;
	lvFind.psz = pLayer->GetName();
	int nIndex2 = ListView_FindItem(hWndLayer, -1, &lvFind);

	CString strName = pLayer->GetName();
	ListView_SetItemText(hWndLayer, nIndex2, 0, (LPSTR)strName.GetBuffer());

	ListView_SetItemText(hWndLayer, nIndex2, 1, szBuf4);

	itoa(pLayer->GetCount(), szBuf4, 10);
	ListView_SetItemText(hWndLayer, nIndex2, 2, szBuf4);
}

void UpdateAllTracingItems(HWND hDlg)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	for(int i = 0; i < pDoc->LayersGetSize(); i++)
	{
		CLayer *pLayer = pDoc->LayersGetAt(i);

		if(!pLayer->IsInternal())
			UpdateTracingItem(hDlg, pLayer);
	}
}

void InsertTracingItem(HWND hDlg, CLayer *pLayer)
{
	if(pLayer == 0)
		return;

	char szBuf4[256];
	HWND hWndLayer = ::GetDlgItem(hDlg, IDC_TRA2);

	LVITEM lvi;
	::ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT | LVIF_STATE;
	lvi.iItem = 0; 
	lvi.pszText = strcpy(szBuf4, (LPCSTR)pLayer->GetName());
//	lvi.pszText = strcpy(szBuf4, (LPCSTR)pLayer->);
	ListView_InsertItem(hWndLayer, &lvi);

	UpdateTracingItem(hDlg, pLayer);
}

void UpdateBlockItem(HWND hDlg, CString strKey, CBlock *pBlock)
{
	if(pBlock == 0)
		return;

	CPegDoc *pDoc = CPegDoc::GetDoc();

	char szBuf4[256];
	HWND hWndBlock = ::GetDlgItem(hDlg, IDC_BLOCKS_LIST2);

	LVFINDINFO lvFind;
	::ZeroMemory(&lvFind, sizeof(lvFind));	
	lvFind.flags = LVFI_STRING;
	lvFind.psz = strKey;
	int nIndex2 = ListView_FindItem(hWndBlock, -1, &lvFind);

	CString strName = strKey;
	ListView_SetItemText(hWndBlock, nIndex2, 0, (LPSTR)strName.GetBuffer());

	itoa(pDoc->BlockGetRefCount(strKey), szBuf4, 10);
	ListView_SetItemText(hWndBlock, nIndex2, 1, szBuf4);

	itoa(pBlock->GetCount(), szBuf4, 10);
	ListView_SetItemText(hWndBlock, nIndex2, 2, szBuf4);
}

void InsertBlockItem(HWND hDlg, CString strKey, CBlock *pBlock)
{
	if(pBlock == 0)
		return;

	char szBuf4[256];
	HWND hWndBlock = ::GetDlgItem(hDlg, IDC_BLOCKS_LIST2);

	LVITEM lvi;
	::ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT | LVIF_STATE;
	lvi.iItem = 0; 
	lvi.pszText = strcpy(szBuf4, (LPCSTR)strKey);
	ListView_InsertItem(hWndBlock, &lvi);

	UpdateBlockItem(hDlg, strKey, pBlock);
}

void LayerSelectChange(HWND hDlg, int nIndex)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	char szz[64];

	HWND hWndLay = ::GetDlgItem(hDlg, IDC_LAY2);
	ListView_GetItemText(hWndLay, nIndex, 0, szz, 64);

	CString strName = szz;
	CLayer* pLayer = pDoc->LayersGet(strName);

	if(pLayer == 0) return;

	::SendDlgItemMessage(hDlg, IDC_LAYWORK, BM_SETCHECK, pLayer->IsHot(), 0L);
	::SendDlgItemMessage(hDlg, IDC_LAYACTIVE, BM_SETCHECK, pLayer->IsWarm(), 0L);
	::SendDlgItemMessage(hDlg, IDC_LAYSTATIC, BM_SETCHECK, pLayer->IsCold(), 0L);
	::SendDlgItemMessage(hDlg, IDC_LAYHIDDEN, BM_SETCHECK, pLayer->IsOff(), 0L);

	SetDlgItemInt(hDlg, IDC_SEGS, pLayer->GetCount(), FALSE);
	SetDlgItemInt(hDlg, IDC_COLOR_ID, pLayer->PenColor(), TRUE);
	::SendMessage(::GetDlgItem(hDlg, IDC_PENSTYLE), CB_SETCURSEL, pLayer->PenStyle(), TRUE);
						
	CPrim::LayerPenColor() = pLayer->PenColor();
	CPrim::LayerPenStyle() = pLayer->PenStyle();

	WndProcPreviewUpdate(hDlg, pLayer);
}

void TracingSelectChange(HWND hDlg, int nIndex)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	char szz[64];

	HWND hWndLay = ::GetDlgItem(hDlg, IDC_TRA2);
	ListView_GetItemText(hWndLay, nIndex, 0, szz, 64);

	CString strName = szz;
	CLayer* pLayer = pDoc->LayersGet(strName);

	if(pLayer == 0) return;

	::SendDlgItemMessage(hDlg, IDC_TRAMAP, BM_SETCHECK, pLayer->IsMapped(), 0L);
	::SendDlgItemMessage(hDlg, IDC_TRAVIEW, BM_SETCHECK, pLayer->IsViewed(), 0L);
	::SendDlgItemMessage(hDlg, IDC_TRACLOAK, BM_SETCHECK, pLayer->IsCloaked(), 0L);
	
	SetDlgItemInt(hDlg, IDC_SEGS, pLayer->GetCount(), FALSE);
	WndProcPreviewUpdate(hDlg, pLayer);
}

void BlockSelectChange(HWND hDlg, int nIndex)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	char szz[64];

	HWND hWndBlock = ::GetDlgItem(hDlg, IDC_BLOCKS_LIST2);
	ListView_GetItemText(hWndBlock, nIndex, 0, szz, 64);

	CString strName = szz;

	CBlock* pBlock;
	pDoc->BlksLookup(strName, pBlock);
	if(pBlock == 0) return;

	SetDlgItemInt(hDlg, IDC_SEGS, (UINT) pBlock->GetCount(), FALSE);
	WndProcPreviewUpdate(hDlg, pBlock);
}

HWND hWndEdit;

///<summary>
/// callback for file->manage dialog box
///</summary>
BOOL CALLBACK DlgProcFileManage(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	WORD wNotifyCode = HIWORD(wParam);

	hLayerList = ::GetDlgItem(hDlg, IDC_LAY2);
	hTracingList = ::GetDlgItem(hDlg, IDC_TRA2);
	hBlockList = ::GetDlgItem(hDlg, IDC_BLOCKS_LIST2);

	if(nMsg == WM_INITDIALOG)
	{
		bFirstTime = true;
		DlgProcFileManageInit(hDlg);
		return (TRUE);
	}
	else if(nMsg == WM_NOTIFY)
	{
		LPNMITEMACTIVATE lpnmia = (LPNMITEMACTIVATE)lParam;
		LPNMLVKEYDOWN pnkd = (LPNMLVKEYDOWN)lParam;
		LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)lParam;

		// custom draw - change the text color
		/////////////////////////////////////////////////////////////////////
		if(lplvcd->nmcd.hdr.code == NM_CUSTOMDRAW)
		{

			switch(wParam)
			{
				case IDC_LAY2:
				{
					LRESULT pResult = CDRF_DODEFAULT;

					switch(lplvcd->nmcd.dwDrawStage)
					{
						case CDDS_PREPAINT:
						{
							pResult = CDRF_NOTIFYITEMDRAW;
							break;
						}
						case CDDS_ITEMPREPAINT:
						{
						//	pResult = CDRF_NOTIFYSUBITEMDRAW;
						//	break;
						//}
						//case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
						//{

							// get index
							int nIndex = lplvcd->nmcd.dwItemSpec;
		//					CLayer *pLayer = GetItemSelected(hDlg, IDC_LAY2, nIndex);
							char szBuf[128];
							HWND hWndLay = ::GetDlgItem(hDlg, IDC_LAY2);
				//			nIndex = ListView_GetSelectionMark(hWndLay);
							ListView_GetItemText(hWndLay, nIndex, 0, szBuf, 128);
							CLayer *pLayer = CPegDoc::GetDoc()->LayersGet(szBuf);

							if(pLayer == 0)
								return FALSE;

							COLORREF crText;
							if(pLayer->IsHot())
								crText = RGB(211,11,0);
							else if(pLayer->IsWarm())
								crText = RGB(255,109,0);
							else if(pLayer->IsCold())
								crText = RGB(200,159,33);
							else
								crText = RGB(127,127,127);
							//if ( (lplvcd->nmcd.dwItemSpec % 3) == 0 )
							//	crText = RGB(255,0,0);
							//else if ( (lplvcd->nmcd.dwItemSpec % 3) == 1 )
							//	crText = RGB(0,255,0);
							//else
							//	crText = RGB(128,128,255);

							// Store the color back in the NMLVCUSTOMDRAW struct.
							lplvcd->clrText = crText;

		//					lplvcd->clrText   = RGB(255,0,0);
		//					lplvcd->clrTextBk = RGB(128,128,255);

							// Tell Windows to paint the control itself.
							pResult = CDRF_NEWFONT;
							break;
						}
					}
					SetWindowLong(hDlg, DWL_MSGRESULT, (LONG)pResult);
					return (TRUE);
				}
				case IDC_TRA2:
				{
					LRESULT pResult = CDRF_DODEFAULT;

					switch(lplvcd->nmcd.dwDrawStage)
					{
						case CDDS_PREPAINT:
						{
							pResult = CDRF_NOTIFYITEMDRAW;
							break;
						}
						case CDDS_ITEMPREPAINT:
						{
						//	pResult = CDRF_NOTIFYSUBITEMDRAW;
						//	break;
						//}
						//case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
						//{

							// get index
							int nIndex = lplvcd->nmcd.dwItemSpec;
		//					CLayer *pLayer = GetItemSelected(hDlg, IDC_LAY2, nIndex);
							char szBuf[128];
							HWND hWndLay = ::GetDlgItem(hDlg, IDC_TRA2);
				//			nIndex = ListView_GetSelectionMark(hWndLay);
							ListView_GetItemText(hWndLay, nIndex, 0, szBuf, 128);
							CLayer *pLayer = CPegDoc::GetDoc()->LayersGet(szBuf);

							if(pLayer == 0)
								return FALSE;

							COLORREF crText;
							if(pLayer->IsMapped())
								crText = RGB(0,67,95);
							else if(pLayer->IsViewed())
								crText = RGB(75,187,0);
							else //if(pLayer->IsCloaked())
								crText = RGB(127,127,127);

							// Store the color back in the NMLVCUSTOMDRAW struct.
							lplvcd->clrText = crText;

		//					lplvcd->clrText   = RGB(255,0,0);
		//					lplvcd->clrTextBk = RGB(128,128,255);

							// Tell Windows to paint the control itself.
							pResult = CDRF_NEWFONT;
							break;
						}
					}
					SetWindowLong(hDlg, DWL_MSGRESULT, (LONG)pResult);
					return (TRUE);
				}
			}
		}
		/////////////////////////////////////////////////////////////////////

		// combo boxes
//		if(lpnmia->hdr.code == NM_RCLICK)
		if(lpnmia->hdr.code == NM_CLICK)
		{
			if(lpnmia->iItem == -1)
				return (TRUE);

			// lpnmia switch for combo boxes
			switch(wParam)
			{
				case IDC_LAY2:
				{
					char szBuf[128];
					ListView_GetItemText(::GetDlgItem(hDlg, IDC_LAY2), lpnmia->iItem, 0, szBuf, 128);

					CPoint point;
					::GetCursorPos(&point);

					DWORD m_dwDropDownCtrlStyle = /*WS_BORDER |*/ WS_CHILD | WS_VISIBLE | //ES_AUTOHSCROLL | ES_AUTOVSCROLL | 
								CBS_DROPDOWNLIST | /*CBS_DISABLENOSCROLL |*/ WS_CLIPCHILDREN | WS_VSCROLL;// | WS_CLIPCHILDREN;
					RECT rect;
					ListView_GetSubItemRect(::GetDlgItem(hDlg, IDC_LAY2), lpnmia->iItem, lpnmia->iSubItem, 
						LVIR_BOUNDS, &rect);
					POINT ptLL;
					ptLL.x = rect.left; ptLL.y = rect.top;

					switch(lpnmia->iSubItem)
					{
						case 1:
						{
							/*HWND hwndCombo = CreateWindowEx(
								0,
								"COMBOBOX", "", 
								m_dwDropDownCtrlStyle,
								ptLL.x, ptLL.y,
								rect.right-rect.left, 200,
								::GetDlgItem(hDlg, IDC_LAY2),
								(HMENU)ID_LAYER_WASH, app.GetInstance(), 0);
							if(!hwndCombo) return false;
							
							::SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) "Work");
							::SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) "Active");
							::SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) "Static");
							::SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) "Hidden");

							int nIndex;
							CLayer *pLayer = GetItemSelected(hDlg, IDC_LAY2, nIndex);

							if(pLayer->IsHot())	::SendMessage(hwndCombo, CB_SETCURSEL, 0, 0);
							else if(pLayer->IsWarm()) ::SendMessage(hwndCombo, CB_SETCURSEL, 1, 0);
							else if(pLayer->IsCold())::SendMessage(hwndCombo, CB_SETCURSEL, 2, 0);
							else if(pLayer->IsOff())::SendMessage(hwndCombo, CB_SETCURSEL, 3, 0);

							::SetFocus(hwndCombo);

							LONG lResult = SetWindowLong(::GetDlgItem(hDlg, IDC_LAY2), GWL_WNDPROC, (LONG)WndProcSubclass);
							if(bFirstTime)
							{
								wpOrigEditProc = (WNDPROC)lResult;
								bFirstTime = false;
							}*/

							break;
						}
						case 3:
						{
							HWND hwndCombo = CreateWindowEx(
								0,
								"COMBOBOX", "", 
								m_dwDropDownCtrlStyle,
								ptLL.x, ptLL.y,
								rect.right-rect.left, 200,
								::GetDlgItem(hDlg, IDC_LAY2),
								(HMENU)IDC_COLOR_ID, app.GetInstance(), 0);
							if(!hwndCombo) return false;
							for (int i = 0; i < sizeof(crHotCols) / sizeof(COLORREF); i++)
								::SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) _itoa(i, szBuf, 10));

							int nIndex;
							CLayer *pLayer = GetItemSelected(hDlg, IDC_LAY2, nIndex);
							::SendMessage(hwndCombo, CB_SETCURSEL, pLayer->PenColor(), 0);

							::SetFocus(hwndCombo);

							LONG lResult = SetWindowLong(::GetDlgItem(hDlg, IDC_LAY2), GWL_WNDPROC, (LONG)WndProcSubclass);
							if(bFirstTime)
							{
								wpOrigEditProc = (WNDPROC)lResult;
								bFirstTime = false;
							}

							break;
						}
						case 4:
						{
							HWND hwndCombo = CreateWindowEx(
								0,
								"COMBOBOX", "", 
								m_dwDropDownCtrlStyle,
								ptLL.x, ptLL.y,
								rect.right-rect.left, 200,
								::GetDlgItem(hDlg, IDC_LAY2),
								(HMENU)IDC_PENSTYLE, app.GetInstance(), 0);
							if(!hwndCombo) return false;

							CPegDoc::GetDoc()->PenStylesFillCB(hwndCombo);

							int nIndex;
							CLayer *pLayer = GetItemSelected(hDlg, IDC_LAY2, nIndex);
							::SendMessage(hwndCombo, CB_SETCURSEL, pLayer->PenStyle(), 0);

							::SetFocus(hwndCombo);

							LONG lResult = SetWindowLong(::GetDlgItem(hDlg, IDC_LAY2), GWL_WNDPROC, (LONG)WndProcSubclass);
							if(bFirstTime)
							{
								wpOrigEditProc = (WNDPROC)lResult;
								bFirstTime = false;
							}

							break;
						}
					}
					break;
				}
				//case IDC_TRA2:
				//{
				//	char szBuf[128];
				//	ListView_GetItemText(::GetDlgItem(hDlg, IDC_TRA2), lpnmia->iItem, 0, szBuf, 128);

				//	CPoint point;
				//	::GetCursorPos(&point);

				//	DWORD m_dwDropDownCtrlStyle = /*WS_BORDER |*/ WS_CHILD | WS_VISIBLE | //ES_AUTOHSCROLL | ES_AUTOVSCROLL | 
				//				CBS_DROPDOWNLIST | /*CBS_DISABLENOSCROLL |*/ WS_CLIPCHILDREN | WS_VSCROLL;// | WS_CLIPCHILDREN;
				//	RECT rect;
				//	ListView_GetSubItemRect(::GetDlgItem(hDlg, IDC_TRA2), lpnmia->iItem, lpnmia->iSubItem, 
				//		LVIR_BOUNDS, &rect);
				//	POINT ptLL;
				//	ptLL.x = rect.left; ptLL.y = rect.top;

				//	switch(lpnmia->iSubItem)
				//	{
				//		case 1:
				//		{
				//			HWND hwndCombo = CreateWindowEx(
				//				0,
				//				"COMBOBOX", "", 
				//				m_dwDropDownCtrlStyle,
				//				ptLL.x, ptLL.y,
				//				rect.right-rect.left, 200,
				//				::GetDlgItem(hDlg, IDC_TRA2),
				//				(HMENU)ID_TRACING_MVC, app.GetInstance(), 0);
				//			if(!hwndCombo) return false;
				//			
				//			::SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) "Map");
				//			::SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) "View");
				//			::SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) "Cloak");

				//			int nIndex;
				//			CLayer *pLayer = GetItemSelected(hDlg, IDC_TRA2, nIndex);

				//			if(pLayer->IsMapped()) ::SendMessage(hwndCombo, CB_SETCURSEL, 0, 0);
				//			else if(pLayer->IsViewed()) ::SendMessage(hwndCombo, CB_SETCURSEL, 1, 0);
				//			else if(pLayer->IsCloaked())::SendMessage(hwndCombo, CB_SETCURSEL, 2, 0);

				//			::SetFocus(hwndCombo);

				//			LONG lResult = SetWindowLong(::GetDlgItem(hDlg, IDC_TRA2), GWL_WNDPROC, (LONG)WndProcSubclass);
				//			if(bFirstTime)
				//			{
				//				wpOrigEditProc = (WNDPROC)lResult;
				//				bFirstTime = false;
				//			}

				//			break;
				//		}
				//	}
				//	break;
				//}
			}
		}

		switch(wParam)
		{
			case IDC_LAY2:
			{
				if(lpnmia->hdr.code == LVN_BEGINLABELEDIT)
				{
					char szBuf[128];
					hWndEdit = ListView_GetEditControl(::GetDlgItem(hDlg, IDC_LAY2));
					::GetWindowText(hWndEdit, szBuf, 128);
					CString strBuf = szBuf;
					if(strBuf == "0")
						return (TRUE);
					else
						return (FALSE);
				}
				else if(lpnmia->hdr.code == LVN_ENDLABELEDIT)
				{
					char text[255];// = "";
					GetWindowText(hWndEdit, text, 255); // get the text into a buffer
//					ListView_SetItemText(::GetDlgItem(hDlg, IDC_LAY2), lpnmia->iItem, 0, text);
					return (TRUE);
				}
				else if(lpnmia->hdr.code == NM_CLICK)
				{
					int nIndex = lpnmia->iItem;
					LayerSelectChange(hDlg, nIndex);
				}
				else if(lpnmia->hdr.code == NM_KILLFOCUS)
				{
					::SendDlgItemMessage(hDlg, IDC_LAYWORK, BM_SETCHECK, false, 0L);
					::SendDlgItemMessage(hDlg, IDC_LAYACTIVE, BM_SETCHECK, false, 0L);
					::SendDlgItemMessage(hDlg, IDC_LAYSTATIC, BM_SETCHECK, false, 0L);
					::SendDlgItemMessage(hDlg, IDC_LAYHIDDEN, BM_SETCHECK, false, 0L);
				}
		
				if(pnkd->hdr.code == LVN_KEYDOWN)
				{
					int nIndex;
					switch(pnkd->wVKey)
					{
						case VK_UP:
							nIndex = ListView_GetSelectionMark(::GetDlgItem(hDlg, IDC_LAY2)) - 1;
							if(nIndex < 0) nIndex = 0;
							LayerSelectChange(hDlg, nIndex);
							break;
						case VK_DOWN:
							nIndex = ListView_GetSelectionMark(::GetDlgItem(hDlg, IDC_LAY2)) + 1;
							int nCount = ListView_GetItemCount(::GetDlgItem(hDlg, IDC_LAY2));
							if(nIndex >= nCount) nIndex = nCount - 1;
							LayerSelectChange(hDlg, nIndex);
							break;
					}
				}
				break;
			}
			case IDC_TRA2:
			{
				if(lpnmia->hdr.code == NM_CLICK)
				{
					int nIndex = lpnmia->iItem;
					TracingSelectChange(hDlg, nIndex);
				}
				else if(lpnmia->hdr.code == NM_KILLFOCUS)
				{
					::SendDlgItemMessage(hDlg, IDC_TRAMAP, BM_SETCHECK, false, 0L);
					::SendDlgItemMessage(hDlg, IDC_TRAVIEW, BM_SETCHECK, false, 0L);
					::SendDlgItemMessage(hDlg, IDC_TRACLOAK, BM_SETCHECK, false, 0L);
				}

				if(pnkd->hdr.code == LVN_KEYDOWN)
				{
					int nIndex;
					switch(pnkd->wVKey)
					{
						case VK_UP:
							nIndex = ListView_GetSelectionMark(::GetDlgItem(hDlg, IDC_TRA2)) - 1;
							if(nIndex < 0) nIndex = 0;
							TracingSelectChange(hDlg, nIndex);
							break;
						case VK_DOWN:
							nIndex = ListView_GetSelectionMark(::GetDlgItem(hDlg, IDC_TRA2)) + 1;
							int nCount = ListView_GetItemCount(::GetDlgItem(hDlg, IDC_TRA2));
							if(nIndex >= nCount) nIndex = nCount - 1;
							TracingSelectChange(hDlg, nIndex);
							break;
					}
				}
				break;
			}
			case IDC_BLOCKS_LIST2:
			{
				if(lpnmia->hdr.code == NM_CLICK)
				{
					int nIndex = lpnmia->iItem;
					BlockSelectChange(hDlg, nIndex);
				}

				if(pnkd->hdr.code == LVN_KEYDOWN)
				{
					int nIndex;
					switch(pnkd->wVKey)
					{
						case VK_UP:
							nIndex = ListView_GetSelectionMark(::GetDlgItem(hDlg, IDC_BLOCKS_LIST2)) - 1;
							if(nIndex < 0) nIndex = 0;
							BlockSelectChange(hDlg, nIndex);
							break;
						case VK_DOWN:
							nIndex = ListView_GetSelectionMark(::GetDlgItem(hDlg, IDC_BLOCKS_LIST2)) + 1;
							int nCount = ListView_GetItemCount(::GetDlgItem(hDlg, IDC_BLOCKS_LIST2));
							if(nIndex >= nCount) nIndex = nCount - 1;
							BlockSelectChange(hDlg, nIndex);
							break;
					}
				}
				break;
			}
		}
	}
	//else if(nMsg == WM_KEYDOWN)
	//{
	//	//if(wParam == VK_F3)
	//		MessageBox(0, 0, 0, 0);
	//}
	else if(nMsg == WM_COMMAND)
	{
		CPegDoc* pDoc = CPegDoc::GetDoc();

		switch(LOWORD(wParam))
		{
			//case IDC_BITMAP_LOCKED:
			//{
			//	pDoc->LayersGet("Bitmap")->SetStateCold();
			//	//pDoc->BitmapLayerGet()->SetStateCold();
			//	break;
			//}
			//case IDC_BITMAP_UNLOCKED:
			//{
			//	pDoc->LayersGet("Bitmap")->SetStateWarm();
			//	//pDoc->BitmapLayerGet()->SetStateWarm();
			//	break;
			//}
			case IDC_BMPINCLUDE:
			{
				pDoc->OnImportBmp();
				CLayer *pLayer = pDoc->LayersGet("Bitmap");
				InsertTracingItem(hDlg, pLayer);
				break;
			}
			case IDC_COLOR_ID:
			{
				if(wNotifyCode == CBN_CLOSEUP)
					DlgProcFileManageDoLayerColor(hDlg, lParam);
				break;
			}
			case IDC_PENSTYLE:
			{
				if(wNotifyCode == CBN_CLOSEUP)
					DlgProcFileManageDoLayerStyle(hDlg, lParam);
				break;
			}
			case IDC_LAYACTIVE:
			{
				CString strName = GetItemText(hDlg, IDC_LAY2);
				if(strName.GetLength() > 0)
					DlgProcFileManageDoLayerActive(hDlg);
	//			else
	//				::SendDlgItemMessage(hDlg, IDC_LAYACTIVE, BM_SETCHECK, false, 0L);

				SetFocus(::GetDlgItem(hDlg, IDC_LAY2));
				break;
			}
			case IDC_LAYDELETE:
			{
				DlgProcFileManageDoLayerDelete(hDlg);
				break;
			}	
			case IDC_LAYHIDDEN:
			{
				CString strName = GetItemText(hDlg, IDC_LAY2);
				if(strName.GetLength() > 0)
					DlgProcFileManageDoLayerHidden(hDlg);
	//			else
	//				::SendDlgItemMessage(hDlg, IDC_LAYHIDDEN, BM_SETCHECK, false, 0L);

				SetFocus(::GetDlgItem(hDlg, IDC_LAY2));
				break;
			}
			case IDC_LAYMELT:
			{
				DlgProcFileManageDoLayerMelt(hDlg);
				break;
			}
			case IDC_LAYNEW:
			{
				szLayerName[0] = 0;
				if(pGetLayerName())
				{
					// mak - prevent duplicate names
					int nIndex = pDoc->LayersLookup(szLayerName);
					if(nIndex >= 0)
						msgWarning(IDS_MSG_LAYER_NO_DUPS, hDlg);
					else
					{
						if(strlen(szLayerName) > 0)
						{
							CLayer* pLayer = new CLayer(szLayerName);
							pDoc->LayersAdd(pLayer);
							InsertLayerItem(hDlg, pLayer);
						}
					}
				}
				break;
			}
			case IDC_LAYRENAME:
			{
				DlgProcFileManageDoLayerRename(hDlg);
				break;
			}
			case IDC_LAYSTATIC:
			{
				CString strName = GetItemText(hDlg, IDC_LAY2);
				if(strName.GetLength() > 0)
					DlgProcFileManageDoLayerStatic(hDlg);
		//		else
		//			::SendDlgItemMessage(hDlg, IDC_LAYSTATIC, BM_SETCHECK, false, 0L);

				SetFocus(::GetDlgItem(hDlg, IDC_LAY2));
				break;
			}
			case IDC_LAYWORK:
			{
				CString strName = GetItemText(hDlg, IDC_LAY2);
				if(strName.GetLength() > 0)
					DlgProcFileManageDoLayerWork(hDlg);
		//		else
		//			::SendDlgItemMessage(hDlg, IDC_LAYWORK, BM_SETCHECK, false, 0L);

				SetFocus(::GetDlgItem(hDlg, IDC_LAY2));
				break;
			}
			case IDC_SOLO:
			{
				DlgProcFileManageDoSolo(hDlg);
				break;
			}
			case IDC_LAY_XLATE:
			{
				CString strName = GetItemText(hDlg, IDC_LAY2);
				if(strName.GetLength() > 0)
				{
					translate::bOneLayer = true;
					translate::strLayerName = strName;
					::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_TRANSLATE_PENS), app.GetSafeHwnd(), DlgProcTranslatePens);	
				}
				break;
			}
			case IDC_LAY_ACTIVEALL:
			{
				//pDoc->OnLayersActiveAll();
				DlgProcFileManageDoLayerActive(hDlg);
				//SetFocus(NULL);
				UpdateAllLayerItems(hDlg);
				SetFocus(::GetDlgItem(hDlg, IDC_LAY2));
				break;
			}
			case IDC_LAY_STATICALL:
			{
				DlgProcFileManageDoLayerStatic(hDlg);
				//pDoc->OnLayersStaticAll();
				//SetFocus(NULL);
				UpdateAllLayerItems(hDlg);
				SetFocus(::GetDlgItem(hDlg, IDC_LAY2));
				break;
			}
			case IDC_LAY_HIDDENALL:
			{
				DlgProcFileManageDoLayerHidden(hDlg);
				//pDoc->OnLayersHideAll();
				//SetFocus(NULL);
				UpdateAllLayerItems(hDlg);
				SetFocus(::GetDlgItem(hDlg, IDC_LAY2));
				break;
			}
			case IDC_UNIQUE:
			{
				for(int i = 0; i < pDoc->LayersGetSize(); i++)
				{
					CLayer *pLayer = pDoc->LayersGetAt(i);
					pLayer->SetPenColor((PENCOLOR)(i%255)+1);
				}
				::UpdateAllLayerItems(hDlg);
				break;
			}
			case IDC_UNBLOCK_SELECTED:
			{
				CString strName = GetItemText(hDlg, IDC_BLOCKS_LIST2);
				if(strName.GetLength() > 0)
				{
					pDoc->FileManageUnblockReferences(strName);

					CBlock *pBlock;
					pDoc->BlksLookup(strName, pBlock);
					UpdateBlockItem(hDlg, strName, pBlock);
				}
				break;
			}
			case IDC_UNBLOCK_ALL:
			{
				pDoc->UnblockToCompressed();

				CString strKey;
				CBlock* pBlock;

				// update block list
				POSITION pos = pDoc->BlksGetStartPosition();
				while(pos != NULL)
				{
					pDoc->BlksGetNextAssoc(pos, strKey, pBlock);
					if(!pBlock->IsAnonymous())
						UpdateBlockItem(hDlg, strKey, pBlock);
				}
				break;
			}
			case IDC_PURGE:
			{
				pDoc->BlksRemoveUnused();

				ListView_DeleteAllItems(::GetDlgItem(hDlg, IDC_BLOCKS_LIST2));
				
				CString strKey;
				CBlock* pBlock;

				// fill block list
				POSITION pos = pDoc->BlksGetStartPosition();
				while(pos != NULL)
				{
					pDoc->BlksGetNextAssoc(pos, strKey, pBlock);
					if(!pBlock->IsAnonymous())
						InsertBlockItem(hDlg, strKey, pBlock);
				}

				WndProcPreviewClear(hDlg);
				break;
			}
			case ID_TRA_TOCLOAK:
			case IDC_TRACLOAK:
			{
				CString strName = GetItemText(hDlg, IDC_TRA2);
				if(strName.GetLength() > 0)
					DlgProcFileManageDoTracingCloak(hDlg);
				else
					::SendDlgItemMessage(hDlg, IDC_TRACLOAK, BM_SETCHECK, false, 0L);

				SetFocus(::GetDlgItem(hDlg, IDC_TRA2));
				break;
			}
			case IDC_TRAEXCLUDE:
			{
				DlgProcFileManageDoTracingExclude(hDlg);
				break;
			}
			case IDC_TRAFUSE:
			{
				DlgProcFileManageDoTracingFuse(hDlg);
				break;
			}
			case IDC_TRAINCLUDE:
			{
				DlgProcFileManageDoTracingInclude(hDlg);
				WndProcPreviewClear(hDlg);
				break;
			}	
//			case IDC_TRAMAP_RETURN:
//			{
//				UpdateAllTracingItems(hDlg);
//				break;
//			}
			case ID_TRA_TOMAP:
			case IDC_TRAMAP:
			{
				CString strName = GetItemText(hDlg, IDC_TRA2);
				if(strName.GetLength() > 0)
					DlgProcFileManageDoTracingMap(hDlg);
				else
					::SendDlgItemMessage(hDlg, IDC_TRAMAP, BM_SETCHECK, false, 0L);

				SetFocus(::GetDlgItem(hDlg, IDC_TRA2));
				break;
			}	
			case ID_TRA_TOVIEW:
			case IDC_TRAVIEW:
			{
				CString strName = GetItemText(hDlg, IDC_TRA2);
				if(strName.GetLength() > 0)
					DlgProcFileManageDoTracingView(hDlg);
				else
					::SendDlgItemMessage(hDlg, IDC_TRAVIEW, BM_SETCHECK, false, 0L);

				SetFocus(::GetDlgItem(hDlg, IDC_TRA2));
				break;
			}
			case IDC_TRA_MAPALL:
			{
				DlgProcFileManageDoTracingMap(hDlg);
				//pDoc->OnTracingMapAll();
				//SetFocus(NULL);
				UpdateAllTracingItems(hDlg);
				SetFocus(::GetDlgItem(hDlg, IDC_TRA2));
				break;
			}
			case IDC_TRA_VIEWALL:
			{
				DlgProcFileManageDoTracingView(hDlg);
				//pDoc->OnTracingViewAll();
				//SetFocus(NULL);
				UpdateAllTracingItems(hDlg);
				SetFocus(::GetDlgItem(hDlg, IDC_TRA2));
				break;
			}
			case IDC_TRA_CLOAKALL:
			{
				DlgProcFileManageDoTracingCloak(hDlg);
				//pDoc->OnTracingCloakAll();
				//SetFocus(NULL);
				UpdateAllTracingItems(hDlg);
				SetFocus(::GetDlgItem(hDlg, IDC_TRA2));
				break;
			}
			case IDOK:
			case IDCANCEL:
			{
				InvalidateRect(app.GetSafeHwnd(), NULL, TRUE);
				::EndDialog(hDlg, TRUE);
				return (TRUE);
			}
		}
	}
	return (FALSE); 		
}

///<summary>
/// Layer is made active.
/// This is a warm state meaning the layer is displayed using hot color set,
/// is detectable, and may have its segments modified or deleted.
/// No new segments are added to an active layer. 
/// Zero or more layers may be active.
///</summary>
void DlgProcFileManageDoLayerActive(HWND hDlg)
{	
	CPegDoc* pDoc = CPegDoc::GetDoc();

	//int nIndex;
	//CLayer *pLayer = GetItemSelected(hDlg, IDC_LAY2, nIndex);

	deque<CString> layers = GetItemsSelected(hDlg, IDC_LAY2);

	for(int i = 0; i < (int)layers.size(); i++)
	{
		CLayer *pLayer = pDoc->LayersGet(layers.at(i));

		if(pLayer == 0)
			continue;
			//return;

		if(pLayer->IsHot())
		{
			if(pLayer->GetName() == "0")
			{
				msgWarning(IDS_MSG_LAYER_NO_ACTIVE, pLayer->GetName(), hDlg);
				::SendDlgItemMessage(hDlg, IDC_LAYACTIVE, BM_SETCHECK, 0, 0L);
				::SendDlgItemMessage(hDlg, IDC_LAYWORK, BM_SETCHECK, 1, 0L);
				continue;
				//return;
			}
			else
			{
				CLayer *pLayerZero = pDoc->LayersGet("0");
				pDoc->WorkLayerSet(pLayerZero);
				UpdateLayerItem(hDlg, pLayerZero);
			}
		}

		pLayer->SetStateWarm();
		UpdateLayerItem(hDlg, pLayer);
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_LAYER_SAFE, pLayer);
	}
}

///<summary>
/// Selected layer is deleted. 
/// Hot layer must be warm'ed before it may be deleted.
/// Layer "0" may never be deleted.
///</summary>
void DlgProcFileManageDoLayerDelete(HWND hDlg)
{	
	CPegDoc* pDoc = CPegDoc::GetDoc();

	//int nIndex;
	//CLayer *pLayer = GetItemSelected(hDlg, IDC_LAY2, nIndex);

	deque<CString> layers = GetItemsSelected(hDlg, IDC_LAY2);

	for(int i = 0; i < (int)layers.size(); i++)
	{
		CLayer *pLayer = pDoc->LayersGet(layers.at(i));

		if(pLayer == 0)
			continue;
			//return;
			
		if(pLayer->GetName() == "0")
			msgWarning(IDS_MSG_LAYER_NO_DELETE_0, hDlg);
		else if (pLayer->IsHot())
			msgWarning(IDS_MSG_LAYER_NO_DELETE_WORK, pLayer->GetName(), hDlg);
		else
		{
			LVFINDINFO lvFind;
			::ZeroMemory(&lvFind, sizeof(lvFind));	
			lvFind.flags = LVFI_STRING;
			lvFind.psz = pLayer->GetName();
			int nIndex2 = ListView_FindItem(::GetDlgItem(hDlg, IDC_LAY2), -1, &lvFind);

			//ListView_DeleteItem(::GetDlgItem(hDlg, IDC_LAY2), nIndex);
			ListView_DeleteItem(::GetDlgItem(hDlg, IDC_LAY2), nIndex2);

			pDoc->UpdateAllViews(NULL, CPegDoc::HINT_LAYER_ERASE, pLayer);
			pDoc->LayersRemove(pLayer->GetName());

			WndProcPreviewClear(hDlg);
		}
	}
}

///<summary>
/// Selected layer is hidden. 
///</summary>
void DlgProcFileManageDoLayerHidden(HWND hDlg)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	//int nIndex;
	//CLayer *pLayer = GetItemSelected(hDlg, IDC_LAY2, nIndex);

	deque<CString> layers = GetItemsSelected(hDlg, IDC_LAY2);

	for(int i = 0; i < (int)layers.size(); i++)
	{
		CLayer *pLayer = pDoc->LayersGet(layers.at(i));

		if(pLayer == 0)
			//return;
			continue;

		if(pLayer->IsHot())
		{
			if(pLayer->GetName() == "0")
			{
				msgWarning(IDS_MSG_LAYER_NO_HIDDEN, pLayer->GetName(), hDlg);
				::SendDlgItemMessage(hDlg, IDC_LAYHIDDEN, BM_SETCHECK, 0, 0L);
				::SendDlgItemMessage(hDlg, IDC_LAYWORK, BM_SETCHECK, 1, 0L);
				//return;
				continue;
			}
			else
			{
				CLayer *pLayerZero = pDoc->LayersGet("0");
				pDoc->WorkLayerSet(pLayerZero);
				UpdateLayerItem(hDlg, pLayerZero);
			}
		}

		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_LAYER_ERASE, pLayer);
		pLayer->RemoveFromSegLists();
		pLayer->SetStateOff();
		UpdateLayerItem(hDlg, pLayer);
	//	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_LAYER_SAFE, pLayer);
	}
}

///<summary>
///Selected layer is converted to a tracing.
///</summary>
void DlgProcFileManageDoLayerMelt(HWND hDlg)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	//int nIndex;
	//CLayer *pLayer = GetItemSelected(hDlg, IDC_LAY2, nIndex);

	deque<CString> layers = GetItemsSelected(hDlg, IDC_LAY2);

	for(int i = 0; i < (int)layers.size(); i++)
	{
		CLayer *pLayer = pDoc->LayersGet(layers.at(i));

		if(pLayer == 0)
			continue;
			//return;

		CString strName = pLayer->GetName();
		CString strNameOld = strName;

		if(strName == "0")
			msgWarning(IDS_MSG_LAYER_NO_MELT_0, hDlg);
		else
		{
			if(pLayer->IsHot())
				pDoc->WorkLayerSet(pDoc->LayersGet("0"));

			if(pDoc->LayerMelt(strName))
			{
				LVFINDINFO lvFind;
				::ZeroMemory(&lvFind, sizeof(lvFind));	
				lvFind.flags = LVFI_STRING;
				lvFind.psz = strNameOld;
				int nIndex2 = ListView_FindItem(::GetDlgItem(hDlg, IDC_LAY2), -1, &lvFind);
				ListView_DeleteItem(::GetDlgItem(hDlg, IDC_LAY2), nIndex2);
				InsertTracingItem(hDlg, pLayer);
			}
		}
	}
}

///<summary>
/// 
///</summary>
void DlgProcFileManageDoLayerRename(HWND hDlg)
{
	//HWND hWndLay = ::GetDlgItem(hDlg, IDC_LAY2);
	//
	//SetFocus(hWndLay);
	//int nIndex = ListView_GetSelectionMark(hWndLay);
	//SetFocus(hWndLay);
	//hWndEdit = ListView_EditLabel(hWndLay, nIndex);

	//ShowWindow(hWndEdit, SW_SHOW);

	///////////////////////////////////////////////////////////////////
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CString strName;

	int nIndex;
	CLayer *pLayer = GetItemSelected(hDlg, IDC_LAY2, nIndex);

	if(pLayer == 0)
		return;

	if(pLayer->GetName() == "0")
		msgWarning(IDS_MSG_LAYER_NO_RENAME_0, hDlg);
	else
	{
		strcpy(szLayerName, pLayer->GetName());
		if(pGetLayerName())
		{
			if (strlen(szLayerName) > 0)
			{
				strName = szLayerName;
				pLayer->SetName(strName);
				
				if (pLayer->IsHot())
					pDoc->WorkLayerSet(pLayer);

				ListView_SetItemText(::GetDlgItem(hDlg, IDC_LAY2), nIndex, 0, szLayerName);
			}
		}
	}	

	// select only the layer that was changed
	HWND hWndLay = ::GetDlgItem(hDlg, IDC_LAY2);
	ListView_SetItemState(hWndLay, -1, 0, LVIS_SELECTED);
	ListView_SetItemState(hWndLay, nIndex, LVIS_SELECTED, LVIS_SELECTED);
}

///<summary>
/// 
///</summary>
void DlgProcFileManageDoLayerStatic(HWND hDlg)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	//int nIndex;
	//CLayer *pLayer = GetItemSelected(hDlg, IDC_LAY2, nIndex);

	deque<CString> layers = GetItemsSelected(hDlg, IDC_LAY2);

	for(int i = 0; i < (int)layers.size(); i++)
	{
		CLayer *pLayer = pDoc->LayersGet(layers.at(i));

		if(pLayer == 0)
			//return;
			continue;

		if (pLayer->IsHot())
		{
			if(pLayer->GetName() == "0")
			{
				msgWarning(IDS_MSG_LAYER_NO_STATIC, pLayer->GetName(), hDlg);
				::SendDlgItemMessage(hDlg, IDC_LAYSTATIC, BM_SETCHECK, 0, 0L);
				::SendDlgItemMessage(hDlg, IDC_LAYWORK, BM_SETCHECK, 1, 0L);
				//return;
				continue;
			}
			else
			{
				CLayer *pLayerZero = pDoc->LayersGet("0");
				pDoc->WorkLayerSet(pLayerZero);
				UpdateLayerItem(hDlg, pLayerZero);
			}
		}

		pLayer->RemoveFromSegLists();
		pLayer->SetStateCold();
		UpdateLayerItem(hDlg, pLayer);
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_LAYER_SAFE, pLayer);
	}
}

///<summary>
/// Selected layer is made the working layer. 
/// This is a hot state meaning the layer is displayed using hot color set,
/// is detectable, and may have its segments modified or deleted.
/// New segments may be added to the layer.
/// Only one layer is in this state at a time.
///</summary>
void DlgProcFileManageDoLayerWork(HWND hDlg)
{	
	CPegDoc* pDoc = CPegDoc::GetDoc();

	int nIndex;
	CLayer *pLayer = GetItemSelected(hDlg, IDC_LAY2, nIndex);

	if(pLayer == 0)
		return;

	CLayer *pLayerOldWork = pDoc->WorkLayerGet();
	if(pLayerOldWork == 0)
		return;
		
	pDoc->WorkLayerSet(pLayer);
	UpdateLayerItem(hDlg, pLayerOldWork);
	UpdateLayerItem(hDlg, pLayer);
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_LAYER_SAFE, pLayer);

	// select only the layer that was changed
	HWND hWndLay = ::GetDlgItem(hDlg, IDC_LAY2);
	ListView_SetItemState(hWndLay, -1, 0, LVIS_SELECTED);
	ListView_SetItemState(hWndLay, nIndex, LVIS_SELECTED, LVIS_SELECTED);
}

///<summary>
/// procedure that launches correct solo path
///</summary>
void DlgProcFileManageDoSolo(HWND hDlg)
{
	char szBuf[128];

	int nTraIndex = ListView_GetSelectionMark(::GetDlgItem(hDlg, IDC_TRA2));
	int nLayerIndex = ListView_GetSelectionMark(::GetDlgItem(hDlg, IDC_LAY2));
	int nBlockIndex = ListView_GetSelectionMark(::GetDlgItem(hDlg, IDC_BLOCKS_LIST2));

	// solo tracing
	if(nTraIndex != -1)
	{
		ListView_GetItemText(::GetDlgItem(hDlg, IDC_TRA2), nTraIndex, 0, szBuf, 128);
		DlgProcFileManageDoSoloTracing(szBuf);
		//DlgProcFileManageDoSoloTracingSpawn(szBuf);
		::EndDialog(hDlg, TRUE);
	}
	// solo layer
	else if(nLayerIndex != -1)
	{
		ListView_GetItemText(::GetDlgItem(hDlg, IDC_LAY2), nLayerIndex, 0, szBuf, 128);
		DlgProcFileManageDoSoloLayer(szBuf);
		//DlgProcFileManageDoSoloLayerSpawn(szBuf);
		::EndDialog(hDlg, TRUE);
	}
	// solo block
	else if(nBlockIndex != -1)
	{
		ListView_GetItemText(::GetDlgItem(hDlg, IDC_BLOCKS_LIST2), nBlockIndex, 0, szBuf, 128);
		DlgProcFileManageDoSoloBlock(szBuf);
		//DlgProcFileManageDoSoloBlockSpawn(szBuf);
		::EndDialog(hDlg, TRUE);
	}
	else
		MessageBox(hDlg, "Please select a layer, tracing or block to solo and try again.", "Solo Error", 0);
}

void DlgProcFileManageDoSoloGrayOptions()
{
	EnableMenuItem(app.GetMenu(), IDM_FILE_NEW, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_OPEN, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_SAVE, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_SAVEAS, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_CLOSE, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_MANAGE, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), ID_FILE_PLOT, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), ID_FILE_MRU1, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), ID_FILE_MRU2, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), ID_FILE_MRU3, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), ID_FILE_MRU4, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), ID_FILE_MRU5, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), ID_FILE_RECOVER, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), ID_VIEW_PRINTPREVIEW, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), ID_EDIT_SEGTOWORK, MF_GRAYED);

	// import and export
	HMENU hSubMenu = ::GetSubMenu(app.GetMenu(), 0);
	EnableMenuItem(hSubMenu, 11, MF_BYPOSITION | MF_GRAYED);
	EnableMenuItem(hSubMenu, 12, MF_BYPOSITION | MF_GRAYED);

	// "remove segs from..."
	hSubMenu = ::GetSubMenu(app.GetMenu(), 1);
	EnableMenuItem(hSubMenu, 13, MF_BYPOSITION | MF_GRAYED);
		
	// clear all
	EnableMenuItem(app.GetMenu(), 9, MF_BYPOSITION | MF_GRAYED);
}

///<summary>
/// Creates the new instance of Pegaesys for viewing the solo-ed item
///</summary>
void DlgProcFileManageDoSoloCreateProcess(HWND &rHwnd)
{
	SECURITY_ATTRIBUTES shouldInherit;
	shouldInherit.nLength = sizeof(SECURITY_ATTRIBUTES);
	shouldInherit.lpSecurityDescriptor = NULL;
	shouldInherit.bInheritHandle = TRUE;

	// prevent multiple solos
//

	// create new process
	STARTUPINFO info;
	GetStartupInfo(&info);
	PROCESS_INFORMATION pi;

	// get pathname for executable
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(app.GetInstance(), szPath, MAX_PATH);
	CString strExePath = szPath;

	// create process
	CreateProcess(strExePath,0,&shouldInherit,&shouldInherit,true,0,0,0,&info,&pi);

	FINDWINDOWHANDLESTRUCT fwhs;
	fwhs.hWndFound = NULL;
	fwhs.ProcessInfo = &pi;

	WaitForInputIdle(pi.hProcess, INFINITE);

	// code to find the hwnd of the new instance of pegaesys
	if(pi.dwProcessId != NULL)
		EnumWindows(EnumWindowsProc, (LPARAM)&fwhs);

	rHwnd = fwhs.hWndFound;

	CloseHandle(&pi);	
}

///<summary>
/// Creates the solo window when a tracing item is selected
///</summary>
void DlgProcFileManageDoSoloTracing(CString strName)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	CPegView *pView = CPegView::GetActiveView();

	pDoc->OnEditTrapQuit();

	app.SetPegState(STATE_SOLO);
	app.SetSoloType(SOLO_TRACING);

	CLayer *pLayer = pDoc->LayersGet(strName);

	if(pLayer == 0)
		return;

	//Hide all layers
	CPegDoc::GetDoc()->LayersSoloHideAll(strSoloHideLayers);

	CString strLayerName = "solo:" + pLayer->GetName();
	CLayer *pNewLayer = new CLayer(strLayerName);
	pDoc->LayersAdd(pNewLayer);
	CLayer *pOldWorkLayer = pDoc->WorkLayerGet();
	pDoc->WorkLayerSet(pNewLayer);
	pOldWorkLayer->SetStateOff();

	if(pNewLayer == 0)
		return;

	// copy ish
	POSITION pos = pLayer->GetHeadPosition();
	while(pos != 0)
	{
		CSeg *pSeg = pLayer->GetNext(pos);
		CSeg* pSegNew = new CSeg(*pSeg);

		if(pSeg != 0)
			pNewLayer->AddTail(pSegNew);
	}

	pView->OnWindowBest();
	pView->OnWindowZoomOut(pView->GetSafeHwnd());

	// disable features for solo window
	DlgProcFileManageDoSoloGrayOptions();

	::ShowWindow(soloStateFrame.GetHWND(), SW_SHOW);
}

void DlgProcFileManageDoSoloTracingSpawn(CString strName)
{
	CString s1 = "SOFTWARE\\Fanning\\Solo";
	HKEY fKey;
	DWORD dwDisp = 0;
	LPDWORD lpdwDisp = &dwDisp;

	DlgProcFileManageDoSoloLoadHwnd();

	// create registry key holding pathname and tracing name
	if(RegCreateKeyEx(HKEY_CURRENT_USER, s1, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &fKey, lpdwDisp) == ERROR_SUCCESS)
	{
		RegSetValueEx(fKey, "tracingname", 0, REG_SZ, (LPBYTE)strName.GetBuffer(strName.GetLength()), strName.GetLength() + 1);
		RegCloseKey(fKey);
	}

	HWND childHwnd;
	DlgProcFileManageDoSoloCreateProcess(childHwnd);

	if(childHwnd != NULL)
		SendNotifyMessage(childHwnd, WM_SOLO_TRACING, 0, 0);
}

///<summary>
/// Creates the solo window when a layer item is selected
///</summary>
void DlgProcFileManageDoSoloLayer(CString strName)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	CPegView *pView = CPegView::GetActiveView();

	pDoc->OnEditTrapQuit();

	app.SetPegState(STATE_SOLO);
	app.SetSoloType(SOLO_LAYER);

	CLayer *pLayer = pDoc->LayersGet(strName);
	if(pLayer == 0)
		return;

	//Hide all layers
	CPegDoc::GetDoc()->LayersSoloHideAll(strSoloHideLayers);

	CString strLayerName = "solo:" + pLayer->GetName();
	CLayer *pNewLayer = new CLayer(strLayerName);
	pDoc->LayersAdd(pNewLayer);
	CLayer *pOldWorkLayer = pDoc->WorkLayerGet();
	pDoc->WorkLayerSet(pNewLayer);
	pOldWorkLayer->SetStateOff();

	if(pNewLayer == 0)
		return;

	// copy ish
	POSITION pos = pLayer->GetHeadPosition();
	while(pos != 0)
	{
		CSeg *pSeg = pLayer->GetNext(pos);
		CSeg *pSegNew = new CSeg(*pSeg);

		if(pSeg != 0)
			pNewLayer->AddTail(pSegNew);
	}

	pView->OnWindowBest();
	pView->OnWindowZoomOut(pView->GetSafeHwnd());

	// disable features for solo window
	DlgProcFileManageDoSoloGrayOptions();

	::ShowWindow(soloStateFrame.GetHWND(), SW_SHOW);
}

//void DlgProcFileManageDoSoloLayer(CString strName)
//{
//	CPegDoc* pDoc = CPegDoc::GetDoc();

//	// get temp path name for file
//	TCHAR szPath[MAX_PATH];
//	GetTempPath(MAX_PATH, szPath);
//	CString strPathName = szPath;
//	strPathName += "__MaKtEmP - layer.jb1";
//
	// open data and store to file
//	CLayer *pLayer = pDoc->LayersGet(strName);
//	CFileJob fj;
//	if(fj.OpenForWrite(strPathName))
//	{
//		fj.WriteHeader();
//		fj.WriteSegs(pLayer);
//		fj.Close();
//	}

	// set block name
//	pDoc->SoloFileSetLayerName(strName);
//	solo.SoloFileSetLayerName(strName);

	//SECURITY_ATTRIBUTES shouldInherit;
	//shouldInherit.nLength = sizeof(SECURITY_ATTRIBUTES);
	//shouldInherit.lpSecurityDescriptor = NULL;
	//shouldInherit.bInheritHandle = TRUE;

	//// put info in registry for child process
	//CString s1 = "SOFTWARE\\Fanning\\Solo";
	//HKEY fKey;
	//DWORD dwDisp = 0;
	//LPDWORD lpdwDisp = &dwDisp;

	//DlgProcFileManageDoSoloLoadHwnd();

	//if(RegCreateKeyEx(HKEY_CURRENT_USER, s1, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, &shouldInherit, &fKey, lpdwDisp) == ERROR_SUCCESS)
	//{
	//	RegSetValueEx(fKey, "tempname", 0, REG_SZ, (LPBYTE)strName.GetBuffer(strName.GetLength()), strName.GetLength() + 1);
	//	RegSetValueEx(fKey, "temppath", 0, REG_SZ, (LPBYTE)strPathName.GetBuffer(strPathName.GetLength()), strPathName.GetLength() + 1);
	//	RegCloseKey(fKey);
	//}

	//HWND childHwnd;
	//DlgProcFileManageDoSoloCreateProcess(childHwnd);

	//if(childHwnd != NULL)
	//	SendNotifyMessage(childHwnd, WM_SOLO_LAYER, 0, 0);
//}

void DlgProcFileManageDoSoloLayerSpawn(CString strName)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	// get temp path name for file
	TCHAR szPath[MAX_PATH];
	GetTempPath(MAX_PATH, szPath);
	CString strPathName = szPath;
	strPathName += "__MaKtEmP - layer.jb1";

	// open data and store to file
	CLayer *pLayer = pDoc->LayersGet(strName);
	CFileJob fj;
	if(fj.OpenForWrite(strPathName))
	{
		fj.WriteHeader();
		fj.WriteSegs(pLayer);
		fj.Close();
	}

	// set block name
	solo.SoloFileSetLayerName(strName);

	SECURITY_ATTRIBUTES shouldInherit;
	shouldInherit.nLength = sizeof(SECURITY_ATTRIBUTES);
	shouldInherit.lpSecurityDescriptor = NULL;
	shouldInherit.bInheritHandle = TRUE;

	// put info in registry for child process
	CString s1 = "SOFTWARE\\Fanning\\Solo";
	HKEY fKey;
	DWORD dwDisp = 0;
	LPDWORD lpdwDisp = &dwDisp;

	DlgProcFileManageDoSoloLoadHwnd();

	if(RegCreateKeyEx(HKEY_CURRENT_USER, s1, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, &shouldInherit, &fKey, lpdwDisp) == ERROR_SUCCESS)
	{
		RegSetValueEx(fKey, "tempname", 0, REG_SZ, (LPBYTE)strName.GetBuffer(strName.GetLength()), strName.GetLength() + 1);
		RegSetValueEx(fKey, "temppath", 0, REG_SZ, (LPBYTE)strPathName.GetBuffer(strPathName.GetLength()), strPathName.GetLength() + 1);
		RegCloseKey(fKey);
	}

	HWND childHwnd;
	DlgProcFileManageDoSoloCreateProcess(childHwnd);

	if(childHwnd != NULL)
		SendNotifyMessage(childHwnd, WM_SOLO_LAYER, 0, 0);
}

/////<summary>
///// Creates the solo window when a block is selected
/////</summary>
void DlgProcFileManageDoSoloBlock(CString strName)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	CPegView *pView = CPegView::GetActiveView();

	pDoc->OnEditTrapQuit();

	app.SetPegState(STATE_SOLO);
	app.SetSoloType(SOLO_BLOCK);

	// add blocks to file;
	CBlock *pBlock;
	pDoc->BlksLookup(strName, pBlock);

	CString strLayerName = "solo:" + strName;
	CLayer *pLayer = new CLayer(strLayerName);

//	POSITION pos2 = player->GetHeadPosition();
	POSITION pos = pBlock->GetHeadPosition();
	while(pos != 0)
	{
		CPrim *pprim = pBlock->GetNext(pos);
		CSeg *pseg = new CSeg(pprim);
		CSeg *pNewSeg = new CSeg(*pseg);
//		player->InsertAfter(pos2, pseg);
		pLayer->AddTail(pNewSeg);
	}

	//Hide all layers
	CPegDoc::GetDoc()->LayersSoloHideAll(strSoloHideLayers);

	pDoc->LayersAdd(pLayer);
	CLayer *pOldWorkLayer = pDoc->WorkLayerGet();
	pDoc->WorkLayerSet(pLayer);
	pOldWorkLayer->SetStateOff();

	pView->OnWindowBest();
	pView->OnWindowZoomOut(pView->GetSafeHwnd());

	// disable features for solo window
	DlgProcFileManageDoSoloGrayOptions();

	::ShowWindow(soloStateFrame.GetHWND(), SW_SHOW);
}

void DlgProcFileManageDoSoloBlockSpawn(CString strName)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	// get temp path name for file
	TCHAR szPath[MAX_PATH];
	GetTempPath(MAX_PATH, szPath);
	CString strPathName = szPath;
	strPathName += "__MaKtEmP.peg";

	// open data and store to file
//	CFileException e;
	CFilePeg* pFilePegDst = new CFilePeg;
//	pFilePegDst->Open(strPathName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite, &e);
	pFilePegDst->Open(strPathName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite, false);
	CFilePeg* pFile;

//	if(e.m_cause == CFileException::sharingViolation)
//	{	
//		msgWarning(IDS_MSG_FILE_LOCKED, strPathName);
//		delete pFilePegDst;
//	}
//	else
//	{
		pFile = pDoc->GetFilePeg();

		pFilePegDst->Unload();
		pFilePegDst->Close();
//	}

	// set block name
//	pDoc->SoloFileSetBlockName(strName);
	solo.SoloFileSetBlockName(strName);

	SECURITY_ATTRIBUTES shouldInherit;
	shouldInherit.nLength = sizeof(SECURITY_ATTRIBUTES);
	shouldInherit.lpSecurityDescriptor = NULL;
	shouldInherit.bInheritHandle = TRUE;

	// put info in registry for child process
	CString s1 = "SOFTWARE\\Fanning\\Solo";
	HKEY fKey;
	DWORD dwDisp = 0;
	LPDWORD lpdwDisp = &dwDisp;

	DlgProcFileManageDoSoloLoadHwnd();

	if(RegCreateKeyEx(HKEY_CURRENT_USER, s1, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, &shouldInherit, &fKey, lpdwDisp) == ERROR_SUCCESS)
	{
		RegSetValueEx(fKey, "tempname", 0, REG_SZ, (LPBYTE)strName.GetBuffer(strName.GetLength()), strName.GetLength() + 1);
		RegSetValueEx(fKey, "temppath", 0, REG_SZ, (LPBYTE)strPathName.GetBuffer(strPathName.GetLength()), strPathName.GetLength() + 1);
		RegCloseKey(fKey);
	}

	HWND childHwnd;
	DlgProcFileManageDoSoloCreateProcess(childHwnd);

	if(childHwnd != NULL)
		SendNotifyMessage(childHwnd, WM_SOLO_BLOCK, 0, 0);
}

///<summary>
/// Loads the HWND for the current instance into the registry
///</summary>
void DlgProcFileManageDoSoloLoadHwnd()
{
	SECURITY_ATTRIBUTES shouldInherit;
	shouldInherit.nLength = sizeof(SECURITY_ATTRIBUTES);
	shouldInherit.lpSecurityDescriptor = NULL;
	shouldInherit.bInheritHandle = TRUE;

	CString s1 = "SOFTWARE\\Fanning\\Solo";
	HKEY fKey;
	DWORD dwDisp = 0;
	LPDWORD lpdwDisp = &dwDisp;

	// get HWND to send to child process
	DWORD handle = (DWORD)app.GetSafeHwnd();
	char HwndTemp[25];
	itoa(handle, HwndTemp, 10);
	CString appHwnd = HwndTemp;

	if(RegCreateKeyEx(HKEY_CURRENT_USER, s1, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, &shouldInherit, &fKey, lpdwDisp) == ERROR_SUCCESS)
	{
		RegSetValueEx(fKey, "hwnd", 0, REG_SZ, (LPBYTE)appHwnd.GetBuffer(appHwnd.GetLength()),appHwnd.GetLength() + 1);
		RegCloseKey(fKey);
	}
}

///<summary>
/// callback loops through all current processes until it finds
/// the correct process
///</summary>
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	DWORD ProcessId;
	FINDWINDOWHANDLESTRUCT *pfwhs = (FINDWINDOWHANDLESTRUCT *)lParam; 
	CString Title;
	GetWindowThreadProcessId(hwnd, &ProcessId); 

	CWnd::FromHandle(hwnd)->GetWindowText(Title);
	if(ProcessId == pfwhs->ProcessInfo->dwProcessId && Title.GetLength() != 0) 
	{ 
		pfwhs->hWndFound = hwnd; 
		return false; 
	} 
	else 
		return true; 
}

///<summary>
/// Hides the tracing layer from being displayed
///</summary>
void DlgProcFileManageDoTracingCloak(HWND hDlg)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	//int nIndex;
	//CLayer *pLayer = GetItemSelected(hDlg, IDC_TRA2, nIndex);

	deque<CString> layers = GetItemsSelected(hDlg, IDC_TRA2);

	for(int i = 0; i < (int)layers.size(); i++)
	{
		CLayer *pLayer = pDoc->LayersGet(layers.at(i));

		if(pLayer == 0)
			continue;
			//return;

		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_LAYER_ERASE, pLayer);
		pLayer->SetStateOff();
		pLayer->SetTracingFlg(CLayer::TRACING_FLG_CLOAKED);
		UpdateTracingItem(hDlg, pLayer);
	}
}

///<summary>
/// excludes/removes tracing item from file
///</summary>
void DlgProcFileManageDoTracingExclude(HWND hDlg)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	//int nIndex;
	//CLayer *pLayer = GetItemSelected(hDlg, IDC_TRA2, nIndex);

	deque<CString> layers = GetItemsSelected(hDlg, IDC_TRA2);

	for(int i = 0; i < (int)layers.size(); i++)
	{
		CLayer *pLayer = pDoc->LayersGet(layers.at(i));

		if(pLayer == 0)
			continue;
			//return;

		if(pLayer->GetName() == "Bitmap")
		{
			LVFINDINFO lvFind;
			::ZeroMemory(&lvFind, sizeof(lvFind));	
			lvFind.flags = LVFI_STRING;
			lvFind.psz = pLayer->GetName();
			int nIndex2 = ListView_FindItem(::GetDlgItem(hDlg, IDC_TRA2), -1, &lvFind);
			ListView_DeleteItem(::GetDlgItem(hDlg, IDC_TRA2), nIndex2);

			pDoc->UpdateAllViews(NULL, CPegDoc::HINT_LAYER_ERASE, pLayer);
			pDoc->LayersRemove(pLayer->GetName());
		//	pLayer->SetStateOff();
			//ListView_DeleteItem(::GetDlgItem(hDlg, IDC_TRA2), nIndex);
			WndProcPreviewClear(hDlg);
		}
		else
		{
			LVFINDINFO lvFind;
			::ZeroMemory(&lvFind, sizeof(lvFind));	
			lvFind.flags = LVFI_STRING;
			lvFind.psz = pLayer->GetName();
			int nIndex2 = ListView_FindItem(::GetDlgItem(hDlg, IDC_TRA2), -1, &lvFind);
			ListView_DeleteItem(::GetDlgItem(hDlg, IDC_TRA2), nIndex2);

			pDoc->UpdateAllViews(NULL, CPegDoc::HINT_LAYER_ERASE, pLayer);
			pDoc->LayersRemove(pLayer->GetName());
			//ListView_DeleteItem(::GetDlgItem(hDlg, IDC_TRA2), nIndex);
			WndProcPreviewClear(hDlg);
		}
	}
}

///<summary>
/// fuses a tracing item into an internal layer item
///</summary>
void DlgProcFileManageDoTracingFuse(HWND hDlg)
{	
	CPegDoc* pDoc = CPegDoc::GetDoc();

	//int nIndex;
	//CLayer *pLayer = GetItemSelected(hDlg, IDC_TRA2, nIndex);

	deque<CString> layers = GetItemsSelected(hDlg, IDC_TRA2);

	for(int i = 0; i < (int)layers.size(); i++)
	{
		CLayer *pLayer = pDoc->LayersGet(layers.at(i));

		if(pLayer == 0)
			continue;
			//return;

		CString strName = pLayer->GetName();
		CString strNameOld = strName;

		// mak - prevent dups
		CString strNameComp = strName.Left(strName.GetLength() - 4);
		CLayer *pLayerComp = pDoc->LayersGet(strNameComp);

		if(pLayerComp != 0)
			msgWarning(IDS_MSG_LAYER_NO_DUPS, hDlg);
		else
		{
			pDoc->TracingFuse(strName);
			
			// unselect buttons
			::SendDlgItemMessage(hDlg, IDC_TRAMAP, BM_SETCHECK, 0, 0);
			::SendDlgItemMessage(hDlg, IDC_TRAVIEW, BM_SETCHECK, 0, 0);
			::SendDlgItemMessage(hDlg, IDC_TRACLOAK, BM_SETCHECK, 0, 0);

			// update text
			LVFINDINFO lvFind;
				::ZeroMemory(&lvFind, sizeof(lvFind));	
				lvFind.flags = LVFI_STRING;
				lvFind.psz = strNameOld;
				int nIndex2 = ListView_FindItem(::GetDlgItem(hDlg, IDC_TRA2), -1, &lvFind);
			ListView_DeleteItem(::GetDlgItem(hDlg, IDC_TRA2), nIndex2);
			InsertLayerItem(hDlg, pLayer);
		}
	}
}

///<summary>
/// includes a tracing file as a reference in the file
///</summary>
void DlgProcFileManageDoTracingInclude(HWND hDlg)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	static DWORD nFilterIndex = 1;

	// Set current directory to main directory
	CString strPathName = pDoc->GetPathName();
	CString strWDirectory = strPathName.Left(strPathName.ReverseFind('\\'));

	//char szFilter[55];
	//SecureZeroMemory(szFilter, 55);
	//::LoadString(app.GetInstance(), IDS_OPENFILE_FILTER_TRACINGS, szFilter, sizeof(szFilter));

	OPENFILENAME of;
	::ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lpstrInitialDir = strWDirectory;
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = 0;
//	of.hwndOwner = hDlg;
	of.hInstance = app.GetInstance();
	//of.lpstrFilter = szFilter;
	of.lpstrFilter = "Job Files (.jb1)\0*.jb1\0Tracing Files (.tra)\0*.tra\0";
	of.nFilterIndex = nFilterIndex;
	of.lpstrFile = new char[MAX_PATH]; 
	of.lpstrFile[0] = 0;
	of.nMaxFile = MAX_PATH;
	of.lpstrTitle = "Include Tracing";
	of.Flags = OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	of.lpstrDefExt = "tra";
	of.lpfnHook = OFNHookProcTracingInclude;
	of.lpTemplateName = MAKEINTRESOURCE(IDD_TRACING_INCLUDE);
//	of.lpTemplateName = MAKEINTRESOURCE(IDD_TRACING_EX);
	
	if(GetOpenFileName(&of))
	{
		nFilterIndex = of.nFilterIndex;
		
		CString strName = of.lpstrFile;
		CString strPath = strName.Left(of.nFileOffset);

		strName = strName.Mid(of.nFileOffset);

		if(pDoc->LayersGet(strName) == 0)
		{
			if(pDoc->TracingMap(strName))
			{
				CLayer* pLayer = pDoc->LayersGet(strName);
				pLayer->SetStateFlg(CLayer::STATE_FLG_RESIDENT);
				
				CString strOpenName = pDoc->GetPathName();
				
				//// MAK - quick fix... taken out so that a tracing isnt fused when first opened
				//// this routine will fuse the tracing layer into the drawing if it is not resident
				//// in the same directory as the peg file.  It has been commented out because it 
				//// was interfering with solo and file recovery loading.
				//if (strOpenName.Find(strPath) == -1)
				//{
				//	pDoc->TracingFuse(strName);
				//	
				//	strName = of.lpstrFile;
				//	strName = strName.Mid(of.nFileOffset, of.nFileExtension - of.nFileOffset - 1);
				//	pLayer->SetName(strName);
				//}

				InsertTracingItem(hDlg, pLayer);
			}
		}
	}
	delete [] of.lpstrFile;

	UpdateAllTracingItems(hDlg);

	WndProcPreviewOnDestroy(hDlg);
	WndProcPreviewOnCreate(hDlg);
}

///<summary>
/// Selected tracing is mapped.
/// This is a cold state meaning the tracing is displayed using warm color set,
/// is not detectable, and may not have its segments modified or deleted.
/// No new segments may be added to the tracing. 
/// Any number of tracings may be active.
///</summary>
void DlgProcFileManageDoTracingMap(HWND hDlg)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	//int nIndex;
	//CLayer *pLayer = GetItemSelected(hDlg, IDC_TRA2, nIndex);

	deque<CString> layers = GetItemsSelected(hDlg, IDC_TRA2);

	for(int i = 0; i < (int)layers.size(); i++)
	{
		CLayer *pLayer = pDoc->LayersGet(layers.at(i));

		if(pLayer == 0)
			continue;
			//return;

		pDoc->TracingMap(pLayer->GetName());
		UpdateTracingItem(hDlg, pLayer);
	}
}

///<summary>
/// 
///</summary>
void DlgProcFileManageDoTracingView(HWND hDlg)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	//int nIndex;
	//CLayer *pLayer = GetItemSelected(hDlg, IDC_TRA2, nIndex);

	deque<CString> layers = GetItemsSelected(hDlg, IDC_TRA2);

	for(int i = 0; i < (int)layers.size(); i++)
	{
		CLayer *pLayer = pDoc->LayersGet(layers.at(i));

		if(pLayer == 0)
			continue;
			//return;

		pDoc->TracingView(pLayer->GetName());
		UpdateTracingItem(hDlg, pLayer);
	}
}

///<summary>
/// initializes the file->manage dialog box
///</summary>
void DlgProcFileManageInit(HWND hDlg)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	// bitmap layer toggle
	//if(pDoc->BitmapLayerGet()->IsCold())
	//if(pDoc->LayersGet("Bitmap")->IsCold())
	//	::CheckRadioButton(hDlg, IDC_BITMAP_LOCKED, IDC_BITMAP_UNLOCKED, IDC_BITMAP_LOCKED);
	//else
	//	::CheckRadioButton(hDlg, IDC_BITMAP_LOCKED, IDC_BITMAP_UNLOCKED, IDC_BITMAP_UNLOCKED);

	char szBuf[MAX_PATH]; 
	::GetWindowText(hDlg, szBuf, MAX_PATH);
	
	CString strName = szBuf;
	strName += " - ";
	strName += pDoc->GetPathName();
	::SetWindowText(hDlg, strName);

	// setup columns
	////////////////////////////////////////////////////////////////////////////////
	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = 120;//116;

	lvc.iSubItem = 0; lvc.pszText = "Name";
	ListView_InsertColumn(hLayerList, 0, &lvc);
	ListView_InsertColumn(hTracingList, 0, &lvc);
	lvc.cx = 110;
	ListView_InsertColumn(hBlockList, 0, &lvc);
	lvc.iSubItem = 1; lvc.pszText = "Status"; lvc.cx = 52;
	ListView_InsertColumn(hLayerList, 1, &lvc);
	ListView_InsertColumn(hTracingList, 1, &lvc);
	lvc.iSubItem = 2; lvc.pszText = "Segs"; lvc.cx = 40;
	ListView_InsertColumn(hLayerList, 2, &lvc);
	ListView_InsertColumn(hTracingList, 2, &lvc);

	//
	lvc.iSubItem = 3; lvc.pszText = "Color"; lvc.cx = 39;
	ListView_InsertColumn(hLayerList, 3, &lvc);
	lvc.iSubItem = 4; lvc.pszText = "Style"; lvc.cx = 65;
	ListView_InsertColumn(hLayerList, 4, &lvc);

	lvc.iSubItem = 1; lvc.pszText = "Refs"; lvc.cx = 35;
	ListView_InsertColumn(hBlockList, 1, &lvc);
	lvc.iSubItem = 2; lvc.pszText = "Segs"; lvc.cx = 40;
	ListView_InsertColumn(hBlockList, 2, &lvc);

	// enable full row select
	ListView_SetExtendedListViewStyle(hLayerList, LVS_EX_FULLROWSELECT);
	ListView_SetExtendedListViewStyle(hTracingList, LVS_EX_FULLROWSELECT);
	ListView_SetExtendedListViewStyle(hBlockList, LVS_EX_FULLROWSELECT);
	///////////////////////////////////////////////////////////////////////////////////////////
	
	for(int i = 0; i < pDoc->LayersGetSize(); i++)
	{
		CLayer* pLayer = pDoc->LayersGetAt(i);

		if(pLayer->IsInternal())
			InsertLayerItem(hDlg, pLayer);
		else
			InsertTracingItem(hDlg, pLayer);
	}

	for (int i = 0; i < sizeof(crHotCols) / sizeof(COLORREF); i++)
		::SendDlgItemMessage(hDlg, IDC_COLOR_ID, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) _itoa(i, szBuf, 10));
	
	pDoc->PenStylesFillCB(::GetDlgItem(hDlg, IDC_PENSTYLE));
	
	CString strKey;
	CBlock* pBlock;
	
	// fill block list
	POSITION pos = pDoc->BlksGetStartPosition();
	while(pos != NULL)
	{
		pDoc->BlksGetNextAssoc(pos, strKey, pBlock);
		if (!pBlock->IsAnonymous())
			InsertBlockItem(hDlg, strKey, pBlock);
	}

	WndProcPreviewClear(hDlg);
}

///<summary>
/// 
///</summary>
void DlgProcFileManageDoLayerColor(HWND hDlg, LPARAM lParam)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	int nIndex;
	CLayer *pLayer = GetItemSelected(hDlg, IDC_LAY2, nIndex);

	if(pLayer == 0)
		return;

	PENCOLOR nPenColor = (PENCOLOR)lParam;
	pLayer->SetPenColor(nPenColor);

	UpdateLayerItem(hDlg, pLayer);
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_LAYER_SAFE, pLayer);
	WndProcPreviewUpdate(hDlg, pLayer);
}

void DlgProcFileManageDoLayerStyle(HWND hDlg, LPARAM lParam)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	int nIndex;
	CLayer *pLayer = GetItemSelected(hDlg, IDC_LAY2, nIndex);

	if(pLayer == 0)
		return;

	CString strName = (LPSTR)lParam;
	pLayer->SetPenStyleName(strName);

	UpdateLayerItem(hDlg, pLayer);
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_LAYER_SAFE, pLayer);
	WndProcPreviewUpdate(hDlg, pLayer);
}