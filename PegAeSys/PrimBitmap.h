#pragma once

class CPrimBitmap : public CPrim
{
private:
	CPnt				m_ptUL;
	CPnt				m_ptLR;
	CPnt				m_ptUR;
	CPnt				m_ptLL;

	CPnt				m_ptUL_ORIGINAL;
	CPnt				m_ptLR_ORIGINAL;
	CPnt				m_ptUR_ORIGINAL;
	CPnt				m_ptLL_ORIGINAL;

	CSize				m_size;
	CSize				m_size_ORIGINAL;

	double				m_dScaleX;
	double				m_dScaleY;
	double				m_dRotation;

	//CString				m_strPath;

	CBitmap				m_bm;
	CBitmap				m_bm_ORIGINAL;

	CPalette			m_pal;

	bool				m_bActLikePrim;

	BITMAPINFOHEADER	BMIH;
	//BYTE*				m_pDrawingSurfaceBits;

	CString				m_strFileName;

	CBitmapEx			m_bitmapEx;

public:	// Constructors and destructor
	CPrimBitmap() {}
	CPrimBitmap(CPnt ptPos);
	CPrimBitmap(CPnt ptPos, HBITMAP hBitmap);
//	CPrimBitmap(OdDbEntity*, int);
//	CPrimBitmap(WORD, CPnt*);
	CPrimBitmap(CPnt*);
//	CPrimBitmap(WORD, CPnt, const CVec&, const CVec&, const CPnt*);
	CPrimBitmap(const CPrimBitmap& src);
	CPrimBitmap(CPnt ptPos, CString strFilename);
	~CPrimBitmap();

public: // Operators
	const CPrimBitmap& operator=(const CPrimBitmap&);
   
public: // Methods - absolute virtuals
	void	Assign(CPrim* pPrim) {*this = *static_cast<CPrimBitmap*>(pPrim);}
	CPrim*& Copy(CPrim*&) const;
	void	Display(CDC* pDC, bool bNodeDisplay);
	void	DisplayOpenGL(bool bNodeDisplay);
	void	DisRep(const CPnt&) const;
	void	FormatExtra(CString& str) const;
	void    FormatGeometry(CString& str) const;
	void	GetAllPts(CPnts& pts);
	CPnt	GetCtrlPt() const;
	void	GetExtents(CPnt&, CPnt&, const CTMat&) const;
	CPnt	GoToNxtCtrlPt() const;
	void	Iron();
	bool	Is(WORD wType) const {return wType == PRIM_BITMAP;}
	bool	IsPtACtrlPt(const CPnt4&) const;
	bool	IsInView() const;
	void	ReadPeg(HFILE hFile);
	CPnt	SelAtCtrlPt(const CPnt4&) const;
	bool	SelUsingLine(const CLine&, CPnts&);
	bool	SelUsingPoint(const CPnt4&, double, CPnt&);
	bool	SelUsingRect(const CPnt&, const CPnt&);
	void	Transform(const CTMat&);
	void	Translate(const CVec&);
	void	TranslateUsingMask(const CVec&, const DWORD);
	bool	Vectorize(const CSeg *pSeg);
	bool	WritePeg(HFILE hFile) const;
	void	Write(CFile&, char*) const;
	void	WriteJob(HFILE hFile, char*) const;
	bool	Write(OdDbEntityPtr&, OdDbDatabasePtr);

	CString			FormatIntStyle() const;
//	const short&	IntStyle() const {return (m_nIntStyle);}
//	const short&	IntStyleId() const {return (m_nIntStyleId);}
//	CPnt			GetPt(int i) const {return (m_Pt[i]);}
//	int 			GetPts() {return (m_wPts);}
	void			ModifyState();
	void			GetState();
	bool			PvtOnCtrlPt(const CPnt4&) const;
//	void			SetIntStyle(const short n) {m_nIntStyle = n;}
//	void			SetIntStyleId(const short n) {m_nIntStyleId = n;}
//	void			SetHatRefVecs(double, double, double);

	void			SetActLikePrim(bool bActLikePrim);

private:
	
	CPoint			UpdateLR(CPoint);
	void			SaveBitmapFile(FILE *pFile) const;
	void			LoadBitmapFile(FILE *pFile) const;

	//void			BitmapLayerSave(CString filename);
	void			BitmapLayerLoad();

	void			Init(CPnt ptPos, HBITMAP hBitmap);

//private:
//	WORD	SwingVertex() const;

//	private:
//		static WORD mS_wEdgeToEvaluate;
//		static WORD mS_wEdge;
//		static WORD mS_wPivotVertex;
//
//	public:
//		static WORD& EdgeToEvaluate() {return mS_wEdgeToEvaluate;}
//		static WORD& Edge() {return mS_wEdge;}
};