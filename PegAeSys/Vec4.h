#pragma once

class CPnt4;
class CVec;

class CVec4 : public CObject
{
private:
	double m_d[4];

public: // Constructors and destructor
	CVec4() {m_d[0] = m_d[1] = m_d[2] = m_d[3] = 0.;}
	CVec4(const double dX, const double dY, const double dZ, const double dW);
	CVec4(const CPnt4& pt0, const CPnt4& pt1);
	CVec4(const CVec4& v);
	CVec4(const CVec& v, const double d);
	
	~CVec4() {}

public: // Operators

	CVec4& operator=(const CVec4& v);
	CVec4& operator+=(const CVec4& v);
	CVec4& operator-=(const CVec4& v);
	CVec4& operator*=(const double d);
	CVec4& operator/=(const double d);

	double& operator[](int i) {return m_d[i];}
	const double& operator[](int i) const {return m_d[i];}	

	void operator()(const double dX, const double dY, const double dZ, const double dW);

	CVec4 operator-() const {return CVec4(- m_d[0], - m_d[1], - m_d[2], - m_d[3]);}

public: // Methods

	void Get(double* d) const {d[0] = m_d[0]; d[1] = m_d[1]; d[2] = m_d[2]; d[3] = m_d[3];}
	
public: // Friends

	friend CPnt4 operator+(const CPnt4& pt, const CVec4& v);
	friend CPnt4 operator-(const CPnt4& pt, const CVec4& v);
	friend CVec4 operator*(const double d, const CVec4& v);
	friend CVec4 operator*(const CTMat& m, const CVec4& v);
	
	friend CVec4 operator*(const CVec4& v, CTMat& m4);
	friend CVec4 operator/(const CVec4& v, const double d);
	friend void Exchange(CVec4& vA, CVec4& vB);
	friend CVec4 Max(const CVec4& vA, const CVec4& vB);
	friend CVec4 Min(const CVec4& vA, const CVec4& vB);
	
	friend CVec4 operator-(const CVec4& v);
	friend CVec4 operator+(const CVec4& v4A, const CVec4& v4B);
	friend CVec4 operator-(const CVec4& v4A, const CVec4& v4B);
	friend CVec4 operator*(const CVec4& v4, const double d);
	friend double operator*(const CVec4& v4A, const CVec4& v4B);
	friend bool operator==(const CVec4& v4A, const CVec4& v4B);
	friend bool operator!=(const CVec4& v4A, const CVec4& v4B);
};

// Constructors

inline CVec4::CVec4(const double dX, const double dY, const double dZ, const double dW)
{
	m_d[0] = dX; 
	m_d[1] = dY; 
	m_d[2] = dZ; 
	m_d[3] = dW;
}

inline CVec4::CVec4(const CVec4& v) 
{
	m_d[0] = v.m_d[0]; 
	m_d[1] = v.m_d[1]; 
	m_d[2] = v.m_d[2]; 
	m_d[3] = v.m_d[3];
}

inline CVec4::CVec4(const CVec& v, const double d) 
{
	m_d[0] = v[0]; 
	m_d[1] = v[1]; 
	m_d[2] = v[2]; 
	m_d[3] = d;
}