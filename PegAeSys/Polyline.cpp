#include "stdafx.h"

#include "PegAEsysDoc.h"

namespace polyline
{
	CPnts	pts_;
}

CPolyline::CPolyline(CPnts& pts)
{
	m_wFlags = 0;
	m_pts.Copy(pts);
}

CPolyline::CPolyline(const CPolyline& src)
{
	m_wFlags = src.m_wFlags;
	m_pts.Copy(src.m_pts);
}

CPolyline& CPolyline::operator=(const CPolyline& src)
{
	m_wFlags = src.m_wFlags;
	m_pts.Copy(src.m_pts);

	return (*this);
}

void CPolyline::operator+=(const CVec& v)
{
	for (WORD w = 0; w < m_pts.GetSize(); w++) 
		m_pts[w] += v;
}

void CPolyline::operator-=(const CVec& v)
{
	for (WORD w = 0; w < m_pts.GetSize(); w++)
		m_pts[w] -= v;
}

void polyline::BeginLineStrip() 
{
//	opengl::BeginLineStrip();
	polyline::pts_.SetSize(0);
}

///<summary>Outputs a simple polyline.</summary>
void polyline::Display(CDC* pDC, const CPnts& pts)
{
	if (pts.GetSize() < 2) return;

	CPnt4 ln[2];
	CPoint pnt[2];

	ln[0] = pts[0];
	ModelView_DoTransform(ln[0]);
		
	for (int n = 1; n < pts.GetSize(); n++) 
	{
		ln[1] = pts[n];
			
		ModelView_DoTransform(ln[1]);
		CPnt ptT = ln[1];
		if (Pnt4_ClipLine(ln[0], ln[1]))
		{
			viewport::DoProjection(pnt, 2, &ln[0]);
			pDC->Polyline(pnt, 2);
		}
		ln[0] = ptT;
	}
}

void polyline::Display(CDC* pDC, const CPnts& pts, short nPenStyle)	
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	if(CPrim::IsSupportedTyp(nPenStyle))
		polyline::Display(pDC, pts);
	else
	{
		CPenStyle* pPenStyle = pDoc->PenStylesGetAtSafe(nPenStyle);
		if (pPenStyle == 0) return;
	
		WORD wDefLen = pPenStyle->GetDefLen();
		if (wDefLen == 0) return;

		CPnt4 ln[2];
		CPoint pnt[2];
		CPnt pt[2];

		int iDashDefId = 0;

		double* dDash = new double[wDefLen];

		pPenStyle->GetDashLen(dDash);

		//pstate.SetPenStyle(1);
		pstate.SetPen(pDC, pstate.PenColor(), 1);

		double dSecLen = Max(.025/* * 96.*/, fabs(dDash[iDashDefId]));
	
		for (int i = 0; i < (int) pts.GetSize() - 1; i++) 
		{
			CVec vLn(pts[i], pts[i + 1]);
			pt[0] = pts[i];
		
			double dVecLen = vLn.Length();
			double dRemDisToEnd = dVecLen;
		
			while (dSecLen <= dRemDisToEnd + DBL_EPSILON) 
			{
				CVec vDash(vLn);
				vDash *= dSecLen / dVecLen;
				pt[1] = pt[0] + vDash;
				dRemDisToEnd -= dSecLen;
				if (dDash[iDashDefId] >= 0.)
				{
					ln[0] = pt[0];
					ln[1] = pt[1];

					ModelView_DoTransform(2, ln);

					if (Pnt4_ClipLine(ln[0], ln[1]))
					{
						viewport::DoProjection(pnt, 2, &ln[0]);
						pDC->Polyline(pnt, 2);
					}
				}
				pt[0] = pt[1];
				iDashDefId = (iDashDefId + 1) % wDefLen;
				dSecLen = Max(.025/* * 96.*/, fabs(dDash[iDashDefId]));
			}
			if (dRemDisToEnd > DBL_EPSILON) 							
			{	// Partial component of dash section must produced
				if (dDash[iDashDefId] >= 0.) 
				{
					pt[1] = pts[i + 1];
			
					ln[0] = pt[0];
					ln[1] = pt[1];

					ModelView_DoTransform(2, ln);

					if (Pnt4_ClipLine(ln[0], ln[1]))
					{
						viewport::DoProjection(pnt, 2, &ln[0]);
						pDC->Polyline(pnt, 2);
					}
				}
			}
			// Length of dash remaining
			dSecLen -= dRemDisToEnd;							
		}
		delete [] dDash;
		
		//pstate.SetPenStyle(nPenStyle);
		pstate.SetPen(pDC, pstate.PenColor(), nPenStyle);
	}
}

///<summary>Outputs a simple polyline with closure.</summary>
void polyline::DisplayLoop(CDC* pDC, const CPnts& pts)
{
	if (pts.GetSize() < 2) return;

	CPnt4 ln[2];
	CPoint pnt[2];

	ln[0] = pts[0];
	ModelView_DoTransform(ln[0]);
		
	for (int n = 1; n <= pts.GetSize(); n++) 
	{
		ln[1] = pts[n % pts.GetSize()];
			
		ModelView_DoTransform(ln[1]);
		CPnt ptT = ln[1];
		if (Pnt4_ClipLine(ln[0], ln[1]))
		{
			viewport::DoProjection(pnt, 2, &ln[0]);
			pDC->Polyline(pnt, 2);
		}
		ln[0] = ptT;
	}
}

void polyline::End(CDC* pDC, short nPenStyle) 
{
//	if (pDC == 0)
//		opengl::End();
//	else
		polyline::Display(pDC, polyline::pts_, nPenStyle);
}

///<summary>Determines how many times (if any), a line segment intersects with polyline.</summary>
bool polyline::SelUsingLine(const CLine& ln, CPnts& ptsInt)
{	
	CPnt4 ptBeg(pts_[0], 1.);
	CPnt4 ptEnd;

	ptsInt.RemoveAll();
	
	ModelView_DoTransform(ptBeg);
		
	for (WORD w = 1; w < pts_.GetSize(); w++)
	{
		ptEnd = CPnt4(pts_[w], 1.);
		ModelView_DoTransform(ptEnd);
		
		CPnt ptInt;
		if (line::Intersection_xy(ln, CLine(ptBeg, ptEnd), ptInt)) 
		{
			double dRel;
			line::RelOfPtToEndPts(ln, ptInt, dRel);
			if (dRel >= - DBL_EPSILON && dRel <= 1. + DBL_EPSILON)
			{
				line::RelOfPtToEndPts(CLine(ptBeg, ptEnd), ptInt, dRel);		
				if (dRel >= - DBL_EPSILON && dRel <= 1. + DBL_EPSILON) 
				{
					ptInt[2] = ptBeg[2] + dRel * (ptEnd[2] - ptBeg[2]);
					ptsInt.Add(ptInt);
				}
			}
		}
		ptBeg = ptEnd;
	}
	return (!ptsInt.IsEmpty());
}

bool polyline::SelUsingPoint(const CPnt4& ptPic, double dTol, double& dRel, CPnt& ptProj)
{
	bool bResult = false;

	double dProjDis;
	
	CPnt4 ptBeg(pts_[0], 1.);
	ModelView_DoTransform(ptBeg);
	
	for (int i = 1; i < (int) pts_.GetSize(); i++) 
	{
		CPnt4 ptEnd = CPnt4(pts_[i], 1.);
		ModelView_DoTransform(ptEnd);
		if (line::EvalPtProx_xy(CLine(ptBeg, ptEnd), ptPic, dTol, ptProj, &dRel, &dProjDis)) 
		{
			ptProj[2] = ptBeg[2] + dRel * (ptEnd[2] - ptBeg[2]);
			bResult = true;
			break;
		}
		ptBeg = ptEnd;
	}
	return (bResult);
}

bool polyline::SelUsingRect(const CPnt& pt1, const CPnt& pt2)
{
	CPnt4 ptBeg(pts_[0], 1.);
	ModelView_DoTransform(ptBeg);
	
	for (WORD w = 1; w < pts_.GetSize(); w++) 
	{
		CPnt4 ptEnd(pts_[w], 1.);
		ModelView_DoTransform(ptEnd);
	
		if (p2DEvalWndLn(pt1, pt2, ptBeg, ptEnd))
			return true;

		ptBeg = ptEnd;
	}
	return false;
}

// Note considering possible closure (see pic)
bool polyline::SelUsingRect(const CPnt& pt1, const CPnt& pt2, const CPnts& pts)
{
	CPnt4 ptBeg(pts[0], 1.);
	ModelView_DoTransform(ptBeg);
	
	for (WORD w = 1; w < pts.GetSize(); w++) 
	{
		CPnt4 ptEnd(pts[w], 1.);
		ModelView_DoTransform(ptEnd);
	
		if (p2DEvalWndLn(pt1, pt2, ptBeg, ptEnd))
			return true;

		ptBeg = ptEnd;
	}
	return false;
}

#include <math.h>

void polyline::SetVertex(const CPnt& pt) 
{
	//if(pt.AssertValid())
	//	return;

	//CPnt pntNew = pt;

	//if((pt[0] <= -1.7e308) || (pt[0] >= 1.7e308))
	////if(isinf(pt[0]))
	//	pntNew[0] = 0;
	//if((pt[1] <= -1.7e308) || (pt[1] >= 1.7e308))
	//	pntNew[1] = 0;
	//if((pt[2] <= -1.7e308) || (pt[2] >= 1.7e308))
	//	pntNew[2] = 0;

	//// MAK - stray segment fix?
	//if(polyline::pts_.GetCount() > 0)
	//{
	//	CPnt pntPrev = polyline::pts_[polyline::pts_.GetCount()-1];
	//	double dLength = CLine(pntPrev, pt).Length();
	//	if(dLength <= DBL_EPSILON)
	//		return;
	//}

//	opengl::SetVertex(pt);
	polyline::pts_.Add(pt);

//	polyline::pts_.Add(pntNew);
}