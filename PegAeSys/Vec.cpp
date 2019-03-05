#include "stdafx.h"

// constructors

CVec::CVec(const CPnt& pt0, const CPnt& pt1) 
{
	m_d[0] = pt1[0] - pt0[0]; 
	m_d[1] = pt1[1] - pt0[1]; 
	m_d[2] = pt1[2] - pt0[2];
}

CVec::CVec(const CLine& ln)
{
	*this = ln[1] - ln[0];
}

CVec::CVec(const CVec4& v)
{
	m_d[0] = v[0];
	m_d[1] = v[1];
	m_d[2] = v[2];
}

// operators

CVec CVec::operator^(const CVec& v) const
{	// Determines the cross product of two vectors. 
	// Notes:	Use of cross product is the prefered method for determining
	//			plane normals. 	Resultant vector is not unitized.
	double dx = m_d[1] * v.m_d[2] - m_d[2] * v.m_d[1];
	double dy = m_d[2] * v.m_d[0] - m_d[0] * v.m_d[2];
	double dz = m_d[0] * v.m_d[1] - m_d[1] * v.m_d[0];
	return (CVec(dx, dy, dz));
}

// Methods

bool CVec::Identical(const CVec& v, double dTol) const
{
    return 
	(
		fabs(m_d[0] - v.m_d[0] ) > dTol ||
		fabs(m_d[1] - v.m_d[1] ) > dTol ||
		fabs(m_d[2] - v.m_d[2] ) > dTol ? false : true
	);
}

bool CVec::IsNull(double dTol) const
{
	return 
	(
		fabs(m_d[0]) > dTol || 
		fabs(m_d[1]) > dTol || 
		fabs(m_d[2]) > dTol ? false : true
	);
}

void CVec::Read(HFILE hFile)
{
	DWORD dwRead;
	ReadFile((HANDLE)hFile, m_d, 3 * sizeof(double), &dwRead, NULL);
}

CString CVec::ToString() const
{
	CString str;
	str.Format("%f;%f;%f\t", m_d[0], m_d[1], m_d[2]);	
	return (str);
}

void CVec::Write(HFILE hFile) const
{
	DWORD dwWrite;
	WriteFile((HANDLE)hFile, m_d, 3 * sizeof(double), &dwWrite, NULL);
}

bool CVec::Collinear(const CVec& v, double dTol) const
{
    if (fabs(m_d[0]) > FLT_EPSILON) 
	{
       double dT = v.m_d[0] / m_d[0]; 
       return (
		   fabs(dT * m_d[1] - v.m_d[1]) > dTol || 
		   fabs(dT * m_d[2] - v.m_d[2]) > dTol) ? false : true;
    } 
	else if (fabs(m_d[1]) > FLT_EPSILON) 
	{
       double dT = v.m_d[1] / m_d[1]; 
       return (
		   fabs(dT * m_d[0] - v.m_d[0]) > dTol || 
		   fabs(dT * m_d[2] - v.m_d[2]) > dTol) ? false : true;
    } 
	else if (fabs(m_d[2]) > FLT_EPSILON) 
	{
       double dT = v.m_d[2] / m_d[2]; 
       return (
		   fabs(dT * m_d[0] - v.m_d[0]) > dTol || 
		   fabs(dT * m_d[1] - v.m_d[1]) > dTol) ? false : true;
    } 
	else 
		return true; // Vector is zero vector .. true makes sense
}

void CVec::Normalize()
{
	double dLength = Length();
	
	if (dLength <= FLT_EPSILON)
		dLength = FLT_EPSILON;

	*this /= dLength; 
}

///<summary>Rotates a vector about an arbitrary axis in space.</summary>
void CVec::RotAboutArbAx(
	const CVec& vRefAx,		// unit vector defining rotation axis
	double dAng)			// rotation angle (ccw positive) in radians
{	
	CTMat tm(ORIGIN, vRefAx, dAng);

	*this = tm * (*this);
}

void CVec::RotAboutZAx(double dSinAng, double dCosAng)
{
	double dX = (m_d[0] * dCosAng - m_d[1] * dSinAng);
	double dY = (m_d[0] * dSinAng + m_d[1] * dCosAng);
	
	m_d[0] = dX;
	m_d[1] = dY;
}

// Friends

///<summary>Determines the angle of defined by a vector.</summary>
// Notes:  If null vector, angle is 0.
// Returns: Angle in radians (0 to 2 pi)
double Vec_Angle_xy(const CVec& v)
{
	double dRetVal;
	
	dRetVal = 0.;

	if (fabs(v.m_d[0]) > DBL_EPSILON || fabs(v.m_d[1]) > DBL_EPSILON) 
	{
		dRetVal = atan2(v.m_d[1], v.m_d[0]);
		if (dRetVal < 0.)
			dRetVal += TWOPI;
	}
	return (dRetVal);
}

double Vec_Length_xy(const CVec& v)
{
	return (sqrt(v.m_d[0] * v.m_d[0] + v.m_d[1] * v.m_d[1]));
}