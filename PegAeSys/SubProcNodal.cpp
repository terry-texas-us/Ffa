#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "Hatch.h"
#include "SubProcNodal.h"
#include "UserAxis.h"

namespace nodal
{
	bool		bAdd;
	CObList*	UPL = 0;
	CSegs*		SegList = 0;
	CObList*	PrimLis;
}

LRESULT CALLBACK SubProcNodal(HWND hwnd, UINT anMsg, WPARAM wParam, LPARAM lParam)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	static	WORD wPrvKeyDwn;
	static CPnt rPrvPos;
	
	if (anMsg == WM_COMMAND)
	{
		long lResult = 0;
		CPnt rCurPos = app.CursorPosGet();
		switch (LOWORD(wParam))
		{
			case ID_OP0:
				nodal::bAdd = !nodal::bAdd;
				if (nodal::bAdd)
					app.SetWindowMode(ID_MODE_NODAL);
				else
					app.SetWindowMode(ID_MODE_NODALR);
				break;
				
			case ID_OP1:
				nodal::AddByPt(rCurPos);
				CPegView::GetActiveView()->InvalidateRect(0, TRUE);
				break;
						 
			case ID_OP2:
				nodal::AddPrimByPt(rCurPos);
				CPegView::GetActiveView()->InvalidateRect(0, TRUE);
				break;
				
			case ID_OP3:
				if (wPrvKeyDwn != ID_OP3) 
				{
					rPrvPos = rCurPos;
					app.RubberBandingStartAtEnable(rCurPos, Rectangles);
					wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP3);
				}
				else
				{
					app.RubberBandingDisable();
					if (rPrvPos != rCurPos)
					{
						nodal::AddByArea(rPrvPos, rCurPos);
						app.ModeLineUnhighlightOp(wPrvKeyDwn);
						CPegView::GetActiveView()->InvalidateRect(0, TRUE);
					}
				}
				break;
				
			case ID_OP4:
				if (wPrvKeyDwn != ID_OP4) 
				{
					rPrvPos = rCurPos;
					app.RubberBandingStartAtEnable(rCurPos, Lines);
					wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP4);
				}
				else
				{
					app.RubberBandingDisable();
					if (rPrvPos != rCurPos) 
					{
						rCurPos = UserAxis::UserAxisSnapLn(rPrvPos, rCurPos);
						nodal::Translate(CVec(rPrvPos, rCurPos));
						app.CursorPosSet(rCurPos);
						CPegView::GetActiveView()->InvalidateRect(0, TRUE);
					}
					app.ModeLineUnhighlightOp(wPrvKeyDwn);
				}
				break;
				
			case ID_OP5:
				if (wPrvKeyDwn != ID_OP5) 
				{
					rPrvPos = rCurPos;
					app.RubberBandingStartAtEnable(rCurPos, Lines);
					wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP5);
				}
				else
				{
					app.RubberBandingDisable();
					if (rPrvPos != rCurPos) 
					{
						rCurPos = UserAxis::UserAxisSnapLn(rPrvPos, rCurPos);
						nodal::CopyPrims(CVec(rPrvPos, rCurPos));
						app.CursorPosSet(rCurPos);
						CPegView::GetActiveView()->InvalidateRect(0, TRUE);
					}
					app.ModeLineUnhighlightOp(wPrvKeyDwn);
				}
				break;
					
			case ID_OP6:
				if (wPrvKeyDwn != ID_OP6) 
				{
					rPrvPos = rCurPos;
					app.RubberBandingStartAtEnable(rCurPos, Lines);
					wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP6);
				}
				else
				{
					app.RubberBandingDisable();
					if (rPrvPos != rCurPos) 
					{
						rCurPos = UserAxis::UserAxisSnapLn(rPrvPos, rCurPos);
						nodal::GenVecs(CVec(rPrvPos, rCurPos));
						app.CursorPosSet(rCurPos);
						CPegView::GetActiveView()->InvalidateRect(0, TRUE);
					}
					app.ModeLineUnhighlightOp(wPrvKeyDwn);
				}
				break;
				
			case ID_OP7:
				if (wPrvKeyDwn != ID_OP7) 
				{
					rPrvPos = rCurPos;
					app.RubberBandingStartAtEnable(rCurPos, Lines);
					wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP7);
				}
				else
				{
					app.RubberBandingDisable();
					if (rPrvPos != rCurPos) 
					{
						rCurPos = UserAxis::UserAxisSnapLn(rPrvPos, rCurPos);
						nodal::GenFilAreas(CVec(rPrvPos, rCurPos));
						app.CursorPosSet(rCurPos);
						CPegView::GetActiveView()->InvalidateRect(0, TRUE);
						app.ModeLineUnhighlightOp(wPrvKeyDwn);
					}
				}
				break;

			case ID_OP8:
				nodal::SegList = new CSegs;
				nodal::UPL = new CObList;
				nodal::PrimLis = new CObList;
				CPegView::GetActiveView()->InvalidateRect(0, TRUE);
				break;
				
			case ID_OP9:
				nodal::AddEngPrim();
				CPegView::GetActiveView()->InvalidateRect(0, TRUE);
				break;
				
			case IDM_ESCAPE:
				if (wPrvKeyDwn != 0) 
					app.RubberBandingDisable();
				else
					pDoc->DoEditPrimitiveEscape();

				// remove all nodes from list
				if (wPrvKeyDwn == 0) 
				{
					nodal::SegList = new CSegs;
					nodal::UPL = new CObList;
					nodal::PrimLis = new CObList;
					CPegView::GetActiveView()->InvalidateRect(0, TRUE);

					// commented out because it was crashingc
					//CUniqPt* pUniqPt;
					//POSITION pos = nodal::UPL->GetHeadPosition();
					//while (pos != 0)
					//{
					//	pUniqPt = (CUniqPt*) nodal::UPL->GetNext(pos);
					//	delete pUniqPt;
					//}
					//nodal::UPL->RemoveAll();
					//delete nodal::UPL;
					//
					//CNodalPrim* pNodalPrim;
					//pos = nodal::PrimLis->GetHeadPosition();
					//while (pos != 0)
					//{
					//	pNodalPrim = (CNodalPrim*) nodal::PrimLis->GetNext(pos);
					//	delete pNodalPrim;
					//}
					//nodal::PrimLis->RemoveAll();
					//delete nodal::PrimLis;
					//
					//delete nodal::SegList;
				}
				app.ModeLineUnhighlightOp(wPrvKeyDwn);
				
			default:
				lResult = !0;
		}
		if (lResult == 0)
			return (lResult);
	}

	return(CallWindowProc(app.GetMainWndProc(), hwnd, anMsg, wParam, lParam));
}

void nodal::AddByArea(CPnt arPt1, CPnt arPt2)
{
	CSeg*		pSeg;
	CPrim*		pPrim;
	DWORD		dwMask;
	POSITION	posPrim;
	CPnt		rWnd[2];
	
	line::Extents_xy(CLine(arPt1, arPt2), rWnd[0], rWnd[1]); 			// Normalize area definition
	
	CPnts pts;

	POSITION pos = detsegs.GetHeadPosition();
	while (pos != 0)
	{
		pSeg = detsegs.GetNext(pos);
		
		posPrim = pSeg->GetHeadPosition();
		while (posPrim != 0)
		{
			pPrim = pSeg->GetNext(posPrim);
			dwMask = nodal::GetPrimMask(pPrim);
			pPrim->GetAllPts(pts);
			
			for (int i = 0; i < (int) pts.GetSize(); i++)
			{
				if (p2DPtInWnd(pts[i], rWnd))
					nodal::UpdLis(nodal::bAdd, pSeg, pPrim, dwMask, i, pts[i]);
			}			
		}
	}
}

void nodal::AddByPt(const CPnt& pt)
{
	DWORD dwMask;
	CPnts pts;

	POSITION pos = detsegs.GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = detsegs.GetNext(pos);
		
		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim != 0)
		{
			CPrim* pPrim = pSeg->GetNext(posPrim);

			dwMask = nodal::GetPrimMask(pPrim);
			pPrim->GetAllPts(pts);
			
			for (int i = 0; i < (int) pts.GetSize(); i++)
			{
				if (CVec(pts[i], pt).Length() <= .05)
					nodal::UpdLis(nodal::bAdd, pSeg, pPrim, dwMask, i, pts[i]);
			}	
		}
	}
}

void nodal::AddEngPrim()
{
	if (detsegs.IsSegEngaged()) {return;}
	
	DWORD dwMask = nodal::GetPrimMask(detsegs.DetPrim());
	CPnts pts;

	detsegs.DetPrim()->GetAllPts(pts);
		
	for (int i = 0; i < (int) pts.GetSize(); i++)
		nodal::UpdLis(nodal::bAdd, detsegs.DetSeg(), detsegs.DetPrim(), dwMask, i, pts[i]);
}

///<summary>Maintains an ordered list of the primatives with at least one identified node.</summary>
void nodal::AddPrimBit(bool abSet, const CPrim* pPrim, int aiBit)
{
	CNodalPrim* pNodalPrim = 0;
		
	POSITION pos = nodal::PrimLis->GetHeadPosition();
	while (pos != 0)
	{
		POSITION posCur = pos;
		pNodalPrim = (CNodalPrim*) nodal::PrimLis->GetNext(pos);
		if (pNodalPrim->pPrim == pPrim)
		{
			pos = posCur; 
			break;
		}
	}
	if (abSet) 
	{
		if (pos == 0)										// Add to list
		{
			pNodalPrim = new CNodalPrim;
			pNodalPrim->pPrim = pPrim;
			pNodalPrim->dwMask = 0;
			nodal::PrimLis->AddTail(pNodalPrim);
		}
		pNodalPrim->dwMask = ibset(pNodalPrim->dwMask, aiBit);
	}
	else
	{
	   if (pos != 0)
			pNodalPrim->dwMask = ibclr(pNodalPrim->dwMask, aiBit);
	}
}

void nodal::AddPrimByPt(const CPnt pt)
{
	CSeg* pSeg = detsegs.SelSegAndPrimUsingPoint(pt);
	if (pSeg == 0)
		return;
	
	CPrim* pPrim = detsegs.DetPrim();

	CPnts pts;
					
	DWORD dwMask = nodal::GetPrimMask(pPrim);
	pPrim->GetAllPts(pts);

	for (int i = 0; i < pts.GetSize(); i++)
		nodal::UpdLis(nodal::bAdd, pSeg, pPrim, dwMask, i, pts[i]);
}

void nodal::CopyPrims(const CVec& vTrns)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	CPegView* pView = CPegView::GetActiveView();
	CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();

	CSeg* pSeg;
	CSeg* pNewSeg;
	
	int iPrimState = pstate.Save();

	POSITION pos = nodal::SegList->GetHeadPosition();
	while (pos != 0)
	{
		pSeg = nodal::SegList->GetNext(pos);
		
		pNewSeg = new CSeg(*pSeg);
					
		pDoc->WorkLayerAddTail(pNewSeg); 
		
		pSeg->Translate(vTrns);
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG, pSeg);
	}
	pstate.Restore(pDC, iPrimState);
}

void nodal::GenFilAreas(const CVec& )//vTrns)
{
	//CPegDoc* pDoc = CPegDoc::GetDoc();
	//CPegView* pView = CPegView::GetActiveView();
	//CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();

	//CSeg*		pSeg;
	//CSeg*		pSegNew;
	//CPrim*		pPrim;
	//CPrimLine*		pLine;
	//CPrimPolygon*	pPolygon;

	//DWORD	dwMask;
	//CPnt	pt[4];
	//CVec	vPlnNorm, vXAx, vYAx;
	//
	//int iPrimState = pstate.Save();
	//
	//POSITION pos = nodal::SegList->GetHeadPosition();
	//while (pos != 0)
	//{
	//	pSeg = nodal::SegList->GetNext(pos);
	//	
	//	POSITION posPrim = pSeg->GetHeadPosition();
	//	while (posPrim != 0)
	//	{
	//		pPrim = pSeg->GetNext(posPrim);
	//		
	//		dwMask = nodal::GetPrimMask(pPrim); 
	//		if (dwMask == 0)
	//			continue;
	//		if (pPrim->Is(CPrim::PRIM_LINE))
	//		{
	//			pLine = static_cast<CPrimLine*>(pPrim);
	//			if ((dwMask & 3) == 3) 
	//			{
	//				pt[0] = pLine->Pt0();
	//				pt[1] = pLine->Pt1();
	//				pt[2] = pt[1] + vTrns;
	//				pt[3] = pt[0] + vTrns;
	//				vXAx = CVec(pt[0], pt[1]);
	//				vYAx = CVec(pt[0], pt[2]);
	//				vPlnNorm = vXAx ^ vYAx;
	//				vPlnNorm.Normalize();
	//				if (vPlnNorm[2] < 0.) 
	//					vPlnNorm = - vPlnNorm;
	//				vXAx.Normalize();
	//				vXAx.RotAboutArbAx(vPlnNorm, hatch::dOffAng);
	//				vYAx = vXAx;
	//				vYAx.RotAboutArbAx(vPlnNorm, HALF_PI);
	//				vXAx *= hatch::dXAxRefVecScal;
	//				vYAx *= hatch::dYAxRefVecScal;
	//				// Project reference origin to plane
	//				pSegNew = new CSeg(new CPrimPolygon(4, pt[0], vXAx, vYAx, pt));
	//				pDoc->WorkLayerAddTail(pSegNew); 
	//				pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSegNew);
	//			}
	//		}
	//		else if (pPrim->Is(CPrim::PRIM_POLYGON))
	//		{
	//			pPolygon = static_cast<CPrimPolygon*>(pPrim);
	//			int iPts = pPolygon->GetPts();
	//				
	//			for (int i = 0; i < iPts; i++)
	//			{
	//				if (btest(dwMask, i) && btest(dwMask, ((i + 1) % iPts)))
	//				{
	//					pt[0] = pPolygon->GetPt(i);
	//					pt[1] = pPolygon->GetPt((i + 1) % iPts);
	//					pt[2] = pt[1] + vTrns;
	//					pt[3] = pt[0] + vTrns;
	//					vXAx = CVec(pt[0], pt[1]);
	//					vYAx = CVec(pt[0], pt[2]);
	//					vPlnNorm = vXAx ^ vYAx;
	//					vPlnNorm.Normalize();
	//					if (vPlnNorm[2] < 0.) 
	//						vPlnNorm = - vPlnNorm;
	//					vXAx.Normalize();
	//					vXAx.RotAboutArbAx(vPlnNorm, hatch::dOffAng);
	//					vYAx = vXAx;
	//					vYAx.RotAboutArbAx(vPlnNorm, HALF_PI);
	//					vXAx *= hatch::dXAxRefVecScal;
	//					vYAx *= hatch::dYAxRefVecScal;
	//					// Project reference origin to plane
	//					pSegNew = new CSeg(new CPrimPolygon(4, pt[0], vXAx, vYAx, pt));
	//					pDoc->WorkLayerAddTail(pSegNew); 
	//					pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSegNew);
	//				}
	//			}
	//		}
	//	}
	//}
	//pstate.Restore(pDC, iPrimState);
}

void nodal::GenVecs(const CVec& vTrns)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CUniqPt* pUniqPt;

	CSeg* pSeg = new CSeg;
					
	POSITION pos = nodal::UPL->GetHeadPosition();
	while (pos != 0)
	{
		pUniqPt = (CUniqPt*) nodal::UPL->GetNext(pos);
		pSeg->AddTail(new CPrimLine(pUniqPt->pt, pUniqPt->pt + vTrns));
	}	
	pDoc->WorkLayerAddTail(pSeg); 
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
}

DWORD nodal::GetPrimMask(CPrim* pPrim)
{
	CNodalPrim* pNodalPrim = 0;
	
	POSITION pos = nodal::PrimLis->GetHeadPosition();
	while(pos != 0)
	{
		POSITION posCur = pos;
		pNodalPrim = (CNodalPrim*) nodal::PrimLis->GetNext(pos);
		if (pNodalPrim->pPrim == pPrim)
		{
			pos = posCur; 
			break;
		}
	}
	return ((pos != 0) ? pNodalPrim->dwMask : 0UL);
}
//Effect:	Translate all control points identified
void nodal::Translate(const CVec& vTrns)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	DWORD	dwMask;
	
	POSITION pos = nodal::SegList->GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = nodal::SegList->GetNext(pos);
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSeg);

		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim != 0)
		{
			CPrim* pPrim = pSeg->GetNext(posPrim);
			dwMask = nodal::GetPrimMask(pPrim); 
			pPrim->TranslateUsingMask(vTrns, dwMask);
		}
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
	}
	CUniqPt* pUniqPt;
	 
	pos = nodal::UPL->GetHeadPosition();
	while (pos != 0)
	{
		pUniqPt = (CUniqPt*) nodal::UPL->GetNext(pos);
		pUniqPt->pt += vTrns;
	}			
}

void nodal::UpdLis(bool abAdd, CSeg* pSeg, CPrim* pPrim, DWORD dwMask, int aiBit, CPnt arPt)
{
	CUniqPt* pUniqPt;
	
	if (abAdd)													// Adding points to list
	{
		if (!btest(dwMask, aiBit))								// Point not already in list
		{
			if (!nodal::SegList->Find(pSeg))
				nodal::SegList->AddTail(pSeg);
			
			nodal::AddPrimBit(abAdd, pPrim, aiBit);
						
			POSITION pos = nodal::UPL->GetHeadPosition();
			while (pos != 0)
			{
				pUniqPt = (CUniqPt*) nodal::UPL->GetNext(pos);
				if (arPt == pUniqPt->pt)						// Point already in list
					{(pUniqPt->iCnt)++; return;}
			}
			pUniqPt = new CUniqPt; pUniqPt->iCnt = 1; pUniqPt->pt = arPt;
			nodal::UPL->AddTail(pUniqPt);
		}
	}
	else														// Removing points from list
	{
		if (btest(dwMask, aiBit))								// Point already in list
		{
			POSITION posDel;
			nodal::AddPrimBit(abAdd, pPrim, aiBit);
			
			POSITION pos = nodal::UPL->GetHeadPosition();
			while (pos != 0)
			{
				posDel = pos;
				pUniqPt = (CUniqPt*) nodal::UPL->GetNext(pos);
				if (arPt == pUniqPt->pt)						// Point found
				{
					(pUniqPt->iCnt)--;
					if (pUniqPt->iCnt == 0) 					// Point was unique
					{	
						nodal::UPL->RemoveAt(posDel);
						delete pUniqPt;
					}
					break;
				}
			}
		}
	}
}
