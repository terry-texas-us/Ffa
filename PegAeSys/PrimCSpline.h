#pragma once

enum ECSplineEndCnds
{
	CSP_RELAXED, 
	CSP_CLAMPED, 
	CSP_CYCLIC, 
	CSP_ANTICYCLIC = 4
};

class CPrimCSpline : public CPrim
{
private:
	WORD	m_wPtsS;
	WORD	m_wEndCndId;
	CVec	m_TanVec[2];
	CPnts	m_pts;

public:	// Constructors and destructor
	CPrimCSpline() {}
	CPrimCSpline(char*);
	CPrimCSpline(WORD nPtsS, WORD, CVec*, CPnt* pts);
	CPrimCSpline(const CPrimCSpline&);
	~CPrimCSpline() {}

public: // Operators
	const CPrimCSpline& operator=(const CPrimCSpline&);

public: // Methods - absolute virtuals
	void	Assign(CPrim* pPrim) {*this = *static_cast<CPrimCSpline*>(pPrim);}
	CPrim*& Copy(CPrim*&) const;
	void	Display(CDC* pDC, bool bNodeDisplay);
	void	DisplayOpenGL(bool bNodeDisplay);
	void	DisRep(const CPnt&) const {msgInformation("C-Spline - ");}
	void	GetAllPts(CPnts& pts) {pts.SetSize(0); pts.Copy(m_pts);}
	void	FormatExtra(CString& str) const;
	void    FormatGeometry(CString& str) const;
	CPnt	GetCtrlPt() const;
	void	GetExtents(CPnt&, CPnt&, const CTMat&) const;
	CPnt	GoToNxtCtrlPt() const;
	void	Iron();
	bool	Is(WORD wType) const {return wType == PRIM_CSPLINE;}
	bool	IsInView() const;
	bool	IsPtACtrlPt(const CPnt4&) const {return false;}
	void	ReadPeg(HFILE hFile);
	CPnt	SelAtCtrlPt(const CPnt4&) const;
	bool	SelUsingLine(const CLine&, CPnts&) {return false;}
	bool	SelUsingPoint(const CPnt4&, double, CPnt&) {return false;}
	bool	SelUsingRect(const CPnt&, const CPnt&);
	void	SetPt(WORD w, const CPnt& pt) {m_pts[w] = pt;}
	void	Transform(const CTMat&);
	void	Translate(const CVec&);
	void	TranslateUsingMask(const CVec&, const DWORD);
	bool	Vectorize(const CSeg *pSeg);
	bool	WritePeg(HFILE hFile) const;
	void	Write(CFile&, char*) const;
	void	WriteJob(HFILE hFile, char*) const;
	bool	Write(OdDbEntityPtr&, OdDbDatabasePtr);

	CPnt	EvaluateCardinal2D(CPnts& ptsCoords, double dTension, int nStep);
};