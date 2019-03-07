#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "DlgProcSetLength.h"

#include "UserAxis.h"

double	p2LNdDisBetLns = 6.;		// Distance between parallel line (user units)
double	p2LNdCenLnEcc = 0.; 		// Center line eccentricity for parallel lines

namespace draw2
{
	double DlgProcDraw2_dDistance = .125;
}

LRESULT CALLBACK SubProcDraw2(HWND hwnd, UINT anMsg, WPARAM wParam, LPARAM lParam)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	static WORD	wPrvKeyDwn = 0;

	static	bool	bContCorn = FALSE;
	static	CSeg*	pSegBegWallSec = 0;
	static	CSeg*	pSegEndWallSec = 0;
	static	CSeg*	pSegSav = 0;
	static	CPrimLine*	pLineBegWallSec = 0;
	static	CPrimLine*	pLineEndWallSec = 0;
	static	CPnt	ptPrvPos;
	static	CLine 	lnPar[4];
	
	CSeg* pSeg;
	CLine ln;
	CPnt ptEnd;
	CPnt ptBeg;
	CPnt ptInt;
	
	if (anMsg == WM_COMMAND)
	{
		CPrimLine*	pLine;
		double	dDisBetLns = p2LNdDisBetLns / app.GetScale();
		long	lResult = 0;
		CPnt	ptCurPos = app.CursorPosGet();

		p2LNdDisBetLns = draw2::DlgProcDraw2_dDistance * app.GetScale();
		
		switch (LOWORD(wParam))
		{
			case ID_HELP_KEY:
			{
                HtmlHelp(hwnd, app.GetAppPath() + "\\PegAeSys.chm::/DoubleLine.htm", HH_DISPLAY_TOPIC, NULL);
				return 0;
			}
			// a test for non-zero z used to result in error message if 0-9 
			case ID_OP0:
				pDoc->SetOptionsFocus(IDC_DRAW2_WIDTH);
				pDoc->SetOptionsPage(CPegDoc::TAB_OPEN_DRAW2);
				::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_TABS_MAIN), app.GetSafeHwnd(), DlgProcSetup);
				break;
				
			case ID_OP1:										// Search for an endcap in proximity of current location
				pSeg = detsegs.SelSegAndPrimUsingPoint(ptCurPos);

				if (pSeg != 0)
				{
					ptCurPos = detsegs.DetPt();
					if (wPrvKeyDwn == 0)
					{	// Starting at existing wall
						pSegBegWallSec = pSeg; 
						pLineBegWallSec = static_cast<CPrimLine*>(detsegs.DetPrim());
					}
					else										
					{	// Ending at existing wall
						pSegEndWallSec = pSeg; 
						pLineEndWallSec = static_cast<CPrimLine*>(detsegs.DetPrim());
					}
					app.CursorPosSet(ptCurPos);
				}
				break;
				
			case ID_OP2:
				if (wPrvKeyDwn != 0) 
					app.RubberBandingDisable();

				if (pSegEndWallSec != 0) 					// Into existing wall
				{
					lnPar[0] = lnPar[2]; 
					lnPar[1] = lnPar[3];
					
					ln(ptPrvPos, ptCurPos);
					ln.GetParallels(dDisBetLns, p2LNdCenLnEcc, lnPar[2], lnPar[3]);
					if (bContCorn)										
					{	// Clean up previous set
						for (int i = 0; i < 2; i++)
						{
							if (line::Intersection_xy(lnPar[i], lnPar[i + 2], ptInt)) 
							{
								lnPar[i][1] = ptInt;
								lnPar[i + 2][0] = ptInt;
							}
							else								// Lines are parallel
								bContCorn = FALSE;
						}
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSegSav);

						pSegSav->RemoveTail();
						POSITION pos = pSegSav->GetTailPosition();
						CPrimLine* pLine = static_cast<CPrimLine*>(pSegSav->GetPrev(pos));
						pLine->SetPt1(lnPar[1][1]);
						pLine = static_cast<CPrimLine*>(pSegSav->GetPrev(pos));
						pLine->SetPt1(lnPar[0][1]);
					}
					else if (pSegBegWallSec != 0) 
					{
						pSegSav = pSegBegWallSec;

						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSegBegWallSec);
						ptBeg = pLineBegWallSec->Pt0();
						ptEnd = pLineBegWallSec->Pt1();
						for (int i = 2; i < 4; i++)
						{
							if (line::Intersection_xy(CLine(ptBeg, ptEnd), lnPar[i], ptInt)) 
								lnPar[i][0] = ptInt;
							else										// Lines are parallel
								bContCorn = FALSE;
						}
						pLine = new CPrimLine(*pLineBegWallSec);
						
						if (CVec(ptBeg, lnPar[2][0]).Length() > CVec(ptBeg, lnPar[3][0]).Length()) 
						{
							pLineBegWallSec->SetPt1(lnPar[3][0]);
							pLine->SetPt0(lnPar[2][0]);
						}
						else
						{
							pLineBegWallSec->SetPt1(lnPar[2][0]);
							pLine->SetPt0(lnPar[3][0]);
						}
						pSegBegWallSec->AddTail(pLine);
						pSegBegWallSec = 0;
					}
					else if (wPrvKeyDwn == ID_OP2) 
					{
						pSegSav = new CSeg;
						pDoc->WorkLayerAddTail(pSegSav);
						pSegSav->AddTail(new CPrimLine(lnPar[2][0], lnPar[3][0]));
					}
					pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSegEndWallSec);
					ptBeg = pLineEndWallSec->Pt0();
					ptEnd = pLineEndWallSec->Pt1();
					
					pSegSav->AddTail(new CPrimLine(pstate.PenColor(), pstate.PenStyle(), lnPar[2]));
					pSegSav->AddTail(new CPrimLine(pstate.PenColor(), pstate.PenStyle(), lnPar[3]));
					pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSegSav);

					pLine = new CPrimLine(*pLineEndWallSec);
					if (CLine(ptPrvPos, ptCurPos).DirRelOfPt(ptBeg) < 0.) 
					{
						pLineEndWallSec->SetPt1(lnPar[3][1]);
						pLine->SetPt0(lnPar[2][1]);
					}
					else
					{
						pLineEndWallSec->SetPt1(lnPar[2][1]);
						pLine->SetPt0(lnPar[3][1]);
					}
					pSegEndWallSec->AddTail(pLine);
					pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSegEndWallSec);
					pSegEndWallSec = 0;

					app.RubberBandingDisable();
					app.ModeLineUnhighlightOp(wPrvKeyDwn);
					bContCorn = FALSE;
				}
				else
				{
					if (wPrvKeyDwn != 0) 
					{
						ptCurPos = UserAxis::UserAxisSnapLn(ptPrvPos, ptCurPos);
						lnPar[0] = lnPar[2]; 
						lnPar[1] = lnPar[3];
						ln(ptPrvPos, ptCurPos);
						ln.GetParallels(dDisBetLns, p2LNdCenLnEcc, lnPar[2], lnPar[3]);
						
						if (bContCorn) 
						{
							for (int i = 0; i < 2; i++)
							{
								if (line::Intersection_xy(lnPar[i], lnPar[i + 2], ptInt)) 
								{
									lnPar[i][1] = ptInt;
									lnPar[i + 2][0] = ptInt;
								}
								else									// Lines are parallel
									bContCorn = FALSE;
							}
							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSegSav);
							delete pSegSav->RemoveTail();

							POSITION pos = pSegSav->GetTailPosition();
							
							CPrimLine* pLine = static_cast<CPrimLine*>(pSegSav->GetPrev(pos));
							pLine->SetPt1(lnPar[1][1]);
							pLine = static_cast<CPrimLine*>(pSegSav->GetPrev(pos));
							pLine->SetPt1(lnPar[0][1]);
						}
						else if (pSegBegWallSec != 0) 
						{
							pSegSav = pSegBegWallSec;

							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSegBegWallSec);
							ptBeg = pLineBegWallSec->Pt0();
							ptEnd = pLineBegWallSec->Pt1();
							for (int i = 2; i < 4; i++)
							{
								if (line::Intersection_xy(CLine(ptBeg, ptEnd), lnPar[i], ptInt)) 
									lnPar[i][0] = ptInt;
								else							// Lines are parallel
									bContCorn = FALSE;
							}
							pLine = new CPrimLine(*pLineBegWallSec);
							
							if (CVec(ptBeg, lnPar[2][0]).Length() > CVec(ptBeg, lnPar[3][0]).Length()) 
							{
								pLineBegWallSec->SetPt1(lnPar[3][0]);
								pLine->SetPt0(lnPar[2][0]);
							}
							else
							{
								pLineBegWallSec->SetPt1(lnPar[2][0]);
								pLine->SetPt0(lnPar[3][0]);
							}
							pSegBegWallSec->AddTail(pLine);
							pSegBegWallSec = 0;
						}
						else if (wPrvKeyDwn == ID_OP2) 
						{
							pSegSav = new CSeg;
							pDoc->WorkLayerAddTail(pSegSav);
							pSegSav->AddTail(new CPrimLine(lnPar[2][0], lnPar[3][0]));
						}						
						pSegSav->AddTail(new CPrimLine(pstate.PenColor(), pstate.PenStyle(), lnPar[2]));
						pSegSav->AddTail(new CPrimLine(pstate.PenColor(), pstate.PenStyle(), lnPar[3]));
						
						pSegSav->AddTail(new CPrimLine(lnPar[3][1], lnPar[2][1]));
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSegSav);
						bContCorn = true;
					}
					wPrvKeyDwn = ID_OP2;
					ptPrvPos = ptCurPos;
					app.CursorPosSet(ptPrvPos);
					app.RubberBandingStartAtEnable(ptPrvPos, Lines);
				}
				ptPrvPos = ptCurPos;
				break;

			// insert bitmap
			case ID_OP3:
			{
//				pDoc->BitmapLayerLoad();
//				CPrimText *pPrim = new CPrimText
//					pDoc->WorkLayerAddTail(new CSeg(pPrim));
				break;
			}
				
			case IDM_RETURN:
				if (wPrvKeyDwn != 0)
				{
					app.RubberBandingDisable();
					app.ModeLineUnhighlightOp(wPrvKeyDwn);
					bContCorn = FALSE;
					pSegBegWallSec = 0; 
					pLineBegWallSec = 0;
					pSegEndWallSec = 0; 
					pLineEndWallSec = 0;
				}
				ptPrvPos = ptCurPos;
				break;
				
			case IDM_ESCAPE:
				if (wPrvKeyDwn != 0) 
					app.RubberBandingDisable();
				else
					pDoc->DoEditPrimitiveEscape();

				app.ModeLineUnhighlightOp(wPrvKeyDwn);
				bContCorn = FALSE;
				pSegBegWallSec = 0; 
				pLineBegWallSec = 0;
				pSegEndWallSec = 0; 
				pLineEndWallSec = 0;
								
			default:
				lResult = !0;
		}
		if (lResult == 0)
			return (lResult);
	}
	return(CallWindowProc(app.GetMainWndProc(), hwnd, anMsg, wParam, lParam));
}	
