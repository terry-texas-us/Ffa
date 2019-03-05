#include "stdafx.h"

#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

void DlgProcEditTrap_CommandsQueryFillPrimList(HWND);
void DlgProcEditTrap_CommandsQueryFillAttrList(HWND);
void DlgProcEditTrap_CommandsQueryFillPtsList(HWND);
void DlgProcEditTrap_CommandsQueryOnDelete(HWND);
void DlgProcEditTrap_CommandsQueryOnPtsSelChange(HWND hDlg, LRESULT index);
CString DlgProcEditTrap_CommandsQueryGetType(CPrim *pPrim);
void DlgProcEditTrap_CommandsQueryFillGeometryList2(HWND hDlg, CPrim* pPrim);
void DlgProcEditTrap_CommandsQueryRemove(HWND hDlg, CPrim *pPrim);

BOOL CALLBACK DlgProcEditTrap_CommandsQuery(HWND hDlg, UINT anMsg, WPARAM awParam, LPARAM lParam)
{
	HWND hWndGeometry = ::GetDlgItem(hDlg, IDC_DRWG_REP_PNT);
	HWND hWndExtra = ::GetDlgItem(hDlg, IDC_DRWG_REP_ATTR);
	WORD wNotifyCode = HIWORD(awParam);

	switch (anMsg)
	{ 
		case WM_INITDIALOG:
		{
			CString 	sBuf;
			char*		pBuf = sBuf.GetBuffer(8);

			for (int i = 0; i < trapsegs.GetCount(); i++)
			{
				sprintf(pBuf, "%05i", i);
				SendDlgItemMessage(hDlg, IDC_DRWG_REP, LB_ADDSTRING, 0, (LPARAM) (LPCSTR) pBuf);
			}
			sBuf.ReleaseBuffer();

			SendDlgItemMessage(hDlg, IDC_DRWG_REP, LB_SETCURSEL, 0, 0L);

			LVCOLUMN lvc;
			lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			lvc.fmt = LVCFMT_LEFT;
			lvc.cx = 75;

			lvc.iSubItem = 0; lvc.pszText = "Property";
			ListView_InsertColumn(hWndExtra, 0, &lvc); 
			lvc.iSubItem = 1; lvc.pszText = "Value";
			ListView_InsertColumn(hWndExtra, 1, &lvc);

			lvc.iSubItem = 0; lvc.pszText = "Property";
			ListView_InsertColumn(hWndGeometry, 0, &lvc); 
			lvc.iSubItem = 1; lvc.pszText = "X-Axis";
			ListView_InsertColumn(hWndGeometry, 1, &lvc);
			lvc.iSubItem = 2; lvc.pszText = "Y-Axis";
			ListView_InsertColumn(hWndGeometry, 2, &lvc);
			lvc.iSubItem = 3; lvc.pszText = "Z-Axis";
			ListView_InsertColumn(hWndGeometry, 3, &lvc);
		
			DlgProcEditTrap_CommandsQueryFillPrimList(hDlg);
			DlgProcEditTrap_CommandsQueryFillAttrList(hDlg);
			DlgProcEditTrap_CommandsQueryFillPtsList(hDlg);
			return (TRUE);
		}
		case  WM_NOTIFY:
		{
			LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)lParam;

			if(lpnmitem->hdr.code == NM_DBLCLK)
			{
				if(lpnmitem->hdr.idFrom == IDC_DRWG_REP_PNT)
				{
					// only available in top view
					if((view::GetDirection() != ZDIR) || (view::GetVwUp() != YDIR))
						return FALSE;

					LVHITTESTINFO hitTest;
					hitTest.pt = lpnmitem->ptAction;

					LRESULT lResult = SendMessage(GetDlgItem(hDlg, IDC_DRWG_REP_PNT), (UINT)LVM_SUBITEMHITTEST, 
						0, (LPARAM)&hitTest);

					if(lResult != -1)
						DlgProcEditTrap_CommandsQueryOnPtsSelChange(hDlg, lResult);
				}
			}

			return TRUE;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(awParam))
			{
				case ID_REP_DELETE:
					DlgProcEditTrap_CommandsQueryOnDelete(hDlg);
					break;

				case IDC_DRWG_REP:
					if(wNotifyCode == LBN_SELCHANGE)
					{
						ListView_DeleteAllItems(hWndExtra);
						ListView_DeleteAllItems(hWndGeometry);

						DlgProcEditTrap_CommandsQueryFillPrimList(hDlg);
						DlgProcEditTrap_CommandsQueryFillAttrList(hDlg);
						DlgProcEditTrap_CommandsQueryFillPtsList(hDlg);
					}
					break;

				case IDC_DRWG_REP_PRIMS:
					if(wNotifyCode == LBN_SELCHANGE)
					{
						ListView_DeleteAllItems(hWndExtra);
						ListView_DeleteAllItems(hWndGeometry);

						DlgProcEditTrap_CommandsQueryFillAttrList(hDlg);
						DlgProcEditTrap_CommandsQueryFillPtsList(hDlg);
					}
					break;

				case IDOK:
				case IDCANCEL:
					EndDialog(hDlg, TRUE);
					return (TRUE);
			}
		}
	}
	return (FALSE);
}

void DlgProcEditTrap_CommandsQueryOnDelete(HWND hDlg)
{
	int iCurSelSeg = int(SendDlgItemMessage(hDlg, IDC_DRWG_REP, LB_GETCURSEL, 0, 0L));
	int iCurSelPrim = int(SendDlgItemMessage(hDlg, IDC_DRWG_REP_PRIMS, LB_GETCURSEL, 0, 0L));
					
	if (iCurSelSeg != LB_ERR && iCurSelPrim != LB_ERR)
	{
		POSITION pos = trapsegs.FindIndex(iCurSelSeg);
		if (pos == NULL)
			return;
		CSeg* pSeg = (CSeg*) trapsegs.GetAt(pos);
		pos = pSeg->FindIndex(iCurSelPrim);
		if (pos == NULL)
			return;
		CPrim* pPrim = pSeg->GetAt(pos);

		DlgProcEditTrap_CommandsQueryRemove(hDlg, pPrim);

		SendDlgItemMessage(hDlg, IDC_DRWG_REP, LB_DELETESTRING, iCurSelSeg, 0L);
		SendDlgItemMessage(hDlg, IDC_DRWG_REP, LB_SETCURSEL, 0, 0L);

		ListView_DeleteAllItems(::GetDlgItem(hDlg, IDC_DRWG_REP_PNT));
		ListView_DeleteAllItems(::GetDlgItem(hDlg, IDC_DRWG_REP_ATTR));
		SendDlgItemMessage(hDlg, IDC_DRWG_REP_PRIMS, LB_RESETCONTENT, 0, 0);
		
		DlgProcEditTrap_CommandsQueryFillPrimList(hDlg);
		DlgProcEditTrap_CommandsQueryFillAttrList(hDlg);
		DlgProcEditTrap_CommandsQueryFillPtsList(hDlg);
	}
}	

void DlgProcEditTrap_CommandsQueryOnPtsSelChange(HWND hDlg, LRESULT index)
{
	HWND hLV = GetDlgItem(hDlg, IDC_DRWG_REP_PNT);

	char buffer[12];
	char *pEnd;
	CPnt point;

	if(index != -1)
	{
		// get coordinates
		LVITEM lvItem;
		lvItem.mask = LVIF_TEXT;
		lvItem.pszText = buffer;
		lvItem.cchTextMax = 12;
		lvItem.iItem = index;
		
		lvItem.iSubItem = 1;
		ListView_GetItem(hLV, &lvItem);
		point[0] = strtod(buffer, &pEnd);

		lvItem.iSubItem = 2;
		ListView_GetItem(hLV, &lvItem);
		point[1] = strtod(buffer, &pEnd);

		app.CursorPosSet(point);
	}
}

void DlgProcEditTrap_CommandsQueryFillAttrList(HWND hDlg)
{
	int iCurSelSeg = int(SendDlgItemMessage(hDlg, IDC_DRWG_REP, LB_GETCURSEL, 0, 0L));
	int iCurSelPrim = int(SendDlgItemMessage(hDlg, IDC_DRWG_REP_PRIMS, LB_GETCURSEL, 0, 0L));
					
	if (iCurSelSeg != LB_ERR && iCurSelPrim != LB_ERR)
	{
		POSITION pos = trapsegs.FindIndex(iCurSelSeg);
		if (pos == NULL)
			return;
		CSeg* pSeg = (CSeg*) trapsegs.GetAt(pos);
		pos = pSeg->FindIndex(iCurSelPrim);
		if (pos == NULL)
			return;
		CPrim* pPrim = pSeg->GetAt(pos);

		//////////////////////////////////////////
		HWND hWndExtra = ::GetDlgItem(hDlg, IDC_DRWG_REP_ATTR);

		LVITEM lvi; 
		::ZeroMemory(&lvi, sizeof(lvi));
		lvi.mask = LVIF_TEXT | LVIF_STATE;
		
		char szBuf[256];
		
		int iItem = 0;
		
		CString str;
		pPrim->FormatExtra(str);
		
		size_t nOff = 0;
		for (size_t nDel = str.Mid(nOff).Find(';'); nDel != - 1;)
		{
			lvi.iItem = iItem; 
			lvi.pszText = strcpy(szBuf, str.Mid(nOff, nDel));
			ListView_InsertItem (hWndExtra, &lvi);
			nOff += nDel + 1;
			nDel = str.Mid(nOff).Find('\t');
			size_t nLen = min(nDel, sizeof(szBuf) - 1);
			strcpy(szBuf, str.Mid(nOff, nLen));
			ListView_SetItemText(hWndExtra, iItem++, 1, szBuf);
			nOff += nDel + 1;
			nDel = str.Mid(nOff).Find(';');
		}			
		/////////////////////////////////////////////////////
	}
}

void DlgProcEditTrap_CommandsQueryFillPtsList(HWND hDlg)
{
	int iCurSelSeg = int(SendDlgItemMessage(hDlg, IDC_DRWG_REP, LB_GETCURSEL, 0, 0L));
	int iCurSelPrim = int(SendDlgItemMessage(hDlg, IDC_DRWG_REP_PRIMS, LB_GETCURSEL, 0, 0L));
					
	if (iCurSelSeg != LB_ERR && iCurSelPrim != LB_ERR)
	{					
		POSITION pos = trapsegs.FindIndex(iCurSelSeg);
		if (pos == NULL)
			return;
		CSeg* pSeg = (CSeg*) trapsegs.GetAt(pos);
		
		pos = pSeg->FindIndex(iCurSelPrim);
		if (pos == NULL)
			return;
		CPrim* pPrim = pSeg->GetAt(pos);
						

		HWND hWndGeometry = ::GetDlgItem(hDlg, IDC_DRWG_REP_PNT);
		DlgProcEditTrap_CommandsQueryFillGeometryList2(hWndGeometry, pPrim);
	}
}	

void DlgProcEditTrap_CommandsQueryFillPrimList(HWND hDlg)
{
	int iCurSel = int(SendDlgItemMessage(hDlg, IDC_DRWG_REP, LB_GETCURSEL, 0, 0L));

	if (iCurSel != LB_ERR)
	{
		CString sBuf = "test";

		SendDlgItemMessage(hDlg, IDC_DRWG_REP_PRIMS, LB_RESETCONTENT, 0, 0L);
						
		POSITION pos = trapsegs.FindIndex(iCurSel);
		if (pos == NULL)
			return;
		CSeg *pSeg = (CSeg*)trapsegs.GetAt(pos);
			
		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim != NULL)
		{
			CPrim *pPrim = pSeg->GetNext(posPrim);
			sBuf = DlgProcEditTrap_CommandsQueryGetType(pPrim);

			SendDlgItemMessage(hDlg, IDC_DRWG_REP_PRIMS, LB_ADDSTRING, 0, 
				(LPARAM) (LPCSTR) sBuf);
		}

		SendDlgItemMessage(hDlg, IDC_DRWG_REP_PRIMS, LB_SETCURSEL, 0, 0L);
	}
}

CString DlgProcEditTrap_CommandsQueryGetType(CPrim *pPrim)
{
	if(pPrim->Is(CPrim::PRIM_LINE))
		return "Line";
	else if(pPrim->Is(CPrim::PRIM_ARC))
		return "Arc";
	else if(pPrim->Is(CPrim::PRIM_SEGREF))
		return "Seg Ref";
	else if(pPrim->Is(CPrim::PRIM_TEXT))
		return "Text";
	else if(pPrim->Is(CPrim::PRIM_POLYGON))
		return "Polygon";
	else if(pPrim->Is(CPrim::PRIM_POLYLINE))
		return "Polyline";
	else if(pPrim->Is(CPrim::PRIM_MARK))
		return "Mark";
	else if(pPrim->Is(CPrim::PRIM_INSERT))
		return "Insert";
	else if(pPrim->Is(CPrim::PRIM_BSPLINE))
		return "BSpline";
	else if(pPrim->Is(CPrim::PRIM_CSPLINE))
		return "CSpline";
	else if(pPrim->Is(CPrim::PRIM_TAG))
		return "Tag";
	else if(pPrim->Is(CPrim::PRIM_DIM))
		return "Dimension";
	else if(pPrim->Is(CPrim::PRIM_BITMAP))
		return "Bitmap";

	else 
		return "Unknown Type";
}

void DlgProcEditTrap_CommandsQueryRemove(HWND, CPrim *pPrim)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();
	CPegView *pView = CPegView::GetActiveView();

	// remove seg from trapsegs
	POSITION pos = trapsegs.GetHeadPosition();
	while(pos != 0)
	{
		CSeg *pSeg = trapsegs.GetNext(pos);

		POSITION index = pSeg->Find(pPrim);
		if(index != 0)
		{
			trapsegs.Remove(pSeg);
			break;
		}
	}

	// remove seg from document
	for (WORD w = 0; w < pDoc->LayersGetSize(); w++)
	{
		CLayer* pLayer = pDoc->LayersGetAt(w);

		POSITION pos = pLayer->GetHeadPosition();
		while(pos != 0)
		{
			CSeg *pSeg = pLayer->GetNext(pos);

			POSITION index = pSeg->Find(pPrim);
			if(index != 0)
			{
				pLayer->Remove(pSeg);
				break;
			}
		}
	}
	pView->InvalidateRect(NULL, TRUE);
} 

void DlgProcEditTrap_CommandsQueryFillGeometryList2(HWND hWndGeometry, CPrim* pPrim)
{
	LVITEM lvi; 
	::ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT | LVIF_STATE;

	char szBuf[64];
	int iItem = 0;
	
	CString strBuf;
	pPrim->FormatGeometry(strBuf);

	size_t nOff = 0;
	for (size_t nDel = strBuf.Mid(nOff).Find(';'); nDel != - 1;)
	{
		lvi.iItem = iItem; 
		lvi.pszText = strcpy(szBuf, strBuf.Mid(nOff, nDel));
		ListView_InsertItem (hWndGeometry, &lvi);
		nOff += nDel + 1;
		nDel = strBuf.Mid(nOff).Find(';');
		strcpy(szBuf, strBuf.Mid(nOff, nDel));
		ListView_SetItemText(hWndGeometry, iItem, 1, szBuf);
		nOff += nDel + 1;
		nDel = strBuf.Mid(nOff).Find(';');
		strcpy(szBuf, strBuf.Mid(nOff, nDel));
		ListView_SetItemText(hWndGeometry, iItem, 2, szBuf);
		nOff += nDel + 1;
		nDel = strBuf.Mid(nOff).Find('\t');
		strcpy(szBuf, strBuf.Mid(nOff, nDel));
		ListView_SetItemText(hWndGeometry, iItem++, 3, szBuf);
		nOff += nDel + 1;
		nDel = strBuf.Mid(nOff).Find(';');
	}							 
}

void DlgProcEditTrap_CommandsQueryGoToPoint(HWND hDlg, CPrim*)
{
	HWND hWndGeometry = ::GetDlgItem(hDlg, IDC_GEOMETRY_LIST);
	int index = ListView_GetSelectionMark(hWndGeometry);
	
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = index;
	lvItem.iSubItem = 1;
	ListView_GetItem(hWndGeometry, &lvItem); 
	CString temp = lvItem.pszText;

	HWND hWndGeometry2 = ::GetDlgItem(hDlg, IDC_GEOMETRY_LIST);
	LVITEM lvItem2;
	lvItem2.mask = LVIF_TEXT;
	lvItem2.iItem = index;
	lvItem2.iSubItem = 2;
	ListView_GetItem(hWndGeometry2, &lvItem2); 
	CString temp2 = lvItem2.pszText;
}