#include "stdafx.h"

#include "PegAEsys.h"
//#include "PegaesysDoc.h"

#include "ddeGItms.h"
#include "Grid.h"
#include "Polyline.h"
#include "UserAxis.h"
#include "UnitsString.h"

#include "subprocnodal.h"

CPrimLine::CPrimLine(const CPnt& ptBeg, const CPnt& ptEnd)
{
	m_nPenColor = pstate.PenColor();
	m_nPenStyle = pstate.PenStyle();
	m_ln(ptBeg, ptEnd);
}

CPrimLine::CPrimLine(PENCOLOR nPenColor, PENSTYLE nPenStyle, const CLine& ln) : m_ln(ln)
{
	m_nPenColor = nPenColor;
	m_nPenStyle = nPenStyle;
}

CPrimLine::CPrimLine(PENCOLOR nPenColor, PENSTYLE nPenStyle, const CPnt& ptBeg, const CPnt& ptEnd)
{	
	m_nPenColor = nPenColor;
	m_nPenStyle = nPenStyle;
	m_ln(ptBeg, ptEnd);
} 

CPrimLine::CPrimLine(const CPrimLine& src)
{
	m_nPenColor = src.m_nPenColor;
	m_nPenStyle = src.m_nPenStyle;
	m_ln = src.m_ln;
}

const CPrimLine& CPrimLine::operator=(const CPrimLine& src)
{
	m_nPenColor = src.m_nPenColor;
	m_nPenStyle = src.m_nPenStyle;
	m_ln = src.m_ln;
	
	return (*this);
}

CPrim*& CPrimLine::Copy(CPrim*& pPrim) const
{
	pPrim = new CPrimLine(*this);
	return (pPrim);
}

///<summary>Cuts a line at two points.</summary>
// Notes:	Line segment between to points goes in pSegs.
void CPrimLine::CutAt2Pts(CPnt* pt, CSegs* pSegs, CSegs* pSegsNew)
{ 
	CPrimLine*	pLine;
	double	dRel[2];
		
	line::RelOfPtToEndPts(m_ln, pt[0], dRel[0]);
	line::RelOfPtToEndPts(m_ln, pt[1], dRel[1]);

	if (dRel[0] <= DBL_EPSILON && dRel[1] >= 1. - DBL_EPSILON)
	{	// Put entire line in trap
		pLine = this;
	}
	else
	{	// Something gets cut
		pLine = new CPrimLine(*this);
		if (dRel[0] > DBL_EPSILON && dRel[1] < 1. - DBL_EPSILON)
		{	// Cut section out of middle
			pLine->SetPt0(pt[1]);
			pSegs->AddTail(new CSeg(pLine));
			
			pLine = new CPrimLine(*this);
			pLine->SetPt0(pt[0]);
			pLine->SetPt1(pt[1]);
			SetPt1(pt[0]);
		}
		else if (dRel[1] < 1. - DBL_EPSILON)
		{	// Cut in two and place begin section in trap
			pLine->SetPt1(pt[1]);
			SetPt0(pt[1]);
		}
		else
		{	// Cut in two and place end section in trap
			pLine->SetPt0(pt[0]);
			SetPt1(pt[0]);
		}
		pSegs->AddTail(new CSeg(this));
	}
	pSegsNew->AddTail(new CSeg(pLine));
}

///<summary>Cuts a line a point.</summary>
void CPrimLine::CutAtPt(const CPnt& pt, CSeg* pSeg)
{
	CLine ln;

	if (m_ln.CutAtPt(pt, ln) != 0)
		pSeg->AddTail(new CPrimLine(m_nPenColor, m_nPenStyle, ln)); 
}

void CPrimLine::Display(CDC* pDC, bool bNodeDisplay) 
{
	PENCOLOR nPenColor = LogicalPenColor();
	PENSTYLE nPenStyle = LogicalPenStyle();
		
	pstate.SetPen(pDC, nPenColor, nPenStyle);

	polyline::BeginLineStrip();
	polyline::SetVertex(m_ln[0]);
	polyline::SetVertex(m_ln[1]);
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

void CPrimLine::DisplayOpenGL(bool)
{
//	PENCOLOR nPenColor = LogicalPenColor();
//	PENSTYLE nPenStyle = LogicalPenStyle();
		
//	opengl::SetCurrentColor(app.PenColorsGetHot(nPenColor));
}

void CPrimLine::DisRep(const CPnt& pt) const
{
	char szLength[64];
	UnitsString_FormatLength(szLength, app.GetUnits(), Length(), 16, 4);

	CString str;
	str.Format("<Line> Color: %s Style: %s - %s @ %f", 
		FormatPenColor(), FormatPenStyle(), szLength, GetAngAboutZAx() / RADIAN);
	msgInformation(str);

	double dLen = Length();
	double dAng = GetAngAboutZAx();
	
	double dRel;
	line::RelOfPtToEndPts(m_ln, pt, dRel);
	
	// Normalize line prior to angle determination
	if (dRel > .5)
		dAng += PI;
	
	dAng = fmod(dAng, TWOPI);
	app.SetEngLen(dLen);
	app.SetEngAngZ(dAng);

	//dde::PostAdvise(dde::EngLenInfo);
	//dde::PostAdvise(dde::EngAngZInfo);
}
void CPrimLine::FormatExtra(CString& str) const
{
	char szLength[64];
	UnitsString_FormatLength(szLength, app.GetUnits(), Length(), 16, 4);

	str.Format("Color;%s\tStyle;%s\tLength;%s\tZ-Angle;%f", 
		FormatPenColor(), FormatPenStyle(), szLength, GetAngAboutZAx() / RADIAN);
}
void CPrimLine::FormatGeometry(CString& str) const
{
	str += "Begin Point;" + m_ln[0].ToString();
	str += "End Point;" + m_ln[1].ToString();
}
void CPrimLine::GetAllPts(CPnts& pts)
{
	pts.SetSize(0);
	pts.Add(m_ln[0]);
	pts.Add(m_ln[1]);
}

///<summary>Determines the extent.</summary>
void CPrimLine::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
{	
	CPnt pt[2];
	
	GetPts(pt[0], pt[1]);

	for (WORD w = 0; w < 2; w++)
	{
		mspace.Transform(pt[w]);
		pt[w] = tm * pt[w];
		ptMin = Min(ptMin, pt[w]);
		ptMax = Max(ptMax, pt[w]);
	}
}

CPnt CPrimLine::GoToNxtCtrlPt() const
{
	if (mS_wCtrlPt == 0)
		mS_wCtrlPt = 1;
	else if (mS_wCtrlPt == 1)
		mS_wCtrlPt = 0;
	else
	{	// Initial rock .. jump to point at lower left or down if vertical
		CPnt ptBeg = Pt0();
		CPnt ptEnd = Pt1();
		
		if (ptEnd[0] > ptBeg[0])				
			mS_wCtrlPt = 0;
		else if (ptEnd[0] < ptBeg[0])
			mS_wCtrlPt = 1;
		else if (ptEnd[1] > ptBeg[1])
			mS_wCtrlPt = 0;
		else
			mS_wCtrlPt = 1;
	}
	return (mS_wCtrlPt == 0 ? Pt0() : Pt1());
}	

bool CPrimLine::IsInView() const
{
	// Test whether a line is wholly or partially within the current view volume.
	
	CPnt4 pt[] = {CPnt4(m_ln[0], 1.), CPnt4(m_ln[1], 1.)};
	ModelView_DoTransform(2, &pt[0]);

	return (Pnt4_ClipLine(pt[0], pt[1]));
}

///<summary>Determines if a line is identified by a point.</summary>
bool CPrimLine::IsPtACtrlPt(const CPnt4& ptPic) const
{
	CPnt4 pt;
	
	for (WORD w = 0; w < 2; w++)
	{
		pt = CPnt4(m_ln[w], 1.);
		ModelView_DoTransform(pt);
	
		if (Pnt4_DistanceTo_xy(ptPic, pt) < mS_dPicApertSiz)
			return true;
	}
	return false;
}
int CPrimLine::IsWithinArea(const CPnt& ptLL, const CPnt& ptUR, CPnt* ptInt)
{
	int i;
	int iLoc[2];

	GetPts(ptInt[0], ptInt[1]);

	for (i = 0; i < 2; i++)
	{
		iLoc[i] = p2DRelOfPtToWnd(ptInt[i], ptLL, ptUR);
	}
	while (iLoc[0] != 0 || iLoc[1] != 0)
	{
		if ((iLoc[0] & iLoc[1]) != 0) 
			return 0;
		
		i = (iLoc[0] != 0) ? 0 : 1;
		if ((iLoc[i] & 1) != 0) 								
		{	// Clip against top
			ptInt[i][0] = ptInt[i][0] + (ptInt[1][0] - ptInt[0][0]) * (ptUR[1] - ptInt[i][1]) / (ptInt[1][1] - ptInt[0][1]);
			ptInt[i][1] = ptUR[1];
		}
		else if ((iLoc[i] & 2) != 0)
		{	// Clip against bottom
			ptInt[i][0] = ptInt[i][0] + (ptInt[1][0] - ptInt[0][0]) * (ptLL[1] - ptInt[i][1]) / (ptInt[1][1] - ptInt[0][1]);
			ptInt[i][1] = ptLL[1];
		}
		else if ((iLoc[i] & 4) != 0)
		{	// Clip against right
			ptInt[i][1] = ptInt[i][1] + (ptInt[1][1] - ptInt[0][1]) * (ptUR[0] - ptInt[i][0]) / (ptInt[1][0] - ptInt[0][0]);
			ptInt[i][0] = ptUR[0];
		}
		else if ((iLoc[i] & 8) != 0)
		{	// Clip against left
			ptInt[i][1] = ptInt[i][1] + (ptInt[1][1] - ptInt[0][1]) * (ptLL[0] - ptInt[i][0]) / (ptInt[1][0] - ptInt[0][0]);
			ptInt[i][0] = ptLL[0];
		}
		iLoc[i] = p2DRelOfPtToWnd(ptInt[i], ptLL, ptUR);
	}
	return (2);
}

void CPrimLine::ReadPeg(HFILE hFile)
{
	CPrim::ReadPeg(hFile);
	m_ln.Read(hFile);
}

double CPrimLine::RelOfPt(const CPnt& pt) const 
{
	double dRel;
	line::RelOfPtToEndPts(m_ln, pt, dRel);
	return dRel;
}

CPnt CPrimLine::SelAtCtrlPt(const CPnt4& ptPic) const
{
	mS_wCtrlPt = USHRT_MAX;

	double dApert = mS_dPicApertSiz;

	for (WORD w = 0; w < 2; w++)
	{
		CPnt4 pt(m_ln[w], 1.);
		ModelView_DoTransform(pt);
	
		double dDis = Pnt4_DistanceTo_xy(ptPic, pt);
	
		if (dDis < dApert) 
		{
			mS_wCtrlPt = w;
			dApert = dDis;
		}
	}
	return (mS_wCtrlPt == USHRT_MAX) ? ORIGIN : m_ln[mS_wCtrlPt];
}

///<summary>Evaluates whether a line intersects line.</summary>
bool CPrimLine::SelUsingLine(const CLine& ln, CPnts& ptsInt)
{	
	polyline::BeginLineStrip();
	polyline::SetVertex(m_ln[0]);
	polyline::SetVertex(m_ln[1]);
	return polyline::SelUsingLine(ln, ptsInt);
}			

///<summary>Evaluates whether a point lies within tolerance specified of line.</summary>
bool CPrimLine::SelUsingPoint(const CPnt4& pt0, double dTol, CPnt& ptProj)
{	
	polyline::BeginLineStrip();
	polyline::SetVertex(m_ln[0]);
	polyline::SetVertex(m_ln[1]);
	return polyline::SelUsingPoint(pt0, dTol, mS_dRel, ptProj);
}	 

///<summary>
///Determines whether a line is partially or wholly within the area defined by the two points passed.
///</summary>
bool CPrimLine::SelUsingRect(const CPnt& pt1, const CPnt& pt2)
{
	polyline::BeginLineStrip();
	polyline::SetVertex(m_ln[0]);
	polyline::SetVertex(m_ln[1]);
	return polyline::SelUsingRect(pt1, pt2);
}

void CPrimLine::Square()
{
	CPnt ptBeg = grid::UserSnapPt(Pt0());
	CPnt ptEnd = grid::UserSnapPt(Pt1());
	
	CPnt pt = CLine(ptBeg, ptEnd).ProjPtAlong(.5);
	double dLen = CVec(ptBeg, ptEnd).Length();
	ptEnd = UserAxis::UserAxisSnapLn(pt, ptEnd);
	SetPt0(Pnt_ProjPtTo(ptEnd, pt, dLen));
	SetPt1(ptEnd);
}

void CPrimLine::TranslateUsingMask(const CVec& v, const DWORD dwMask)
{
	if ((dwMask & 1) == 1)
		SetPt0(Pt0() + v);
	
	if ((dwMask & 2) == 2)
		SetPt1(Pt1() + v);
}

bool CPrimLine::Vectorize(const CSeg *)
{
	return false;
}

bool CPrimLine::WritePeg(HFILE hFile) const
{
	FilePeg_WriteWord2(hFile, PRIM_LINE);

	FilePeg_WriteWord2(hFile, m_nPenColor);
	FilePeg_WriteWord2(hFile, m_nPenStyle);
	m_ln.Write(hFile);

	return true;
}