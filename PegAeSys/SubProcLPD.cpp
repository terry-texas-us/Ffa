#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "ParLines.h"
#include "SubProcLPD.h"
#include "UnitsString.h"
#include "UserAxis.h"

namespace lpd
{
	EJust	eJust = Center;
	EElbow	eElbow = Mittered;		// elbow type (0 Mittered 1 Radial)
	bool	bGenTurnVanes = true;	// turning vanes generation flag
	double	dSecWid;
	double	dSecDep;
	double	dInsRadElFac = 1.5;		// inside radius elbow factor
	bool	bContSec = false;
	CPnt	pt[5];
	CSeg*	pEndCapSeg = 0;

	double dDep[3] = {.0625, .0625, .0625};
	double dWid[3] = {.125, .125, .125};
}

using namespace lpd;

LRESULT CALLBACK SubProcLPD(HWND hwnd, UINT anMsg, WPARAM wParam, LPARAM lParam)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

//	lpd::dDep[0] = lpd::dDep[1] = lpd::dDep[2] = .0625;
//	lpd::dWid[0] = lpd::dWid[1] = lpd::dWid[2] = .125;

//	static double dDep[3] = {.0625, .0625, .0625};
//	static double dWid[3] = {.125, .125, .125};

	static WORD wPrvKeyDwn = 0;
	static CPnt ptPrv;
	static CLine lnPar[4];
	static CSeg* pEndCapSeg = 0; 
	static CPrimMark* pEndCapMark = 0;
	static int iEndCapId = 0;		

	CPrim*	pLeftLine;
	CPrim*	pRightLine;
	CLine	lnLead; 
	int 	iRet = 0;
	CLine	lnCap;

	double	dAng;
	int 	iJus[2];
	
	double	dSlo[2] = {4., 4.};
	
	if(anMsg == WM_COMMAND)
	{
		long lResult = 0;
		CPnt ptCur = app.CursorPosGet();
		
		switch(LOWORD(wParam))
		{
			// a test for non-zero z used to result in error message if 0-9 
			case IDM_RETURN:
				if (wPrvKeyDwn != 0)
				{
					app.RubberBandingDisable();
					app.ModeLineUnhighlightOp(wPrvKeyDwn);
					lpd::bContSec = false;
					pEndCapSeg = 0; 
					pEndCapMark = 0;
				}
				ptPrv = ptCur;
				break;
				
            case ID_HELP_KEY:
			{
                HtmlHelp(hwnd, app.GetAppPath() + "\\PegAeSys.chm::/LowPressureDuct.htm", HH_DISPLAY_TOPIC, NULL);
				return 0;
			}

			case ID_OP0:
				lpd::SetOptions(&dWid[1], &dDep[1]);
				break;
				
			case ID_OP1:	// Search for an endcap in proximity of current location
				if(lpd::SelEndCapUsingPoint(ptCur, pEndCapSeg, pEndCapMark)) 
				{	// Endcap located
					char szLen[32];
					char szWid[32];
					
					ptPrv = pEndCapMark->GetPt();
					dWid[1] = pEndCapMark->GetDat(0);
					dDep[1] = pEndCapMark->GetDat(1);
					lpd::bContSec = false;
					
					iEndCapId = (wPrvKeyDwn == 0) ? 1 : - 1; // 1 (start) and -1 (end)

					CString strMes("Cross sectional dimension (Width by Depth) is ");
					UnitsString_FormatLength(szLen, max(app.GetUnits(), Inches), dWid[1], 12, 2);
					strMes += szLen;
					UnitsString_FormatLength(szWid, max(app.GetUnits(), Inches), dDep[1], 12, 2);
					strMes += " by ";
					strMes += szWid;
					msgInformation(strMes);
					app.CursorPosSet(ptPrv);
				}
				break;
				
			case ID_OP2:
				if(wPrvKeyDwn != 0) 
					app.RubberBandingDisable();
				
				if(pEndCapSeg == 0)
				{	// No end-cap consideration
					if(wPrvKeyDwn == ID_OP2) 
					{
						ptCur = UserAxis::UserAxisSnapLn(ptPrv, ptCur);
						lnPar[0] = lnPar[2]; 
						lnPar[1] = lnPar[3];
						lnLead(ptPrv, ptCur);
						if(ParLines_GenPts(.5, dWid[1], lnLead, &lnPar[2]))
						{	// Lead line vector is not null
							if(lpd::bContSec) 
							{
								if(ParLines_CleanCorners(&lnPar[0], &lnPar[2])) 
								{
									lpd::GenElbow(dWid[1], dDep[1], lnPar);
									
									if(!lpd::bContSec) 
										return 0;
								}
								else
								{
									ptPrv = Mid(lnPar[0][0], lnPar[1][0]);
									lnLead(ptPrv, ptCur);
									
									ParLines_GenPts(.5, dWid[1], lnLead, &lnPar[2]);
									
									pDoc->AnyLayerRemove(lpd::pEndCapSeg);
									detsegs.Remove(lpd::pEndCapSeg);
									trapsegs.Remove(lpd::pEndCapSeg);
									pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, lpd::pEndCapSeg);
									
									lpd::pEndCapSeg->RemovePrims();
									delete lpd::pEndCapSeg;
								}
							}
							lpd::GenSection(dWid[1], dDep[1], &lnPar[2]);
							lpd::bContSec = true;
							lpd::pEndCapSeg = pDoc->WorkLayerGetTail();
						}
						else			
							// Zero length section specified
							msgInformation(0);
					}
					else if (wPrvKeyDwn == ID_OP3) 
					{
						ptCur = UserAxis::UserAxisSnapLn(ptPrv, ptCur);
						lnLead(ptPrv, ptCur);
						if (ParLines_GenPts(.5, dWid[0], lnLead, &lnPar[2]))
						{	// Lead line vector is not null
							lpd::GenTransition(Begin, iJus, dSlo, dWid, dDep, &lnPar[2]);
							ptCur = Mid(lnPar[2][1], lnPar[3][1]);
							lpd::bContSec = true;
							lpd::pEndCapSeg = 0;
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
						lpd::bContSec = lpd::Fnd2LnsGivLn(pPrimCapLine, RADIAN, pLeftLine, &lnPar[2], pRightLine, &lnPar[3]);
						lpd::pEndCapSeg = pEndCapSeg;
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
				if (wPrvKeyDwn != 0) 
					app.RubberBandingDisable();
				dWid[2] = dWid[1]; 
				dDep[2] = dDep[1];
				lpd::SetOptions(&dWid[2], &dDep[2]);
				iJus[1] = lpd::eJust;
				iJus[1] *= 4; //??
				if (pEndCapMark == 0)				 // No end-cap consideration
				{
					if (wPrvKeyDwn == 0) 			// Beginning initial section
					{
						dWid[0] = dWid[1]; 
						dDep[0] = dDep[1]; 
						iJus[0] = iJus[1];
						dWid[1] = dWid[2]; 
						dDep[1] = dDep[2];
						wPrvKeyDwn = ID_OP3;
					}
					else if (wPrvKeyDwn == ID_OP2)
					{
						ptCur = UserAxis::UserAxisSnapLn(ptPrv, ptCur); // Constrain point to user axis
						lnPar[0] = lnPar[2]; lnPar[1] = lnPar[3];
						lnLead(ptPrv, ptCur);
						if (ParLines_GenPts(.5, dWid[1], lnLead, &lnPar[2]))	// Lead line vector is not nu
						{
							if (lpd::bContSec) 
							{
								if (ParLines_CleanCorners(&lnPar[0], &lnPar[2])) 
								{
									lpd::GenElbow(dWid[1], dDep[1], lnPar);
									if (! lpd::bContSec) 
										return 0;
								}
								else
								{
									ptPrv = Mid(lnPar[0][0], lnPar[1][0]);
									lnLead(ptPrv, ptCur);
									ParLines_GenPts(.5, dWid[1], lnLead, &lnPar[2]);
									pDoc->AnyLayerRemove(lpd::pEndCapSeg);
									detsegs.Remove(lpd::pEndCapSeg);
									trapsegs.Remove(lpd::pEndCapSeg);
									pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, lpd::pEndCapSeg);
									lpd::pEndCapSeg->RemovePrims();
									delete lpd::pEndCapSeg;
								}
							}
							lpd::GenTransition(End, &iJus[1], dSlo, &dWid[1], &dDep[1], &lnPar[2]);
							dWid[1] = dWid[2]; 
							dDep[1] = dDep[2];
							ptCur = Mid(lnPar[2][1], lnPar[3][1]);
							lpd::bContSec = false;
							lpd::pEndCapSeg = 0;
							wPrvKeyDwn = ID_OP2;
						}
						else							 // Zero length section specified
							msgInformation(0);
					}
					else if (wPrvKeyDwn == ID_OP3) 
					{
						ptCur = UserAxis::UserAxisSnapLn(ptPrv, ptCur); // Constrain point to user axis
						lnPar[0] = lnPar[2]; lnPar[1] = lnPar[3];
						lnLead(ptPrv, ptCur);
						if (ParLines_GenPts(.5, dWid[0], lnLead, &lnPar[2]))	// Lead line vector is not nu
						{
							lpd::GenTransition(Both, iJus, dSlo, dWid, dDep, &lnPar[2]);
							dWid[1] = dWid[2]; 
							dDep[1] = dDep[2];
							lpd::bContSec = false;
							lpd::pEndCapSeg = 0;
							wPrvKeyDwn = ID_OP2;
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
						lpd::bContSec = lpd::Fnd2LnsGivLn(pPrimCapLine, RADIAN, pLeftLine, &lnPar[2], pRightLine, &lnPar[3]);
						lpd::pEndCapSeg = pEndCapSeg;
					}
					pEndCapSeg = 0; 
					pEndCapMark = 0;
					wPrvKeyDwn = ID_OP2;
				}
				ptPrv = ptCur;
				app.CursorPosSet(ptPrv);
				app.RubberBandingStartAtEnable(ptPrv, Lines);
				break;
	  
			case ID_OP4:
				if (wPrvKeyDwn != 0) 
					app.RubberBandingDisable();
				if (lpd::GenTap(ptPrv, ptCur, dWid[1], dDep[1], lnPar)) 
				{
					wPrvKeyDwn = 0;
					lpd::bContSec = false;
					pEndCapSeg = 0;
					pEndCapMark = 0;
					ptPrv = ptCur;
				}
				else
					msgInformation(0);
				break;
	  
			case ID_OP5:										// Full ell takeoff
				if (wPrvKeyDwn != 0) 
					app.RubberBandingDisable();
				ptPrv = lpd::GenTakeoff(ptPrv, ptCur, &dWid[1], &dDep[1], lnPar); // Full elbow takeoff generated
				app.CursorPosSet(ptPrv);
				app.RubberBandingStartAtEnable(ptPrv, Lines);
				lpd::bContSec = false;
				wPrvKeyDwn = ID_OP2;
				break;
				
			case ID_OP6:										// Bullhead tee
				if (wPrvKeyDwn != 0) 
					app.RubberBandingDisable();
				ptPrv = lpd::GenTee(ptPrv, ptCur, &dWid[1], &dDep[1], lnPar); // Bullhead tee generated
				app.CursorPosSet(ptPrv);
				app.RubberBandingStartAtEnable(ptPrv, Lines);
				lpd::bContSec = false;
				wPrvKeyDwn = ID_OP2;
				break;
				
			case ID_OP7:										//	Search for an endcap in proximity of current location
				// dialog to "Select direction", 'Up.Down.'
				if (iRet >= 0) 
				{
					if (pEndCapMark == 0)			  // No end-cap consideration
					{
						if (wPrvKeyDwn == ID_OP2) 
						{
							ptCur = UserAxis::UserAxisSnapLn(ptPrv, ptCur); // Constrain point to user axis
							lnPar[0] = lnPar[2]; lnPar[1] = lnPar[3];
							lnLead(ptPrv, ptCur);
							if (ParLines_GenPts(.5, dWid[1], lnLead, &lnPar[2]))	// Lead line vector is not
							{
								if (lpd::bContSec) 
								{
									if (ParLines_CleanCorners(&lnPar[0], &lnPar[2])) 
									{
										lpd::GenElbow(dWid[1], dDep[1], lnPar);
										if (! lpd::bContSec) 
											return 0;
									}
									else
									{
										ptPrv = Mid(lnPar[0][0], lnPar[1][0]);
										lnLead(ptPrv, ptCur);
										ParLines_GenPts(.5, dWid[1], lnLead, &lnPar[2]);
										
										pDoc->AnyLayerRemove(lpd::pEndCapSeg);
										detsegs.Remove(lpd::pEndCapSeg);
										trapsegs.Remove(lpd::pEndCapSeg);
										pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, lpd::pEndCapSeg);
										
										lpd::pEndCapSeg->RemovePrims();
										delete lpd::pEndCapSeg;
									}
								}
								// lpd::GenRiseDrop(INDEX('UD', C_ANS(:1)), dWid[1], dDep[1], lnPar);
								ptCur = Mid(lnPar[2][1], lnPar[3][1]);
							}
							else						  // Zero length section specified
								msgInformation(0);
						}
						else if (wPrvKeyDwn == ID_OP3) 
						{
							ptCur = UserAxis::UserAxisSnapLn(ptPrv, ptCur); // Constrain point to user axis
							lnLead(ptPrv, ptCur);
							if (ParLines_GenPts(.5, dWid[0], lnLead, &lnPar[2]))	// Lead line vector is not
							{
								lpd::GenTransition(Begin, iJus, dSlo, dWid, dDep, &lnPar[2]);
								// lpd::GenRiseDrop(INDEX('UD', C_ANS(:1)), dWid[1], dDep[1], lnPar);
								ptCur = Mid(lnPar[2][1], lnPar[3][1]);
							}
							else						  // Zero length section specified
								msgInformation(0);
						}
					}
					else										// Work with existing end-cap
					{
						pEndCapSeg = 0; 
						pEndCapMark = 0;
					}
					lpd::bContSec = false;
					lpd::pEndCapSeg = 0;
					wPrvKeyDwn = ID_OP2;
					ptPrv = ptCur;
					app.CursorPosSet(ptPrv);
					app.RubberBandingStartAtEnable(ptPrv, Lines);
				}
				break;
				
			case ID_OP9:
				dAng = 0.;
				if (pEndCapMark != 0)				 // Work with existing end-cap
				{
					if (pEndCapMark->PenColor() == 15 && pEndCapMark->MarkStyle() == 8) 
					{
						POSITION posPrim = pEndCapSeg->Find(pEndCapMark);
						pEndCapSeg->GetNext(posPrim);
						CPrimLine* pLine = static_cast<CPrimLine*>(pEndCapSeg->GetAt(posPrim));
						dAng = fmod((double)pLine->GetAngAboutZAx(), (double)PI);
						if (dAng <= RADIAN) 
							dAng += PI;
						dAng -= HALF_PI;
					}
					pEndCapSeg = 0; 
					pEndCapMark = 0;
				}
				lpd::GenSizeNote(ptCur, dAng, dWid[1], dDep[1]);
				if (wPrvKeyDwn != 0) 
					app.RubberBandingDisable();
				wPrvKeyDwn = 0;
				lpd::bContSec = false;
				break;
				
			case IDM_ESCAPE:
				if (wPrvKeyDwn != 0) 
					app.RubberBandingDisable();
				else
					pDoc->DoEditPrimitiveEscape();

				app.ModeLineUnhighlightOp(wPrvKeyDwn);
				lpd::bContSec = false;
				pEndCapSeg = 0; 
				pEndCapMark = 0;
				
			default:
				lResult = !0;
		}
		if (lResult == 0)
			return (lResult);
	}
	return(CallWindowProc(app.GetMainWndProc(), hwnd, anMsg, wParam, lParam));
}

///<summary>
///Locates and returns the first two lines that have an endpoint which coincides with
///the endpoints of the specified line.
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
bool lpd::Fnd2LnsGivLn(CPrimLine* pTestLine, double dAccAng, CPrim*& pLeftLine, CLine* lnLeft, CPrim*& pRightLine, CLine* lnRight)
{
	CSeg*		pSeg;
	CPrim*		pPrim;
	POSITION	posPrim;
	
	CPnt		ptBeg, ptEnd;
	CLine		lnTest;
	double		dLnAng, dLnAng0;

	pLeftLine = 0;
	int iRel1 = 0;
	
	pTestLine->GetLine(lnTest);
	
	dLnAng0 = fmod((double)pTestLine->GetAngAboutZAx(), (double)PI);

	POSITION pos = detsegs.GetTailPosition();
	while (pos != 0)
	{
		pSeg = (CSeg*) detsegs.GetPrev(pos);
		
		posPrim = pSeg->GetHeadPosition();
		while (posPrim != 0)
		{
			pPrim = pSeg->GetNext(posPrim);
			if (pPrim == pTestLine || !pPrim->Is(CPrim::PRIM_LINE))
				continue;
			static_cast<CPrimLine*>(pPrim)->GetPts(ptBeg, ptEnd);
			if (ptBeg == lnTest[0] || ptBeg == lnTest[1])
			{	// Exchange points
				CPnt ptTmp = ptBeg; 
				ptBeg = ptEnd; 
				ptEnd = ptTmp;
			} 
			else if (ptEnd != lnTest[0] && ptEnd != lnTest[1])	//	No endpoint coincides with one of the test line endpoints
				continue;

			dLnAng = fmod((double)line::GetAngAboutZAx(CLine(ptBeg, ptEnd)), (double)PI);
			if (fabs(fabs(dLnAng0 - dLnAng) - HALF_PI) <= dAccAng) 
			{
				if (pLeftLine == 0)									// No qualifiers yet
				{
					iRel1 = lnTest.DirRelOfPt(ptBeg);
					pLeftLine = pPrim;
					(*lnLeft)(ptBeg, ptEnd);
				}
				else
				{
					if (iRel1 == lnTest.DirRelOfPt(ptBeg))	// Both lines are on the same side of test line
					{
						pRightLine = pPrim;
						(*lnRight)(ptBeg, ptEnd);
						if (lnRight->DirRelOfPt((*lnLeft)[0]) != 1) 
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

///<summary>Generates a low pressure mitered or radial elbow.</summary>
// Returns: 0 sections are parallel
//			1 elbow generated (left turn)
//			- 1 elbow generated (right turn)
int lpd::GenElbow(double adWid, double adDep, CLine* pLns)
{	
	CPegDoc* pDoc = CPegDoc::GetDoc();

	static double dSlo = 0.;
	  
	int iDir = pLns[0].DirRelOfPt(pLns[2][1]);

	if (iDir == 0)
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
	
	if (lpd::pEndCapSeg != 0) 
	{
		CPrimMark* pMark = static_cast<CPrimMark*>(lpd::pEndCapSeg->GetHead());
		dSecWid = pMark->GetDat(0);
		dSecDep = pMark->GetDat(1);
		
		pDoc->AnyLayerRemove(lpd::pEndCapSeg);
		detsegs.Remove(lpd::pEndCapSeg);
		trapsegs.Remove(lpd::pEndCapSeg);
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, lpd::pEndCapSeg);
	
		lpd::pEndCapSeg->RemovePrims();
		delete lpd::pEndCapSeg;
	}
	else
	{
		dSecWid = adWid;
		dSecDep = adDep;
	}
	double dMinWid = Min(dSecWid, adWid) * (lpd::dInsRadElFac - .5);
	double dMaxWid = Max(dSecWid, adWid);
	
	double dAng = line::AngleBetweenLn_xy(pLns[iIn[0]], pLns[iIn[1]]);
	
	if (lpd::eElbow == Mittered)
	{	// Mitered elbow
		dDepIn = 3. / app.GetScale();
		dDepOut = dDepIn;
	}
	else
	{
		dTanAng = tan((PI - dAng) * .5);
		dDepIn = dMinWid / dTanAng;
		dDepOut = (dMinWid + dMaxWid) / dTanAng;
	}
	ptIn[0] = pLns[iIn[0]].ProjToBegPt(dDepIn);
	ptIn[1] = pLns[iIn[1]].ProjToEndPt(dDepIn);
	
	ptOut[0] = pLns[iOut[0]].ProjToBegPt(dDepOut);
	ptOut[1] = pLns[iOut[1]].ProjToEndPt(dDepOut);

	pt[0] = pLns[iIn[0]].ProjPt(ptOut[0]);
	
	line::RelOfPtToEndPts(pLns[iIn[0]], pt[0], dRel[0]);
	line::RelOfPtToEndPts(pLns[iIn[0]], ptIn[0], dRel[1]);
	
	if (dRel[1] < dRel[0]) 
		pt[0] = ptIn[0];
	
	pt[1] = pLns[iIn[1]].ProjPt(ptOut[1]);
	
	line::RelOfPtToEndPts(pLns[iIn[1]], pt[1], dRel[0]);
	line::RelOfPtToEndPts(pLns[iIn[1]], ptIn[1], dRel[1]);
	
	if (dRel[1] > dRel[0]) 
		pt[1] = ptIn[1];
	
	pt[2] = pLns[iOut[0]].ProjPt(pt[0]);
	pt[3] = pLns[iOut[1]].ProjPt(pt[1]);
	
	pLns[iIn[0]][1] = pt[0];
	pLns[iOut[0]][1] = pt[2];
	
	lpd::GenSection(dSecWid, dSecDep, &pLns[0]);
	
	CSeg* pSeg = new CSeg;
	
	if (lpd::eElbow == Mittered)
	{	// Mitered elobw
		lpd::GenEndCap(pt[0], pt[2], dSecWid, dSecDep, pSeg);
		pSeg->AddTail(new CPrimLine(pt[0], pLns[iIn[1]][0]));
		pSeg->AddTail(new CPrimLine(pLns[iIn[1]][0], pt[1]));
		pSeg->AddTail(new CPrimLine(pt[2], pLns[iOut[1]][0]));
		pSeg->AddTail(new CPrimLine(pLns[iOut[1]][0], pt[3]));
		if (lpd::bGenTurnVanes) 
		{
			pSeg->AddTail(new CPrimLine(2, 1, pLns[iIn[1]][0], pLns[iOut[1]][0]));
		}
	}
	else
	{	// Radial elbow
		CPnt ptCentIn;
		CPnt ptCentOut;
		CVec vMajAx;
		CVec vMinAx;
		
		line::ProjPtFrom_xy(CLine(ptIn[0], pLns[iIn[1]][0]), 0., (double) iDir * dMinWid, &ptCentIn);
		line::ProjPtFrom_xy(CLine(ptOut[0], pLns[iOut[1]][0]), 0., (double) iDir * (dMinWid + dMaxWid), &ptCentOut);
		
		lpd::GenEndCap(pt[0], pt[2], dSecWid, dSecDep, pSeg);
		
		if (pt[0] != ptIn[0]) 
			pSeg->AddTail(new CPrimLine(pt[0], ptIn[0]));
		if (pt[2] != ptOut[0]) 
			pSeg->AddTail(new CPrimLine(pt[2], ptOut[0]));
		if (iDir == - 1) 
		{
			ptIn[0] = ptIn[1];
			ptOut[0] = ptOut[1];
		}
	  
		if (dMinWid > DBL_EPSILON)
		{
			vMajAx = CVec(ptCentIn, ptIn[0]);
			vMinAx = CVec(ptCentIn, Pnt_RotAboutArbPtAndAx(ptIn[0], ptCentIn, ZDIR, HALF_PI));
			pSeg->AddTail(new CPrimArc(ptCentIn, vMajAx, vMinAx, dAng));
		}
		vMajAx = CVec(ptCentOut, ptOut[0]);
		vMinAx = CVec(ptCentOut, Pnt_RotAboutArbPtAndAx(ptOut[0], ptCentOut, ZDIR, HALF_PI));
		pSeg->AddTail(new CPrimArc(ptCentOut, vMajAx, vMinAx, dAng));

		if (pt[1] != ptIn[1]) 
			pSeg->AddTail(new CPrimLine(ptIn[1], pt[1]));
		if (pt[3] != ptOut[1]) 
			pSeg->AddTail(new CPrimLine(ptOut[1], pt[3]));
	}
	lpd::GenEndCap(pt[1], pt[3], adWid, adDep, pSeg);
	
	pDoc->WorkLayerAddTail(pSeg); 
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
	pLns[iIn[1]][0] = pt[1];
	pLns[iOut[1]][0] = pt[3];
	
	delete [] pt;

	return (iDir);
}

///<summary>Generates an end-cap.</summary>
// Notes:	End-caps are segments containing a line and a marker.  The line 
//			defines the orientation of the end-cap.  The marker contains
//			information about the cross-section (width, depth)
//			and optionally a number which might be used for something like cfm.
// Parameters:	pt1 	first endpoint of end-cap
//				pt2 	second endpoint of end-cap
//				dWid	width data
//				dDep	depth data
//				pSeg
void lpd::GenEndCap(const CPnt& pt1, const CPnt& pt2, double dWid, double dDep, CSeg* pSeg)
{
	CPnt ptMid = Mid(pt1, pt2);
		
	double d[] = {dWid, dDep};
	
	CPrimMark* pPrimMark = new CPrimMark(15, 8, ptMid);
	pPrimMark->SetDat(2, d);
	pSeg->AddTail(pPrimMark);
	
	pSeg->AddTail(new CPrimLine(pt1, pt2));

}

///<summary>Generates rise or drop fitting.</summary>
// Notes:	This routine is a member of the DUCTWORK APPLICATION.
// Returns: 1 rise/drop produced without section
//			3 rise/drop produced with section
// Parameters:	acRisDrop	rise or drop indicator
//								1 rise
//								2 drop
//				adWid		width of horizontal section
//				adDep		depth of horizontal section
//				pLns		(in)  2:3 definition parallel line set
//							(out) 0:1 parallel line set for cross section
//								  2:3 parallel line set for thru section
int lpd::GenRiseDrop(char acRisDrop, double adWid, double adDep, CLine* pLns)	
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	static double	dSlo = 0.;
	
	int iResult = 1;
	double dSecWid = adWid;
	double dSecDep = adDep;
	double dSSiz = 3. / app.GetScale();
	double dSecLen = Pnt_DistanceTo_xy(pLns[2][0], pLns[2][1]);
	
	CLine ln(pLns[2][1], pLns[3][1]);
	
	if (dSecLen <= dSecDep * .5 + dSSiz) 
	{
		if (dSecLen < dSecDep * .5) 
		{
			ln[0] = pLns[2].ProjToEndPt(dSecDep * .5);
			ln[1] = pLns[3].ProjPt(ln[0]);
		}
	}
	else
	{
		pLns[2][1] = pLns[2].ProjToBegPt(dSecDep * .5 + dSSiz);
		pLns[3][1] = pLns[3].ProjPt(pLns[2][1]);
		lpd::GenSection(dSecWid, dSecDep, &pLns[2]);
		iResult = 3;
	}
	ParLines_GenPts(.5, dSecDep, ln, &pLns[0]);
	lpd::GenSection(dSecDep, dSecWid, &pLns[0]);
	ln(pLns[2][1], pLns[3][1]);
	pLns[2](pLns[1][0], pLns[0][0]);
	pLns[3](pLns[1][1], pLns[0][1]);
	lpd::GenSection(dSecWid, dSecDep, &pLns[2]);
	
	CSeg* pSeg = new CSeg;
					
	if (iResult == 3) 
	{
		pSeg->AddTail(new CPrimLine(ln[0], pLns[2][0]));
		pSeg->AddTail(new CPrimLine(ln[1], pLns[3][0]));
	}
	pSeg->AddTail(new CPrimLine(pstate.PenColor(), acRisDrop, pLns[3][0], pLns[2][1]));
	pSeg->AddTail(new CPrimLine(pstate.PenColor(), acRisDrop, pLns[2][0], pLns[3][1]));
	
	pDoc->WorkLayerAddTail(pSeg); 
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
	
	return (iResult);
}

///<summary>Generates rectangular section using a set of parallel lines.</summary>
// Parameters:	dSecWid width of section
//				dSecDep depth of sections
//				pLns	(in) set of parallel lines produced using before
//							 transition dimensions
//						(out) set of parallel lines produced using after
//							  transition dimensions
bool lpd::GenSection(double dSecWid, double dSecDep, CLine* pLns) 
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	double dSecLen = pLns[0].Length();

	if (dSecLen <= DBL_EPSILON) 
		return false;

	CSeg* pSeg = new CSeg;
	
	lpd::GenEndCap(pLns[0][0], pLns[1][0], dSecWid, dSecDep, pSeg);
	pSeg->AddTail(new CPrimLine(pstate.PenColor(), pstate.PenStyle(), pLns[0]));
	lpd::GenEndCap(pLns[0][1], pLns[1][1], dSecWid, dSecDep, pSeg);
	pSeg->AddTail(new CPrimLine(pstate.PenColor(), pstate.PenStyle(), pLns[1]));
	pDoc->WorkLayerAddTail(pSeg);
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);

	return true;
}

void lpd::GenSizeNote(CPnt arPt, double adAng, double adWid, double adDep)
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

	char pNote[48];
	char pSize[16];
	
	UnitsString_FormatLength(pSize, max(app.GetUnits(), Inches), adWid, 8, 0);
	strcpy(pNote, string_TrimLeadingSpace(pSize));
	strcat(pNote, "/");

	UnitsString_FormatLength(pSize, max(app.GetUnits(), Inches), adDep, 8, 0);
	strcat(pNote, string_TrimLeadingSpace(pSize));

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

	CSeg* pSeg = new CSeg(new CPrimText(fd, rs, pNote));
	pDoc->WorkLayerAddTail(pSeg);
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
	pstate.Restore(pDC, iPrimState);
}

CPnt lpd::GenTakeoff(CPnt arPrvPt, CPnt arCurPt, double* adWid, double* adDep, CLine* pLns)
{	// Generates a full elbow takeoff fitting.
	// Returns: Center point of end cap of exit transition.

	CPegDoc* pDoc = CPegDoc::GetDoc();

	CSeg*	pEndCapSeg;
	CPrimMark* pEndCapMark;
	CPrim*	pPrimLn1;
	CPrim*	pPrimLn2;

	CPnt	ptBeg;
	CPnt	ptEnd;
	CLine	lnCap;
	CLine	rPar[2];
	CLine	lnLead;
	
	int 	iJus[1];
	double	dDep[2];
	double	dDepChg;
	double	dDSiz;
	double	dScal;
	double	dSlo[1];
	double	dSSiz;
	double	dTransLen;
	double	dWid[2];
	double	dWidChg;
	double	dX;
	double	dY;
	int 	iDir;
	
	dWid[0] = *adWid;
	dDep[0] = *adDep;
	if (!lpd::SelEndCapUsingPoint(arCurPt, pEndCapSeg, pEndCapMark)) 
	{
		msgInformation(IDS_MSG_LPD_NO_END_CAP_LOC); 	
		return (arPrvPt);
	}
	dWid[1] = pEndCapMark->GetDat(0);										// Save end-cap width
	dDep[1] = pEndCapMark->GetDat(1);										// Save end-cap depth
	
	POSITION pos = pEndCapSeg->Find(pEndCapMark);
	pEndCapSeg->GetNext(pos);
	CPrimLine* pPrimCapLn = static_cast<CPrimLine*>(pEndCapSeg->GetAt(pos));							// Get end-cap line
	pPrimCapLn->GetLine(lnCap);
	if (!lpd::Fnd2LnsGivLn(pPrimCapLn, RADIAN, pPrimLn1, &rPar[0], pPrimLn2, &rPar[1])) 
	{
		msgWarning(IDS_MSG_LPD_NOT_DUCT_SEC);
		return (arPrvPt);
	}	 
	*adWid = dWid[0] + dWid[1];
	*adDep = Max(dDep[0], dDep[1]);
	dDSiz = Max(dWid[0], dWid[1]);								// Save maximum end-cap width for damper size
	iDir = rPar[0].DirRelOfPt(arPrvPt);

	double dEcc1 = 0.;
	double dEcc2 = 0.;
	int iOut = 0;	
	
	if (iDir == 1)												// Turn to left of lead line
		{dEcc1 = 1.; dEcc2 = dWid[0] / *adWid;}
	else if (iDir == - 1)										// Turn to right of lead line
		{dEcc2 = dWid[1] / *adWid; iOut = 1;}
	
	ptBeg = rPar[iOut][0];
	ptEnd = rPar[iOut][1];
	lnLead[0] = arPrvPt;
	lnLead[1] = rPar[iOut].ProjPt(arPrvPt);
	ParLines_GenPtsAndClean(.5, dWid[0], lnLead, pLns, lpd::bContSec);
	if (lpd::bContSec) 
		lpd::GenElbow(dWid[0], dDep[0], pLns);
	dX = rPar[iOut][1][0] - rPar[iOut][0][0];
	dY = rPar[iOut][1][1] - rPar[iOut][0][1];
	lnLead[0] = lnLead[1];
	CVec v(100. * dX, 100. * dY, 0.);
	lnLead[1] = lnLead[0] + v;
	if ((ParLines_GenPtsAndClean(dEcc1, dWid[0], lnLead, pLns, true) & 1) == 1) 
	{
		lpd::pEndCapSeg = 0;
		lpd::GenElbow(dWid[0], dDep[0], pLns);
	}
	pDoc->AnyLayerRemove(pEndCapSeg);
	detsegs.Remove(pEndCapSeg);
	trapsegs.Remove(pEndCapSeg);
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pEndCapSeg);
	
	pEndCapSeg->RemovePrims();
	delete pEndCapSeg;

	rPar[0][1] = CLine(pLns[2][0], pLns[3][0]).ProjPt(rPar[0][0]);
	rPar[1][1] = CLine(pLns[2][0], pLns[3][0]).ProjPt(rPar[1][0]);
	iJus[0] = 0;
	dSlo[0] = 0.;
	lpd::GenSection(dWid[1], dDep[1], &rPar[0]);
	dWid[0] = *adWid;
	dWid[1] = *adDep;
	lpd::SetOptions(adWid, adDep);
	iJus[0] = lpd::eJust;
	dSlo[0] = 4.;
	dWid[1] = *adWid;
	dDep[1] = *adDep;
	dScal = app.GetScale();
	dTransLen = Max(1. / dScal, lpd::TransitionLen(iJus[0], dSlo[0], dWid, dDep, &dWidChg, &dDepChg));
	lnLead[0] = rPar[iOut][1];
	lnLead[1] = lnLead.ProjToEndPt(dTransLen);
	ParLines_GenPts(dEcc2, dWid[0], lnLead, rPar);
	lpd::GenTransition(Begin, iJus, dSlo, dWid, dDep, &rPar[0]);
	if (lpd::bGenTurnVanes) 
	{
		CVec vMajAx(.01, 0., 0.);
		CVec vMinAx(0., .01, 0.);
		
		lpd::pt[2] = Pnt_ProjPtTo_xy(rPar[0][1], rPar[1][1], dEcc2 * dWid[1]);
		dSSiz  = 3. / dScal;							 // Scale seam size
		line::ProjPtFrom_xy(CLine(lpd::pt[2], rPar[1][1]), 0., dSSiz, &lpd::pt[3]);
		dDSiz = dDSiz / dWid[0] * dWid[1];
		line::ProjPtFrom_xy(CLine(lpd::pt[2], rPar[1][1]), 0., dDSiz + dSSiz, &lpd::pt[4]);
		CSeg* pSeg = new CSeg;
		pSeg->AddTail(new CPrimLine(1, pstate.PenStyle(), lnLead[0], lpd::pt[2]));
		pSeg->AddTail(new CPrimArc(1, pstate.PenStyle(), lpd::pt[3], vMajAx, vMinAx, TWOPI));
		pSeg->AddTail(new CPrimLine(1, pstate.PenStyle(), lpd::pt[3], lpd::pt[4]));
		pDoc->WorkLayerAddTail(pSeg);
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
	}
	arCurPt[0] = (rPar[0][1][0] + rPar[1][1][0]) * .5f;
	arCurPt[1] = (rPar[0][1][1] + rPar[1][1][1]) * .5f;
	return (arCurPt);
}

bool lpd::GenTap(CPnt arPrvPt, CPnt arCurPos, double adWid, double adDep, CLine* pLns)
{	// Generates rectangular tap fitting.
	CPegDoc* pDoc = CPegDoc::GetDoc();

	int 	iJus;
	double	dDep[2], dTSiz, dWid[2];
	int 	iOut;
	CPnt	rProjPt;
	CLine	lnLead;

	double	dSlo = 1.;
	if (detsegs.SelLineUsingPoint(arCurPos) == 0)
		return false;

	CPrimLine* pLine = static_cast<CPrimLine*>(detsegs.DetPrim());

	lnLead[0] = arPrvPt;
	lnLead[1] = pLine->Ln().ProjPt(arPrvPt);
	ParLines_GenPtsAndClean(.5, adWid, lnLead, pLns, lpd::bContSec);

	if(lpd::bContSec) 
		lpd::GenElbow(adWid, adDep, pLns);

	dTSiz = 9. / app.GetScale();
	iJus = lnLead.DirRelOfPt(arCurPos);
	
	if(iJus == 1) 
		iOut = 3;
	else if(iJus == - 1) 
		iOut = 2;
	else
		return false;
	
	iJus = - iJus;
	dWid[0] = adWid;
	dWid[1] = adWid + dTSiz;
	dDep[0] = adDep;
	dDep[1] = adDep;
	lpd::GenTransition(End, &iJus, &dSlo, dWid, dDep, &pLns[2]);

	if (lpd::bGenTurnVanes) 
	{
		CVec vMajAx(.01, 0., 0.);
		CVec vMinAx(0., .01, 0.);
		CPnt ptBeg = pLns[iOut].ProjToBegPt(- dTSiz / 3.);
		CPnt ptEnd = lnLead.ProjToBegPt(- dTSiz / 2.);
		
		CSeg* pSeg = new CSeg;
		pSeg->AddTail(new CPrimArc(1, pstate.PenStyle(), ptBeg, vMajAx, vMinAx, TWOPI));
		pSeg->AddTail(new CPrimLine(1, pstate.PenStyle(), ptBeg, ptEnd));
		pDoc->WorkLayerAddTail(pSeg);
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
	}

	return true;
}

///<summary>Generates a mitered bullhead tee fitting.</summary>
// Returns: Center point of end cap of exit transition.
//				arPrvPt
//				arCurPt 
//				adWid
//				adDep 
//				pLns
CPnt lpd::GenTee(CPnt arPrvPt, CPnt arCurPt, double* adWid, double* adDep, CLine* pLns)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	int 	iJus;
	double	dD1, dD2, dDep[2], dDepChg, dDir, dDSiz, dEcc, dEcc1, dEcc2, dScal, dSlo[1], dSSiz, dTransLen, dWid[2], dWidChg;
	int 	iIn, iOut;
	CSeg*	pEndCapSeg;
	CPrimMark*	pEndCapMark;
	CPrim*	pPrimLn1;
	CPrim*	pPrimLn2;
	CPnt	ptCapMrk;
	CLine	lnCap;
	CLine	rPar[2], rTmpLns[2];
	CLine lnLead;	 
	dWid[0] = *adWid;
	dDep[0] = *adDep;
	
	if (!lpd::SelEndCapUsingPoint(arCurPt, pEndCapSeg, pEndCapMark)) 
	{
		msgInformation(IDS_MSG_LPD_NO_END_CAP_LOC); 	
		return (arPrvPt);
	}	
	ptCapMrk = pEndCapMark->GetPt();
	dWid[1] = pEndCapMark->GetDat(0);								// End-cap width data
	dDep[1] = pEndCapMark->GetDat(1);								// End-cap depth data
	
	POSITION pos = pEndCapSeg->Find(pEndCapMark);
	pEndCapSeg->GetNext(pos);
	CPrimLine* pPrimCapLn = static_cast<CPrimLine*>(pEndCapSeg->GetAt(pos));							// Get end-cap line
	pPrimCapLn->GetLine(lnCap);
	
	if (!lpd::Fnd2LnsGivLn(pPrimCapLn, RADIAN, pPrimLn2, &rPar[1], pPrimLn1, &rPar[0])) 
	{
		msgWarning(IDS_MSG_LPD_NOT_DUCT_SEC);
		return (arPrvPt);
	}	 
	*adWid = dWid[0] + dWid[1];
	*adDep = Max(dDep[0], dDep[1]);
	dDSiz = Max(dWid[0], dWid[1]);					// Save maximum end-cap width for damper size
	dDir = (double) CLine(arPrvPt, ptCapMrk).DirRelOfPt(arCurPt);		// Determine direction of turn in tee fitting
	if (dDir > DBL_EPSILON) 				  // Turn to left of lead line
		{iIn = 2; iOut = 3; dEcc = 1.; dEcc1 = 0.; dEcc2 = dWid[0] / *adWid;}
	else if (dDir < - DBL_EPSILON)			   // Turn to right of lead line
		{iIn = 3; iOut = 2; dEcc = 0.; dEcc1 = 1.; dEcc2 = dWid[1] / *adWid;}
	else									  // Direction of turn could not be determined
	{
		msgInformation(IDS_MSG_LPD_UNK_TURN_DIR);
		return (arPrvPt);
	}
	lnLead[0] = arPrvPt;
	lnLead[1] = lnCap.ProjPt(arPrvPt);
	ParLines_GenPtsAndClean(.5, dWid[0], lnLead, pLns, lpd::bContSec);
	if (lpd::bContSec) 
		lpd::GenElbow(dWid[0], dDep[0], pLns);
	CVec vLead(lnLead);
	lnLead[0] = lnLead[1];
	CVec v(- 100. * dDir * vLead[1], 100. * dDir * vLead[0], 0.);
	lnLead[1] = lnLead[0] + v;
	if ((ParLines_GenPtsAndClean(dEcc, dWid[0], lnLead, pLns, true) & 1) == 1) 
	{
		lpd::pEndCapSeg = 0;
		lpd::GenElbow(dWid[0], dDep[0], pLns);
	}
	rTmpLns[0] = pLns[2];
	rTmpLns[1] = pLns[3];
	lpd::pEndCapSeg = pEndCapSeg;
	pLns[2] = rPar[1];
	pLns[3] = rPar[0];
	ParLines_GenPtsAndClean(dEcc1, dWid[1], lnLead, pLns, true);
	lpd::GenElbow(dWid[1], dDep[1], pLns);
	dD1 = Pnt_DistanceTo_xy(rTmpLns[0][0], rTmpLns[0][1]);
	dD2 = Pnt_DistanceTo_xy(pLns[2][0], pLns[2][1]);
	lnLead[0] = pLns[iIn][0];
	if (fabs(dD1 - dD2) > DBL_EPSILON + DBL_EPSILON * fabs(dD2))	  // Need a section
	{
		iJus = 0;
		dSlo[0] = 0.;
		if (dD1 > dD2) 
		{
			rTmpLns[0][1] = CLine(pLns[2][0], pLns[3][0]).ProjPt(rTmpLns[0][0]);
			rTmpLns[1][1] = CLine(pLns[2][0], pLns[3][0]).ProjPt(rTmpLns[1][0]);
			
			lpd::GenSection(dWid[0], dDep[0], &rTmpLns[0]);
		}
		else
		{
			lnLead[0] = rTmpLns[iOut - 2][0];
			pLns[2][1] = CLine(rTmpLns[0][0], rTmpLns[1][0]).ProjPt(pLns[2][0]);
			pLns[3][1] = CLine(rTmpLns[0][0], rTmpLns[1][0]).ProjPt(pLns[3][0]);
			
			lpd::GenSection(dWid[1], dDep[1], &pLns[2]);
		}
	}
	dWid[0] = *adWid;
	dDep[0] = *adDep;
	lpd::SetOptions(adWid, adDep);
	iJus = lpd::eJust;
	dSlo[0] = 4.;
	dWid[1] = *adWid;
	dDep[1] = *adDep;
	dScal = app.GetScale();
	dTransLen = Max(1. / dScal, lpd::TransitionLen(iJus, dSlo[0], dWid, dDep, &dWidChg, &dDepChg));
	lnLead[1] = lnLead.ProjToEndPt(dTransLen);
	ParLines_GenPts(dEcc2, dWid[0], lnLead, &pLns[2]);
	lpd::GenTransition(Begin, &iJus, dSlo, dWid, dDep, &pLns[2]);
	if (lpd::bGenTurnVanes) 
	{
		CVec vMajAx(.01, 0., 0.);
		CVec vMinAx(0., .01, 0.);

		lpd::pt[2] = Pnt_ProjPtTo_xy(pLns[2][1], pLns[3][1], dEcc2 * dWid[1]);
		dSSiz  = 3. / dScal;									// Scale seam size
		line::ProjPtFrom_xy(CLine(lpd::pt[2], pLns[3][1]), 0., dSSiz, &lpd::pt[3]);
		dDSiz = dDSiz / dWid[0] * dWid[1];
		line::ProjPtFrom_xy(CLine(lpd::pt[2], pLns[3][1]), 0., dDSiz + dSSiz, &lpd::pt[4]);
		CSeg* pSeg = new CSeg;
		pSeg->AddTail(new CPrimLine(1, pstate.PenStyle(), lnLead[0], lpd::pt[2]));
		pSeg->AddTail(new CPrimArc(1, pstate.PenStyle(), lpd::pt[3], vMajAx, vMinAx, TWOPI));
		pSeg->AddTail(new CPrimLine(1, pstate.PenStyle(), lpd::pt[3], lpd::pt[4]));
		pDoc->WorkLayerAddTail(pSeg);
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
	}
	arCurPt[0] = (pLns[2][1][0] + pLns[3][1][0]) * .5f;
	arCurPt[1] = (pLns[2][1][1] + pLns[3][1][1]) * .5f;
	return (arCurPt);
}

///<summary>Generates transition at the using at set of parallel lines.</summary>
//				eEnd	endpoints where transition occurs
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
int lpd::GenTransition(ETransition eEnd, int* aiJus, double* adSlo, double* adWid, double* adDep, CLine* pLns) 
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

	if (dSecLen <= DBL_EPSILON) 
		return false;

	CSeg* pSeg = new CSeg;
	
	if (eEnd == Begin || eEnd == Both)							// Transition occurs at beginning
	{
		dTransLen = lpd::TransitionLen(aiJus[0], adSlo[0], adWid, adDep, &dWidChg, &dDepChg);
		if (dSecLen < dTransLen) 
			{dTransLen = dSecLen; iResult = 2;}
		dSecLen -= dTransLen;
		if (aiJus[0] == 0) 
		{
			line::ProjPtFrom_xy(CLine(ptLeftBeg, ptLeftEnd), dTransLen, dWidChg, &ptLeftBeg);
			line::ProjPtFrom_xy(CLine(ptRightBeg, ptRightEnd), dTransLen, - dWidChg, &ptRightBeg);
		}
		else if (aiJus[0] > 0) 
		{
			ptLeftBeg = Pnt_ProjPtTo_xy(ptLeftBeg, ptLeftEnd, dTransLen);
			line::ProjPtFrom_xy(CLine(ptRightBeg, ptRightEnd), dTransLen, - dWidChg, &ptRightBeg);
		}
		else
		{
			line::ProjPtFrom_xy(CLine(ptLeftBeg, ptLeftEnd), dTransLen, dWidChg, &ptLeftBeg);
			ptRightBeg = Pnt_ProjPtTo_xy(ptRightBeg, ptRightEnd, dTransLen);
		}
		lpd::GenEndCap(pLns[0][0], pLns[1][0], adWid[0], adDep[0], pSeg);
		pSeg->AddTail(new CPrimLine(pLns[1][0], ptRightBeg));
		lpd::GenEndCap(ptRightBeg, ptLeftBeg, adWid[1], adDep[1], pSeg);
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
	if (eEnd == End || eEnd == Both)										
	{	// Transition occurs at end
		CPnt pt;
		dTransLen = lpd::TransitionLen(aiJus[I0], adSlo[I0], &adWid[I0], &adDep[I0], &dWidChg, &dDepChg);
		if (dSecLen < dTransLen) 
		{
			iResult = 4;
			dTransLen = dSecLen;
		}
		ptLeftEnd = Pnt_ProjPtTo_xy(ptLeftEnd, ptLeftBeg, dTransLen);
		ptRightEnd = Pnt_ProjPtTo_xy(ptRightEnd, ptRightBeg, dTransLen);
		if (aiJus[I0] == 0) 
		{
			line::ProjPtFrom_xy(CLine(ptLeftBeg, ptLeftEnd), dSecLen, dWidChg, &pt);
			pLns[0][1] = pt;
			line::ProjPtFrom_xy(CLine(ptRightBeg, ptRightEnd), dSecLen, - dWidChg, &pt);
			pLns[1][1] = pt;
		}
		else if (aiJus[I0] > 0) 
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
	if (dSecLen > DBL_EPSILON)											// Produce straight section
	{
		lpd::GenEndCap(ptLeftBeg, ptRightBeg, adWid[I0], adDep[I0], pSeg);
		pSeg->AddTail(new CPrimLine(ptRightBeg, ptRightEnd));
		lpd::GenEndCap(ptRightEnd, ptLeftEnd, adWid[I0], adDep[I0], pSeg);
		pSeg->AddTail(new CPrimLine(ptLeftEnd, ptLeftBeg));
	}
	if (eEnd == End || eEnd == Both)			 // Produce end transition
	{
		lpd::GenEndCap(ptLeftEnd, ptRightEnd, adWid[I0], adDep[I0], pSeg);
		pSeg->AddTail(new CPrimLine(ptRightEnd, pLns[1][1]));
		lpd::GenEndCap(pLns[1][1], pLns[0][1], adWid[I0 + 1], adDep[I0 + 1], pSeg);
		pSeg->AddTail(new CPrimLine(pLns[0][1], ptLeftEnd));
	}
	if (pSeg->IsEmpty())
		delete pSeg;
	else
	{
		pDoc->WorkLayerAddTail(pSeg);
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
	}		 
	return (iResult);
}

///<summary>Picks an end-cap if within specified pick aperture.</summary>
// Notes:	Only check for actual end-cap marker is by attributes.
//			No error processing for invalid width or depth values.
//			Segment data contains whatever primative follows marker 
//			(hopefully this is associated end-cap line).
bool lpd::SelEndCapUsingPoint(const CPnt& pt, CSeg*& pSeg, CPrimMark*& pMark)
{	
	CPnt ptProj;

	CPnt4 ptView(pt, 1.);
	ModelView_DoTransform(ptView);
	
	double dTol = detsegs.PicApertSiz();
	
	POSITION pos = detsegs.GetHeadPosition();
	while (pos != 0)
	{
		pSeg = detsegs.GetNext(pos);

		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim != 0)
		{
			CPrim* pPrim = pSeg->GetNext(posPrim);
			
			if (pPrim->Is(CPrim::PRIM_MARK))
			{
				pMark = static_cast<CPrimMark*>(pPrim);
				if (pMark->PenColor() == 15 && pMark->MarkStyle() == 8)
				{
					if (pMark->SelUsingPoint(ptView, dTol, ptProj))
					{
						return true;
					}
				}
			}
		}
	}
	pSeg = 0;
	pMark = 0;
	return false;
}

///<summary>Sets the width and depth of ductwork.</summary>
void lpd::SetOptions(double* dSecWid, double* dSecDep)
{	
	CPegDoc *pDoc = CPegDoc::GetDoc();

	lpd::dSecWid = *dSecWid;
	lpd::dSecDep = *dSecDep;

	pDoc->SetOptionsFocus(IDC_WIDTH);
	pDoc->SetOptionsPage(CPegDoc::TAB_OPEN_LPD);
	if(::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_TABS_MAIN), app.GetSafeHwnd(), DlgProcSetup))
	{
		*dSecWid = Max(0., lpd::dSecWid);
		*dSecDep = Max(0., lpd::dSecDep);
	}
}

double lpd::TransitionLen(
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
	if (aiJus == 0) 
	{
		*adWidChg *= .5;
		dLen *= .5;
	}
	return (dLen);
}