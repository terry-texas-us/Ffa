#pragma once

class CSegs : public CObList
{
public: // Constructors and destructor
	CSegs() {};

	virtual ~CSegs() {}

	const CSegs& operator=(const CSegs&);
	
public:	// Base class wrappers	
	POSITION	AddHead(CSeg* pSeg) {return (CObList::AddHead((CObject*) pSeg));}
	POSITION	AddTail(CSeg* pSeg) {return (CObList::AddTail((CObject*) pSeg));}
	void		AddTail(CSegs* pSegs) {CObList::AddTail((CObList*) pSegs);}
	POSITION	Find(CSeg* pSeg) {return (CObList::Find((CObject*) pSeg));}
	CSeg*		GetNext(POSITION& pos) const {return (CSeg*) CObList::GetNext(pos);}
	CSeg*		GetPrev(POSITION& pos) const {return (CSeg*) CObList::GetPrev(pos);}
	CSeg*		RemoveHead() {return (CSeg*) CObList::RemoveHead();}
	CSeg*		RemoveTail() {return (CSeg*) CObList::RemoveTail();}

public: // Methods
	void		BreakPolylines();
	void		BreakSegRefs();
	void		Display(CDC* pDC) const;
	int			GetBlockRefCount(const CString& strName) const;
	void		GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const;
	int			GetPenStyleRefCount(PENSTYLE nPenStyle) const;
	void		ModifyMarkers();
	void		ModifyNotes(const CFontDef& cd, const CCharCellDef& ccd, int iAtt = 0);
	void		ModifyPenColor(PENCOLOR nPenColor);
	void		ModifyPenStyle(PENSTYLE nPenStyle);
	void		PenTranslation(WORD, PENCOLOR*, PENCOLOR*);
	void		PenStyleTranslation(const PENSTYLE& nStyleNew, const PENSTYLE& nStyleOld);
	POSITION	Remove(CSeg* pSeg);
	int			RemoveEmptyNotes();
	int			RemoveEmptySegs();
	void		RemoveSegs();
	CSeg*		SelSegUsingPoint(const CPnt& pt);
	void		SetBitmapActLikePrim(bool bActLikePrim);
	void		Transform(const CTMat& tm);
	void		Translate(const CVec& v) const;
	void		Write(CFile& f, char* p);
	void		WritePeg(CFile& f, char* p);
};