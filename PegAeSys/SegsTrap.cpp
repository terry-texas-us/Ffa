#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "Undo.h"

CSegsTrap::CSegsTrap()
{
	m_bIdentify = true;
	m_nPenColor = 15;
}

void CSegsTrap::AddSegsAtPt(CSegsDet* pSegs2, CPnt pt)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	
	CPnt	rPt;
	double dPicApertSiz = detsegs.PicApertSiz();

	// Convert pick point to view coordinates
	CPnt4 ptView(pt, 1.);
	ModelView_DoTransform(ptView);

	CPrimPolygon::EdgeToEvaluate() = 0;
	CPrimPolyline::EdgeToEvaluate() = 0; 	

	/*POSITION pos = pSegs->GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = pSegs->GetNext(pos);*/
	
	POSITION pos = detsegs.GetHeadPosition();
	while(pos != 0)
	{
		CSeg* pSeg = detsegs.GetNext(pos);

		if (Find(pSeg) != 0) 				// already in trap
			continue;
			 
		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim != 0)
		{
			
			//for(int i = 0; i < pDoc->LayersGetSize(); i++)
			//{
			//	CLayer *pLayer = pDoc->LayersGetAt(i);
			//	if(pLayer->Find(pSeg) && !pLayer->IsMapped())
			//	{
			//		CPrim* pPrim = pSeg->GetNext(posPrim);
			//		if (pPrim->SelUsingPoint(ptView, dPicApertSiz, rPt)) // Segment within tolerance
			//		{
			//			AddTail(pSeg);
			//			pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE_TRAP, pSeg);
			//			break;							 
			//		}
			//	}
			//}
			
			CPrim* pPrim = pSeg->GetNext(posPrim);
			if (pPrim->SelUsingPoint(ptView, dPicApertSiz, rPt)) // Segment within tolerance
			{
				AddTail(pSeg);
				pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE_TRAP, pSeg);
				break;							 
			}
		}
	}
	app.StatusLineDisplay(TrapCnt);
}

void CSegsTrap::Compress()
{
	// Builds a single segment from two or more segments in trap.
	// The new segment is added to the hot layer even if the trap contained
	// segments from one or more warm layers.

	CPegDoc* pDoc = CPegDoc::GetDoc();

	if (GetCount() <= 1)
		return;
	
	CSeg* pSegNew = new CSeg;
	
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		
		pDoc->AnyLayerRemove(pSeg);
		detsegs.Remove(pSeg);
		pSegNew->AddTail(pSeg);
		// delete the original segment but not its primitives
		delete pSeg;
	}
	// emtpy trap segment list
	RemoveAll();
	pDoc->WorkLayerAddTail(pSegNew);
	AddTail(pSegNew);
	
	pSegNew->SortTextOnY();
}

void CSegsTrap::Copy(const CVec& vTrns)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		CSeg* pNewSeg = new CSeg(*pSeg);
					
		pDoc->WorkLayerAddTail(pNewSeg); 
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
		pSeg->Translate(vTrns);

		LPARAM lHint = (m_bIdentify) ? CPegDoc::HINT_SEG_SAFE_TRAP : CPegDoc::HINT_SEG_SAFE;
		pDoc->UpdateAllViews(NULL, lHint, pSeg);
	}
}

void CSegsTrap::DeleteSegs()
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{	
		CSeg* pSeg = GetNext(pos);
		pDoc->AnyLayerRemove(pSeg);
		detsegs.Remove(pSeg);
		pSeg->RemovePrims();
		delete pSeg;
	}
	RemoveAll();
}

///<summary>Expands compressed segments.</summary>
// The new segments are added to the hot layer even if the trap contained
// segments from one or more warm layers.
void CSegsTrap::Expand()
{ 
	CPegDoc* pDoc = CPegDoc::GetDoc();

	if(IsEmpty())
		return;
	
	CSeg* pSeg;
	CSeg* pNewSeg;
	CPrim* pPrim;

	// need copy of entry trap segment list for iteration
	CSegs* pSegs = new CSegs;
	pSegs->AddTail(this);
	RemoveAll();
		
	POSITION pos = pSegs->GetHeadPosition();
	while (pos != 0)
	{
		pSeg = pSegs->GetNext(pos);
		
		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim != 0)
		{
			pPrim = pSeg->GetNext(posPrim);
			pNewSeg = new CSeg(pPrim);
			pDoc->WorkLayerAddTail(pNewSeg);
			AddTail(pNewSeg);
		}
		pDoc->AnyLayerRemove(pSeg);

		// mak - added error checking to find... seems to stop the crashing
		POSITION posFind = detsegs.Find(pSeg);
		if(posFind != NULL)
			detsegs.RemoveAt(posFind);

		delete pSeg;
	}
	delete pSegs;
}

void CSegsTrap::RemoveSegsAtPt(CPnt pt)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CPnt rPt;
	double dPicApertSiz = detsegs.PicApertSiz();

	int iNmbFnd = 0;
	
	// Convert pick point to view coordinates
	CPnt4 ptView(pt, 1.);
	ModelView_DoTransform(ptView);

	CPrimPolygon::EdgeToEvaluate() = 0;
	CPrimPolyline::EdgeToEvaluate() = 0; 	

	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		
		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim != 0)
		{
			CPrim* pPrim = pSeg->GetNext(posPrim);
			if (pPrim->SelUsingPoint(ptView, dPicApertSiz, rPt)) // Segment within tolerance
			{
				RemoveAt(Find(pSeg));
				iNmbFnd++;
				pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
				break;							 
			}
		}
	}
}

void CSegsTrap::Square()
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEGS_ERASE_SAFE_TRAP, this);
	
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		pSeg->Square();
	}
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEGS_SAFE_TRAP, this);
}

void CSegsTrap::TransformSegs(const CTMat& tm)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	//undo.SaveState();

	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEGS_ERASE_SAFE_TRAP, this);
	Transform(tm);
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEGS_SAFE_TRAP, this);
}

void CSegsTrap::Vectorize()
{
	POSITION pos1 = GetHeadPosition();
	while(pos1 != 0)
	{
		CSeg *pSeg = GetNext(pos1);
		pSeg->Vectorize();
	}
}