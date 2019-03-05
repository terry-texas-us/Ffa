#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysView.h"

#include "Preview.h"

CBitmap* WndProcPreview_pBitmap = NULL;

LRESULT CALLBACK WndProcPreview(HWND hwnd, UINT nMsg, UINT nParam, LPARAM lParam)
{
	switch (nMsg)
	{
		case WM_CREATE:
			WndProcPreviewOnCreate(hwnd);
			return (FALSE);
		case WM_DESTROY:
			WndProcPreviewOnDestroy(hwnd);
			return (FALSE);
		case WM_PAINT:
			WndProcPreviewOnDraw(hwnd);
			return (FALSE);
		case WM_SETFOCUS:
			TRACE0("Preview WM_SETFOCUS message\n");
			break;
		case WM_KILLFOCUS:
			TRACE0("Preview WM_KILLFOCUS message\n");
			break;
		case WM_LBUTTONDOWN:
			::SetFocus(hwnd);
			TRACE0("Preview WM_LBUTTONDOWN message\n");
			return (FALSE);
		case WM_RBUTTONDOWN:
			TRACE0("Preview WM_RBUTTONDOWN message\n");
			break;
		case WM_KEYDOWN:
			TRACE0("Preview WM_KEYDOWN message\n");
			return (FALSE);
		case WM_CHAR:
			TRACE0("Preview WM_CHAR message\n");
			break;
	}
	return DefWindowProc(hwnd, nMsg, nParam, lParam);
}

void WndProcPreviewClear(HWND hDlg)
{
	HWND hWndPreview = ::GetDlgItem(hDlg, IDC_LAYER_PREVIEW);

	CRect rc; 
	::GetClientRect(hWndPreview, &rc);

	CDC dcMem;
	dcMem.CreateCompatibleDC(0);

	CBitmap* pBitmapOld = (CBitmap*) dcMem.SelectObject(WndProcPreview_pBitmap);
//	dcMem.PatBlt(0, 0, rc.right, rc.bottom, BLACKNESS);
	dcMem.FillSolidRect(0, 0, rc.right, rc.bottom, crHotCols[app.BackgroundColorGet()]);
	
	dcMem.SelectObject(pBitmapOld);
	::InvalidateRect(hWndPreview, 0, TRUE);
}		

void WndProcPreviewOnCreate(HWND hwnd)
{
	CPegView* pView = CPegView::GetActiveView();
	CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();

	CRect rc; 
	::GetClientRect(hwnd, &rc);
	WndProcPreview_pBitmap = new CBitmap;
	WndProcPreview_pBitmap->CreateCompatibleBitmap(pDC, int(rc.right), int(rc.bottom));
}

void WndProcPreviewOnDestroy(HWND)
{
	if (WndProcPreview_pBitmap != NULL)
	{
		delete WndProcPreview_pBitmap;
		WndProcPreview_pBitmap = NULL;
	}
}

void WndProcPreviewOnDraw(HWND hwnd)
{
	PAINTSTRUCT ps;
	
	CRect rc; 
	::GetClientRect(hwnd, &rc);

	CDC dc;
	dc.Attach(::BeginPaint(hwnd, &ps));
	
	CDC dcMem;
	dcMem.CreateCompatibleDC(NULL);
	
	CBitmap* pBitmap = dcMem.SelectObject(WndProcPreview_pBitmap);
	dc.BitBlt(0, 0, rc.right, rc.bottom, &dcMem, 0, 0, SRCCOPY);
	dcMem.SelectObject(pBitmap);

	dc.Detach();
	
	::EndPaint(hwnd, &ps);
}

void WndProcPreviewUpdate(HWND hDlg, CBlock* pBlock)
{
	HWND hWndPreview = ::GetDlgItem(hDlg, IDC_LAYER_PREVIEW);

	CRect rc; 
	::GetClientRect(hWndPreview, &rc);
		
	CDC dcMem;
	dcMem.CreateCompatibleDC(NULL);
	
	CBitmap* pBitmap = dcMem.SelectObject(WndProcPreview_pBitmap);
	
	// mak - display the background in the correct color
	dcMem.FillSolidRect(0, 0, rc.right, rc.bottom, crHotCols[app.BackgroundColorGet()]);
//	dcMem.FillSolidRect(0, 0, rc.right, rc.bottom, crHotCols[0]);
//	dcMem.PatBlt(0, 0, rc.right, rc.bottom, BLACKNESS);
	
	viewport::PushActive();
	viewport::Set(rc.right, rc.bottom);
	viewport::SetDeviceWidthInInches(double(dcMem.GetDeviceCaps(HORZSIZE)) / 25.4);
	viewport::SetDeviceHeightInInches(double(dcMem.GetDeviceCaps(VERTSIZE)) / 25.4);

	CTMat tm;

	CPnt ptMin(FLT_MAX, FLT_MAX, FLT_MAX);
	CPnt ptMax(- FLT_MAX, - FLT_MAX, - FLT_MAX);
	
	pBlock->GetExtents(ptMin, ptMax, tm);
	
	double dUExt = ptMax[0] - ptMin[0];
	double dVExt = ptMax[1] - ptMin[1];
		
	view::PushActive();

	view::SetCenteredWnd(dUExt, dVExt);

	CPnt ptTarget((ptMin[0] + ptMax[0]) / 2., (ptMin[1] + ptMax[1]) / 2., 0.);
	
	view::SetTarget(ptTarget);
	view::SetEye(view::GetDirection());
	
	// mak - show marker spot of base
	CPrimMark marker(9, pstate.PenStyle(), pBlock->GetBasePt());

	int iPrimState = pstate.Save();
	pBlock->Display(&dcMem, false);
	//mak - display marker base
	pstate.SetMarkStyle(3);
	marker.Display(&dcMem, false);
	pstate.Restore(&dcMem, iPrimState);
	
	view::PopActive();
	viewport::PopActive();

	dcMem.SelectObject(pBitmap);
	::InvalidateRect(hWndPreview, 0, TRUE);
}

void WndProcPreviewUpdate(HWND hDlg, CSegs* pSegs)
{
	HWND hWndPreview = ::GetDlgItem(hDlg, IDC_LAYER_PREVIEW);
	
	CRect rc; 
	::GetClientRect(hWndPreview, &rc);
	
	CDC dcMem;
	dcMem.CreateCompatibleDC(NULL);
	
	CBitmap* pBitmap = dcMem.SelectObject(WndProcPreview_pBitmap);

	// mak - display the background in the correct color
//	dcMem.FillSolidRect(0, 0, rc.right, rc.bottom, crHotCols[0]);
	dcMem.FillSolidRect(0, 0, rc.right, rc.bottom, crHotCols[app.BackgroundColorGet()]);
	
	viewport::PushActive();
	viewport::Set(rc.right, rc.bottom);
	viewport::SetDeviceWidthInInches(double(dcMem.GetDeviceCaps(HORZSIZE)) / 25.4);
	viewport::SetDeviceHeightInInches(double(dcMem.GetDeviceCaps(VERTSIZE)) / 25.4);

	CTMat tm;

	CPnt ptMin(FLT_MAX, FLT_MAX, FLT_MAX);
	CPnt ptMax(- FLT_MAX, - FLT_MAX, - FLT_MAX);
	
	pSegs->GetExtents(ptMin, ptMax, tm);
	
	double dUExt = ptMax[0] - ptMin[0];
	double dVExt = ptMax[1] - ptMin[1];

	view::PushActive();
	
	view::SetCenteredWnd(dUExt, dVExt);
	
	CPnt ptTarget((ptMin[0] + ptMax[0]) / 2., (ptMin[1] + ptMax[1]) / 2., 0.);
	
	view::SetTarget(ptTarget);
	view::SetEye(view::GetDirection());
		
	int iPrimState = pstate.Save();
	pSegs->Display(&dcMem);
	pstate.Restore(&dcMem, iPrimState);
	
	view::PopActive();
	viewport::PopActive();

	dcMem.SelectObject(pBitmap);
	::InvalidateRect(hWndPreview, 0, TRUE);
}