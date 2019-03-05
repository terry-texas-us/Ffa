#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "Pegaesysview.h"
#include "OwnerDraw.h"
#include "Polyline.h"

void DlgProcTranslatePensPaint(HWND hDlg, RECT dimen, RECT dimen2);
void SetupTranslatePens_DrawEntire(LPDRAWITEMSTRUCT, int);
void SetupTranslatePens_Init(HWND);

static PENCOLOR penPaintOld = 1;
static PENCOLOR penPaintNew = 1;

static PENSTYLE penStyleOld = 1;
static PENSTYLE penStyleNew = 1;

// holds data for the single layer version which is in the manage dialog
namespace translate
{
	bool bOneLayer = false;
	CString strLayerName = "0";
}

BOOL CALLBACK DlgProcTranslatePens(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	// rectangle for preview
	RECT dimen;
	dimen.top = 10;
	dimen.bottom = 30;
	dimen.left = 10;
	dimen.right = 140;

	RECT dimen2;
	dimen2.top = 10;
	dimen2.bottom = 30;
	dimen2.left = 154;
	dimen2.right = 284;

	// color
	char szBuf[16];

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			if(translate::bOneLayer)
				::SetWindowText(hDlg, "Translate Pens - Layer: " + translate::strLayerName);

			// pen colors
			for (int i = 0; i < sizeof(crHotCols) / sizeof(COLORREF); i++)
			{
				::SendDlgItemMessage(hDlg, IDC_TRANSPEN_OLD, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) _itoa(i, szBuf, 10));
				::SendDlgItemMessage(hDlg, IDC_TRANSPEN_NEW, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) _itoa(i, szBuf, 10));
			}

			::SendDlgItemMessage(hDlg, IDC_TRANSPEN_OLD, CB_SETCURSEL, 1, 0L);
			::SendDlgItemMessage(hDlg, IDC_TRANSPEN_NEW, CB_SETCURSEL, 1, 0L);

			// pen styles
			pDoc->PenStylesFillCB(::GetDlgItem(hDlg, IDC_TRANSPEN_OLD2));
			pDoc->PenStylesFillCB(::GetDlgItem(hDlg, IDC_TRANSPEN_NEW2));

			::SendDlgItemMessage(hDlg, IDC_TRANSPEN_OLD2, CB_SETCURSEL, 1, 0L);
			::SendDlgItemMessage(hDlg, IDC_TRANSPEN_NEW2, CB_SETCURSEL, 1, 0L);

			penPaintOld = 1;
			penPaintNew = 1;

			penStyleOld = 1;
			penStyleNew = 1;

			return (TRUE);
			break;
		}
		case WM_PAINT:
		{
			DlgProcTranslatePensPaint(hDlg, dimen, dimen2);
			return (TRUE);
			break;
		}
		case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT) lParam;
			
			if(lpDIS->CtlID == IDC_TRANSPEN_OLD)
			{
				if (lpDIS->itemID == - 1)					// Empty combo box
					OwnerDraw_Focus(lpDIS, 0);				// Draw only focus rectangle
				else
				{
					switch (lpDIS->itemAction)
					{
						case ODA_DRAWENTIRE:
							SetupTranslatePens_DrawEntire(lpDIS, - 2);
							break;
						
						case ODA_SELECT:
							OwnerDraw_Select(lpDIS, 0);				  
							break;
						
						case ODA_FOCUS:
							OwnerDraw_Focus(lpDIS, 0);
							break;
					}

					penPaintOld = (PENCOLOR)lpDIS->itemID;
					InvalidateRect(hDlg, &dimen, TRUE);
				}
			}
			else if(lpDIS->CtlID == IDC_TRANSPEN_NEW)
			{
				if (lpDIS->itemID == - 1)					// Empty combo box
					OwnerDraw_Focus(lpDIS, 0);				// Draw only focus rectangle
				else
				{
					switch (lpDIS->itemAction)
					{
						case ODA_DRAWENTIRE:
							SetupTranslatePens_DrawEntire(lpDIS, - 2);
							break;
						
						case ODA_SELECT:
							OwnerDraw_Select(lpDIS, 0);				  
							break;
						
						case ODA_FOCUS:
							OwnerDraw_Focus(lpDIS, 0);
							break;
					}

					penPaintNew = (PENCOLOR)lpDIS->itemID;
					InvalidateRect(hDlg, &dimen2, TRUE);
				}
			}

		} 
		case WM_COMMAND:
		{
			if(HIWORD(wParam) == CBN_SELCHANGE)
			{
				switch(LOWORD(wParam))
				{
					case IDC_TRANSPEN_OLD:
					{
						LRESULT lrSel = ::SendDlgItemMessage(hDlg, IDC_TRANSPEN_OLD, CB_GETCURSEL, 0, 0L);
						if(lrSel != -1)
						{
							penPaintOld = (PENCOLOR)lrSel;

							InvalidateRect(hDlg, &dimen, TRUE);
							SendMessage(hDlg, WM_PAINT, 0, 0);
						}
					}
					case IDC_TRANSPEN_NEW:
					{
						LRESULT lrSel = ::SendDlgItemMessage(hDlg, IDC_TRANSPEN_NEW, CB_GETCURSEL, 0, 0L);
						if(lrSel != -1)
						{
							penPaintNew = (PENCOLOR)lrSel;

							InvalidateRect(hDlg, &dimen2, TRUE);
							SendMessage(hDlg, WM_PAINT, 0, 0);
						}
					}
					case IDC_TRANSTYLE_OLD:
					{
						LRESULT lrSel = ::SendDlgItemMessage(hDlg, IDC_TRANSTYLE_OLD, CB_GETCURSEL, 0, 0L);
						if(lrSel != -1)
						{
							penStyleOld = (PENSTYLE)lrSel;

							InvalidateRect(hDlg, &dimen, TRUE);
							SendMessage(hDlg, WM_PAINT, 0, 0);
						}
					}
					case IDC_TRANSTYLE_NEW:
					{
						LRESULT lrSel = ::SendDlgItemMessage(hDlg, IDC_TRANSTYLE_NEW, CB_GETCURSEL, 0, 0L);
						if(lrSel != -1)
						{
							penStyleNew = (PENSTYLE)lrSel;

							InvalidateRect(hDlg, &dimen2, TRUE);
							SendMessage(hDlg, WM_PAINT, 0, 0);
						}
					}
				}
			}

			switch (LOWORD(wParam)) 
			{
				case IDC_TRANSPEN_APPLY:
				{
					if(!translate::bOneLayer)
						CPegDoc::GetDoc()->PenTranslation(1, &penPaintNew, &penPaintOld);
					else
					{
						CLayer *pLayer = CPegDoc::GetDoc()->LayersGet(translate::strLayerName);
						pLayer->PenTranslation(1, &penPaintNew, &penPaintOld);
					}

					CPegView::GetActiveView()->InvalidateRect(0, true);
					break;
				}
				case IDC_TRANSTYLE_APPLY:
				{
					if(!translate::bOneLayer)
						CPegDoc::GetDoc()->PenStyleTranslation(penStyleNew, penStyleOld);
					else
					{
						CLayer *pLayer = CPegDoc::GetDoc()->LayersGet(translate::strLayerName);
						pLayer->PenStyleTranslation(penStyleNew, penStyleOld);
					}

					CPegView::GetActiveView()->InvalidateRect(0, true);
					break;
				}
				case IDOK:
				case IDCANCEL:
				{
					translate::bOneLayer = false;
					::EndDialog(hDlg, IDOK);
					return (TRUE);
				}
			}

			return TRUE;
			break;
		}
	}
	return FALSE; 
}

void SetupTranslatePens_Init(HWND hDlg)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	HWND hWndComboBox = ::GetDlgItem(hDlg, IDC_TRANSPEN_OLD);
	
	pDoc->PenStylesFillCB(hWndComboBox);
	PENSTYLE nPenStyle = pstate.PenStyle();
	::SendMessage(hWndComboBox, CB_SETCURSEL, nPenStyle, 0L);
}

void DlgProcTranslatePensPaint(HWND hDlg, RECT dimen, RECT dimen2)
{
	COLORREF old;
	PAINTSTRUCT ps;
	CDC dc;
	dc.Attach(::BeginPaint(hDlg, &ps));

	CDC m_dcMem;
	m_dcMem.CreateCompatibleDC( NULL );

	// draw pen color
	dc.SetTextColor(RGB(0, 0, 0));
	old = dc.SetBkColor(RGB(255, 255, 255));

	CBrush brush(crHotCols[app.BackgroundColorGet()]);
	dc.FillRect(&dimen, &brush);
	dc.FillRect(&dimen2, &brush);

	////////////////////////////////////////////////////////////////////////////
	// draw pen style

	dc.SetTextColor(RGB(0, 0, 0));
	dc.SetBkColor(crHotCols[penPaintOld]);

	PENCOLOR nPenColor = pstate.PenColor();
	PENSTYLE nPenStyle = pstate.PenStyle();

	// old style
	pstate.SetPen(&dc, penPaintOld, penStyleOld);
				
	viewport::PushActive();
	viewport::Set(dimen.right + dimen.left, dimen.bottom + dimen.top);

	double dUExt = double(dimen.right + dimen.left) / double(dc.GetDeviceCaps(LOGPIXELSX)); 
	double dVExt = double(dimen.bottom + dimen.top) / double(dc.GetDeviceCaps(LOGPIXELSY)); 
	
	view::PushActive();
	view::Initialize();
	view::SetWnd(0., 0., dUExt, dVExt);

	view::SetTarget(ORIGIN);
	view::SetEye(ZDIR);

	// HACK - 0.1 margins
	CLine ln(CPnt(0.1, dVExt / 2., 0.), CPnt(dUExt - 0.09, dVExt / 2., 0.));
	ln.Display(&dc);
	
	view::PopActive();
	viewport::PopActive();

	// new style
	pstate.SetPen(&dc, penPaintNew, penStyleNew);
				
	viewport::PushActive();
	viewport::Set(dimen2.right + dimen2.left, dimen2.bottom + dimen2.top);

	dUExt = double(dimen2.right + dimen2.left) / double(dc.GetDeviceCaps(LOGPIXELSX)); 
	dVExt = double(dimen2.bottom + dimen2.top) / double(dc.GetDeviceCaps(LOGPIXELSY)); 
	
	view::PushActive();
	view::Initialize();
	view::SetWnd(0., 0., dUExt, dVExt);

	view::SetTarget(ORIGIN);
	view::SetEye(ZDIR);

	// HACK - 0.1 margins
	CLine ln2(CPnt(1.61, dVExt / 2., 0.), CPnt(dUExt - 1.6, dVExt / 2., 0.));
	ln2.Display(&dc);
	
	view::PopActive();
	viewport::PopActive();

	///////////////////////////////////////////////////////////
	pstate.SetPen(&dc, nPenColor, nPenStyle);
	
	// restore defaults
	dc.SetBkColor(old);
	dc.Detach();

	::EndPaint(hDlg, &ps);
}

// pen functions
///<summary>Draw the pen color number and a rectangle filled with the pen color.</summary>
void  SetupTranslatePens_DrawEntire(LPDRAWITEMSTRUCT lpDIS, int inflate)
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