#include "stdafx.h"

#include "PegAEsys.h"

#include "Grid.h"
#include "Polyline.h"

using namespace grid;

namespace grid
{
	bool	bDotsOn = false;
	bool	bLnsOn = false;
	bool	bSnapOn = false;

	//int		iDotsMax = 32;
	int		iDotsMax = 128;

	double	dXSnapInt = .001302083;  //	dXSnapInt = 1. / app.GetScale(96) / app.GetUnitsPrec(8);
	double	dYSnapInt = .001302083;
	double	dZSnapInt = .001302083;
	double	dXDotInt = 1.;
	double	dYDotInt = 1.;
	double	dZDotInt = 0.;
	double	dXLnInt = 4.;
	double	dYLnInt = 4.;
	double	dZLnInt = 4.;
	double	dXOrig = 0.;
	double	dYOrig = 0.;
	double	dZOrig = 0.;
}

void DisplayPixel(CDC* pDC, COLORREF, CPnt);

//void grid::Init()
//{
//	SetSnapState(false);
//	SetLinesState(false);
//	SetDotsState(false);

//	SetDotsMax(32);
//	SetDotsMax(128);
	
//	dXSnapInt = 1. / app.GetScale() / app.GetUnitsPrec();
//	dYSnapInt = 1. / app.GetScale() / app.GetUnitsPrec();
//	dZSnapInt = 1. / app.GetScale() / app.GetUnitsPrec();

//	dXDotInt = 1.;
//	dYDotInt = 1.;
//	dZDotInt = 0.;
//	dXLnInt = 4.;
//	dYLnInt = 4.;
//	dZLnInt = 4.;

//	dXOrig = dYOrig = dZOrig = 0;
//}

///<summary>
///Generates a marker display centered about the user origin in one or more of the three orthogonal planes 
///for the current user grid.
///</summary>
void grid::Display(CDC* pDC)
{	
	int	i2;
	
	double dHalfPts = iDotsMax * .5;
	CPnt ptUsrOrig = GetOrign();
	
	if(bDotsOn)
	{	
		CPnt pt;

		if(fabs(dYDotInt) > DBL_EPSILON && fabs(dZDotInt) > DBL_EPSILON) 
		{
			pt[0] = ptUsrOrig[0];
			pt[2] = ptUsrOrig[2] - dHalfPts * dZDotInt;
			for (int i = 0; i < iDotsMax; i++)
			{
				pt[1] = ptUsrOrig[1] - dHalfPts * dYDotInt;
				for (i2 = 0; i2 < iDotsMax; i2++)
				{
					DisplayPixel(pDC, app.PenColorsGetHot(1), pt);
					pt[1] += dYDotInt;
				}
				pt[2] += dZDotInt;
			}
		}
		if (fabs(dXDotInt) > DBL_EPSILON && fabs(dZDotInt) > DBL_EPSILON) 
		{
			pt[0] = ptUsrOrig[0] - dHalfPts * dXDotInt;
			pt[1] = ptUsrOrig[1];
			for (int i = 0; i < iDotsMax; i++)
			{
				pt[2] = ptUsrOrig[2] - dHalfPts * dZDotInt;
				for (i2 = 0; i2 < iDotsMax; i2++)
				{
					DisplayPixel(pDC, app.PenColorsGetHot(2), pt);
					pt[2] += dZDotInt;
				}
				pt[0] += dXDotInt;
			}
		}
		if (fabs(dXDotInt) > DBL_EPSILON && fabs(dYDotInt) > DBL_EPSILON) 
		{
			pt[1] = ptUsrOrig[1] - dHalfPts * dYDotInt;
			pt[2] = ptUsrOrig[2];
			for (int i = 0; i < iDotsMax; i++)
			{
				pt[0] = ptUsrOrig[0] - dHalfPts * dXDotInt;
				for (i2 = 0; i2 < iDotsMax; i2++)
				{
					DisplayPixel(pDC, app.PenColorsGetHot(3), pt);
					pt[0] += dXDotInt;
				}
				pt[1] += dYDotInt;
			}
		}
	}
	if (bLnsOn)
	{
		if (fabs(dXLnInt) > DBL_EPSILON && fabs(dYLnInt) > DBL_EPSILON) 
		{
			CLine ln;
	
			int i;
			PENCOLOR nPenColor = pstate.PenColor();
			PENSTYLE nPenStyle = pstate.PenStyle();
			pstate.SetPen(pDC, 15, 2);
			
			ln[0][0] = ptUsrOrig[0] - dHalfPts * dXLnInt;
			ln[1][0] = ptUsrOrig[0] + dHalfPts * dXLnInt;
			ln[0][1] = ptUsrOrig[1] - dHalfPts * dYLnInt;
			ln[0][2] = ptUsrOrig[2];
			ln[1][2] = ptUsrOrig[2];
			for (i = 0; i < iDotsMax; i++)
			{
				ln[1][1] = ln[0][1];
				ln.Display(pDC);
				ln[0][1] += dYLnInt;
			}
			ln[0][1] = ptUsrOrig[1] - dHalfPts * dYLnInt;
			ln[1][1] = ptUsrOrig[1] + dHalfPts * dYLnInt;
			for (i = 0; i < iDotsMax; i++)
			{
				ln[1][0] = ln[0][0];
				ln.Display(pDC);
				ln[0][0] += dXLnInt;
			}
			pstate.SetPen(pDC, nPenColor, nPenStyle);
		}
	}
}

///<summary>Set a pixel.</summary>
void DisplayPixel(CDC* pDC, COLORREF alRGB, CPnt ptWrld)
{
	CPnt4 pt(ptWrld, 1.);
	
	ModelView_DoTransform(pt);

	if(Pnt4_IsInView(pt))
	{	// Point is within current view volume
		CPoint pnt;
		pnt = viewport::DoProjection(pt);
		pDC->SetPixel(pnt, alRGB);
	}
}

///<summary>Determines the nearest point on system constraining grid.</summary>
CPnt grid::UserSnapPt(const CPnt& arPt)
{
	CPnt	pt = arPt;
	CPnt	ptUsrOrig = GetOrign();
		
	if (IsSnapEnabled())
	{
		if (fabs(dXSnapInt) > DBL_EPSILON)
		{
			pt[0] -= fmod((arPt[0] - ptUsrOrig[0]), dXSnapInt);
			if (fabs(pt[0] - arPt[0]) > dXSnapInt * .5)
				pt[0] += fsign(dXSnapInt, arPt[0] - ptUsrOrig[0]);
		}
		if (fabs(dYSnapInt) > DBL_EPSILON)
		{
			pt[1] -= fmod((arPt[1] - ptUsrOrig[1]), dYSnapInt);
			if (fabs(pt[1] - arPt[1]) > dYSnapInt * .5)
				pt[1] += fsign(dYSnapInt, arPt[1] - ptUsrOrig[1]);
		}
		if (fabs(dZSnapInt) > DBL_EPSILON)
		{
			pt[2] -= fmod((arPt[2] - ptUsrOrig[2]), dZSnapInt);
			if (fabs(pt[2] - arPt[2]) > dZSnapInt * .5)
				pt[2] += fsign(dZSnapInt, arPt[2] - ptUsrOrig[2]);
		}
	}
	return (pt);
}

CPnt grid::GetOrign() 
{
	return (CPnt(dXOrig, dYOrig, dZOrig));
}

CPnt grid::SetOrign(const CPnt& ptOrig)
{
	CPnt pt(dXOrig, dYOrig, dZOrig);
	
	dXOrig = ptOrig[0];
	dYOrig = ptOrig[1];
	dZOrig = ptOrig[2];

	return (pt);
}