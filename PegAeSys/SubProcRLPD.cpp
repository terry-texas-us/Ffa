#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "SubProcCut.h"
#include "SubProcRLPD.h"

#include "ParLines.h"
#include "UnitsString.h"
#include "UserAxis.h"

namespace rlpd
{
	EJust0	eJust0 = Center0;
	bool	bGenCenterline = false;
	bool	bContSec = false;
	double	dSecDiam;
	double	dInsideRadiusFactor = 1.5;		// inside radius elbow factor

	double	dDiam[3] = {.125, .125, .125};
}

CSeg* pgEndCapSeg = 0;

LRESULT CALLBACK SubProcRLPD(HWND hwnd, UINT anMsg, WPARAM wParam, LPARAM lParam)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	CPegView* pView = CPegView::GetActiveView();
	CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();

	static WORD wPrvKeyDwn = 0;
	static CPnt ptPrv;
	static CLine lnPar[4];
	static CSeg* pEndCapSeg = 0; 
	static CPrimMark* pEndCapMark = 0;
	static int iEndCapId = 0;		

	CPrim*	pLeftLine;
	CPrim*	pRightLine;
	CLine	lnLead; 
	CLine	lnCap;

	double	dAng;
	int iJus[2];
	
	double	dSlo[2] = {4., 4.};
	
	if(anMsg == WM_COMMAND)
	{
		long lResult = 0;

		CPnt ptCur = app.CursorPosGet();
		
		switch(LOWORD(wParam))
		{
			// a test for non-zero z used to result in error message if 0-9 
			
			case IDM_RETURN:
				if(wPrvKeyDwn != 0)
				{
					app.RubberBandingDisable();
					app.ModeLineUnhighlightOp(wPrvKeyDwn);
					rlpd::bContSec = false;
					pEndCapSeg = 0; 
					pEndCapMark = 0;
				}
				ptPrv = ptCur;
				break;
				
			case ID_OP0:
				rlpd::SetOptions(&rlpd::dDiam[1]);
				break;
				
			case ID_OP1:			// Search for an endcap in proximity of current location
				if(rlpd::SelEndCapUsingPoint(ptCur, pEndCapSeg, pEndCapMark)) 
				{	// Endcap located
					char szLen[32];
					char* pszLen;
												
					ptPrv = pEndCapMark->GetPt();
					rlpd::dDiam[1] = pEndCapMark->GetDat(0);
					rlpd::bContSec = false;
					
					iEndCapId = (wPrvKeyDwn == 0) ? 1 : - 1; // 1 (start) and -1 (end)
					
					CString strMes("Cross sectional dimension (Diameter) is ");
					UnitsString_FormatLength(szLen, max(app.GetUnits(), Inches), rlpd::dDiam[1], 12, 2);
					
					pszLen = szLen;
					while (!isdigit(pszLen[0]))
						pszLen ++;
					
					strMes += pszLen;
					msgInformation(strMes);
					app.CursorPosSet(ptPrv);					
				}
				break;
				
			case ID_OP2:
				if (wPrvKeyDwn != 0) 
					app.RubberBandingDisable();
				
				if(pEndCapSeg == 0)
				{	// No end-cap consideration
					if(wPrvKeyDwn == ID_OP2) 
					{
						ptCur = UserAxis::UserAxisSnapLn(ptPrv, ptCur);
						lnPar[0] = lnPar[2]; 
						lnPar[1] = lnPar[3];
						lnLead(ptPrv, ptCur);
						if(ParLines_GenPts(.5, rlpd::dDiam[2], lnLead, &lnPar[2]))
						{	// Lead line vector is not null
							if(rlpd::bContSec) 
							{
								if(ParLines_CleanCorners(&lnPar[0], &lnPar[2])) 
								{
									rlpd::GenElbow(rlpd::dDiam[2], rlpd::dDiam[2], lnPar);
									
									if(!rlpd::bContSec) 
										return 0;
								}
								else
								{
									CPnt ptBegLeft = lnPar[0][0];
									CPnt ptBegRight = lnPar[1][0];
									ptPrv[0] = (ptBegLeft[0] + ptBegRight[0]) * .5f;
									ptPrv[1] = (ptBegLeft[1] + ptBegRight[1]) * .5f;
									lnLead(ptPrv, ptCur);
									
									ParLines_GenPts(.5, rlpd::dDiam[2], lnLead, &lnPar[2]);
									
									pDoc->AnyLayerRemove(pgEndCapSeg);
									detsegs.Remove(pgEndCapSeg);
									trapsegs.Remove(pgEndCapSeg);
									pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pgEndCapSeg);
									
									pgEndCapSeg->RemovePrims();
									delete pgEndCapSeg;
								}
							}  
							rlpd::GenSection(rlpd::dDiam[2], &lnPar[2]);
							rlpd::bContSec = true;
							pgEndCapSeg = pDoc->WorkLayerGetTail();
						}
						else			
							// Zero length section specified
							msgInformation(0);
					}
					else if(wPrvKeyDwn == ID_OP3) 
					{
						ptCur = UserAxis::UserAxisSnapLn(ptPrv, ptCur);
						lnLead(ptPrv, ptCur);
						if(ParLines_GenPts(.5, rlpd::dDiam[2], lnLead, &lnPar[2]))
						{	// Lead line vector is not null
							rlpd::GenTransition(rlpd::Begin, iJus, dSlo, rlpd::dDiam, rlpd::dDiam, &lnPar[2]);
							ptCur[0] = (lnPar[2][1][0] + lnPar[3][1][0]) * .5f;
							ptCur[1] = (lnPar[2][1][1] + lnPar[3][1][1]) * .5f;
							rlpd::bContSec = true;
							pgEndCapSeg = 0;
						}
						else			
							// Zero length section specified
							msgInformation(0);
					}
				}
				else
				{	// Work with existing end-cap
					if (iEndCapId > 0)			
					{	// Begin at end-cap
						POSITION pos = pEndCapSeg->Find(pEndCapMark);
						pEndCapSeg->GetNext(pos);
						CPrimLine* pPrimCapLine = static_cast<CPrimLine*>(pEndCapSeg->GetAt(pos));
						rlpd::bContSec = rlpd::Fnd2LnsGivLn(pPrimCapLine, RADIAN, pLeftLine, &lnPar[2], pRightLine, &lnPar[3]);
						pgEndCapSeg = pEndCapSeg;
					}
					pEndCapSeg = 0; 
					pEndCapMark = 0;
				}
				wPrvKeyDwn = ID_OP2;
				ptPrv = ptCur;
				app.CursorPosSet(ptPrv);
				app.RubberBandingStartAtEnable(ptPrv, Lines);
				break;
	
			case ID_OP3:
			{
				double distCent = 0;
				CPnt ptTemp;
				
				if(wPrvKeyDwn != 0) 
					app.RubberBandingDisable();
				rlpd::dDiam[1] = rlpd::dDiam[2]; 
				rlpd::SetOptions(&rlpd::dDiam[2]);
				distCent = rlpd::dDiam[2];
				iJus[1] = rlpd::eJust0;
				//iJus[1] *= 4; //??
				
				if(pEndCapMark == 0)		 // No end-cap consideration
				{
					if(wPrvKeyDwn == 0) 			// Beginning initial section
					{
						rlpd::dDiam[0] = rlpd::dDiam[1]; 
						iJus[0] = iJus[1];
						rlpd::dDiam[1] = rlpd::dDiam[2]; 
						wPrvKeyDwn = ID_OP3;
					}
					else if(wPrvKeyDwn == ID_OP2)
					{
						ptCur = UserAxis::UserAxisSnapLn(ptPrv, ptCur); // Constrain point to user axis
						lnPar[0] = lnPar[2]; lnPar[1] = lnPar[3];
						lnLead(ptPrv, ptCur);
						if(ParLines_GenPts(.5, rlpd::dDiam[1], lnLead, &lnPar[2]))	// Lead line vector is not nu
						{
							CPnt ptCent[4];
		
							ptCent[0] = lnPar[2][0];
							ptCent[1] = lnPar[2][1];
							ptCent[2] = lnPar[3][0];
							ptCent[3] = lnPar[3][1];
						
							if(rlpd::bContSec) 
							{
								if(ParLines_CleanCorners(&lnPar[0], &lnPar[2])) 
								{
									rlpd::GenElbow(rlpd::dDiam[1], 0, lnPar);
									if(! rlpd::bContSec) 
										return 0;
								}
								else
								{
									ptPrv[0] = (lnPar[0][0][0] + lnPar[1][0][0]) * .5f;
									ptPrv[1] = (lnPar[0][0][1] + lnPar[1][0][1]) * .5f;
									lnLead(ptPrv, ptCur);
									ParLines_GenPts(.5, rlpd::dDiam[1], lnLead, &lnPar[2]);
									
									pDoc->AnyLayerRemove(pgEndCapSeg);
									detsegs.Remove(pgEndCapSeg);
									trapsegs.Remove(pgEndCapSeg);
									pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pgEndCapSeg);
									
									pgEndCapSeg->RemovePrims();
									delete pgEndCapSeg;
								}
							}
								
							rlpd::GenTransition(rlpd::End, &iJus[1], dSlo, &rlpd::dDiam[1], &rlpd::dDiam[1], &lnPar[2]);
				
							if(rlpd::bGenCenterline)
							{
								CSeg* pSeg = pDoc->WorkLayerGetTail();

								ptCent[0] = Mid(ptCent[0], ptCent[2]);
								ptCent[1] = Mid(ptCent[1], ptCent[3]);
			
								ptCent[0] = Pnt_ProjPtTo(ptCent[0], ptCent[1], - distCent / 2);
								ptCent[1] = Pnt_ProjPtTo(ptCent[1], ptCent[0], - distCent / 2);

								pSeg->AddTail(new CPrimLine(1, 10, ptCent[0], ptCent[1]));

								::InvalidateRect(hwnd, 0, 1);
							}
							rlpd::dDiam[1] = rlpd::dDiam[2];
							ptCur[0] = (lnPar[2][1][0] + lnPar[3][1][0]) * .5f;
							ptCur[1] = (lnPar[2][1][1] + lnPar[3][1][1]) * .5f;
							rlpd::bContSec = false;
							pgEndCapSeg = 0;
							wPrvKeyDwn = ID_OP2;
						}
						else							 // Zero length section specified
							msgInformation(0);
					}
					else if(wPrvKeyDwn == ID_OP3) 
					{
						ptCur = UserAxis::UserAxisSnapLn(ptPrv, ptCur); // Constrain point to user axis
						lnPar[0] = lnPar[2]; lnPar[1] = lnPar[3];
						lnLead(ptPrv, ptCur);
						if(ParLines_GenPts(.5, rlpd::dDiam[0], lnLead, &lnPar[2]))	// Lead line vector is not nu
						{
							rlpd::GenTransition(rlpd::Both, iJus, dSlo, rlpd::dDiam, rlpd::dDiam, &lnPar[2]);
							rlpd::dDiam[1] = rlpd::dDiam[2]; 
							rlpd::bContSec = false;
							pgEndCapSeg = 0;
							wPrvKeyDwn = ID_OP2;
						}
						else							 // Zero length section specified
							msgInformation(0);
					}
				}
				else								   // Work with existing end-cap
				{
					if(iEndCapId > 0)				// Begin at end-cap
					{
						POSITION pos = pEndCapSeg->Find(pEndCapMark);
						pEndCapSeg->GetNext(pos);
						CPrimLine* pPrimCapLine = static_cast<CPrimLine*>(pEndCapSeg->GetAt(pos));
						rlpd::bContSec = rlpd::Fnd2LnsGivLn(pPrimCapLine, RADIAN, pLeftLine, &lnPar[2], pRightLine, &lnPar[3]);
						pgEndCapSeg = pEndCapSeg;
					}
					pEndCapSeg = 0; 
					pEndCapMark = 0;
					wPrvKeyDwn = ID_OP2;
				}
				ptTemp = ptPrv;		//allows for turn after transition
				app.RubberBandingDisable();
				wPrvKeyDwn = ID_OP2;
				ptPrv = ptCur;
				app.CursorPosSet(ptPrv);
				app.RubberBandingStartAtEnable(ptPrv, Lines);
	
				ptCur = Pnt_ProjPtTo(ptCur, ptTemp, rlpd::dInsideRadiusFactor * rlpd::dDiam[2]);

				app.CursorPosSet(ptCur);
				SubProcRLPD(hwnd, WM_COMMAND, ID_OP2, lParam);
								
				ptPrv = ptCur;
				app.CursorPosSet(ptPrv);
				app.RubberBandingStartAtEnable(ptPrv, Lines);
			}
				break;
	  
			case ID_OP4:
			{
				CSeg *pSeg;
				CObject *pObj;
				CPnt pt[2], ptCent[3];

				if(!detsegs.SelSegAndPrimUsingPoint(ptCur))
					break;
				if(wPrvKeyDwn != 0) 
					app.RubberBandingDisable();
		
				iJus[1] = rlpd::eJust0;
				//iJus[1] *= 4; //??
				if(pEndCapMark == 0)				 // No end-cap consideration
				{
					if(wPrvKeyDwn == ID_OP2)
					{
						ptCur = UserAxis::UserAxisSnapLn(ptPrv, ptCur); // Constrain point to user axis
						lnPar[0] = lnPar[2];
						lnPar[1] = lnPar[3];
						
						CPrimLine* pLine = static_cast<CPrimLine*>(detsegs.DetPrim());
						lnLead[0] = ptPrv;
						lnLead[1] = pLine->Ln().ProjPt(ptPrv);
						
						if(Pnt_DistanceTo_xy(lnLead[0], lnLead[1]) <= rlpd::dDiam[2])
						{
							wPrvKeyDwn = 0;
							break;
						}

						if (ParLines_GenPts(.5, rlpd::dDiam[2], lnLead, &lnPar[2]))	// Lead line vector is not nu
						{
							ptCent[0] = lnPar[2][0];
							ptCent[1] = lnPar[2][1];
							ptCent[2] = lnPar[3][0];
							ptCent[3] = lnPar[3][1];

							rlpd::dDiam[1] = rlpd::dDiam[2];
							rlpd::dDiam[2] *= 2;
							dSlo[0] = dSlo[1] /= 12;
							rlpd::GenTransition(rlpd::End, &iJus[1], dSlo, &rlpd::dDiam[1], &rlpd::dDiam[2], &lnPar[2]);
							ptCur[0] = (lnPar[2][1][0] + lnPar[3][1][0]) * .5f;
							ptCur[1] = (lnPar[2][1][1] + lnPar[3][1][1]) * .5f;
							rlpd::bContSec = false;
							pgEndCapSeg = 0;
							wPrvKeyDwn = 0;	//ID_OP2;
						}
						else							 // Zero length section specified
							msgInformation(0);
					}	
				}
				else								   // Work with existing end-cap
				{
					if (iEndCapId > 0)				// Begin at end-cap
					{
						POSITION pos = pEndCapSeg->Find(pEndCapMark);
						pEndCapSeg->GetNext(pos);
						CPrimLine* pPrimCapLine = static_cast<CPrimLine*>(pEndCapSeg->GetAt(pos));
						rlpd::bContSec = rlpd::Fnd2LnsGivLn(pPrimCapLine, RADIAN, pLeftLine, &lnPar[2], pRightLine, &lnPar[3]);
						pgEndCapSeg = pEndCapSeg;
					}
					pEndCapSeg = 0; 
					pEndCapMark = 0;
					wPrvKeyDwn = 0;	//ID_OP2;
				}
				ptPrv = ptCur;
				app.CursorPosSet(ptPrv);
				wPrvKeyDwn = 0;

				pSeg = pDoc->WorkLayerGetTail();
				pObj = pSeg->GetTail();
				pSeg->RemoveTail();
				pSeg->RemoveTail();
				pSeg->AddTail(pObj);
				
				pt[0] = lnPar[3][1];
				pt[1] = Pnt_RotAboutArbPtAndAx(pt[0], ptPrv, ZDIR, HALF_PI);
				pt[2] = Pnt_RotAboutArbPtAndAx(pt[1], ptPrv, ZDIR, PI);
				if(Pnt_DistanceTo_xy(pt[1], lnPar[3][0]) < Pnt_DistanceTo_xy(pt[2], lnPar[3][0]))
					pt[1] = pt[2];
				pt[1] = Pnt_ProjPtTo(ptCur, pt[1], rlpd::dDiam[2] * 0.25);
				pt[2] = lnPar[2][1];
				CPrimArc* pArc = new CPrimArc(pt[0], pt[1], pt[2]);
				pSeg->AddTail(pArc);

				cut::CutPrimsAtPt(pt[0]);
				cut::CutPrimsAtPt(pt[2]);
				pt[1] = Pnt_ProjPtTo(pt[0], pt[2], rlpd::dDiam[2] * 0.25);
				pDoc->DoPrimDelete(pt[1]);
			
				if(rlpd::bGenCenterline)
				{
					ptCent[0] = Pnt_ProjPtTo(ptCent[0], ptCent[2], rlpd::dDiam[2] / 4);
					ptCent[1] = Pnt_ProjPtTo(ptCent[1], ptCent[3], rlpd::dDiam[2] / 4);
			
					ptCent[0] = Pnt_ProjPtTo(ptCent[0], ptCent[1], -rlpd::dDiam[2] / 4);
					ptCent[1] = Pnt_ProjPtTo(ptCent[1], ptCent[0], -rlpd::dDiam[2] / 4);
					pSeg->AddTail(new CPrimLine(1, 10, ptCent[0], ptCent[1]));
				}
				::InvalidateRect(hwnd, 0, 1);
				rlpd::dDiam[2] /= 2;
			}
				break;
	  
			case ID_OP5:										// Full ell takeoff
				break;
				
			case ID_OP6:										// Tee
			{
				double slope[2] = {0.5, 0.5};
				double dDist = 0.125;
				CSeg *pSeg;
				CPnt ptBeg, ptEnd, ptMid;
				CPnt pt[5], ptCent[3];

				if(!detsegs.SelSegAndPrimUsingPoint(ptCur))
					break;
				if(wPrvKeyDwn != 0) 
					app.RubberBandingDisable();
		
				iJus[1] = rlpd::eJust0;
				//iJus[1] *= 4; 
				if(pEndCapMark == 0)				 // No end-cap consideration
				{
					if(wPrvKeyDwn == ID_OP2)
					{
						ptCur = UserAxis::UserAxisSnapLn(ptPrv, ptCur); // Constrain point to user axis
						lnPar[0] = lnPar[2];
						lnPar[1] = lnPar[3];
						
						CPrimLine* pLine = static_cast<CPrimLine*>(detsegs.DetPrim());
						lnLead[0] = ptPrv;
						lnLead[1] = pLine->Ln().ProjPt(ptPrv);
						     
						if(Pnt_DistanceTo_xy(lnLead[0], lnLead[1]) <= rlpd::dDiam[2])
						{
							wPrvKeyDwn = 0;
							break;
						}

						if(detsegs.SelSegAndPrimUsingPoint(ptCur) != 0)  // find diameter of duct
						{
							if(detsegs.DetPrim()->Is(CPrim::PRIM_LINE))
							{
								(static_cast<CPrimLine*>(detsegs.DetPrim()))->GetPts(ptBeg, ptEnd);
								CSeg::IgnorePrim() = detsegs.DetPrim();

								CPnt4 ptView(ptEnd, 1.);
								ModelView_DoTransform(ptView);
								if (detsegs.SelSegAndPrimAtCtrlPt(ptView))
								{
									if (detsegs.DetPrim()->Is(CPrim::PRIM_LINE))
									{
										(static_cast<CPrimLine*>(detsegs.DetPrim()))->GetPts(ptBeg, ptEnd);
										ptMid = Mid(ptBeg, ptEnd);
										if (rlpd::SelEndCapUsingPoint(ptMid, pEndCapSeg, pEndCapMark)) 
										dDist = pEndCapMark->GetDat(0);
									}
								}
							}
						}
						if (ParLines_GenPts(.5, rlpd::dDiam[2], lnLead, &lnPar[2]))	// Lead line vector is not nu
						{
							ptCent[0] = lnPar[2][0];
							ptCent[1] = lnPar[2][1];
							ptCent[2] = lnPar[3][0];
							ptCent[3] = lnPar[3][1];

							rlpd::dDiam[1] = rlpd::dDiam[2];
							rlpd::dDiam[2] *= 2;
							rlpd::GenTransition(rlpd::End, &iJus[1], slope, &rlpd::dDiam[1], &rlpd::dDiam[2], &lnPar[2]);
							ptCur[0] = (lnPar[2][1][0] + lnPar[3][1][0]) * .5f;
							ptCur[1] = (lnPar[2][1][1] + lnPar[3][1][1]) * .5f;
							rlpd::bContSec = false;
							pgEndCapSeg = 0;
							wPrvKeyDwn = 0;	
						}
						else							 // Zero length section specified
							msgInformation(0);
					}	
				}
				else								   // Work with existing end-cap
				{
					if (iEndCapId > 0)				// Begin at end-cap
					{
						POSITION pos = pEndCapSeg->Find(pEndCapMark);
						pEndCapSeg->GetNext(pos);
						CPrimLine* pPrimCapLine = static_cast<CPrimLine*>(pEndCapSeg->GetAt(pos));
						rlpd::bContSec = rlpd::Fnd2LnsGivLn(pPrimCapLine, RADIAN, pLeftLine, &lnPar[2], pRightLine, &lnPar[3]);
						pgEndCapSeg = pEndCapSeg;
					}
					pEndCapSeg = 0; 
					pEndCapMark = 0;
					wPrvKeyDwn = 0;	
				}
				ptPrv = ptCur;
				app.CursorPosSet(ptPrv);
				wPrvKeyDwn = 0;

				pSeg = pDoc->WorkLayerGetTail();
				pSeg->RemoveTail();
				pSeg->RemoveTail();
				pSeg->RemoveTail();
				pSeg->RemoveTail();
				
				pt[0] = Pnt_ProjPtTo(lnPar[3][0], lnPar[2][0], rlpd::dDiam[2] / 4);
				pt[1] = Pnt_ProjPtTo(lnPar[3][1], ptCur, rlpd::dDiam[2] * 0.05);
				pt[2] = lnPar[3][1];
				
				pt[3] = Pnt_ProjPtTo(lnPar[2][0], pt[0], rlpd::dDiam[2] / 4);
				pt[4] = Pnt_ProjPtTo(lnPar[2][1], ptCur, rlpd::dDiam[2] * 0.05);
				pt[5] = lnPar[2][1];
				
				pSeg->AddTail(new CPrimArc(pt[0], pt[1], pt[2]));
				pSeg->AddTail(new CPrimArc(pt[3], pt[4], pt[5]));
      
				cut::CutPrimsAtPt(pt[2]);
				cut::CutPrimsAtPt(pt[5]);      
				pt[1] = Pnt_ProjPtTo(pt[2], pt[5], rlpd::dDiam[2] * 0.25);
				pDoc->DoPrimDelete(pt[1]);

				pt[1] = Pnt_ProjPtTo(lnPar[3][1], lnPar[3][0], -dDist);
				pt[4] = Pnt_ProjPtTo(lnPar[2][1], lnPar[2][0], -dDist);

				pSeg->AddTail(new CPrimLine(pt[1], lnPar[3][1]));
				pSeg->AddTail(new CPrimLine(pt[4], lnPar[2][1]));

				if (rlpd::bGenCenterline)
				{
					ptCent[0] = Pnt_ProjPtTo(ptCent[0], ptCent[2], rlpd::dDiam[2] / 4);
					ptCent[1] = Pnt_ProjPtTo(ptCent[1], ptCent[3], rlpd::dDiam[2] / 4);
			
					ptCent[0] = Pnt_ProjPtTo(ptCent[0], ptCent[1], -rlpd::dDiam[2] / 4);
					ptCent[1] = Pnt_ProjPtTo(ptCent[1], ptCent[0], -rlpd::dDiam[2] / 4);
					pSeg->AddTail(new CPrimLine(1, 10, ptCent[0], ptCent[1]));
				}
				::InvalidateRect(hwnd, 0, 1);
				rlpd::dDiam[2] /= 2;
			}
				break;
				
			case ID_OP7:
			case ID_OP8:
				//	Search for an endcap in proximity of current location
				if(rlpd::SelEndCapUsingPoint(ptCur, pEndCapSeg, pEndCapMark)) 
				{	// Endcap located
					CVec vVwPlnNorm = view::GetDirection();
					PENCOLOR nPenColor;
					PENSTYLE nPenStyle;
					
					CPnt ptTemp, ptBeg, ptEnd, ptMid;
					CPnt ptUpDn[3];
					int i = 0;

					ptPrv = pEndCapMark->GetPt();
					rlpd::dDiam[1] = pEndCapMark->GetDat(0);	//duct width
					i = 0;
					//ptBeg = ptPrv + dDiam[1] / 4;		//find line on end cap mark
					ptBeg[2] = 0;
					while(detsegs.SelSegAndPrimUsingPoint(ptBeg) == 0 && i < 500)
					{
						ptBeg = Pnt_RotAboutArbPtAndAx(ptBeg, ptPrv, vVwPlnNorm, 1);
						ptCur = ptBeg;
						i++;
					}
					if(detsegs.SelSegAndPrimUsingPoint(ptBeg) != 0)
					{
						if(detsegs.DetPrim()->Is(CPrim::PRIM_LINE))
						{	// endpoints of endcap line
							(static_cast<CPrimLine*>(detsegs.DetPrim()))->GetPts(ptBeg, ptEnd);
						}
					}
					CSeg::IgnorePrim() = detsegs.DetPrim();		//find perpendicular line ie. length of duct

					CPnt4 ptView(ptBeg, 1.);
					ModelView_DoTransform(ptView);
					if(detsegs.SelSegAndPrimAtCtrlPt(ptView) == 0) {break;}
					if(!detsegs.DetPrim()->Is(CPrim::PRIM_LINE)) {break;}
					
					ptBeg = detsegs.DetPt();
					(static_cast<CPrimLine*>(detsegs.DetPrim()))->GetPts(ptBeg, ptMid);
					if(Pnt_DistanceTo_xy(ptBeg, ptEnd) > Pnt_DistanceTo_xy(ptMid, ptEnd))	//arrange points
					{
						ptTemp = ptBeg;
						ptBeg = ptMid;
						ptMid = ptTemp;
					}
					ptMid = Pnt_ProjPtTo(ptBeg, ptMid, rlpd::dDiam[1]);	
					ptTemp = Pnt_RotAboutArbPtAndAx(ptBeg, ptMid, vVwPlnNorm, HALF_PI);
					if(detsegs.SelSegAndPrimUsingPoint(ptTemp) == 0)							
						ptTemp = Pnt_RotAboutArbPtAndAx(ptTemp, ptMid, vVwPlnNorm, PI);
					ptEnd = Pnt_RotAboutArbPtAndAx(ptBeg, ptPrv, vVwPlnNorm, HALF_PI);
					
					CSeg* pSeg;
					CVec vMajAx(ptPrv, ptBeg);
					CVec vMinAx(ptPrv, ptEnd);
											
					ptEnd = Pnt_RotAboutArbPtAndAx(ptBeg, ptPrv, vVwPlnNorm, PI);
					ptUpDn[0] = ptBeg;
					ptUpDn[1] = ptEnd;

					cut::CutPrimsAtPt(ptUpDn[0]);
					cut::CutPrimsAtPt(ptUpDn[1]);
					ptBeg = Pnt_ProjPtTo(ptUpDn[0], ptUpDn[1], rlpd::dDiam[1] * 0.25);
					pDoc->DoPrimDelete(ptBeg);
						
					ptUpDn[2] = Pnt_ProjPtTo(ptUpDn[0], ptMid, rlpd::dInsideRadiusFactor * rlpd::dDiam[1]); 
					ptUpDn[3] = Pnt_ProjPtTo(ptUpDn[1], ptTemp, rlpd::dInsideRadiusFactor * rlpd::dDiam[1]); 
					pSeg = new CSeg(new CPrimLine(ptUpDn[2], ptUpDn[3]));
					ptEnd = Pnt_RotAboutArbPtAndAx(ptUpDn[1], ptUpDn[0],vVwPlnNorm, HALF_PI);
					if(Pnt_DistanceTo_xy(ptUpDn[0], ptUpDn[2]) > Pnt_DistanceTo_xy(ptUpDn[2], ptEnd))  //Solid line for up and down
						pSeg->AddTail(new CSeg(new CPrimArc(ptPrv, vMajAx, vMinAx, PI)));
					else
						pSeg->AddTail(new CSeg(new CPrimArc(ptPrv, vMajAx, vMinAx, -PI)));

					nPenColor = pstate.PenColor();
					nPenStyle = pstate.PenStyle();
					if(LOWORD(wParam) == ID_OP8)
						pstate.SetPen(pDC, 1, 2);
					
					if(Pnt_DistanceTo_xy(ptUpDn[0], ptUpDn[2]) < Pnt_DistanceTo_xy(ptUpDn[2], ptEnd))  //Solid or dashed line
						pSeg->AddTail(new CSeg(new CPrimArc(ptPrv, vMajAx, vMinAx, PI)));
					else
						pSeg->AddTail(new CSeg(new CPrimArc(ptPrv, vMajAx, vMinAx, -PI)));
						
					pDoc->WorkLayerAddTail(pSeg);
					pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
					
					if(LOWORD(wParam) == ID_OP8)
					{
						pstate.SetPen(pDC, nPenColor, nPenStyle);
					}
				}
				break;
				
			case ID_OP9:
			{
				dAng = 0.;
				CVec vVwPlnNorm = view::GetDirection();
				int i = 0;
				CPnt ptBeg, ptEnd, ptMid;

				ptBeg = ptCur;
				if (rlpd::SelEndCapUsingPoint(ptBeg, pEndCapSeg, pEndCapMark)) 
				{
					rlpd::dDiam[1] = pEndCapMark->GetDat(0);
					rlpd::GenSizeNote(ptCur, dAng, rlpd::dDiam[1]);		//if endcap found, get details and print dimension
					break;
				}
				while (!detsegs.SelSegAndPrimUsingPoint(ptBeg) && i < 100)
				{
					i++;										//moves cursor up to intercept
					ptBeg[1] += detsegs.PicApertSiz();		//line
				}
				if (i == 100)
				{
					i = 0;
					while (!detsegs.SelSegAndPrimUsingPoint(ptBeg) && i < 100)
					{
						i++;									//moves cursor to the right
						ptBeg[2] += detsegs.PicApertSiz();	// to intercept line
					}
				}
				if (detsegs.SelSegAndPrimUsingPoint(ptBeg) == 0) {break;}
				if (!detsegs.DetPrim()->Is(CPrim::PRIM_LINE)) {break;}
				dAng = 0.;
				vVwPlnNorm = view::GetDirection();
									
				(static_cast<CPrimLine*>(detsegs.DetPrim()))->GetPts(ptBeg, ptEnd);
				ptMid = Mid(ptBeg, ptEnd);							//signified by user
				if (rlpd::SelEndCapUsingPoint(ptMid, pEndCapSeg, pEndCapMark)) 
				{
					rlpd::dDiam[1] = pEndCapMark->GetDat(0);
					rlpd::GenSizeNote(ptCur, dAng, rlpd::dDiam[1]);		//if endcap line, find details and print dimension
				}
				else				//find other line
				{
					CSeg::IgnorePrim() = detsegs.DetPrim();

					CPnt4 ptView(ptBeg, 1.);
					ModelView_DoTransform(ptView);
					if (detsegs.SelSegAndPrimAtCtrlPt(ptView) == 0) {break;}
					if (!detsegs.DetPrim()->Is(CPrim::PRIM_LINE)) {break;}
							
					ptBeg = detsegs.DetPt();
					(static_cast<CPrimLine*>(detsegs.DetPrim()))->GetPts(ptBeg, ptEnd);
					ptMid = Mid(ptBeg, ptEnd);
					if (rlpd::SelEndCapUsingPoint(ptMid, pEndCapSeg, pEndCapMark))	//if endcap line, find details and print dimension
					{
						rlpd::dDiam[1] = pEndCapMark->GetDat(0);
						rlpd::GenSizeNote(ptCur, dAng, rlpd::dDiam[1]);
					}
				}
				break;
			}
				
			case IDM_ESCAPE:
				if(wPrvKeyDwn != 0) 
					app.RubberBandingDisable();
				else
					pDoc->DoEditPrimitiveEscape();

				app.ModeLineUnhighlightOp(wPrvKeyDwn);
				rlpd::bContSec = false;
				pEndCapSeg = 0; 
				pEndCapMark = 0;
				
			default:
				lResult = !0;
		}
		if(lResult == 0)
			return (lResult);
	}
	return(CallWindowProc(app.GetMainWndProc(), hwnd, anMsg, wParam, lParam));
}	

///<summary>Sets the diameter of round ductwork.</summary>
void rlpd::SetOptions(double* dSecDiam)
{	
	CPegDoc *pDoc = CPegDoc::GetDoc();

	rlpd::dSecDiam = *dSecDiam;

	pDoc->SetOptionsFocus(IDC_DIAMETER);
	pDoc->SetOptionsPage(CPegDoc::TAB_OPEN_RLPD);
	if(::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_TABS_MAIN), app.GetSafeHwnd(), DlgProcSetup))
	{
		*dSecDiam = Max(0., rlpd::dSecDiam);
	}
}

///<summary>
///Locates and returns the first two lines that have an endpoint which coincides
///with the endpoints of the specified line.
///</summary>
// Notes:	The lines are oriented so direction vectors defined
//			by each point to the specified line. 
//			No check is made to see if lines are colinear.
//			Ln1 is always to the left of the direction vector
//			defined by ln2.
//			Lines are normal to to test line (and therefore
//			parallel to each other) if acceptance angle is 0.
// Returns: true	qualifying lines located
//			false	otherwise
// Parameters:	pTestLine	pointer to test line
//				dAccAng 	angle tolerance for qualifying line (radians)
//				pLeftLine	segment identifier for left line
//				lnLeft		endpoints of left line
//				pRightLine	segment identifier for right line
//				lnRight 	endpoints of right line
bool rlpd::Fnd2LnsGivLn(CPrimLine* pTestLine, double dAccAng, CPrim*& pLeftLine, CLine* lnLeft, CPrim*& pRightLine, CLine* lnRight)
{
	CSeg*		pSeg;
	CPrim*		pPrim;
	POSITION	posPrim;
	
	CPnt		ptBeg, ptEnd;
	CPnt		ptTestLineBeg, ptTestLineEnd;
	
	double		dLnAng, dLnAng0;

	pLeftLine = 0;
	int iRel1 = 0;
	
	pTestLine->GetPts(ptTestLineBeg, ptTestLineEnd);
	
	dLnAng0 = fmod((double)pTestLine->GetAngAboutZAx(), (double)PI);

	POSITION pos = detsegs.GetTailPosition();
	while(pos != 0)
	{
		pSeg = (CSeg*) detsegs.GetPrev(pos);
		
		posPrim = pSeg->GetHeadPosition();
		while(posPrim != 0)
		{
			pPrim = pSeg->GetNext(posPrim);
			if(pPrim == pTestLine || !pPrim->Is(CPrim::PRIM_LINE))
				continue;
			
			static_cast<CPrimLine*>(pPrim)->GetPts(ptBeg, ptEnd);
			if(ptBeg == ptTestLineBeg || ptBeg == ptTestLineEnd)
			{	// Exchange points
				CPnt ptTmp = ptBeg; 
				ptBeg = ptEnd; 
				ptEnd = ptTmp;
			} 
			else if(ptEnd != ptTestLineBeg && ptEnd != ptTestLineEnd)	//	No endpoint coincides with one of the test line endpoints
				continue;

			dLnAng = fmod((double)line::GetAngAboutZAx(CLine(ptBeg, ptEnd)), (double)PI);
			if(fabs(fabs(dLnAng0 - dLnAng) - HALF_PI) <= dAccAng) 
			{
				if(pLeftLine == 0)									// No qualifiers yet
				{
					iRel1 = CLine(ptTestLineBeg, ptTestLineEnd).DirRelOfPt(ptBeg);
					pLeftLine = pPrim;
					(*lnLeft)(ptBeg, ptEnd);
				}
				else
				{
					if(iRel1 == CLine(ptTestLineBeg, ptTestLineEnd).DirRelOfPt(ptBeg))	// Both lines are on the same side of test line
					{
						pRightLine = pPrim;
						(*lnRight)(ptBeg, ptEnd);
						if(lnRight->DirRelOfPt((*lnLeft)[0]) != 1) 
						{
							pRightLine = pLeftLine;
							*lnRight = *lnLeft;
							pLeftLine = pPrim;
							(*lnLeft)(ptBeg, ptEnd);
						}
						return true;
					}
				}
			}
		}
	}
	return false;
}

///<summary>Generates a low pressure radial elbow.</summary>
// Returns: 0 sections are parallel
//			1 elbow generated (left turn)
//			- 1 elbow generated (right turn)
int rlpd::GenElbow(double adWid, double adDep, CLine* pLns)
{	
	CPegDoc* pDoc = CPegDoc::GetDoc();

	static double dSlo = 0.;
	  
	int iDir = pLns[0].DirRelOfPt(pLns[2][1]);

	if(iDir == 0)
		// No turn indicated
		return 0;
	
	CPnt* pt = new CPnt[6];

	double	dDepIn;
	double	dDepOut;
	double	dRel[2];
	double	dSecDep;
	double	dSecWid;
	double	dTanAng;
	
	CPnt	ptIn[2];
	CPnt	ptOut[2];
	
	int 	iIn[2];
	int 	iOut[2];
	
	if (iDir == 1)
	{	// Turn to left
		iIn[0] = 0;
		iOut[0] = 1;
	}
	else
	{	// Turn to right
		iIn[0] = 1; 
		iOut[0] = 0;
	}

	iIn[1] = iIn[0] + 2;
	iOut[1] = iOut[0] + 2;
	
	if (pgEndCapSeg != 0) 
	{
		CPrimMark* pMark = static_cast<CPrimMark*>(pgEndCapSeg->GetHead());
		dSecWid = pMark->GetDat(0);
		dSecDep = pMark->GetDat(1);
		
		pDoc->AnyLayerRemove(pgEndCapSeg);
		detsegs.Remove(pgEndCapSeg);
		trapsegs.Remove(pgEndCapSeg);
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pgEndCapSeg);
	
		pgEndCapSeg->RemovePrims();
		delete pgEndCapSeg;
	}
	else
	{
		dSecWid = adWid;
		dSecDep = adDep;
	}
	double dMinWid = Min(dSecWid, adWid) * (rlpd::dInsideRadiusFactor - .5);
	double dMaxWid = Max(dSecWid, adWid);
	
	double dAng = line::AngleBetweenLn_xy(pLns[iIn[0]], pLns[iIn[1]]);
	
	dTanAng = tan((PI - dAng) * .5);
	dDepIn = dMinWid / dTanAng;
	dDepOut = (dMinWid + dMaxWid) / dTanAng;

	ptIn[0] = pLns[iIn[0]].ProjToBegPt(dDepIn);
	ptIn[1] = pLns[iIn[1]].ProjToEndPt(dDepIn);
	
	ptOut[0] = pLns[iOut[0]].ProjToBegPt(dDepOut);
	ptOut[1] = pLns[iOut[1]].ProjToEndPt(dDepOut);

	pt[0] = pLns[iIn[0]].ProjPt(ptOut[0]);
	
	line::RelOfPtToEndPts(pLns[iIn[0]], pt[0], dRel[0]);
	line::RelOfPtToEndPts(pLns[iIn[0]], ptIn[0], dRel[1]);
	
	if(dRel[1] < dRel[0]) 
		pt[0] = ptIn[0];
	
	pt[1] = pLns[iIn[1]].ProjPt(ptOut[1]);
	
	line::RelOfPtToEndPts(pLns[iIn[1]], pt[1], dRel[0]);
	line::RelOfPtToEndPts(pLns[iIn[1]], ptIn[1], dRel[1]);
	
	if(dRel[1] > dRel[0]) 
		pt[1] = ptIn[1];
	
	pt[2] = pLns[iOut[0]].ProjPt(pt[0]);
	pt[3] = pLns[iOut[1]].ProjPt(pt[1]);
	
	pLns[iIn[0]][1] = pt[0];
	pLns[iOut[0]][1] = pt[2];
	
	rlpd::GenSection(dSecWid, &pLns[0]);
	
	CSeg* pSeg = new CSeg;
	
	CPnt ptCentIn;
	CPnt ptCentOut;
	CVec vMajAx;
	CVec vMinAx;
	
	line::ProjPtFrom_xy(CLine(ptIn[0], pLns[iIn[1]][0]), 0., (double) iDir * dMinWid, &ptCentIn);
	line::ProjPtFrom_xy(CLine(ptOut[0], pLns[iOut[1]][0]), 0., (double) iDir * (dMinWid + dMaxWid), &ptCentOut);
	
	rlpd::GenEndCap(pt[0], pt[2], dSecWid, dSecDep, pSeg);
	
	if(pt[0] != ptIn[0]) 
		pSeg->AddTail(new CPrimLine(pt[0], ptIn[0]));
	if(pt[2] != ptOut[0]) 
		pSeg->AddTail(new CPrimLine(pt[2], ptOut[0]));
	if(iDir == - 1) 
	{
		ptIn[0] = ptIn[1];
		ptOut[0] = ptOut[1];
	}
	  
	if(dMinWid > DBL_EPSILON)
	{
		vMajAx = CVec(ptCentIn, ptIn[0]);
		vMinAx = CVec(ptCentIn, Pnt_RotAboutArbPtAndAx(ptIn[0], ptCentIn, ZDIR, HALF_PI));
		pSeg->AddTail(new CPrimArc(ptCentIn, vMajAx, vMinAx, dAng));
	}
	vMajAx = CVec(ptCentOut, ptOut[0]);
	vMinAx = CVec(ptCentOut, Pnt_RotAboutArbPtAndAx(ptOut[0], ptCentOut, ZDIR, HALF_PI));
	pSeg->AddTail(new CPrimArc(ptCentOut, vMajAx, vMinAx, dAng));

	if(pt[1] != ptIn[1]) 
		pSeg->AddTail(new CPrimLine(ptIn[1], pt[1]));
	if(pt[3] != ptOut[1]) 
		pSeg->AddTail(new CPrimLine(ptOut[1], pt[3]));
	
	rlpd::GenEndCap(pt[1], pt[3], adWid, adDep, pSeg);
	
	pDoc->WorkLayerAddTail(pSeg); 
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);

	pLns[iIn[1]][0] = pt[1];
	pLns[iOut[1]][0] = pt[3];
	
	delete [] pt;

	return (iDir);
}

void rlpd::GenSizeNote(CPnt arPt, double adAng, double adDiam)
{	// Generates text segment representing width and depth of a piece of duct.

	CPegDoc* pDoc = CPegDoc::GetDoc();
	CPegView* pView = CPegView::GetActiveView();
	CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();

	CVec vXAx(.06, 0., 0.);
	CVec vYAx(0., .1, 0.);
	
	double dSinAng = sin(adAng);
	double dCosAng = cos(adAng);
	
	vXAx.RotAboutZAx(dSinAng, dCosAng);
	vYAx.RotAboutZAx(dSinAng, dCosAng);
	CRefSys rs(arPt, vXAx, vYAx);

	char pSize[16];
	char* p;

	UnitsString_FormatLength(pSize, max(app.GetUnits(), Inches), adDiam, 8, 0);
	p = string_TrimLeadingSpace(pSize);	
	
	int iPrimState = pstate.Save();
	
	pstate.SetPenColor(2);
	
	CFontDef fd;
	pstate.GetFontDef(fd);
	fd.TextHorAlignSet(CFontDef::HOR_ALIGN_CENTER);
	fd.TextVerAlignSet(CFontDef::VER_ALIGN_MIDDLE);
		
	CCharCellDef ccd;
	pstate.GetCharCellDef(ccd);
	ccd.TextRotAngSet(0.);
	pstate.SetCharCellDef(ccd);

	CSeg* pSeg = new CSeg(new CPrimText(fd, rs, p));
	pDoc->WorkLayerAddTail(pSeg);
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);

	pstate.Restore(pDC, iPrimState);
}

///<summary>Generates round section using a set of parallel lines.</summary>
// Parameters:	dSecDiam diameter of sections
//				pLns	(in) set of parallel lines produced using before
//							 transition dimensions
//						(out) set of parallel lines produced using after
//							  transition dimensions
bool rlpd::GenSection(double dSecDiam, CLine* pLns) 
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	double dSecLen = pLns[0].Length();

	if(dSecLen <= DBL_EPSILON) 
		return false;

	CPnt ptLeftBeg(pLns[0][0]);
	CPnt ptLeftEnd(pLns[0][1]);
	CPnt ptRightBeg(pLns[1][0]);
	CPnt ptRightEnd(pLns[1][1]);
	
	CSeg* pSeg = new CSeg;
	
	rlpd::GenEndCap(ptLeftBeg, ptRightBeg, dSecDiam, dSecDiam, pSeg);
	pSeg->AddTail(new CPrimLine(pstate.PenColor(), pstate.PenStyle(), pLns[0]));
	rlpd::GenEndCap(ptLeftEnd, ptRightEnd, dSecDiam, dSecDiam, pSeg);
	pSeg->AddTail(new CPrimLine(pstate.PenColor(), pstate.PenStyle(), pLns[1]));
	if(rlpd::bGenCenterline)
	{
		ptRightBeg = Pnt_ProjPtTo(ptRightBeg, ptLeftBeg, dSecDiam / 2);		//		find points
		ptRightEnd = Pnt_ProjPtTo(ptRightEnd, ptLeftEnd, dSecDiam / 2);
		
		ptRightBeg = Pnt_ProjPtTo(ptRightBeg, ptRightEnd, -dSecDiam / 2);
		ptRightEnd = Pnt_ProjPtTo(ptRightEnd, ptRightBeg, -dSecDiam / 2);
		
		pSeg->AddTail(new CPrimLine(1, 10, ptRightBeg, ptRightEnd));
	}
	pDoc->WorkLayerAddTail(pSeg);
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
	
	return true;
}

///<summary>Picks an end-cap if within specified pick aperture.</summary>
// Notes:	Only check for actual end-cap marker is by attributes.
//			No error processing for invalid width or depth values.
//			Segment data contains whatever primative follows marker 
//			(hopefully this is associated end-cap line).
bool rlpd::SelEndCapUsingPoint(const CPnt& pt, CSeg*& pSeg, CPrimMark*& pMark)
{	
	CPnt ptProj;

	CPnt4 ptView(pt, 1.);
	ModelView_DoTransform(ptView);
	
	double dTol = detsegs.PicApertSiz();
	
	POSITION pos = detsegs.GetHeadPosition();
	while(pos != 0)
	{
		pSeg = detsegs.GetNext(pos);

		POSITION posPrim = pSeg->GetHeadPosition();
		while(posPrim != 0)
		{
			CPrim* pPrim = pSeg->GetNext(posPrim);
			if(pPrim->Is(CPrim::PRIM_MARK))
			{
				pMark = static_cast<CPrimMark*>(pPrim);
				if(pMark->PenColor() == 15 && pMark->MarkStyle() == 8 && pMark->SelUsingPoint(ptView, dTol, ptProj))
				{
					return true;
				}
			}
		}
	}
	pSeg = 0; 
	pMark = 0;
	return false;
}

///<summary>Generates an end-cap.</summary>
// Notes:	End-caps are segments containing a line and a marker.  The line 
//			defines the orientation of the end-cap.  The marker contains
//			information about the cross-section (width, depth)
//			and optionally a number which might be used for something like cfm.
// Parameters:	pt1 	first endpoint of end-cap
//				pt2 	second endpoint of end-cap
//				adWid	width data
//				adDep	depth data
//				pSeg
void rlpd::GenEndCap(const CPnt& pt1, const CPnt& pt2, double dWid, double dDep, CSeg* pSeg)
{
	CPnt ptMid = Mid(pt1, pt2);
	
	double d[] = {dWid, dDep};
	
	CPrimMark* pPrimMark = new CPrimMark(15, 8, ptMid);
	pPrimMark->SetDat(2, d);
	pSeg->AddTail(pPrimMark);

	pSeg->AddTail(new CPrimLine(pt1, pt2));
}

///<summary>Generates transition at the using at set of parallel lines.</summary>
// Parameters:	eEnd	endpoints where transition occurs
//				aiJus	justification
//							0 centered
//							> 0 taper to right
//							< 0 taper to left
//				adSlo	slope of sections
//				adWid	width of sections
//				adDep	depth of sections
//				pLns	(in) set of parallel lines produced using before
//							 transition dimensions
//						(out) set of parallel lines produced using after
//							  transition dimensions
int rlpd::GenTransition(rlpd::ETransition eEnd, int* aiJus, double* adSlo, double* adWid, double* adDep, CLine* pLns) 
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CPnt ptLeftBeg(pLns[0][0]);
	CPnt ptLeftEnd(pLns[0][1]);
	CPnt ptRightBeg(pLns[1][0]);
	CPnt ptRightEnd(pLns[1][1]);

	double dDepChg;
	double dTransLen;
	double dWidChg;
	
	int iResult = 1;
	int I0 = 0;

	double dSecLen = pLns[0].Length();

	if(dSecLen <= DBL_EPSILON) 
		return false;

	CSeg* pSeg = new CSeg;
	
	if(eEnd == Begin || eEnd == Both)							// Transition occurs at beginning
	{
		dTransLen = rlpd::TransitionLen(aiJus[0], adSlo[0], adWid, adDep, &dWidChg, &dDepChg);
		if(dSecLen < dTransLen) 
			{dTransLen = dSecLen; iResult = 2;}
		dSecLen -= dTransLen;
		if(aiJus[0] == 0) 
		{
			line::ProjPtFrom_xy(CLine(ptLeftBeg, ptLeftEnd), dTransLen, dWidChg, &ptLeftBeg);
			line::ProjPtFrom_xy(CLine(ptRightBeg, ptRightEnd), dTransLen, - dWidChg, &ptRightBeg);
		}
		else if(aiJus[0] > 0) 
		{
			ptLeftBeg = Pnt_ProjPtTo_xy(ptLeftBeg, ptLeftEnd, dTransLen);
			line::ProjPtFrom_xy(CLine(ptRightBeg, ptRightEnd), dTransLen, - dWidChg, &ptRightBeg);
		}
		else
		{
			line::ProjPtFrom_xy(CLine(ptLeftBeg, ptLeftEnd), dTransLen, dWidChg, &ptLeftBeg);
			ptRightBeg = Pnt_ProjPtTo_xy(ptRightBeg, ptRightEnd, dTransLen);
		}
		rlpd::GenEndCap(pLns[0][0], pLns[1][0], adWid[0], adDep[0], pSeg);
		pSeg->AddTail(new CPrimLine(pLns[1][0], ptRightBeg));
		rlpd::GenEndCap(ptRightBeg, ptLeftBeg, adWid[1], adDep[1], pSeg);
		pSeg->AddTail(new CPrimLine(ptLeftBeg, pLns[0][0]));
		
		pLns[0][0] = ptLeftBeg;
		pLns[1][0] = ptRightBeg;
		ptLeftEnd = CLine(ptLeftEnd, ptRightEnd).ProjPt(ptLeftBeg);
		ptRightEnd = CLine(ptLeftEnd, ptRightEnd).ProjPt(ptRightBeg);
		pLns[0][1] = ptLeftEnd;
		pLns[1][1] = ptRightEnd;
		if (dSecLen <= DBL_EPSILON) 
			eEnd = None;
		I0 = 1;
	}
	if(eEnd == End || eEnd == Both)										
	{	// Transition occurs at end
		CPnt pt;
		dTransLen = rlpd::TransitionLen(aiJus[I0], adSlo[I0], &adWid[I0], &adDep[I0], &dWidChg, &dDepChg);
		if(dSecLen < dTransLen) 
		{
			iResult = 4;
			dTransLen = dSecLen;
		}
		ptLeftEnd = Pnt_ProjPtTo_xy(ptLeftEnd, ptLeftBeg, dTransLen);
		ptRightEnd = Pnt_ProjPtTo_xy(ptRightEnd, ptRightBeg, dTransLen);
		if(aiJus[I0] == 0) 
		{
			line::ProjPtFrom_xy(CLine(ptLeftBeg, ptLeftEnd), dSecLen, dWidChg, &pt);
			pLns[0][1] = pt;
			line::ProjPtFrom_xy(CLine(ptRightBeg, ptRightEnd), dSecLen, - dWidChg, &pt);
			pLns[1][1] = pt;
		}
		else if(aiJus[I0] > 0) 
		{
			line::ProjPtFrom_xy(CLine(ptRightBeg, ptRightEnd), dSecLen, - dWidChg, &pt);
			pLns[1][1] = pt;
		}
		else
		{
			line::ProjPtFrom_xy(CLine(ptLeftBeg, ptLeftEnd), dSecLen, dWidChg, &pt);
			pLns[0][1] = pt;
		}
		pLns[0][0] = CLine(ptLeftEnd, ptRightEnd).ProjPt(pLns[0][1]);
		pLns[1][0] = CLine(ptLeftEnd, ptRightEnd).ProjPt(pLns[1][1]);
		dSecLen = dSecLen - dTransLen;
	}
	if(dSecLen > DBL_EPSILON)											// Produce straight section
	{
		rlpd::GenEndCap(ptLeftBeg, ptRightBeg, adWid[I0], adDep[I0], pSeg);
		pSeg->AddTail(new CPrimLine(ptRightBeg, ptRightEnd));
		rlpd::GenEndCap(ptRightEnd, ptLeftEnd, adWid[I0], adDep[I0], pSeg);
		pSeg->AddTail(new CPrimLine(ptLeftEnd, ptLeftBeg));
	}
	if(eEnd == End || eEnd == Both)			 // Produce end transition
	{
		rlpd::GenEndCap(ptLeftEnd, ptRightEnd, adWid[I0], adDep[I0], pSeg);
		pSeg->AddTail(new CPrimLine(ptRightEnd, pLns[1][1]));
		rlpd::GenEndCap(pLns[1][1], pLns[0][1], adWid[I0 + 1], adDep[I0 + 1], pSeg);
		pSeg->AddTail(new CPrimLine(pLns[0][1], ptLeftEnd));
	}
	if(pSeg->IsEmpty())
		delete pSeg;
	else
	{
		pDoc->WorkLayerAddTail(pSeg);
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
	}		 
	return (iResult);
}

double rlpd::TransitionLen(
	int aiJus,		// justification
						//		0 centered
						//		> 0 taper to right
						//		< 0 taper to left
	double	adSlo,		// slope of the section sides
	double* adWid,		// width of sections
	double* adDep,		// depth of sections
	double* adWidChg,	// change in width
	double* adDepChg)	// change in depth

// Returns:

{
	double dLen;
	
	*adWidChg = adWid[1] - adWid[0];								// Determine change in width
	*adDepChg = adDep[1] - adDep[0];								// Determine change in depth
	
	dLen = Max(fabs(*adWidChg), fabs(*adDepChg)) * adSlo;		// Determine length required for transition
	if(aiJus == 0) 
	{
		*adWidChg *= .5;
		dLen *= .5;
	}
	return (dLen);
}