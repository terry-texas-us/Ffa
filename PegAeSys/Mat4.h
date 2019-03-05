#pragma once

#include "Vec4.h"

class CMat4 : public CObject
{

protected:

	CVec4 m_v[4];

public: // Constructors and destructor

	CMat4() {}
	CMat4(const CMat4& m);
	CMat4(const CVec4& v0, const CVec4& v1, const CVec4& v2, const CVec4& v3);
	
	virtual ~CMat4() {}

public: // Operators

	CMat4& operator=(const CMat4& m);

	CMat4& operator+=(const CMat4& m);
	CMat4& operator-=(const CMat4& m);
	CMat4& operator*=(const CMat4& m);
	CMat4& operator*=(const double d);
	CMat4& operator/=(const double d);

	CVec4& operator[](int i) {return m_v[i];}
	const CVec4& operator[](int i) const {return m_v[i];}	

public: // Special methods

	CMat4&	Identity();
	CMat4&	Inverse();
	CMat4	Transpose();

public: // Friends

	friend CMat4 operator+(const CMat4& mA, const CMat4& mB);
	friend CMat4 operator-(const CMat4& mA, const CMat4& mB);
	friend CMat4 operator*(const CMat4& mA, const CMat4& mB);
	friend CMat4 operator*(const CMat4& mA, const double d);
	friend CMat4 operator*(const double d, const CMat4& mA);
	friend CMat4 operator/(const CMat4& mA, const double d);
	
	friend bool operator==(const CMat4& mA, const CMat4& mB);
	friend bool operator!=(const CMat4& mA, const CMat4& mB);
};

inline CMat4::CMat4(const CMat4& m)
{
	m_v[0] = m.m_v[0];
	m_v[1] = m.m_v[1];
	m_v[2] = m.m_v[2];
	m_v[3] = m.m_v[3];
}

inline CMat4::CMat4(const CVec4& v0, const CVec4& v1, const CVec4& v2, const CVec4& v3) 
{
	m_v[0] = v0; 
	m_v[1] = v1; 
	m_v[2] = v2; 
	m_v[3] = v3;
}

inline CMat4& CMat4::operator=(const CMat4& m) 
{
	m_v[0] = m[0]; 
	m_v[1] = m[1]; 
	m_v[2] = m[2]; 
	m_v[3] = m[3];

	return *this;
}
