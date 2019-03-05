#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "OwnerDraw.h"
#include "Polyline.h"

void DlgProcSetupTabPaint(HWND hDlg, RECT dimen);
void SetupPenColor_DrawEntire(LPDRAWITEMSTRUCT, int);
void SetupPenStyle_DrawEntire(LPDRAWITEMSTRUCT lpDIS, int inflate);
void SetupPenStyle_Init(HWND);

static PENCOLOR penPaint = 1;
static PENSTYLE penStylePaint = 1;

BOOL CALLBACK DlgProcSetupTabPen(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	// rectangle for preview
	RECT dimen;
	dimen.top = 10;
	dimen.bottom = 30;
	dimen.left = 20;
	dimen.right = 370;

	// color
	char szBuf[16];
	int i;
	PENCOLOR nPenColor;

	// style
	BOOL bTranslated;
	char szBuf2[32];
	PENSTYLE nPenStyle;

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			// color
			penPaint = pstate.PenColor();
			for (i = 0; i < sizeof(crHotCols) / sizeof(COLORREF); i++)
				::SendDlgItemMessage(hDlg, IDC_COL_LIST, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) _itoa(i, szBuf, 10));
			nPenColor = pstate.PenColor();
			::SendDlgItemMessage(hDlg, IDC_COL_LIST, CB_SETCURSEL, nPenColor, 0L);

			// style
			penStylePaint = pstate.PenStyle();
			SetupPenStyle_Init(hDlg);

			return (TRUE);
			break;
		}
		case WM_PAINT:
		{
			DlgProcSetupTabPaint(hDlg, dimen);

			return (TRUE);
			break;
		}
		case WM_DIALOG_FOCUS:
		{
			switch(pDoc->GetOptionsFocus())
			{
				case IDC_COL_LIST:
					SetFocus(::GetDlgItem(hDlg, IDC_COL_LIST));
					break;
				case IDC_PENSTYLES:
					SetFocus(::GetDlgItem(hDlg, IDC_PENSTYLES));
					break;
				default:
					SetFocus(GetWindow(hDlg,GW_CHILD));
					break;
			}
			return TRUE;
		}
		case WM_DIALOG_OK:
		{
			// color
			nPenColor = (PENCOLOR) ::GetDlgItemInt(hDlg, IDC_COL_LIST, 0, FALSE);
			pstate.SetPenColor(nPenColor);

			// style
			bTranslated = TRUE;
			nPenStyle = PENSTYLE(GetDlgItemInt(hDlg, IDC_PENSTYLES, &bTranslated, TRUE));
			if (bTranslated == 0)
			{
				bTranslated = ::GetDlgItemText(hDlg, IDC_PENSTYLES, (LPSTR) szBuf2, sizeof(szBuf2));
				if (bTranslated != 0)
					nPenStyle = pDoc->PenStylesLookup(szBuf2);
				bTranslated = nPenStyle != PENSTYLE_LookupErr;
			}
			if (bTranslated)
				pstate.SetPenStyle(nPenStyle);

			return TRUE;
			break;
		}
		case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT) lParam;
			
			if(lpDIS->CtlID == IDC_COL_LIST)
			{
				if (lpDIS->itemID == - 1)					// Empty combo box
					OwnerDraw_Focus(lpDIS, 0);				// Draw only focus rectangle
				else
				{
					switch (lpDIS->itemAction)
					{
						case ODA_DRAWENTIRE:
							SetupPenColor_DrawEntire(lpDIS, - 2);
							break;
						
						case ODA_SELECT:
							OwnerDraw_Select(lpDIS, 0);				  
							break;
						
						case ODA_FOCUS:
							OwnerDraw_Focus(lpDIS, 0);
							break;
					}

					penPaint = (PENCOLOR)lpDIS->itemID;
					InvalidateRect(hDlg, &dimen, TRUE);

				}
			}
			else if(lpDIS->CtlID == IDC_PENSTYLES)
			{
				if (lpDIS->itemID == - 1)	// Empty combo box .. Draw only focus rectangle
					OwnerDraw_Focus(lpDIS, 0);
				else
				{
					switch (lpDIS->itemAction)
					{
						case ODA_DRAWENTIRE:
							SetupPenStyle_DrawEntire(lpDIS, 0);
							break;
						
						case ODA_SELECT:
							OwnerDraw_Select(lpDIS, 0);	
							break;
						
						case ODA_FOCUS:
							OwnerDraw_Focus(lpDIS, 0);
							break;
					}

					penStylePaint = (PENSTYLE)lpDIS->itemID;
					InvalidateRect(hDlg, &dimen, TRUE);
				}
			}
			return TRUE;
			break;
		} 
		case WM_COMMAND:
		{
			if(HIWORD(wParam) == CBN_SELCHANGE)
			{
				if(LOWORD(wParam) == IDC_PENSTYLES)
				{
					LRESULT lrSel = ::SendDlgItemMessage(hDlg, IDC_PENSTYLES, CB_GETCURSEL, 0, 0L);
					if(lrSel != -1)
					{
						penStylePaint = (PENSTYLE)lrSel;

						InvalidateRect(hDlg, &dimen, TRUE);
						SendMessage(hDlg, WM_PAINT, 0, 0);
					}
				}
				else if(LOWORD(wParam) == IDC_COL_LIST)
				{
					LRESULT lrSel = ::SendDlgItemMessage(hDlg, IDC_COL_LIST, CB_GETCURSEL, 0, 0L);
					if(lrSel != -1)
					{
						penPaint = (PENCOLOR)lrSel;

						InvalidateRect(hDlg, &dimen, TRUE);
						SendMessage(hDlg, WM_PAINT, 0, 0);
					}
				}
			}

			return TRUE;
			break;
		}
	}
	return FALSE; 
}

void DlgProcSetupTabPaint(HWND hDlg, RECT dimen)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	COLORREF old;
	PAINTSTRUCT ps;
	CDC dc;
	dc.Attach(::BeginPaint(hDlg, &ps));

	CDC m_dcMem;
	m_dcMem.CreateCompatibleDC( NULL );

	dc.Rectangle(&dimen);

	// draw background
	dc.SetTextColor(RGB(0, 0, 0));
	old = dc.SetBkColor(RGB(255, 255, 255));		
//	CBrush brush(RGB(0, 0, 0));
//	dc.FillRect(&dimen, &brush);
	dc.FillSolidRect(dimen.left, dimen.top, dimen.right - dimen.left, 
		dimen.bottom - dimen.top, crHotCols[app.BackgroundColorGet()]);

	//////////////////////////////////////////////////////////////////////////
	// draw pen style
	char szBuf[32];
	::SendDlgItemMessage(hDlg, IDC_PENSTYLES, CB_GETLBTEXT, penStylePaint, (LPARAM) (LPCSTR) szBuf);

	PENSTYLE nStyle = pDoc->PenStylesLookup(szBuf);
	if(nStyle == PENSTYLE_LookupErr) {return;}

	dc.SetTextColor(RGB(0, 0, 0));
	dc.SetBkColor(crHotCols[penPaint]);

	PENCOLOR nPenColor = pstate.PenColor();
	PENSTYLE nPenStyle = pstate.PenStyle();
	pstate.SetPen(&dc, penPaint, nStyle);
//	pstate.SetPen(&dc, 0, nStyle);
				
	viewport::PushActive();
	viewport::Set(dimen.right + dimen.left, dimen.bottom + dimen.top);

	double dUExt = double(dimen.right + dimen.left) / double(dc.GetDeviceCaps(LOGPIXELSX)); 
	double dVExt = double(dimen.bottom + dimen.top) / double(dc.GetDeviceCaps(LOGPIXELSY)); 
	
	view::PushActive();
	view::Initialize();
	view::SetWnd(0., 0., dUExt, dVExt);

	view::SetTarget(ORIGIN);
	view::SetEye(ZDIR);

	// HACK - changed to 0.2 margins so it would fit on black... don't know why
	CLine ln(CPnt(0.2, dVExt / 2., 0.), CPnt(dUExt - 0.2, dVExt / 2., 0.));
	ln.Display(&dc);
	
	view::PopActive();
	viewport::PopActive();

	pstate.SetPen(&dc, nPenColor, nPenStyle);
	/////////////////////////////////////////////////////////
	
	dc.SetBkColor(old);
	dc.Detach();

	::EndPaint(hDlg, &ps);
}

// pen functions
///<summary>Draw the pen color number and a rectangle filled with the pen color.</summary>
void  SetupPenColor_DrawEntire(LPDRAWITEMSTRUCT lpDIS, int inflate)
{
	CDC dc;
	dc.Attach(lpDIS->hDC);

	char szBuf[32];

	CRect rc;
	::CopyRect(&rc, &lpDIS->rcItem);

	::SendMessage(lpDIS->hwndItem, CB_GETLBTEXT, lpDIS->itemID, (LPARAM) (LPCSTR) szBuf);

	dc.SetTextColor(RGB(0, 0, 0));
	dc.SetBkColor(RGB(255, 255, 255)), 	

	dc.ExtTextOut(rc.right - 16, rc.top + 2, ETO_CLIPPED, &rc, szBuf, lstrlen(szBuf), 0);

	::InflateRect(&rc, inflate - 2, inflate - 2);
	rc.right -= 24;
	
	CBrush brush(crHotCols[lpDIS->itemID]);
	dc.FillRect(&rc, &brush);

	OwnerDraw_Select(lpDIS, inflate);
	OwnerDraw_Focus(lpDIS, inflate);
	
	dc.Detach();
}

// pen style
///<summary>Draw the pen style number and a sample line showing its appearance.</summary>
void  SetupPenStyle_DrawEntire(LPDRAWITEMSTRUCT lpDIS, int)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CDC dc;
	dc.Attach(lpDIS->hDC);

	LRESULT lrIndex = ::SendMessage(lpDIS->hwndItem, CB_GETCURSEL, 0, 0);
	if (lrIndex == CB_ERR) {return;}

	char szBuf[32];
	::SendMessage(lpDIS->hwndItem, CB_GETLBTEXT, lpDIS->itemID, (LPARAM) (LPCSTR) szBuf);
	
	PENSTYLE nStyle = pDoc->PenStylesLookup(szBuf);
	if (nStyle == PENSTYLE_LookupErr) {return;}
		
	CRect rc;
	::CopyRect(&rc, &lpDIS->rcItem);
	
	dc.SetTextColor(RGB(0, 0, 0));
	dc.SetBkColor(RGB(255, 255, 255)), 	

	dc.ExtTextOut(rc.right - 72, rc.top + 2, ETO_CLIPPED, &rc, szBuf, lstrlen(szBuf), 0);
	
	PENCOLOR nPenColor = pstate.PenColor();
	PENSTYLE nPenStyle = pstate.PenStyle();
	pstate.SetPen(&dc, 0, nStyle);

	rc.right -= 80;
				
	viewport::PushActive();
	viewport::Set(rc.right + rc.left, rc.bottom + rc.top);

	double dUExt = double(rc.right + rc.left) / double(dc.GetDeviceCaps(LOGPIXELSX)); 
	double dVExt = double(rc.bottom + rc.top) / double(dc.GetDeviceCaps(LOGPIXELSY)); 
	
	view::PushActive();
	view::Initialize();
	view::SetWnd(0., 0., dUExt, dVExt);

	view::SetTarget(ORIGIN);
	view::SetEye(ZDIR);

	CLine ln(CPnt(0., dVExt / 2., 0.), CPnt(dUExt, dVExt / 2., 0.));
	ln.Display(&dc);
	
	view::PopActive();
	viewport::PopActive();
	
	OwnerDraw_Select(lpDIS, 0);
	OwnerDraw_Focus(lpDIS, 0);
	
	pstate.SetPen(&dc, nPenColor, nPenStyle);
	dc.Detach();
}

void SetupPenStyle_Init(HWND hDlg)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	HWND hWndComboBox = ::GetDlgItem(hDlg, IDC_PENSTYLES);
	
	pDoc->PenStylesFillCB(hWndComboBox);
	PENSTYLE nPenStyle = pstate.PenStyle();
	::SendMessage(hWndComboBox, CB_SETCURSEL, nPenStyle, 0L);
}