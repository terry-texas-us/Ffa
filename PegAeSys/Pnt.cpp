#include "stdafx.h"

CPnt::CPnt(const CPnt4& pt) 
{
	m_d[0] = pt[0] / pt[3];
	m_d[1] = pt[1] / pt[3];
	m_d[2] = pt[2] / pt[3];
}

CPnt& CPnt::operator=(const CPnt& pt) 
{
	m_d[0] = pt.m_d[0];
	m_d[1] = pt.m_d[1];
	m_d[2] = pt.m_d[2];
	
	return *this;
}

void CPnt::Read(CFile& fl)
{
	fl.Read(m_d, 3 * sizeof(double));
}

void CPnt::Read(HFILE hFile)
{
	DWORD dwRead;
	ReadFile((HANDLE)hFile, m_d, 3 * sizeof(double), &dwRead, NULL);
}

CString CPnt::ToString() const
{
	CString str;
	str.Format("%f;%f;%f\t", m_d[0], m_d[1], m_d[2]);	
	return (str);
}

void CPnt::Write(CFile& fl) const
{
	fl.Write(m_d, 3 * sizeof(double));
}

void CPnt::Write(HFILE hFile) const
{
	DWORD dwWrite;
	WriteFile((HANDLE)hFile, m_d, 3 * sizeof(double), &dwWrite, NULL);
}

CPnt operator+(const CPnt& pt, const CVec& v)
{
	return CPnt(pt.m_d[0] + v.m_d[0], pt.m_d[1] + v.m_d[1], pt.m_d[2] + v.m_d[2]);
}

CVec operator-(const CPnt& ptA, const CPnt& ptB)
{
	return CVec(ptA.m_d[0] - ptB.m_d[0], ptA.m_d[1] - ptB.m_d[1], ptA.m_d[2] - ptB.m_d[2]);
}	

CPnt operator-(const CPnt& pt, const CVec& v)
{
	return CPnt(pt.m_d[0] - v.m_d[0], pt.m_d[1] - v.m_d[1], pt.m_d[2] - v.m_d[2]);
}

bool operator==(const CPnt& ptA, const CPnt& ptB)
{ 
	return (ptA.m_d[0] == ptB.m_d[0]) && (ptA.m_d[1] == ptB.m_d[1]) && (ptA.m_d[2] == ptB.m_d[2]); 
}

bool operator!=(const CPnt& ptA, const CPnt& ptB)
{ 
	return !(ptA == ptB);
}

CPnt Max(const CPnt& ptA, const CPnt& ptB)
{
	return CPnt(Max(ptA.m_d[0], ptB.m_d[0]), Max(ptA.m_d[1], ptB.m_d[1]), Max(ptA.m_d[2], ptB.m_d[2]));
}

CPnt Mid(const CPnt& ptA, const CPnt& ptB)		
{
	return ptA + .5 * (ptB - ptA);
}

CPnt Min(const CPnt& ptA, const CPnt& ptB)
{
	return CPnt(Min(ptA.m_d[0], ptB.m_d[0]), Min(ptA.m_d[1], ptB.m_d[1]), Min(ptA.m_d[2], ptB.m_d[2]));
}

///<summary>Determines the distance between two points.</summary>
double Pnt_DistanceTo_xy(const CPnt& pt1, const CPnt& pt2)
{	
	CVec v(pt1, pt2); v[2] = 0.;
	return (v.Length());
}

///<summary>Projects a point toward or beyond another point.</summary>
// Returns: projected point
// Parameters:	pt		point
//				ptTo	point defining direction vector
//				dDis	magnitude of projection

CPnt Pnt_ProjPtTo(const CPnt& pt, const CPnt& ptTo, const double dDis)		
{
	CVec v(pt, ptTo);
	
	double dLen = v.Length();
	if (dLen > DBL_EPSILON) 
	{
		v *= dDis / dLen;
		return (pt + v);
	}
	return (pt);
}

///<summary>Projects a point toward another point the projection distance.</summary>
// Returns: projected point (just point one if points coincide)
CPnt Pnt_ProjPtTo_xy(const CPnt& pt, const CPnt& ptTo, const double dDis)
{
	CVec v(pt, ptTo); v[2] = 0.;
	
	double dLen = v.Length();
	
	if (dLen > DBL_EPSILON)
	{
		v *= dDis / dLen;
		return (pt + v);
	}
	return (pt);
}

///<summary>Rotates a point about another point in plane defined by the point and z-axis.</summary>
// Returns: point after rotaion .. p[2] unchanged
CPnt Pnt_RotateAboutPt_xy(CPnt pt, const CPnt& ptPvt, const double dAng)
{
	double dSinAng = sin(dAng);
	double dCosAng = cos(dAng);

	CVec v(ptPvt, pt); v[2] = 0.;

	pt.m_d[0] = ptPvt.m_d[0] + (v[0] * dCosAng - v[1] * dSinAng);
	pt.m_d[1] = ptPvt.m_d[1] + (v[0] * dSinAng + v[1] * dCosAng);

	return (pt);
}

///<summary>Determines if a point is contained by a window.</summary>
// Returns: true iff point is in window
//			FALSE otherwise
bool p2DPtInWnd(const CPnt& pt, CPnt* ptWnd)
{
	double dReps = DBL_EPSILON + fabs(DBL_EPSILON * pt[0]);
	
	if (ptWnd[0][0] > pt[0] + dReps || ptWnd[1][0] < pt[0] - dReps)
		return false;
	
	dReps = DBL_EPSILON + fabs(DBL_EPSILON * pt[1]);
	if (ptWnd[0][1] > pt[1] + dReps || ptWnd[1][1] < pt[1] - dReps)
		return false;
	
	return true;
}

///<summary>Determines relationship of a point to a window.</summary>
// Returns: 
//		0 - point is contained in window
//		bit 1 set - point above window
//		bit 2 set - point below window
//		bit 4 set - point to the right of window
//		bit 8 set - point to the left of window
int p2DRelOfPtToWnd(const CPnt& pt, const CPnt& ptLL, const CPnt& ptUR)
{
	int iRetVal = 0;
	
	if (pt[1] > ptUR[1] + DBL_EPSILON) 
	{
		iRetVal = 1;
	}
	else if (pt[1] < ptLL[1] - DBL_EPSILON) 
	{
		iRetVal = 2;
	}
	if (pt[0] > ptUR[0] + DBL_EPSILON) 
	{
		iRetVal |= 4;
	}
	else if (pt[0] < ptLL[0] - DBL_EPSILON) 
	{
		iRetVal |= 8;
	}
	return (iRetVal);
}

///<summary>Rotates a point about another point and arbitrary axis in space.</summary>
// Returns: Point after rotation
// Parameters:	pt		point to be rotated
//				ptRef	point about which rotation will occur
//				vRef	unit vector defining rotation axis
//				adAng	rotation angle (ccw positive) in radians
CPnt Pnt_RotAboutArbPtAndAx(const CPnt& pt, const CPnt& ptRef, const CVec& vRef, double adAng)	
{
	CTMat tm(ptRef, vRef, adAng);

	CPnt ptRot = pt;
	ptRot = tm * ptRot;
	return (ptRot);
}

///<summary>Finds center point of a circle given radius and two tangent vectors.</summary>

// Notes:	A radius and two lines define four center points.  The center point
//			selected is on the concave side of the angle formed by the two vectors
//			defined by the line endpoints.	These two vectors are oriented with
//			the tail of the second vector at the head of the first.

// Returns: TRUE	center point determined
//			FALSE	endpoints of first line coincide or endpoints of second line coincide or
//					two lines are parallel or four points are not coplanar
bool pFndCPGivRadAnd4Pts(double adRad, CPnt arLn1Beg, CPnt arLn1End, CPnt arLn2Beg, CPnt arLn2End, CPnt* ptCP)
{
	double	dA1, dA2, dB1, dB2, dC1RAB1, dC2RAB2, dDet, dSgnRad, dV1Mag, dV2Mag;
	CVec vPlnNorm;

	CVec v1(arLn1Beg, arLn1End);					// Determine vector defined by endpoints of first line
	dV1Mag = v1.Length();									// Determine length of first vector
	if (dV1Mag <= DBL_EPSILON)											// Endpoints of first line coincide
		return false;
	
	CVec v2(arLn2Beg, arLn2End);					// Determine vector defined by endpoints of second line
	dV2Mag = v2.Length();									// Determine length of second vector
	if (dV2Mag <= DBL_EPSILON)											// Endpoints of second line coincide
		return false;
	
	vPlnNorm = v1 ^ v2; 						// Determine vector normal to tangent vectors
	vPlnNorm.Normalize();
	if (vPlnNorm.IsNull())										// Two lines are parallel
		return false;
	
	if (fabs((vPlnNorm * CVec(arLn1Beg, arLn2Beg))) > DBL_EPSILON) // Four points are not coplanar
		return false;
	
	CTMat tm(arLn1Beg, vPlnNorm);
	
	arLn1End = tm * arLn1End;							// Transform points to z0 plane
	arLn2Beg = tm * arLn2Beg;
	arLn2End = tm * arLn2End;
	dA1 = - arLn1End[1] / dV1Mag;									// Determine normalized coefficients of line perpendicular to first line
	dB1 = arLn1End[0] / dV1Mag;
	v2[0] = arLn2End[0] - arLn2Beg[0]; 						// Determine vector defined by transformed endpoints of second line
	v2[1] = arLn2End[1] - arLn2Beg[1];
	dA2 = - v2[1] / dV2Mag; 									// Determine normalized coefficients of line perpendicular to second line
	dB2 = v2[0] / dV2Mag;
	dDet = dA2 * dB1 - dA1 * dB2;								// Determine the determinant
	
	dSgnRad = (arLn1End[0] * arLn2End[1] - arLn2End[0] * arLn1End[1]) >= 0. ? - fabs(adRad) : fabs(adRad);
	
	dC1RAB1 = dSgnRad;
	dC2RAB2 = (arLn2Beg[0] * arLn2End[1] - arLn2End[0] * arLn2Beg[1]) / dV2Mag + dSgnRad;
	(*ptCP)[0] = (dB2 * dC1RAB1 - dB1 * dC2RAB2) / dDet;
	(*ptCP)[1] = (dA1 * dC2RAB2 - dA2 * dC1RAB1) / dDet;
	(*ptCP)[2] = 0.;
	tm.Inverse();
	*ptCP = tm * (*ptCP);
	return true;
}