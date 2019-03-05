#pragma once

class CSegsDet;

class CSegsTrap : public CSegs
{
private:
	bool		m_bIdentify;	// trap is displayed using special pen color and style
	PENCOLOR	m_nPenColor;
	PENSTYLE	m_nPenStyle;
	CPnt		m_ptPvt;
public:
	CSegsTrap();
	~CSegsTrap() {}
	
	void		AddSegsAtPt(CSegsDet*, CPnt);
	void		Compress();
	void		Copy(const CVec& vTrns);
	void		DeleteSegs();
	void		Expand();
	bool		Identify() const {return m_bIdentify;}
	PENCOLOR	PenColor() const {return m_nPenColor;}
	PENSTYLE	PenStyle() const {return m_nPenStyle;}
	void		RemoveSegsAtPt(CPnt pt);
	void		SetIdentify(bool b) {m_bIdentify = b;}
	void		Square();
	void		TransformSegs(const CTMat& tm);
	CPnt&		PvtPt() {return m_ptPvt;}
	void		Vectorize();
//	void		SetActLikePrim(bool bActLikePrim) { SetActLikePrim(bActLikePrim); }
	void		SetActLikePrim(bool bActLikePrim) { SetBitmapActLikePrim(bActLikePrim); }
};

extern CSegsTrap trapsegs;