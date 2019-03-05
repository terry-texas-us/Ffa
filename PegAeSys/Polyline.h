#pragma once

class CPolyline
{
private:

	WORD	m_wFlags;		// bit 1 set - polygon is closed
								// bit 8 set - segment oriented linetype mapping
	CPnts	m_pts;
	
public: // Constructors and destructor
	CPolyline() {m_wFlags = 0;}
	CPolyline(CPnts& pts);

	CPolyline(const CPolyline& src);

	~CPolyline() {}
	
public: // Operators
	CPolyline& operator=(const CPolyline& src);
	
	void operator+=(const CVec& v);
	void operator-=(const CVec& v);
	
	CPnt& operator[](int i) {return m_pts[i];}
	const CPnt& operator[](int i) const {return m_pts[i];}	
};

namespace polyline
{
	void BeginLineStrip();
	void Display(CDC* pDC, const CPnts& pts);
	void Display(CDC* pDC, const CPnts& pts, short nPenStyle);
	void DisplayLoop(CDC* pDC, const CPnts& pts);
	void End(CDC* pDC, short nPenStyle);
	bool SelUsingLine(const CLine& ln, CPnts& ptsInt);
	bool SelUsingPoint(const CPnt4& ptPic, double dTol, double& dRel, CPnt& ptProj);
	bool SelUsingRect(const CPnt& pt1, const CPnt& pt2);
	bool SelUsingRect(const CPnt& pt1, const CPnt& pt2, const CPnts& pts);
	void SetVertex(const CPnt& pt);
}
