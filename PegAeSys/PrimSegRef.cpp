#include "stdafx.h"

#include "PegAEsysDoc.h"

#include "DlgProcEditOps.h"

#include "subprocnodal.h"

CPrimSegRef::CPrimSegRef()
{
	m_pt = ORIGIN;
	m_vZ = ZDIR;
	
	m_vScale(1., 1., 1.);
	m_dRotation = 0.;
	
	m_wColCnt = 1;
	m_wRowCnt = 1;
	m_dColSpac = 0.;
	m_dRowSpac = 0.;
}	

CPrimSegRef::CPrimSegRef(const CString& strName, const CPnt& pt)
{
	m_strName = strName;
	m_pt = pt;
	m_vZ = ZDIR;
	m_vScale(1., 1., 1.);
	m_dRotation = 0.;
	m_wColCnt = 1;
	m_wRowCnt = 1;
	m_dColSpac = 0.;
	m_dRowSpac = 0.;
}

CPrimSegRef::CPrimSegRef(const CPrimSegRef& src)
{
	m_strName = src.m_strName;
	m_pt = src.m_pt;
	m_vZ = src.m_vZ;
	m_vScale = src.m_vScale;
	m_dRotation = src.m_dRotation;
	m_wColCnt = src.m_wColCnt;
	m_wRowCnt = src.m_wRowCnt;
	m_dColSpac = src.m_dColSpac;
	m_dRowSpac = src.m_dRowSpac;
}

const CPrimSegRef& CPrimSegRef::operator=(const CPrimSegRef& src)
{
	m_strName = src.m_strName;
	m_pt = src.m_pt;
	m_vZ = src.m_vZ;
	m_vScale = src.m_vScale;
	m_dRotation = src.m_dRotation;
	m_wColCnt = src.m_wColCnt;
	m_wRowCnt = src.m_wRowCnt;
	m_dColSpac = src.m_dColSpac;
	m_dRowSpac = src.m_dRowSpac;

	return (*this);
}

void CPrimSegRef::ModifyState()
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	CBlock* pBlock;
	if (pDoc->BlksLookup(m_strName, pBlock) == 0)
		return;

	POSITION pos = pBlock->GetHeadPosition();
	while (pos != 0)
	{
		CPrim *pPrim = pBlock->GetNext(pos);
		pPrim->ModifyState();
	}
}

void CPrimSegRef::GetState()
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	CBlock* pBlock;
	if (pDoc->BlksLookup(m_strName, pBlock) == 0)
		return;

	POSITION pos = pBlock->GetHeadPosition();
	while (pos != 0)
	{
		CPrim *pPrim = pBlock->GetNext(pos);
		pPrim->GetState();
	}
}

bool CPrimSegRef::SelUsingLine(const CLine& ln, CPnts& ptsInt) 
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	bool bResult = false;

	CBlock* pBlock;

	if(pDoc->BlksLookup(m_strName, pBlock) == 0) 
		return (bResult);

	CPnt ptBase = pBlock->GetBasePt();
	
	CTMat tm = BuildTransformMatrix(ptBase);

	mspace.InvokeNew();
	mspace.SetLocalTM(tm);
	
	POSITION posPrim = pBlock->GetHeadPosition();
	while (posPrim != 0)
	{
		if ((pBlock->GetNext(posPrim))->SelUsingLine(ln, ptsInt))
			{bResult = true; break;}
	}
	mspace.Return();
	return (bResult);
}

CTMat CPrimSegRef::BuildTransformMatrix(const CPnt& ptBase) const
{
	CTMat tm1; tm1.Translate(ORIGIN - ptBase);
	CTMat tm2; tm2.Scale(m_vScale);
	CTMat tm3; tm3.RotateZ(sin(m_dRotation), cos(m_dRotation));
	CTMat tm4(ORIGIN, m_vZ);
	CTMat tm5; tm5.Translate(m_pt - ORIGIN);
	
	return (tm1 * tm2 * tm3 * tm4 * tm5);
//	return (tm1 * tm3 * tm4 * tm5);
//	return (tm1 * tm2 * tm4 * tm5);
}

CPrim*& CPrimSegRef::Copy(CPrim*& pPrim) const
{
	pPrim = new CPrimSegRef(*this);
	return (pPrim);
}

void CPrimSegRef::Display(CDC* pDC, bool bNodeDisplay)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CBlock* pBlock;
	if (pDoc->BlksLookup(m_strName, pBlock) == 0)
		return;

	CPnt ptBase = pBlock->GetBasePt();
	CTMat tm = BuildTransformMatrix(ptBase);
		
	mspace.InvokeNew();
	mspace.SetLocalTM(tm);

	pBlock->Display(pDC, false);
	
	mspace.Return();

	if(bNodeDisplay)
	{
		CPnts pts;
		GetAllPts(pts);

		if((app.m_iModeId == ID_MODE_NODAL) || (app.m_iModeId == ID_MODE_NODALR))
		{
			CPrim *pPrim = (CPrim*)this;
			app.DisplayPoint(pDC, &pts, nodal::GetPrimMask(pPrim));
		}
		else
			app.DisplayPoint(pDC, &pts, 0);
	}
}

void CPrimSegRef::DisplayOpenGL(bool)
{

}

void CPrimSegRef::DisRep(const CPnt&) const 
{
	CString str;
	str.Format("<SegRef> Color: %s Style: %s SegmentName %s", 
		FormatPenColor(), FormatPenStyle(), m_strName);
	msgInformation(str);
}

void CPrimSegRef::FormatExtra(CString& str) const
{
	str.Format("Color;%s\tStyle;%s\tSegment Name;%s\tRotation Angle;%f",
		FormatPenColor(), FormatPenStyle(), m_strName, m_dRotation);
}

void CPrimSegRef::FormatGeometry(CString& str) const
{
	str += "Insertion Point;" + m_pt.ToString();
	str += "Normal;" + m_vZ.ToString();
	str += "Scale;" + m_vScale.ToString();
}

CPnt CPrimSegRef::GetCtrlPt() const 
{
	CPnt pt;
	pt = m_pt;
	return (pt);
}

void CPrimSegRef::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
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

void CPrimSegRef::Iron()
{
	m_pt[2] = 0;

	CPegDoc *pDoc = CPegDoc::GetDoc();

	CBlock* pBlock;
	if (pDoc->BlksLookup(m_strName, pBlock) == 0)
		return;

	CPnt pntBase = pBlock->GetBasePt();
	pntBase[2] = 0;
	pBlock->SetBasePt(pntBase);

	POSITION pos = pBlock->GetHeadPosition();
	while (pos != 0)
	{
		CPrim *pPrim = pBlock->GetNext(pos);
		pPrim->Iron();
	}
}

bool CPrimSegRef::IsInView() const
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

CPnt CPrimSegRef::SelAtCtrlPt(const CPnt4& ptPic) const 
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

bool CPrimSegRef::SelUsingRect(const CPnt& pt1, const CPnt& pt2)
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
bool CPrimSegRef::SelUsingPoint(const CPnt4& pt, double dtol, CPnt& ptProj)
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

void CPrimSegRef::ReadPeg(HFILE hFile)
{
	CPrim::ReadPeg(hFile);
	
	FilePeg_ReadString2(hFile, m_strName);
	m_pt.Read(hFile);
	m_vZ.Read(hFile);
	m_vScale.Read(hFile);
	FilePeg_ReadDouble2(hFile, m_dRotation);
	FilePeg_ReadWord2(hFile, m_wColCnt);
	FilePeg_ReadWord2(hFile, m_wRowCnt);
	FilePeg_ReadDouble2(hFile, m_dColSpac);
	FilePeg_ReadDouble2(hFile, m_dRowSpac);
}

void CPrimSegRef::Transform(const CTMat& tm)
{
	m_pt = tm * m_pt;
	m_vZ = tm * m_vZ;

//	if (fabs(m_vZ[0]) <= FLT_EPSILON && fabs(m_vZ[1]) <= FLT_EPSILON)


		m_vScale = tm * m_vScale;

	/////////////////////////////////////////////////////////////////////////


//	//////////////////////////////////////////////////////////////////////////
////	m_pt[2] = 0;
//
//	CPegDoc *pDoc = CPegDoc::GetDoc();
//
//	CBlock* pBlock;
//	if (pDoc->BlksLookup(m_strName, pBlock) == 0)
//		return;
//
//	CPnt pntBase = pBlock->GetBasePt();
////	pntBase[2] = 0;
////	pBlock->SetBasePt(pntBase);
//
//	CPnt cursor;// = new CPnt;// = app.CursorPosGet();
//
////	cursor[0] = 0 - app.CursorPosGet()[0];//cursor[0];
////	cursor[1] = 0 - app.CursorPosGet()[1];//cursor[1];
////	cursor[2] = 0 - app.CursorPosGet()[2];//cursor[2];
//
////	m_pt[0] -= pntBase[0];
////	m_pt[1] -= pntBase[1];
////	m_pt[2] -= pntBase[2];
//
//	CVec tempv;
//	tempv[0] = m_pt[0] + cursor[0];
//	tempv[1] = m_pt[1] + cursor[1];
//	tempv[2] = m_pt[2] + cursor[2];
//	tempv[3] = 0;
//
////	tm.Translate(tempv);
//
////	tm[0][3] += tempv[0]; 
////	tm[1][3] += tempv[1]; 
////	tm[2][3] += tempv[2];
//
//	CTMat tm2 = tm;
//
//	CPnt trans;
//	trans[0] = m_pt[0] + cursor[0];
//	trans[1] = m_pt[1] + cursor[1];
//	trans[2] = m_pt[2] + cursor[2];
//
//	tm2.Translate((CPnt)trans);
//
////	tm2.Translate(pntBase - app.CursorPosGet());//trapsegs.PvtPt());
////	tm2 *= dlgproceditops::GetRotTrnMat();
////	tm2.Translate(pntBase + app.CursorPosGet());
////	trapsegs.TransformSegs(tm);
//
//	POSITION pos = pBlock->GetHeadPosition();
//	while (pos != 0)
//	{
//		CPrim *pPrim = pBlock->GetNext(pos);
//		pPrim->Transform(tm2);
//	}
//
////	m_pt[0] += pntBase[0];
////	m_pt[1] += pntBase[1];
////	m_pt[2] += pntBase[2];
//
////	m_pt[0] -= cursor[0];
////	m_pt[1] -= cursor[1];
////	m_pt[2] -= cursor[2];
}

void CPrimSegRef::TranslateUsingMask(const CVec& v, DWORD dwMask)
{
	if (dwMask != 0)
		m_pt += v;
}

bool CPrimSegRef::Vectorize(const CSeg *)
{
	return false;
}

bool CPrimSegRef::WritePeg(HFILE hFile) const
{
	FilePeg_WriteWord2(hFile, PRIM_SEGREF);
	FilePeg_WriteWord2(hFile, m_nPenColor);
	FilePeg_WriteWord2(hFile, m_nPenStyle);
	FilePeg_WriteString2(hFile, m_strName); 
	m_pt.Write(hFile);
	m_vZ.Write(hFile);
	m_vScale.Write(hFile);
	FilePeg_WriteDouble2(hFile, m_dRotation);
	FilePeg_WriteWord2(hFile, m_wColCnt);
	FilePeg_WriteWord2(hFile, m_wRowCnt);
	FilePeg_WriteDouble2(hFile, m_dColSpac);
	FilePeg_WriteDouble2(hFile, m_dRowSpac);

	return true;
}

void CPrimSegRef::SetPenColor(PENCOLOR newCol)
{
	m_nPenColor = newCol;

	CPegDoc *pDoc = CPegDoc::GetDoc();

	CBlock* pBlock;
	if (pDoc->BlksLookup(m_strName, pBlock) == 0)
		return;

	POSITION pos = pBlock->GetHeadPosition();
	while (pos != 0)
	{
		CPrim *pPrim = pBlock->GetNext(pos);
		pPrim->SetPenColor(newCol);
	}
}

void CPrimSegRef::SetPenStyle(PENSTYLE newSty)
{
	m_nPenStyle = newSty;

	CPegDoc *pDoc = CPegDoc::GetDoc();

	CBlock* pBlock;
	if (pDoc->BlksLookup(m_strName, pBlock) == 0)
		return;

	POSITION pos = pBlock->GetHeadPosition();
	while (pos != 0)
	{
		CPrim *pPrim = pBlock->GetNext(pos);
		pPrim->SetPenStyle(newSty);
	}
}