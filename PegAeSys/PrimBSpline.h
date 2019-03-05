#pragma once

class CPrimBSpline : public CPrim
{
private:
	CPnts	m_pts;

public:	// Constructors and destructor
	CPrimBSpline() {}
	CPrimBSpline(char*, int iVer);
	CPrimBSpline(WORD, CPnt*);
	CPrimBSpline(const CPrimBSpline&);
	~CPrimBSpline() {}
	
public: // Operators
	const CPrimBSpline& operator=(const CPrimBSpline&);

public: // Methods - absolute virtuals
	void	Assign(CPrim* pPrim) {*this = *static_cast<CPrimBSpline*>(pPrim);}
	CPrim*& Copy(CPrim*&) const;
	void	Display(CDC* pDC, bool bNodeDisplay);
	void	DisplayOpenGL(bool bNodeDisplay);
	void	DisRep(const CPnt&) const;
	void	GetAllPts(CPnts& pts) {pts.SetSize(0); pts.Copy(m_pts);}
	void	FormatExtra(CString& str) const;
	void    FormatGeometry(CString& str) const;
	CPnt	GetCtrlPt() const;
	void	GetExtents(CPnt&, CPnt&, const CTMat&) const;
	CPnt	GoToNxtCtrlPt() const;
	void	Iron();
	bool	Is(WORD wType) const {return wType == PRIM_BSPLINE;}
	bool	IsInView() const;
	bool	IsPtACtrlPt(const CPnt4&) const {return false;}
	void	ReadPeg(HFILE hFile);
	CPnt	SelAtCtrlPt(const CPnt4&) const;
	bool	SelUsingLine(const CLine&, CPnts&);
	bool	SelUsingPoint(const CPnt4&, double, CPnt&);
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

	int 	GenPts(const int, const CPnts& pts) const;	
};