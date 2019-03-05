#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysView.h"

#include "ddeGItms.h"
#include "Polyline.h"
#include "Text.h"
#include "UnitsString.h"

#include "subprocnodal.h"

WORD CPrimDim::mS_wFlags = 0;

CPrimDim::CPrimDim(PENCOLOR nPenColor, PENSTYLE nPenStyle, const CLine& ln) : m_ln(ln)
{
	m_nPenColor = nPenColor;
	m_nPenStyle = nPenStyle;
	
	m_nTextPenColor = 5;
	pstate.GetFontDef(m_fd);
	SetDefaultNote();
}
CPrimDim::CPrimDim(const CPrimDim& src) 
{
	m_nPenColor = src.m_nPenColor;
	m_nPenStyle = src.m_nPenStyle;
	m_ln = src.m_ln;
    
	m_nTextPenColor = src.m_nTextPenColor;
	m_fd = src.m_fd;
	m_rs = src.m_rs;
	m_strText = src.m_strText;
}
const CPrimDim& CPrimDim::operator=(const CPrimDim& src)
{
	m_nPenColor = src.m_nPenColor;
	m_nPenStyle = src.m_nPenStyle;
	m_ln = src.m_ln;
	
	m_nTextPenColor = src.m_nTextPenColor;
    m_fd = src.m_fd;
	m_rs = src.m_rs;
	m_strText = src.m_strText;
	
	return (*this);
}

CPrim*& CPrimDim::Copy(CPrim*& pPrim) const
{
	pPrim = new CPrimDim(*this);
	return (pPrim);
}
///<summary>Cuts a dimension line at two points.</summary>
// Parameters:	pt	
//				pSeg	segment to place optional line not defined by the cut 
//						points
//				pSegNew segment to place line defined by the cut points
void CPrimDim::CutAt2Pts(CPnt* pt, CSegs* pSegs, CSegs* pSegsNew)
{ 
	CPrimDim*	pDim;
	double	dRel[2];
		
	line::RelOfPtToEndPts(m_ln, pt[0], dRel[0]);
	line::RelOfPtToEndPts(m_ln, pt[1], dRel[1]);

	if (dRel[0] <= DBL_EPSILON && dRel[1] >= 1. - DBL_EPSILON)
		// Put entire dimension in trap
		pDim = this;
	else
	{	// Something gets cut
		pDim = new CPrimDim(*this);
		if (dRel[0] > DBL_EPSILON && dRel[1] < 1. - DBL_EPSILON)
		{	// Cut section out of middle
			pDim->SetPt0(pt[1]);
			pDim->SetDefaultNote();
			
			pSegs->AddTail(new CSeg(pDim));
			
			pDim = new CPrimDim(*this);
			pDim->SetPt0(pt[0]);
			pDim->SetPt1(pt[1]);
			pDim->SetDefaultNote();
			m_ln[1] = pt[0];
		}
		else if (dRel[1] < 1. - DBL_EPSILON)
		{	// Cut in two and place begin section in trap
			pDim->SetPt1(pt[1]);
			pDim->SetDefaultNote();
			m_ln[0] = pt[1];
		}
		else
		{	// Cut in two and place end section in trap
			pDim->SetPt0(pt[0]);
			pDim->SetDefaultNote();
			m_ln[1] = pt[0];
		}
		SetDefaultNote();
		pSegs->AddTail(new CSeg(this));
	}
	pSegsNew->AddTail(new CSeg(pDim));
}

void CPrimDim::CutAtPt(const CPnt& pt, CSeg* pSeg)
{
	CLine ln;

	if (m_ln.CutAtPt(pt, ln) != 0)
	{
		CPrimDim* pDim = new CPrimDim(*this);

		pDim->m_ln = ln;
		pDim->SetDefaultNote();
		pSeg->AddTail(pDim);
	}
	SetDefaultNote();
}	 

void CPrimDim::Display(CDC* pDC, bool bNodeDisplay)
{
	PENCOLOR nPenColor = LogicalPenColor();

	pstate.SetPen(pDC, nPenColor, LogicalPenStyle());
	m_ln.Display(pDC);

	pstate.SetPenColor(m_nTextPenColor);
		
	PENSTYLE nPenStyle = pstate.PenStyle();
	//pstate.SetPenStyle(1);
	pstate.SetPen(pDC, pstate.PenColor(), nPenStyle);
		
	text_Display0(pDC, m_fd, m_rs, m_strText);

	//pstate.SetPenStyle(nPenStyle);
	pstate.SetPen(pDC, pstate.PenColor(), nPenStyle);

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

void CPrimDim::DisplayOpenGL(bool )//bNodeDisplay)
{
	//PENCOLOR nPenColor = LogicalPenColor();

	//opengl::SetCurrentColor(app.PenColorsGetHot(nPenColor));
	//opengl::BeginLineStrip();
	//	opengl::SetVertex(m_ln[0]);
	//	opengl::SetVertex(m_ln[1]);
	//opengl::End();

	//opengl::SetCurrentColor(app.PenColorsGetHot(m_nTextPenColor));
	//text_Display0(0, m_fd, m_rs, m_strText);
}

void CPrimDim::DisRep(const CPnt& pt) const
{
	//char szBuf[16];
	//CString str2 = _itoa(Length(), szBuf, 10);

	CString str;
	str.Format("<Dim> Color: %s Style: %s Length: %s", FormatPenColor(), FormatPenStyle(), Text());
	msgInformation(str);

	double dLen = Length();
	double dAng = m_ln.GetAngAboutZAx();

	double dRel;
	line::RelOfPtToEndPts(m_ln, pt, dRel);
	
	if (dRel > .5)
		// Normalize line prior to angle determination
		dAng += PI;
	
	dAng = fmod(dAng, TWOPI);
	app.SetEngLen(dLen);
	app.SetEngAngZ(dAng);
	//dde::PostAdvise(dde::EngLenInfo);
	//dde::PostAdvise(dde::EngAngZInfo);
}
void CPrimDim::FormatExtra(CString& str) const
{
	str.Format("Color;%s\tStyle;%s", FormatPenColor(), FormatPenStyle());
}
void CPrimDim::FormatGeometry(CString& str) const
{
	str += "Begin Point;" + m_ln[0].ToString();
	str += "End Point;" + m_ln[1].ToString();
}
void CPrimDim::GetAllPts(CPnts& pts)
{
	pts.SetSize(0);
	pts.Add(m_ln[0]);
	pts.Add(m_ln[1]);
}
// Determination of text extent.
void CPrimDim::GetBoundingBox(CPnts& ptsBox, double dSpacFac) const
{
	text_GetBoundingBox(m_fd, m_rs, m_strText.GetLength(), dSpacFac, ptsBox);
}

///<summary>Determines the extent.</summary>
void CPrimDim::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
{	
	CPnt pt[2] = {m_ln[0], m_ln[1]};
	
	for (WORD w = 0; w < 2; w++)
	{
		mspace.Transform(pt[w]);
		pt[w] = tm * pt[w];
		ptMin = Min(ptMin, pt[w]);
		ptMax = Max(ptMax, pt[w]);
	}
}

CPnt CPrimDim::GoToNxtCtrlPt() const
{
	if (mS_wCtrlPt == 0)
		mS_wCtrlPt = 1;
	else if (mS_wCtrlPt == 1)
		mS_wCtrlPt = 0;
	else
	{	// Initial rock .. jump to point at lower left or down if vertical
		CPnt ptBeg = m_ln[0];
		CPnt ptEnd = m_ln[1];
		
		if (ptEnd[0] > ptBeg[0])				
			mS_wCtrlPt = 0;
		else if (ptEnd[0] < ptBeg[0])
			mS_wCtrlPt = 1;
		else if (ptEnd[1] > ptBeg[1])
			mS_wCtrlPt = 0;
		else
			mS_wCtrlPt = 1;
	}
	return (mS_wCtrlPt == 0 ? m_ln[0] : m_ln[1]);
}	

void CPrimDim::Iron()
{
	m_ln[0][2] = 0; 
	m_ln[1][2] = 0; 

	CPnt pnt_ = m_rs.Origin(); 
	pnt_[2] = 0;
	m_rs.SetOrigin(pnt_);

	CVec xDir = m_rs.DirX();
	CVec yDir = m_rs.DirY();
	xDir[2] = 0;
	yDir[2] = 0;
	m_rs.SetDirX(xDir);
	m_rs.SetDirY(yDir);
}

bool CPrimDim::IsInView() const
{
	CPnt4 pt[] = {CPnt4(m_ln[0], 1.), CPnt4(m_ln[1], 1.)};
		
	ModelView_DoTransform(2, &pt[0]);

	return (Pnt4_ClipLine(pt[0], pt[1]));
}

bool CPrimDim::IsPtACtrlPt(const CPnt4& ptPic) const
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
void CPrimDim::ModifyState()
{
	if ((mS_wFlags & 0x0001) != 0)
		CPrim::ModifyState();
	
	if ((mS_wFlags & 0x0002) != 0)
		pstate.GetFontDef(m_fd);
}

void CPrimDim::GetState()
{
	if ((mS_wFlags & 0x0001) != 0)
		CPrim::GetState();

	if ((mS_wFlags & 0x0002) != 0)
		pstate.SetFontDef(CPegView::GetActiveView()->GetDC(), m_fd);
}

void CPrimDim::ReadPeg(HFILE hFile) 
{
	CPrim::ReadPeg(hFile);
	m_ln.Read(hFile);

	FilePeg_ReadShort2(hFile, m_nTextPenColor);
	m_fd.Read(hFile);
	m_rs.Read(hFile);
	FilePeg_ReadString2(hFile, m_strText);
}

double CPrimDim::RelOfPt(const CPnt& pt) const 
{
	double dRel;
	line::RelOfPtToEndPts(m_ln, pt, dRel);
	return dRel;
}
CPnt CPrimDim::SelAtCtrlPt(const CPnt4& ptPic) const
{
	mS_wCtrlPt = USHRT_MAX;

	double dAPert = mS_dPicApertSiz;

	for (WORD w = 0; w < 2; w++)
	{
		CPnt4 pt(m_ln[w], 1.);
		ModelView_DoTransform(pt);
	
		double dDis = Pnt4_DistanceTo_xy(ptPic, pt);
	
		if (dDis < dAPert) 
		{
			mS_wCtrlPt = w;
			dAPert = dDis;
		}
	}
	return (mS_wCtrlPt == USHRT_MAX) ? ORIGIN : m_ln[mS_wCtrlPt];
}

bool CPrimDim::SelUsingPoint(const CPnt4& pt0, double dTol, CPnt& ptProj)
{
	mS_wFlags &= ~0x0003;

	CPnt4 pt[4];
	pt[0] = CPnt4(m_ln[0], 1.);
	pt[1] = CPnt4(m_ln[1], 1.);
	ModelView_DoTransform(2, &pt[0]);

	CLine ln; 
	ln[0] = pt[0]; 
	ln[1] = pt[1];
	double	dProjDis;
	if (line::EvalPtProx_xy(ln, pt0, dTol, ptProj, &mS_dRel, &dProjDis)) 
	{
		ptProj[2] = ln[0][2] + mS_dRel * (ln[1][2] - ln[0][2]);
		mS_wFlags |= 0x0001;
		return true;
	}
	CPnts ptsExt;
	GetBoundingBox(ptsExt, 0.);
	
	pt[0] = CPnt4(ptsExt[0], 1.);
	pt[1] = CPnt4(ptsExt[1], 1.);
	pt[2] = CPnt4(ptsExt[2], 1.);
	pt[3] = CPnt4(ptsExt[3], 1.);
	ModelView_DoTransform(4, pt);

	for (size_t n = 0; n < 4; n++)
	{
		if (CLine(pt[n], pt[(n + 1) % 4]).DirRelOfPt(pt0) < 0)
			return false;
	}
	ptProj = pt0;
	mS_wFlags |= 0x0002;
	return true;
}

///<summary>Evaluates whether a line intersects a dimension line.</summary>
bool CPrimDim::SelUsingLine(const CLine& ln, CPnts& ptsInt)
{
	polyline::BeginLineStrip();
	polyline::SetVertex(m_ln[0]);
	polyline::SetVertex(m_ln[1]);

	return polyline::SelUsingLine(ln, ptsInt);
}			
bool CPrimDim::SelUsingRect(const CPnt& pt1, const CPnt& pt2)
{
	polyline::BeginLineStrip();
	polyline::SetVertex(m_ln[0]);
	polyline::SetVertex(m_ln[1]);

	if (!polyline::SelUsingRect(pt1, pt2))
	{
		CPnts pts;
		GetBoundingBox(pts, 0.);
		return polyline::SelUsingRect(pt1, pt2, pts);
	}
	return true;
}

void CPrimDim::SetDefaultNote()
{
	m_rs.SetOrigin(m_ln.ProjPtAlong(.5));
	double dAng = 0.;	
	char cText0 = m_strText[0];
	if (cText0 != 'R' && cText0 != 'D')
	{
        dAng = m_ln.GetAngAboutZAx();
		double dDis = .075;
		if (dAng > HALF_PI + RADIAN && dAng < TWOPI - HALF_PI + RADIAN) 
		{
			dAng -= PI; 
			dDis = - dDis;
		}
		CPnt ptOrigin;
		line::ProjPtFrom_xy(CLine(m_rs.Origin(), m_ln[1]), 0., dDis, &ptOrigin);
		m_rs.SetOrigin(ptOrigin);
	}
	CVec vPlnNorm = view::GetDirection();
	
	CVec vRefXAx;
	CVec vRefYAx;

	vRefYAx = view::GetVwUp();
	vRefYAx.RotAboutArbAx(vPlnNorm, dAng);
	vRefYAx *= .1;
	vRefXAx = vRefYAx;
	vRefXAx.RotAboutArbAx(vPlnNorm, - HALF_PI);
	vRefXAx *= .6;
	
	m_rs.SetDirX(vRefXAx);
	m_rs.SetDirY(vRefYAx);

	char szBuf[64];
	UnitsString_FormatLength(szBuf, app.GetUnits(), m_ln.Length(), 16, 4);
	m_strText = szBuf;
	m_strText.TrimLeft();
	if (cText0 == 'R' || cText0 == 'D')
		m_strText = cText0 + m_strText;
}	

void CPrimDim::Transform(const CTMat& tm)
{
	if ((mS_wFlags & 0x0001) != 0)
	{
		m_ln[0] = tm * m_ln[0]; 
		m_ln[1] = tm * m_ln[1];
	}
	if ((mS_wFlags & 0x0002) != 0)
	{
		m_rs.Transform(tm);
	}
}

void CPrimDim::Translate(const CVec& v)
{
	m_ln += v;
	m_rs.SetOrigin(m_rs.Origin() + v);
}
void CPrimDim::TranslateUsingMask(const CVec& v, const DWORD dwMask)
{
	if ((dwMask & 1) == 1)
		m_ln[0] += v;
	
	if ((dwMask & 2) == 2)
		m_ln[1] += v;

	SetDefaultNote();
}

bool CPrimDim::Vectorize(const CSeg *)
{
	return false;
}

bool CPrimDim::WritePeg(HFILE hFile) const 
{
	FilePeg_WriteWord2(hFile, PRIM_DIM);

	FilePeg_WriteWord2(hFile, m_nPenColor);
	FilePeg_WriteWord2(hFile, m_nPenStyle);
	m_ln.Write(hFile);

	FilePeg_WriteWord2(hFile, m_nTextPenColor);
	m_fd.Write(hFile);
	m_rs.Write(hFile);
	FilePeg_WriteString2(hFile, m_strText);

	return true;
}