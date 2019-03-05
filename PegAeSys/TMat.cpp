#include "stdafx.h"

///<summary>
///Constructs transformation matrix required to transform points about a reference point and axis.
///</summary>
// Notes:	Assumes reference vector is a unit vector.	Uses right handed convention.
//			Based on the following equation: [ti] * [r] * [t], where
//	ti translation of reference point to origin
//	r  rotation about origin =
//	| ax*ax+(1-ax*ax)*ca  ax*ay*(1-ca)+az*sa  ax*az*(1-ca)-ay*sa |
//	| ax*ay*(1-ca)-az*sa  ay*ay+(1-ay*ay)*ca  ay*az*(1-ca)+ax*sa |
//	| ax*az*(1-ca)+ay*sa  ay*az*(1-ca)-ax*sa  az*az+(1-az*az)*ca |
//	t	translation of reference point back to intitial position
CTMat::CTMat(
	const CPnt& pt, 	// reference point
	const CVec& v,		// reference axis vector (unit)
	const double dAng)	// angle (radians)
{	
	double dSinAng = sin(dAng);
	double dCosAng = cos(dAng);

	double dXSqrd = v[0] * v[0];
	double dYSqrd = v[1] * v[1];
	double dZSqrd = v[2] * v[2];

	m_v[0][0] = (dXSqrd + (1. - dXSqrd) * dCosAng);
	m_v[0][1] = (v[0] * v[1] * (1. - dCosAng) - v[2] * dSinAng);
	m_v[0][2] = (v[0] * v[2] * (1. - dCosAng) + v[1] * dSinAng);
	m_v[0][3] = - m_v[0][0] * pt[0] - m_v[0][1] * pt[1] - m_v[0][2] * pt[2] + pt[0];

	m_v[1][0] = (v[0] * v[1] * (1. - dCosAng) + v[2] * dSinAng);
	m_v[1][1] = (dYSqrd + (1. - dYSqrd) * dCosAng);
	m_v[1][2] = (v[1] * v[2] * (1. - dCosAng) - v[0] * dSinAng);
	m_v[1][3] = - m_v[1][0] * pt[0] - m_v[1][1] * pt[1] - m_v[1][2] * pt[2] + pt[1];

	m_v[2][0] = (v[0] * v[2] * (1. - dCosAng) - v[1] * dSinAng);
	m_v[2][1] = (v[1] * v[2] * (1. - dCosAng) + v[0] * dSinAng);
	m_v[2][2] = (dZSqrd + (1. - dZSqrd) * dCosAng);
	m_v[2][3] = - m_v[2][0] * pt[0] - m_v[2][1] * pt[1] - m_v[2][2] * pt[2] + pt[2];

	m_v[3](0., 0., 0., 1.);
}

///<summary>Build matrix which will transform points onto z=0 plane with origin at 0,0</summary>
// Notes:	X and y axis vectors do not need to be normalized. If they are not unit vectors, 
//			appropriate scaling is applied in the x and/or y axis.
CTMat::CTMat(
	const CPnt& ptO,	// origin of reference system
	const CVec& vXAx,	// x-axis of reference system
	const CVec& vYAx)	// y-axis of reference system
{	
	CVec vN(vXAx ^ vYAx);
	vN.Normalize();
	
	DefUsingArbPtAndAx(ptO, vN);
	
	// Transform x-axis reference vector onto z=0 plane
	CVec vXAxT = vXAx;
	vXAxT = *this * vXAxT;
	
	double dX = vXAxT[0];
	double dY = vXAxT[1];
	
	CVec vScale(1. / sqrt(dX * dX + dY * dY), 1., 1.);
	
	vXAxT.Normalize();
	
	// To get x-axis reference vector as x-axis
	CTMat tm;
	*this *= tm.RotateZ(- vXAxT[1], vXAxT[0]);
		
	// Transform y-axis reference vector onto z=0 plane
	CVec vYAxT = vYAx;
	vYAxT = *this * vYAxT;
	
	if (fabs(vYAxT[1]) <= DBL_EPSILON) 
		return;
	
	vScale[1] = 1. / vYAxT[1];
	vScale[2] = 1.;
	
	// Add shear to matrix which gets positive y-axis reference vector as y-axis
	if (fabs(vYAxT[0]) > DBL_EPSILON)
	{
		double dShrFac = - vYAxT[0] / vYAxT[1];
		for (int i = 0; i < 4; i++)
			m_v[0][i] += m_v[1][i] * dShrFac;
	}
	Scale(vScale);
}

///<summary>Builds rotation transformation matrices.</summary>
// Note: Angles (in degrees) for each axis
CTMat::CTMat(int* iOrd, const CVec& vAng)
{	
	Identity();
	
	for (int i = 0; i < 3; i++) 
	{
		double dAng = Arc(vAng[iOrd[i]]);
		if (fabs(dAng) > FLT_EPSILON)
		{
			double dSinAng = sin(dAng);
			double dCosAng = cos(dAng);

			CTMat tm;
			if (iOrd[i] == 0)
			{
				*this *= tm.RotateX(dSinAng, dCosAng);
			}
			else if (iOrd[i] == 1)
			{
				*this *= tm.RotateY(dSinAng, dCosAng);
			}
			else
			{
				*this *= tm.RotateZ(dSinAng, dCosAng);
			}
		}
	}
}

///<summary>
///Constructs transformation matrix required to transform points on an arbitrary plane to the z=0 
///plane with point at origin.
///</summary>
// Notes:	Assumes plane normal is a unit vector. Uses right handed convention.
//			See Rodgers, 3-9 Rotation about an arbitrary axis in space
void CTMat::DefUsingArbPtAndAx(
	const CPnt& ptO,	// point on plane which defines origin
	const CVec& vN) 	// unit vector defining plane normal
{	
	Identity();
	Translate(ORIGIN - ptO);

	double dAbsNy = fabs(vN[1]);
	double dAbsNz = fabs(vN[2]);

	// avoid sqrt use if Ny or Nz are 0.
	
	double d = 0.;
	if (dAbsNz <= DBL_EPSILON) 
		d = dAbsNy;
	else if (dAbsNy <= DBL_EPSILON) 
		d = dAbsNz;
	else
		d = sqrt(dAbsNy * dAbsNy + dAbsNz * dAbsNz);
	
	// sin(Rx) = Ny / d; cos(Rx) = Nz / d

	CTMat tm;
	if (d > DBL_EPSILON)
	{
		tm.RotateX(vN[1] / d, vN[2] / d);
		*this *= tm;
	}
	// sin(Ry) = Nx; cos(Ry) = d;

	if (fabs(vN[0]) > DBL_EPSILON)
	{
		tm.RotateY(- vN[0], d);
		*this *= tm;
	}
}

CTMat CTMat::RotateX(const double dSinAng, const double dCosAng)
{	
	// Initializes a matrix for x-rotation, the y-axis rotates to the z-axis
	
	m_v[0](1., 0., 0., 0.);
	m_v[1](0., dCosAng, - dSinAng, 0.);
	m_v[2](0., dSinAng, dCosAng, 0.);
	m_v[3](0., 0., 0., 1.); 

	return (*this);
}	

CTMat CTMat::RotateY(const double dSinAng, const double dCosAng)
{	
	// Initializes a matrix for y-rotation, the z-axis rotates to the x-axis
	
	m_v[0](dCosAng, 0., dSinAng, 0.);
	m_v[1](0., 1., 0., 0.);
	m_v[2](- dSinAng, 0., dCosAng, 0.);
	m_v[3](0., 0., 0., 1.); 

	return (*this);
}	

CTMat CTMat::RotateZ(const double dSinAng, const double dCosAng)
{
	
	m_v[0](dCosAng, - dSinAng, 0., 0.);
	m_v[1](dSinAng, dCosAng, 0., 0.);
	m_v[2](0., 0., 1., 0.);
	m_v[3](0., 0., 0., 1.);
	
	return (*this);
}

void CTMat::Scale(const CVec& v)
{
	for (int i = 0; i < 4; i++)
	{
		m_v[0][i] *= v[0]; m_v[1][i] *= v[1]; m_v[2][i] *= v[2];
	}
}

void CTMat::Translate(const CPnt& pt)
{
	m_v[0][3] += pt[0]; 
	m_v[1][3] += pt[1]; 
	m_v[2][3] += pt[2];
}

void CTMat::Translate(const CVec& v)
{
	m_v[0][3] += v[0]; 
	m_v[1][3] += v[1]; 
	m_v[2][3] += v[2];
}

CLine operator*(const CTMat& tm, const CLine& ln)
{
	CLine lnOut;

	lnOut.m_pt[0] = tm * ln.m_pt[0];
	lnOut.m_pt[1] = tm * ln.m_pt[1];
	
	return lnOut;
}

CPnt operator*(const CTMat& tm, const CPnt& pt)
{
	CPnt ptOut;

	ptOut.m_d[0] = pt.m_d[0] * tm.m_v[0][0] + pt.m_d[1] * tm.m_v[0][1] + pt.m_d[2] * tm.m_v[0][2] + tm.m_v[0][3];
	ptOut.m_d[1] = pt.m_d[0] * tm.m_v[1][0] + pt.m_d[1] * tm.m_v[1][1] + pt.m_d[2] * tm.m_v[1][2] + tm.m_v[1][3];
	ptOut.m_d[2] = pt.m_d[0] * tm.m_v[2][0] + pt.m_d[1] * tm.m_v[2][1] + pt.m_d[2] * tm.m_v[2][2] + tm.m_v[2][3];

	return ptOut;
}

CPnt4 operator*(const CTMat& tm, const CPnt4& pt) 
{
	CPnt4 ptOut;

	for (int i = 0; i < 4; i++)
	{
		ptOut[i] = pt.m_d[0] * tm.m_v[i][0] + pt.m_d[1] * tm.m_v[i][1] + pt.m_d[2] * tm.m_v[i][2] + pt.m_d[3] * tm.m_v[i][3];
	}

	return ptOut;
}

CVec operator*(const CTMat& tm, const CVec& v)
{
	CVec vOut;

	vOut.m_d[0] = v.m_d[0] * tm.m_v[0][0] + v.m_d[1] * tm.m_v[0][1] + v.m_d[2] * tm.m_v[0][2];
	vOut.m_d[1] = v.m_d[0] * tm.m_v[1][0] + v.m_d[1] * tm.m_v[1][1] + v.m_d[2] * tm.m_v[1][2];
	vOut.m_d[2] = v.m_d[0] * tm.m_v[2][0] + v.m_d[1] * tm.m_v[2][1] + v.m_d[2] * tm.m_v[2][2];
	
	return vOut;
}

CVec4 operator*(const CTMat& tm, const CVec4& v) 
{
	CVec4 vOut;
	
	for (int i = 0; i < 4; i++)
	{
		vOut[i] = v.m_d[0] * tm.m_v[i][0] + v.m_d[1] * tm.m_v[i][1] + v.m_d[2] * tm.m_v[i][2] + v.m_d[3] * tm.m_v[i][3];
	}
	return vOut;	
}