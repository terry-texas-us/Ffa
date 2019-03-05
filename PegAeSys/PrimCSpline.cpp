#include "stdafx.h"

#include "PegAEsys.h"

#include "Polyline.h"

#include "subprocnodal.h"

CPrimCSpline::CPrimCSpline(WORD nPtsS, WORD nEndCndId, CVec* vTan, CPnt* pts)
{
	m_nPenColor = pstate.PenColor();
	m_nPenStyle = pstate.PenStyle();

	m_wPtsS = nPtsS;
	m_wEndCndId = nEndCndId;
	m_TanVec[0] = vTan[0]; 
	m_TanVec[1] = vTan[1];

	for(WORD w = 0; w < nPtsS; w++)
		m_pts.Add(pts[w]);
}	

CPrimCSpline::CPrimCSpline(const CPrimCSpline& src)
{
	m_nPenColor = src.m_nPenColor;
	m_nPenStyle = src.m_nPenStyle;
	m_wPtsS = src.m_wPtsS;
	m_wEndCndId = src.m_wEndCndId;
	m_TanVec[0] = src.m_TanVec[0]; 
	m_TanVec[1] = src.m_TanVec[1];
	m_pts.Copy(src.m_pts);
}

const CPrimCSpline& CPrimCSpline::operator=(const CPrimCSpline& src)
{
	m_nPenColor = src.m_nPenColor;
	m_nPenStyle = src.m_nPenStyle;
	m_wPtsS = src.m_wPtsS;
	m_wEndCndId = src.m_wEndCndId;
	m_TanVec[0] = src.m_TanVec[0]; 
	m_TanVec[1] = src.m_TanVec[1];
	m_pts.Copy(src.m_pts);	
	
	return (*this);
}

CPrim*& CPrimCSpline::Copy(CPrim*& pPrim) const
{
	pPrim = new CPrimCSpline(*this);
	return (pPrim);
}

void CPrimCSpline::Display(CDC* pDC, bool bNodeDisplay)
{
	PENCOLOR nPenColor = LogicalPenColor();
	PENSTYLE nPenStyle = LogicalPenStyle();

	int pStateSave = pstate.Save();
	pstate.SetPen(pDC, nPenColor, nPenStyle);

	CPnts ptsCoords;
	//ptsCoords.SetSize(m_pts.GetSize());
	for(int j = 0; j < m_pts.GetSize(); j++) 
		ptsCoords.Add(m_pts[j]);

	if(ptsCoords.GetSize() >= 4)
	{
		polyline::BeginLineStrip();

		while(ptsCoords.GetSize() >= 4)				//While there are atleast 4 entries in the list
		{
			//polyline::BeginLineStrip();
			for(int x = 0; x <= 12; x++)			// iterate from 0 to 1 in 500 steps
			{
				CPnt coord = EvaluateCardinal2D(ptsCoords, .5, x * (1/12));
				polyline::SetVertex(coord);
			}
			ptsCoords.RemoveAt(0);					// Remove the first entry of the list
			//polyline::End(pDC, nPenStyle);
		}

		polyline::End(pDC, nPenStyle);
	}

	pstate.Restore(pDC, pStateSave);

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

CPnt CPrimCSpline::EvaluateCardinal2D(CPnts& ptsCoords, double dTension, int nStep)
{
	double s = (1 - dTension) / 2;
	double u = nStep;

	double UMC0 = ((-s)*u*u*u) + (2*s*u*u) - (s*u);
	double UMC1 = ((2-s)*u*u*u) + ((s-3)*u*u) + 1;
	double UMC2 = ((s-2)*u*u*u) + ((3-(2*s))*u*u) + (s*u);
	double UMC3 = (s*u*u*u) - (s*u*u);

	double x = (UMC0 * ptsCoords[0][0]) + (UMC1 * ptsCoords[1][0]) + (UMC2 * ptsCoords[2][0]) + (UMC3 * ptsCoords[3][0]);
	double y = (UMC0 * ptsCoords[0][1]) + (UMC1 * ptsCoords[1][1]) + (UMC2 * ptsCoords[2][1]) + (UMC3 * ptsCoords[3][1]);
	
	return CPnt(x, y, 0);
}

void CPrimCSpline::DisplayOpenGL(bool)
{
	//PENCOLOR nPenColor = LogicalPenColor();
	//opengl::SetCurrentColor(app.PenColorsGetHot(nPenColor));
	//opengl::BeginLineStrip();

	//for (WORD w = 0; w < m_pts.GetSize(); w++)
	//{
	//	opengl::SetVertex(m_pts[w]);
	//}
	//opengl::End();
}

void CPrimCSpline::FormatGeometry(CString& str) const
{
	for (WORD w = 0; w < m_pts.GetSize(); w++)
	{
		str += "Control Point;" + m_pts[w].ToString();
	}
}

void CPrimCSpline::FormatExtra(CString& str) const
{
	str.Format("Color;%s\tStyle;%s\tControl Points;%d", FormatPenColor(), FormatPenStyle(), m_pts.GetSize());
}

CPnt CPrimCSpline::GetCtrlPt() const
{
	CPnt pt;
	pt = m_pts[m_pts.GetSize() / 2];
	return (pt);
}

///<summary>Determines the extent.</summary>
void CPrimCSpline::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
{
	CPnt pt;

	for (WORD w = 0; w < m_pts.GetSize(); w++)
	{
		pt = m_pts[w];
		mspace.Transform(pt);
		pt = tm * pt;
		ptMin = Min(ptMin, pt);
		ptMax = Max(ptMax, pt);
	}
}

CPnt CPrimCSpline::GoToNxtCtrlPt() const
{
	CPnt pt;

	int i = m_pts.GetSize() - 1;
	
	if (mS_dRel <= DBL_EPSILON)
		pt = m_pts[i];
	else if (mS_dRel >= 1. - DBL_EPSILON)
		pt = m_pts[0];
	else if (m_pts[i][0] > m_pts[0][0])
		pt = m_pts[0];
	else if (m_pts[i][0] < m_pts[0][0])
		pt = m_pts[i];
	else if (m_pts[i][1] > m_pts[0][1])
		pt = m_pts[0];
	else														
		pt = m_pts[i];

	return (pt);
}

void CPrimCSpline::Iron()
{
	for(int i = 0; i < m_pts.GetSize(); i++)
		m_pts[i][2] = 0;
}

bool CPrimCSpline::IsInView() const
{
	CPnt4 pt[2];
	
	pt[0] = m_pts[0];
	ModelView_DoTransform(pt[0]);

	for (WORD w = 1; w < m_pts.GetSize(); w++) 
	{
		pt[1] = m_pts[w];
		ModelView_DoTransform(pt[1]);
	
		if (Pnt4_ClipLine(pt[0], pt[1]))
			return true;
		
		pt[0] = pt[1];
	}

	return false;
}

bool CPrimCSpline::SelUsingRect(const CPnt& pt1, const CPnt& pt2)
{
	return polyline::SelUsingRect(pt1, pt2, m_pts);
}

void CPrimCSpline::ReadPeg(HFILE hFile)
{
	CPrim::ReadPeg(hFile);
	
	FilePeg_ReadWord2(hFile, m_wPtsS);
	WORD wPts;
	FilePeg_ReadWord2(hFile, wPts);
	FilePeg_ReadWord2(hFile, m_wEndCndId);

	m_TanVec[0].Read(hFile);
	m_TanVec[1].Read(hFile);
	
	CPnt pt;
	for (WORD w = 0; w < wPts; w++)
	{
		pt.Read(hFile);
		m_pts.Add(pt);
	}
}

CPnt CPrimCSpline::SelAtCtrlPt(const CPnt4& ptPicVw) const
{
	mS_wCtrlPt = USHRT_MAX;
	return (ptPicVw);
}

void CPrimCSpline::Transform(const CTMat& tm)
{
	for (WORD w = 0; w < m_pts.GetSize(); w++)
		m_pts[w] = tm * m_pts[w];
	
	if (m_wEndCndId == 1) 
	{
		m_TanVec[0] = tm * m_TanVec[0];
		m_TanVec[1] = tm * m_TanVec[1];
	}
}

void CPrimCSpline::Translate(const CVec& v)
{
	for (WORD w = 0; w < m_pts.GetSize(); w++) 
		m_pts[w] += v;
}

void CPrimCSpline::TranslateUsingMask(const CVec& v, const DWORD dwMask)
{
	for (WORD w = 0; w < m_pts.GetSize(); w++) 
		if (((dwMask >> w) & 1UL) == 1)
			m_pts[w] += v;
}

bool CPrimCSpline::Vectorize(const CSeg *)
{
	return false;
}

bool CPrimCSpline::WritePeg(HFILE hFile) const
{
	FilePeg_WriteWord2(hFile, PRIM_CSPLINE);
	FilePeg_WriteWord2(hFile, m_nPenColor);
	FilePeg_WriteWord2(hFile, m_nPenStyle);
	FilePeg_WriteWord2(hFile, m_wPtsS);
	FilePeg_WriteWord2(hFile, (WORD) m_pts.GetSize());
	FilePeg_WriteWord2(hFile, m_wEndCndId);

	m_TanVec[0].Write(hFile);
	m_TanVec[1].Write(hFile);

	for (WORD w = 0; w < m_pts.GetSize(); w++)
		m_pts[w].Write(hFile);

	return true;
}