// PegAEsysView.cpp : implementation of the CPegView class
#include "stdafx.h"

#include "MainFrm.h"
#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "Dde.h"
#include "DdeGItms.h"

#include "DlgProcSetLength.h"
#include "FileBitmap.h"
#include "Grid.h"
#include "UnitsString.h"

#include "SubProcDraw.h"

#include "Job2State.h"
extern CJob2State job2state;

#include "scroll.h"
#include "clip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "DlgProcPrintPreview.h"

#include "Mouse.h"

// mak - status windows
#include "StateInfoFrm.h"
extern CInfoStatusFrm statusframe;
extern CInfoModeFrm modeframe;
extern CInfoOdomFrm odomframe;
extern CBackgroundLayer BackgroundLayer;

extern CModelView PegView_mvLast;
extern CTMat tmEditSeg;

#include "DlgProcPrintPreview.h"
//extern CPrintFrm printframe;
extern CStateFrm stateframe;
extern CSoloStateFrm soloStateFrame;

#include "DlgProcPlotPreview.h"
extern CPlotFrm plotframe;

#include "DlgProcBitmapEdit.h"
extern CBitmapEdit bitmapEdit;
#include "DlgProcBackgroundImage.h"
extern CBackgroundImage backgroundImage;

UINT CALLBACK OFNHookProcBitmapLoad(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);

CPegView::CPegView()
{
	m_bViewClipping = false;
	m_bViewBackgroundImage = false;
	m_bViewOdometer = false;
	m_bViewPenWidths = false;
	m_bViewPrintPreview = false;
	m_bViewRendered = false;
	m_bViewTrueTypeFonts = true;
	m_bViewWireframe = false;
	m_bViewScrollBars = true;
	m_bViewOrigin = true;
	m_bViewColorByLayer = false;

	m_nBackgroundTrans = 90;
	m_dBackgroundScaleX = 1.;
	m_dBackgroundScaleY = 1.;
	m_dBackgroundRotation = 0.;

	m_eClipMode = MODE_NONE;
	m_eDefaultView = VIEW_UNCHANGED;
}

void CPegView::BackgroundImageDisplay(CDC* pDC)
{
	//if(app.GetPegState() == STATE_NORMAL)
	{
		if(!m_bViewBackgroundImage || (HBITMAP) m_bmBackgroundImage == NULL)
			return;
	}

	{
		BITMAP bm;
		m_bmBackgroundImage.GetBitmap(&bm);
		CDC dcMem;
		dcMem.CreateCompatibleDC(NULL);
		CBitmap* pBitmap = dcMem.SelectObject(&m_bmBackgroundImage);

		double dRatio = vpActive.GetWidthInInches() / view::GetUExt();

		BITMAP bm2;
		m_bmBackgroundImage.GetBitmap(&bm2);
		CSize m_size(bm2.bmWidth, bm2.bmHeight);

		int iWidDst = (int)(m_size.cx * dRatio);
		int iHgtDst = (int)(m_size.cy * dRatio);

		// get distance to translate up
		//CPnt4 pnt22(0, m_size.cy, 0, 1);
		//ModelView_DoTransform(pnt22);
		//CPoint pt22 = viewport::DoProjection(pnt22);
		/*viewport::DoProjectionInverse(pnt22);
		m_ptLR = pnt22;
		m_ptLR = view::GetMatrixInverse() * m_ptLR;*/

		//CPnt m_ptUL(0, 0, 0);
		CPnt m_ptUL = backgroundImage.GetPoint();
		CPnt m_ptLR;

		CPnt4 pntUL(m_ptUL, 1.);
		ModelView_DoTransform(pntUL);
		CPoint ptUL = viewport::DoProjection(pntUL);

		BLENDFUNCTION blend;
		blend.BlendOp = AC_SRC_OVER;
		blend.BlendFlags = 0;
		blend.SourceConstantAlpha = (BYTE)m_nBackgroundTrans;
		blend.AlphaFormat = 0;

		pDC->AlphaBlend(ptUL.x, ptUL.y - iHgtDst, iWidDst, iHgtDst, &dcMem, 0, 0, m_size.cx, m_size.cy, blend);
		//pDC->StretchBlt(ptUL.x, ptUL.y - iHgtDst, iWidDst, iHgtDst, &dcMem, 0, 0, m_size.cx, m_size.cy, SRCCOPY);

		//m_BackgroundBitmap->Display(pDC, false);
		
		dcMem.SelectObject(pBitmap);
	}
}

// works
//void CPegView::BackgroundImageDisplay(CDC* pDC)
//{
//	//if(app.GetPegState() == STATE_NORMAL)
//	{
//		if(!m_bViewBackgroundImage || (HBITMAP) m_bmBackgroundImage == NULL)
//			return;
//	}
//
//	//{
//		int iWidDst = int(vpActive.GetWidth());
//		int iHgtDst = int(vpActive.GetHeight());
//		
//		BITMAP bm;
//		m_bmBackgroundImage.GetBitmap(&bm);
//		CDC dcMem;
//		dcMem.CreateCompatibleDC(NULL);
//		CBitmap* pBitmap = dcMem.SelectObject(&m_bmBackgroundImage);
//		CPalette* pPalette = pDC->SelectPalette(&m_palBackgroundImage, FALSE);
//		pDC->RealizePalette();
//		
//		//CPnt ptTargetActive = vActive.GetTarget();
//		CPnt ptTargetActive = view::GetTarget();
//		CPnt ptTargetOver = vOver.GetTarget();
//		double dU = ptTargetActive[0] - ptTargetOver[0];
//		double dV = ptTargetActive[1] - ptTargetOver[1];
//
//		////////////////////////////////////////////////////////////////////////////
//		// find "correct" dimensions
//		////////////////////////////////////////////////////////////////////////////
//		double ratio = viewport::GetAspectRatio();
//		double invRatio = 1/ratio;
//
//		if(bm.bmHeight > (int)(invRatio * (double)bm.bmWidth))
//			bm.bmWidth = (int)(invRatio * (double)bm.bmHeight);
//		else
//			bm.bmHeight = (int)(ratio * (double)bm.bmWidth);
//		////////////////////////////////////////////////////////////////////////////
//		
//		// Determine the region of the bitmap to tranfer to display
//		CRect rcWnd;
//		rcWnd.left =   Round((view::GetUMin() - vOver.GetUMin() + dU) / vOver.GetUExt() * (double) bm.bmWidth);
//		rcWnd.top =    Round((1. - (view::GetVMax() - vOver.GetVMin() + dV) / vOver.GetVExt()) * (double) bm.bmHeight);
//		rcWnd.right =  Round((view::GetUMax() - vOver.GetUMin() + dU) / vOver.GetUExt() * (double) bm.bmWidth);
//		rcWnd.bottom = Round((1. - (view::GetVMin() - vOver.GetVMin() + dV) / vOver.GetVExt()) * (double) bm.bmHeight);
//	
//		int iWidSrc = rcWnd.Width();
//		int iHgtSrc = rcWnd.Height();
//
//		BLENDFUNCTION blend;
//		blend.BlendOp = AC_SRC_OVER;
//		//blend.BlendFlags = 0;
//		blend.SourceConstantAlpha = 128;
//		blend.AlphaFormat = 0;
//
//		//pDC->AlphaBlend(0, 0, iWidDst, iHgtDst, &dcMem, (int) rcWnd.left, (int) rcWnd.top, iWidSrc, iHgtSrc, blend);
//		pDC->StretchBlt(0, 0, iWidDst, iHgtDst, &dcMem, (int) rcWnd.left, (int) rcWnd.top, iWidSrc, iHgtSrc, SRCCOPY);
//		
//		dcMem.SelectObject(pBitmap);
//		pDC->SelectPalette(pPalette, FALSE);
//	//}
//}

void CPegView::OnClippingRegion()
{
	m_bViewClipping = !m_bViewClipping;

	static int nContext = -99;

	if(m_bViewClipping)
	{
		HDC hdc = ::GetDC(app.GetSafeHwnd());
		app.CheckMenuItem(ID_VIEW_CLIP, MF_CHECKED);
		nContext = ::SaveDC(hdc);
		ReleaseDC(app.GetSafeHwnd(), hdc); 
	}
	else
	{
		app.CheckMenuItem(ID_VIEW_CLIP, MF_UNCHECKED);
		HDC hdc = ::GetDC(app.GetSafeHwnd()); 
		::RestoreDC(hdc, nContext);
		ReleaseDC(app.GetSafeHwnd(), hdc); 
	}

	CPegView::GetActiveView()->InvalidateRect(NULL, true);
}

void CPegView::ClipDisplay(CDC*)
{
	if(IsClipped())
	{
		HDC hdc = ::GetDC(app.GetSafeHwnd()); 

		// paint background
		SelectClipRgn(hdc, clip::OldHrgn);
//		CPnt4 vPtUL = clip::ptUL;
//		CPnt4 vPtLR = clip::ptLR;
		CPnt4 vPtUL(job2state.m_rectClip.left, job2state.m_rectClip.top, 0, 1);
		CPnt4 vPtLR(job2state.m_rectClip.right, job2state.m_rectClip.bottom, 0, 1);

		ModelView_DoTransform(vPtUL);
		ModelView_DoTransform(vPtLR);
		CPoint pntUL = viewport::DoProjection(vPtUL);
		CPoint pntLR = viewport::DoProjection(vPtLR);

		// Use the rect coordinates to define a clipping region. 
		clip::Hrgn = CreateRectRgn(pntUL.x, pntUL.y, pntLR.x, pntLR.y);

		// select background region
		SelectClipRgn(hdc, clip::OldHrgn);
		HBRUSH hPinkBrush = ::CreateSolidBrush(crHotCols[0]);//RGB(180, 180, 180));
		::FillRect(hdc, &clip::OldRect, hPinkBrush);
		::DeleteObject(hPinkBrush);

		// select viewing region
		SelectClipRgn(hdc, clip::Hrgn);

		HBRUSH hBlackBrush = ::CreateSolidBrush(crHotCols[0]);
		::FillRect(hdc, &clip::OldRect, hBlackBrush);
		::DeleteObject(hBlackBrush);

		if(printStates.GetDispClipRect())
			::FrameRgn(hdc, clip::Hrgn, CBrush(RGB(127, 127, 127)), 1, 1);
		
		ReleaseDC(app.GetSafeHwnd(), hdc);
	}
}

void CPegView::OriginDisplay(CDC *pDC)
{
	if(m_bViewOrigin)
	{
		// get original color
		PENCOLOR pColor = pstate.PenColor();
		PENSTYLE pStyle = pstate.PenStyle();

		pstate.SetPen(pDC, (PENCOLOR)OriginGetColor1(), 1);

		CPnt4 ptView(CPnt(0, 0, 0), 1);
		ModelView_DoTransform(ptView);
		
	//	if(!Pnt4_IsInView(ptView))
		{
			CPoint pnt = viewport::DoProjection(ptView);
			pDC->MoveTo(pnt.x, pnt.y - 20); pDC->LineTo(pnt.x, pnt.y + 20);
			pDC->MoveTo(pnt.x - 20, pnt.y); pDC->LineTo(pnt.x + 20, pnt.y);
		}

		// working origin
		pstate.SetPen(pDC, (PENCOLOR)OriginGetColor2(), 1);
		CPnt4 ptView2(grid::GetOrign(), 1);
		ModelView_DoTransform(ptView2);

	//	if(!Pnt4_IsInView(ptView2))
		{
			// Set brush to hollow
			LOGBRUSH logbrush;
			logbrush.lbStyle = BS_HOLLOW;
			HBRUSH hbrush = CreateBrushIndirect(&logbrush);
			HBRUSH holdbrush = (HBRUSH)pDC->SelectObject((HBRUSH)hbrush);

			CPoint pnt = viewport::DoProjection(ptView2);
			pDC->Ellipse(pnt.x - 10, pnt.y - 10, pnt.x + 10, pnt.y + 10);
			pDC->MoveTo(pnt.x - 10, pnt.y - 10); pDC->LineTo(pnt.x + 10, pnt.y + 10);
			pDC->MoveTo(pnt.x - 10, pnt.y + 10); pDC->LineTo(pnt.x + 10, pnt.y - 10);

			pDC->SelectObject((HBRUSH)holdbrush);
		}

		//pstate.SetPenColor(pColor);
		//pstate.SetPenStyle(pStyle);
		pstate.SetPen(pDC, pColor, pStyle);
	}
}

void CPegView::PivotDisplay(CDC *pDC)
{
	if(app.m_iModeId == ID_MODE_EDIT)
	{
		// get original color
		PENCOLOR pColor = pstate.PenColor();
		PENSTYLE pStyle = pstate.PenStyle();

		CPnt4 ptView(trapsegs.PvtPt(), 1);
		ModelView_DoTransform(ptView);

		// working origin
		pstate.SetPen(pDC, (PENCOLOR)OriginGetColor2(), 1);

		//	if(!Pnt4_IsInView(ptView2))
		{
			// Set brush to hollow
			LOGBRUSH logbrush;
			logbrush.lbStyle = BS_HOLLOW;
			HBRUSH hbrush = CreateBrushIndirect(&logbrush);
			HBRUSH holdbrush = (HBRUSH)pDC->SelectObject((HBRUSH)hbrush);

			CPoint pnt = viewport::DoProjection(ptView);

			int nSize = 10;
			pDC->MoveTo(pnt.x - nSize, pnt.y - nSize); pDC->LineTo(pnt.x + nSize, pnt.y - nSize);
			pDC->MoveTo(pnt.x + nSize, pnt.y - nSize); pDC->LineTo(pnt.x + nSize, pnt.y + nSize);
			pDC->MoveTo(pnt.x + nSize, pnt.y + nSize); pDC->LineTo(pnt.x - nSize, pnt.y + nSize);
			pDC->MoveTo(pnt.x - nSize, pnt.y + nSize); pDC->LineTo(pnt.x - nSize, pnt.y - nSize);

			pDC->MoveTo(pnt.x - 15, pnt.y); pDC->LineTo(pnt.x + 15, pnt.y);
			pDC->MoveTo(pnt.x, pnt.y + 15); pDC->LineTo(pnt.x, pnt.y - 15);

			pDC->SelectObject((HBRUSH)holdbrush);
		}

		//pstate.SetPenColor(pColor);
		pstate.SetPen(pDC, pColor, pStyle);
	}
}

bool CPegView::OnEraseBkgnd(HWND hwnd, HDC hdc)
{
	hdc = ::GetDC(hwnd);

	HBRUSH hbr;

	if(IsClipped())
		hbr = ::CreateSolidBrush(RGB(180, 180, 180));
	else
		hbr = (HBRUSH)::GetClassLong(hwnd, GCL_HBRBACKGROUND);

	RECT rClient;
	::GetClientRect(hwnd, &rClient);

	::FillRect(hdc, &rClient, hbr);
	::ReleaseDC(hwnd, hdc);
	return 0;
}

void CPegView::DisplayOdometer()
{
	static int fuOptions = ETO_CLIPPED | ETO_OPAQUE;

	CPnt pt = app.CursorPosGet();

	// if the z coordinate is off of the 0 plane, turn on
	if(!m_bViewOdometer && pt[2] != 0)
		OdometerEnable(true);

	m_vRelPos = pt - grid::GetOrign();
	
	//dde::PostAdvise(dde::RelPosXInfo);
	//dde::PostAdvise(dde::RelPosYInfo);
	//dde::PostAdvise(dde::RelPosZInfo);
	
	if (m_bViewOdometer)
	{
		char szBuf[32];
		//if (app.RubberBandingGetType() == Lines)
		//{
		//	CLine ln(app.RubberBandingGetStart(), pt);
		//
		//	double dLen = ln.Length();
		//	double dAng = line::GetAngAboutZAx(ln);
		//	UnitsString_FormatLength(szBuf, max(app.GetUnits(), Engineering), dLen, 16, 4);

		//	CString strRep;
		//	strRep += szBuf;
		//	UnitsString_FormatAngle(szBuf, dAng);
		//	strRep += szBuf;	
	
		//	msgInformation(strRep);
		//}

		if(odomframe.GetDC() == 0) return;
		CDC *pDC = odomframe.GetDC();
		
		CFont* pFont = pDC->SelectObject(app.m_pFontApp);
		UINT nTextAlign = pDC->SetTextAlign(TA_LEFT | TA_TOP);
		COLORREF crText = pDC->SetTextColor(app.GetTextCol()); 

		// mak... took this line out... put in the one below
		COLORREF crBk = pDC->SetBkColor(RGB(128, 128, 128));//~app.GetTextCol());

		CRect rcClient; 
		::GetClientRect(odomframe.GetHWND(), &rcClient);
		GetClientRect(&rcClient);

		TEXTMETRIC tm; 
		pDC->GetTextMetrics(&tm);

		int iLeft = 3;
		CRect rc(iLeft, 0, 200, 200);

		////////////////////////////////////////////////////////////////////////
		// bitmap
		////////////////////////////////////////////////////////////////////////
		HBITMAP hBitmap = ::LoadBitmap(app.GetInstance(), MAKEINTRESOURCE(IDB_ODOM));
		if(hBitmap != NULL) 
		{
			CDC dcMemory;
			dcMemory.CreateCompatibleDC(0);

			HBITMAP hOldBitmap = (HBITMAP)SelectObject(dcMemory, hBitmap);
			pDC->BitBlt(0, 0, 160, 160, &dcMemory, 0, 0, SRCCOPY);

			SelectObject(dcMemory, hOldBitmap);

			DeleteObject(hBitmap);
			DeleteObject(hOldBitmap);
		}
		
		///////////////////////////////////////////////////////////////////////
		//	Displays cursor position in cartesian format
		///////////////////////////////////////////////////////////////////////
		//print "Cartesian"
		crText = pDC->SetTextColor(crHotCols[11]);								
		
		// mak - for white backgrounds, etc.
		pDC->SetBkMode(TRANSPARENT);
//		crBk = pDC->SetBkColor(RGB(255,255,255));//crHotCols[0]);

		// ************* moved to bitmap
//		rc.SetRect(iLeft, rcClient.top + 2 * tm.tmHeight, rcClient.right, rcClient.top + 3 * tm.tmHeight);
//		UnitsString_FormatLength(szBuf, app.GetUnits(), m_vRelPos[0], 16, 8);
//		pDC->ExtTextOut(rc.left, rc.top, fuOptions, &rc, "Cartesian", 9, NULL);
		crText = pDC->SetTextColor(app.GetTextCol());	

		rc.SetRect(iLeft, rcClient.top + 3 * tm.tmHeight, rcClient.right, rcClient.top + 4 * tm.tmHeight);
		UnitsString_FormatLength(szBuf, app.GetUnits(), m_vRelPos[0], 16, 8);
		pDC->ExtTextOut(rc.left, rc.top, fuOptions, &rc, szBuf, (UINT) strlen(szBuf), 0);
		
		rc.SetRect(iLeft, rcClient.top + 4 * tm.tmHeight, rcClient.right, rcClient.top + 5 * tm.tmHeight);
		UnitsString_FormatLength(szBuf, app.GetUnits(), m_vRelPos[1], 16, 8);
		pDC->ExtTextOut(rc.left, rc.top, fuOptions, &rc, szBuf, (UINT) strlen(szBuf), 0);
		
		rc.SetRect(iLeft, rcClient.top + 5 * tm.tmHeight, rcClient.right, rcClient.top + 6 * tm.tmHeight);
		UnitsString_FormatLength(szBuf, app.GetUnits(), m_vRelPos[2], 16, 8);
		pDC->ExtTextOut(rc.left, rc.top, fuOptions, &rc, szBuf, (UINT) strlen(szBuf), 0);

		///////////////////////////////////////////////////////////////////////
		//DISPLAYS POLAR CORDINATE SYSTEM IN ODOMETER
		///////////////////////////////////////////////////////////////////////
		double magnitude, angle;

		magnitude = sqrt(m_vRelPos[0] * m_vRelPos[0] +
						 m_vRelPos[1] * m_vRelPos[1] +
						 m_vRelPos[2] * m_vRelPos[2]);
		
		angle = atan(m_vRelPos[1] / m_vRelPos[0]);			//format 
		angle = angle * 180 / PI;							//angle
		if (m_vRelPos[0] < 0 && m_vRelPos[1] > 0)
			angle = 180 + angle;
		else if (m_vRelPos[0] < 0)
			angle += 180;
		else if (m_vRelPos[0] > 0 && m_vRelPos[1] < 0)
			angle = 360 + angle;
		else if (m_vRelPos[0] == 0 && m_vRelPos[1] < 0)
			angle = 270;
		
		// ************* moved to bitmap
		////print "polar"
		//crText = pDC->SetTextColor(crHotCols[11]);
		//rc.SetRect(iLeft, rcClient.top + 6 * tm.tmHeight, rcClient.right, rcClient.top + 7 * tm.tmHeight);
		//UnitsString_FormatLength(szBuf, app.GetUnits(), m_vRelPos[0], 16, 8);
		//pDC->ExtTextOut(rc.left, rc.top, fuOptions, &rc, "Polar", 5, NULL);
		crText = pDC->SetTextColor(app.GetTextCol());

		//print magnitude
		rc.SetRect(iLeft, rcClient.top + 7 * tm.tmHeight, rcClient.right, rcClient.top + 8 * tm.tmHeight);
		UnitsString_FormatLength(szBuf, app.GetUnits(), magnitude, 16, 8);
		pDC->ExtTextOut(rc.left, rc.top, fuOptions, &rc, szBuf, strlen(szBuf), NULL);

		//print angle from positive x direction
		rc.SetRect(iLeft, rcClient.top + 8 * tm.tmHeight, rcClient.right, rcClient.top + 9 * tm.tmHeight);
		char szBuf3[32];
		_ltoa(long(angle), szBuf3, 10);
		strcpy(szBuf, " ");
		strcat(szBuf, szBuf3);
		strcat(szBuf, "°");
		pDC->ExtTextOut(rc.left, rc.top, fuOptions, &rc, szBuf, strlen(szBuf), NULL);

		/// print layer name of segment under mouse
		CPegDoc *pDoc = CPegDoc::GetDoc();
		CSeg *pSeg = detsegs.DetSeg();
		CPrim *pPrim = detsegs.DetPrim();
		
		pt = app.CursorPosGet();
		CLayer* pLayer = pDoc->LayersSelUsingPoint(pt);

		detsegs.SetSeg(pSeg);
		detsegs.SetPrim(pPrim);

		if((pLayer == NULL) || (pLayer->IsOff()))
			strcpy(szBuf, " ");
		else
			strcpy(szBuf, pLayer->GetName());

		if(pLayer != NULL)
		{
			if(pLayer->IsMapped())								// mapped tracing layer
				crText = pDC->SetTextColor(crHotCols[82]);
			else if(pLayer->IsViewed())							// viewed tracing layer
				crText = pDC->SetTextColor(crHotCols[142]);
			else if(pLayer->IsWarm())							// active layer
				crText = pDC->SetTextColor(crHotCols[30]);
			else if(pLayer->IsCold())							// static layer
				crText = pDC->SetTextColor(crHotCols[50]);
			else if(pLayer->IsHot())							// working layer
				crText = pDC->SetTextColor(crHotCols[9]);
		}

		rc.SetRect(iLeft, rcClient.top + 0 * tm.tmHeight, rcClient.right, rcClient.top + 1 * tm.tmHeight);
		pDC->ExtTextOut(rc.left, rc.top, fuOptions, &rc, szBuf, strlen(szBuf), NULL);

		// extract pen color and style from primitive
		CSeg* pSegIt = detsegs.SelSegAndPrimUsingPoint(pt);

		if(pSegIt != 0)
		{
			int nPenColor = detsegs.DetPrim()->PenColor();
			int nPenStyle = detsegs.DetPrim()->PenStyle();

			CString strTemp;
			strTemp.Format("  P%i  L%i", nPenColor, nPenStyle);

//			crText = pDC->SetTextColor(app.GetTextCol());
			rc.SetRect(iLeft, rcClient.top + 1 * tm.tmHeight, rcClient.right, rcClient.top + 2 * tm.tmHeight);
			pDC->ExtTextOut(rc.left, rc.top, fuOptions, &rc, strTemp.GetBuffer(), strlen(strTemp.GetBuffer()), NULL);
		}

		// restore device context to its original state
		pDC->SetBkColor(crBk);
		pDC->SetTextColor(crText);
		pDC->SetTextAlign(nTextAlign);
		pDC->SelectObject(pFont);

		// fixed Q and A key problems... set the detsegs after done
		detsegs.SetSeg(pSeg);
		detsegs.SetPrim(pPrim);
	}  
}

void CPegView::DoCameraRotate(int iDir)
{
	CVec vN = view::GetEye() - view::GetTarget();
	vN.Normalize();

	CVec vU(view::GetVwUp() ^ vN);
	vU.Normalize();
	
	CVec vV(vN ^ vU);
	vU.Normalize();
	
	CPnt ptEye;

	switch (iDir) 
	{
		case ID_CAMERA_ROTATELEFT:
		{
			ptEye = Pnt_RotAboutArbPtAndAx(view::GetEye(), view::GetTarget(), vV, - 10. * RADIAN);
			view::SetEye(ptEye);
			break;
		}
		case ID_CAMERA_ROTATERIGHT:
			ptEye = Pnt_RotAboutArbPtAndAx(view::GetEye(), view::GetTarget(), vV, 10. * RADIAN);
			view::SetEye(ptEye);
			break;

		case ID_CAMERA_ROTATEUP:
			ptEye = Pnt_RotAboutArbPtAndAx(view::GetEye(), view::GetTarget(), vU, - 10. * RADIAN);
			view::SetEye(ptEye);
			break;

		case ID_CAMERA_ROTATEDOWN:
			ptEye = Pnt_RotAboutArbPtAndAx(view::GetEye(), view::GetTarget(), vU, 10. * RADIAN);
			view::SetEye(ptEye);
			break;

		case ID_CAMERA_ZROT_LEFT:
			vV.RotAboutZAx(sin(-5. * RADIAN), cos(-5. * RADIAN));
			//vV.RotAboutZAx(sin(-1.), cos(-1.));
			break;

		case ID_CAMERA_ZROT_RIGHT:
			vV.RotAboutZAx(sin(5. * RADIAN), cos(5. * RADIAN));
			//vV.RotAboutZAx(sin(1.), cos(1.));
			break;
	}

	//view::SetEye(ptEye);
	view::SetVwUp(vV);
	InvalidateRect(NULL, TRUE);
}

void CPegView::DoCustomMouseClick(LPTSTR lpCmds)
{
	HWND hWndTarget = GetFocus();
	char* pIdx = &lpCmds[0];
	while (*pIdx != 0)
	{		   
		if (*pIdx == '{')
		{			
			*pIdx++;
			int iVkValue = atoi(pIdx);
			::PostMessage(hWndTarget, WM_KEYDOWN, iVkValue, 0L);
			while ((*pIdx != 0) && (*pIdx != '}')) 
				pIdx++;
		}
		else
		{
			::PostMessage(hWndTarget, WM_CHAR, *pIdx, 0L);
			pIdx++; 
		}
	}
}

void CPegView::DoWindowPan0(int dir)
{
	CPnt pt = app.CursorPosGet();
	
	double dRatio = viewport::GetWidthInInches() / view::GetUExt();
	double d = 1. / dRatio;
	
	switch (dir)
	{
		case ID_WINDOW_PAN_LEFT: 				  
			pt[0] -= d;
			break;
		
		case ID_WINDOW_PAN_RIGHT:
			pt[0] += d;
			break;
		
		case ID_WINDOW_PAN_UP: 	   
			pt[1] += d;
			break;
		
		case ID_WINDOW_PAN_DOWN: 					
			pt[1] -= d;
			break;
	}
	view::SetTarget(pt);
	view::SetEye(view::GetDirection());
	
	app.CursorPosSet(pt);
	InvalidateRect(NULL, TRUE);
}	

CPegView* CPegView::GetActiveView()
{
	return &viewThe;
}

void CPegView::On3dViewsAxonometric()
{
	int lResult = ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_PROJ_AXONOMETRIC), GetSafeHwnd(), DlgProcProjAxonometric);
	InvalidateRect(NULL, TRUE);

	if(lResult == IDOK)
		SetProjectionMenuText(ID_3DVIEWS_AXONOMETRIC);
}

void CPegView::On3dViewsBack()
{
	view::SetEye(YDIR);
	view::SetDirection(YDIR);
	view::SetVwUp(ZDIR);
	view::SetPerspectiveEnabled(false);
	InvalidateRect(NULL, TRUE);

	SetProjectionMenuText(ID_3DVIEWS_BACK);
}

void CPegView::On3dViewsBottom()
{
	view::SetEye(- ZDIR);
	view::SetDirection(- ZDIR);
	view::SetVwUp(YDIR);
	view::SetPerspectiveEnabled(false);
	InvalidateRect(NULL, TRUE);

	SetProjectionMenuText(ID_3DVIEWS_BOTTOM);
}

void CPegView::On3dViewsFront()
{
	view::SetEye(- YDIR);
	view::SetDirection(- YDIR);
	view::SetVwUp(ZDIR);
	view::SetPerspectiveEnabled(false);
	InvalidateRect(NULL, TRUE);

	SetProjectionMenuText(ID_3DVIEWS_FRONT);
}

void CPegView::On3dViewsIsometric()
{
//	m_wOpenFocus = IDC_COL_LIST;
//	m_wOpenTab = TAB_OPEN_ISOMETRIC;
	CPegDoc *pDoc = CPegDoc::GetDoc();
	pDoc->SetOptionsPage(CPegDoc::TAB_OPEN_ISOMETRIC);
	int lResult = ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_PROJ_MAIN), app.GetSafeHwnd(), DlgProcProjection);

//    int lResult = ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_PROJ_ISOMETRIC), GetSafeHwnd(), DlgProcProjIsometric);
	InvalidateRect(NULL, TRUE);

	if(lResult == IDOK)
		SetProjectionMenuText(ID_3DVIEWS_ISOMETRIC);
}

void CPegView::On3dViewsLeft()
{
	view::SetEye(- XDIR);
	view::SetDirection(- XDIR);
	view::SetVwUp(ZDIR);
	view::SetPerspectiveEnabled(false);
	InvalidateRect(NULL, TRUE);

	SetProjectionMenuText(ID_3DVIEWS_LEFT);
}

void CPegView::On3dViewsOblique()
{
	int lResult = ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_PROJ_OBLIQUE), GetSafeHwnd(), DlgProcProjOblique);
	InvalidateRect(NULL, TRUE);

	if(lResult == IDOK)
		SetProjectionMenuText(ID_3DVIEWS_OBLIQUE);
}

void CPegView::On3dViewsPerspective()
{
	int lResult = ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_PROJ_PERSPECTIVE), GetSafeHwnd(), DlgProcProjPerspective);
	InvalidateRect(NULL, TRUE);

	if(lResult == IDOK)
		SetProjectionMenuText(ID_3DVIEWS_PERSPECTIVE);
}

void CPegView::On3dViewsRight()
{
	view::SetEye(XDIR);
	view::SetDirection(XDIR);
	view::SetVwUp(ZDIR);
	view::SetPerspectiveEnabled(false);
	InvalidateRect(NULL, TRUE);

	SetProjectionMenuText(ID_3DVIEWS_RIGHT);

}

void CPegView::On3dViewsTop()
{
	view::SetEye(ZDIR);
	view::SetDirection(ZDIR);
	view::SetVwUp(YDIR);
	view::SetPerspectiveEnabled(false);
	InvalidateRect(NULL, TRUE);

	SetProjectionMenuText(ID_3DVIEWS_TOP);
}

void CPegView::SetProjectionMenuText(WORD wProjType)
{
	::CheckMenuRadioItem(app.GetMenu(), ID_3DVIEWS_TOP, ID_3DVIEWS_PERSPECTIVE, wProjType, MF_BYCOMMAND);

	CString strProjText = "Proj:";
	switch(wProjType)
	{
		case ID_3DVIEWS_TOP:				strProjText.Append("Top"); break;
		case ID_3DVIEWS_BOTTOM:				strProjText.Append("Bottom"); break;
		case ID_3DVIEWS_FRONT:				strProjText.Append("Front"); break;
		case ID_3DVIEWS_BACK:				strProjText.Append("Back"); break;
		case ID_3DVIEWS_LEFT:				strProjText.Append("Left"); break;
		case ID_3DVIEWS_RIGHT:				strProjText.Append("Right"); break;
		case ID_3DVIEWS_ISOMETRIC:			strProjText.Append("Iso"); break;
		case ID_3DVIEWS_AXONOMETRIC:		strProjText.Append("Axo"); break;
		case ID_3DVIEWS_OBLIQUE:			strProjText.Append("Obl"); break;
		case ID_3DVIEWS_PERSPECTIVE:		strProjText.Append("Persp"); break;
	}

	// mak - update "proj:" text
	HMENU hMenu;
	if((hMenu = app.GetMenu()) != 0)
	{
		HMENU hPopMenu = ::GetSubMenu(hMenu, 7);
		::ModifyMenu(hMenu, 7, MF_BYPOSITION | MF_STRING | MF_POPUP, UINT_PTR(hPopMenu), strProjText);
		::DrawMenuBar(app.GetSafeHwnd());
	}
}

void CPegView::OnBackgroundImageLoad()
{
	OnBackgroundImageRemove();

	static DWORD nFilterIndex = 1;

	//char szFilter[27];
	//SecureZeroMemory(szFilter, 27);
	//::LoadString(app.GetInstance(), IDS_OPENFILE_FILTER_BITMAPS, szFilter, sizeof(szFilter));

	OPENFILENAME of;
	::ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = 0;
	of.hInstance = app.GetInstance();
	//of.lpstrFilter = szFilter;
	of.lpstrFilter = "Bitmap Files (.bmp)\0*.bmp\0";
	of.nFilterIndex = nFilterIndex;
	of.lpstrFile = new char[MAX_PATH];
	of.lpstrFile[0] = 0;
	of.nMaxFile = MAX_PATH;
	of.lpstrTitle = "Load Background Image";
	of.Flags = OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	of.lpstrDefExt = "bmp";
	of.lpfnHook = OFNHookProcBitmapLoad;
	of.lpTemplateName = MAKEINTRESOURCE(IDD_BITMAP_OPEN);
		
	if (GetOpenFileName(&of))
	{
		CFileBitmap fbmp(of.lpstrFile);
		
		fbmp.Load(of.lpstrFile, m_bmBackgroundImage, m_palBackgroundImage);
		m_bViewBackgroundImage = true;
		app.CheckMenuItem(ID_VIEW_BACKGROUNDIMAGE, (m_bViewBackgroundImage ? MF_CHECKED : MF_UNCHECKED));
		InvalidateRect(NULL, TRUE);

		m_BackgroundBitmap = new CPrimBitmap(CPnt(0, 0, 0), (HBITMAP)m_bmBackgroundImage);

		// set Upper Left Point
		//CPnt pntUL = app.CursorPosGet();
		//view::Initialize();
		//double nX, nY;
		//vActive.GetCenter(nX, nY);
		//::GetWindowRect(
		CPnt pntUL(10, 10, 0);
		backgroundImage.SetPoint(pntUL);

		app.SetPegState(STATE_BACKGROUND);
		ShowWindow(backgroundImage.GetHWND(), SW_SHOW);
	}

	delete [] of.lpstrFile;
}

void CPegView::OnBackgroundImageRemove()
{
	if ((HBITMAP)m_bmBackgroundImage != NULL)
	{
		m_bmBackgroundImage.DeleteObject();
		m_palBackgroundImage.DeleteObject();
        m_bViewBackgroundImage = false;
		app.CheckMenuItem(ID_VIEW_BACKGROUNDIMAGE, (m_bViewBackgroundImage ? MF_CHECKED : MF_UNCHECKED));
		InvalidateRect(NULL, TRUE);

		app.SetPegState(STATE_NORMAL);
		ShowWindow(backgroundImage.GetHWND(), SW_HIDE);
	}
}

void CPegView::OnCameraRotateLeft()
{
    DoCameraRotate(ID_CAMERA_ROTATELEFT);
}

void CPegView::OnCameraRotateRight()
{
	DoCameraRotate(ID_CAMERA_ROTATERIGHT);
}

void CPegView::OnCameraRotateUp()
{
	DoCameraRotate(ID_CAMERA_ROTATEUP);
}

void CPegView::OnCameraRotateDown()
{
	DoCameraRotate(ID_CAMERA_ROTATEDOWN);
}

void CPegView::OnCameraZRotateLeft()
{
	DoCameraRotate(ID_CAMERA_ZROT_LEFT);
}

void CPegView::OnCameraZRotateRight()
{
	DoCameraRotate(ID_CAMERA_ZROT_RIGHT);
}

void CPegView::OnLButtonDown(HWND, BOOL, int, int, UINT)
{
	DoCustomMouseClick(mouse::szLeftMouseDown);
}

void CPegView::OnLButtonUp(HWND, int, int, UINT)
{
	DoCustomMouseClick(mouse::szLeftMouseUp);
} 

bool IsAltKeyDown()
{
	//int nVirtKey  = ::GetKeyState(VK_MENU);
	int nVirtKey  = ::GetKeyState(VK_CONTROL);
	return (bool)(nVirtKey & 0x8000);
}

void CPegView::OnMouseMove(HWND hwnd, int x, int y, UINT)
{
	CDC* pDC = GetDC();

	//if(IsAltKeyDown())
	//{
	//	HCURSOR hCurs = ::LoadCursor(0, IDC_HAND);
	//	::SetCursor(hCurs);

	//	DoWindowPan(app.GetSafeHwnd(), viewport::GetWidthInInches() / view::GetUExt());
	//	//this->InvalidateRect(0, TRUE);

	//	return;
	//}
	
	app.SetModeCursor(app.m_iModeId);
		
	DisplayOdometer();

	// mak - updates pen colors immediately
	pstate.SetPen(pDC, pstate.PenColor(), pstate.PenStyle());
			
	if(app.RubberBandingGetType() == Lines) 
	{
		CPoint pnt[2];
		int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);

		pnt[0] = app.m_pntGinStart;
		pnt[1] = app.m_pntGinEnd;
		pDC->Polyline(pnt, 2);
				
		app.m_pntGinEnd.x = x;
		app.m_pntGinEnd.y = y;
		pnt[1] = app.m_pntGinEnd;
		pDC->Polyline(pnt, 2);
				
		pstate.SetROP2(pDC, iDrawMode);
	}
	else if(app.RubberBandingGetType() == Quads)
	{
		CPoint pnt[4];
				
		int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
				
		pnt[0] = app.m_pntGinStart;
		pnt[1].x = app.m_pntGinStart.x + (app.m_pntGinCur.x - app.m_pntGinEnd.x);
		pnt[1].y = app.m_pntGinStart.y + (app.m_pntGinCur.y - app.m_pntGinEnd.y);
		pnt[2] = app.m_pntGinCur;
		pnt[3] = app.m_pntGinEnd;
		
		if(pnt[1].x != app.m_pntGinStart.x || pnt[1].y != app.m_pntGinStart.y)
			pDC->Polyline(pnt, 4);
				
		app.m_pntGinCur.x = x;
		app.m_pntGinCur.y = y;
		pnt[1].x = app.m_pntGinStart.x + (app.m_pntGinCur.x - app.m_pntGinEnd.x);
		pnt[1].y = app.m_pntGinStart.y + (app.m_pntGinCur.y - app.m_pntGinEnd.y);
		pnt[2] = app.m_pntGinCur;
		pDC->Polyline(pnt, 4);
				
		pstate.SetROP2(pDC, iDrawMode);
	}
	else if(app.RubberBandingGetType() == Circles) 
	{
		// circles - radius
		if(draw::eCircleType == draw::CIRCLE_CENTER_RADIUS)
		{
			int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
			CBrush* pBrushOld = (CBrush*) pDC->SelectStockObject(NULL_BRUSH);
			
			int iRadiusX = abs(app.m_pntGinEnd.x - app.m_pntGinStart.x);
			int iRadiusY = abs(app.m_pntGinEnd.y - app.m_pntGinStart.y);
			int iRadius = (int) sqrt((double) (iRadiusX * iRadiusX + iRadiusY * iRadiusY) + .5);

			pDC->Ellipse(app.m_pntGinStart.x - iRadius, app.m_pntGinStart.y + iRadius,
				app.m_pntGinStart.x + iRadius, app.m_pntGinStart.y - iRadius);
			app.m_pntGinEnd.x = x;
			app.m_pntGinEnd.y = y;
			iRadiusX = abs(app.m_pntGinEnd.x - app.m_pntGinStart.x);
			iRadiusY = abs(app.m_pntGinEnd.y - app.m_pntGinStart.y);
			iRadius = (int) sqrt((double) (iRadiusX * iRadiusX + iRadiusY * iRadiusY) + .5);
			pDC->Ellipse(app.m_pntGinStart.x - iRadius, app.m_pntGinStart.y + iRadius,
				app.m_pntGinStart.x + iRadius, app.m_pntGinStart.y - iRadius);

			pDC->SelectObject(pBrushOld);
			pstate.SetROP2(pDC, iDrawMode);
		}
		// circles - diameter
		else //if(draw::eCircleType == draw::CIRCLE_DIAMETER)
		{
			int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
			CBrush* pBrushOld = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);

			CPnt pntStart(app.m_pntGinStart.x, app.m_pntGinStart.y, 0);
			CPnt pntEnd(app.m_pntGinEnd.x, app.m_pntGinEnd.y, 0);
			CPrimLine tempLine(pntStart, pntEnd);
			CPnt pntCenter = tempLine.GetCtrlPt();

			int iRadiusX = (int)(abs(app.m_pntGinEnd.x - app.m_pntGinStart.x));
			int iRadiusY = (int)(abs(app.m_pntGinEnd.y - app.m_pntGinStart.y));
			int iRadius = (int) sqrt((double) (iRadiusX * iRadiusX + iRadiusY * iRadiusY));
			iRadius = iRadius / 2;

			RECT rect;
			rect.left = (long)(pntCenter[0] - iRadius);
			rect.right = (long)(pntCenter[0] + iRadius);
			rect.top = (long)(pntCenter[1] - iRadius);
			rect.bottom = (long)(pntCenter[1] + iRadius);
			pDC->Ellipse(&rect);

			app.m_pntGinEnd.x = x;
			app.m_pntGinEnd.y = y;

			pntStart(app.m_pntGinStart.x, app.m_pntGinStart.y, 0);
			pntEnd(app.m_pntGinEnd.x, app.m_pntGinEnd.y, 0);
			CPrimLine tempLine2(pntStart, pntEnd);
			pntCenter = tempLine2.GetCtrlPt();

			int iRadiusX2 = (int)(abs(app.m_pntGinEnd.x - app.m_pntGinStart.x));
			int iRadiusY2 = (int)(abs(app.m_pntGinEnd.y - app.m_pntGinStart.y));
			int iRadius2 = (int) sqrt((double) (iRadiusX2 * iRadiusX2 + iRadiusY2 * iRadiusY2));
			iRadius2 = iRadius2 / 2;

			rect.left = (long)(pntCenter[0] - iRadius2);
			rect.right = (long)(pntCenter[0] + iRadius2);
			rect.top = (long)(pntCenter[1] - iRadius2);
			rect.bottom = (long)(pntCenter[1] + iRadius2);
			pDC->Ellipse(&rect);

			pDC->SelectObject(pBrushOld);
			pstate.SetROP2(pDC, iDrawMode);
		}
	}
	else if (app.RubberBandingGetType() == Rectangles) 
	{
		int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
		CBrush* pBrushOld = (CBrush*) pDC->SelectStockObject(NULL_BRUSH);
		pDC->Rectangle(app.m_pntGinStart.x, app.m_pntGinStart.y, app.m_pntGinEnd.x, app.m_pntGinEnd.y);

		app.m_pntGinEnd.x = x;
		app.m_pntGinEnd.y = y;
		pDC->Rectangle(app.m_pntGinStart.x, app.m_pntGinStart.y, app.m_pntGinEnd.x, app.m_pntGinEnd.y);
		pDC->SelectObject(pBrushOld);
		pstate.SetROP2(pDC, iDrawMode);
	}

	CSeg* pSeg;
	CPrim* pPrim;

	if (AppGetGinCur(pSeg, pPrim))
	{
		CTMat tm;
		app.SetEditSegEnd(app.CursorPosGet());
		tm.Translate(CVec(app.GetEditSegBeg(), app.GetEditSegEnd()));
		
		int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
		int iPrimState = pstate.Save();
				
		if (trapsegs.Identify() && trapsegs.Find(pSeg) != 0)
			CPrim::SpecPenColor() = trapsegs.PenColor();
				
		if (pPrim != 0)
		{
			pPrim->Display(pDC, false);
			pPrim->Transform(tm);
			pPrim->Display(pDC, false);
		}
		else if (pSeg != 0)
		{
			pSeg->Display(pDC, false);
			pSeg->Transform(tm);
			pSeg->Display(pDC, false); 				
		}
		CPrim::SpecPenColor() = 0;
	
		tmEditSeg *= tm;
				
		pstate.Restore(pDC, iPrimState);
		pstate.SetROP2(pDC, iDrawMode);
			
		app.SetEditSegBeg(app.GetEditSegEnd());
	}

//	::RedrawWindow(hWnd, 0, 0, RDW_UPDATENOW);
}

void CPegView::OnPrimPerpJump()
{
	CPnt pt = app.CursorPosGet();

	if (detsegs.SelSegAndPrimUsingPoint(pt) != 0)
	{
		if (detsegs.DetPrim()->Is(CPrim::PRIM_LINE))
		{
			CPrimLine* pPrimLine = static_cast<CPrimLine*>(detsegs.DetPrim());
			pt = pPrimLine->ProjPt(app.m_ptCursorPosWorld);
			app.CursorPosSet(pt);
		}
	}
}

void CPegView::OnPrimSnapTo()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptDet;
	
	if(detsegs.IsSegEngaged())
	{
		CPrim* pPrim = detsegs.DetPrim();
		
		CPnt4 ptView(pt, 1.);
		ModelView_DoTransform(ptView);
		
		double dPicApertSiz = detsegs.PicApertSiz();
	
		CPrimPolygon::EdgeToEvaluate() = CPrimPolygon::Edge();
		CPrimPolyline::EdgeToEvaluate() = CPrimPolyline::Edge();

		if(pPrim->SelUsingPoint(ptView, dPicApertSiz, ptDet))
		{
			ptDet = pPrim->GoToNxtCtrlPt();
			detsegs.DetPt() = ptDet;
			
			pPrim->DisRep(ptDet);
			app.CursorPosSet(ptDet);
			return;
		}
	}
	if(detsegs.SelSegAndPrimUsingPoint(pt) != 0)
	{
		ptDet = detsegs.DetPt();
		detsegs.DetPrim()->DisRep(ptDet);
		app.CursorPosSet(ptDet);
	}
}

void CPegView::OnRButtonDown(HWND, BOOL, int, int, UINT)
{		   
	DoCustomMouseClick(mouse::szRightMouseDown);
}

void CPegView::OnRButtonUp(HWND, int, int, UINT)
{		 
	DoCustomMouseClick(mouse::szRightMouseUp);
}

void CPegView::OnRelativeMovesDown()
{
	CPnt pt = app.CursorPosGet();
	pt[1] -= app.GetDimLen();
	app.CursorPosSet(pt);
}

void CPegView::OnRelativeMovesDownRotate()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[1] -= app.GetDimLen();
    pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetDimAngZ() * RADIAN);
	app.CursorPosSet(pt);
}

void CPegView::OnRelativeMovesEngDown()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[1] -= app.GetEngLen();
    pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetEngAngZ());
	app.CursorPosSet(pt);
}

void CPegView::OnRelativeMovesEngDownRotate()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[1] -= app.GetEngLen();
	pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetEngAngZ() + app.GetDimAngZ() * RADIAN);
	app.CursorPosSet(pt);
}

void CPegView::OnRelativeMovesEngIn()
{
	CPnt pt = app.CursorPosGet();
	pt[2] -= app.GetEngLen();
	app.CursorPosSet(pt);
}

void CPegView::OnRelativeMovesEngLeft()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[0] -= app.GetEngLen();
    pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetEngAngZ());
	app.CursorPosSet(pt);
}

void CPegView::OnRelativeMovesEngLeftRotate()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[0] -= app.GetEngLen();
	pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetEngAngZ() + app.GetDimAngZ() * RADIAN);
	app.CursorPosSet(pt);
}

void CPegView::OnRelativeMovesEngOut()
{
	CPnt pt = app.CursorPosGet();
	pt[2] += app.GetEngLen();
	app.CursorPosSet(pt);
}

void CPegView::OnRelativeMovesEngRight()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[0] += app.GetEngLen();
    pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetEngAngZ());
	app.CursorPosSet(pt);
}

void CPegView::OnRelativeMovesEngRightRotate()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[0] += app.GetEngLen();
    pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetEngAngZ() + app.GetDimAngZ() * RADIAN);
	app.CursorPosSet(pt);
}

void CPegView::OnRelativeMovesEngUp()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[1] += app.GetEngLen();
    pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetEngAngZ());
	app.CursorPosSet(pt);
}

void CPegView::OnRelativeMovesEngUpRotate()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[1] += app.GetEngLen();
    pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetEngAngZ() + app.GetDimAngZ() * RADIAN);
	app.CursorPosSet(pt);
}

void CPegView::OnRelativeMovesIn()
{
	CPnt pt = app.CursorPosGet();
	pt[2] -= app.GetDimLen();
	app.CursorPosSet(pt);
}

void CPegView::OnRelativeMovesLeft()
{
	CPnt pt = app.CursorPosGet();
	pt[0] -= app.GetDimLen();
	app.CursorPosSet(pt);
}

void CPegView::OnRelativeMovesLeftRotate()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[0] -= app.GetDimLen();
    pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetDimAngZ() * RADIAN);
	app.CursorPosSet(pt);
}

void CPegView::OnRelativeMovesOut()
{
	CPnt pt = app.CursorPosGet();
	pt[2] += app.GetDimLen();
	app.CursorPosSet(pt);
}

void CPegView::OnRelativeMovesRight()
{
	CPnt pt = app.CursorPosGet();
	pt[0] += app.GetDimLen();
	app.CursorPosSet(pt);
}

void CPegView::OnRelativeMovesRightRotate()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[0] += app.GetDimLen();
    pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetDimAngZ() * RADIAN);
	app.CursorPosSet(pt);
}

void CPegView::OnRelativeMovesUp()
{
	CPnt pt = app.CursorPosGet();
	pt[1] += app.GetDimLen();
	app.CursorPosSet(pt);
}

void CPegView::OnRelativeMovesUpRotate()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[1] += app.GetDimLen();
    pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetDimAngZ() * RADIAN);
	app.CursorPosSet(pt);
}

void CPegView::OnSetFocus(HWND, HWND)
{
    app.SetWindowMode(app.m_iModeId);
}

void CPegView::OnSetupDimAngle()
{
	CPegDoc *pDoc = CPegDoc::GetDoc();
	pDoc->SetOptionsFocus(IDC_ANG);
	pDoc->SetOptionsPage(CPegDoc::TAB_OPEN_DIMENSION);
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_OPTIONS_TABS_MAIN), GetSafeHwnd(), DlgProcOptions);
	app.StatusLineDisplay(DimAng);
}

void CPegView::OnSetupDimLength()
{
	CPegDoc *pDoc = CPegDoc::GetDoc();
	pDoc->SetOptionsFocus(IDC_DISTANCE);
	pDoc->SetOptionsPage(CPegDoc::TAB_OPEN_DIMENSION);
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_OPTIONS_TABS_MAIN), GetSafeHwnd(), DlgProcOptions);
}

void CPegView::OnSize(HWND, UINT, int cx, int cy)
{
//	if(cx < 200)
//		SetWindowPos(app.GetSafeHwnd(), NULL, 0, 0, 200, cy, SWP_NOMOVE);
//	if(cy < 200)
//		SetWindowPos(app.GetSafeHwnd(), NULL, 0, 0, cx, 200, SWP_NOMOVE);

	viewport::Set(cx, cy);
	view::SizeUpdate();

	ClipInit();

	SendMessage(modeframe.GetHWND(), WM_SIZE, 0, MAKELPARAM(cx, cy));
	SendMessage(odomframe.GetHWND(), WM_SIZE, 0, MAKELPARAM(cx, cy));
//	SendMessage(BackgroundLayer.GetHWND(), WM_SIZE, 0, MAKELPARAM(cx, cy));

	SendMessage(stateframe.GetHWND(), WM_SIZE, 0, MAKELPARAM(cx, cy));
	SendMessage(plotframe.GetHWND(), WM_SIZE, 0, MAKELPARAM(cx, cy));

	SendMessage(soloStateFrame.GetHWND(), WM_SIZE, 0, MAKELPARAM(cx, cy));

	SendMessage(bitmapEdit.GetHWND(), WM_SIZE, 0, MAKELPARAM(cx, cy));
	SendMessage(backgroundImage.GetHWND(), WM_SIZE, 0, MAKELPARAM(cx, cy));
	
	BringWindowToTop(app.GetSafeHwnd());
}

void CPegView::DoScroll(int dir)
{
	CPnt pt = view::GetTarget();
	double dRatio = viewport::GetWidthInInches() / view::GetUExt();
	double d = 1. / dRatio;
	d /= 4;
	
	// top
	if((view::GetDirection() == ZDIR) && (view::GetVwUp() == YDIR))
	{
		switch(dir)
		{
			case ID_WINDOW_PAN_LEFT:	pt[0] -= d;	break;
			case ID_WINDOW_PAN_RIGHT:	pt[0] += d;	break;
			case ID_WINDOW_PAN_UP: 		pt[1] += d;	break;
			case ID_WINDOW_PAN_DOWN: 	pt[1] -= d;	break;
		}
	}
	// bottom
	else if((view::GetDirection() == -ZDIR) && (view::GetVwUp() == YDIR))
	{
		switch(dir)
		{
			case ID_WINDOW_PAN_LEFT:	pt[0] += d;	break;
			case ID_WINDOW_PAN_RIGHT:	pt[0] -= d;	break;
			case ID_WINDOW_PAN_UP: 		pt[1] += d;	break;
			case ID_WINDOW_PAN_DOWN: 	pt[1] -= d;	break;
		}
	}
	// left
	else if((view::GetDirection() == -XDIR) && (view::GetVwUp() == ZDIR))
	{
		switch(dir)
		{
			case ID_WINDOW_PAN_LEFT:	pt[1] += d;	break;
			case ID_WINDOW_PAN_RIGHT:	pt[1] -= d;	break;
			case ID_WINDOW_PAN_UP: 		pt[2] += d;	break;
			case ID_WINDOW_PAN_DOWN: 	pt[2] -= d;	break;
		}
	}
	// right
	else if((view::GetDirection() == XDIR) && (view::GetVwUp() == ZDIR))
	{
		switch(dir)
		{
			case ID_WINDOW_PAN_LEFT:	pt[1] -= d;	break;
			case ID_WINDOW_PAN_RIGHT:	pt[1] += d;	break;
			case ID_WINDOW_PAN_UP: 		pt[2] += d;	break;
	 		case ID_WINDOW_PAN_DOWN: 	pt[2] -= d;	break;
		}
	}
	// front
	else if((view::GetDirection() == -YDIR) && (view::GetVwUp() == ZDIR))
	{
		switch(dir)
		{
			case ID_WINDOW_PAN_LEFT:	pt[0] -= d;	break;
			case ID_WINDOW_PAN_RIGHT:	pt[0] += d;	break;
			case ID_WINDOW_PAN_UP: 		pt[2] += d;	break;
			case ID_WINDOW_PAN_DOWN: 	pt[2] -= d;	break;
		}
	}
	// back
	else if((view::GetDirection() == YDIR) && (view::GetVwUp() == ZDIR))
	{
		switch(dir)
		{
			case ID_WINDOW_PAN_LEFT:	pt[0] += d;	break;
			case ID_WINDOW_PAN_RIGHT:	pt[0] -= d;	break;
			case ID_WINDOW_PAN_UP: 		pt[2] += d;	break;
			case ID_WINDOW_PAN_DOWN: 	pt[2] -= d;	break;
		}
	}
	else // top view
	{
		switch(dir)
		{
			case ID_WINDOW_PAN_LEFT:	pt[0] -= d;	break;
			case ID_WINDOW_PAN_RIGHT:	pt[0] += d;	break;
			case ID_WINDOW_PAN_UP: 		pt[1] += d;	break;
			case ID_WINDOW_PAN_DOWN: 	pt[1] -= d;	break;
		}
	}

	view::SetTarget(pt);
	view::SetEye(view::GetDirection());
	
	InvalidateRect(NULL, TRUE);
}

void CPegView::OnVScroll(HWND hwnd, HWND, UINT code, int)
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	GetScrollInfo(hwnd, SB_VERT, &si);

	int npages;
	DWORD direction = 0;

	switch (code) 
	{
		case SB_LINEUP:
		case SB_PAGEUP:
//			for(int i = 0; i < scroll::nVScrollSens; i++)
				DoScroll(ID_WINDOW_PAN_UP);
			break;
		case SB_LINEDOWN:
		case SB_PAGEDOWN:
//			for(int i = 0; i < scroll::nVScrollSens; i++)
				DoScroll(ID_WINDOW_PAN_DOWN);
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			si.fMask = SIF_TRACKPOS;
			GetScrollInfo(hwnd, SB_VERT, &si);

			int newScroll = si.nTrackPos;

			if(newScroll == (int)((scroll::nVScrollMax - scroll::nVScrollThumbSize) / 2))
				scroll::tempPos = ((scroll::nVScrollMax - scroll::nVScrollThumbSize) / 2);
			
			npages = (int)(newScroll - scroll::tempPos);
			scroll::tempPos = newScroll;

			if(npages > 0)
				direction = ID_WINDOW_PAN_DOWN;
			else if(npages < 0)
				direction = ID_WINDOW_PAN_UP;

			npages = abs(npages);

			for(int i = 0; i < npages; i++)
//				for(int j = 0; j < scroll::nVScrollSens; j++)
					DoScroll(direction);
			break;
    }

	si.fMask = SIF_DISABLENOSCROLL | SIF_POS | SIF_PAGE;
	si.nPos = ((scroll::nVScrollMax - scroll::nVScrollThumbSize) / 2);
	si.nPage = scroll::nVScrollThumbSize;
    SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
}

void CPegView::OnHScroll(HWND hwnd, HWND, UINT code, int)
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	GetScrollInfo(hwnd, SB_HORZ, &si);

	int npages;
	DWORD direction = 0;

	switch (code) 
	{
		case SB_LINEUP:
		case SB_PAGEUP:
//			for(int i = 0; i < scroll::nHScrollSens; i++)
				DoScroll(ID_WINDOW_PAN_LEFT);
			break;
		case SB_LINEDOWN:
		case SB_PAGEDOWN:
//			for(int i = 0; i < scroll::nHScrollSens; i++)
				DoScroll(ID_WINDOW_PAN_RIGHT);
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			si.fMask = SIF_TRACKPOS;
			GetScrollInfo(hwnd, SB_HORZ, &si);

			int newScroll = si.nTrackPos;
			if(newScroll == (int)((scroll::nHScrollMax - scroll::nHScrollThumbSize) / 2))
				scroll::tempPos = ((scroll::nHScrollMax - scroll::nHScrollThumbSize) / 2);
			
			npages = (int)(newScroll - scroll::tempPos);
			scroll::tempPos = newScroll;

			if(npages > 0)
				direction = ID_WINDOW_PAN_RIGHT;
			else if(npages < 0)
				direction = ID_WINDOW_PAN_LEFT;

			npages = abs(npages);

			for(int i = 0; i < npages; i++)
//				for(int j = 0; j < scroll::nHScrollSens; j++)
					DoScroll(direction);
			break;
    }

	si.fMask = SIF_DISABLENOSCROLL | SIF_POS | SIF_PAGE;
	si.nPos = ((scroll::nHScrollMax - scroll::nHScrollThumbSize) / 2);
	si.nPage = scroll::nHScrollThumbSize;
    SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
}

void CPegView::UpdateScroll()
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);

	// vertical
	si.fMask = SIF_DISABLENOSCROLL | SIF_POS | SIF_PAGE;
	si.nPos = ((scroll::nVScrollMax - scroll::nVScrollThumbSize) / 2);
	si.nPage = scroll::nVScrollThumbSize;
	SetScrollInfo(app.GetSafeHwnd(), SB_VERT, &si, TRUE);

	// horizontal
	si.fMask = SIF_DISABLENOSCROLL | SIF_POS | SIF_PAGE;
	si.nPos = ((scroll::nHScrollMax - scroll::nHScrollThumbSize) / 2);
	si.nPage = scroll::nHScrollThumbSize;
	SetScrollInfo(app.GetSafeHwnd(), SB_HORZ, &si, TRUE);
}

void CPegView::OnScrollBars()
{
	ScrollbarEnable(!m_bViewScrollBars);
}

void CPegView::ScrollbarEnable(bool bValue)
{
	m_bViewScrollBars = bValue;
	app.CheckMenuItem(ID_VIEW_SCROLLBARS, (m_bViewScrollBars ? MF_CHECKED : MF_UNCHECKED));
	ShowScrollBar(app.GetSafeHwnd(), SB_HORZ, m_bViewScrollBars);
	ShowScrollBar(app.GetSafeHwnd(), SB_VERT, m_bViewScrollBars);
	InvalidateRect(NULL, TRUE);
}

void CPegView::OnMouseWheel(HWND hwnd, int, int, int zDelta, UINT)
{
	// 120 = zoom in
	if(zDelta > 0)
		OnWindowZoomIn(hwnd);
	else
		OnWindowZoomOut(hwnd);
}

void CPegView::ClipInit()
{
	// Retrieve the application's client rectangle
	RECT rctTmp; 
	GetClientRect(&rctTmp); 

	clip::OldHrgn = ::CreateRectRgn(rctTmp.left, rctTmp.top, rctTmp.right, rctTmp.bottom);
	clip::OldRect = rctTmp;	
}

void CPegView::ClipStartAll()
{
	HDC hdc = ::GetDC(app.GetSafeHwnd());
	::SelectClipRgn(hdc, clip::OldHrgn);
	ReleaseDC(app.GetSafeHwnd(), hdc);
}

void CPegView::ClipEndAll()
{
	HDC hdc = ::GetDC(app.GetSafeHwnd());
	::SelectClipRgn(hdc, clip::Hrgn);
	ReleaseDC(app.GetSafeHwnd(), hdc);
}

void CPegView::OnUpdate(CPegView*, LPARAM lHint, CObject* pHint)
{
	CDC* pDC = GetDC();

	int iPrimState = 0;
	int iDrawMode = 0;

	if ((lHint & CPegDoc::HINT_SAFE) == CPegDoc::HINT_SAFE)
	{
		iPrimState = pstate.Save();
	}
	if ((lHint & CPegDoc::HINT_ERASE) == CPegDoc::HINT_ERASE)
	{
		iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
	}
	if ((lHint & CPegDoc::HINT_TRAP) == CPegDoc::HINT_TRAP)
	{
		CPrim::SpecPenColor() = trapsegs.PenColor();
	}
	switch (lHint)
	{
		case CPegDoc::HINT_PRIM:
		case CPegDoc::HINT_PRIM_SAFE:
		case CPegDoc::HINT_PRIM_ERASE_SAFE:
			((CPrim*) pHint)->Display(pDC, false);
			break;

		case CPegDoc::HINT_SEG:
		case CPegDoc::HINT_SEG_SAFE:
		case CPegDoc::HINT_SEG_ERASE_SAFE:
		case CPegDoc::HINT_SEG_SAFE_TRAP:
		case CPegDoc::HINT_SEG_ERASE_SAFE_TRAP:
			((CSeg*) pHint)->Display(pDC, false);
			break;

		case CPegDoc::HINT_SEGS:
		case CPegDoc::HINT_SEGS_SAFE:
		case CPegDoc::HINT_SEGS_SAFE_TRAP:
		case CPegDoc::HINT_SEGS_ERASE_SAFE_TRAP:
			((CSegs*) pHint)->Display(pDC);
			break;

		case CPegDoc::HINT_LAYER:
		case CPegDoc::HINT_LAYER_SAFE:
		case CPegDoc::HINT_LAYER_ERASE:
			((CLayer*) pHint)->Display(pDC, false);
			break;

		default:
			::InvalidateRect(m_hWnd, 0L, TRUE);
	}
	if ((lHint & CPegDoc::HINT_TRAP) == CPegDoc::HINT_TRAP)
	{
		CPrim::SpecPenColor() = 0;
	}
	if ((lHint & CPegDoc::HINT_ERASE) == CPegDoc::HINT_ERASE)
	{
		pstate.SetROP2(pDC, iDrawMode);
	}
	if ((lHint & CPegDoc::HINT_SAFE) == CPegDoc::HINT_SAFE)
	{
		pstate.Restore(pDC, iPrimState);
	}
}

void CPegView::OnViewBackgroundImage()
{
	if((HBITMAP)m_bmBackgroundImage != NULL)
	{
		m_bViewBackgroundImage = !m_bViewBackgroundImage;
		app.CheckMenuItem(ID_VIEW_BACKGROUNDIMAGE, (m_bViewBackgroundImage ? MF_CHECKED : MF_UNCHECKED));
		InvalidateRect(NULL, TRUE);

		//if(m_bViewBackgroundImage)
		//	ShowWindow(backgroundImage.GetHWND(), SW_SHOW);
		//else
		//	ShowWindow(backgroundImage.GetHWND(), SW_HIDE);
	}

	/////////////////////////////////////////////////////////////

//	CPegDoc *pDoc = CPegDoc::GetDoc();
////	CLayer *pLayer = pDoc->LayersGet("Bitmap");
//
//	CLayer *pLayer = pDoc->BitmapLayerGet();
//
//	if(pLayer == 0)
//		return;
//
//	if(pLayer->IsOff())
//		pLayer->SetStateCold();
//	else
//		pLayer->SetStateOff();
//
//	// need to refresh the display
//	InvalidateRect(NULL, TRUE);
}

void CPegView::ShowBackgroundImage(bool bValue)
{
	if((HBITMAP)m_bmBackgroundImage != NULL)
	{
		//m_bViewBackgroundImage = !m_bViewBackgroundImage;
		m_bViewBackgroundImage = bValue;
		app.CheckMenuItem(ID_VIEW_BACKGROUNDIMAGE, (m_bViewBackgroundImage ? MF_CHECKED : MF_UNCHECKED));
		InvalidateRect(NULL, TRUE);
	}
}

//void CPegView::OnViewBackgroundImage()
//{
//	//if((HBITMAP)m_bmBackgroundImage != NULL)
//	//{
//	//	m_bViewBackgroundImage = !m_bViewBackgroundImage;
//	//	app.CheckMenuItem(ID_VIEW_BACKGROUNDIMAGE, (m_bViewBackgroundImage ? MF_CHECKED : MF_UNCHECKED));
//	//	InvalidateRect(NULL, TRUE);
//	//}
//
//	/////////////////////////////////////////////////////////////
//
//	CPegDoc *pDoc = CPegDoc::GetDoc();
////	CLayer *pLayer = pDoc->LayersGet("Bitmap");
//
//	CLayer *pLayer = pDoc->BitmapLayerGet();
//
//	if(pLayer == 0)
//		return;
//
//	if(pLayer->IsOff())
//		pLayer->SetStateCold();
//	else
//		pLayer->SetStateOff();
//
//	// need to refresh the display
//	InvalidateRect(NULL, TRUE);
//}

void CPegView::OnViewLighting()
{
//    ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_VIEW_LIGHTING), app.GetSafeHwnd(), DlgProcViewLighting);
}

void CPegView::OnViewOdometer()
{
	OdometerEnable(!m_bViewOdometer);
}

void CPegView::OdometerEnable(bool bValue)
{
	m_bViewOdometer = bValue;
	app.CheckMenuItem(ID_VIEW_ODOMETER, (m_bViewOdometer ? MF_CHECKED : MF_UNCHECKED));
	::ShowWindow(odomframe.GetHWND(), (m_bViewOdometer ? SW_SHOW : SW_HIDE));
	BringWindowToTop(app.GetSafeHwnd());
	CPegDoc::GetDoc()->UpdateAllViews(NULL, 0L, NULL);
}

void CPegView::ColorByLayerEnable(bool bValue)
{
	m_bViewColorByLayer = bValue;
	app.CheckMenuItem(ID_VIEW_LAYERSBYCOLOR, (m_bViewColorByLayer ? MF_CHECKED : MF_UNCHECKED));
	CPegDoc::GetDoc()->UpdateAllViews(NULL, 0L, NULL);
}

void CPegView::OnViewColorByLayer()
{
	ColorByLayerEnable(!m_bViewColorByLayer);
	app.StatusLineDisplay(WorkColor);
}

void CPegView::OnViewPenWidths()
{
	PenWidthsEnable(!m_bViewPenWidths);
}

void CPegView::PenWidthsEnable(bool bValue)
{
	m_bViewPenWidths = bValue;
	app.CheckMenuItem(ID_VIEW_PENWIDTHS, m_bViewPenWidths ? MF_CHECKED : MF_UNCHECKED);
	InvalidateRect(NULL, TRUE);
}

void CPegView::PenWidthsSetFile(CString strFilename)
{
	m_strPenWidthsFile = strFilename;
}

CString	CPegView::PenWdithsGetFile()
{
	return m_strPenWidthsFile;
}

void CPegView::OnViewPrintPreview()
{
	static int wPrevState = STATE_NORMAL;
	if(app.GetPegState() != STATE_PLOT)
		wPrevState = app.GetPegState();

	switch(app.GetPegState())
	{
		case STATE_BACKGROUND:
			UnloadStateNormal();
			app.SetPegState(STATE_PLOT);
			ShowWindow(backgroundImage.GetHWND(), SW_HIDE);
			break;
		case STATE_NORMAL:
			UnloadStateNormal();
			app.SetPegState(STATE_PLOT);
			break;
		case STATE_PLOT:
			UnloadStatePlot();
			app.SetPegState((EPegState)wPrevState);
			break;
	}

//	switch(app.GetPegState())
//	{
//		case STATE_NORMAL:	app.SetPegState(STATE_PLOT); break;
//		case STATE_PLOT:	app.SetPegState(STATE_NORMAL); break;
//	}

	app.CheckMenuItem(ID_VIEW_PRINTPREVIEW, m_bViewPrintPreview ? MF_CHECKED : MF_UNCHECKED);
	InvalidateRect(NULL, TRUE);

	switch(app.GetPegState())
	{
		case STATE_NORMAL:
			LoadStateNormal();
			break;
		case STATE_PLOT:
			LoadStatePlot();
			break;
		case STATE_BACKGROUND:
			app.SetPegState(STATE_BACKGROUND);
			ShowWindow(backgroundImage.GetHWND(), SW_SHOW);
			LoadStateNormal();
			break;
	}
}

void CPegView::LoadStateNormal()
{
	//ShowWindow(stateframe.GetHWND(), SW_HIDE);
	//ShowWindow(plotframe.GetHWND(), SW_HIDE);
//	ShowWindow(app.GetStatButtonHwnd(), SW_HIDE);

	app.CheckMenuItem(ID_VIEW_PRINTPREVIEW, ((app.GetPegState() == STATE_PLOT)  ? MF_CHECKED : MF_UNCHECKED));
}

void CPegView::UnloadStateNormal()
{


}

void CPegView::LoadStatePlot()
{
//	OnClippingRegion();
	DlgProcPlotPreviewEnable();
	ShowWindow(stateframe.GetHWND(), SW_SHOW);

	if(plotframe.IsVisible())
		ShowWindow(plotframe.GetHWND(), SW_SHOW);

//	ShowWindow(app.GetStatButtonHwnd(), SW_SHOW);
//	::SetWindowText(app.GetStatButtonHwnd(), "Plot Mode");

	app.CheckMenuItem(ID_VIEW_PRINTPREVIEW, ((app.GetPegState() == STATE_PLOT)  ? MF_CHECKED : MF_UNCHECKED));
}

void CPegView::UnloadStatePlot()
{
	DlgProcPlotPreviewDisable();
//	OnClippingRegion();
	ShowWindow(stateframe.GetHWND(), SW_HIDE);
	ShowWindow(plotframe.GetHWND(), SW_HIDE);
}

void CPegView::OnViewRefresh()
{
	InvalidateRect(NULL, TRUE);
}

void CPegView::OnViewRendered()
{
	m_bViewRendered = !m_bViewRendered;
	app.CheckMenuItem(ID_VIEW_RENDERED, m_bViewRendered ? MF_CHECKED : MF_UNCHECKED);
	InvalidateRect(NULL, TRUE);
}

void CPegView::OnViewTrueTypeFonts()
{
	TTFEnable(!m_bViewTrueTypeFonts);
}

void CPegView::TTFEnable(bool bValue)
{
	m_bViewTrueTypeFonts = bValue;
	UINT nCheck = m_bViewTrueTypeFonts ? MF_CHECKED : MF_UNCHECKED;
	app.CheckMenuItem(ID_VIEW_TRUETYPEFONTS, nCheck);
	InvalidateRect(NULL, TRUE);
}

void CPegView::OriginEnable(bool bValue)
{
	m_bViewOrigin = bValue;
}

void CPegView::OriginSetColor1(int nIndex)
{
	m_nOriginColor1 = nIndex;
}

void CPegView::OriginSetColor2(int nIndex)
{
	m_nOriginColor2 = nIndex;
}

int CPegView::OriginGetColor1()
{
	return m_nOriginColor1;
}

int CPegView::OriginGetColor2()
{
	return m_nOriginColor2;
}

void CPegView::OnViewWindow(HWND hwnd)
{
	CPoint pnt;
	::GetCursorPos(&pnt);

	HMENU hMenu = ::LoadMenu(app.GetInstance(), MAKEINTRESOURCE(IDR_WINDOW));
	hMenu = GetSubMenu(hMenu, 0);
	::TrackPopupMenu(hMenu, TPM_LEFTALIGN, pnt.x, pnt.y, 0, hwnd, 0);
	::DestroyMenu(hMenu);
}

void CPegView::OnViewWireframe()
{
	m_bViewWireframe = !m_bViewWireframe;
	app.CheckMenuItem(ID_VIEW_WIREFRAME, m_bViewWireframe ? MF_CHECKED : MF_UNCHECKED);

//	m_bViewRendered = !m_bViewWireframe;
//	app.CheckMenuItem(ID_VIEW_RENDERED, m_bViewRendered ? MF_CHECKED : MF_UNCHECKED);

	InvalidateRect(NULL, TRUE);
}

void CPegView::OnViewZoom()
{
	if(::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_VIEW_ZOOM), GetSafeHwnd(), DlgProcViewZoom) > 0) 
		InvalidateRect(NULL, TRUE);
}

void CPegView::OnWindowBest()
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
		InvalidateRect(NULL, TRUE);
	}
}

void CPegView::OnWindowNormal(HWND hwnd)
{
	::DoWindowPan(hwnd, 1.);
}

void CPegView::OnWindowPan(HWND hwnd)
{
	::DoWindowPan(hwnd, viewport::GetWidthInInches() / view::GetUExt());
}

void CPegView::OnWindowPanDown()
{
	DoWindowPan0(ID_WINDOW_PAN_DOWN);
}

void CPegView::OnWindowPanLeft()
{
	DoWindowPan0(ID_WINDOW_PAN_LEFT);
}

void CPegView::OnWindowPanRight()
{
	DoWindowPan0(ID_WINDOW_PAN_RIGHT);
}

void CPegView::OnWindowPanUp()
{
	DoWindowPan0(ID_WINDOW_PAN_UP);
}

void CPegView::OnWindowZoomIn(HWND hwnd)
{
	::DoWindowPan(hwnd, viewport::GetWidthInInches() / view::GetUExt() * 1.25);
}

void CPegView::OnWindowZoomOut(HWND hwnd)
{
	::DoWindowPan(hwnd, viewport::GetWidthInInches() / view::GetUExt() / 1.25);
}

//bool CPegView::Pnt4_IsInView(const CPnt4& pt)
//{
//	return ::PtInRegion(clip::Hrgn, (int)pt[0], (int)pt[1]) != 0;
////	return false;
//}
//
//bool CPegView::Pnt4_ClipLine(CPnt4& , CPnt4&)//ptA, CPnt4& ptB)
//{
//	//RECT rect;
//	//rect.bottom = (LONG)max(ptA[1], ptB[1]);
//	//rect.left = (LONG)min(ptA[0], ptB[0]);
//	//rect.right = (LONG)max(ptA[0], ptB[0]);
//	//rect.top = (LONG)min(ptA[1], ptB[1]);
//
//	//return ::RectInRegion(clip::Hrgn, &rect) != 0;
//
//	return false;
//}
//
//void CPegView::OnClipView()
//{
//	static CPnt pt[8];
//	static bool bFirst = true;
//
//	switch(m_eClipMode)
//	{
//		case MODE_NONE:
//			break;
//		case MODE_PLOT_CLIPSET:
//			if(bFirst) 
//			{
//				bFirst = false;
//				pt[0] = app.CursorPosGet();
//				app.RubberBandingStartAtEnable(pt[0], Rectangles);
//			}
//			else 
//			{
//				bFirst = true;
//				pt[1] = app.CursorPosGet();
//
//				job2state.m_rectClip.left = (LONG)min(pt[0][0], pt[1][0]);
//				job2state.m_rectClip.top = (LONG)max(pt[0][1], pt[1][1]);
//				job2state.m_rectClip.right = (LONG)max(pt[0][0], pt[1][0]);
//				job2state.m_rectClip.bottom = (LONG)min(pt[0][1], pt[1][1]);
//
//				app.RubberBandingDisable();
//				InvalidateRect(NULL, TRUE);
//
//				SetClipMode(MODE_NONE);
//				SendMessage(plotframe.GetHWND(), WM_PLOT_UPDATE, 0, 0);
//			}
//			break;
//		case MODE_PLOT_ORIGINSET:
//			job2state.m_pntOrigin = app.CursorPosGet();
//			
//			SetClipMode(MODE_NONE);
//			SendMessage(plotframe.GetHWND(), WM_PLOT_UPDATE, 0, 0);
//			break;
//	}
//}

void DoWindowPan(HWND hwnd, double dRatio)
{
	double dUExt = viewport::GetWidthInInches() / dRatio;
	double dVExt = viewport::GetHeightInInches() / dRatio;
	
	view::SetCenteredWnd(dUExt, dVExt);

	CPnt ptCur = app.CursorPosGet();

	CVec vDirection = view::GetDirection();
	CPnt ptTarget = view::GetTarget();
	
	line::IntersectionWithPln(ptCur, vDirection, ptTarget, vDirection, &ptCur);
	
	view::SetTarget(ptCur);
	view::SetEye(vDirection);

	app.CursorPosSet(ptCur);
	::InvalidateRect(hwnd, 0, TRUE);

	app.StatusLineDisplay(WndRatio);
}