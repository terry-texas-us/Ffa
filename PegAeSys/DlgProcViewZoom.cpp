#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "DlgProcViewZoom.h"
#include "Polygon.h"

HBITMAP CDlgViewZoom::m_hbmKeyplan = 0;
CRect	CDlgViewZoom::m_rcWnd;

CModelView PegView_mvLast;

bool bKeyplan = false;

void DlgProcViewZoomInit(HWND hDlg);
void DlgProcViewZoomRefresh(HWND hDlg);
void DlgProcViewZoomDoBest();
void DlgProcViewZoomDoMoreLess(HWND hDlg);
void DlgProcViewZoomDoLast();
void DlgProcViewZoomDoOK(HWND hDlg);
void DlgProcViewZoomDoOverview();
void DlgProcViewZoomDoPan(double dRatio);

BOOL CALLBACK DlgProcViewZoom(HWND ahDlg, UINT anMsg, WPARAM wParam, LPARAM)
{
	WORD wNotifyCode = HIWORD(wParam);
	switch (anMsg) 
	{
		case WM_INITDIALOG: 									
			DlgProcViewZoomInit(ahDlg);
			return (TRUE);

		case WM_COMMAND:
			switch (LOWORD(wParam)) 
			{
				case IDOK:
					DlgProcViewZoomDoOK(ahDlg);
					::EndDialog(ahDlg, TRUE);
					return (TRUE);
					
				case IDCANCEL:
					::EndDialog(ahDlg, FALSE);
					return (TRUE);

				case IDC_BEST:
					DlgProcViewZoomDoBest();
					::EndDialog(ahDlg, wParam);
					return (TRUE);
					
				case IDC_MORELESS:
					DlgProcViewZoomDoMoreLess(ahDlg);
					return (TRUE);
				
				case IDC_LAST:
					DlgProcViewZoomDoLast();
					::EndDialog(ahDlg, wParam);
					return (TRUE);
				
				case IDC_NORMAL:
					DlgProcViewZoomDoPan(1.);
					::EndDialog(ahDlg, wParam);
					return (TRUE);

				case IDC_OVERVIEW:
					DlgProcViewZoomDoOverview();
					::EndDialog(ahDlg, wParam);
					return (TRUE);
				
				case IDC_PAN:
					DlgProcViewZoomDoPan(viewport::GetWidthInInches() / view::GetUExt());
					::EndDialog(ahDlg, wParam);
					return (TRUE);

				case IDC_RATIO:
					if (wNotifyCode == EN_CHANGE)
					{
						if (GetFocus() == ::GetDlgItem(ahDlg, IDC_RATIO))
						{
							double dRatio = DlgBoxGetItemDouble(ahDlg, IDC_RATIO);
							if (dRatio >= DBL_EPSILON)
								::SendDlgItemMessage(ahDlg, IDC_KEYPLAN_AREA, WM_USER_ON_NEW_RATIO, 0, (LPARAM) (LPDWORD) &dRatio);
							return (TRUE);
						}
					}
					break;
			}
			break;
	}
	return (FALSE); 											
}

void DlgProcViewZoomDoBest()
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CPnt ptMin;
	CPnt ptMax;
	
	pDoc->GetExtents(ptMin, ptMax, view::GetMatrix());
	
	// extents return range - 1 to 1
	if (ptMin[0] < ptMax[0])
	{
		view::GetActive(PegView_mvLast);
		
		double dUExt = view::GetUExt() * (ptMax[0] - ptMin[0]) / 2.;
		double dVExt = view::GetVExt() * (ptMax[1] - ptMin[1]) / 2.;
		
		view::SetCenteredWnd(dUExt, dVExt);
		
		CTMat tm; pDoc->GetExtents(ptMin, ptMax, tm);
		
		CPnt ptTarget = CPnt((ptMin[0] + ptMax[0]) / 2., (ptMin[1] + ptMax[1]) / 2., (ptMin[2] + ptMax[2]) / 2.);
		
		view::SetTarget(ptTarget);
		view::SetEye(view::GetDirection());
		
		app.CursorPosSet(ptTarget);
	}
}

void DlgProcViewZoomDoLast()
{
	CModelView vTmp;
	view::GetActive(vTmp);
	view::SetActive(PegView_mvLast);
	PegView_mvLast = vTmp;
}

void DlgProcViewZoomDoMoreLess(HWND hDlg)
{	
	// Enables/disables the keyplan.  Enabling the keyplan causes the dialog to grow.
	// Info indicating size and position are buried in a control.
	
	char szBuf[16];
	::GetWindowText(::GetDlgItem(hDlg, IDC_LESS_AREA), szBuf, sizeof(szBuf));
	
	if (bKeyplan)
	{
		CRect rcDef;
		
		::GetWindowRect(::GetDlgItem(hDlg, IDC_LESS_AREA), &rcDef);
		::SetWindowPos(hDlg, 0, 0, 0, atoi(szBuf), rcDef.Height(), SWP_NOZORDER | SWP_NOMOVE);
	}
	else
	{	
		::SetWindowPos(hDlg, 0, 0, 0, atoi(szBuf), atoi(szBuf + 6), SWP_NOZORDER | SWP_NOMOVE);
	}
	bKeyplan = !bKeyplan;
	::EnableWindow(::GetDlgItem(hDlg, IDC_KEYPLAN_AREA), bKeyplan);

	DlgProcViewZoomRefresh(hDlg);
}

void DlgProcViewZoomDoOK(HWND hDlg)
{
	CRect rcKey;
	
	if (bKeyplan)
	{
		::GetClientRect(::GetDlgItem(hDlg, IDC_KEYPLAN_AREA), &rcKey);
						
		CPnt ptTarget;
		ptTarget = view::GetTarget();
		double dUMinOverview = vOver.GetUMin();
		double dVMinOverview = vOver.GetVMin();

		double dUMin = CDlgViewZoom::m_rcWnd.left / double(rcKey.right) * vOver.GetUExt() + dUMinOverview;
		double dUMax = CDlgViewZoom::m_rcWnd.right / double(rcKey.right) * vOver.GetUExt() + dUMinOverview;
		
		double dVMin = (- CDlgViewZoom::m_rcWnd.bottom / double(rcKey.bottom + 1)) * vOver.GetVExt() + dVMinOverview;
		double dVMax = (- CDlgViewZoom::m_rcWnd.top / double(rcKey.bottom + 1)) * vOver.GetVExt() + dVMinOverview;
		
		double dRatio = viewport::GetWidthInInches() / fabs(dUMax - dUMin);
		view::GetActive(PegView_mvLast);
		view::AdjustWnd(dUMin, dVMin, dUMax, dVMax, dRatio);
		view::SetWnd(dUMin, dVMin, dUMax, dVMax);
	}
	else
	{	
		double dRatio = DlgBoxGetItemDouble(hDlg, IDC_RATIO);
		if (dRatio >= DBL_EPSILON)
		{
			view::GetActive(PegView_mvLast);
			DoWindowPan(app.GetSafeHwnd(), dRatio);
		}
	}
}

void DlgProcViewZoomDoOverview()
{
	view::Initialize();
}

void DlgProcViewZoomDoPan(double dRatio)
{
	view::GetActive(PegView_mvLast);

	DoWindowPan(app.GetSafeHwnd(), dRatio);
}

void DlgProcViewZoomInit(HWND hDlg)
{
	double dViewportWidth = viewport::GetWidthInInches();
	double dViewportHeight = viewport::GetHeightInInches(); 
	
	double dRatio = dViewportWidth / view::GetUExt();
	DlgBoxSetItemDouble(hDlg, IDC_RATIO, dRatio);
	
	double dUMin = - dViewportWidth * .5; 
	double dUMax = dUMin + dViewportWidth;

	double dVMin = - dViewportHeight * .5; 
	double dVMax = dVMin + dViewportHeight;

	view::AdjustWnd(dUMin, dVMin, dUMax, dVMax, 1.);
	
	double dUExt = fabs(dUMax - dUMin);
	double dVExt = fabs(dVMax - dVMin);

	CPnt ptCur = app.CursorPosGet();
	CVec vDirection = view::GetDirection();
	CPnt ptTarget = view::GetTarget();
	line::IntersectionWithPln(ptCur, vDirection, ptTarget, vDirection, &ptCur);
				
	dUMin = ptCur[0] - (dUExt * .5); 
	dUMax = dUMin + dUExt;
	dVMin = ptCur[1] - (dVExt * .5); 
	dVMax = dVMin + dVExt;
			
	CRect rcKey;
	::GetClientRect(::GetDlgItem(hDlg, IDC_KEYPLAN_AREA), &rcKey);
	
	double dUMinOverview = ptTarget[0] + vOver.GetUMin();
	double dVMinOverview = ptTarget[1] + vOver.GetVMin();

	CDlgViewZoom::m_rcWnd.left =   Round((dUMin - dUMinOverview) / vOver.GetUExt() * rcKey.right);
	CDlgViewZoom::m_rcWnd.right =  Round((dUMax - dUMinOverview) / vOver.GetUExt() * rcKey.right);
	CDlgViewZoom::m_rcWnd.top =    Round((1. - (dVMax - dVMinOverview) / vOver.GetVExt()) * rcKey.bottom);
	CDlgViewZoom::m_rcWnd.bottom = Round((1. - (dVMin - dVMinOverview) / vOver.GetVExt()) * rcKey.bottom);
			
	CRect rcMoreArea;
	::GetWindowRect(hDlg, &rcMoreArea);
		
	char szBuf[16];
	wsprintf(szBuf, "%05u %05u", rcMoreArea.Width(), rcMoreArea.Height());
	
	::SetWindowText(::GetDlgItem(hDlg, IDC_LESS_AREA), szBuf);
	
	::EnableWindow(::GetDlgItem(hDlg, IDC_KEYPLAN_AREA), bKeyplan);
	
	DlgProcViewZoomRefresh(hDlg);
}

void DlgProcViewZoomRefresh(HWND hDlg)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	CPegView* pView = CPegView::GetActiveView();
	CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();

	char szBuf[16];
	::GetWindowText(::GetDlgItem(hDlg, IDC_LESS_AREA), szBuf, sizeof(szBuf));
	
	if (bKeyplan)
	{
		::SetWindowText(::GetDlgItem(hDlg, IDC_MORELESS), "&Less");
	
		CDC dcMem;
		dcMem.CreateCompatibleDC(NULL);
		if (CDlgViewZoom::m_hbmKeyplan == 0)
		{
			CRect rcKey;
			::GetClientRect(::GetDlgItem(hDlg, IDC_KEYPLAN_AREA), &rcKey);
			CDlgViewZoom::m_hbmKeyplan = CreateCompatibleBitmap(pDC->GetSafeHdc(), rcKey.right, rcKey.bottom);
		}
		dcMem.SelectObject(CBitmap::FromHandle(CDlgViewZoom::m_hbmKeyplan));
		
		BITMAP bitmap;
		::GetObject(CDlgViewZoom::m_hbmKeyplan, sizeof(BITMAP), (LPSTR) &bitmap);
		dcMem.PatBlt(0, 0, bitmap.bmWidth, bitmap.bmHeight, BLACKNESS);
	
		// Build a view volume which provides an overview bitmap

		viewport::PushActive();
		viewport::Set(bitmap.bmWidth, bitmap.bmHeight);
		viewport::SetDeviceWidthInInches(double(dcMem.GetDeviceCaps(HORZSIZE)) / 25.4);
		viewport::SetDeviceHeightInInches(double(dcMem.GetDeviceCaps(VERTSIZE)) / 25.4);
		
		view::PushActive();
		view::Initialize();
		view::GetActive(vOver);
										
		CPrim::SpecPolygonStyle() = POLYGON_HOLLOW;
		pDoc->LayersDisplayAll(&dcMem);
		CPrim::SpecPolygonStyle() = - 1;

		::InvalidateRect(::GetDlgItem(hDlg, IDC_KEYPLAN_AREA), 0, TRUE);
		
		view::PopActive();
		viewport::PopActive();
	}
	else
	{			
		::SetWindowText(::GetDlgItem(hDlg, IDC_MORELESS), "&More");
		
		CRect rcLessArea;
		::GetWindowRect(::GetDlgItem(hDlg, IDC_LESS_AREA), &rcLessArea);
		::SetWindowPos(hDlg, 0, 0, 0, atoi(szBuf), rcLessArea.Height(), SWP_NOZORDER | SWP_NOMOVE);
	}
}