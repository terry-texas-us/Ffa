#pragma once

class CPrimMark : public CPrim
{
private:
	short	m_nMarkStyle;
	CPnt	m_pt;
	WORD	m_Dats;
	double* m_dDat;

public: // Constructors and destructor
	CPrimMark();
	CPrimMark(char*, int iVer);
	CPrimMark(OdDbEntity*);
	CPrimMark(PENCOLOR nPenColor, short nMarkStyle, const CPnt& pt);
	CPrimMark(const CPrimMark& src);
	~CPrimMark();		
	
public: // Operators
	const	CPrimMark& operator=(const CPrimMark& src);

public: // Methods - absolute virtuals
	void	Assign(CPrim* pPrim) {*this = *static_cast<CPrimMark*>(pPrim);}
	CPrim*& Copy(CPrim*&) const;
	void	Display(CDC* pDC, bool bNodeDisplay);
	void	DisplayOpenGL(bool bNodeDisplay);
	void	DisRep(const CPnt&) const;
	void	FormatExtra(CString& str) const;
	void    FormatGeometry(CString& str) const;
	void	GetAllPts(CPnts& pts) {pts.SetSize(0); pts.Add(m_pt);}
	CPnt	GetCtrlPt() const;
	void	GetExtents(CPnt&, CPnt&, const CTMat&) const;
	CPnt	GoToNxtCtrlPt() const {return (m_pt);}
	void	Iron() {m_pt[2] = 0;}
	bool	Is(WORD wType) const {return wType == PRIM_MARK;}
	bool	IsInView() const;
	bool	IsPtACtrlPt(const CPnt4&) const;
	void	ReadPeg(HFILE hFile);
	CPnt	SelAtCtrlPt(const CPnt4&) const;
	bool	SelUsingLine(const CLine&, CPnts&) {return false;}
	bool	SelUsingPoint(const CPnt4&, double, CPnt&);
	bool	SelUsingRect(const CPnt&, const CPnt&);
	void	Transform(const CTMat&); 
	void	Translate(const CVec& v) {m_pt += v;}
	void	TranslateUsingMask(const CVec&, const DWORD);
	bool	Vectorize(const CSeg *pSeg);
	bool	WritePeg(HFILE hFile) const;
	void	Write(CFile&, char*) const;
	void	WriteJob(HFILE hFile, char*) const;
	bool	Write(OdDbEntityPtr&, OdDbDatabasePtr);

public: // Methods
	double	GetDat(WORD wDat) const {return (m_dDat[wDat]);}
	CPnt	GetPt() const {return (m_pt);}
	short&	MarkStyle() {return m_nMarkStyle;}
	void	ModifyState();
	void	GetState();
	void	SetDat(WORD, double*);
	void	SetPt(CPnt pt) {m_pt = pt;}
};