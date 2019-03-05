#pragma once

class CPrimArc : public CPrim 
{
private:
	CPnt	m_ptCenter;
	CVec	m_vMajAx;
	CVec	m_vMinAx;
	double	m_dSwpAng;

public: // Constructors and destructor
	CPrimArc() {}
	CPrimArc(char*, int iVer);	
	CPrimArc(OdDbEntity*, int);
	CPrimArc(const CPnt&, const CVec&, const CVec&, double);
	CPrimArc(PENCOLOR nPenColor, PENSTYLE nPenStyle, const CPnt&, const CVec&, const CVec&, double);
	CPrimArc(const CPnt&, const CPnt&);
	CPrimArc(const CPnt&, const CPnt&, const CPnt&);
	CPrimArc(const CPrimArc&);
	~CPrimArc() {}

public: // Operators
	const CPrimArc& operator=(const CPrimArc&);

public: // Methods - absolute virtuals
	void		Assign(CPrim* pPrim) {*this = *static_cast<CPrimArc*>(pPrim);}
	CPrim*&		Copy(CPrim*&) const;
	void		Display(CDC* pDC, bool bNodeDisplay);
	void		DisplayOpenGL(bool bNodeDisplay);
	void		DisRep(const CPnt&) const;
	void		GetAllPts(CPnts& pts) {pts.SetSize(0); pts.Add(m_ptCenter);}
	CPnt		GetCtrlPt() const {return (m_ptCenter);}
	void		FormatExtra(CString& str) const;
	void		FormatGeometry(CString& str) const;
	void		GetExtents(CPnt&, CPnt&, const CTMat&) const;
	CPnt		GoToNxtCtrlPt() const;
	void		Iron();
	bool		Is(WORD wType) const {return wType == PRIM_ARC;}
	bool		IsInView() const;
	bool		IsPtACtrlPt(const CPnt4&) const; 
	void		ReadPeg(HFILE hFile);
	CPnt		SelAtCtrlPt(const CPnt4&) const;
	bool		SelUsingLine(const CLine& ln, CPnts& ptsInt);
	bool		SelUsingPoint(const CPnt4&, double, CPnt&);
	bool		SelUsingRect(const CPnt&, const CPnt&);
	void		Transform(const CTMat&); 
	void		Translate(const CVec& v) {m_ptCenter += v;}
	void		TranslateUsingMask(const CVec&, const DWORD);
	bool		Vectorize(const CSeg *pSeg);
	bool		WritePeg(HFILE hFile) const;
	void		Write(CFile&, char*) const;
	void		WriteJob(HFILE hFile, char*) const;
	bool		Write(OdDbEntityPtr&, OdDbDatabasePtr);

public: // Methods
	void		CutAtPt(const CPnt&, CSeg*);
	void		CutAt2Pts(CPnt*, CSegs*, CSegs*);
	void		GenPts(const CPnt& ptCent, const CVec& vMajAx, const CVec& vMinAx, double dSwpAng) const;
	CPnt		GetBegPt() const;
	void		GetBoundingBox(CPnts&) const;
	CPnt		GetEndPt() const;
	CVec		GetMajAx() const {return (m_vMajAx);}
	CVec		GetMinAx() const {return (m_vMinAx);}
	const CPnt&	Center() const {return (m_ptCenter);}
	double		GetSwpAng() const {return (m_dSwpAng);}
	void		GetXYExtents(CPnt, CPnt, CPnt*, CPnt*);
	int 		IsWithinArea(const CPnt&, const CPnt&, CPnt*);
	void		SetCP(const CPnt pt) {m_ptCenter = pt;}
	void		SetMajAx(const CVec& v) {m_vMajAx = v;}
	void		SetMinAx(const CVec& v) {m_vMinAx = v;}
	void		SetSwpAng(double dAng) {m_dSwpAng = dAng;}
	double		SwpAngToPt(const CPnt&);
};

CPnt pFndPtOnArc(const CPnt&, const CVec&, const CVec&, const double);