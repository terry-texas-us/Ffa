#pragma once

class CPrimSegRef : public CPrim
{
private:
	CString		m_strName;
	CPnt		m_pt;
	CVec		m_vZ;

	CVec		m_vScale;
	double		m_dRotation;

	WORD		m_wColCnt;
	WORD		m_wRowCnt;
	double		m_dColSpac;
	double		m_dRowSpac;

public: // Constructors and destructor
	CPrimSegRef();
	CPrimSegRef(const CString& strName, const CPnt& pt);
	CPrimSegRef(OdDbEntity*, int);
	CPrimSegRef(const CPrimSegRef&);
	virtual ~CPrimSegRef() {};

public: // Operators
	const CPrimSegRef& operator=(const CPrimSegRef&);

public: // Methods - absolute virtuals
	void	Assign(CPrim* pPrim) {*this = *static_cast<CPrimSegRef*>(pPrim);}
	CPrim*& Copy(CPrim*&) const;
	void	Display(CDC* pDC, bool bNodeDisplay);
	void	DisplayOpenGL(bool bNodeDisplay);
	void	DisRep(const CPnt&) const;
	void	GetAllPts(CPnts& pts) {pts.SetSize(0); pts.Add(m_pt);}
	void	Iron();
	void	FormatExtra(CString& str) const;
	void    FormatGeometry(CString& str) const;
	CPnt	GetCtrlPt() const;
	void	GetExtents(CPnt&, CPnt&, const CTMat&) const;
	CPnt	GoToNxtCtrlPt() const {return m_pt;}
	bool	Is(WORD wType) const {return wType == PRIM_SEGREF;}
	bool	IsInView() const;
	bool	IsPtACtrlPt(const CPnt4&) const {return false;}
	void	ReadPeg(HFILE hFile);
	CPnt	SelAtCtrlPt(const CPnt4&) const;
	bool	SelUsingLine(const CLine&, CPnts&);
	bool	SelUsingPoint(const CPnt4&, double, CPnt&);
	bool	SelUsingRect(const CPnt&, const CPnt&);
	void	Transform(const CTMat&);
	void	Translate(const CVec& v) {m_pt += v;}
	void	TranslateUsingMask(const CVec& v, const DWORD dwMask);
	bool	Vectorize(const CSeg *pSeg);
	bool	WritePeg(HFILE hFile) const;
	void	Write(CFile&, char*) const;
	void	WriteJob(HFILE hFile, char*) const;
	bool	Write(OdDbEntityPtr&, OdDbDatabasePtr);

public: // Methods
	CTMat	BuildTransformMatrix(const CPnt& ptBase) const;
	WORD&		ColCnt() {return m_wColCnt;}
	double& 	ColSpacing() {return m_dColSpac;}
	CString		GetName() const {return m_strName;}
	double 		GetRotation() {return m_dRotation;}
	CVec		GetScale() const {return m_vScale;}
	CPnt&		InsPt() {return m_pt;}
	CVec&		ZRefVec() {return m_vZ;}
	WORD&		RowCnt() {return m_wRowCnt;}
	double& 	RowSpacing() {return m_dRowSpac;}
	void		ModifyState();
	void		GetState();

	// mak
	void	SetPenColor(PENCOLOR newCol);
	void	SetPenStyle(PENSTYLE newSty);
};