#pragma once

class CBlock : public CSeg
{
private:

	WORD		m_wBlkTypFlgs;		// block type flag values
									//		b0 set - anonymous block
									//		b1 set - block has attribute definitions
									//		b2 set - block is an external reference
									//		b3 set - not used
									//		b4 set - block is externally dependent
									//		b5 set - block is a resolved external reference
									//		b6 set - definition is referenced
	CPnt		m_ptBase;			// block base point
	CString		m_strXRefPathName; 	// external reference (XRef) path name

public:
	CBlock() {m_wBlkTypFlgs = 0;}
	CBlock(WORD, const CPnt&);
	CBlock(WORD, const CPnt&, const CString&);
	
	CPnt		GetBasePt() const {return m_ptBase;}
	WORD		GetBlkTypFlgs() const {return m_wBlkTypFlgs;}
	void		GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const;
	bool		HasAttributes() const {return (m_wBlkTypFlgs & 2) == 2;}
	bool		IsAnonymous() const {return (m_wBlkTypFlgs & 1) == 1;}
	bool		IsFromExternalReference() const {return (m_wBlkTypFlgs & 4) == 4;}
	void		SetBlkTypFlgs(WORD wBlkTypFlgs) {m_wBlkTypFlgs = wBlkTypFlgs;}
	void		SetBasePt(const CPnt& pt) {m_ptBase = pt;}
};

typedef CTypedPtrMap<CMapStringToOb, CString, CBlock*> CBlocks;