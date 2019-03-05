#include "stdafx.h"
		
const CSegs& CSegs::operator=(const CSegs& src)
{
	RemoveAll();

	POSITION pos = src.GetHeadPosition();
	while(pos != 0)
	{
		CSeg *pSeg = src.GetNext(pos);
		AddTail(pSeg);
	}

	return (*this);
}

void CSegs::BreakPolylines()
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		pSeg->BreakPolylines();
	}
}

void CSegs::BreakSegRefs()
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		pSeg->BreakSegRefs();
	}
}

void CSegs::Display(CDC* pDC) const
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		pSeg->Display(pDC, true);
	}
}

POSITION CSegs::Remove(CSeg* pSeg)
{
	POSITION pos = Find(pSeg);
	if (pos != 0)
		RemoveAt(pos);
	
	return (pos);
}

int CSegs::GetBlockRefCount(const CString& strBlkNam) const
{
	int iCount = 0;
	
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		iCount += pSeg->GetBlockRefCount(strBlkNam);
	}
	return (iCount);
}

///<summary>Determines the extent of all segments in list.</summary>
void CSegs::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
{	
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		pSeg->GetExtents(ptMin, ptMax, tm);
	}
}

int CSegs::GetPenStyleRefCount(PENSTYLE nPenStyle) const
{
	int iCount = 0;
	
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		iCount += pSeg->GetPenStyleRefCount(nPenStyle);
	}
	return (iCount);
}

void CSegs::ModifyMarkers()
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		pSeg->ModifyMarkers();
	}
}

void CSegs::ModifyPenColor(PENCOLOR nPenColor)
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		pSeg->ModifyPenColor(nPenColor);
	}
}

void CSegs::ModifyPenStyle(PENSTYLE nStyle)
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
		(GetNext(pos))->ModifyPenStyle(nStyle);
}

void CSegs::ModifyNotes(const CFontDef& fd, const CCharCellDef& ccd, int iAtt)
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		pSeg->ModifyNotes(fd, ccd, iAtt);
	}
}

void CSegs::PenTranslation(WORD wCols, PENCOLOR* pColNew, PENCOLOR* pCol)
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
		(GetNext(pos))->PenTranslation(wCols, pColNew, pCol);
}

void CSegs::PenStyleTranslation(const PENSTYLE& nStyleNew, const PENSTYLE& nStyleOld)
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
		(GetNext(pos))->PenStyleTranslation(nStyleNew, nStyleOld);
}

int CSegs::RemoveEmptyNotes()
{
	int iCount = 0;
	
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		iCount += pSeg->RemoveEmptyNotes();
	}
	return (iCount);
}

int CSegs::RemoveEmptySegs()
{
	int iCount = 0;
	
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		POSITION posPrev = pos;
		CSeg* pSeg = GetNext(pos);

		// remove from trap if trapped
		if(trapsegs.Find(pSeg) != 0)
			trapsegs.Remove(pSeg);

		// remove from file
		if (pSeg->GetCount() == 0)
		{
			RemoveAt(posPrev);
			delete pSeg;
			iCount++;
		}
	}
	return (iCount);
}

void CSegs::RemoveSegs()
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		pSeg->RemovePrims();
		delete pSeg;
	}
	RemoveAll();
}

CSeg* CSegs::SelSegUsingPoint(const CPnt& pt)
{
	CPnt ptEng;
	
	CSeg* pPicSeg = 0;

	CPnt4 ptView(pt, 1.);
	ModelView_DoTransform(ptView);	
	
	CTMat tm = view::GetMatrixInverse();
	
	double dPicApert = detsegs.PicApertSiz(); 
	
	CPrimPolygon::EdgeToEvaluate() = 0; 	
	CPrimPolyline::EdgeToEvaluate() = 0; 	

	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		if (pSeg->SelPrimUsingPoint(ptView, dPicApert, ptEng) != 0)
			pPicSeg = pSeg;
	}
	return (pPicSeg);		
}

void CSegs::SetBitmapActLikePrim(bool bActLikePrim)
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg *pSeg = GetNext(pos);
		pSeg->SetBitmapActLikePrim(bActLikePrim);
	}

}

void CSegs::Transform(const CTMat& tm)
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		pSeg->Transform(tm);
	}
}

void CSegs::Translate(const CVec& v) const
{
	if(v.IsNull())
		return;

	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		pSeg->Translate(v);
	}
}

void CSegs::Write(CFile& f, char* p)
{
	POSITION pos = GetHeadPosition();
	while (pos != 0) 		
		GetNext(pos)->Write(f, p);
}

void CSegs::WritePeg(CFile& f, char* p)
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		//GetNext(pos)->WritePeg((HFILE)f.m_hFile);
		GetNext(pos)->WriteJob((HFILE)f.m_hFile, p);
	}
}