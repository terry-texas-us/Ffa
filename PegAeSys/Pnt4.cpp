#include "stdafx.h"

#include "PegaesysView.h"

CPnt4::CPnt4(const double dX, const double dY, const double dZ, const double dW)
{
	m_d[0] = dX; 
	m_d[1] = dY; 
	m_d[2] = dZ; 
	m_d[3] = dW;
}
CPnt4& CPnt4::operator=(const CPnt4& pt) 
{
	m_d[0] = pt.m_d[0];
	m_d[1] = pt.m_d[1];
	m_d[2] = pt.m_d[2];
	m_d[3] = pt.m_d[3]; 
	
	return *this;
}
CPnt4& CPnt4::operator=(const CPnt& pt) 
{
	m_d[0] = pt[0];
	m_d[1] = pt[1];
	m_d[2] = pt[2];
	m_d[3] = 1.;

	return *this;
}
void CPnt4::operator+=(const CVec4& v) 
{
	m_d[0] += v[0]; 
	m_d[1] += v[1]; 
	m_d[2] += v[2]; 
	m_d[3] += v[3]; 
}
void CPnt4::operator-=(const CVec4& v) 
{
	m_d[0] -= v[0]; 
	m_d[1] -= v[1]; 
	m_d[2] -= v[2]; 
	m_d[3] -= v[3]; 
}
void CPnt4::operator*=(const double d) 
{
	m_d[0] *= d; 
	m_d[1] *= d; 
	m_d[2] *= d; 
	m_d[3] *= d;
}
void CPnt4::operator/=(const double d) 
{
	m_d[0] /= d; 
	m_d[1] /= d; 
	m_d[2] /= d; 
	m_d[3] /= d;
}
void CPnt4::operator()(const double dX, const double dY, const double dZ, const double dW) 
{
	m_d[0] = dX;
	m_d[1] = dY;
	m_d[2] = dZ;
	m_d[3] = dW;
}
CPnt4 CPnt4::operator*(const double t) const
{ 
	return CPnt4(m_d[0] * t, m_d[1] * t, m_d[2] * t, m_d[3] * t); 
}
CPnt4 CPnt4::operator/(const double t) const
{ 
	return CPnt4(m_d[0] / t, m_d[1] / t, m_d[2] / t, m_d[3] / t); 
}
CPnt4 operator+(const CPnt4& pt, const CVec4& v)
{ 
	return CPnt4(pt.m_d[0] + v.m_d[0], pt.m_d[1] + v.m_d[1], pt.m_d[2] + v.m_d[2], pt.m_d[3] + v.m_d[3]); 
}
CVec4 operator-(const CPnt4& ptA, const CPnt4& ptB)
{  
	return CVec4(ptA.m_d[0] - ptB.m_d[0], ptA.m_d[1] - ptB.m_d[1], ptA.m_d[2] - ptB.m_d[2], ptA.m_d[3] - ptB.m_d[3]);
}
CPnt4 operator-(const CPnt4& pt, const CVec4& v)
{ 
	return CPnt4(pt.m_d[0] - v.m_d[0], pt.m_d[1] - v.m_d[1], pt.m_d[2] - v.m_d[2], pt.m_d[3] - v.m_d[3]); 
}
CPnt4 operator*(const double d, const CPnt4& pt)
{ 
	return pt * d; 
}
bool operator==(const CPnt4& ptA, const CPnt4& ptB)
{ 
	return (ptA.m_d[0] == ptB.m_d[0]) && (ptA.m_d[1] == ptB.m_d[1]) && (ptA.m_d[2] == ptB.m_d[2]) && (ptA.m_d[3] == ptB.m_d[3]); 
}
bool operator!=(const CPnt4& ptA, const CPnt4& ptB)
{ 
	return !(ptA == ptB); 
}
///<summary>Determines the xy distance between two points.</summary>
double Pnt4_DistanceTo_xy(const CPnt4& pt1, const CPnt4& pt2)
{	
	double dX = pt2.m_d[0] / pt2.m_d[3] - pt1.m_d[0] / pt1.m_d[3];
	double dY = pt2.m_d[1] / pt2.m_d[3] - pt1.m_d[1] / pt1.m_d[3];

	return sqrt(dX * dX + dY * dY);
}
CPnt4 Max(const CPnt4& ptA, const CPnt4& ptB)
{ 
	return CPnt4(Max(ptA.m_d[0], ptB.m_d[0]), Max(ptA.m_d[1], ptB.m_d[1]), Max(ptA.m_d[2], ptB.m_d[2]), Max(ptA.m_d[3], ptB.m_d[3])); 
}
CPnt4 Min(const CPnt4& ptA, const CPnt4& ptB)
{ 
	return CPnt4(Min(ptA.m_d[0], ptB.m_d[0]), Min(ptA.m_d[1], ptB.m_d[1]), Min(ptA.m_d[2], ptB.m_d[2]), Min(ptA.m_d[3], ptB.m_d[3])); 
}
bool Pnt4_ClipLine(CPnt4& ptA, CPnt4& ptB)
{	
//	if(CPegView::GetActiveView()->IsClipped())
//		return CPegView::GetActiveView()->Pnt4_ClipLine(ptA, ptB);

	double dBoundaryCodeA[] = 
	{	
		ptA.m_d[3] + ptA.m_d[0], ptA.m_d[3] - ptA.m_d[0],
		ptA.m_d[3] + ptA.m_d[1], ptA.m_d[3] - ptA.m_d[1],
		ptA.m_d[3] + ptA.m_d[2], ptA.m_d[3] - ptA.m_d[2]
	};
	double dBoundaryCodeB[] = 
	{
		ptB.m_d[3] + ptB.m_d[0], ptB.m_d[3] - ptB.m_d[0],
		ptB.m_d[3] + ptB.m_d[1], ptB.m_d[3] - ptB.m_d[1],
		ptB.m_d[3] + ptB.m_d[2], ptB.m_d[3] - ptB.m_d[2]
	};
	
	int iOutCodeA = 0;
	int iOutCodeB = 0;
	
	for (int iBC = 0; iBC < 6; iBC++)
	{
		if (dBoundaryCodeA[iBC] <= 0.)
			iOutCodeA |= (1 << iBC);
		if (dBoundaryCodeB[iBC] <= 0.)
			iOutCodeB |= (1 << iBC);
	}
	
	if ((iOutCodeA & iOutCodeB) != 0)
		return false;
	if ((iOutCodeA | iOutCodeB) == 0)
		return true;

	double dTIn = 0.;
	double dTOut = 1.;
	
	double dTHit;

	for (int i = 0; i < 6; i++)
	{
		if (dBoundaryCodeB[i] < 0.)
		{
			dTHit = dBoundaryCodeA[i] / (dBoundaryCodeA[i] - dBoundaryCodeB[i]);
			dTOut = Min(dTOut, dTHit);
		}
		else if (dBoundaryCodeA[i] < 0.)
		{
			dTHit = dBoundaryCodeA[i] / (dBoundaryCodeA[i] - dBoundaryCodeB[i]);
			dTIn = Max(dTIn, dTHit);
		}
		if (dTIn > dTOut)
			return false;
	}
	CPnt4 pt(ptA);

	if (iOutCodeA != 0)
	{
		ptA = pt + dTIn * (ptB - pt);
	}
	if (iOutCodeB != 0)
	{
		ptB = pt + dTOut * (ptB - pt);
	}
	return true;
}

bool Pnt4_IsInView(const CPnt4& pt)
{	
//	if(CPegView::GetActiveView()->IsClipped())
//		return CPegView::GetActiveView()->Pnt4_IsInView(pt);

		///////////////////////////////////////////////////////
		// Performs a containment test on a point.
	if (pt.m_d[3] + pt.m_d[0] <= 0. || pt.m_d[3] - pt.m_d[0] <= 0.) return false;
	if (pt.m_d[3] + pt.m_d[1] <= 0. || pt.m_d[3] - pt.m_d[1] <= 0.) return false;
	if (pt.m_d[3] + pt.m_d[2] <= 0. || pt.m_d[3] - pt.m_d[2] <= 0.) return false;

	return true;
}