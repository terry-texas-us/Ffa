#pragma once

#include "Polygon.h"

class CPrimPolygon : public CPrim
{
private:
	short	m_nIntStyle;
	short	m_nIntStyleId;
	WORD	m_wPts;
	CPnt	m_ptOrig;
	CVec	m_vPosXAx;
	CVec	m_vPosYAx;
	CPnt*	m_Pt;

public:	// Constructors and destructor
	CPrimPolygon();
	CPrimPolygon(char*, int iVer);
	CPrimPolygon(OdDbEntity*, int);
	CPrimPolygon(WORD, CPnt*);
	CPrimPolygon(WORD, CPnt, const CVec&, const CVec&, const CPnt*);
	CPrimPolygon(const CPrimPolygon& src);
	~CPrimPolygon();

public: // Operators
	const CPrimPolygon& operator=(const CPrimPolygon&);
   
public: // Methods - absolute virtuals
	void	Assign(CPrim* pPrim) {*this = *static_cast<CPrimPolygon*>(pPrim);}
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
	bool	Is(WORD wType) const {return wType == PRIM_POLYGON;}
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
	const short&	IntStyle() const {return (m_nIntStyle);}
	const short&	IntStyleId() const {return (m_nIntStyleId);}
	CPnt			GetPt(int i) const {return (m_Pt[i]);}
	int 			GetPts() {return (m_wPts);}
	void			ModifyState();
	void			GetState();
	bool			PvtOnCtrlPt(const CPnt4&) const;
	void			SetIntStyle(const short n) {m_nIntStyle = n;}
	void			SetIntStyleId(const short n) {m_nIntStyleId = n;}
	void			SetHatRefVecs(double, double, double);

//private:
	WORD	SwingVertex() const;

	private:
		static WORD mS_wEdgeToEvaluate;
		static WORD mS_wEdge;
		static WORD mS_wPivotVertex;

	public:
		static WORD& EdgeToEvaluate() {return mS_wEdgeToEvaluate;}
		static WORD& Edge() {return mS_wEdge;}
};