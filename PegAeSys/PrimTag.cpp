#include "stdafx.h"

#include "PegAEsys.h"

CPrimTag::CPrimTag(const CPrimTag& src)
{
	m_nPenColor = src.m_nPenColor;
	m_nPenStyle = src.m_nPenStyle;
	m_Pt = src.m_Pt;
}

const CPrimTag& CPrimTag::operator=(const CPrimTag& src)
{
	m_nPenColor = src.m_nPenColor;
	m_nPenStyle = src.m_nPenStyle;
	m_Pt = src.m_Pt;
	return (*this);
}

CPrim*& CPrimTag::Copy(CPrim*& pPrim) const
{
	pPrim = new CPrimTag(*this);
	return (pPrim);
}

void CPrimTag::Display(CDC*, bool)
{
	
}

void CPrimTag::DisplayOpenGL(bool)
{
		//PENCOLOR nPenColor = LogicalPenColor();
//		opengl::SetCurrentColor(app.PenColorsGetHot(nPenColor));

//		opengl::BeginPoints();
//			opengl::SetVertex(m_Pt);
//		opengl::End();
}

void CPrimTag::DisRep(const CPnt&) const
{
	CString str;
	str.Format("<Tag> Color: %s Style: %s", FormatPenColor(), FormatPenStyle());
	msgInformation(str);
}

void CPrimTag::FormatExtra(CString& str) const
{
	str.Format("Color;%s\tStyle;%s", FormatPenColor(), FormatPenStyle());
}

void CPrimTag::FormatGeometry(CString& str) const
{
	str += "Point;" + m_Pt.ToString();
}

CPnt CPrimTag::GetCtrlPt() const
{
	CPnt pt;
	pt = m_Pt;
	return (pt);
}

///<summary>Determines the extent.</summary>
void CPrimTag::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
{
	CPnt pt = m_Pt;

	mspace.Transform(pt);
	pt = tm * pt;
	ptMin = Min(ptMin, pt);
	ptMax = Max(ptMax, pt);
}

bool CPrimTag::IsInView() const
{
	CPnt4 pt(m_Pt, 1.);

	ModelView_DoTransform(pt);

	return (Pnt4_IsInView(pt));
}

CPnt CPrimTag::SelAtCtrlPt(const CPnt4& ptPic) const
{
	mS_wCtrlPt = USHRT_MAX;

	CPnt4 pt(m_Pt, 1.);
	ModelView_DoTransform(pt);

	if (Pnt4_DistanceTo_xy(ptPic, pt) < mS_dPicApertSiz)
	{
		mS_wCtrlPt = 0;
		return m_Pt;
	}
	return ORIGIN;
}

bool CPrimTag::SelUsingPoint(const CPnt4& ptPic, double dTol, CPnt& ptProj)
{
	CPnt4 pt(m_Pt, 1.);
	
	ModelView_DoTransform(pt);

	ptProj = pt;

	return (Pnt4_DistanceTo_xy(ptPic, pt) <= dTol) ? true : false;
}

bool CPrimTag::SelUsingRect(const CPnt& pt1, const CPnt& pt2)
{
	CPnt4 pt(m_Pt, 1.);
	ModelView_DoTransform(pt);
	
	return ((pt[0] >= pt1[0] && pt[0] <= pt2[0] && pt[1] >= pt1[1] && pt[1] <= pt2[1]) ? true : false);
}

bool CPrimTag::IsPtACtrlPt(const CPnt4& ptPic) const
{
	CPnt4 pt(m_Pt, 1.);
	ModelView_DoTransform(pt);

	return ((Pnt4_DistanceTo_xy(ptPic, pt) < mS_dPicApertSiz) ? true : false);
}

void CPrimTag::ReadPeg(HFILE hFile)
{
	CPrim::ReadPeg(hFile);
	m_Pt.Read(hFile);
}

void CPrimTag::Transform(const CTMat& tm)
{
	m_Pt = tm * m_Pt;
}

void CPrimTag::TranslateUsingMask(const CVec& v, const DWORD dwMask)
{
	if (dwMask != 0)
		m_Pt += v;
}

bool CPrimTag::Vectorize(const CSeg *)
{
	return false;
}

bool CPrimTag::WritePeg(HFILE hFile) const
{
	FilePeg_WriteWord2(hFile, PRIM_TAG);
	FilePeg_WriteWord2(hFile, m_nPenColor);
	FilePeg_WriteWord2(hFile, m_nPenStyle);
	m_Pt.Write(hFile);
	return true;
}