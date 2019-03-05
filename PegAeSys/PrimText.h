#pragma once

class CPrimText : public CPrim
{
private:
	CFontDef	m_fd;
	CRefSys		m_rs;
	CString		m_strText;

public:	// Constructors and destructor
	CPrimText() {}
	CPrimText(char*, int iVer);
	CPrimText(OdDbEntity*, int);
	CPrimText(const CFontDef& fd, const CRefSys& rs, const char* pszText);
	CPrimText(const CPrimText&);
	~CPrimText() {};

public: // Operators
	const CPrimText& operator=(const CPrimText&);
	
public: // Methods - absolute virtuals
	void	Assign(CPrim* pPrim) {*this = *static_cast<CPrimText*>(pPrim);}
	CPrim*& Copy(CPrim*&) const;
	void	Display(CDC* pDC, bool bNodeDisplay);
	void	DisplayOpenGL(bool bNodeDisplay);
	void	DisRep(const CPnt&) const;
	void	GetAllPts(CPnts& pts) {pts.SetSize(0); CPnt pt = m_rs.Origin(); pts.Add(pt);}
	void	FormatExtra(CString& str) const;
	void    FormatGeometry(CString& str) const;
	CPnt	GetCtrlPt() const;
	void	GetExtents(CPnt&, CPnt&, const CTMat&) const;
	CPnt	GoToNxtCtrlPt() const {return (m_rs.Origin());}
	void	Iron();
	bool	IsInView() const;
	bool	Is(WORD wType) const {return wType == PRIM_TEXT;}
	bool	IsPtACtrlPt(const CPnt4&) const;
	void	ModifyState();
	void	GetState();
	void	ModifyNotes(const CFontDef& fd, const CCharCellDef& ccd, int iAtt);
	CPnt	SelAtCtrlPt(const CPnt4&) const;
	bool	SelUsingLine(const CLine&, CPnts&);
	bool	SelUsingPoint(const CPnt4&, double, CPnt&);
	bool	SelUsingRect(const CPnt&, const CPnt&);
	void	Translate(const CVec& v) {m_rs.SetOrigin(m_rs.Origin() + v);}
	void	TranslateUsingMask(const CVec&, const DWORD);
	void	ReadPeg(HFILE hFile);
	void	Transform(const CTMat&);
	bool	Vectorize(const CSeg *pSeg);
	bool	WritePeg(HFILE hFile) const;
	void	Write(CFile&, char*) const;
	void	WriteJob(HFILE hFile, char*) const;
	bool	Write(OdDbEntityPtr&, OdDbDatabasePtr);
	
public: // Methods
	void				GetBoundingBox(CPnts&, double) const;
	void				GetFontDef(CFontDef& fd) const {fd = m_fd;}
	void				GetRefSys(CRefSys& rs) const {rs = m_rs;}
	const CString&		Text() const {return m_strText;}
	CVec				RefNorm() const {CVec vNorm; m_rs.GetUnitNormal(vNorm); return vNorm;}
	CPnt				RefPt() const {return m_rs.Origin();}
	void				SetText(const CString& str) {m_strText = str;}
	CString				GetText() { return m_strText; }
};