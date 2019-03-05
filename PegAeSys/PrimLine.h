#pragma once

class CPrimLine : public CPrim
{
private:
	CLine	m_ln;

public: // Constructors and destructor
	CPrimLine() {}
	CPrimLine(char*, int iVer);
	CPrimLine(OdDbEntity*);
	CPrimLine(const CPnt& pt0, const CPnt& pt1);
	CPrimLine(PENCOLOR nPenColor, PENSTYLE nPenStyle, const CLine& ln);
	CPrimLine(PENCOLOR nPenColor, PENSTYLE nPenStyle, const CPnt& pt0, const CPnt& pt1);	
	CPrimLine(const CPrimLine& src);
	~CPrimLine() {};
	
public: // Operators
	const CPrimLine& operator=(const CPrimLine& src);
	
public: //	Methods - absolute virtuals
	void	Assign(CPrim* pPrim) {*this = *static_cast<CPrimLine*>(pPrim);}
	CPrim*& Copy(CPrim*&) const;
	void	Display(CDC* pDC, bool bNodeDisplay);
	void	DisplayOpenGL(bool bNodeDisplay);
	void	DisRep(const CPnt&) const;
	void	FormatExtra(CString& str) const;
	void    FormatGeometry(CString& str) const;
	void	GetAllPts(CPnts& pts);
	void	Iron() {m_ln[0][2] = 0; m_ln[1][2] = 0;}
	CPnt	GetCtrlPt() const {return m_ln.ProjPtAlong(.5);}
	void	GetExtents(CPnt&, CPnt&, const CTMat&) const;
	CPnt	GoToNxtCtrlPt() const;
	bool	Is(WORD wType) const {return wType == PRIM_LINE;}
	bool	IsInView() const;
	bool	IsPtACtrlPt(const CPnt4&) const;
	void	ReadPeg(HFILE hFile);
	CPnt	SelAtCtrlPt(const CPnt4&) const;
	bool	SelUsingLine(const CLine& ln, CPnts& ptsInt);
	bool	SelUsingPoint(const CPnt4&, double, CPnt&);
	bool	SelUsingRect(const CPnt&, const CPnt&);
	void	Transform(const CTMat& tm) {SetPt0(tm * Pt0()); SetPt1(tm * Pt1());}
	void	Translate(const CVec& v) {m_ln += v;}
	void	TranslateUsingMask(const CVec&, const DWORD);
	bool	Vectorize(const CSeg *pSeg);
	bool	WritePeg(HFILE hFile) const;
	void	Write(CFile&, char*) const;
	void	WriteJob(HFILE hFile, char*) const;
	bool	Write(OdDbEntityPtr&, OdDbDatabasePtr);
	
public: // Methods - virtuals
	void	CutAt2Pts(CPnt*, CSegs*, CSegs*);
	void	CutAtPt(const CPnt&, CSeg*);
	int 	IsWithinArea(const CPnt&, const CPnt&, CPnt*); 

public: // Methods
	double		GetAngAboutZAx() const {return m_ln.GetAngAboutZAx();}	
	void		GetLine(CLine& ln) const {ln = m_ln;}
 	void		GetPts(CPnt& ptBeg, CPnt& ptEnd) const {ptBeg = m_ln[0]; ptEnd = m_ln[1];}
	const CPnt&	Pt0() const {return m_ln[0];}
	const CPnt&	Pt1() const {return m_ln[1];}
	const CLine& Ln() const {return m_ln;}
	double		Length() const {return (m_ln.Length());}
	CPnt		ProjPt(const CPnt& pt) const {return (m_ln.ProjPt(pt));}
	double		RelOfPt(const CPnt& pt) const;
	void		SetPt0(const CPnt& pt) {m_ln[0] = pt;}
	void		SetPt1(const CPnt& pt) {m_ln[1] = pt;}
	void		Square();
};