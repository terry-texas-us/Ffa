#include "stdafx.h"

#include "PegAEsys.h"

#include "subprocnodal.h"

CPrimMark::CPrimMark()
{
	m_nPenColor = 1;
	m_nMarkStyle = 2;
	m_pt = ORIGIN;
	m_Dats = 0;
	m_dDat = 0;
}

CPrimMark::CPrimMark(PENCOLOR nPenColor, short nMarkStyle, const CPnt& pt) : m_pt(pt)
{	
	m_nPenColor = nPenColor;
	m_nMarkStyle = nMarkStyle;
	m_Dats = 0;
	m_dDat = 0;
} 

CPrimMark::CPrimMark(const CPrimMark& src)
{
	m_nPenColor = src.m_nPenColor;
	m_nMarkStyle = src.m_nMarkStyle;
	m_pt = src.m_pt;
	m_Dats = src.m_Dats;
	m_dDat = (m_Dats == 0) ? 0 : new double[m_Dats];

	for (WORD w = 0; w < m_Dats; w++) m_dDat[w] = src.m_dDat[w]; 
}

CPrimMark::~CPrimMark()
{
	if (m_Dats != 0)
		delete [] m_dDat;
}

const CPrimMark& CPrimMark::operator=(const CPrimMark& src)
{
	m_nPenColor = src.m_nPenColor;
	m_nMarkStyle = src.m_nMarkStyle;;
	m_pt = src.m_pt;
	if (m_Dats != src.m_Dats)
	{
		if (m_Dats != 0)
			delete [] m_dDat;

		m_Dats = src.m_Dats;
		
		m_dDat = (m_Dats == 0) ? 0 : new double[m_Dats];
	}
	for (WORD w = 0; w < m_Dats; w++) m_dDat[w] = src.m_dDat[w]; 
	
	return (*this);
}

CPrim*& CPrimMark::Copy(CPrim*& pPrim) const
{
	pPrim = new CPrimMark(*this);
	return (pPrim);
}

void CPrimMark::Display(CDC* pDC, bool bNodeDisplay)
{
	// show/hide marker in print preview
	if(!printStates.GetPrintMarkers() && (app.GetPegState() == STATE_PLOT))
		return;

	CPnt4 pt(m_pt, 1.);
	ModelView_DoTransform(pt);

	if (Pnt4_IsInView(pt))
	{
		CPoint pnt = viewport::DoProjection(pt);
		int i;

		switch (m_nMarkStyle)
		{
			// pixel
			case 0:
				pDC->SetPixel(pnt, app.PenColorsGetHot(LogicalPenColor()));
				break;
			
			// none
			case 1:
				break;

			default:
			case 2: // small plus
				for (i = - 4; i <= 4; i++)
				{
					pDC->SetPixel(pnt.x + i , pnt.y, crHotCols[LogicalPenColor()]);
					pDC->SetPixel(pnt.x , pnt.y + i, crHotCols[LogicalPenColor()]);
				}
				break;
		
			case 3: // small cross
				for (i = - 4; i <= 4; i++)
				{
					pDC->SetPixel(pnt.x + i , pnt.y - i, crHotCols[LogicalPenColor()]);
					pDC->SetPixel(pnt.x + i, pnt.y + i, crHotCols[LogicalPenColor()]);
				}
				break;
		}		
	}

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

void CPrimMark::DisplayOpenGL(bool)
{
	//PENCOLOR nPenColor = LogicalPenColor();
	//opengl::SetCurrentColor(app.PenColorsGetHot(nPenColor));

	//opengl::BeginPoints();
	//	opengl::SetVertex(m_pt);
	//opengl::End();
}

void CPrimMark::DisRep(const CPnt&) const
{
	CString str;
	str.Format("<Mark> Color: %s Style: %s", FormatPenColor(), FormatPenStyle());
	msgInformation(str);
}

void CPrimMark::FormatExtra(CString& str) const
{
	str.Format("Color;%s\tStyle;%d", FormatPenColor(), m_nMarkStyle);
}

void CPrimMark::FormatGeometry(CString& str) const
{
	str += "Point;" + m_pt.ToString();
}

CPnt CPrimMark::GetCtrlPt() const 
{
	CPnt pt;
	pt = m_pt;
	return (pt);
}

///<summary>Determines the extent.</summary>
void CPrimMark::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
{	
	CPnt pt = m_pt;

	mspace.Transform(pt);
	pt = tm * pt;
	ptMin = Min(ptMin, pt);
	ptMax = Max(ptMax, pt);
}

bool CPrimMark::IsInView() const
{
	CPnt4 pt(m_pt, 1.);

	ModelView_DoTransform(pt);

	return (Pnt4_IsInView(pt));
}

CPnt CPrimMark::SelAtCtrlPt(const CPnt4& ptPic) const
{
	mS_wCtrlPt = USHRT_MAX;

	CPnt4 pt(m_pt, 1.);
	ModelView_DoTransform(pt);
	
	if (Pnt4_DistanceTo_xy(ptPic, pt) < mS_dPicApertSiz) 
	{
		mS_wCtrlPt = 0;
		return m_pt;
	}
	return ORIGIN;
}

bool CPrimMark::SelUsingPoint(const CPnt4& ptPic, double dTol, CPnt& ptProj)
{
	CPnt4 pt(m_pt, 1.);
	
	ModelView_DoTransform(pt);

	ptProj = pt;

	return (Pnt4_DistanceTo_xy(ptPic, pt) <= dTol) ? true : false;
}

bool CPrimMark::SelUsingRect(const CPnt& pt1, const CPnt& pt2)
{
	CPnt4 pt(m_pt, 1.);
	ModelView_DoTransform(pt);
	
	return ((pt[0] >= pt1[0] && pt[0] <= pt2[0] && pt[1] >= pt1[1] && pt[1] <= pt2[1]) ? true : false);
}

bool CPrimMark::IsPtACtrlPt(const CPnt4& ptPic) const
{
	CPnt4 pt(m_pt, 1.);
	ModelView_DoTransform(pt);

	return ((Pnt4_DistanceTo_xy(ptPic, pt) < mS_dPicApertSiz) ? true : false);
}

void CPrimMark::ModifyState()
{
	CPrim::ModifyState();
	m_nMarkStyle = pstate.MarkStyle();
}

void CPrimMark::GetState()
{
	CPrim::GetState();
	pstate.SetMarkStyle(m_nMarkStyle);
}

void CPrimMark::ReadPeg(HFILE hFile)
{
	CPrim::ReadPeg(hFile);
	
	m_pt.Read(hFile);
	FilePeg_ReadWord2(hFile, m_Dats);

	if (m_Dats == 0)
		m_Dats = 0;
	else
		m_dDat = new double[m_Dats];

	for (WORD w = 0; w < m_Dats; w++)
		FilePeg_ReadDouble2(hFile, m_dDat[w]);
}

void CPrimMark::SetDat(WORD wDats, double* dDat)
{
	if (m_Dats != wDats)
	{
		if (m_Dats != 0)
		{
			delete [] m_dDat;
		}
		m_Dats = wDats;
		m_dDat = (m_Dats == 0) ? 0 : new double[m_Dats];
	}
	for (WORD w = 0; w < m_Dats; w++)
	{
		m_dDat[w] = dDat[w];
	}
}

void CPrimMark::Transform(const CTMat& tm)
{
	m_pt = tm * m_pt;
}

void CPrimMark::TranslateUsingMask(const CVec& v, const DWORD dwMask)
{
	if (dwMask != 0)
		m_pt += v;
}

bool CPrimMark::Vectorize(const CSeg *)
{
	return false;
}

bool CPrimMark::WritePeg(HFILE hFile) const
{
	FilePeg_WriteWord2(hFile, PRIM_MARK);
	FilePeg_WriteWord2(hFile, m_nPenColor);
	FilePeg_WriteWord2(hFile, m_nMarkStyle);
	m_pt.Write(hFile);
	FilePeg_WriteWord2(hFile, m_Dats);

	for (WORD w = 0; w < m_Dats; w++)
		FilePeg_WriteDouble2(hFile, m_dDat[w]);

	return true;
}