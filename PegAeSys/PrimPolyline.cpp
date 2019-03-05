#include "stdafx.h"

#include "PegAEsys.h"

#include "ddeGItms.h"
#include "Polyline.h"
#include "UnitsString.h"

#include "subprocnodal.h"

WORD CPrimPolyline::mS_wEdgeToEvaluate = 0;
WORD CPrimPolyline::mS_wEdge = 0;
WORD CPrimPolyline::mS_wPivotVertex = 0;

CPrimPolyline::CPrimPolyline()
{
	m_wFlags = 0;
}

CPrimPolyline::CPrimPolyline(CPnts& pts)
{
	m_nPenColor = pstate.PenColor();
	m_nPenStyle = pstate.PenStyle();
	
	m_wFlags = 0;
	m_pts.Copy(pts);
}

CPrimPolyline::CPrimPolyline(WORD wPts, CPnt* pPts)
{
	m_nPenColor = pstate.PenColor();
	m_nPenStyle = pstate.PenStyle();
	
	m_pts.SetSize(wPts);

	m_wFlags = 0;
	for (WORD w = 0; w < wPts; w++)
	{
		m_pts[w] = pPts[w];
	}
}

CPrimPolyline::CPrimPolyline(const CPrimPolyline& src)
{
	m_nPenColor = src.m_nPenColor;
	m_nPenStyle = src.m_nPenStyle;
	m_wFlags = src.m_wFlags;
	m_pts.Copy(src.m_pts);
}

const CPrimPolyline& CPrimPolyline::operator=(const CPrimPolyline& src)
{
	m_nPenColor = src.m_nPenColor;
	m_nPenStyle = src.m_nPenStyle;
	m_wFlags = src.m_wFlags;
	m_pts.Copy(src.m_pts);
	return (*this);
}

CPrim*& CPrimPolyline::Copy(CPrim*& pPrim) const
{
	pPrim = new CPrimPolyline(*this);
	return (pPrim);
}

void CPrimPolyline::Display(CDC* pDC, bool bNodeDisplay)
{
	pstate.SetPen(pDC, m_nPenColor, m_nPenStyle);
	
	if(IsLooped())
		polyline::DisplayLoop(pDC, m_pts);
	else
		polyline::Display(pDC, m_pts);

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

void CPrimPolyline::DisplayOpenGL(bool)
{
	//PENCOLOR nPenColor = LogicalPenColor();
	//opengl::SetCurrentColor(app.PenColorsGetHot(nPenColor));

	//if(IsLooped() || IsLooped2())
	//	opengl::BeginLineLoop();
	//else
	//	opengl::BeginLineStrip();
	//
	//for (WORD w = 0; w < m_pts.GetSize(); w++)
	//{
	//	opengl::SetVertex(m_pts[w]);
	//}
	//opengl::End();
}

void CPrimPolyline::DisRep(const CPnt& ptPic) const
{
	CString strRep = "<Polyline Edge> ";

	WORD wPts = WORD(m_pts.GetSize());

	if (mS_wEdge > 0 && mS_wEdge <= wPts)
	{
		CPnt ptBeg = m_pts[mS_wEdge - 1];
		CPnt ptEnd = m_pts[mS_wEdge % wPts];

		if (mS_wPivotVertex < wPts)
		{
			ptBeg = m_pts[mS_wPivotVertex];
			ptEnd = m_pts[SwingVertex()];
		}
		double dAng;
		double dLen = CVec(ptBeg, ptEnd).Length();						// Length of edge
		
		if (CVec(ptPic, ptBeg).Length() > dLen * .5)
			dAng = line::GetAngAboutZAx(CLine(ptEnd, ptBeg));
		else
			dAng = line::GetAngAboutZAx(CLine(ptBeg, ptEnd));
		
		char szBuf[24];
		UnitsString_FormatLength(szBuf, app.GetUnits(), dLen, 16, 4);
		strRep += szBuf;
		sprintf(szBuf, " @ %6.2f degrees", dAng / RADIAN);
		strRep += szBuf;	
		msgInformation(strRep);
		
		app.SetEngLen(dLen);
		app.SetEngAngZ(dAng);
		//dde::PostAdvise(dde::EngLenInfo);
		//dde::PostAdvise(dde::EngAngZInfo);
	}
}
void CPrimPolyline::FormatGeometry(CString& str) const
{
	for (WORD w = 0; w < m_pts.GetSize(); w++)
	{
		str += "Point;" + m_pts[w].ToString();
	}
}
void CPrimPolyline::FormatExtra(CString& str) const
{
	str.Format("Color;%s\tStyle;%s\tPoints;%d", FormatPenColor(), FormatPenStyle(), m_pts.GetSize());
}
CPnt CPrimPolyline::GetCtrlPt() const 
{
	WORD wPts = WORD(m_pts.GetSize());
	WORD wBeg = WORD(mS_wEdge - 1);
	WORD wEnd = WORD(mS_wEdge % wPts);
	CPnt pt = CLine(m_pts[wBeg], m_pts[wEnd]).ProjPtAlong(.5);
	return (pt);
}

///<summary>Determines the extent.</summary>
void CPrimPolyline::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
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

CPnt CPrimPolyline::GoToNxtCtrlPt() const
{
	WORD wPts = WORD(m_pts.GetSize());
	
	if (mS_wPivotVertex >= wPts)
	{	// have not yet rocked to a vertex
		WORD wBeg = WORD(mS_wEdge - 1);
		WORD wEnd = WORD(mS_wEdge % wPts);

		if (m_pts[wEnd][0] > m_pts[wBeg][0])
			mS_wPivotVertex = wBeg;
		else if (m_pts[wEnd][0] < m_pts[wBeg][0])
			mS_wPivotVertex = wEnd;
		else if (m_pts[wEnd][1] > m_pts[wBeg][1])
			mS_wPivotVertex = wBeg;
		else
			mS_wPivotVertex = wEnd;
	}
	else if (mS_wPivotVertex == 0)
	{
		if (mS_wEdge == 1)
			mS_wPivotVertex = 1;
		else
			mS_wPivotVertex = wPts - 1;
	}
	else if (mS_wPivotVertex == wPts - 1)										
	{
		if (mS_wEdge == wPts)
			mS_wPivotVertex = 0;
		else
			mS_wPivotVertex--;
	}
	else
	{
		if (mS_wEdge == mS_wPivotVertex)
			mS_wPivotVertex--;
		else
			mS_wPivotVertex++;
	}
	return (m_pts[mS_wPivotVertex]);
}

void CPrimPolyline::Iron()
{
	for(int i = 0; i < m_pts.GetSize(); i++)
		m_pts[i][2] = 0;
}

bool CPrimPolyline::IsInView() const
{
	CPnt4	pt[2];
	
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

bool CPrimPolyline::PvtOnCtrlPt(const CPnt4& ptView) const
{
	WORD wPts = WORD(m_pts.GetSize());
	
	if (mS_wPivotVertex >= wPts)
		// Not engaged at a vertex
		return false;
	
	CPnt4 ptCtrl(m_pts[mS_wPivotVertex], 1.);
	ModelView_DoTransform(ptCtrl);
	
	if (Pnt4_DistanceTo_xy(ptCtrl, ptView) >= mS_dPicApertSiz)		
		// Not on proper vertex
		return false;

	if (mS_wPivotVertex == 0)					
		mS_wEdge = WORD(mS_wEdge == 1 ? wPts : 1); 
	else if (mS_wPivotVertex == wPts - 1)
		mS_wEdge = WORD(mS_wEdge == wPts ? mS_wEdge - 1 : wPts);
	else if (mS_wPivotVertex == mS_wEdge) 
		mS_wEdge++;
	else 
		mS_wEdge--;
	
	return true;
}	
CPnt CPrimPolyline::SelAtCtrlPt(const CPnt4& ptPic) const
{
	WORD wPts = WORD(m_pts.GetSize());
	
	mS_wCtrlPt = USHRT_MAX;
	double dApert = mS_dPicApertSiz;
	
	mS_wPivotVertex = wPts;

	for (WORD w = 0; w < wPts; w++) 
	{
		CPnt4 pt(m_pts[w], 1.);
		ModelView_DoTransform(pt);
		
		double dDis = Pnt4_DistanceTo_xy(ptPic, pt);
		
		if (dDis < dApert) 
		{
			mS_wCtrlPt = w;
			dApert = dDis;
	
			mS_wEdge = WORD(w + 1); 
			mS_wPivotVertex = w;
		}
	}
	return (mS_wCtrlPt == USHRT_MAX) ? ORIGIN : m_pts[mS_wCtrlPt];
}

bool CPrimPolyline::SelUsingPoint(const CPnt4& pt, double dTol, CPnt& ptProj)
{
	double dProjDis;
	
	WORD wPts = WORD(m_pts.GetSize());
	if (mS_wEdgeToEvaluate > 0 && mS_wEdgeToEvaluate <= wPts)
	{	// Evaluate specified edge of polyline
		CPnt4 ptBeg(m_pts[mS_wEdgeToEvaluate - 1], 1.);
		CPnt4 ptEnd(m_pts[mS_wEdgeToEvaluate % wPts], 1.);

		ModelView_DoTransform(ptBeg);
		ModelView_DoTransform(ptEnd);

		if (line::EvalPtProx_xy(CLine(ptBeg, ptEnd), pt, dTol, ptProj, &mS_dRel, &dProjDis)) 
		{
			ptProj[2] = ptBeg[2] + mS_dRel * (ptEnd[2] - ptBeg[2]);
			return true;
		}
	}
	else
	{	// Evaluate entire polyline
		WORD wEdges = (WORD) m_pts.GetSize();
		if(!IsLooped())
			wEdges--;

		CPnt4 ptBeg(m_pts[0], 1.);
		ModelView_DoTransform(ptBeg);

		for(WORD w = 1; w <= wEdges; w++) 
		{
			CPnt4 ptEnd(m_pts[w % wPts], 1.);
			ModelView_DoTransform(ptEnd);
				
			if(line::EvalPtProx_xy(CLine(ptBeg, ptEnd), pt, dTol, ptProj, &mS_dRel, &dProjDis)) 
			{
				ptProj[2] = ptBeg[2] + mS_dRel * (ptEnd[2] - ptBeg[2]);
				mS_wEdge = w;
				mS_wPivotVertex = wPts;
				return true;
			}
			ptBeg = ptEnd;
		}
	}
	return false;
}

bool CPrimPolyline::SelUsingRect(const CPnt& pt1, const CPnt& pt2)
{
	return polyline::SelUsingRect(pt1, pt2, m_pts);
}

void CPrimPolyline::ReadPeg(HFILE hFile)
{
	CPrim::ReadPeg(hFile);
	WORD wPts;
	FilePeg_ReadWord2(hFile, wPts);
	
	CPnt pt;
	for (WORD w = 0; w < wPts; w++)
	{
		pt.Read(hFile);
		m_pts.Add(pt);
	}
}

void CPrimPolyline::Transform(const CTMat& tm)
{
	for (WORD w = 0; w < m_pts.GetSize(); w++)
		m_pts[w] = tm * m_pts[w];
}
void CPrimPolyline::Translate(const CVec& v)
{
	for (WORD w = 0; w < m_pts.GetSize(); w++) 
		m_pts[w] += v;
}
void CPrimPolyline::TranslateUsingMask(const CVec& v, const DWORD dwMask)
{
	for (WORD w = 0; w < m_pts.GetSize(); w++) 
		if (((dwMask >> w) & 1UL) == 1)
			m_pts[w] += v;
}

bool CPrimPolyline::Vectorize(const CSeg *pSeg)
{
	POSITION curPos = pSeg->Find(this);

	if(curPos == 0)
		return false;

	for(int j = 0; j < m_pts.GetSize() - 1; j++)
	{
		CPrimLine *pPrimLine = new CPrimLine(PenColor(), PenStyle(), m_pts[j], m_pts[j+1]);
		((CObList*)pSeg)->InsertAfter(curPos, pPrimLine);
	}

	// add final line if closed polyline
	if(IsLooped())
	{
		CPrimLine *pPrimLine = new CPrimLine(PenColor(), PenStyle(), m_pts[m_pts.GetSize() - 1], m_pts[0]);
		((CObList*)pSeg)->InsertAfter(curPos, pPrimLine);
	}

	return true;
}

bool CPrimPolyline::WritePeg(HFILE hFile) const
{
	FilePeg_WriteWord2(hFile, PRIM_POLYLINE);
	FilePeg_WriteWord2(hFile, m_nPenColor);
	FilePeg_WriteWord2(hFile, m_nPenStyle);
	FilePeg_WriteWord2(hFile, (WORD) m_pts.GetSize());

	for (WORD w = 0; w < m_pts.GetSize(); w++)
		m_pts[w].Write(hFile);

	return true;
}

WORD CPrimPolyline::SwingVertex() const
{
	WORD wPts = WORD(m_pts.GetSize());
	
	WORD wSwingVertex;
	
	if (mS_wPivotVertex == 0)
		wSwingVertex = WORD(mS_wEdge == 1 ? 1 : wPts - 1); 
	else if (mS_wPivotVertex == WORD(wPts - 1))										
		wSwingVertex = WORD(mS_wEdge == wPts ? 0 : mS_wPivotVertex - 1);
	else
		wSwingVertex = WORD(mS_wEdge == mS_wPivotVertex ? mS_wPivotVertex - 1 : mS_wPivotVertex + 1);
	
	return (wSwingVertex);
}