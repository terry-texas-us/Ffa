#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "SubProcCut.h"

///<summary>Cuts primatives.</summary>
LRESULT CALLBACK SubProcCut(HWND hwnd, UINT anMsg, WPARAM wParam, LPARAM lParam)
{
	static WORD wPrvKeyDwn = 0;
	static CPnt rPrvPos;

	CPegDoc *pDoc = CPegDoc::GetDoc();

	CPegView* pView = CPegView::GetActiveView();
	CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();
	
	if (anMsg == WM_COMMAND)
	{
		LRESULT lResult = 0;
		CPnt ptCur = app.CursorPosGet();
								
		switch (LOWORD(wParam))
		{
			case ID_OP0:
				break;
				
			case ID_OP1:
				cut::CutPrimsAtPt(ptCur);
				break;

			case ID_OP2:		
				if (wPrvKeyDwn != ID_OP2) 
				{
					rPrvPos = ptCur;
					app.RubberBandingStartAtEnable(ptCur, Lines);
					wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP2);
				}
				else
				{
					cut::CutPrimsByLn(rPrvPos, ptCur);
					app.RubberBandingDisable();
					app.ModeLineUnhighlightOp(wPrvKeyDwn);
				}
				break;
				
			case ID_OP4:

				if (wPrvKeyDwn != ID_OP4) 
				{
					rPrvPos = ptCur;
					app.RubberBandingStartAtEnable(ptCur, Rectangles);
					wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP4);
				}
				else 
				{
					CPnt rLL, rUR;
					
					rLL[0] = Min(rPrvPos[0], ptCur[0]);
					rLL[1] = Min(rPrvPos[1], ptCur[1]);
					rUR[0] = Max(rPrvPos[0], ptCur[0]);
					rUR[1] = Max(rPrvPos[1], ptCur[1]);
					cut::CutSegsByArea(pDC, rLL, rUR);
					app.RubberBandingDisable();
					app.ModeLineUnhighlightOp(wPrvKeyDwn);
				}
				app.StatusLineDisplay();
				break;
				
			case ID_OP7:
				if (wPrvKeyDwn != ID_OP7) 
				{
					rPrvPos = ptCur;
					wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP7);
				}
				else
				{
					cut::CutPrimAt2Pts(pDC, rPrvPos, ptCur);
					app.ModeLineUnhighlightOp(wPrvKeyDwn);
				}
				app.StatusLineDisplay();
				break;
				
			case IDM_RETURN:
				app.RubberBandingDisable();
				app.ModeLineUnhighlightOp(wPrvKeyDwn);
				break;

			case IDM_ESCAPE:
				if (wPrvKeyDwn != 0) 
					app.RubberBandingDisable();
				else
					pDoc->DoEditPrimitiveEscape();

				app.ModeLineUnhighlightOp(wPrvKeyDwn);
				break;
				
			default:
				lResult = !lResult;
		}
		if (lResult == 0) 
			return (lResult);
	}
	return (CallWindowProc(app.GetMainWndProc(), hwnd, anMsg, wParam, lParam));
}

///<summary>Cuts a primative at two pnts and puts non-null middle piece in trap.</summary>
// Notes:	Accuracy of arc section cuts diminishes with high 
//			eccentricities. if two cut points are coincident 
//			nothing happens.
void cut::CutPrimAt2Pts(CDC* pDC, CPnt pt1, CPnt pt2)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	if (pt1 == pt2) {return;}

	double dRel[2];
	CPnt	ptCut[2];
	 
	PENCOLOR nPenColor = pstate.PenColor();
	PENSTYLE nPenStyle = pstate.PenStyle();
		
	CTMat tm = view::GetMatrixInverse();

	CPnt4 ptView[] = {CPnt4(pt1, 1.), CPnt4(pt2, 1.)};
	
	ModelView_DoTransform(2, ptView);
	
	CSegs* pSegsOut = new CSegs;
	CSegs* pSegsIn = new CSegs;
	
	POSITION posSeg;
	POSITION posSegPrv;
	
	double dPicApertSiz = detsegs.PicApertSiz();
	
	for (posSeg = detsegs.GetHeadPosition(); (posSegPrv = posSeg) != 0;)
	{
		CSeg* pSeg = detsegs.GetNext(posSeg);
		
		if (trapsegs.Find(pSeg) != 0)
			continue;
		
		POSITION posPrim1;
		POSITION posPrim2;		
		
		for (posPrim1 = pSeg->GetHeadPosition(); (posPrim2 = posPrim1) != 0;)
		{
			CPrim* pPrim = pSeg->GetNext(posPrim1);
			
			if (!pPrim->SelUsingPoint(ptView[0], dPicApertSiz, ptCut[0]))
				continue;
			dRel[0] = CPrim::Rel();
			if (!pPrim->SelUsingPoint(ptView[1], dPicApertSiz, ptCut[1]))
				continue;
			dRel[1] = CPrim::Rel(); 		
			// Both pick points are within tolerance of primative
			ptCut[0] = tm * ptCut[0];
			ptCut[1] = tm * ptCut[1];
			if (dRel[0] > dRel[1])
			{
				CPnt ptTmp = ptCut[0];
				ptCut[0] = ptCut[1]; ptCut[1] = ptTmp;
			}
			pSeg->RemoveAt(posPrim2);

			pPrim->CutAt2Pts(ptCut, pSegsOut, pSegsIn);
		}
		if (pSeg->IsEmpty())
		{	// seg was emptied remove from lists
			pDoc->AnyLayerRemove(pSeg);
			detsegs.RemoveAt(posSegPrv);
			pSeg->RemovePrims();
			delete pSeg;
		}
	}
	if (pSegsOut->GetCount() > 0)
	{
		pDoc->WorkLayerAddTail(pSegsOut);
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEGS, pSegsOut);
	}
	if (pSegsIn->GetCount() > 0)
	{
		pDoc->WorkLayerAddTail(pSegsIn);
		trapsegs.AddTail(pSegsIn);
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEGS_TRAP, pSegsIn);
	}
	delete pSegsIn;
	delete pSegsOut;
	
	pstate.SetPen(pDC, nPenColor, nPenStyle);
	app.StatusLineDisplay(TrapCnt);
}

///<summary>Cuts all primatives which intersect with line defined by two points.</summary>
// Notes: Colinear fill area edges are not considered to intersect.
void cut::CutPrimsByLn(CPnt pt1, CPnt pt2)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	CSegs* pSegs = new CSegs;

	CPnts ptsInt;

	CPnt4 ptView[] = {CPnt4(pt1, 1.), CPnt4(pt2, 1.)};
	ModelView_DoTransform(2, ptView);
	
	CTMat tm = view::GetMatrixInverse();
	
	POSITION pos = detsegs.GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = detsegs.GetNext(pos);

		if (trapsegs.Find(pSeg) != 0)
			continue;
		
		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim != 0)
		{
			CPrim* pPrim = pSeg->GetNext(posPrim);
			
			ptsInt.SetSize(0);
			pPrim->SelUsingLine(CLine(ptView[0], ptView[1]), ptsInt);
			for (WORD w = 0; w < ptsInt.GetSize(); w++)
			{
				CSeg* pSegNew = new CSeg;
				
				ptsInt[w] = tm * ptsInt[w];
				
				pDoc->UpdateAllViews(NULL, CPegDoc::HINT_PRIM_ERASE_SAFE, pPrim);
				pPrim->CutAtPt(ptsInt[w], pSegNew);
				pDoc->UpdateAllViews(NULL, CPegDoc::HINT_PRIM_SAFE, pPrim);

				if(!pSegNew->IsEmpty())
					pSegs->AddTail(pSegNew); 
			}
		}
	}
	if(!pSegs->IsEmpty())
	{
		pDoc->WorkLayerAddTail(pSegs);
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEGS_SAFE, pSegs);
	}
	delete pSegs;
}

///<summary>Cuts a primative at a point.</summary>
void cut::CutPrimsAtPt(CPnt pt)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CSegs* pSegs = new CSegs;

	CPnt ptCut;
	
	CTMat tm = view::GetMatrixInverse();
	
	CPnt4 ptView(pt, 1.);
	ModelView_DoTransform(ptView);
	
	double dPicApertSiz = detsegs.PicApertSiz();

	POSITION pos = detsegs.GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = detsegs.GetNext(pos);
		
		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim != 0)
		{
			CPrim* pPrim = pSeg->GetNext(posPrim);
			
			if (pPrim->SelUsingPoint(ptView, dPicApertSiz, ptCut)) 
			{	// Pick point is within tolerance of primative
				CSeg* pSegNew = new CSeg;

				ptCut = tm * ptCut;
				pDoc->UpdateAllViews(NULL, CPegDoc::HINT_PRIM_ERASE_SAFE, pPrim);
				pPrim->CutAtPt(ptCut, pSegNew);
				pDoc->UpdateAllViews(NULL, CPegDoc::HINT_PRIM_SAFE, pPrim);

				if(!pSegNew->IsEmpty())
					pSegs->AddTail(pSegNew); 
				break;											
			}
		}
	}
	if(!pSegs->IsEmpty())
	{
		pDoc->WorkLayerAddTail(pSegs);
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEGS_SAFE, pSegs);
	}
	delete pSegs;
}

// This new function keeps the segments that are cut in their respective layers.
// The previous function, commented below, would put both new and old (in and out) segments
// in the working layer.
void cut::CutSegsByArea(CDC* pDC, CPnt ptLL, CPnt ptUR)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CSeg*		pSeg;
	CPrim*		pPrim;

	int 	iInts;
	CPnt	ptInt[10];

	PENCOLOR nPenColor = pstate.PenColor();
	PENSTYLE nPenStyle = pstate.PenStyle();

	CSegs* pSegsOut = new CSegs;
	CSegs* pSegsIn = new CSegs;

	POSITION posSeg, posSegPrv;
	for(posSeg = detsegs.GetHeadPosition(); (posSegPrv = posSeg) != 0;)
	{
		pSeg = detsegs.GetNext(posSeg);
		
		// ignore if already trapped
		if (trapsegs.Find(pSeg) != 0)
			continue;
		
		POSITION posPrim, posPrimPrv;
		for (posPrim = pSeg->GetHeadPosition(); (posPrimPrv = posPrim) != 0;)
		{
			pPrim = pSeg->GetNext(posPrim);

			if ((iInts = pPrim->IsWithinArea(ptLL, ptUR, ptInt)) == 0)
			{
//				MessageBox(0, "zero", 0, 0);
				continue;
			}

//			CString buffer;
//			itoa(iInts, buffer.GetBuffer(), 10);
//			MessageBox(0, buffer, 0, 0);

			pSegsOut = new CSegs;
			pSegsIn = new CSegs;

			CString strLayerName = pDoc->LayerFind(pSeg);
			CLayer *pLayerFind = pDoc->LayersGet(strLayerName);
			if(pLayerFind == 0)
				pLayerFind = pDoc->WorkLayerGet();

			pSeg->RemoveAt(posPrimPrv);
			
			for(int i = 0; i < iInts; i += 2)
			{
				if(i != 0)
					pSegsOut->RemoveTail();
				pPrim->CutAt2Pts(&ptInt[i], pSegsOut, pSegsIn);
			}			

			if (pSegsOut->GetCount() > 0)
			{
				//pDoc->WorkLayerAddTail(pSegsOut);
				pLayerFind->AddTail(pSegsOut);
				pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEGS, pSegsOut);
			}
			if (pSegsIn->GetCount() > 0)
			{
				//pDoc->WorkLayerAddTail(pSegsIn);
				pLayerFind->AddTail(pSegsIn);
				trapsegs.AddTail(pSegsIn);
				pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEGS_SAFE_TRAP, pSegsIn);
			}

		}
		if (pSeg->IsEmpty())
		{	// seg was emptied remove from lists
			pDoc->AnyLayerRemove(pSeg);
			detsegs.RemoveAt(posSegPrv);
			pSeg->RemovePrims();
			delete pSeg;
		}
	}
				
	
	delete pSegsIn;
	delete pSegsOut;
	
	pstate.SetPen(pDC, nPenColor, nPenStyle);
	app.StatusLineDisplay(TrapCnt);
}

//void cut::CutSegsByArea(CDC* pDC, CPnt ptLL, CPnt ptUR)
//{
//	CPegDoc* pDoc = CPegDoc::GetDoc();
//
//	CSeg*		pSeg;
//	CPrim*		pPrim;
//
//	int 	iInts;
//	CPnt	ptInt[10];
//
//	PENCOLOR nPenColor = pstate.PenColor();
//	PENSTYLE nPenStyle = pstate.PenStyle();
//
//	CSegs* pSegsOut = new CSegs;
//	CSegs* pSegsIn = new CSegs;
//
//	POSITION posSeg, posSegPrv;
//	for(posSeg = detsegs.GetHeadPosition(); (posSegPrv = posSeg) != 0;)
//	{
//		pSeg = detsegs.GetNext(posSeg);
//		
//		// ignore if already trapped
//		if (trapsegs.Find(pSeg) != 0)
//			continue;
//		
//		POSITION posPrim, posPrimPrv;
//		for (posPrim = pSeg->GetHeadPosition(); (posPrimPrv = posPrim) != 0;)
//		{
//			pPrim = pSeg->GetNext(posPrim);
//
//			if ((iInts = pPrim->IsWithinArea(ptLL, ptUR, ptInt)) == 0)
//			{
////				MessageBox(0, "zero", 0, 0);
//				continue;
//			}
//
////			CString buffer;
////			itoa(iInts, buffer.GetBuffer(), 10);
////			MessageBox(0, buffer, 0, 0);
//
//			pSeg->RemoveAt(posPrimPrv);
//			
//			for(int i = 0; i < iInts; i += 2)
//			{
//				if(i != 0)
//					pSegsOut->RemoveTail();
//				pPrim->CutAt2Pts(&ptInt[i], pSegsOut, pSegsIn);
//			}			 
//		}
//		if (pSeg->IsEmpty())
//		{	// seg was emptied remove from lists
//			pDoc->AnyLayerRemove(pSeg);
//			detsegs.RemoveAt(posSegPrv);
//			pSeg->RemovePrims();
//			delete pSeg;
//		}
//	}
//				
//	if (pSegsOut->GetCount() > 0)
//	{
//		pDoc->WorkLayerAddTail(pSegsOut);
//		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEGS, pSegsOut);
//	}
//	if (pSegsIn->GetCount() > 0)
//	{
//		pDoc->WorkLayerAddTail(pSegsIn);
//		trapsegs.AddTail(pSegsIn);
//		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEGS_SAFE_TRAP, pSegsIn);
//	}
//	delete pSegsIn;
//	delete pSegsOut;
//	
//	pstate.SetPen(pDC, nPenColor, nPenStyle);
//	app.StatusLineDisplay(TrapCnt);
//}