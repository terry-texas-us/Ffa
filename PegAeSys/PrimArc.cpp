#include "stdafx.h"

#include "ddeGItms.h"
#include "Polyline.h"
#include "UnitsString.h"

#include "SubProcDraw.h"
#include "subprocnodal.h"

CPrimArc::CPrimArc(const CPrimArc& src)
{
	m_nPenColor = src.m_nPenColor;
	m_nPenStyle = src.m_nPenStyle;
	m_ptCenter = src.m_ptCenter;
	m_vMajAx = src.m_vMajAx;
	m_vMinAx = src.m_vMinAx;
	m_dSwpAng = src.m_dSwpAng; 
}

///<summary>Arc is constructed using a center point, a major and minor vector and a sweep ang.</summary>
CPrimArc::CPrimArc(const CPnt& ptCP, const CVec& vMajAx, const CVec& vMinAx, double dSwpAng)
	: m_ptCenter(ptCP), m_vMajAx(vMajAx), m_vMinAx(vMinAx)
{
	m_nPenColor = pstate.PenColor();
	m_nPenStyle = pstate.PenStyle();
	m_dSwpAng = dSwpAng; 
}

///<summary>Arc is constructed using a center point, a major and minor vector and a sweep ang.</summary>
CPrimArc::CPrimArc(PENCOLOR nPenColor, PENSTYLE nPenStyle, const CPnt& ptCP, const CVec& vMajAx, const CVec& vMinAx, double dSwpAng)
	: m_ptCenter(ptCP), m_vMajAx(vMajAx), m_vMinAx(vMinAx)
{
	m_nPenColor = nPenColor;
	m_nPenStyle = nPenStyle;
	m_dSwpAng = dSwpAng; 
}

///<summary>Circle is constructed using a center point and a begin point about view plane normal.</summary>
CPrimArc::CPrimArc(const CPnt& ptCent, const CPnt& ptBeg)
{
	m_nPenColor = pstate.PenColor();
	m_nPenStyle = pstate.PenStyle();
	
	CVec vPlnNorm = - view::GetDirection();
	
	m_ptCenter = ptCent;
	m_vMajAx = CVec(ptCent, ptBeg);
	m_vMinAx = m_vMajAx;
	m_vMinAx.RotAboutArbAx(vPlnNorm, HALF_PI);
	m_dSwpAng = TWOPI;
}

///<summary>Arc is constructed using three points on arc.</summary>
CPrimArc::CPrimArc(const CPnt& ptBeg, const CPnt& ptInt, const CPnt& ptEnd)
{
	m_nPenColor = pstate.PenColor();
	m_nPenStyle = pstate.PenStyle();
		
	m_dSwpAng = 0.;
	
	if(draw::eArcType == draw::ARC_THREE_POINT)
	{
		CVec vBegInt(ptBeg, ptInt);
		CVec vBegEnd(ptBeg, ptEnd);
		CVec vPlnNorm = vBegInt ^ vBegEnd;
		vPlnNorm.Normalize();

		// Build transformation matrix which will get int and end points to
		// z=0 plane with beg point as origin
		
		CTMat tm(ptBeg, vPlnNorm);
		CPnt pt[3];

		pt[0] = ptBeg;
		pt[1] = ptInt;
		pt[2] = ptEnd;
		
		pt[1] = tm * pt[1];
		pt[2] = tm * pt[2];
		
		double dDet = (pt[1][0] * pt[2][1] - pt[2][0] * pt[1][1]);
		
		if (fabs(dDet) > DBL_EPSILON)
		{	// Three points are not colinear
			double dT = ((pt[2][0] - pt[1][0]) * pt[2][0] + pt[2][1] * (pt[2][1] - pt[1][1])) / dDet;
			
			m_ptCenter[0] = (pt[1][0] - pt[1][1] * dT) * .5;
			m_ptCenter[1] = (pt[1][1] + pt[1][0] * dT) * .5;
			m_ptCenter[2] = 0.;
			tm.Inverse();
			
			// Transform back to original plane
			m_ptCenter = tm * m_ptCenter;						
		
			// None of the points coincide with center point

			CTMat tm(m_ptCenter, vPlnNorm);

			double dAng[3];

			pt[1] = ptInt;
			pt[2] = ptEnd;
		
			for (int i = 0; i < 3; i++) 
			{	// Translate points into z=0 plane with center point at origin
				pt[i] = tm * pt[i];
				dAng[i] = atan2(pt[i][1], pt[i][0]);
				if (dAng[i] < 0.)
					dAng[i] += TWOPI;
			}
			double dMin = Min(dAng[0], dAng[2]);
			double dMax = Max(dAng[0], dAng[2]);
		
			if (fabs(dAng[1] - dMax) > DBL_EPSILON && fabs(dAng[1] - dMin) > DBL_EPSILON) 
			{	// Inside line is not colinear with outside lines
				m_dSwpAng = dMax - dMin;
				if (dAng[1] > dMin && dAng[1] < dMax) 
				{
					if (dAng[0] == dMax)
						m_dSwpAng = - m_dSwpAng;
				}
				else 
				{
					m_dSwpAng = TWOPI - m_dSwpAng;
					if (dAng[2] == dMax)
						m_dSwpAng = - m_dSwpAng;
				}
				CPnt ptRot = Pnt_RotAboutArbPtAndAx(ptBeg, m_ptCenter, vPlnNorm, HALF_PI);
				
				m_vMajAx = CVec(m_ptCenter, ptBeg);
				m_vMinAx = CVec(m_ptCenter, ptRot);
			}
		}
	}
	else if(draw::eArcType == draw::ARC_C2_POINT)
	{
		CVec vPlnNorm;
		vPlnNorm = view::GetDirection();

		m_ptCenter = ptBeg;

		CVec vecAB(ptBeg, ptInt);
		CVec vecAC(ptBeg, ptEnd);
		CVec vecBC(ptInt, ptEnd);

		m_vMajAx = vecAB;
		CPnt ptRot = Pnt_RotAboutArbPtAndAx(ptInt, m_ptCenter, vPlnNorm, HALF_PI);
		m_vMinAx = CVec(m_ptCenter, ptRot);

		double aLen = vecBC.Length();
		double bLen = vecAC.Length();
		double cLen = vecAB.Length();

		m_dSwpAng = (double)acos( ((bLen * bLen) + (cLen * cLen) - (aLen * aLen)) / (2 * bLen * cLen) );

		// test whether inside or outside to fix angles

		CPnt ptMin = Pnt_RotAboutArbPtAndAx(ptInt, ptBeg, vPlnNorm, m_dSwpAng);
//		CPnt ptMin = Pnt_RotAboutArbPtAndAx(ptEnd, ptBeg, vPlnNorm, m_dSwpAng);
		
//		CVec vMinAx(ptBeg, ptMin);
		CVec vMinAx(ptMin, ptBeg);
	}
}

const CPrimArc& CPrimArc::operator=(const CPrimArc& src)
{
	m_nPenColor = src.m_nPenColor;
	m_nPenStyle = src.m_nPenStyle;
	m_ptCenter = src.m_ptCenter;
	m_vMajAx = src.m_vMajAx;
	m_vMinAx = src.m_vMinAx;
	m_dSwpAng = src.m_dSwpAng; 
	return (*this);
}

CPrim*& CPrimArc::Copy(CPrim*& pPrim) const
{
	pPrim = new CPrimArc(*this);
	return (pPrim);
}

//void CPrimArc::CutAt2Pts(CPnt* pt, CSegs* pSegs, CSegs* pSegsNew)
//{ 
//	CPrimArc* pArc;
//	
//	double dRel[2];
//	
//	dRel[0] = SwpAngToPt(pt[0]) / m_dSwpAng;
//	dRel[1] = SwpAngToPt(pt[1]) / m_dSwpAng;
//
////	char buffer[34];
////	itoa(dRel[0] * 1000, buffer, 10);
////	MessageBox(0, buffer, 0, 0);
//	
//	// entire arc
//	if (dRel[0] <= DBL_EPSILON && dRel[1] >= 1. - DBL_EPSILON)
//	{	// Put entire arc in trap
//		pArc = this;
//	}
//	// entire circle
////	else if((fabs(m_dSwpAng - TWOPI) <= DBL_EPSILON) && ())
////	{
//
////	}
//	else
//	{	// Something gets cut
//		CVec vPlnNorm = m_vMajAx ^ m_vMinAx;
//		vPlnNorm.Normalize();
//	
//		//// if circle
//		//if(fabs(m_dSwpAng - TWOPI) <= DBL_EPSILON)
//		//{
//		//	m_dSwpAng = (dRel[1] - dRel[0]) * TWOPI;
//		//
//		//	m_vMajAx.RotAboutArbAx(vPlnNorm, dRel[0] * TWOPI);
//		//	m_vMinAx.RotAboutArbAx(vPlnNorm, dRel[0] * TWOPI);
//		//
//		//	pArc = new CPrimArc(*this);
//		//
//		//	m_vMajAx.RotAboutArbAx(vPlnNorm, m_dSwpAng);
//		//	m_vMinAx.RotAboutArbAx(vPlnNorm, m_dSwpAng);
//		//
//		//	m_dSwpAng = TWOPI - m_dSwpAng;
//		//}
//		//else														
//		{	// Arc section with a cut
//			pArc = new CPrimArc(*this);
//			double dSwpAng = m_dSwpAng;
//			
//			double dAng1 = dRel[0] * m_dSwpAng;
//			double dAng2 = dRel[1] * m_dSwpAng;
//		
//			// Cut section out of middle
//			if (dRel[0] > DBL_EPSILON && dRel[1] < 1. - DBL_EPSILON)
//			{
//				pArc->SetSwpAng(dAng1);
//				pSegs->AddTail(new CSeg(pArc));
//			
//				m_vMajAx.RotAboutArbAx(vPlnNorm, dAng1);
//				m_vMinAx.RotAboutArbAx(vPlnNorm, dAng1);
//				m_dSwpAng = dAng2 - dAng1;
//			
//				pArc = new CPrimArc(*this);
//			
//				m_vMajAx.RotAboutArbAx(vPlnNorm, m_dSwpAng);
//				m_vMinAx.RotAboutArbAx(vPlnNorm, m_dSwpAng);
//				m_dSwpAng = dSwpAng - dAng2;
//			}
//			// Cut section in two and place begin section in trap
//			else if (dRel[1] < 1. - DBL_EPSILON)
//			{
//				pArc->SetSwpAng(dAng2);
//			
//				m_vMajAx.RotAboutArbAx(vPlnNorm, dAng2);
//				m_vMinAx.RotAboutArbAx(vPlnNorm, dAng2);
//				m_dSwpAng = dSwpAng - dAng2;
//			}
//			// Cut section in two and place end section in trap
//			else
//			{
//				m_dSwpAng = dAng1;
//			
//				CVec v = m_vMajAx;
//				v.RotAboutArbAx(vPlnNorm, dAng1);
//				pArc->SetMajAx(v);
//				v = m_vMinAx;
//				v.RotAboutArbAx(vPlnNorm, dAng1);
//				pArc->SetMinAx(v);
//				pArc->SetSwpAng(dSwpAng - dAng1);
//			}
//		}
////		if(m_dSwpAng != 0)
//		pSegs->AddTail(new CSeg(this));
//	}
//	pSegsNew->AddTail(new CSeg(pArc));
//}

void CPrimArc::CutAt2Pts(CPnt* pt, CSegs* pSegs, CSegs* pSegsNew)
{
	// pSegs = pSegsOut
	// pSegsNew = pSegsIn

	CPrimArc* pArc;
	
	double dRel[2];	
	dRel[0] = SwpAngToPt(pt[0]) / m_dSwpAng;
	dRel[1] = SwpAngToPt(pt[1]) / m_dSwpAng;

	// if circle
	if(fabs(m_dSwpAng - TWOPI) <= DBL_EPSILON)
	{
		// all of circle
		if(1)//true
		{
			
		}
		// else part of circle
		else
		{
			CVec vPlnNorm = m_vMajAx ^ m_vMinAx;
			vPlnNorm.Normalize();

			m_dSwpAng = (dRel[1] - dRel[0]) * TWOPI;
		
			m_vMajAx.RotAboutArbAx(vPlnNorm, dRel[0]);// * TWOPI);
			m_vMinAx.RotAboutArbAx(vPlnNorm, dRel[0]);// * TWOPI);
		
			pArc = new CPrimArc(*this);
		
			m_vMajAx.RotAboutArbAx(vPlnNorm, m_dSwpAng);
			m_vMinAx.RotAboutArbAx(vPlnNorm, m_dSwpAng);
		
			m_dSwpAng = TWOPI - m_dSwpAng;

			pSegs->AddTail(new CSeg(this));
			pSegsNew->AddTail(new CSeg(pArc));
		}
	}
	// else arc
	else
	{
		// all of arc
		if(dRel[0] <= DBL_EPSILON && dRel[1] >= 1. - DBL_EPSILON)
		{	// Put entire arc in trap
			pArc = this;
			pSegsNew->AddTail(new CSeg(pArc));
		}
		// part of arc - something gets cut
		else
		{
			CVec vPlnNorm = m_vMajAx ^ m_vMinAx;
			vPlnNorm.Normalize();
	
			// Arc section with a cut
			pArc = new CPrimArc(*this);
			double dSwpAng = m_dSwpAng;
			
			double dAng1 = dRel[0] * m_dSwpAng;
			double dAng2 = dRel[1] * m_dSwpAng;
		
			// Cut section out of middle
			if (dRel[0] > DBL_EPSILON && dRel[1] < 1. - DBL_EPSILON)
			{
				pArc->SetSwpAng(dAng1);
				pSegs->AddTail(new CSeg(pArc));
			
				m_vMajAx.RotAboutArbAx(vPlnNorm, dAng1);
				m_vMinAx.RotAboutArbAx(vPlnNorm, dAng1);
				m_dSwpAng = dAng2 - dAng1;
			
				pArc = new CPrimArc(*this);
			
				m_vMajAx.RotAboutArbAx(vPlnNorm, m_dSwpAng);
				m_vMinAx.RotAboutArbAx(vPlnNorm, m_dSwpAng);
				m_dSwpAng = dSwpAng - dAng2;
			}
			// Cut section in two and place begin section in trap
			else if (dRel[1] < 1. - DBL_EPSILON)
			{
				pArc->SetSwpAng(dAng2);
			
				m_vMajAx.RotAboutArbAx(vPlnNorm, dAng2);
				m_vMinAx.RotAboutArbAx(vPlnNorm, dAng2);
				m_dSwpAng = dSwpAng - dAng2;
			}
			// Cut section in two and place end section in trap
			else
			{
				m_dSwpAng = dAng1;
			
				CVec v = m_vMajAx;
				v.RotAboutArbAx(vPlnNorm, dAng1);
				pArc->SetMajAx(v);
				v = m_vMinAx;
				v.RotAboutArbAx(vPlnNorm, dAng1);
				pArc->SetMinAx(v);
				pArc->SetSwpAng(dSwpAng - dAng1);
			}

			pSegs->AddTail(new CSeg(this));
			pSegsNew->AddTail(new CSeg(pArc));
		}
	}
}

void CPrimArc::CutAtPt(const CPnt& pt, CSeg* pSeg)
{
	double dRel = SwpAngToPt(pt) / m_dSwpAng;
			
	// Nothing to cut
	if (dRel <= DBL_EPSILON || dRel >= 1. - DBL_EPSILON)
		return;

	// if circle
	if (fabs(m_dSwpAng - TWOPI) <= DBL_EPSILON)
	{
		//Do not fragment a circle
		return;    
	}
	//if not circle
	else
	{
		double dSwpAng = m_dSwpAng * dRel;

		CPrimArc* pArc = new CPrimArc(*this);
		pArc->SetSwpAng(dSwpAng);
		pSeg->AddTail(pArc); 

		CVec vPlnNorm = m_vMajAx ^ m_vMinAx;
		vPlnNorm.Normalize();
		
		m_vMajAx.RotAboutArbAx(vPlnNorm, dSwpAng);
		m_vMinAx.RotAboutArbAx(vPlnNorm, dSwpAng);
		m_dSwpAng -= dSwpAng;
	}
}

void CPrimArc::Display(CDC* pDC, bool bNodeDisplay)
{
	if (fabs(m_dSwpAng) <= DBL_EPSILON) return;

	PENCOLOR nPenColor = LogicalPenColor();
	PENSTYLE nPenStyle = LogicalPenStyle();

	pstate.SetPen(pDC, nPenColor, nPenStyle);

	polyline::BeginLineStrip();
	GenPts(m_ptCenter, m_vMajAx, m_vMinAx, m_dSwpAng);
	polyline::End(pDC, nPenStyle);

	if(bNodeDisplay)
	{
		CPnts pts;
		GetAllPts(pts);

		if((app.m_iModeId == ID_MODE_NODAL) || (app.m_iModeId == ID_MODE_NODALR))
		{
			CPrim *pPrim = (CPrim*)this;
			app.DisplayPoint(pDC, &pts, nodal::GetPrimMask(pPrim));
		}
		else
			app.DisplayPoint(pDC, &pts, 0);
	}
}

void CPrimArc::DisplayOpenGL(bool )//bNodeDisplay)
{
	//PENCOLOR nPenColor = LogicalPenColor();
	//opengl::SetCurrentColor(app.PenColorsGetHot(nPenColor));
	//
	//if (fabs(m_dSwpAng) <= DBL_EPSILON)
	//	return;
	//
	//CTMat tm(m_ptCenter, m_vMajAx, m_vMinAx);
	//tm.Inverse();
	//
	//// Number of points based on angle and a smothness coefficient (temporarily fixed at 8.)
	//double dLen = Max(m_vMajAx.Length(), m_vMinAx.Length());
	//int iPts = Max(2, abs(Round(m_dSwpAng / TWOPI * 32.)));
	//	iPts = Min(64, Max(iPts, abs(Round(m_dSwpAng * dLen / .250))));
	//
	//// Generate points using the double angle algorithm.

	//double dAng = m_dSwpAng / (iPts - 1);
	//double dCos = cos(dAng);
	//double dSin = sin(dAng);
	//
	//CPnt pt[2];
	//	
	//opengl::BeginLineStrip();

	//pt[0](1., 0., 0.);
	//
	//for (int i = 0; i < iPts; i++) 
	//{
	//	pt[1][0] = pt[0][0] * dCos - pt[0][1] * dSin;
	//	pt[1][1] = pt[0][1] * dCos + pt[0][0] * dSin;
	//	pt[1][2] = 0.;
	//
	//	pt[0] = tm * pt[0];
	//			
	//	opengl::SetVertex(pt[0]);
	//	
	//	pt[0] = pt[1];
	//}
	//opengl::End();
}

void CPrimArc::DisRep(const CPnt&) const
{
	CString str;
	str.Format("<Arc> Color: %s Style: %s SweepAngle %f MajorAxisLength: %f", 
		FormatPenColor(), FormatPenStyle(), m_dSwpAng, m_vMajAx.Length());
	msgInformation(str);
}

///<summary>
///Generates a set of points which may be used to represent a arc using
///a double angle algorithm.
///</summary>
void CPrimArc::GenPts(const CPnt& ptCent, const CVec& vMajAx, const CVec& vMinAx, double dSwpAng) const
{
	// Number of points based on angle and a smoothness coefficient
	double dLen = Max(vMajAx.Length(), vMinAx.Length());
	int iPts = Max(2, abs(Round(dSwpAng / TWOPI * 32.)));	
	iPts = Min(128, Max(iPts, abs(Round(dSwpAng * dLen / .250))));
	
	CTMat tm(ptCent, vMajAx, vMinAx);
	tm.Inverse();
	
	double dAng = m_dSwpAng / (iPts - 1);
	double dCos = cos(dAng);
	double dSin = sin(dAng);
	
	CPnt pt(1., 0., 0.);
	
	for (int i = 0; i < iPts; i++) 
	{
		polyline::SetVertex(tm * pt);
		pt(pt[0] * dCos - pt[1] * dSin, pt[1] * dCos + pt[0] * dSin, 0.);
	}
}

void CPrimArc::FormatGeometry(CString& str) const
{
	str += "Center Point;" + m_ptCenter.ToString();
	str += "Major Axis;" + m_vMajAx.ToString();
	str += "Minor Axis;" + m_vMinAx.ToString();
	str += "Plane Normal;" + (m_vMajAx ^ m_vMinAx).ToString();
}

void CPrimArc::FormatExtra(CString& str) const
{
	str.Format("Color;%s\tStyle;%s\tSweep Angle;%f\tMajor Axis Length;%f", 
		FormatPenColor(), FormatPenStyle(), m_dSwpAng, m_vMajAx.Length());
}

CPnt CPrimArc::GetBegPt() const
{
	return (m_ptCenter + m_vMajAx);
}

///<summary>Determines the bounding region. This is always a quad, but it may not be xy oriented.</summary>
void CPrimArc::GetBoundingBox(CPnts& ptsBox) const
{	
	ptsBox.SetSize(4);
	ptsBox[0] = CPnt(- 1., - 1., 0.);
	ptsBox[1] = CPnt(1., - 1., 0.);
	ptsBox[2] = CPnt(1., 1., 0.);
	ptsBox[3] = CPnt(- 1., 1., 0.);

	if (m_dSwpAng < 3. * TWOPI / 4.)
	{
		double dEndX = cos(m_dSwpAng);
		double dEndY = sin(m_dSwpAng);

		if (dEndX >= 0.) 
		{
			if (dEndY >= 0.)
			{	// Arc ends in quadrant one
				ptsBox[0][0] = dEndX;
				ptsBox[0][1] = 0.;
				ptsBox[1][1] = 0.;
				ptsBox[2][1] = dEndY;
				ptsBox[3][0] = dEndX;
				ptsBox[3][1] = dEndY;
			}
		}
		else
		{
			if (dEndY >= 0.)
			{	// Arc ends in quadrant two
				ptsBox[0][0] = dEndX;
				ptsBox[0][1] = 0.;
				ptsBox[1][1] = 0.;
				ptsBox[3][0] = dEndX;
			}
			else
			{	// Arc ends in quadrant three
				ptsBox[0][1] = dEndY;
				ptsBox[1][1] = dEndY;
			}
		}
	}
	CTMat tm(m_ptCenter, m_vMajAx, m_vMinAx);
	tm.Inverse();

	// caused a crash... which is weird, so i expanded
	//for (WORD w = 0; w < 4; ptsBox[w++] = tm * ptsBox[w]);
	for (WORD w = 0; w < 4; w++)
		ptsBox[w] = tm * ptsBox[w];
}

CPnt CPrimArc::GetEndPt() const 
{
	CTMat tm(m_ptCenter, m_vMajAx, m_vMinAx);
	tm.Inverse();

	CPnt pt(cos(m_dSwpAng), sin(m_dSwpAng), 0.);

	pt = tm * pt;
	return (pt);
}

void CPrimArc::GetXYExtents(CPnt arBeg, CPnt arEnd, CPnt* arMin, CPnt* arMax)
{	// Determines the xy extent of an arc segment.
	
	double dx = double(m_ptCenter[0] - arBeg[0]);
	double dy = double(m_ptCenter[1] - arBeg[1]);
	
	double dRad = sqrt(dx * dx + dy * dy);
	  
	(*arMin)[0] = m_ptCenter[0] - dRad;
	(*arMin)[1] = m_ptCenter[1] - dRad;
	(*arMax)[0] = m_ptCenter[0] + dRad;
	(*arMax)[1] = m_ptCenter[1] + dRad;
	
	if (arBeg[0] >= m_ptCenter[0]) 
	{
		if (arBeg[1] >= m_ptCenter[1])								// Arc begins in quadrant one
		{
			if (arEnd[0] >= m_ptCenter[0]) 
			{
				if (arEnd[1] >= m_ptCenter[1])							// Arc ends in quadrant one
				{
					if (arBeg[0] > arEnd[0])						// Arc in qraudrant one only
					{
						(*arMin)[0] = arEnd[0];
						(*arMin)[1] = arBeg[1];
						(*arMax)[0] = arBeg[0];
						(*arMax)[1] = arEnd[1];
					}
				}
				else											// Arc ends in quadrant four
					(*arMax)[0] = Max(arBeg[0], arEnd[0]);
			}
			else
			{
				if (arEnd[1] >= m_ptCenter[1])							// Arc ends in quadrant two
				{
					(*arMin)[0] = arEnd[0];
					(*arMin)[1] = Min(arBeg[1], arEnd[1]);
				}
				else											// Arc ends in quadrant three
					(*arMin)[1] = arEnd[1];
				(*arMax)[0] = arBeg[0];
			}
		}
		else													// Arc begins in quadrant four
		{
			if (arEnd[0] >= m_ptCenter[0]) 
			{
				if (arEnd[1] >= m_ptCenter[1])							// Arc ends in quadrant one
				{
					(*arMin)[0] = Min(arBeg[0], arEnd[0]);
					(*arMin)[1] = arBeg[1];
					(*arMax)[1] = arEnd[1];
				}
				else											// Arc ends in quadrant four
				{
					if (arBeg[0] < arEnd[0])						// Arc in qraudrant one only
					{
						(*arMin)[0] = arBeg[0];
						(*arMin)[1] = arBeg[1];
						(*arMax)[0] = arEnd[0];
						(*arMax)[1] = arEnd[1];
					}
				}
			}
			else
			{
				if (arEnd[1] >= m_ptCenter[1])							// Arc ends in quadrant two
				{
					(*arMin)[0] = arEnd[0];
					(*arMin)[1] = arBeg[1];
				}
				else											// Arc ends in quadrant three
					(*arMin)[1] = Min(arBeg[1], arEnd[1]);
			}
		}
	}
	else
	{
		if (arBeg[1] >= m_ptCenter[1])								// Arc begins in quadrant two
		{
			if (arEnd[0] >= m_ptCenter[0]) 
			{
				if (arEnd[1] >= m_ptCenter[1])							// Arc ends in quadrant one
					(*arMax)[1] = Max(arBeg[1], arEnd[1]);
				else											// Arc ends in quadrant four
				{
					(*arMax)[0] = arEnd[0];
					(*arMax)[1] = arBeg[1];
				}
			}
			else
			{
				if (arEnd[1] >= m_ptCenter[1])							// Arc ends in quadrant two
				{
					if (arBeg[0] > arEnd[0])						// Arc in qraudrant two only
					{
						(*arMin)[0] = arEnd[0];
						(*arMin)[1] = arEnd[1];
						(*arMax)[0] = arBeg[0];
						(*arMax)[1] = arBeg[1];
					}
				}
				else											// Arc ends in quadrant three
				{
					(*arMin)[1] = arEnd[1];
					(*arMax)[0] = Max(arBeg[0], arEnd[0]);
					(*arMax)[1] = arBeg[1];
				}
			}
		}
		else													// Arc begins in quadrant three
		{
			if (arEnd[0] >= m_ptCenter[0]) 
			{
				if (arEnd[1] >= m_ptCenter[1])							// Arc ends in quadrant one
					(*arMax)[1] = arEnd[1];
				else											// Arc ends in quadrant four
				{
					(*arMax)[0] = arEnd[0];
					(*arMax)[1] = Max(arBeg[1], arEnd[1]);
				}
				(*arMin)[0] = arBeg[0];
			}
			else
			{
				if (arEnd[1] >= m_ptCenter[1])							// Arc ends in quadrant two
					(*arMin)[0] = Min(arBeg[0], arEnd[0]);
				else											// Arc ends in quadrant three
				{
					if (arBeg[0] < arEnd[0])						// Arc in qraudrant three only
					{
						(*arMin)[0] = arBeg[0];
						(*arMin)[1] = arEnd[1];
						(*arMax)[0] = arEnd[0];
						(*arMax)[1] = arBeg[1];
					}
				}
			}
		}
	}
}

///<summary>Determines the extent. Actually the extents of the bounding region of the arc.</summary>
void CPrimArc::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
{
	CPnts ptsRegion;
	GetBoundingBox(ptsRegion);

	CPnt pt;

	for (WORD w = 0; w < 4; w++)
	{
		pt = ptsRegion[w];
		mspace.Transform(pt);
		pt = tm * pt;
		ptMin = Min(ptMin, pt);
		ptMax = Max(ptMax, pt);
	}
}

bool CPrimArc::IsPtACtrlPt(const CPnt4& ptPic) const
{
	// Determines if a point is on a control point of the arc.
	
	CPnt4 pt[] = {CPnt4(GetBegPt(), 1.), CPnt4(GetEndPt(), 1.)};
	
	for (WORD w = 0; w < 2; w++)
	{
		ModelView_DoTransform(pt[w]);

		if (Pnt4_DistanceTo_xy(ptPic, pt[w]) < mS_dPicApertSiz)
			return true;
	}
	return false;
}

int CPrimArc::IsWithinArea(const CPnt& ptLL, const CPnt& ptUR, CPnt* ptInt)
{
	CVec vPlnNorm = m_vMajAx ^ m_vMinAx;
	vPlnNorm.Normalize();
	
	if (!(ZDIR ^ vPlnNorm).IsNull())
		// not on plane normal to z-axis
		return 0;
		
	if (fabs(m_vMajAx.Length() - m_vMinAx.Length()) > FLT_EPSILON) 
		// not radial
		return 0;

	CPnt ptMin, ptMax;
		
	CPnt ptBeg = GetBegPt();
	CPnt ptEnd = GetEndPt();
//	CPnt ptEnd = (pFndPtOnArc(m_ptCenter, m_vMajAx, m_vMinAx, m_dSwpAng));
	
	if (vPlnNorm[2] < 0.) 
	{
		CPnt pt = ptBeg;
		ptBeg = ptEnd; ptEnd = pt;
		
		vPlnNorm = - vPlnNorm;
		m_vMajAx = CVec(m_ptCenter, ptBeg); 
		m_vMinAx = vPlnNorm ^ m_vMajAx;
	}
	
	GetXYExtents(ptBeg, ptEnd, &ptMin, &ptMax);
	
	if (ptMin[0] >= ptLL[0] && ptMax[0] <= ptUR[0] && ptMin[1] >= ptLL[1] && ptMax[1] <= ptUR[1]) 
	{	// Totally within window boundaries
		ptInt[0] = ptBeg;
		ptInt[1] = ptEnd;
		return (2);
	}
	if (ptMin[0] >= ptUR[0] || ptMax[0] <= ptLL[0] || ptMin[1] >= ptUR[1] || ptMax[1] <= ptLL[1])
		// No extent overlap
		return 0;
			
	CPnt ptWrk[8];

	double dDis, dOff;
	int iSecs = 0;
		
	double dRad = CVec(m_ptCenter, ptBeg).Length();
	if (ptMax[0] > ptUR[0])
	{	// Arc may intersect with right window boundary
		dDis = (double) (ptUR[0] - m_ptCenter[0]);
		dOff = sqrt(dRad * dRad - dDis * dDis);
		if (m_ptCenter[1] - dOff >= ptLL[1] && m_ptCenter[1] - dOff <= ptUR[1]) 
		{
			ptWrk[iSecs][0] = ptUR[0]; 
			ptWrk[iSecs++][1] = m_ptCenter[1] - dOff;
		}
		if (m_ptCenter[1] + dOff <= ptUR[1] && m_ptCenter[1] + dOff >= ptLL[1]) 
		{
			ptWrk[iSecs][0] = ptUR[0]; 
			ptWrk[iSecs++][1] = m_ptCenter[1] + dOff;
		}
	}
	if (ptMax[1] > ptUR[1]) 
	{	// Arc may intersect with top window boundary
		dDis = (double) (ptUR[1] - m_ptCenter[1]);
		dOff = sqrt(dRad * dRad - dDis * dDis);
		if (m_ptCenter[0] + dOff <= ptUR[0] && m_ptCenter[0] + dOff >= ptLL[0]) 
		{
			ptWrk[iSecs][0] = m_ptCenter[0] + dOff; 
			ptWrk[iSecs++][1] = ptUR[1];
		}
		if (m_ptCenter[0] - dOff >= ptLL[0] && m_ptCenter[0] - dOff <= ptUR[0]) 
		{	ptWrk[iSecs][0] = m_ptCenter[0] - dOff; 
			ptWrk[iSecs++][1] = ptUR[1];
		}
	}
	if (ptMin[0] < ptLL[0]) 
	{	// Arc may intersect with left window boundary
		dDis = (double) (m_ptCenter[0] - ptLL[0]);
		dOff = sqrt(dRad * dRad - dDis * dDis);
		if (m_ptCenter[1] + dOff <= ptUR[1] && m_ptCenter[1] + dOff >= ptLL[1]) 
		{	ptWrk[iSecs][0] = ptLL[0]; 
			ptWrk[iSecs++][1] = m_ptCenter[1] + dOff;
		}
		if (m_ptCenter[1] - dOff >= ptLL[1] && m_ptCenter[1] - dOff <= ptUR[1]) 
		{	ptWrk[iSecs][0] = ptLL[0]; 
			ptWrk[iSecs++][1] = m_ptCenter[1] - dOff;
		}
	}
	if (ptMin[1] < ptLL[1]) 
	{	// Arc may intersect with bottom window boundary
		dDis = (double) (m_ptCenter[1] - ptLL[1]);
		dOff = sqrt(dRad * dRad - dDis * dDis);
		if (m_ptCenter[0] - dOff >= ptLL[0] && m_ptCenter[0] - dOff <= ptUR[0]) 
		{	ptWrk[iSecs][0] = m_ptCenter[0] - dOff; 
			ptWrk[iSecs++][1] = ptLL[1];
		}
		if (m_ptCenter[0] + dOff <= ptUR[0] && m_ptCenter[0] + dOff >= ptLL[0]) 
		{	ptWrk[iSecs][0] = m_ptCenter[0] + dOff;
			ptWrk[iSecs++][1] = ptLL[1];
		}
	}
	if (iSecs == 0) 
		return 0;
		
	double dBegAng = atan2((double) (ptBeg[1] - m_ptCenter[1]), (double) (ptBeg[0] - m_ptCenter[0])); // Arc begin angle (- pi to pi)

	int i2;
		
	double dIntAng[8];
	double dWrkAng;
	int iInts = 0;
	for (i2 = 0; i2 < iSecs; i2++)		
	{	// Loop thru possible intersections
		dWrkAng = atan2((double) (ptWrk[i2][1] - m_ptCenter[1]), (double) (ptWrk[i2][0] - m_ptCenter[0])); // Current intersection angle (- pi to
		dIntAng[iInts] = dWrkAng - dBegAng; 					// Sweep from begin to intersection
		if (dIntAng[iInts] < 0.)
			dIntAng[iInts] += TWOPI;
		if (fabs(dIntAng[iInts]) - m_dSwpAng < 0.) 
		{	// Intersection lies on arc
			int i;
			for (i = 0; i < iInts && ptWrk[i2] != ptInt[i]; i++);
			if (i == iInts) 								// Unique intersection
				ptInt[iInts++] = ptWrk[i2];
		}
	}
	if (iInts == 0)
		// None of the intersections are on sweep of arc 
		return 0;
		
	for (int i1 = 0; i1 < iInts; i1++)
	{	// Sort intersections from begin to end of sweep
		for (i2 = 1; i2 < iInts - i1; i2++)
		{
			if (fabs(dIntAng[i2]) < fabs(dIntAng[i2 - 1])) 
			{
				double dAng = dIntAng[i2 - 1];
				dIntAng[i2 - 1] = dIntAng[i2];
				dIntAng[i2] = dAng;
				CPnt pt = ptInt[i2 - 1];
				ptInt[i2 - 1] = ptInt[i2];
				ptInt[i2] = pt;
			}
		}
	}
	//if (fabs(m_dSwpAng - TWOPI) <= DBL_EPSILON)
	//{ // Arc is a circle in disuise
	
	//}
	//else
	{	
		if (ptBeg[0] >= ptLL[0] && ptBeg[0] <= ptUR[0] && ptBeg[1] >= ptLL[1] && ptBeg[1] <= ptUR[1])
		{	// Add beg point to int set
			for (int i = iInts; i > 0; i--)
				ptInt[i] = ptInt[i - 1];	
			ptInt[0] = ptBeg; 
			iInts++;
		}
		if (ptEnd[0] >= ptLL[0] && ptEnd[0] <= ptUR[0] && ptEnd[1] >= ptLL[1] && ptEnd[1] <= ptUR[1])
		{	// Add end point to int set
			ptInt[iInts] = ptEnd; 
			iInts++;
		}
	}
	return (iInts);
}

CPnt CPrimArc::GoToNxtCtrlPt() const
{
	double dAng = (mS_dRel <= DBL_EPSILON) ? m_dSwpAng : 0.;
	return (pFndPtOnArc(m_ptCenter, m_vMajAx, m_vMinAx, dAng));
}

void CPrimArc::Iron()
{
	m_ptCenter[2] = 0; 
	m_vMajAx[2] = 0; 
	m_vMinAx[2] = 0;
}

bool CPrimArc::IsInView() const
{
	CPnts pts;

	GetBoundingBox(pts);
	
	CPnt4 ptBeg(pts[0], 1.);
	ModelView_DoTransform(ptBeg);
	
	for (WORD w = 1; w < 4; w++) 
	{
		CPnt4 ptEnd(pts[w], 1.);
		ModelView_DoTransform(ptEnd);
	
		if (Pnt4_ClipLine(ptBeg, ptEnd))
			return true;
		
		ptBeg = ptEnd;
	}
	return false;

	//return true;
}

void CPrimArc::ReadPeg(HFILE hFile)
{
	CPrim::ReadPeg(hFile);
	
	m_ptCenter.Read(hFile);
	m_vMajAx.Read(hFile);
	m_vMinAx.Read(hFile);
	FilePeg_ReadDouble2(hFile, m_dSwpAng);
}

///<summary>
///Determines the best control point on arc within specified tolerance.
///Control points for arcs are the points at start and end of the sweep.
///</summary>
CPnt CPrimArc::SelAtCtrlPt(const CPnt4& ptPic) const
{
	mS_wCtrlPt = USHRT_MAX;
	
	double dAPert = mS_dPicApertSiz;
	
	CPnt ptCtrl[] = {GetBegPt(), GetEndPt()};

	for (WORD w = 0; w < 2; w++) 
	{
		CPnt4 pt(ptCtrl[w], 1.);

		ModelView_DoTransform(pt);
		
		double dDis = Pnt4_DistanceTo_xy(ptPic, pt);
		
		if ( dDis < dAPert) 
		{
			mS_wCtrlPt = w;
			dAPert = dDis;
		}
	}
	return (mS_wCtrlPt == USHRT_MAX) ? ORIGIN : ptCtrl[mS_wCtrlPt];
}

///<summary>Determines if a line crosses arc.</summary>
bool CPrimArc::SelUsingLine(const CLine& ln, CPnts& ptsInt)
{	
	polyline::BeginLineStrip();
	GenPts(m_ptCenter, m_vMajAx, m_vMinAx, m_dSwpAng);
	return polyline::SelUsingLine(ln, ptsInt);
}

bool CPrimArc::SelUsingPoint(const CPnt4& ptPic, double dTol, CPnt& ptProj)
{
	polyline::BeginLineStrip();
	GenPts(m_ptCenter, m_vMajAx, m_vMinAx, m_dSwpAng);
	return (polyline::SelUsingPoint(ptPic, dTol, mS_dRel, ptProj));
}

bool CPrimArc::SelUsingRect(const CPnt& pt1, const CPnt& pt2)
{
	polyline::BeginLineStrip();
	GenPts(m_ptCenter, m_vMajAx, m_vMinAx, m_dSwpAng);
	return polyline::SelUsingRect(pt1, pt2);
}

double CPrimArc::SwpAngToPt(const CPnt& pt)
{
	CVec vPlnNorm = m_vMajAx ^ m_vMinAx;
	vPlnNorm.Normalize();
	
	CTMat tm(m_ptCenter, vPlnNorm);

	CPnt ptBeg = m_ptCenter + m_vMajAx;
	CPnt ptMid = pt;

	// Translate points into z=0 plane	  
	ptBeg = tm * ptBeg;
	ptMid = tm * ptMid;

	// determine if angle is greater than or less than PI
	if(vPlnNorm[2] <= 0)
		return (line::AngleBetweenLn_xy(CLine(ORIGIN, ptBeg), CLine(ORIGIN, ptMid)));
	else
		return (line::AngleBetweenLn_xy(CLine(ORIGIN, ptMid), CLine(ORIGIN, ptBeg)));			
}

void CPrimArc::Transform(const CTMat& tm)
{
	m_ptCenter = tm * m_ptCenter;
	m_vMajAx = tm * m_vMajAx;
	m_vMinAx = tm * m_vMinAx;
}

void CPrimArc::TranslateUsingMask(const CVec& v, const DWORD dwMask)
{
	if (dwMask != 0)
		m_ptCenter += v;
}

bool CPrimArc::Vectorize(const CSeg *pSeg)
{
	POSITION curPos = pSeg->Find(this);

	if(curPos == 0)
		return false;

	double dLen = Max(m_vMajAx.Length(), m_vMinAx.Length());
	int iPts = Max(2, abs(Round(m_dSwpAng / TWOPI * 32.)));	
	iPts = Min(128, Max(iPts, abs(Round(m_dSwpAng * dLen / .250))));

	CPnts pnts;
	pnts.SetSize(iPts);
	
	CTMat tm(m_ptCenter, m_vMajAx, m_vMinAx);
	tm.Inverse();
	
	double dAng = m_dSwpAng / (iPts - 1);
	double dCos = cos(dAng);
	double dSin = sin(dAng);
	
	CPnt pt(1., 0., 0.);
	
	for (int i = 0; i < iPts; i++) 
	{
		pnts.SetAt(i, tm * pt);
		pt(pt[0] * dCos - pt[1] * dSin, pt[1] * dCos + pt[0] * dSin, 0.);
	}

	for(int j = 0; j < iPts - 1; j++)
	{
		CPrimLine *pPrimLine = new CPrimLine(PenColor(), PenStyle(), pnts.GetAt(j), pnts.GetAt(j+1));
		((CObList*)pSeg)->InsertAfter(curPos, pPrimLine);
	}

	return true;
}

bool CPrimArc::WritePeg(HFILE hFile) const
{
	FilePeg_WriteWord2(hFile, PRIM_ARC);
	FilePeg_WriteWord2(hFile, m_nPenColor);
	FilePeg_WriteWord2(hFile, m_nPenStyle);
	m_ptCenter.Write(hFile);
	m_vMajAx.Write(hFile);
	m_vMinAx.Write(hFile);
	FilePeg_WriteDouble2(hFile, m_dSwpAng);

	return true;
}

CPnt pFndPtOnArc(const CPnt& ptCP, const CVec& vMajAx, const CVec& vMinAx, const double dAng) 
{
	CTMat tm(ptCP, vMajAx, vMinAx);
	tm.Inverse();

	CPnt pt(cos(dAng), sin(dAng), 0.);

	pt = tm * pt;
	return (pt);
}