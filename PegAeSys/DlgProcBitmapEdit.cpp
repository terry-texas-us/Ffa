#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

//#include "filejob.h"

//#include "StateInfoFrm.h"
//#include "DlgProcPrintPreview.h"

//#include "DlgProcPlotPreview.h"

#include "DlgProcBitmapEdit.h"

//////////////////////////////////////////////////////////////////////////////////////////////////

//CSoloStateFrm soloStateFrame(0, 100, 200, 300);
//CSoloStateFrm::CSoloStateFrm(int x, int y, int width, int height) { SetInfo(x, y, width, height); }
//void CSoloStateFrm::OnPaint(HWND) {}

//////////////////////////////////////////////////////////////////////////////////////////////////

CBitmapEdit bitmapEdit(0, 100, 200, 300);
CBitmapEdit::CBitmapEdit(int x, int y, int width, int height) { SetInfo(x, y, width, height); }
void CBitmapEdit::OnPaint(HWND) {}

//void DlgProcSoloStateRestore(HWND hDlg);

BOOL CALLBACK DlgProcBitmapEdit(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();
//	CPegView *pView = CPegView::GetActiveView();

//	UINT bState;
//	bool bValue;

	//static int nSelect = IDC_PLOT_1;

	static CPnt pt[8];

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
			point.x = cRect.right - 105;
			point.y = cRect.bottom - 361;

			SetWindowPos(hDlg, HWND_TOPMOST, point.x, point.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

			return (TRUE);
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{
				case IDC_STOP_EDIT:
					pDoc->OnBitmapEdit();
					return TRUE;
				case IDC_BITMAP_MELT:
					trapsegs.SetActLikePrim(false);
					trapsegs.RemoveAll();
					CPegView::GetActiveView()->InvalidateRect(0, TRUE);
					//POSITION pos = trapsegs.GetHeadPosition();
					//while (pos != 0)
					//{
					//	CSeg* pSeg = trapsegs.GetNext(pos);
					return TRUE;
				//case IDC_BITMAP_SELECT1:
				//	if (wPrvKeyDwn != ID_OP4) 
				//	{
				//		pt[0] = app.CursorPosGet();
				//		app.RubberBandingStartAtEnable(pt[0], Rectangles);
				//	}
				//	else 
				//	{
				//		pt[1] = app.CursorPosGet();
				//		app.RubberBandingDisable();
				//	}
				//	return TRUE;
				//case IDC_BITMAP_SELECT1:

		
			}

			break;
		}
	}
	return (FALSE); 
}