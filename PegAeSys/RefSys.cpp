#include "stdafx.h"

CRefSys::CRefSys(const CPnt& ptOrigin, const CVec& vDirX, const CVec& vDirY)
{
	m_ptOrigin = ptOrigin;
	m_vDirX = vDirX;
	m_vDirY = vDirY;
}

CRefSys::CRefSys(const CPnt& ptOrigin, const CCharCellDef& ccd)
{
	m_ptOrigin = ptOrigin;

	CVec vNorm = view::GetDirection();

	m_vDirY = view::GetVwUp();
	m_vDirY.RotAboutArbAx(vNorm, ccd.TextRotAngGet());
	
	m_vDirX = m_vDirY;
	m_vDirX.RotAboutArbAx(vNorm, - HALF_PI);
	m_vDirY.RotAboutArbAx(vNorm, ccd.ChrSlantAngGet());
	m_vDirX *= .6 * ccd.ChrHgtGet() * ccd.ChrExpFacGet();
	m_vDirY *= ccd.ChrHgtGet();

	m_ccd = ccd;
}

CRefSys::CRefSys(const CRefSys& src)
{
	m_ptOrigin = src.m_ptOrigin;
	m_vDirX = src.m_vDirX;
	m_vDirY = src.m_vDirY;
}

CRefSys& CRefSys::operator=(const CRefSys& src)
{
	m_ptOrigin = src.m_ptOrigin;
	m_vDirX = src.m_vDirX;
	m_vDirY = src.m_vDirY;
	
	return (*this);
}

CTMat CRefSys::GetTransformMatrix() const
{
	return CTMat(m_ptOrigin, m_vDirX, m_vDirY);
}

void CRefSys::Read(HFILE hFile)
{
	m_ptOrigin.Read(hFile);
	m_vDirX.Read(hFile);
	m_vDirY.Read(hFile);
}

///<summary>Takes the current reference directions and rescales using passed character cell state.</summary>
void CRefSys::Rescale(const CCharCellDef& ccd)
{
	CVec vNorm;
	GetUnitNormal(vNorm);
	m_vDirX.Normalize();
	m_vDirY = m_vDirX;
	m_vDirY.RotAboutArbAx(vNorm, HALF_PI + ccd.ChrSlantAngGet());
	m_vDirX *= .6 * ccd.ChrHgtGet() * ccd.ChrExpFacGet();
	m_vDirY *= ccd.ChrHgtGet();

	m_ccd = ccd;
}

void CRefSys::Transform(const CTMat& tm)
{
	m_ptOrigin = tm * m_ptOrigin;
	m_vDirX = tm * m_vDirX;
	m_vDirY = tm * m_vDirY;
}

void CRefSys::Write(HFILE hFile) const
{
	m_ptOrigin.Write(hFile);
	m_vDirX.Write(hFile);
	m_vDirY.Write(hFile);
}