#include "stdafx.h"

CMat4& CMat4::operator+=(const CMat4& m) 
{
	m_v[0] += m[0];
	m_v[1] += m[1];
	m_v[2] += m[2];
	m_v[3] += m[3];
	
	return *this;
}

CMat4& CMat4::operator-=(const CMat4& m) 
{
	m_v[0] -= m[0]; 
	m_v[1] -= m[1]; 
	m_v[2] -= m[2]; 
	m_v[3] -= m[3];
	
	return *this;
}

CMat4& CMat4::operator*=(const double d)
{
	m_v[0] *= d; 
	m_v[1] *= d; 
	m_v[2] *= d; 
	m_v[3] *= d;
	
	return *this;
}

CMat4& CMat4::operator*=(const CMat4& mB) 
{
	CMat4 mA(*this);
	
	for (int iRow = 0; iRow < 4; iRow++)
	{
		CVec4 vRowA(mA[0][iRow], mA[1][iRow], mA[2][iRow], mA[3][iRow]);
		
		for (int iCol = 0; iCol < 4; iCol++)
			(*this)[iCol][iRow] = vRowA * mB[iCol];
	}
	return *this;
}

CMat4& CMat4::operator/=(const double d) 
{
	m_v[0] /= d; 
	m_v[1] /= d; 
	m_v[2] /= d; 
	m_v[3] /= d;
	
	return *this;
}

// Special methods

CMat4 CMat4::Transpose() 
{
	return CMat4(
		CVec4(m_v[0][0], m_v[1][0], m_v[2][0], m_v[3][0]),
		CVec4(m_v[0][1], m_v[1][1], m_v[2][1], m_v[3][1]),
		CVec4(m_v[0][2], m_v[1][2], m_v[2][2], m_v[3][2]),
		CVec4(m_v[0][3], m_v[1][3], m_v[2][3], m_v[3][3]));
}

CMat4& CMat4::Identity()
{
	m_v[0](1., 0., 0., 0.);
	m_v[1](0., 1., 0., 0.);
	m_v[2](0., 0., 1., 0.);
	m_v[3](0., 0., 0., 1.); 
	
	return (*this);
}

CMat4& CMat4::Inverse()		// Gauss-Jordan elimination with partial pivoting
{
	CMat4 mA(*this);				// As a evolves from original mat into identity
	this->Identity();					// mB evolves from identity into inverse(a)
	
	int i, i1;

	// Loop over cols of mA from left to right, eliminating above and below diag
	for (int iCol = 0; iCol < 4; iCol++) 
	{	// Find largest pivot in column iCol among rows iCol..3
		i1 = iCol; 		// Row with largest pivot candidate
		for (i = iCol + 1; i < 4; i++)
			if (fabs(mA[i][iCol]) > fabs(mA[i1][iCol]))
				i1 = i;

		// Swap rows i1 and iCol in mA and mB to put pivot on diagonal
		Exchange(mA[i1], mA[iCol]);
		Exchange((*this)[i1], (*this)[iCol]);

		// Scale row iCol to have mA unit diagonal
		if (mA[iCol][iCol] == 0.)
		{
			TRACE("CMat4::Inverse: singular matrix, can't invert\n");
		}
		(*this)[iCol] /= mA[iCol][iCol];
		mA[iCol] /= mA[iCol][iCol];

		// Eliminate off-diagonal elems in col iCol of mA, doing identical ops to mB
		for (i = 0; i < 4; i++)
			if (i != iCol) 
			{
				(*this)[i] -= mA[i][iCol] * (*this)[iCol];
				mA[i] -= mA[i][iCol] * mA[iCol];
			}
	}
	return *this;
}

// Friends 

/*
CMat4 operator-(const CMat4& mA)
{ 
	return CMat4(- mA.m_v[0], - mA.m_v[1], - mA.m_v[2], - mA.m_v[3]); 
}
*/
CMat4 operator+(const CMat4& mA, const CMat4& mB)
{ 
	return CMat4(mA[0] + mB[0], mA[1] + mB[1], mA[2] + mB[2], mA[3] + mB[3]);
}

CMat4 operator-(const CMat4& mA, const CMat4& mB)
{ 
	return CMat4(mA[0] - mB[0], mA[1] - mB[1], mA[2] - mB[2], mA[3] - mB[3]); 
}

CMat4 operator*(const CMat4& mA, const CMat4& mB) 
{
	CMat4 mC;
	
	for (int iRow = 0; iRow < 4; iRow++)
	{
		CVec4 vRowA(mA[0][iRow], mA[1][iRow], mA[2][iRow], mA[3][iRow]);
		
		for (int iCol = 0; iCol < 4; iCol++)
			mC[iCol][iRow] = vRowA * mB[iCol];
	}
	return mC;
}

CMat4 operator*(const CMat4& mA, const double d)
{ 
	return CMat4(mA[0] * d, mA[1] * d, mA[2] * d, mA[3] * d); 
}

CMat4 operator*(const double d, const CMat4& mA)
{ 
	return mA * d; 
}

CMat4 operator/(const CMat4& mA, const double d)
{ 
	return CMat4(mA[0] / d, mA[1] / d, mA[2] / d, mA[3] / d); 
}

bool operator==(const CMat4& mA, const CMat4& mB)
{ 
	return ((mA[0] == mB[0]) && (mA[1] == mB[1]) && (mA[2] == mB[2]) && (mA[3] == mB[3])); 
}

bool operator!=(const CMat4& mA, const CMat4& mB)
{ 
	return !(mA == mB);
}