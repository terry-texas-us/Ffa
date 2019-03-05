#include "stdafx.h"

#include "CharCellDef.h"

CCharCellDef::CCharCellDef()
{
	m_dChrHgt = .1;
	m_dChrExpFac = 1.;
	m_dTextRotAng = 0.;
	m_dChrSlantAng = 0.;
}

CCharCellDef::CCharCellDef(double dTxtOffAng, double dChrSlantAng, double dChrExpFac, double dChrHgt)
{
	m_dChrHgt = dChrHgt;
	m_dChrExpFac = dChrExpFac;
	m_dTextRotAng = dTxtOffAng;
	m_dChrSlantAng = dChrSlantAng;
}

CCharCellDef::CCharCellDef(const CCharCellDef& fd) 
{
	m_dChrHgt = fd.m_dChrHgt;
	m_dChrExpFac = fd.m_dChrExpFac;
	m_dTextRotAng = fd.m_dTextRotAng;
	m_dChrSlantAng = fd.m_dChrSlantAng;
}

CCharCellDef& CCharCellDef::operator=(const CCharCellDef& fd)
{
	m_dChrHgt = fd.m_dChrHgt;
	m_dChrExpFac = fd.m_dChrExpFac;
	m_dTextRotAng = fd.m_dTextRotAng;
	m_dChrSlantAng = fd.m_dChrSlantAng;

	return (*this);
}

///<summary>Produces the reference system vectors for a single charater cell.</summary>
void CharCellDef_EncdRefSys(const CCharCellDef& ccd, CVec& vRefX, CVec& vRefY)
{
	CVec vNorm = view::GetDirection();

	vRefY = view::GetVwUp();
	vRefY.RotAboutArbAx(vNorm, ccd.m_dTextRotAng);
	
	vRefX = vRefY;
	vRefX.RotAboutArbAx(vNorm, - HALF_PI);
	vRefY.RotAboutArbAx(vNorm, ccd.m_dChrSlantAng);
	vRefX *= .6 * ccd.m_dChrHgt * ccd.m_dChrExpFac;
	vRefY *= ccd.m_dChrHgt;
}