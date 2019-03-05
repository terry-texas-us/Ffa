#pragma once

typedef short PENSTYLE;
typedef short PENCOLOR;

class CSegs;
class CSeg;

class CPrim : public CObject
{
public:
	static const WORD BUFFER_SIZE = 4096;

	static const PENCOLOR PENCOLOR_BYBLOCK = 0;
	static const PENCOLOR PENCOLOR_BYLAYER = 256;
	static const PENSTYLE PENSTYLE_BYBLOCK = 32766;
	static const PENSTYLE PENSTYLE_BYLAYER = 32767;

	static const WORD PRIM_MARK		= 256;		// 0x0100
	static const WORD PRIM_INSERT	= 257;		// 0x0101
	static const WORD PRIM_SEGREF	= 258;		// 0x0102
	static const WORD PRIM_LINE		= 512;		// 0x0200
	static const WORD PRIM_POLYGON	= 1024;		// 0x0400
	static const WORD PRIM_ARC		= 4099;		// 0x1003
	static const WORD PRIM_BSPLINE	= 8192;		// 0x2000
	static const WORD PRIM_CSPLINE	= 8193;		// 0x2001
	static const WORD PRIM_POLYLINE	= 8194;		// 0x2002
	static const WORD PRIM_TEXT		= 16384;	// 0x4000
	static const WORD PRIM_TAG		= 16640;	// 0x4100
	static const WORD PRIM_DIM		= 16896;	// 0x4200

	static const WORD PRIM_BITMAP	= 0x4201;

protected:
	PENCOLOR	m_nPenColor;
	PENSTYLE	m_nPenStyle;

	static	PENCOLOR	mS_nLayerPenColor;
	static	PENSTYLE	mS_nLayerPenStyle;
	static	PENCOLOR	mS_nSpecPenColor;
	static	PENSTYLE	mS_nSpecPenStyle;
	static	short		mS_nSpecPolygonStyle;
	static	WORD		mS_wCtrlPt;
	static	double		mS_dRel;
	static	double		mS_dPicApertSiz;

public: // Constructors and destructor
	CPrim() {m_nPenColor = 0; m_nPenStyle = 0;}
	virtual ~CPrim() {}
	
public: // Methods - absolute virtuals
	virtual void	Assign(CPrim*) = 0;
	virtual CPrim*&	Copy(CPrim*&) const = 0; 
	virtual void	Display(CDC* pDC, bool bNodeDisplay) = 0;
	virtual void	DisplayOpenGL(bool bNodeDisplay) = 0;
	virtual void	DisRep(const CPnt&) const = 0;
	virtual void	FormatExtra(CString& str) const = 0;
	virtual void	FormatGeometry(CString& str) const = 0;
	virtual void	GetAllPts(CPnts& pts) = 0;
	virtual CPnt	GetCtrlPt() const = 0;
	virtual void	GetExtents(CPnt&, CPnt&, const CTMat&) const = 0;
	virtual CPnt	GoToNxtCtrlPt() const = 0;
	virtual void	Iron() = 0;
	virtual bool	Is(WORD wType) const = 0;
	virtual bool	IsInView() const = 0;
	virtual bool	IsPtACtrlPt(const CPnt4&) const = 0;
	virtual void	ReadPeg(HFILE hFile) = 0;
	virtual CPnt	SelAtCtrlPt(const CPnt4&) const = 0;
	virtual bool	SelUsingLine(const CLine& ln, CPnts& ptsInt) = 0;
	virtual bool	SelUsingPoint(const CPnt4&, double, CPnt&) = 0;
	virtual bool	SelUsingRect(const CPnt&, const CPnt&) = 0;
	virtual void	Transform(const CTMat&) = 0;
	virtual void	Translate(const CVec&) = 0;
	virtual void	TranslateUsingMask(const CVec&, const DWORD) = 0;
	virtual bool	Vectorize(const CSeg *pSeg) = 0;
	virtual bool	WritePeg(HFILE hFile) const = 0;
	virtual void	Write(CFile& fl, char*) const = 0;
	virtual void	WriteJob(HFILE hFile, char*) const = 0;
	virtual bool	Write(OdDbEntityPtr&, OdDbDatabasePtr) = 0;

public: // Methods - virtuals
	virtual void	CutAt2Pts(CPnt*, CSegs*, CSegs*) {}
	virtual void	CutAtPt(const CPnt&, CSeg*) {}
	virtual int 	IsWithinArea(const CPnt&, const CPnt&, CPnt*) {return 0;}
	virtual void	ModifyState();
	virtual void	GetState();
	virtual bool	PvtOnCtrlPt(const CPnt4&) const {return false;}
	
public: // Methods
	CString				FormatPenColor() const;
	CString				FormatPenStyle() const;
	PENCOLOR			LogicalPenColor() const;
	PENSTYLE			LogicalPenStyle() const;
	PENCOLOR&			PenColor() {return m_nPenColor;}
	PENSTYLE&			PenStyle() {return m_nPenStyle;}
	virtual void		SetPenColor(PENCOLOR newCol) { m_nPenColor = newCol; }
	virtual void		SetPenStyle(PENSTYLE newSty) { m_nPenStyle = newSty; }

	virtual CString		GetText() {CString str = ""; return str;}
	virtual void		SetText(const CString&) { }

public:	// Methods - static 
	static WORD&		CtrlPt() {return mS_wCtrlPt;}

	// mak - added three to exclusion list
	static bool			IsSupportedTyp(int iTyp);

	static PENCOLOR&	LayerPenColor() {return mS_nLayerPenColor;}
	static PENSTYLE&	LayerPenStyle() {return mS_nLayerPenStyle;}
	static double&		PicApertSiz() {return mS_dPicApertSiz;}
	static double&		Rel() {return mS_dRel;}
	static PENSTYLE&	SpecPenStyle() {return mS_nSpecPenStyle;}
	static PENCOLOR&	SpecPenColor() {return mS_nSpecPenColor;}
	static short&		SpecPolygonStyle() {return mS_nSpecPolygonStyle;}
};

CVec Prim_ComputeArbitraryAxis(const CVec& vNorm);

inline bool Prim_IsByBlockPenStyle(const char* sName) {return (_stricmp(sName, "ByBlock") == 0 ? true : false);}
inline bool Prim_IsByLayerPenStyle(const char* sName) {return (_stricmp(sName, "ByLayer") == 0 ? true : false);}