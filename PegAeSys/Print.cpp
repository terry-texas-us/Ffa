#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "Print.h"

namespace print
{
	bool	bUserAbort;
	HWND	hDlgPrint;

	// mak
	CString	strDeviceName;
	CString	strPortName;
}

///<summary>Process messages for the abort dialog.</summary>
BOOL CALLBACK print::AbortProc(HDC, int)
{
	MSG msg;
	
	while (!print::bUserAbort && ::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		if (!print::hDlgPrint || !::IsDialogMessage(print::hDlgPrint, &msg))
		{	// Message not intended for the abort dialog
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	return (!print::bUserAbort);
}

///<summary>Processes messages for printer abort dialog box.</summary>
BOOL CALLBACK print::DlgProcAbort(HWND hDlg, UINT nMsg, WPARAM , LPARAM )
{
	char szBuf[80];
	
	switch (nMsg)
	{
		case WM_INITDIALOG:
			sprintf(szBuf, "'%s' on '%s'", strDeviceName.GetBuffer(), strPortName.GetBuffer());
			::SetDlgItemText(hDlg, IDC_PROFILE, szBuf);
			::EnableMenuItem(::GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
			return TRUE;

		case WM_COMMAND:
			print::bUserAbort = true;
			::EnableWindow(::GetParent(hDlg), TRUE);
			::DestroyWindow(hDlg);
			print::hDlgPrint = 0;
			return TRUE;
	}
	return FALSE;
}

///<summary>Draws the all layers at 1 to 1 on a printer DC.</summary>
void print::FilePlot(WPARAM wParam)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CPnt ptMin;
	CPnt ptMax;
	CTMat tm;
		
	pDoc->GetExtents(ptMin, ptMax, tm);
	if (ptMin[0] >= ptMax[0] && ptMin[1] >= ptMax[1])
	{
		msgInformation("Drawing is empty. No plot produced.");
		return;
	}
	CPrintDialog dlg(FALSE);
	
	if (dlg.DoModal() == 0)
		return;
	
	::EnableWindow(app.GetSafeHwnd(), FALSE);

	CDC dc;
	dc.Attach(dlg.GetPrinterDC());

	// mak
	strDeviceName = dlg.GetDeviceName();
	strPortName = dlg.GetPortName();

	print::bUserAbort = false;
	bool bError = false;
	
	DLGPROC lpAbortDlg = (DLGPROC) MakeProcInstance((FARPROC) print::DlgProcAbort, app.GetInstance());
	print::hDlgPrint = ::CreateDialog(app.GetInstance(), MAKEINTRESOURCE(IDD_PRINTABORT), app.GetSafeHwnd(), lpAbortDlg);
	
	::ShowWindow(print::hDlgPrint, SW_NORMAL);
			
	ABORTPROC lpAbortProc = (ABORTPROC) MakeProcInstance((FARPROC) print::AbortProc, app.GetInstance());
	dc.SetAbortProc(lpAbortProc);
		
	// mak - I changed the name of the plot file to the filename
	DOCINFO di;
	::ZeroMemory(&di, sizeof(DOCINFO)); 
	di.cbSize = sizeof(DOCINFO);
//	di.lpszDocName = pDoc->GetTitle();
    	
	if (dc.StartDoc(&di) > 0)
	{
		int iHorzRes = dc.GetDeviceCaps(HORZRES);
		int iVertRes = dc.GetDeviceCaps(VERTRES);

		double dHorzSizeInches = dc.GetDeviceCaps(HORZSIZE) / 25.4;
		double dVertSizeInches = dc.GetDeviceCaps(VERTSIZE) / 25.4; 
		
		viewport::PushActive();
		viewport::Set(iHorzRes, iVertRes);
		viewport::SetDeviceWidthInInches(dHorzSizeInches);
		viewport::SetDeviceHeightInInches(dVertSizeInches);
		
		if (LOWORD(wParam) == ID_FILE_PLOT_HALF)
		{
			dHorzSizeInches *= 2.;
			dVertSizeInches *= 2.;
		}
		else if (LOWORD(wParam) == ID_FILE_PLOT_QUARTER)
		{
			dHorzSizeInches *= 4.;
			dVertSizeInches *= 4.;
		}
		view::PushActive();
		view::Initialize();
		view::SetWnd(0., 0., dHorzSizeInches, dVertSizeInches);
				
		double dPageMinY = 0.;
		double dPageMaxY = dVertSizeInches;

		double dMarginX = printStates.GetMarginX();
		double dMarginY = printStates.GetMarginY();

		while (dPageMinY < ptMax[1] - ptMin[1])
		{
			double dPageMinX = 0.;
			double dPageMaxX = dHorzSizeInches;

			while (dPageMinX < ptMax[0] - ptMin[0])
			{
				// mak - added the addition of ptMin to move the drawing origin
				// to the plotter origin.  The plotter was still clipping one pixel
				// on the left and bottom so I added the dMargin as a fudge factor remover
				if(printStates.GetDispLayer())
					view::SetTarget(CPnt(dPageMinX + ptMin[0] - dMarginX - .15, dPageMinY + ptMin[1] - dMarginY, 0.));
				else
					view::SetTarget(CPnt(dPageMinX + ptMin[0] - dMarginX, dPageMinY + ptMin[1] - dMarginY, 0.));

				// mak - this is the old one that replaced the above
//				view::SetTarget(CPnt(dPageMinX, dPageMinY, 0.));

				view::SetEye(ZDIR);
												
				if (dc.StartPage() > 0)
				{
					CPegView::GetActiveView()->BackgroundImageDisplay(&dc);
					pDoc->LayersDisplayAll(&dc);
//					pDoc->LayersPrintAll(&dc);

					if (dc.EndPage() <= 0)
					{
						bError = true;
						goto HERE;
//						break;
					}
				}
				dPageMinX = dPageMaxX;
				dPageMaxX += dHorzSizeInches;
			}
			if (bError)
				goto HERE;
//				break;
				
			dPageMinY = dPageMaxY;
			dPageMaxY += dVertSizeInches;
		}
HERE:
		if (!bError)
			dc.EndDoc();
		
		view::PopActive();	
		viewport::PopActive();
	}
	if (!print::bUserAbort)
	{
		::DestroyWindow(print::hDlgPrint);
	}
		::EnableWindow(app.GetSafeHwnd(), TRUE);
	dc.DeleteDC();
}

///<summary>Draws the current view on a printer DC.</summary>
void print::FilePrintCurrentView()
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CPrintDialog dlg(FALSE);
	
	if(dlg.DoModal() == 0)
		return;
	
	::EnableWindow(app.GetSafeHwnd(), FALSE);

	CDC dc;
	dc.Attach(dlg.GetPrinterDC());

	// mak
	strDeviceName = dlg.GetDeviceName();
	strPortName = dlg.GetPortName();

	print::bUserAbort = false;
	bool bError = FALSE;
	
	DLGPROC lpAbortDlg = (DLGPROC) MakeProcInstance((FARPROC) print::DlgProcAbort, app.GetInstance());
	print::hDlgPrint = ::CreateDialog(app.GetInstance(), MAKEINTRESOURCE(IDD_PRINTABORT), app.GetSafeHwnd(), lpAbortDlg);

	::ShowWindow(print::hDlgPrint, SW_NORMAL);
			
	ABORTPROC lpAbortProc = (ABORTPROC) MakeProcInstance((FARPROC) print::AbortProc, app.GetInstance());
	dc.SetAbortProc(lpAbortProc);
		
	DOCINFO di;
	::ZeroMemory(&di, sizeof(DOCINFO)); 
	di.cbSize = sizeof(DOCINFO);
	di.lpszDocName = "Current View";
    	
	if(dc.StartDoc(&di) > 0)
	{
		if(dc.StartPage() > 0)
		{
			int iHorzRes = dc.GetDeviceCaps(HORZRES);
			int iVertRes = dc.GetDeviceCaps(VERTRES);
			
			viewport::PushActive();
			viewport::Set(iHorzRes, iVertRes);
			viewport::SetDeviceWidthInInches(double(dc.GetDeviceCaps(HORZSIZE)) / 25.4);
			viewport::SetDeviceHeightInInches(double(dc.GetDeviceCaps(VERTSIZE)) / 25.4);
			
			view::PushActive();
			view::AdjustWnd(double(iVertRes) / double(iHorzRes));
						
			CPegView::GetActiveView()->BackgroundImageDisplay(&dc);
			pDoc->LayersDisplayAll(&dc);
			
			view::PopActive();
			viewport::PopActive();

			if(dc.EndPage() > 0)
				dc.EndDoc();
			else
				bError = true;
		}
	}
	else
		bError = true;

	if (!print::bUserAbort)
	{
		::DestroyWindow(print::hDlgPrint);
	}
	::EnableWindow(app.GetSafeHwnd(), TRUE);
	dc.DeleteDC();
}