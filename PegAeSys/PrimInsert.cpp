#include "stdafx.h"

#include "PegAEsysDoc.h"

#include "subprocnodal.h"

CPrimInsert::CPrimInsert()
{
	m_pt = ORIGIN;
	m_vX = XDIR;
	m_vY = YDIR;
	m_vZ = ZDIR;
	m_wColCnt = 1;
	m_wRowCnt = 1;
	m_dColSpac = 0.;
	m_dRowSpac = 0.;
}

CPrimInsert::CPrimInsert(const CString& strName, const CPnt& pt)
{
	m_strName = strName;
	m_pt = pt;
	m_vX = XDIR;
	m_vY = YDIR;
	m_vZ = ZDIR;
	m_wColCnt = 1;
	m_wRowCnt = 1;
	m_dColSpac = 0.;
	m_dRowSpac = 0.;
}

CPrimInsert::CPrimInsert(const CPrimInsert& src)
{
	m_strName = src.m_strName;
	m_pt = src.m_pt;
	m_vX = src.m_vX;
	m_vY = src.m_vY;
	m_vZ = src.m_vZ;
	m_wColCnt = src.m_wColCnt;
	m_wRowCnt = src.m_wRowCnt;
	m_dColSpac = src.m_dColSpac;
	m_dRowSpac = src.m_dRowSpac;
}

const CPrimInsert& CPrimInsert::operator=(const CPrimInsert& src)
{
	m_strName = src.m_strName;
	m_pt = src.m_pt;
	m_vX = src.m_vX;
	m_vY = src.m_vY;
	m_vZ = src.m_vZ;
	m_wColCnt = src.m_wColCnt;
	m_wRowCnt = src.m_wRowCnt;
	m_dColSpac = src.m_dColSpac;
	m_dRowSpac = src.m_dRowSpac;

	return (*this);
}

CTMat CPrimInsert::BuildTransformMatrix(const CPnt& ptBase) const
{
	CTMat tm(m_pt, m_vX, m_vY);
	tm.Translate(ptBase);
	tm.Inverse();
	return tm;
}

CPrim*& CPrimInsert::Copy(CPrim*& pPrim) const
{
	pPrim = new CPrimInsert(*this);
	return (pPrim);
}

void CPrimInsert::Display(CDC* , bool)//pDC, bool bNodeDisplay)
{
//	CPegDoc* pDoc = CPegDoc::GetDoc();
//
//	CBlock* pBlock;
//	if (pDoc->BlksLookup(m_strName, pBlock) == 0) 
//		return;
//
//	CPnt ptBase = pBlock->GetBasePt();
//	CTMat tm = BuildTransformMatrix(ptBase);
//	
//	mspace.InvokeNew();
//	mspace.SetLocalTM(tm);
//			
//	pBlock->Display(pDC, false);
//	
//	mspace.Return();
//
}

void CPrimInsert::DisplayOpenGL(bool)
{

}

void CPrimInsert::DisRep(const CPnt&) const 
{
	CString str;
	str.Format("<Insert> Color: %s Style: %s", FormatPenColor(), FormatPenStyle());
	msgInformation(str);
}

void CPrimInsert::FormatExtra(CString& str) const
{
	str.Format("Color;%s\tStyle;%s", FormatPenColor(), FormatPenStyle());
}

void CPrimInsert::FormatGeometry(CString& str) const
{
	str += "Insertion Point;" + m_pt.ToString();
	str += "X Axis;" + m_vX.ToString();
	str += "Y Axis;" + m_vY.ToString();
	str += "Z Axis;" + m_vZ.ToString();
}

CPnt CPrimInsert::GetCtrlPt() const 
{
	CPnt pt;
	pt = m_pt;
	return (pt);
}

void CPrimInsert::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CBlock* pBlock;
	
	if (pDoc->BlksLookup(m_strName, pBlock) == 0) {return;}

	CPnt ptBase = pBlock->GetBasePt();

	CTMat tmIns = BuildTransformMatrix(ptBase);
	
	mspace.InvokeNew();
	mspace.SetLocalTM(tmIns);

	pBlock->GetExtents(ptMin, ptMax, tm);

	mspace.Return();
}

bool CPrimInsert::IsInView() const
{
	// Test whether an instance of a block is wholly or partially within the current view volume.
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CBlock* pBlock;

	if (pDoc->BlksLookup(m_strName, pBlock) == 0) {return false;}

	CPnt ptBase = pBlock->GetBasePt();

	CTMat tm = BuildTransformMatrix(ptBase);
	
	mspace.InvokeNew();
	mspace.SetLocalTM(tm);
	
	bool bInView = pBlock->IsInView();
	
	mspace.Return();
	return (bInView);
}

CPnt CPrimInsert::SelAtCtrlPt(const CPnt4& ptPic) const 
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	mS_wCtrlPt = USHRT_MAX;
	CPnt ptCtrl;

	CBlock* pBlock;

	if (pDoc->BlksLookup(m_strName, pBlock) == 0) {return ptCtrl;}

	CPnt ptBase = pBlock->GetBasePt();
	
	CTMat tm = BuildTransformMatrix(ptBase);
	
	mspace.InvokeNew();
	mspace.SetLocalTM(tm);
	
	POSITION pos = pBlock->GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = pBlock->GetNext(pos);
		ptCtrl = pPrim->SelAtCtrlPt(ptPic);
		if (mS_wCtrlPt != USHRT_MAX)
		{
			mspace.Transform(ptCtrl);	
			break;
		}
	}
	mspace.Return();
	return ptCtrl;
}

bool CPrimInsert::SelUsingRect(const CPnt& pt1, const CPnt& pt2)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CBlock* pBlock;

	if (pDoc->BlksLookup(m_strName, pBlock) == 0) {return false;}

	CPnt ptBase = pBlock->GetBasePt();
	
	CTMat tm = BuildTransformMatrix(ptBase);
	
	mspace.InvokeNew();
	mspace.SetLocalTM(tm);
	
	bool bResult = pBlock->SelUsingRect(pt1, pt2);
	
	mspace.Return();
	return (bResult);
}

///<summary>Evaluates whether a point lies within tolerance specified of block.</summary>
bool CPrimInsert::SelUsingPoint(const CPnt4& pt, double dtol, CPnt& ptProj)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	bool bResult = false;

	CBlock* pBlock;

	if (pDoc->BlksLookup(m_strName, pBlock) == 0) {return (bResult);}
 
	CPnt ptBase = pBlock->GetBasePt();
	
	CTMat tm = BuildTransformMatrix(ptBase);
	
	mspace.InvokeNew();
	mspace.SetLocalTM(tm);
	
	POSITION posPrim = pBlock->GetHeadPosition();
	while (posPrim != 0)
	{
		if ((pBlock->GetNext(posPrim))->SelUsingPoint(pt, dtol, ptProj))
			{bResult = true; break;}
	}
	mspace.Return();
	return (bResult);
}	 

void CPrimInsert::ReadPeg(HFILE hFile)
{
	CPrim::ReadPeg(hFile);
	
	FilePeg_ReadString2(hFile, m_strName);
    m_pt.Read(hFile);
	m_vX.Read(hFile);
	m_vY.Read(hFile);
	m_vZ.Read(hFile);
	FilePeg_ReadWord2(hFile, m_wColCnt);
	FilePeg_ReadWord2(hFile, m_wRowCnt);
	FilePeg_ReadDouble2(hFile, m_dColSpac);
	FilePeg_ReadDouble2(hFile, m_dRowSpac);
}

void CPrimInsert::Transform(const CTMat& tm)
{
	m_pt = tm * m_pt;
	m_vX = tm * m_vX;
	m_vY = tm * m_vY;
	m_vZ = tm * m_vZ;
}

void CPrimInsert::TranslateUsingMask(const CVec& v, DWORD dwMask)
{
	if (dwMask != 0)
		m_pt += v;
}

bool CPrimInsert::Vectorize(const CSeg *)
{
	return false;
}

bool CPrimInsert::WritePeg(HFILE hFile) const
{
	FilePeg_WriteWord2(hFile, PRIM_INSERT);
	FilePeg_WriteWord2(hFile, m_nPenColor);
	FilePeg_WriteWord2(hFile, m_nPenStyle);
	FilePeg_WriteString2(hFile, m_strName); 
	m_pt.Write(hFile);
	m_vX.Write(hFile);
	m_vY.Write(hFile);
	m_vZ.Write(hFile);
	FilePeg_WriteWord2(hFile, m_wColCnt);
	FilePeg_WriteWord2(hFile, m_wRowCnt);
	FilePeg_WriteDouble2(hFile, m_dColSpac);
	FilePeg_WriteDouble2(hFile, m_dRowSpac);

	return true;
}