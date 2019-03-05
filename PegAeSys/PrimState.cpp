#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysView.h"

// State list maintenance
CPrimState* psSav[] = {0, 0, 0, 0};

HPEN		hPen[8] = {0, 0, 0, 0, 0, 0, 0, 0};
PENSTYLE	nPenStyles[8];
int 		nPenWidths[8];
COLORREF	crColRef[8];

CPrimState::CPrimState()
{
	m_nMarkStyle = 2;	
	m_nPenColor = 1;
	m_nPenStyle = 1;	
	m_nPolygonIntStyle = POLYGON_HOLLOW;
	m_nPolygonIntStyleId = 1;
}

const CPrimState& CPrimState::operator=(const CPrimState& src)
{
	m_fd = src.m_fd;

	m_nPenColor = src.m_nPenColor;
	m_nPenStyle = src.m_nPenStyle;	
	m_nMarkStyle = src.m_nMarkStyle;	
	m_nPolygonIntStyle = src.m_nPolygonIntStyle;
	m_nPolygonIntStyleId = src.m_nPolygonIntStyleId;

	return (*this);
}

void CPrimState::Restore(CDC* pDC, int iSaveId)
{
	if (iSaveId >= sizeof(psSav) / sizeof(psSav[0]))
		return;
	
	if (psSav[iSaveId] != 0) 
	{
		SetPen(pDC, psSav[iSaveId]->PenColor(), psSav[iSaveId]->PenStyle());
		
		m_fd = psSav[iSaveId]->m_fd;

		SetTxtAlign(pDC, m_fd.TextHorAlign(), m_fd.TextVerAlign());

		SetPolygonIntStyle(psSav[iSaveId]->PolygonIntStyle());
		SetPolygonIntStyleId(psSav[iSaveId]->PolygonIntStyleId());

		// mak - added
		SetMarkStyle(psSav[iSaveId]->MarkStyle());
		
		delete psSav[iSaveId];
		psSav[iSaveId] = 0;
	}
}

int CPrimState::Save()
{
	int iSaveId = sizeof(psSav) / sizeof(psSav[0]) - 1;
	
	while (iSaveId >= 0 && psSav[iSaveId] != 0)
		iSaveId--;

	if (iSaveId < 0) 
		msgWarning(IDS_MSG_SAVE_STATE_LIST_ERROR);
	else
	{
		SetPolygonIntStyle(pstate.PolygonIntStyle());
		psSav[iSaveId] = new CPrimState;
		*psSav[iSaveId] = pstate;
	}
	// return id to use for restore reference
	return (iSaveId);
}

void CPrimState::SetBrush(CDC *, PENCOLOR)//pDC, PENCOLOR nPenColor)
{
//	HBRUSH hBrushNew = ::CreateSolidBrush(pColTbl[nPenColor]);
//	pDC->SelectObject(CBrush::FromHandle(hBrushNew));
}

void CPrimState::SetPen(CDC* pDC, PENCOLOR nPenColor, PENSTYLE nPenStyle)
{	
	m_nPenColor = nPenColor;
	m_nPenStyle = nPenStyle;
	
	if (CPrim::SpecPenColor() != 0)
		nPenColor = CPrim::SpecPenColor();

	if (nPenColor == CPrim::PENCOLOR_BYLAYER)
		nPenColor = WORD(CPrim::LayerPenColor());
	
	if (CPrim::SpecPenStyle() != 0)
		nPenStyle = CPrim::SpecPenStyle();
	
	if (nPenStyle == CPrim::PENSTYLE_BYLAYER)
		nPenStyle = CPrim::LayerPenStyle();
	
	SetPen(pDC, nPenColor, 1, nPenStyle);
}

/////<summary>Sets the pen used to for LineTo, PolyLine, PolyPolyLine, or Arc to draw the lines.</summary>
//void CPrimState::SetPen(CDC* pDC, PENCOLOR nPenColor, int nPenWidth, PENSTYLE nPenStyle)
//{	
//	CPegView* pView = CPegView::GetActiveView();
//
//	static HPEN hPenCur;
//
//	static int nPens = sizeof(hPen) / sizeof(hPen[0]);
//
//	switch (nPenStyle) 
//	{
//		case 0:
//			nPenStyle = PS_NULL;
//			break;
//		
//		case 2:
//			nPenStyle = PS_DOT;
//			break;
//		
//		case 3:
//			nPenStyle = PS_DASH;
//			break;
//		
//		case 6:
//			nPenStyle = PS_DASHDOT;
//			break;
//		
//		case 7:
//			nPenStyle = PS_DASHDOTDOT;
//			break;
//		
//		default:
//			nPenStyle = PS_SOLID;
//	}
//	double dLogWidth = 0.;
//
//	if (pView->GetViewPenWidths())
//	{
//		int iLogPixelsX = pDC->GetDeviceCaps(LOGPIXELSX);
//		dLogWidth = app.PenWidthsGet(nPenColor) * double(iLogPixelsX);
//		dLogWidth *= Min(1., viewport::GetWidthInInches() / view::GetUExt());
//	}
//	nPenWidth = Round(dLogWidth);
//
//	pDC->SetTextColor(pColTbl[nPenColor]);
//
//
//	
////	int iPen = 0;
////	for (int i = 0; i < nPens; i++)
////	{
////		if (hPen[i] != 0 && nPenStyles[i] == nPenStyle && nPenWidths[i] == nPenWidth && crColRef[i] == pColTbl[nPenColor]) 
////		{
////			//if (hPenCur != hPen[i])
////			{
////				hPenCur = hPen[i];
////				pDC->SelectObject(CPen::FromHandle(hPenCur));
////			}
////			return;
////		}
////		if (hPen[i] == 0)
////			iPen = i;		
////	}
////	
//	HPEN hPenNew = ::CreatePen(nPenStyle, nPenWidth, pColTbl[nPenColor]);
//
////	if (hPenNew != 0) 
////	{
////		hPenCur = hPenNew;
//		pDC->SelectObject(CPen::FromHandle(hPenNew));
//
////		if (hPen[iPen] != 0)
////			::DeleteObject(hPen[iPen]);
//		
////		hPen[iPen] = hPenNew;
////		nPenStyles[iPen] = nPenStyle; 
////		nPenWidths[iPen] = nPenWidth; 
////		crColRef[iPen] = pColTbl[nPenColor];
////	}
//}

void CPrimState::SetPen(CDC* pDC, PENCOLOR nPenColor, int nPenWidth, PENSTYLE nPenStyle)
{	
	CPegView* pView = CPegView::GetActiveView();

	static HPEN hPenCur;

	static int nPens = sizeof(hPen) / sizeof(hPen[0]);
//	static int nPens = 8;

	switch (nPenStyle) 
	{
		case 0:
			nPenStyle = PS_NULL;
			break;
		
		case 2:
			nPenStyle = PS_DOT;
			break;
		
		case 3:
			nPenStyle = PS_DASH;
			break;
		
		case 6:
			nPenStyle = PS_DASHDOT;
			break;
		
		case 7:
			nPenStyle = PS_DASHDOTDOT;
			break;
		
		default:
			nPenStyle = PS_SOLID;
	}
	double dLogWidth = 0.;

	if (pView->GetViewPenWidths())
	{
		int iLogPixelsX = pDC->GetDeviceCaps(LOGPIXELSX);
		dLogWidth = app.PenWidthsGet(nPenColor) * double(iLogPixelsX);
		dLogWidth *= Min(1., viewport::GetWidthInInches() / view::GetUExt());
	}
	nPenWidth = Round(dLogWidth);

	pDC->SetTextColor(pColTbl[nPenColor]);
	
	int iPen = 0;
	for (int i = 0; i < nPens; i++)
	{
		if (hPen[i] != 0 && nPenStyles[i] == nPenStyle && nPenWidths[i] == nPenWidth && crColRef[i] == pColTbl[nPenColor]) 
		{
			//if (hPenCur != hPen[i])
			{
				hPenCur = hPen[i];
				pDC->SelectObject(CPen::FromHandle(hPenCur));
			}
			return;
		}
		if (hPen[i] == 0)
			iPen = i;		
	}
	
	
	HPEN hPenNew = ::CreatePen(nPenStyle, nPenWidth, pColTbl[nPenColor]);
	pDC->SelectObject(CPen::FromHandle(hPenNew));

	if (hPenNew != 0) 
	{
		hPenCur = hPenNew;
		pDC->SelectObject(CPen::FromHandle(hPenNew));

		if (hPen[iPen] != 0)
			::DeleteObject(hPen[iPen]);
		
		hPen[iPen] = hPenNew;
		nPenStyles[iPen] = nPenStyle; 
		nPenWidths[iPen] = nPenWidth; 
		crColRef[iPen] = pColTbl[nPenColor];
	}
}

void CPrimState::SetPenColor(PENCOLOR nPenColor)
{
	CPegView* pView = CPegView::GetActiveView();
	CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();
	
	m_nPenColor = nPenColor;
	SetPen(pDC, nPenColor, 0, m_nPenStyle);
}

void CPrimState::SetPenColor(CDC* pDC, PENCOLOR nPenColor)
{
	//CPegView* pView = CPegView::GetActiveView();
	//CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();
	
	m_nPenColor = nPenColor;
	SetPen(pDC, nPenColor, 0, m_nPenStyle);
}

void CPrimState::SetPenStyle(PENSTYLE nPenStyle)
{
	CPegView* pView = CPegView::GetActiveView();
	CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();

	m_nPenStyle = nPenStyle;
	SetPen(pDC, m_nPenColor, 0, nPenStyle);
}

void CPrimState::SetPenStyle(CDC* pDC, PENSTYLE nPenStyle)
{
//	CPegView* pView = CPegView::GetActiveView();
//	CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();

	m_nPenStyle = nPenStyle;
	SetPen(pDC, m_nPenColor, 0, nPenStyle);
}

int CPrimState::SetROP2(CDC* pDC, int iDrawMode)
{
	// Sets the current foreground mix mode. GDI uses the foreground mix mode to combine pens and
	// interiors of filled objects with the colors already on the screen. The foreground mix mode
	// defines how colors from the brush or pen and the colors in the existing image are to be combined. 

//	if(crHotCols[0] == RGB(0xFF, 0xFF, 0xFF))
	if(crHotCols[app.BackgroundColorGet()] == RGB(0xFF, 0xFF, 0xFF))
	{
		if (iDrawMode == R2_XORPEN)
			iDrawMode = R2_NOTXORPEN;
	}

//	if(crHotCols[app.BackgroundColorGet()] != RGB(0x00, 0x00, 0x00))
//	{
//		if (iDrawMode == R2_XORPEN)
//		{
////			if(crHotCols[app.BackgroundColorGet()] == RGB(0xff, 0xff, 0xff))
//				iDrawMode = R2_NOTXORPEN;
////			else
////				iDrawMode = R2_COPYPEN;
//		}
//	}

//	if(iDrawMode == R2_COPYPEN)
//		iDrawMode = R2_NOTCOPYPEN;
//	else if(iDrawMode == R2_XORPEN)
//		iDrawMode = R2_COPYPEN;

	return (pDC->SetROP2(iDrawMode));
}

void CPrimState::SetTxtAlign(CDC* pDC, WORD wHor, WORD wVer)
{
	//if (wHor == 1)
	//	nFlgs = TA_LEFT;
	//else if (wHor == 2)
	//	nFlgs = TA_CENTER;
	//else
	//	nFlgs = TA_RIGHT;

	//if (wVer == 2)
	//	nFlgs |= TA_TOP;
	//else
	//	nFlgs |= TA_BASELINE;

	m_fd.TextHorAlignSet(wHor);
	m_fd.TextVerAlignSet(wVer);

	pDC->SetTextAlign(TA_LEFT | TA_BASELINE);
}

void CPrimState::SetFontDef(CDC* pDC, const CFontDef& fd)
{
	m_fd = fd;
	SetTxtAlign(pDC, m_fd.TextHorAlign(), m_fd.TextVerAlign());
}