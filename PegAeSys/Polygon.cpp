#include "stdafx.h"

#include "PegAEsys.h"

#include "Polygon.h"

///<summary>Generates polygon.</summary>
// The polygon is closed automatically by drawing a line from the last vertex to the first.
// Arrays of vertices are previously modelview transformed and clipped to view volume.
void Polygon_Display(CDC* pDC, CPnt4s& pta) 
{	
	int iPts = (int) pta.GetSize();
	if (iPts >= 2)
	{
		CPoint* pnt = new CPoint[iPts];
		
		viewport::DoProjection(pnt, pta);
	
		if (pstate.PolygonIntStyle() == POLYGON_SOLID)
		{
			CBrush brush(pColTbl[pstate.PenColor()]);
			CBrush* pBrushOld = pDC->SelectObject(&brush);
			pDC->Polygon(pnt, iPts);
			pDC->SelectObject(pBrushOld);
		}
		else if (pstate.PolygonIntStyle() == POLYGON_HOLLOW)
		{
			CBrush* pBrushOld = (CBrush*) pDC->SelectStockObject(NULL_BRUSH);
			pDC->Polygon(pnt, iPts);
			pDC->SelectObject(pBrushOld);
		}
		else
		{
			pDC->Polygon(pnt, iPts);
		}
		delete [] pnt;
	}
}

///<summary>Determines points necessary to represent a polygon with line segments.</summary>
// ptCP		polygon center point
// vXRefAx 	x-axis reference vector
// vYRefAx 	y-axis reference vector
// iPts		number of points defining polygon
// pt			points defining polygon
void Polygon_GenPts(const CPnt& ptCP, const CVec& vXRefAx, const CVec& vYRefAx, int iPts, CPnt *pt) 		
{
	CTMat	tm(ptCP, vXRefAx, vYRefAx);
	
	tm.Inverse();
	
	// Determine the parameter (angular increment)
	double dAngInc = TWOPI / double(iPts - 1);
	double dCosInc = cos(dAngInc);
	double dSinInc = sin(dAngInc);
	
	pt[0](1., 0., 0.);
	
	for(int i = 0; i < iPts - 1; i++) 
		pt[i + 1](pt[i][0] * dCosInc - pt[i][1] * dSinInc, pt[i][1] * dCosInc + pt[i][0] * dSinInc, 0.);
	
	for(int i = 0; i < iPts; i++)
		pt[i] = tm * pt[i];
	
}

///<summary>Sutherland-hodgman-like polygon clip by clip plane.</summary>
///<remarks>Visibility determined using dot product.</remarks>
// ptaIn	coordinates of vetices of input polygon
// ptQ 	coordinates of any point on clip plane
// vQNorm	normal vector of clip plane
// ptaOut	coordinates of vertices of output polygon
void Polygon_IntersectionWithPln(CPnt4s& ptaIn, const CPnt4& ptQ, const CVec4& vQNorm, CPnt4s& ptaOut)
{
	if (ptaIn.IsEmpty()) return;
	
	CPnt4 pt;
	CPnt4 ptEdge[2];
	bool bEdgeVis[2];
	
	bool bVisVer0 = CVec4(ptQ, ptaIn[0]) * vQNorm >= - DBL_EPSILON ? true : false;
	
	ptEdge[0] = ptaIn[0];
	bEdgeVis[0] = bVisVer0;
	
	if (bVisVer0)
	{
		ptaOut.Add(ptaIn[0]);
	}
	int iPtsIn = (int) ptaIn.GetSize();
	for (int i = 1; i < iPtsIn; i++)
	{	
		ptEdge[1] = ptaIn[i];
		bEdgeVis[1] = CVec4(ptQ, ptEdge[1]) * vQNorm >= - DBL_EPSILON ? true : false;
		
		if (bEdgeVis[0] != bEdgeVis[1])
		{	// Vertices of edge on opposite sides of clip plane
			pt = line::IntersectionWithPln(ptEdge[0], ptEdge[1], ptQ, vQNorm);
			ptaOut.Add(pt);
		}
		if (bEdgeVis[1])
		{
			ptaOut.Add(ptaIn[i]);
		}
		ptEdge[0] = ptEdge[1];
		bEdgeVis[0] = bEdgeVis[1];
	}
	if (ptaOut.GetSize() != 0 && bEdgeVis[0] != bVisVer0)
	{	// first and last vertices on opposite sides of clip plane
		pt = line::IntersectionWithPln(ptEdge[0], ptaIn[0], ptQ, vQNorm);		
		ptaOut.Add(pt);
	}
}

///<summary>Sutherland-hodgman-like polygon clip by view volume.</summary>
void Pnt4_ClipPolygon(CPnt4s& pta)
{	
	static CPnt4 ptPln[] = 
	{
		CPnt4(- 1., 0., 0., 1.), CPnt4(1., 0., 0., 1.),
		CPnt4(0., - 1., 0., 1.), CPnt4(0., 1., 0., 1.),
		CPnt4(0., 0., - 1., 1.), CPnt4(0., 0., 1., 1.)
	}; 
	
	static CVec4 vPln[] = 
	{
		CVec4(1., 0., 0., 0.), CVec4(- 1., 0., 0., 0.),
		CVec4(0., 1., 0., 0.), CVec4(0., - 1., 0., 0.),
		CVec4(0., 0., 1., 0.), CVec4(0., 0., - 1., 0.)
	};
	
	CPnt4s ptaOut;
	
	for (int iPln = 0; iPln < 6; iPln++)
	{
		Polygon_IntersectionWithPln(pta, ptPln[iPln], vPln[iPln], ptaOut);
		
		int iPtsOut = (int) ptaOut.GetSize();
		pta.SetSize(iPtsOut);
		
		if (iPtsOut == 0)
			break;
		
		pta.Copy(ptaOut);
		ptaOut.RemoveAll();
	}
}