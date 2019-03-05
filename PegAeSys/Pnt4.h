#pragma once

typedef CArray<CPnt4, CPnt4&> CPnt4s;

class CPnt4 : public CObject
{
private:
	
	double m_d[4];

public: // Constructors and destructor

	CPnt4() {m_d[0] = m_d[1] = m_d[2] = m_d[3] = 0.;}
	CPnt4(const double dX, const double dY, const double dZ, const double dW);
	CPnt4(const CPnt4& pt) {m_d[0] = pt.m_d[0]; m_d[1] = pt.m_d[1]; m_d[2] = pt.m_d[2]; m_d[3] = pt.m_d[3];}
	
	CPnt4(const CPnt& pt, const double d) {m_d[0] = pt[0]; m_d[1] = pt[1]; m_d[2] = pt[2]; m_d[3] = d;}
		
	~CPnt4() {}

public: // Operators

	CPnt4& operator=(const CPnt& pt);	
	CPnt4& operator=(const CPnt4& pt);
	
	void operator+=(const CVec4& v);
	void operator-=(const CVec4& v);
	void operator*=(const double t);
	void operator/=(const double t);

	double& operator[](int i) {return m_d[i];}
	const double& operator[](int i) const {return m_d[i];}	

	void operator()(const double dX, const double dY, const double dZ, const double dW);
	
	CPnt4 operator*(const double t) const;
	CPnt4 operator/(const double t) const;
	
public: // Methods

public: // Friends

	friend CPnt4 operator+(const CPnt4& pt, const CVec4& v);
	friend CVec4 operator-(const CPnt4& ptA, const CPnt4& ptB);
	friend CPnt4 operator-(const CPnt4& pt, const CVec4& v);
	friend CPnt4 operator*(const CTMat& tm, const CPnt4& pt);
	
	friend bool operator==(const CPnt4& v4A, const CPnt4& v4B);
	friend bool operator!=(const CPnt4& v4A, const CPnt4& v4B);

	friend CPnt4 Max(const CPnt4& ptA, const CPnt4& ptB);
	friend CPnt4 Min(const CPnt4& ptA, const CPnt4& ptB);

	friend double Pnt4_DistanceTo_xy(const CPnt4&, const CPnt4&);
	friend bool	Pnt4_ClipLine(CPnt4& ptA, CPnt4& ptB);
	friend void Pnt4_ClipPolygon(CPnt4s& pta);
	friend bool Pnt4_IsInView(const CPnt4& pt);
};
