#pragma once

class CPrim;
class CPrimLine;
class CPrimText;

class CSegsDet : public CSegs
{
private:
	CPnt	m_ptDet;
	double	m_dPicApertSiz;
	CSeg*	m_pDetSeg;
	CPrim*	m_pDetPrim;

public:
	CSegsDet();
	CSegsDet(const CSegsDet &b);
	~CSegsDet() {}

	CSegsDet& CSegsDet::operator= (const CSegsDet& param);

	void		Build();
	void		DelLast();
	CSeg*&		DetSeg() {return m_pDetSeg;}
	CPrim*&		DetPrim() {return m_pDetPrim;}
	CPnt&		DetPt() {return m_ptDet;}
	bool		IsSegEngaged() const {return m_pDetSeg != 0;}
	CSeg*		SelLineUsingPoint(const CPnt& pt);
	CSeg*		SelSegAndPrimUsingPoint(const CPnt&);
	CSeg*		SelSegAndPrimAtCtrlPt(const CPnt4& pt);
	CPrimText*	SelTextUsingPoint(const CPnt& pt);
	double&		PicApertSiz() {return m_dPicApertSiz;}

	void		SetSeg(CSeg *pSeg) { m_pDetSeg = pSeg; }
	void		SetPrim(CPrim *pPrim) { m_pDetPrim = pPrim; }
};

extern CSegsDet	detsegs;