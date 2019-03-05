#include "stdafx.h"

#include "Line.h"
#include "Polyline.h"

CLine CLine::operator-(const CVec& v) const
{
	return (CLine(m_pt[0] - v, m_pt[1] - v));
} 

CLine CLine::operator+(const CVec& v) const
{
	return (CLine(m_pt[0] + v, m_pt[1] + v));
}

///<summary>Determines if lines are parallel.</summary>
bool CLine::operator||(const CLine& r) const
{	
	CVec vBeg1End1(m_pt[0], m_pt[1]);
	CVec vBeg2End2(r.m_pt[0], r.m_pt[1]);

	double dDet = vBeg1End1[0] * vBeg2End2[1] - vBeg2End2[0] * vBeg1End1[1];

	return (fabs(dDet) > DBL_EPSILON) ? false : true;
}

///<summary>Cuts a line a point.</summary>
WORD CLine::CutAtPt(const CPnt& pt, CLine& ln)
{
	WORD wRet = 0;
	
	ln = *this;

	if (pt != m_pt[0] && pt != m_pt[1])
	{
		ln.m_pt[1] = pt;
		m_pt[0] = pt;
		
		wRet++;
	}
	return (wRet);
}

///<summary>Determines which side of a directed line a point is on.</summary>
// Notes:	Relation is found using determinant (3rd order).
//				d  = begx * (endy - y) - endx * (begy - y) + x * (begy - endy)
// Returns: 1 point is to left of line
//			0 point is on line
//			- 1 point is to right of line
int CLine::DirRelOfPt(const CPnt& pt) const
{
	double dDet = m_pt[0][0] * (m_pt[1][1] - pt[1]) - 
				  m_pt[1][0] * (m_pt[0][1] - pt[1]) + 
				  pt[0] * (m_pt[0][1] - m_pt[1][1]);
	
	if (dDet > DBL_EPSILON)
		return (1);
	else if (dDet < - DBL_EPSILON)
		return (- 1);
	else
		return 0;
}

void CLine::Display(CDC* pDC) const	
{
	PENSTYLE nPenStyle = pstate.PenStyle();
	
	if(CPrim::IsSupportedTyp(nPenStyle))
//	if(nPenStyle == 1)
	{
		CPnt4 pt[] = {CPnt4(m_pt[0], 1.), CPnt4(m_pt[1], 1.)};
	
		ModelView_DoTransform(2, pt);

		if(Pnt4_ClipLine(pt[0], pt[1]))
		{
			CPoint pnt[2];
			viewport::DoProjection(pnt, 2, pt);
			pDC->Polyline(pnt, 2);
		}
	}
	else
	{
		polyline::BeginLineStrip();
		polyline::SetVertex(m_pt[0]);
		polyline::SetVertex(m_pt[1]);
		polyline::End(pDC, nPenStyle);
	}
}

double CLine::Length() const
{
	CVec v(m_pt[0], m_pt[1]);

	return (v.Length());
}

///<summary>Generates coordinate sets for parallel lines.</summary>
// Notes:	The first of the two parallel lines lies to the left of line,
//			and the second to the right.
//			Eccentricity: 0. on line, > 0. to left of line, < 0. to right of line
bool CLine::GetParallels(double dDis, double dEcc, CLine& lnLeft, CLine& lnRight) const
{
	lnLeft = *this;
	lnRight = *this;
	
	double dLen = Length();
	
	if (dLen > DBL_EPSILON)
	{
		double dX = (m_pt[1][1] - m_pt[0][1]) * dDis / dLen;
		double dY = (m_pt[1][0] - m_pt[0][0]) * dDis / dLen;
	
		dEcc += .5;
	
		lnLeft += CVec(- dX * dEcc, dY * dEcc, 0.);
		lnRight += CVec(dX * (1. - dEcc),  - dY * (1. - dEcc), 0.);

		return true;
	}
	return false;
}

CVec CLine::GetVector() const
{
	return CVec(m_pt[0], m_pt[1]);
}

///<summary>Projects a point onto line.</summary>
CPnt CLine::ProjPt(const CPnt& pt) const
{
	CVec vBegEnd(m_pt[0], m_pt[1]);

	double dSum = vBegEnd.SquaredLength();
	
	if (dSum > DBL_EPSILON)
	{
		CVec vBegPt(m_pt[0], pt);

		double dScale = (vBegPt * vBegEnd) / dSum;

		vBegEnd *= dScale;
	}
	return (m_pt[0] + vBegEnd);
}

CPnt CLine::ProjPtAlong(double dT) const
{	
	// Determines the coordinates of point projected along a line.
	// t = 0 point is the begin point
	// t = 1 point is the end point
	// The range of values for t is not clamped to this interval

	return m_pt[0] + (m_pt[1] - m_pt[0]) * dT;
}	

///<summary>Projects end point toward or beyond the begin point of line.</summary>
CPnt CLine::ProjToBegPt(double dDis) const
{
	CVec vEndBeg(m_pt[1], m_pt[0]);
	
	double dLen = vEndBeg.Length();
	
	if (dLen > DBL_EPSILON) 
		vEndBeg *= dDis / dLen;
	
	return (m_pt[1] + vEndBeg);
}

// Effect: Projects begin point toward or beyond the end point of line.
CPnt CLine::ProjToEndPt(double dDis) const
{
	CVec vBegEnd(m_pt[0], m_pt[1]);
	
	double dLen = vBegEnd.Length();
	
	if (dLen > DBL_EPSILON) 
		vBegEnd *= dDis / dLen;
	
	return (m_pt[0] + vBegEnd);
}

void CLine::Read(HFILE hFile)
{
	m_pt[0].Read(hFile);
	m_pt[1].Read(hFile);
}

void CLine::Write(HFILE hFile) const
{
	m_pt[0].Write(hFile);
	m_pt[1].Write(hFile);
}

///<summary>Determines the angle between two lines.</summary>
// Notes:  Angle is found using the inner product.
//				  v1 dot v2
//		 ang = acos ( ----------------- )
//			  len(v1) * len(v2)
//	   Angle is between 0 and 2 pi. If angle is 0 lines are in same
//	   direction and if angle is pi lines are in opposite direction.
//	   To get acute angle, all angles greater than half pi must be
//	   subtracted from pi.
// Returns: angle between lines (in radians)
// Parameters:	ln1		first line
//				ptBeg2	begin point of second line
//				ptEnd2	end point of second line
double line::AngleBetweenLn_xy(const CLine& ln1, const CLine& ln2)	
{
	CVec v1(ln1); v1[2] = 0.;
	CVec v2(ln2); v2[2] = 0.;

	double dSumProd = v1.SquaredLength() * v2.SquaredLength();

	if (dSumProd > DBL_EPSILON) 
	{
		double dVal = (v1 * v2) / sqrt(dSumProd);
		
		dVal = Max(- 1., Min(1., dVal));
		
		return (acos(dVal));
	}
	return (0.);
}

///<summary>Constrains a line to nearest axis pivoting on first endpoint.</summary>
// Notes:	Offset angle only support about z-axis
// Returns: Point after snap
CPnt line::ConstrainToAx(const CLine& ln, double dInfAng, double dAxOffAng)
{	
	CTMat tm;
	tm.Translate(ORIGIN - ln[0]);
	
	CTMat tmZRot;
	tm *= tmZRot.RotateZ(- sin(dAxOffAng * RADIAN), cos(dAxOffAng * RADIAN));
	
	CPnt pt = ln[1];
	
	pt = tm * pt;

	double dX = pt[0] * pt[0];
	double dY = pt[1] * pt[1];
	double dZ = pt[2] * pt[2];

	double dLen = sqrt(dX + dY + dZ);
	
	if (dLen > DBL_EPSILON)
	{	// Not a zero length line
		if (dX >= Max(dY, dZ))
		{	// Major component of line is along x-axis
			dLen = sqrt(dY + dZ);
			if (dLen > DBL_EPSILON) 				// Not already on the x-axis
				if (dLen / fabs(pt[0]) < tan(dInfAng * RADIAN)) 
				{	// Within cone of influence .. snap to x-axis
					pt[1] = 0.; 
					pt[2] = 0.;
				}
		}
		else if (dY >= dZ)							
		{	// Major component of line is along y-axis
			dLen = sqrt(dX + dZ);
			if (dLen > DBL_EPSILON)					// Not already on the y-axis
				if (dLen / fabs(pt[1]) < tan(dInfAng * RADIAN)) 
				{	// Within cone of influence .. snap to y-axis
					pt[0] = 0.; 
					pt[2] = 0.;
				}
		}
		else 
		{
			dLen = sqrt(dX + dY);
			if (dLen > DBL_EPSILON)					// Not already on the z-axis
				if (dLen / fabs(pt[2]) < tan(dInfAng * RADIAN)) 
				{	// Within cone of influence .. snap to z-axis
					pt[0] = 0.; 
					pt[1] = 0.;
				}
		}
	}
	tm.Inverse();
	pt = tm * pt;
	return (pt);
}

///<summary>
///Evaluates the proximity of a point to a line segment.
///The distance from a point to a line segment is the distance to the line only 
///if the normal from the point to the line strikes the line between the segment endpoints.	
///Otherwise it is the distance from the point to the nearest endpoint.
///</summary>
//		The value of parameter t is found from:
//
//		   Rel = -[(Begx - Px)(Endx - Begx) + (Begy - Py)(Endy - Begy)]
//			   ------------------------------------------
//			  (Endx - Begx)^2 + (Endy - Begy)^2
//
//		If nearest point is in the segment, the distance to it is:
//
//		   Dis = sqrt{[(Begx - Px) + Rel(Endx - Begx)]^2 + [(Begy - Py) + Rel(Endy - Begy)]^2}
//
//		By truncating the value of Rel to be between 0 and 1 Dis is correct
//		for all cases.
// Returns: true if point is within acceptance aperture of line segment
//		false otherwise
bool line::EvalPtProx_xy(const CLine& ln, const CPnt& pt, const double adApert, CPnt& ptProj, double* adRel, double* adDis)
{
	double dApert = adApert;

	if (pt[0] < Min(ln.m_pt[0][0], ln.m_pt[1][0]) - dApert) return false;
	if (pt[0] > Max(ln.m_pt[0][0], ln.m_pt[1][0]) + dApert) return false;
	if (pt[1] < Min(ln.m_pt[0][1], ln.m_pt[1][1]) - dApert) return false;
	if (pt[1] > Max(ln.m_pt[0][1], ln.m_pt[1][1]) + dApert) return false;

	double dPBegX = ln.m_pt[0][0] - pt[0];
	double dPBegY = ln.m_pt[0][1] - pt[1];
	
	double dBegEndX = ln.m_pt[1][0] - ln.m_pt[0][0];
	double dBegEndY = ln.m_pt[1][1] - ln.m_pt[0][1];

	double dDivr = dBegEndX * dBegEndX + dBegEndY * dBegEndY;
	if (dDivr <= DBL_EPSILON) 
	{
		*adRel = 0.;
		*adDis = dPBegX * dPBegX + dPBegY * dPBegY;
	}
	else 
	{
		*adRel = - (dPBegX * dBegEndX + dPBegY * dBegEndY) / dDivr;
		*adRel = Max(0., Min(1., *adRel));
		double dx = dPBegX + *adRel * dBegEndX;
		double dy = dPBegY + *adRel * dBegEndY;
		*adDis = dx * dx + dy * dy;
	}
	if (*adDis > adApert * adApert) 
		return false;
	
	*adDis = sqrt(*adDis);
	ptProj[0] = ln[0][0] + (*adRel * dBegEndX);
	ptProj[1] = ln[0][1] + (*adRel * dBegEndY);

	return true;
}

///<summary>Determines the xy extent of a line.</summary>
void line::Extents_xy(const CLine& ln, CPnt& arLo, CPnt& arHi)
{
	arLo[0] = Min(ln[0][0], ln[1][0]);
	arLo[1] = Min(ln[0][1], ln[1][1]);
	
	arHi[0] = Max(ln[0][0], ln[1][0]);
	arHi[1] = Max(ln[0][1], ln[1][1]);
}

///<summary>Determines the angle of a line defined by 2 points.</summary>
// Notes: If null length or parallel to z-axis, angle is 0.
// Returns: Angle in radians (0 to 2 pi)
double line::GetAngAboutZAx(const CLine& ln)
{
	CVec vBegEnd(ln);

	double dAng = 0.;
	
	if (fabs(vBegEnd[0]) > DBL_EPSILON || fabs(vBegEnd[1]) > DBL_EPSILON)
	{
		dAng = atan2(vBegEnd[1], vBegEnd[0]);
		
		if (dAng < 0.)
			dAng += TWOPI;
	}	
	return (dAng);
}

///<summary> Finds intersection of two lines in space.</summary>
// Returns:  TRUE intersection determined
//		 FALSE endpoints of line 1 or 2 coincide or
//		   2 lines are parallel or 4 points are not coplanar
int line::Intersection(const CLine& ln1, const CLine& ln2, CPnt& ptInt)
{
	CVec v1(ln1[0], ln1[1]);
	if (v1.IsNull())		// Endpoints of first line coincide
		return (FALSE);

	CVec v2(ln2[0], ln2[1]);
	if (v2.IsNull())			// Endpoints of second line coincide
		return (FALSE);

	CVec vPlnNorm(v1 ^ v2);
	vPlnNorm.Normalize();
	if (vPlnNorm.IsNull())			// Two lines are parallel
		return (FALSE);

	CVec v3(ln1[0], ln2[0]);
	if (fabs((vPlnNorm * v3)) > DBL_EPSILON) // Four points are not coplanar
		return (FALSE);

	CTMat tm(ln1[0], vPlnNorm);

	CPnt rL1P1;
	CPnt rL1P2(ln1[1]);
	rL1P2 = tm * rL1P2;
	CPnt rL2P1(ln2[0]);
	rL2P1 = tm * rL2P1;
	CPnt rL2P2(ln2[1]);
	rL2P2 = tm * rL2P2;
	
	if (line::Intersection_xy(CLine(rL1P1, rL1P2), CLine(rL2P1, rL2P2), ptInt)) 
	{
		ptInt[2] = 0.;
		tm.Inverse();
		ptInt = tm * ptInt;

		return (TRUE);
	}
	return (FALSE);
}

///<summary>Determines intersection of two lines.</summary>
// Returns: true successful completion
//			false ohterwise (parallel lines)
bool line::Intersection_xy(const CLine& ln1, const CLine& ln2, CPnt& ptInt)
{	
	CVec vBeg1End1 = ln1.GetVector();
	CVec vBeg2End2 = ln2.GetVector();

	double dDet = vBeg1End1[0] * vBeg2End2[1] - vBeg2End2[0] * vBeg1End1[1];
	
	if (fabs(dDet) > DBL_EPSILON) 
	{
		CVec vBeg1Beg2(ln1[0], ln2[0]);
		
		double dT = (vBeg1Beg2[1] * vBeg2End2[0] - vBeg2End2[1] * vBeg1Beg2[0]) / dDet;
		
		vBeg1End1 *= dT;
		ptInt = ln1[0] - vBeg1End1;
		return true;
	}
	return false;
}

///<summary>Determines the intersection of a line and a plane.</summary>
// Notes:	Line is defined using parametric representation. Plane is
//			defined by its normal vector and any point on plane.
// Parameters:	
//			ptEnd		one endpoint of line
//			vDir		vector defining orientation of line
//			ptOnPln 	point on plane
//			vPlnNorm	vector normal to plane
//			ptInt		intersection on line an plane
// Returns: true if intersection determined and false if line parallel to plane
bool line::IntersectionWithPln(const CPnt& ptEnd, const CVec& vDir, const CPnt& ptOnPln, const CVec& vPlnNorm, CPnt *ptInt)	
{	
	double dDotProd = vPlnNorm * vDir;
	
	if (fabs(dDotProd) > DBL_EPSILON)									// Line and plane are not parallel
	{
		CVec v(vDir);
		CVec vOnPln(ORIGIN, ptOnPln);			
		CVec vEnd(ORIGIN, ptEnd);
		
		double dD = - (vPlnNorm * vOnPln);						// Coefficient of plane on which point lies
		double dT = - ((vPlnNorm * vEnd) + dD) / dDotProd;		// Parameter on the line where it intersects the plane
		
		v *= dT;
		*ptInt = ptEnd + v;
		return true;
	}
	return false; 											// Line and plane are parallel
}

///<summary>
///Calculates intersection point of line and a plane. Should only be used if points are
///known to be on opposite sides of the plane.
///</summary>
// Parameters:
//			pt0		begin point of line
//			pt1		end point of line
//			ptOnPln		any point on clip plane
//			vPlnNorm	clip plane normal vector
// Returns: Intersection point. 
//			If line and plane are parallel begin point of line is returned. Not good!
CPnt4 line::IntersectionWithPln(const CPnt4& pt0, const CPnt4& pt1, const CPnt4& ptOnPln, const CVec4& vPlnNorm)
{
	CVec4 vDir(pt0, pt1);
	double dDotProd = vPlnNorm * vDir;
	
	if (fabs(dDotProd) > DBL_EPSILON)
	{
		CVec4 vPtPt0(ptOnPln, pt0);
		vDir *= (vPlnNorm * vPtPt0) / dDotProd;
	}
	else
	{	// Line and the plane are parallel .. force return to begin point
		vDir *= 0.;
	}
	return (pt0 - vDir);
}

///<summary>
///Projects a point from begin point toward end point the parallel projection distance. 
///The resulting point is then projected perpendicular to the line defined by the two points the
///perpendicular projection distance. 
///</summary>
// Notes:	A positive perpendicular projection distance will result in a point to
//			the left of the direction vector defined by the two points.
//			Projected point is undefined if point one and point two coincide.
// Returns: TRUE  successful completion
//			FALSE failure (p1 and p2 coincide)
int line::ProjPtFrom_xy(const CLine& ln, double dParDis, double dPerDis, CPnt* arProjPt)
{
	double dX = ln[1][0] - ln[0][0];
	double dY = ln[1][1] - ln[0][1];

	double dLen = sqrt(dX * dX + dY * dY);
	
	if (dLen <= DBL_EPSILON)
		return (FALSE);
	
	double dRatio;
	*arProjPt = ln[0];
	if (fabs(dParDis) > DBL_EPSILON)
	{
		dRatio = dParDis / dLen;
		dLen = dParDis;
		dX = dRatio * dX;
		dY = dRatio * dY;
		(*arProjPt)[0] = ln[0][0] + dX;
		(*arProjPt)[1] = ln[0][1] + dY;
	}
	if (fabs(dPerDis) > DBL_EPSILON)
	{
		dRatio = dPerDis / dLen;
		(*arProjPt)[0] -= dRatio * dY;
		(*arProjPt)[1] += dRatio * dX;
	}
	return (TRUE);
}

///<summary>
///Determines the relationship of a point on a line to the endpoints defining the line.
///</summary>
//			parametric relationship of point to line endpoints
//			< 0 point to left of directed segment
//			= 0 point same as first endpoint of line
//			> 0 < 1 point between endpoints of line
//			= 1 point same as second endpoint of line
//			> 1 point to right of directed segment
// Notes:  Results unpredictable if point does not lie on the line.
	 
// Returns: true  successful completion
//			false coincidental endpoints .. relationship undefined
bool line::RelOfPtToEndPts(const CLine& ln, const CPnt& pt, double& dRel) 
{
	CVec v(ln);
	
	if (fabs(v[0]) > DBL_EPSILON)
	{
		dRel = (pt[0] - ln[0][0]) / v[0];
		return true;
	}
	
	if (fabs(v[1]) > DBL_EPSILON)
	{
		dRel = (pt[1] - ln[0][1]) / v[1];
		return true;
	}
	
	if (fabs(v[2]) > DBL_EPSILON)
	{
		dRel = (pt[2] - ln[0][2]) / v[2];
		return true;
	}
	
	return false;
}

///<summary>Determines if line segment in wholly or partially contained within window passed.</summary>
// Notes:  Assumes window passed with min/max corners correct.
// Returns: TRUE line is wholly or partially within window
//			FALSE otherwise
bool p2DEvalWndLn(const CPnt& ptLL, const CPnt& ptUR, const CPnt& ptBeg, const CPnt& ptEnd)
{
	CPnt	pt[2];
	pt[0] = ptBeg; pt[1] = ptEnd;

	double	dX = ptEnd[0] - ptBeg[0];
	double	dY = ptEnd[1] - ptBeg[1];
	int 	i = 1;
	
	int 	iOut[2];
	iOut[0] = p2DRelOfPtToWnd(pt[0], ptLL, ptUR);

	for (;;)
	{
		iOut[i] = p2DRelOfPtToWnd(pt[i], ptLL, ptUR);

		if (iOut[0] == 0 && iOut[1] == 0) 
			return true;
		if ((iOut[0] & iOut[1]) != 0)
			return false;
		i = (iOut[0] == 0) ? 1 : 0;

		if ((iOut[i] & 1) == 1) 								// Above window
		{
			pt[i][0] = pt[i][0] + dX * (ptUR[1] - pt[i][1]) / dY;
			pt[i][1] = ptUR[1];
		}
		else if ((iOut[i] & 2) == 2)							// Below window
		{
			pt[i][0] = pt[i][0] + dX * (ptLL[1] - pt[i][1]) / dY;
			pt[i][1] = ptLL[1];
		}
		else if ((iOut[i] & 4) == 4) 
		{
			pt[i][1] = pt[i][1] + dY * (ptUR[0] - pt[i][0]) / dX;
			pt[i][0] = ptUR[0];
		}
		else 
		{
			pt[i][1] = pt[i][1] + dY * (ptLL[0] - pt[i][0]) / dX;
			pt[i][0] = ptLL[0];
		}
	}
}

int pFndSwpAngGivPlnAnd3Lns(const CVec& vPlnNorm, const CPnt arP1, const CPnt arP2, const CPnt arP3, const CPnt& ptCP, double *adTheta)
{
	double dT[3];
	CPnt   rR[3];

	if (arP1 == ptCP || arP2 == ptCP || arP3 == ptCP)
		return (FALSE);

	// None of the points coincide with center point
	CTMat tm(ptCP, vPlnNorm);
	rR[0] = arP1;
	rR[1] = arP2;
	rR[2] = arP3;
	for (int i = 0; i < 3; i++) 
	{	// Translate points into z=0 plane with center point at origin
		rR[i] = tm * rR[i];
		dT[i] = atan2(rR[i][1], rR[i][0]);
		if (dT[i] < 0.)
			dT[i] += TWOPI;
	}
	double dTMin = Min(dT[0], dT[2]);
	double dTMax = Max(dT[0], dT[2]);
	if (fabs(dT[1] - dTMax) > DBL_EPSILON && fabs(dT[1] - dTMin) > DBL_EPSILON) 
	{	// Inside line is not colinear with outside lines
		double dTheta = dTMax - dTMin;
		if (dT[1] > dTMin && dT[1] < dTMax) 
		{
			if (dT[0] == dTMax)
				dTheta = - dTheta;
		}
		else 
		{
			dTheta = TWOPI - dTheta;
			if (dT[2] == dTMax)
				dTheta = - dTheta;
		}
		*adTheta = dTheta;

		return (TRUE);
	}
	return (FALSE);
}