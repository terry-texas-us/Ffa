#include "stdafx.h"

CVec4::CVec4(const CPnt4& pt0, const CPnt4& pt1) 
{
	m_d[0] = pt1[0] - pt0[0]; 
	m_d[1] = pt1[1] - pt0[1]; 
	m_d[2] = pt1[2] - pt0[2];
	m_d[3] = pt1[3] - pt0[3];
}

CVec4& CVec4::operator=(const CVec4& v) 
{
	m_d[0] = v.m_d[0]; 
	m_d[1] = v.m_d[1]; 
	m_d[2] = v.m_d[2]; 
	m_d[3] = v.m_d[3]; 
	return *this;
}

CVec4& CVec4::operator+=(const CVec4& v) 
{
	m_d[0] += v.m_d[0]; 
	m_d[1] += v.m_d[1]; 
	m_d[2] += v.m_d[2]; 
	m_d[3] += v.m_d[3]; 
	return *this;
}

CVec4& CVec4::operator-=(const CVec4& v) 
{
	m_d[0] -= v.m_d[0]; 
	m_d[1] -= v.m_d[1]; 
	m_d[2] -= v.m_d[2]; 
	m_d[3] -= v.m_d[3]; 
	return *this;
}

CVec4& CVec4::operator*=(const double d) 
{
	m_d[0] *= d; 
	m_d[1] *= d; 
	m_d[2] *= d; 
	m_d[3] *= d;
	return *this;
}

CVec4& CVec4::operator/=(const double d) 
{
	m_d[0] /= d; 
	m_d[1] /= d; 
	m_d[2] /= d; 
	m_d[3] /= d;
	return *this;
}

void CVec4::operator()(const double dX, const double dY, const double dZ, const double dW) 
{
	m_d[0] = dX;
	m_d[1] = dY;
	m_d[2] = dZ;
	m_d[3] = dW;
}

CVec4 operator-(const CVec4& vA)
{
	return CVec4(- vA.m_d[0], - vA.m_d[1], - vA.m_d[2], - vA.m_d[3]);
}

CVec4 operator+(const CVec4& vA, const CVec4& vB)
{ 
	return CVec4(vA.m_d[0] + vB.m_d[0], vA.m_d[1] + vB.m_d[1], vA.m_d[2] + vB.m_d[2], vA.m_d[3] + vB.m_d[3]); 
}

CVec4 operator-(const CVec4& vA, const CVec4& vB)
{  
	CVec4 v = vA;
	return v -= vB;
}

CVec4 operator*(const CVec4& vA, const double d)
{ 
	return CVec4(d * vA.m_d[0], d * vA.m_d[1], d * vA.m_d[2], d * vA.m_d[3] ); 
}

CVec4 operator*(const double d, const CVec4& vA)
{ 
	return vA * d; 
}

double operator*(const CVec4& vA, const CVec4& vB)
{ 
	return (vA.m_d[0] * vB.m_d[0] + vA.m_d[1] * vB.m_d[1] + vA.m_d[2] * vB.m_d[2] + vA.m_d[3] * vB.m_d[3]); 
}

CVec4 operator/(const CVec4& vA, const double d)
{ 
	double d_inv = 1. / d; 
	return CVec4(vA.m_d[0] * d_inv, vA.m_d[1] * d_inv, vA.m_d[2] * d_inv, vA.m_d[3] * d_inv); 
}

bool operator==(const CVec4& vA, const CVec4& vB)
{ 
	return (vA.m_d[0] == vB.m_d[0]) && (vA.m_d[1] == vB.m_d[1]) && (vA.m_d[2] == vB.m_d[2]) && (vA.m_d[3] == vB.m_d[3]); 
}

bool operator!=(const CVec4& vA, const CVec4& vB)
{ 
	return !(vA == vB); 
}

void Exchange(CVec4& vA, CVec4& vB)
{ 
	CVec4 vTmp(vA); 
	vA = vB; 
	vB = vTmp; 
}

CVec4 Max(const CVec4& vA, const CVec4& vB)
{ 
	return CVec4(Max(vA.m_d[0], vB.m_d[0]), Max(vA.m_d[1], vB.m_d[1]), Max(vA.m_d[2], vB.m_d[2]), Max(vA.m_d[3], vB.m_d[3])); 
}

CVec4 Min(const CVec4& vA, const CVec4& vB)
{ 
	return CVec4(Min(vA.m_d[0], vB.m_d[0]), Min(vA.m_d[1], vB.m_d[1]), Min(vA.m_d[2], vB.m_d[2]), Min(vA.m_d[3], vB.m_d[3])); 
}