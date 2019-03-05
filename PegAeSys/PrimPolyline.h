#pragma once

class CPrimPolyline : public CPrim
{
private:
	WORD	m_wFlags;
	CPnts	m_pts;

public:	// Constructors and destructor
	CPrimPolyline();
	CPrimPolyline(char*);
	CPrimPolyline(OdDbEntity*, int);
	CPrimPolyline(CPnts& pts);
	CPrimPolyline(WORD wPts, CPnt* pPts);
	CPrimPolyline(const CPrimPolyline&);
	~CPrimPolyline() {}

public: // Operators
	const CPrimPolyline& operator=(const CPrimPolyline&);
	
public: // Methods - absolute virtuals
	void	Assign(CPrim* pPrim) {*this = *static_cast<CPrimPolyline*>(pPrim);}
	CPrim*& Copy(CPrim*&) const;
	void	Display(CDC* pDC, bool bNodeDisplay);
	void	DisplayOpenGL(bool bNodeDisplay);
	void	DisRep(const CPnt&) const;
	void	GetAllPts(CPnts& pts) {pts.SetSize(0); pts.Copy(m_pts);}
	void	FormatExtra(CString& str) const;
	void    FormatGeometry(CString& str) const;
	WORD	GetFlag() const {return m_wFlags;}
	CPnt	GetCtrlPt() const;
	void	GetExtents(CPnt&, CPnt&, const CTMat&) const;
	CPnt	GoToNxtCtrlPt() const;
	void	Iron();
	bool	Is(WORD wType) const {return wType == PRIM_POLYLINE;}
	bool	IsInView() const;
	bool	IsPtACtrlPt(const CPnt4&) const {return false;}
	bool	PvtOnCtrlPt(const CPnt4& ptView) const;
	void	ReadPeg(HFILE hFile);
	CPnt	SelAtCtrlPt(const CPnt4&) const;
	bool	SelUsingLine(const CLine&, CPnts&) {return false;}
	bool	SelUsingPoint(const CPnt4&, double, CPnt&);
	bool	SelUsingRect(const CPnt&, const CPnt&);
	void	SetFlag(const WORD w) {m_wFlags = w;}
	void	SetPt(WORD w, const CPnt& pt) {m_pts[w] = pt;}
	void	Transform(const CTMat&);	
	void	Translate(const CVec&);
	void	TranslateUsingMask(const CVec&, const DWORD);
	bool	Vectorize(const CSeg *pSeg);
	bool	WritePeg(HFILE hFile) const;
	void	Write(CFile&, char*) const;
	void	WriteJob(HFILE hFile, char*) const;
	bool	Write(OdDbEntityPtr&, OdDbDatabasePtr);

public: // Methods
	bool	IsLooped() const {return (m_wFlags & AD_PLINE_CLOSED) == AD_PLINE_CLOSED;}
	static const WORD AD_PLINE_CLOSED = 1;

private:
	WORD	SwingVertex() const;

	private:
		static	WORD mS_wEdgeToEvaluate;
		static	WORD mS_wEdge;
		static	WORD mS_wPivotVertex;

	public:
		static	WORD& EdgeToEvaluate() {return mS_wEdgeToEvaluate;}
		static	WORD& Edge() {return mS_wEdge;}
};