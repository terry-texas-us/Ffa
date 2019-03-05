#include "stdafx.h"

#include "PegAEsys.h"

#include "Polyline.h"

#include "subprocnodal.h"

CPrimBSpline::CPrimBSpline(WORD wPts, CPnt* pt)
{
	m_nPenColor = pstate.PenColor();
	m_nPenStyle = pstate.PenStyle();
	
	for (WORD w = 0; w < wPts; w++)
		m_pts.Add(pt[w]);
}	

CPrimBSpline::CPrimBSpline(const CPrimBSpline& src)
{
	m_nPenColor = src.m_nPenColor;
	m_nPenStyle = src.m_nPenStyle;
	m_pts.Copy(src.m_pts);
}

const CPrimBSpline& CPrimBSpline::operator=(const CPrimBSpline& src)
{
	m_nPenColor = src.m_nPenColor;
	m_nPenStyle = src.m_nPenStyle;
	m_pts.Copy(src.m_pts);

	return (*this);
}

bool CPrimBSpline::SelUsingLine(const CLine& ln, CPnts& ptsInt)
{
	ptsInt.RemoveAll();

	CPnts temp;

	for(int i = 0; i < m_pts.GetSize()-1; i++)
	{
		polyline::BeginLineStrip();
		polyline::SetVertex(m_pts[i]);
		polyline::SetVertex(m_pts[i+1]);
		if(polyline::SelUsingLine(ln, ptsInt))
			temp.Add(ptsInt[0]);
	}

	return (!temp.IsEmpty());
}

CPrim*& CPrimBSpline::Copy(CPrim*& pPrim) const
{
	pPrim = new CPrimBSpline(*this);
	return (pPrim);
}

void CPrimBSpline::Display(CDC* pDC, bool bNodeDisplay)
{
	PENCOLOR nPenColor = LogicalPenColor();
	PENSTYLE nPenStyle = LogicalPenStyle();

	int pStateSave = pstate.Save();
	pstate.SetPen(pDC, nPenColor, nPenStyle);

	polyline::BeginLineStrip();
	GenPts(3, m_pts);
	polyline::End(pDC, nPenStyle);

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

void CPrimBSpline::DisplayOpenGL(bool )//bNodeDisplay)
{
	//PENCOLOR nPenColor = LogicalPenColor();
	//PENSTYLE nPenStyle = LogicalPenStyle();

	//opengl::SetCurrentColor(app.PenColorsGetHot(nPenColor));
	//opengl::BeginLineStrip();
	//
	//for (WORD w = 0; w < m_pts.GetSize(); w++)
	//{
	//	opengl::SetVertex(m_pts[w]);
	//}
	//opengl::End();
}

void CPrimBSpline::DisRep(const CPnt&) const
{
	CString str;
	str.Format("<BSpline> Color: %s Style: %s", FormatPenColor(), FormatPenStyle());
	msgInformation(str);
}

void CPrimBSpline::FormatGeometry(CString& str) const
{
	for (WORD w = 0; w < m_pts.GetSize(); w++)
	{
		str += "Control Point;" + m_pts[w].ToString();
	}
}

void CPrimBSpline::FormatExtra(CString& str) const
{
	str.Format("Color;%s\tStyle;%s\tControl Points;%d",
		FormatPenColor(), FormatPenStyle(), m_pts.GetSize());
}

CPnt CPrimBSpline::GetCtrlPt() const 
{
	CPnt pt;
	pt = m_pts[m_pts.GetSize() / 2];
	return (pt);
}

///<summary>Determines the extent.</summary>
void CPrimBSpline::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
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

CPnt CPrimBSpline::GoToNxtCtrlPt() const
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

void CPrimBSpline::Iron()
{
	for(int i = 0; i < m_pts.GetSize(); i++)
		m_pts[i][2] = 0;
}

bool CPrimBSpline::IsInView() const
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

CPnt CPrimBSpline::SelAtCtrlPt(const CPnt4& ptPicVw) const
{
	mS_wCtrlPt = USHRT_MAX;
	return (ptPicVw);
}

bool CPrimBSpline::SelUsingPoint(const CPnt4& ptPic, double dTol, CPnt& ptProj)
{
	polyline::BeginLineStrip();
	GenPts(3, m_pts);
	return (polyline::SelUsingPoint(ptPic, dTol, mS_dRel, ptProj));
	//return false;
}

bool CPrimBSpline::SelUsingRect(const CPnt& pt1, const CPnt& pt2)
{
	return polyline::SelUsingRect(pt1, pt2, m_pts);
}

void CPrimBSpline::ReadPeg(HFILE hFile)
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

void CPrimBSpline::Transform(const CTMat& tm)
{
	for (WORD w = 0; w < m_pts.GetSize(); w++)
		m_pts[w] = tm * m_pts[w];
}

void CPrimBSpline::Translate(const CVec& v)
{
	for (WORD w = 0; w < m_pts.GetSize(); w++) 
		m_pts[w] += v;
}

void CPrimBSpline::TranslateUsingMask(const CVec& v, const DWORD dwMask)
{
	for (WORD w = 0; w < m_pts.GetSize(); w++) 
		if (((dwMask >> w) & 1UL) == 1)
			m_pts[w] += v;
}

bool CPrimBSpline::Vectorize(const CSeg *pSeg)
{
	POSITION curPos = pSeg->Find(this);

	if(curPos == 0)
		return false;

	CPnts pnts;
	static int index = 0;

	const int iOrder = 3;

	// code taken from genpts
	int n65 = 65;
	int n66 = 66;

	// support for more than 64 points
	n65 = 512; n66 = 513;

	double* dKnot = new double[n65 * n66];
	if (dKnot == 0)
	{
		msgWarning(IDS_MSG_MEM_ALLOC_ERR); 
		return 0;
	}
	int iPts = 8 * m_pts.GetSize(); 
	double* dWght = &dKnot[n65];
	
	int i, i2, i4;
	
	int iTMax = (m_pts.GetSize() - 1) - iOrder + 2;
	int iKnotVecMax = (m_pts.GetSize() - 1) + iOrder;			// Maximum number of dKnot vectors
	
	for (i = 0; i < n65 * n65; i++)								// Set weighting value array with zeros
		dWght[i] = 0.;
			
	for (i = 0; i <= iKnotVecMax; i++)							// Determine dKnot vectors
	{
		if (i <= iOrder - 1)									// Beginning of curve
			dKnot[i] = 0.;
		else if (i >= iTMax + iOrder)							// End of curve
			dKnot[i] = dKnot[i - 1];
		else
		{
			i2 = i - iOrder;
			if (m_pts[i2] == m_pts[i2 + 1])						// Repeating vertices
				dKnot[i] = dKnot[i - 1];
			else												// Successive internal vectors
				dKnot[i] = dKnot[i - 1] + 1.;
		}			 
	}

	if (dKnot[iKnotVecMax] != 0.) 
	{
		double G = 0.;
		double H = 0.;
		double Z = 0.;
		double T, W1, W2;
		double dStep = dKnot[iKnotVecMax] / (double) (iPts - 1);
		int iPts2 = 0;
		for (i4 = iOrder - 1; i4 <= iOrder + iTMax; i4++)
		{
			for (i = 0; i <= iKnotVecMax - 1; i++)					// Calculate values for weighting value
			{
				if (i != i4 || dKnot[i] == dKnot[i + 1]) 
					dWght[n65 * i + 1] = 0.;
				else
					dWght[n65 * i + 1] = 1.;
			}
			for (T = dKnot[i4]; T <= dKnot[i4 + 1] - dStep; T += dStep)
			{
				iPts2++;
				for (i2 = 2; i2 <= iOrder; i2++)
				{
					for (i = 0; i <= (int) m_pts.GetSize() - 1; i++) 	// Determine first term of weighting function equation
					{
						if (dWght[n65 * i + i2 - 1] == 0.) 
							W1 = 0.;
						else
							W1 = ((T - dKnot[i]) * dWght[n65 * i + i2 - 1]) / (dKnot[i + i2 - 1] - dKnot[i]);

						if (dWght[n65 * (i + 1) + i2 - 1] == 0.) 	// Determine second term of weighting function equation
							W2 = 0.;
						else
							W2 = ((dKnot[i + i2] - T) * dWght[n65 * (i + 1) + i2 - 1]) / (dKnot[i + i2] - dKnot[i + 1]);
						
						dWght[n65 * i + i2] = W1 + W2;
						G = m_pts[i][0] * dWght[n65 * i + i2] + G;
						H = m_pts[i][1] * dWght[n65 * i + i2] + H;
						Z = m_pts[i][2] * dWght[n65 * i + i2] + Z;
					}
					if (i2 == iOrder) 
						break;
					G = 0.; H = 0.; Z = 0.;
				}
				pnts.Add(CPnt(G, H, Z));
				G = 0.; H = 0.; Z = 0.;
			}
		}
		iPts = iPts2 + 1;
	}
	else				// either iOrder greater than number of control points or all control points coincidental
	{
		iPts = 2;

		index = 0;
		pnts.Add(m_pts[0]);
	}

	pnts.Add(m_pts[m_pts.GetUpperBound()]);

	// - create lines
	for(int j = 0; j < iPts - 1; j++)
	{
		CPrimLine *pPrimLine = new CPrimLine(PenColor(), PenStyle(), pnts.GetAt(j), pnts.GetAt(j+1));
		((CObList*)pSeg)->InsertAfter(curPos, pPrimLine);
	}
	
	delete [] dKnot;
	
	return true;
}

bool CPrimBSpline::WritePeg(HFILE hFile) const
{
	FilePeg_WriteWord2(hFile, PRIM_BSPLINE);
	FilePeg_WriteWord2(hFile, m_nPenColor);
	FilePeg_WriteWord2(hFile, m_nPenStyle);
	FilePeg_WriteWord2(hFile, (WORD) m_pts.GetSize());

	for (WORD w = 0; w < m_pts.GetSize(); w++)
		m_pts[w].Write(hFile);

	return true;
}

///<summary>
///Generates the required B-spline basis dKnot vectors and B-spline curves of various iOrder
///using the Cox and de Boor algorithm.
///</summary>
// Notes:	If the iOrder equals the number of vertices, and 
//			there are no multiple vertices, a Bezier curve will
//			be generated. As the iOrder decreases the curve 
//			produced will lie closer to the defining polygon.  
//			When the iOrder is two the generated curve is a 
//			series of straight lines which are identical to the 
//			defining polygon.  Increasing the iOrder "tightens" 
//			the curve.	Additional shape control can be obtained by 
//			use of repeating vertices.
// Parameters:	iOrder 		iOrder of B-spline basis
//				iPts			number of points generated along curve
//				pt			1 based
int CPrimBSpline::GenPts(const int iOrder, const CPnts& pts) const
{
	// FIX: this fixed the too many points crash
	int n65 = 65;
	int n66 = 66;

	// support for more than 64 points
	n65 = 512; n66 = 513;

	double* dKnot = new double[n65 * n66];
//	double* dKnot = new double[66 * 66 + 1];
//	double* dKnot = new double[80 * 80];

	if (dKnot == 0)
	{
		msgWarning(IDS_MSG_MEM_ALLOC_ERR); 
		return 0;
	}
	int iPts = 8 * pts.GetSize();
	double* dWght = &dKnot[n65];
	
	int i, i2, i4;
	
	int iTMax = (pts.GetSize() - 1) - iOrder + 2;
	int iKnotVecMax = (pts.GetSize() - 1) + iOrder;				// Maximum number of dKnot vectors
	
	for (i = 0; i < n65 * n65; i++)								// Set weighting value array with zeros
		dWght[i] = 0.;
			
	for (i = 0; i <= iKnotVecMax; i++)							// Determine dKnot vectors
	{
		if (i <= iOrder - 1)									// Beginning of curve
			dKnot[i] = 0.;
		else if (i >= iTMax + iOrder)							// End of curve
			dKnot[i] = dKnot[i - 1];
		else
		{
			i2 = i - iOrder;
			if (pts[i2] == pts[i2 + 1])							// Repeating vertices
				dKnot[i] = dKnot[i - 1];
			else												// Successive internal vectors
				dKnot[i] = dKnot[i - 1] + 1.;
		}			 
	}
	if (dKnot[iKnotVecMax] != 0.) 
	{
		double G = 0.;
		double H = 0.;
		double Z = 0.;
		double T, W1, W2;
		double dStep = dKnot[iKnotVecMax] / (double) (iPts - 1);
		int iPts2 = 0;
		for (i4 = iOrder - 1; i4 <= iOrder + iTMax; i4++)
		{
			for (i = 0; i <= iKnotVecMax - 1; i++)					// Calculate values for weighting value
			{
				if (i != i4 || dKnot[i] == dKnot[i + 1]) 
					dWght[n65 * i + 1] = 0.;
				else
					dWght[n65 * i + 1] = 1.;
			}
			for (T = dKnot[i4]; T <= dKnot[i4 + 1] - dStep; T += dStep)
			{
				iPts2++;
				for (i2 = 2; i2 <= iOrder; i2++)
				{
					for (i = 0; i <= (int) pts.GetSize() - 1; i++) 	// Determine first term of weighting function equation
					{
						if (dWght[n65 * i + i2 - 1] == 0.) 
							W1 = 0.;
						else
							W1 = ((T - dKnot[i]) * dWght[n65 * i + i2 - 1]) / (dKnot[i + i2 - 1] - dKnot[i]);

						if (dWght[n65 * (i + 1) + i2 - 1] == 0.) 	// Determine second term of weighting function equation
							W2 = 0.;
						else
							W2 = ((dKnot[i + i2] - T) * dWght[n65 * (i + 1) + i2 - 1]) / (dKnot[i + i2] - dKnot[i + 1]);
						
						dWght[n65 * i + i2] = W1 + W2;
						G = pts[i][0] * dWght[n65 * i + i2] + G;
						H = pts[i][1] * dWght[n65 * i + i2] + H;
						Z = pts[i][2] * dWght[n65 * i + i2] + Z;
					}
					if (i2 == iOrder) 
						break;
					G = 0.; H = 0.; Z = 0.;
				}
				polyline::SetVertex(CPnt(G, H, Z));
				G = 0.; H = 0.; Z = 0.;
			}
		}
		iPts = iPts2 + 1;
	}
	else				// either iOrder greater than number of control points or all control points coincidental
	{
		iPts = 2;
		polyline::SetVertex(pts[0]);
	}
	polyline::SetVertex(pts[pts.GetUpperBound()]);
	
	delete [] dKnot;
	
	return (iPts);
}