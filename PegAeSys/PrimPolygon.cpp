#include "stdafx.h"

#include "PegAEsys.h"

#include "ddeGItms.h"
#include "Hatch.h"
#include "Polyline.h"
#include "UnitsString.h"

#include "subprocnodal.h"

WORD CPrimPolygon::mS_wEdgeToEvaluate = 0;
WORD CPrimPolygon::mS_wEdge = 0;
WORD CPrimPolygon::mS_wPivotVertex = 0;

typedef struct tagFilAreaEdgLis 
{
	double dMinY;			// minimum y extent of edge
	double dMaxY;			// maximum y extent of edge
	double dX;				// x intersection on edge
	union 
	{
		double dInvSlope;		// inverse slope of edge
		double dStepSiz;		// change in x for each scanline
	};
} pFilAreaEdgLis;

CPrimPolygon::CPrimPolygon()
{
	m_nIntStyle = POLYGON_HOLLOW;
	m_nIntStyleId = 1;
	m_wPts = 0;
	m_Pt = 0;
	m_ptOrig = ORIGIN;
	m_vPosXAx = XDIR;
	m_vPosYAx = YDIR;
}

CPrimPolygon::CPrimPolygon(WORD wPts, CPnt* pt)
{
	m_nPenColor = 0;
//	m_nPenStyle = 0;
	m_nIntStyle = POLYGON_SOLID;
	m_nIntStyleId = 0;
	m_wPts = wPts;
	m_ptOrig = pt[0];
	m_vPosXAx = XDIR;
	m_vPosYAx = YDIR;
	m_Pt = new CPnt[m_wPts];

	for (WORD w = 0; w < m_wPts; w++)
		m_Pt[w] = pt[w];
}

CPrimPolygon::CPrimPolygon(WORD wPts, CPnt ptOrig, const CVec& vXAx, const CVec& vYAx, const CPnt* ppt)
{
	m_nPenColor	= pstate.PenColor();
//	m_nPenStyle = pstate.PenStyle();
	m_nIntStyle = pstate.PolygonIntStyle();
	m_nIntStyleId = pstate.PolygonIntStyleId();
	m_wPts = wPts;
	m_ptOrig = ptOrig;
	m_vPosXAx = vXAx;
	m_vPosYAx = vYAx;
	m_Pt = new CPnt[m_wPts];
	
	for (WORD w = 0; w < m_wPts; w++)
		m_Pt[w] = ppt[w];
}

CPrimPolygon::CPrimPolygon(const CPrimPolygon& src)
{
	m_nPenColor	= src.m_nPenColor;
//	m_nPenStyle = src.m_nPenStyle;
	m_nIntStyle = src.m_nIntStyle;
	m_nIntStyleId = src.m_nIntStyleId;
	m_ptOrig = src.m_ptOrig;
	m_vPosXAx = src.m_vPosXAx;
	m_vPosYAx = src.m_vPosYAx;
	m_wPts = src.m_wPts;
	m_Pt = new CPnt[m_wPts];
	for (WORD w = 0; w < m_wPts; w++)
		m_Pt[w] = src.m_Pt[w];
}

const CPrimPolygon& CPrimPolygon::operator=(const CPrimPolygon& src)
{
	m_nPenColor = src.m_nPenColor;
//	m_nPenStyle = src.m_npenStyle;
	m_nIntStyle = src.m_nIntStyle;
	m_nIntStyleId = src.m_nIntStyleId;
	m_ptOrig = src.m_ptOrig;
	m_vPosXAx = src.m_vPosXAx;
	m_vPosYAx = src.m_vPosYAx;
	
	if (m_wPts != src.m_wPts)
	{
		m_wPts = src.m_wPts;
		delete [] m_Pt;
		m_Pt = new CPnt[m_wPts];
	}
	for (WORD w = 0; w < m_wPts; w++)
		m_Pt[w] = src.m_Pt[w];

	return (*this);
}

CPrimPolygon::~CPrimPolygon()
{
	delete [] m_Pt;
}

bool CPrimPolygon::SelUsingLine(const CLine& ln, CPnts& ptsInt)
{
	ptsInt.RemoveAll();

	CPnts temp;

	for(WORD w = 0; w < m_wPts-1; w++)
	{
		polyline::BeginLineStrip();
		polyline::SetVertex(m_Pt[w]);
		polyline::SetVertex(m_Pt[w+1]);
		if(polyline::SelUsingLine(ln, ptsInt))
			temp.Add(ptsInt[0]);
	}

	polyline::BeginLineStrip();
	polyline::SetVertex(m_Pt[m_wPts-1]);
	polyline::SetVertex(m_Pt[0]);
	if(polyline::SelUsingLine(ln, ptsInt))
		temp.Add(ptsInt[0]);

	return (!temp.IsEmpty());
}

CPrim*& CPrimPolygon::Copy(CPrim*& pPrim) const
{
	pPrim = new CPrimPolygon(*this);
	return (pPrim);
}

void CPrimPolygon::Display(CDC* pDC, bool bNodeDisplay)
{
	PENCOLOR nPenColor = LogicalPenColor();
	pstate.SetPen(pDC, nPenColor, 1);

	short nPolygonStyle = mS_nSpecPolygonStyle == - 1 ? m_nIntStyle : mS_nSpecPolygonStyle;
	pstate.SetPolygonIntStyle(nPolygonStyle); // hollow, solid, pattern, hatch 
	pstate.SetPolygonIntStyleId(m_nIntStyleId);
	
	int iPtLstsId = m_wPts;
	
	if (m_nIntStyle == POLYGON_HATCH)
	{
		CTMat tm(m_ptOrig, m_vPosXAx, m_vPosYAx);
		DisplayFilAreaHatch(pDC, tm, 1, &iPtLstsId, m_Pt);
	}
	else														// Fill area interior style is 
	{															// hollow, solid or pattern
	 	CPnt4s pta;
		
		pta.SetSize(m_wPts);

		for (WORD w = 0; w < m_wPts; w++)
		{
			pta[w] = CPnt4(m_Pt[w], 1.);
		}
		ModelView_DoTransform(pta);
		Pnt4_ClipPolygon(pta);
		Polygon_Display(pDC, pta);
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

void CPrimPolygon::DisplayOpenGL(bool)
{
	//PENCOLOR nPenColor = LogicalPenColor();

	//	opengl::SetCurrentColor(app.PenColorsGetHot(nPenColor));
	//	
	//	CVec vNorm = CVec(m_Pt[0], m_Pt[1]) ^ CVec(m_Pt[0], m_Pt[2]);
	//	vNorm.Normalize();
	//	double dNormal[3]; vNorm.Get(dNormal);

	//	bool bIsConvex = m_wPts == 3;

	//	if (m_wPts == 4)
	//	{
	//		CTMat tm(m_Pt[0], vNorm);
	//		
	//		CPnt pt[4]; for (int i = 0; i < 4; i++) {pt[i] = tm * m_Pt[i];}
	//		
	//		CVec v1(pt[0], pt[1]);
	//		CVec v2(pt[0], pt[2]);
	//		CVec v3(pt[0], pt[3]);

	//		bIsConvex = Signum((v1 ^ v2)[2]) == Signum((v2 ^ v3)[2]);
	//	}
	//	if (bIsConvex)
	//	{
	//		opengl::BeginPolygon();
	//		glNormal3dv(dNormal);
	//		for (WORD w = 0; w < m_wPts; w++)
	//		{
	//			opengl::SetVertex(m_Pt[w]);
	//		}
	//		opengl::End();
	//	}
	//	else
	//	{	// must assume the polygon is concave or complex
	//		TRACE("tessing concave polygon, number of verticies: %d\n", m_wPts);
	//		GLUtesselator *tess;
	//		tess = gluNewTess();
	//		
	//		gluTessCallback(tess, GLU_TESS_BEGIN, (void (CALLBACK*) ()) &glBegin);
	//		gluTessCallback(tess, GLU_TESS_VERTEX, (void (CALLBACK*) ()) &glVertex3dv);
	//		gluTessCallback(tess, GLU_TESS_END, (void (CALLBACK*) ()) &glEnd);

	//		GLdouble dPt[256][3];
	//		
	//		gluTessBeginPolygon(tess, 0);
	//			gluTessBeginContour(tess);
	//			//gluTessNormal(tess, dNormal[0], dNormal[1], dNormal[2]);
	//			gluTessNormal(tess, 0., 0., 0.);
	//			for (WORD w = 0; w < m_wPts; w++)
	//			{
	//				m_Pt[w].Get(dPt[w]);
	//				gluTessVertex(tess, dPt[w], dPt[w]);
	//			}
	//			gluTessEndContour(tess);
	//		gluTessEndPolygon(tess);
	//		
	//		gluDeleteTess(tess);
	//	}	
}

void CPrimPolygon::DisRep(const CPnt& ) const
{
	CString strRep("<Polygon Edge> ");

	if (mS_wEdge > 0 && mS_wEdge <= m_wPts)
	{
		CPnt* pBegPt = &m_Pt[mS_wEdge - 1];
		CPnt* pEndPt = &m_Pt[mS_wEdge % m_wPts];

//*********************************************************//
//  MAK - removed code to fix angle display
//*********************************************************//
//		if (mS_wPivotVertex < m_wPts)
//		{
//			pBegPt = &m_Pt[mS_wPivotVertex];
//			pEndPt = &m_Pt[SwingVertex()];
//
//			pEndPt = &m_Pt[mS_wPivotVertex];
//			pBegPt = &m_Pt[SwingVertex()];
//		}
//*********************************************************//

		double dAng;
		double dLen = CVec(*pBegPt, *pEndPt).Length();				// Length of edge
		
//*********************************************************//
//  MAK - removed code to fix angle display
//*********************************************************//
//		if (CVec(ptPic, *pBegPt).Length() > dLen * .5)
//			dAng = line::GetAngAboutZAx(CLine(*pEndPt, *pBegPt));
//		else
//			dAng = line::GetAngAboutZAx(CLine(*pBegPt, *pEndPt));
//*********************************************************//

		// line was added when previous code removed in order to fix angle display
		dAng = line::GetAngAboutZAx(CLine(*pBegPt, *pEndPt));
		
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
void CPrimPolygon::FormatGeometry(CString& str) const
{
	str += "Hatch Origin;" + m_ptOrig.ToString();
	str += "X Axis;" + m_vPosXAx.ToString();
	str += "Y Axis;" + m_vPosYAx.ToString();
	
	for (WORD w = 0; w < m_wPts; w++)
	{
		str += "Vertex Point;" + m_Pt[w].ToString();
	}
}

CString CPrimPolygon::FormatIntStyle() const
{
	CString strStyle[] = {"Hollow", "Solid", "Pattern", "Hatch"};
	
	CString str = (m_nIntStyle >= 0 && m_nIntStyle <= 3) ? strStyle[m_nIntStyle] : "Invalid!";

	return (str);
}

void CPrimPolygon::FormatExtra(CString& str) const
{
	str.Format("Color;%s\tStyle;%s\tPoints;%d", FormatPenColor(), FormatPenStyle(), m_wPts);
}

CPnt CPrimPolygon::GetCtrlPt() const 
{
	//WORD wBeg = WORD(mS_wEdge - 1);
	WORD wBeg = WORD(mS_wEdge);
	WORD wEnd = WORD(mS_wEdge % m_wPts);
	CPnt pt = CLine(m_Pt[wBeg], m_Pt[wEnd]).ProjPtAlong(.5);

	return (pt);
}

CPnt CPrimPolygon::GoToNxtCtrlPt() const
{
	if (mS_wPivotVertex >= m_wPts)
	{	// have not yet rocked to a vertex
		WORD wBeg = WORD(mS_wEdge - 1);
		WORD wEnd = WORD(mS_wEdge % m_wPts);

		if (m_Pt[wEnd][0] > m_Pt[wBeg][0])
			mS_wPivotVertex = wBeg;
		else if (m_Pt[wEnd][0] < m_Pt[wBeg][0])
			mS_wPivotVertex = wEnd;
		else if (m_Pt[wEnd][1] > m_Pt[wBeg][1])
			mS_wPivotVertex = wBeg;
		else
			mS_wPivotVertex = wEnd;
	}
	else if (mS_wPivotVertex == 0)
	{
		if (mS_wEdge == 1)
			mS_wPivotVertex = 1; 
		else
			mS_wPivotVertex = WORD(m_wPts - 1); 
	}
	else if (mS_wPivotVertex == WORD(m_wPts - 1))										
	{
		if (mS_wEdge == m_wPts)
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
	return (m_Pt[mS_wPivotVertex]);
}

void CPrimPolygon::Iron()
{
	m_vPosXAx[2] = 0;
	m_vPosYAx[2] = 0;

	for(WORD w = 0; w < m_wPts; w++)
		m_Pt[w][2] = 0;

	m_ptOrig[2] = 0;
}

bool CPrimPolygon::SelUsingPoint(const CPnt4& pt, double tol, CPnt& ptProj)
{
	double dProjDis;	
	
	if (mS_wEdgeToEvaluate > 0 && mS_wEdgeToEvaluate <= m_wPts)
	{	// Evaluate specified edge of polygon
		CPnt4 ptBeg(m_Pt[mS_wEdgeToEvaluate - 1], 1.);
		CPnt4 ptEnd(m_Pt[mS_wEdgeToEvaluate % m_wPts], 1.);

		ModelView_DoTransform(ptBeg);
		ModelView_DoTransform(ptEnd);

		if (line::EvalPtProx_xy(CLine(ptBeg, ptEnd), pt, tol, ptProj, &mS_dRel, &dProjDis)) 
		{
			ptProj[2] = ptBeg[2] + mS_dRel * (ptEnd[2] - ptBeg[2]);
			return true;
		}
	}
	else
	{	// Evaluate entire polygon
		CPnt4 ptBeg(m_Pt[0], 1.);
		ModelView_DoTransform(ptBeg);
		
		for (WORD w = 1; w <= m_wPts; w++) 
		{
			CPnt4 ptEnd(m_Pt[w % m_wPts], 1.);
			ModelView_DoTransform(ptEnd);
			
			if (line::EvalPtProx_xy(CLine(ptBeg, ptEnd), pt, tol, ptProj, &mS_dRel, &dProjDis)) 
			{
				ptProj[2] = ptBeg[2] + mS_dRel * (ptEnd[2] - ptBeg[2]);
				mS_wEdge = w;
				mS_wPivotVertex = m_wPts;
				return true;
			}
			ptBeg = ptEnd;
		}
	}
	return false;
}

bool CPrimPolygon::SelUsingRect(const CPnt& pt1, const CPnt& pt2)
{
	CPnts pts;
	for (WORD w = 0; w < m_wPts; w++)
		pts.Add(m_Pt[w]);

	return polyline::SelUsingRect(pt1, pt2, pts);
}
void CPrimPolygon::ModifyState()
{
	CPrim::ModifyState();
	
	m_nIntStyle = pstate.PolygonIntStyle();
	m_nIntStyleId = pstate.PolygonIntStyleId();
}

void CPrimPolygon::GetState()
{
	CPrim::GetState();

	pstate.SetPolygonIntStyle(m_nIntStyle);
	pstate.SetPolygonIntStyleId(m_nIntStyleId);
}

bool CPrimPolygon::PvtOnCtrlPt(const CPnt4& ptView) const
{
	if (mS_wPivotVertex >= m_wPts)
		// Not engaged at a vertex
		return false;
	
	CPnt4 ptCtrl(m_Pt[mS_wPivotVertex], 1.);
	ModelView_DoTransform(ptCtrl);
	
	if (Pnt4_DistanceTo_xy(ptCtrl, ptView) >= mS_dPicApertSiz)		
		// Not on proper vertex
		return false;

	if (mS_wPivotVertex == 0)					
		mS_wEdge = WORD(mS_wEdge == 1 ? m_wPts : 1); 
	else if (mS_wPivotVertex == m_wPts - 1)
		mS_wEdge = WORD(mS_wEdge == m_wPts ? mS_wEdge - 1 : m_wPts);
	else if (mS_wPivotVertex == mS_wEdge) 
		mS_wEdge++;
	else 
		mS_wEdge--;
	
	return true;
}	
void CPrimPolygon::GetAllPts(CPnts& pts)
{
	pts.SetSize(0);
	for (WORD w = 0; w < m_wPts; w++)
		pts.Add(m_Pt[w]);
}

// Determines the extent.
void CPrimPolygon::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
{
	CPnt pt;

	for (WORD w = 0; w < m_wPts; w++)
	{
		pt = m_Pt[w];
		mspace.Transform(pt);
		pt = tm * pt;
		ptMin = Min(ptMin, pt);
		ptMax = Max(ptMax, pt);
	}
}

bool CPrimPolygon::IsInView() const
{
	CPnt4 pt[2];
	
	pt[0] = m_Pt[0];
	ModelView_DoTransform(pt[0]);
	
	for (int i = m_wPts - 1; i >= 0; i--) 
	{
		pt[1] = m_Pt[i];
		ModelView_DoTransform(pt[1]);
	
		if (Pnt4_ClipLine(pt[0], pt[1]))
			return true;
		pt[0] = pt[1];
	}
	return false;
}

bool CPrimPolygon::IsPtACtrlPt(const CPnt4& ptPic) const
{
	for (WORD w = 0; w < m_wPts; w++)
	{
		CPnt4 pt(m_Pt[w], 1.);
		ModelView_DoTransform(pt);
		
		if (Pnt4_DistanceTo_xy(ptPic, pt) < mS_dPicApertSiz)
			return true;
	}
	return false;
}

void CPrimPolygon::ReadPeg(HFILE hFile)
{
	CPrim::ReadPeg(hFile);

	DWORD dwRead;
	
	m_nIntStyle = m_nPenStyle; // note polygon style stuffed up into unused pen style on io
	ReadFile((HANDLE)hFile, &m_nIntStyleId, sizeof(short), &dwRead, NULL);
	FilePeg_ReadWord2(hFile, m_wPts);
	m_ptOrig.Read(hFile);
	m_vPosXAx.Read(hFile);
	m_vPosYAx.Read(hFile);
	
	if (m_Pt != 0)
		delete [] m_Pt;

	m_Pt = new CPnt[m_wPts];

	for (WORD w = 0; w < m_wPts; w++)
		m_Pt[w].Read(hFile);
}

CPnt CPrimPolygon::SelAtCtrlPt(const CPnt4& ptPic) const
{
	mS_wCtrlPt = USHRT_MAX;
	double dApert = mS_dPicApertSiz;
	
	mS_wPivotVertex = m_wPts;

	for (WORD w = 0; w < m_wPts; w++) 
	{
		CPnt4 pt(m_Pt[w], 1.);
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
	return (mS_wCtrlPt == USHRT_MAX) ? ORIGIN : m_Pt[mS_wCtrlPt];
}

void CPrimPolygon::SetHatRefVecs(double dOffAng, double dXScal, double dYScal)
{
	m_vPosXAx = CVec(m_Pt[0], m_Pt[1]);
	m_vPosYAx = CVec(m_Pt[0], m_Pt[2]);
	
	CVec vPlnNorm(m_vPosXAx ^ m_vPosYAx);
	vPlnNorm.Normalize();

	if (vPlnNorm[2] < 0)
		vPlnNorm = - vPlnNorm;

	m_vPosXAx.Normalize();
	m_vPosXAx.RotAboutArbAx(vPlnNorm, dOffAng);
	m_vPosYAx = m_vPosXAx;
	m_vPosYAx.RotAboutArbAx(vPlnNorm, HALF_PI);
	m_vPosXAx *= dXScal;
	m_vPosYAx *= dYScal;
}

void CPrimPolygon::Transform(const CTMat& tm)
{
	m_ptOrig = tm * m_ptOrig;
	m_vPosXAx = tm * m_vPosXAx;
	m_vPosYAx = tm * m_vPosYAx;
	for (WORD w = 0; w < m_wPts; w++)
		m_Pt[w] = tm * m_Pt[w];
}

void CPrimPolygon::Translate(const CVec& v)
{
	m_ptOrig += v;
	for (WORD w = 0; w < m_wPts; w++) 
		m_Pt[w] += v;
}

void CPrimPolygon::TranslateUsingMask(const CVec& v, const DWORD dwMask)
{
	// nothing done to hatch coordinate origin
	
	for (WORD w = 0; w < m_wPts; w++)
	{
		if (((dwMask >> w) & 1UL) == 1) {m_Pt[w] += v;}
	}
}

bool CPrimPolygon::Vectorize(const CSeg *pSeg)
{
	if(m_nIntStyle == POLYGON_HOLLOW)
	{
		POSITION curPos = pSeg->Find(this);

		if(curPos == 0)
			return false;

		// polygon pen style is always 0
		PENSTYLE pStyle = PenStyle();
		if(pStyle == 0)
			pStyle = 1;

		for(int i = 0; i < m_wPts - 1; i++)
		{
			CPrimLine *pPrimLine = new CPrimLine(PenColor(), pStyle, m_Pt[i], m_Pt[i+1]);
			((CObList*)pSeg)->InsertAfter(curPos, pPrimLine);
		}

		CPrimLine *pPrimLine = new CPrimLine(PenColor(), pStyle, m_Pt[m_wPts-1], m_Pt[0]);
		((CObList*)pSeg)->InsertAfter(curPos, pPrimLine);

		return true;
	}

	return false;
}

bool CPrimPolygon::WritePeg(HFILE hFile) const
{
	FilePeg_WriteWord2(hFile, PRIM_POLYGON);
	FilePeg_WriteWord2(hFile, m_nPenColor);
	FilePeg_WriteWord2(hFile, m_nIntStyle);  // note polygon style stuffed up into unused pen style on io
	FilePeg_WriteWord2(hFile, m_nIntStyleId);
	FilePeg_WriteWord2(hFile, m_wPts);
	m_ptOrig.Write(hFile);
	m_vPosXAx.Write(hFile);
	m_vPosYAx.Write(hFile);
	
	for (WORD w = 0; w < m_wPts; w++)
		m_Pt[w].Write(hFile);

	return true;
}

///<summary>A fill area set primative with interior style hatch is generated using lines.</summary>
// Parameters:	pDC
//				iSets		number of point lists
//				iPtLstsId	starting indicies for point lists
void DisplayFilAreaHatch(CDC* pDC, CTMat& tm, const int iSets, const int* iPtLstsId, CPnt* pta)
{
	double dCurStrLen;
	double dEps1;
	double dMaxY;
	double dRemDisToEdg;
	double dScan;
	double dSecBeg;
	double dStrLen[8];
	int i;
	int i2;
	int iBegEdg;
	int iCurEdg;
	int iEndEdg;
	int iPts;
	int iStrId;
	
	CTMat tmInv;

	pFilAreaEdgLis edg[65];

	CLine ln;
	CLine lnS;
	CVec vEdg;

	PENCOLOR nPenColor = pstate.PenColor();
	PENSTYLE nPenStyle = pstate.PenStyle();

	//pstate.SetPenStyle(1);
	//pstate.SetPenColor(1);
	pstate.SetPen(pDC, nPenColor, 1);
	
	int iTblId = hatch::iTableOffset[pstate.PolygonIntStyleId()];
	int iHatLns = int(hatch::fTableValue[iTblId++]);

	for (int i0 = 0; i0 < iHatLns; i0++) 
	{
		int iStrs = int(hatch::fTableValue[iTblId++]);			// number of strokes in line definition
		double dTotStrLen = hatch::fTableValue[iTblId++];		// length of all strokes in line definition
		double dSinAng = sin(hatch::fTableValue[iTblId]);		// angle at which line will be drawn
		double dCosAng = cos(hatch::fTableValue[iTblId++]);
		double dX = hatch::fTableValue[iTblId++];				// displacement to origin of initial line
		double dY = hatch::fTableValue[iTblId++];
		double dShift = hatch::fTableValue[iTblId++];			// x-axis origin shift between lines
		double dSpac = hatch::fTableValue[iTblId++];			// spacing between lines

		for (i = 0; i < iStrs; i++) 							// length of each stoke in line definition
			dStrLen[i] = hatch::fTableValue[iTblId++];

		// Rotate origin on z0 plane so hatch x-axis becomes positive x-axis
		double dHatOrigX = dX * dCosAng - dY * (- dSinAng);
		double dHatOrigY = dX * (- dSinAng) + dY * dCosAng;

		// Add rotation to matrix which gets current scan lines parallel to x-axis
		CTMat tmRotZ;
		tm *= tmRotZ.RotateZ(- dSinAng, dCosAng);
		tmInv = tm;
		tmInv.Inverse();

		int iActEdgs = 0;
		int iBegPt = 0;

		for (i = 0; i < iSets; i++) 
		{
			if (i != 0)
				iBegPt = iPtLstsId[i - 1];
			ln[0] = pta[iBegPt];
			ln[0] = tm * ln[0];		// Apply transform to get areas first point in z0 plane
			
			iPts = iPtLstsId[i] - iBegPt;					// Determine number of points in current area
			for (i2 = iBegPt; i2 < (int) iPtLstsId[i]; i2++) 
			{
				ln[1] = pta[((i2 - iBegPt + 1) % iPts) + iBegPt];
				ln[1] = tm * ln[1];
				vEdg[0] = ln[1][0] - ln[0][0]; 					// Determine x and y-components of edge
				vEdg[1] = ln[1][1] - ln[0][1];
				if (fabs(vEdg[1]) > DBL_EPSILON * Vec_Length_xy(vEdg)) // Edge is not horizontal
				{
					dMaxY = Max((double) ln[0][1], (double) ln[1][1]);
					iCurEdg = iActEdgs + 1;
					// Find correct insertion point for edge in edge list using ymax as sort key
					while (iCurEdg != 1 && edg[iCurEdg - 1].dMaxY < dMaxY) 
					{
						edg[iCurEdg] = edg[iCurEdg - 1];		// Move entry down
						iCurEdg--;
					}
					// Insert information about new edge
					edg[iCurEdg].dMaxY = dMaxY;
					edg[iCurEdg].dInvSlope = vEdg[0] / vEdg[1];
					if (ln[0][1] > ln[1][1]) 
					{
						edg[iCurEdg].dMinY = ln[1][1];
						edg[iCurEdg].dX = ln[0][0];
					}
					else 
					{
						edg[iCurEdg].dMinY = ln[0][1];
						edg[iCurEdg].dX = ln[1][0];
					}
					iActEdgs++; 								// Increment count of active edges in edge list
				}
				ln[0] = ln[1];
			}
		}
		// Determine where first scan position is
		dScan = edg[1].dMaxY - fmod((edg[1].dMaxY - dHatOrigY), dSpac);
		if (edg[1].dMaxY < dScan) 
			dScan = dScan - dSpac;
		dSecBeg = dHatOrigX + dShift * (dScan - dHatOrigY) / dSpac;
		// Edge list pointers
		iBegEdg = 1;
		iEndEdg = 1;
		// Determine relative epsilon to be used for extent tests
l1: 	dEps1 = DBL_EPSILON + DBL_EPSILON * fabs(dScan);
		while (iEndEdg <= iActEdgs && edg[iEndEdg].dMaxY >= dScan - dEps1) 
		{
			// Set x intersection back to last scanline
			edg[iEndEdg].dX += edg[iEndEdg].dInvSlope * (dSpac + dScan - edg[iEndEdg].dMaxY);
			// Determine the change in x per scan
			edg[iEndEdg].dStepSiz = - edg[iEndEdg].dInvSlope * dSpac;
			iEndEdg++;
		}
		for (i = iBegEdg; i < iEndEdg; i++) 
		{
			iCurEdg = i;
			if (edg[i].dMinY < dScan - dEps1) 
			{
				// Edge y-extent overlaps current scan . determine intersections
				edg[i].dX += edg[i].dStepSiz;
				while (iCurEdg > iBegEdg && edg[iCurEdg].dX < edg[iCurEdg - 1].dX) 
				{
					edg[0] = edg[iCurEdg];
					edg[iCurEdg] = edg[iCurEdg - 1];
					edg[iCurEdg - 1] = edg[0];
					iCurEdg--;
				}
			}
			else												// Edge y-extent does not overlap current scan 
			{													// remove edge from active edge list
				iBegEdg++;
				while (iCurEdg >= iBegEdg) 
				{
					edg[iCurEdg] = edg[iCurEdg - 1];
					iCurEdg--;
				}
			}
		}
		if (iEndEdg != iBegEdg) 
		{
			// At least one pair of edge intersections .. generate scan lines for each pair
			iCurEdg = iBegEdg;
			lnS[0][1] = dScan;
			lnS[1][1] = dScan;
			for (i = 1; i <= (iEndEdg - iBegEdg) / 2; i++) 
			{
				lnS[0][0] = edg[iCurEdg].dX - fmod((edg[iCurEdg].dX - dSecBeg), dTotStrLen);
				if (lnS[0][0] > edg[iCurEdg].dX)
					lnS[0][0] -= dTotStrLen;
				iStrId = 0;
				dRemDisToEdg = edg[iCurEdg].dX - lnS[0][0];
				dCurStrLen = dStrLen[iStrId];
				while (dCurStrLen <= dRemDisToEdg + DBL_EPSILON) 
				{
					lnS[0][0] += dCurStrLen;
					dRemDisToEdg -= dCurStrLen;
					iStrId = (iStrId + 1) % iStrs;
					dCurStrLen = dStrLen[iStrId];
				}
				lnS[0][0] = edg[iCurEdg].dX;
				dCurStrLen -= dRemDisToEdg;
				dRemDisToEdg = edg[iCurEdg + 1].dX - edg[iCurEdg].dX;
				while (dCurStrLen <= dRemDisToEdg + DBL_EPSILON) 
				{
					lnS[1][0] = lnS[0][0] + dCurStrLen;
					if ((iStrId & 1) == 0) 
					{
						ln = tmInv * lnS;
						ln.Display(pDC);
					}
					dRemDisToEdg -= dCurStrLen;
					iStrId = (iStrId + 1) % iStrs;
					dCurStrLen = dStrLen[iStrId];
					lnS[0][0] = lnS[1][0];
				}
				if (dRemDisToEdg > DBL_EPSILON && (iStrId & 1) == 0) 
				{
					// Partial component of dash section must produced
					lnS[1][0] = edg[iCurEdg + 1].dX;
					ln = tmInv * lnS;
					ln.Display(pDC);
				}
				iCurEdg = iCurEdg + 2;
			}
			// Update position of scan line
			dScan -= dSpac;
			dSecBeg -= dShift;
			goto l1;
		}
		tm *= tmRotZ.RotateZ(dSinAng, dCosAng);
	}

	pstate.SetPen(pDC, nPenColor, nPenStyle);
}

WORD CPrimPolygon::SwingVertex() const
{
	WORD wSwingVertex;
	
	if (mS_wPivotVertex == 0)
		wSwingVertex = WORD(mS_wEdge == 1 ? 1 : m_wPts - 1); 
	else if (mS_wPivotVertex == WORD(m_wPts - 1))										
		wSwingVertex = WORD(mS_wEdge == m_wPts ? 0 : mS_wPivotVertex - 1);
	else
		wSwingVertex = WORD(mS_wEdge == mS_wPivotVertex ? mS_wPivotVertex - 1 : mS_wPivotVertex + 1);
	
	return (wSwingVertex);
}