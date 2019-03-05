#pragma once

class CPrim;

class CPrimDim : public CPrim
{
private:
	CLine		m_ln;
	
	PENCOLOR	m_nTextPenColor;
	CFontDef	m_fd;
	CRefSys		m_rs;
	CString		m_strText;

public:	// Constructors and destructor
	CPrimDim() {}
	CPrimDim(PENCOLOR nPenColor, PENSTYLE nPenStyle, const CLine& ln);
	CPrimDim(char*);
	CPrimDim(const CPrimDim& src); 
	~CPrimDim() {}
	
public: // Operators
	const CPrimDim& operator=(const CPrimDim& src);

public: // Methods - absolute virtuals
	void		Assign(CPrim* pPrim) {*this = *static_cast<CPrimDim*>(pPrim);}
	CPrim*&		Copy(CPrim*&) const;
	void		Display(CDC* pDC, bool bNodeDisplay);
	void		DisplayOpenGL(bool bNodeDisplay);
	void		DisRep(const CPnt&) const;
	void		FormatExtra(CString& str) const;
	void		FormatGeometry(CString& str) const;
	void		GetAllPts(CPnts& pts);
	CPnt		GetCtrlPt() const {return m_ln.ProjPtAlong(.5);}
	void		GetExtents(CPnt&, CPnt&, const CTMat&) const;
	CPnt		GoToNxtCtrlPt() const;
	void		Iron();
	bool		Is(WORD wType) const {return wType == PRIM_DIM;}
	bool		IsInView() const;
	bool		IsPtACtrlPt(const CPnt4&) const;
	void		ReadPeg(HFILE hFile);
	CPnt		SelAtCtrlPt(const CPnt4&) const;
	bool		SelUsingLine(const CLine& ln, CPnts& ptInt);
	bool		SelUsingPoint(const CPnt4&, double, CPnt&);
	bool		SelUsingRect(const CPnt&, const CPnt&);
	void		Transform(const CTMat&); 
	void		Translate(const CVec&);
	void		TranslateUsingMask(const CVec&, const DWORD);
	bool		Vectorize(const CSeg *pSeg);
	bool		WritePeg(HFILE hFile) const;
	void		Write(CFile&, char*) const;
	void		WriteJob(HFILE hFile, char*) const;
	bool		Write(OdDbEntityPtr&, OdDbDatabasePtr);

public:	// Methods - virtuals
	void		CutAt2Pts(CPnt*, CSegs*, CSegs*);
	void		CutAtPt(const CPnt&, CSeg*);
	void		ModifyState();
	void		GetState();

public:	// Methods
	void				GetBoundingBox(CPnts& ptsBox, double dSpacFac) const;
	const CFontDef&		FontDef() const {return m_fd;}
	const CLine&		Line() const {return m_ln;}
	void				GetPts(CPnt& ptBeg, CPnt& ptEnd) const {ptBeg = m_ln[0]; ptEnd = m_ln[1];}
	void				GetRefSys(CRefSys& rs) const {rs = m_rs;}
	double				Length() const {return m_ln.Length();}
	double				RelOfPt(const CPnt& pt) const;
	void				SetDefaultNote();
	void				SetPt0(const CPnt& pt) {m_ln[0] = pt;}
	void				SetPt1(const CPnt& pt) {m_ln[1] = pt;}
	void				SetText(const CString& str) {m_strText = str;}
	void				SetTextHorAlign(WORD w) {m_fd.TextHorAlignSet(w);}
	void				SetTextPenColor(PENCOLOR nPenColor) {m_nTextPenColor = nPenColor;}
	void				SetTextVerAlign(WORD w) {m_fd.TextVerAlignSet(w);}
	const CString&		Text() const {return m_strText;}
	const PENCOLOR&		TextPenColor() const {return m_nTextPenColor;}

private:
	static WORD mS_wFlags;	// bit 1	clear if dimension selected at note
							//			set if dimension selected at line
};