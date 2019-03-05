#pragma once

#include "CharCellDef.h"

class CCharCellDef;
class CRefSys
{
protected:
	CPnt	m_ptOrigin;
	CVec	m_vDirX;
	CVec	m_vDirY;

	CCharCellDef m_ccd;

public: // Constructors and destructor
	
	CRefSys() {m_ptOrigin = ORIGIN; m_vDirX = XDIR; m_vDirY = YDIR;}
	CRefSys(const CPnt& ptOrigin, const CVec& vDirX, const CVec& vDirY);
	CRefSys(const CPnt& ptOrigin, const CCharCellDef& ccd);
	
	CRefSys(const CRefSys& cs);
	
	~CRefSys() {}

public: // Operators
	
	CRefSys& operator=(const CRefSys&);

public: // Methods

	const CVec&	DirX() const {return m_vDirX;}
	const CVec&	DirY() const {return m_vDirY;}
	CTMat		GetTransformMatrix() const;
	void		GetUnitNormal(CVec& vNorm) const {vNorm = m_vDirX ^ m_vDirY; vNorm.Normalize();}
	const CPnt& Origin() const {return m_ptOrigin;}
//	void		Read(CFile& fl);
	void		Read(HFILE hFile);
	void		Rescale(const CCharCellDef& ccd);
	void		Set(const CPnt& ptOrigin, const CVec& vDirX, const CVec& vDirY) {m_ptOrigin = ptOrigin; m_vDirX = vDirX; m_vDirY = vDirY;}
	void		SetOrigin(const CPnt& ptOrigin) {m_ptOrigin = ptOrigin;}
	void		SetDirX(const CVec& vDirX) {m_vDirX = vDirX;}
	void		SetDirY(const CVec& vDirY) {m_vDirY = vDirY;}
	void		Transform(const CTMat& tm);
//	void		Write(CFile& fl) const;
	void		Write(HFILE hFile) const;

	// mak
	CVec		GetXDir() { return m_vDirX; }
	CVec		GetYDir() { return m_vDirY; }
};
