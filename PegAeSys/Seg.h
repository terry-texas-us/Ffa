#pragma once

class CBlock;

class CSeg : public CObList
{
public:
	CSeg() {}
	CSeg(CPrim* p) {AddTail(p);}
	CSeg(const CSeg&);	
	CSeg(const CBlock&);

	void		BreakPolylines();
	void		BreakSegRefs();
	void		Display(CDC* pDC, bool bNodeDisplay);//, int nColor = -1);
	POSITION	FindAndRemovePrim(CPrim*);
	CPrim*		GetAt(POSITION pos) const {return (CPrim*) CObList::GetAt(pos);}
	int			GetBlockRefCount(const CString& strName) const;
	void		GetExtents(CPnt&, CPnt&, const CTMat&) const;
	CPrim*		GetNext(POSITION& pos) const {return ((CPrim*) CObList::GetNext(pos));}
	int			GetPenStyleRefCount(PENSTYLE nPenStyle) const;
	void		InsertBefore(POSITION pos, CSeg* pSeg);
	bool		IsInView() const;
	void		ModifyMarkers();
	void		ModifyNotes(const CFontDef& fd, const CCharCellDef& ccd, int iAtt = 0);
	void		ModifyPenColor(PENCOLOR nPenColor);
	void		ModifyPenStyle(PENSTYLE nPenStyle);
	void		PenTranslation(WORD, PENCOLOR*, PENCOLOR*);
	void		PenStyleTranslation(const PENSTYLE& nStyleNew, const PENSTYLE& nStyleOld);
	int			RemoveEmptyNotes();
	void		RemovePrims();
	CPrim*		SelPrimAtCtrlPt(const CPnt4&, CPnt*) const;
	CPrim*		SelPrimUsingPoint(const CPnt4&, double&, CPnt&);
	bool		SelUsingRect(const CPnt& pt1, const CPnt& pt2) const;
	void		SetBitmapActLikePrim(bool bActLikePrim);
	void		SortTextOnY();
	void		Square();
	void		Transform(const CTMat& tm);
	void		Translate(const CVec&) const;
	void		Vectorize() const;
	void		WritePeg(HFILE hFile);
	void		Write(CFile&, char*);
	void		WriteJob(HFILE hFile, char*);
	void		Write(OdDbObjectId, OdDbDatabasePtr);

	

// mak 
public: // Operators
//	bool operator==(const CSeg* pSeg) const;
//	bool operator==(CSeg pSeg) const;
////	friend bool operator==(const CSeg &p1, const CSeg &p2);
//	friend bool operator==(CSeg p1, CSeg p2);

public:
	static CPrim*& IgnorePrim() {return mS_pPrimIgnore;}

private:
	static CPrim* mS_pPrimIgnore;
};