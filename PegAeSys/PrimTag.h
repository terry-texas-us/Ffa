#pragma once

class CPrimTag : public CPrim
{
private:
	CPnt	m_Pt;

public:	// Constructors and destructor
	CPrimTag() {}
	CPrimTag(const CPrimTag&);
	CPrimTag(char*);

public: // Operators
	const CPrimTag& operator=(const CPrimTag&);

public: // Methods - absolute virtuals
	void	Assign(CPrim* pPrim) {*this = *static_cast<CPrimTag*>(pPrim);}
	CPrim*& Copy(CPrim*&) const;
	void	Display(CDC* pDC, bool bNodeDisplay);
	void	DisplayOpenGL(bool bNodeDisplay);
	void	DisRep(const CPnt&) const;
	void	GetAllPts(CPnts& pts) {pts.SetSize(0); pts.Add(m_Pt);}
	void	FormatExtra(CString& str) const;
	void    FormatGeometry(CString& str) const;
	CPnt	GetCtrlPt() const;
	void	GetExtents(CPnt&, CPnt&, const CTMat&) const;
	CPnt	GoToNxtCtrlPt() const {return (m_Pt);}
	void	Iron() {m_Pt[2] = 0;}
	bool	Is(WORD wType) const {return wType == PRIM_TAG;}
	bool	IsInView() const;
	bool	IsPtACtrlPt(const CPnt4&) const;
	CPnt	SelAtCtrlPt(const CPnt4&) const;
	bool	SelUsingLine(const CLine&, CPnts&) {return false;}
	bool	SelUsingPoint(const CPnt4&, double, CPnt&);
	bool	SelUsingRect(const CPnt&, const CPnt&);
	void	Translate(const CVec& v) {m_Pt += v;}
	void	TranslateUsingMask(const CVec&, const DWORD);
	void	ReadPeg(HFILE hFile);
	void	Transform(const CTMat&); 
	bool	Vectorize(const CSeg *pSeg);
	bool	WritePeg(HFILE hFile) const;
	void	Write(CFile&, char*) const;
	void	WriteJob(HFILE hFile, char*) const;
	bool	Write(OdDbEntityPtr&, OdDbDatabasePtr);
};