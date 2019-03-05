#pragma once

class CPrimInsert : public CPrim
{
private:
	CString		m_strName;
	CPnt		m_pt;
	CVec		m_vX;
	CVec		m_vY;
	CVec		m_vZ;
	
	CVec		m_vScale;
	double		m_dRotation;

	WORD		m_wColCnt;
	WORD		m_wRowCnt;
	double		m_dColSpac;
	double		m_dRowSpac;

public: // Constructors and destructor
	CPrimInsert();
	CPrimInsert(const CString& strName, const CPnt& pt);
	CPrimInsert(const CPrimInsert&);
	~CPrimInsert() {};

public: // Operators
	const CPrimInsert& operator=(const CPrimInsert&);

public: // Methods - absolute virtuals
	void	Assign(CPrim* pPrim) {*this = *static_cast<CPrimInsert*>(pPrim);}
	CPrim*& Copy(CPrim*&) const;
	void	Display(CDC* pDC, bool bNodeDisplay);
	void	DisplayOpenGL(bool bNodeDisplay);
	void	DisRep(const CPnt&) const;
	void	GetAllPts(CPnts& pts) {pts.SetSize(0); pts.Add(m_pt);}
	void	FormatExtra(CString& str) const;
	void    FormatGeometry(CString& str) const;
	CPnt	GetCtrlPt() const;
	void	GetExtents(CPnt&, CPnt&, const CTMat&) const;
	CPnt	GoToNxtCtrlPt() const {return m_pt;}
	void	Iron() {m_pt[2] = 0;}
	bool	Is(WORD wType) const {return wType == PRIM_INSERT;}
	bool	IsInView() const;
	bool	IsPtACtrlPt(const CPnt4&) const {return false;}
	void	ReadPeg(HFILE hFile);
	CPnt	SelAtCtrlPt(const CPnt4&) const;
	bool	SelUsingLine(const CLine&, CPnts&) {return false;}
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

	CTMat		BuildTransformMatrix(const CPnt& ptBase) const;
	CString&	BlkNam() {return m_strName;}
	WORD&		ColCnt() {return m_wColCnt;}
	double& 	ColSpacing() {return m_dColSpac;}
	CPnt&		InsPt() {return m_pt;}
	CVec&		XRefVec() {return m_vX;}
	CVec&		YRefVec() {return m_vY;}
	CVec&		ZRefVec() {return m_vZ;}
	WORD&		RowCnt() {return m_wRowCnt;}
	double& 	RowSpacing() {return m_dRowSpac;}
};