#pragma once

namespace line
{
	double	AngleBetweenLn_xy(const CLine&, const CLine&);
	CPnt4	IntersectionWithPln(const CPnt4& ptBeg, const CPnt4& ptEnd, const CPnt4& ptQ, const CVec4& vQNorm);
	bool	IntersectionWithPln(const CPnt&, const CVec&, const CPnt&, const CVec&, CPnt*);
	CPnt	ConstrainToAx(const CLine&, double dInfAng, double dAxOffAng);
	bool	EvalPtProx_xy(const CLine&, const CPnt&, const double, CPnt& ptProj, double*, double*);
	void	Extents_xy(const CLine&, CPnt&, CPnt&);
	double	GetAngAboutZAx(const CLine&);
	int 	Intersection(const CLine&, const CLine&, CPnt&);
	bool	Intersection_xy(const CLine&, const CLine&, CPnt&);
	int 	ProjPtFrom_xy(const CLine&, double, double, CPnt*);
	bool	RelOfPtToEndPts(const CLine&, const CPnt&, double&);
}

	bool	p2DEvalWndLn(const CPnt&, const CPnt&, const CPnt&, const CPnt&);
	int 	pFndSwpAngGivPlnAnd3Lns(const CVec&, const CPnt, const CPnt, const CPnt, const CPnt&, double*);

class CLine : public CObject
{
private:
	CPnt m_pt[2];

public:	// Constructors and destructor
	
	CLine() {}
	CLine(const CPnt& pt0, const CPnt& pt1) {m_pt[0] = pt0; m_pt[1] = pt1;}
	CLine(const CLine& ln) {m_pt[0] = ln.m_pt[0]; m_pt[1] = ln.m_pt[1];}
	
	~CLine() {};

public:	// Operators
	
	CLine&	operator=(const CLine& ln) {m_pt[0] = ln.m_pt[0]; m_pt[1] = ln.m_pt[1]; return (*this);}
	
	void	operator+=(const CVec& v) {m_pt[0] += v; m_pt[1] += v;}
	void	operator-=(const CVec& v) {m_pt[0] -= v; m_pt[1] -= v;}
	
	CPnt& operator[](int i) {return m_pt[i];}
	const CPnt& operator[](int i) const {return m_pt[i];}	

	void operator()(const CPnt& pt0, const CPnt& pt1) {m_pt[0] = pt0; m_pt[1] = pt1;}

	CLine	operator+(const CVec& v) const;
	CLine	operator-(const CVec& v) const; 

	bool	operator||(const CLine&) const; 	// determines if lines are parallel

public: // Methods

	WORD	CutAtPt(const CPnt&, CLine&);
	int 	DirRelOfPt(const CPnt&) const;
	void	Display(CDC* pDC) const;	
	double	GetAngAboutZAx() const {return line::GetAngAboutZAx(*this);}
	double	Length() const;
	bool	GetParallels(double, double, CLine&, CLine&) const;
	CVec	GetVector() const;
	bool	IsNull() const	{return (m_pt[0] == m_pt[1]) ? true : false;}
	CPnt	ProjPt(const CPnt& pt) const;
	CPnt	ProjPtAlong(double) const;
	CPnt	ProjToBegPt(double) const;
	CPnt	ProjToEndPt(double) const;
//	void	Read(CFile&);
	void	Read(HFILE hFile);
//	void	Write(CFile&) const;
	void	Write(HFILE hFile) const;

public: // Friends
	friend CLine operator*(const CTMat& tm, const CLine& ln);

	friend bool line::EvalPtProx_xy(const CLine&, const CPnt&, const double, CPnt& ptProj, double*, double*);
};
