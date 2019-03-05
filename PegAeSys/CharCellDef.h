#pragma once

class CCharCellDef
{
private:
	double		m_dChrHgt;		// height of character cell
	double		m_dChrExpFac;	// expansion factor applied to character cell width
	double		m_dTextRotAng;	// rotation applied to the character cell
	double		m_dChrSlantAng;	// rotation applied to the vertical component of the character cell

public:
	
	CCharCellDef();
	CCharCellDef(double dTextRotAng, double dChrSlantAng, double dChrExpFac, double dChrHgt);
	
	CCharCellDef(const CCharCellDef& ccd);

	CCharCellDef& operator=(const CCharCellDef& ccd);

	double		ChrExpFacGet() const {return (m_dChrExpFac);}
	void		ChrExpFacSet(double d) {m_dChrExpFac = d;}
	double		ChrHgtGet() const {return (m_dChrHgt);}
	void		ChrHgtSet(double d) {m_dChrHgt = d;}
	double		ChrSlantAngGet() const {return (m_dChrSlantAng);}
	void		ChrSlantAngSet(double d) {m_dChrSlantAng = d;}
	double		TextRotAngGet() const {return (m_dTextRotAng);}
	void		TextRotAngSet(double d) {m_dTextRotAng = d;}

	friend void CharCellDef_EncdRefSys(const CCharCellDef& fd, CVec& vRefX, CVec& vRefY);
};

void CharCellDef_EncdRefSys(const CCharCellDef& ccd, CVec& vXAxRef, CVec& vYAxRef);