#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "Grid.h"
#include "SubProcFixup.h"
#include "UserAxis.h"

namespace fixup
{
	double dInfluenceAngle = 2.0;
	double dFilletChamfer = 0.25;
}

LRESULT CALLBACK SubProcFixup(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	static	WORD	wPrvKeyDwn = 0;
	
	static	CSeg*	pSegPrv;
	static	CSeg*	pSegRef;
	static	CSeg*	pSegSec;
	static	CPrim*	pPrimPrv;
	static	CPrim*	pPrimRef;
	static	CPrim*	pPrimSec;

	static CPnt ptInt;
	static CLine lnPrv;
	static CLine lnRef;
	static CLine lnSec;
	
	CPrimLine*	pLine;
	CPnt	ptCP;					
	CVec	vMinAx;
	CVec	vMajAx;
	CVec	vPlnNorm;
	
	if (nMsg == WM_MOUSEMOVE)
	{
		;
	}
	else if (nMsg == WM_COMMAND)
	{
		CPnt ptInt;
		CPnt ptCurPos = app.CursorPosGet();
		switch (LOWORD(wParam)) 
		{
			case ID_HELP_KEY:
                HtmlHelp(hwnd, app.GetAppPath() + "\\PegAeSys.chm::/Fixup.htm", HH_DISPLAY_TOPIC, NULL);
				return 0;
			
			// options
			case ID_OP0:
				pDoc->SetOptionsFocus(IDC_FIX_AX_TOL);
				pDoc->SetOptionsPage(CPegDoc::TAB_OPEN_FIXUP);
				::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_TABS_MAIN), app.GetSafeHwnd(), DlgProcSetup);
				return 0;
				
			// reference
			case ID_OP1:
				if (pSegRef != 0)
				{
					pDoc->UpdateAllViews(NULL, CPegDoc::HINT_PRIM, pPrimRef);
				}
				pSegRef = detsegs.SelSegAndPrimUsingPoint(ptCurPos);
				if (pSegRef == 0)
				{
					return 0;
				}
				pPrimRef = detsegs.DetPrim();
				if (!pPrimRef->Is(CPrim::PRIM_LINE))
				{
					return 0;
				}
				ptCurPos = detsegs.DetPt();
				static_cast<CPrimLine*>(pPrimRef)->GetLine(lnRef);
				
				if (wPrvKeyDwn == 0)
					wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP1);
				else if (wPrvKeyDwn == ID_OP1)
					;
				else
				{
					CPrimLine* pLinePrv = static_cast<CPrimLine*>(pPrimPrv);
					if (!line::Intersection(lnPrv, lnRef, ptInt))
					{
						msgInformation(0);
						return 0;
					}
					if (wPrvKeyDwn == ID_OP2)
					{
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSegPrv);
						if (CVec(pLinePrv->Pt1(), ptInt).Length() < CVec(pLinePrv->Pt1(), ptInt).Length()) 
							pLinePrv->SetPt0(ptInt);
						else
							pLinePrv->SetPt1(ptInt);
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSegPrv);
					}
					else if (wPrvKeyDwn == ID_OP3)
					{
						if (CVec(lnPrv[0], ptInt).Length() < CVec(lnPrv[1], ptInt).Length())
							lnPrv[0] = lnPrv[1];
						lnPrv[1] = ptInt;
						if (CVec(lnRef[1], ptInt).Length() < CVec(lnRef[0], ptInt).Length())
							lnRef[1] = lnRef[0];
						lnRef[0] = ptInt;
	//					if (pFndCPGivRadAnd4Pts(fixup::dSiz, lnPrv[0], lnPrv[1], lnRef[0], lnRef[1], &ptCP)) 
//						if (pFndCPGivRadAnd4Pts(prefFixup.GetFilletChamfer(), lnPrv[0], lnPrv[1], lnRef[0], lnRef[1], &ptCP)) 
						if (pFndCPGivRadAnd4Pts(fixup::dFilletChamfer, lnPrv[0], lnPrv[1], lnRef[0], lnRef[1], &ptCP)) 
						{
							lnPrv[1] = lnPrv.ProjPt(ptCP);
							lnRef[0] = lnRef.ProjPt(ptCP);
							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSegPrv);
							pLinePrv->SetPt0(lnPrv[0]);
							pLinePrv->SetPt1(lnPrv[1]);
							pSegPrv->AddTail(new CPrimLine(lnPrv[1], lnRef[0]));
							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSegPrv);
						}
					}
					else if (wPrvKeyDwn == ID_OP4)
					{
						if (CVec(lnPrv[0], ptInt).Length() < CVec(lnPrv[1], ptInt).Length())
							lnPrv[0] = lnPrv[1];
						lnPrv[1] = ptInt;
						if (CVec(lnRef[1], ptInt).Length() < CVec(lnRef[0], ptInt).Length())
							lnRef[1] = lnRef[0];
						lnRef[0] = ptInt;
//						if (pFndCPGivRadAnd4Pts(fixup::dSiz, lnPrv[0], lnPrv[1], lnRef[0], lnRef[1], &ptCP)) 
						//if (pFndCPGivRadAnd4Pts(prefFixup.GetFilletChamfer(), lnPrv[0], lnPrv[1], lnRef[0], lnRef[1], &ptCP)) 
						if (pFndCPGivRadAnd4Pts(fixup::dFilletChamfer, lnPrv[0], lnPrv[1], lnRef[0], lnRef[1], &ptCP)) 
						{
							double dAng;
							lnPrv[1] = lnPrv.ProjPt(ptCP);
							lnRef[0] = lnRef.ProjPt(ptCP);

							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSegPrv);
							pLinePrv->SetPt0(lnPrv[0]);
							pLinePrv->SetPt1(lnPrv[1]);
							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSegPrv);
							
							CVec rPrvEndInter(lnPrv[1], ptInt);
							CVec rPrvEndRefBeg(lnPrv[1], lnRef[0]);
							vPlnNorm = rPrvEndInter ^ rPrvEndRefBeg;
							vPlnNorm.Normalize();
							pFndSwpAngGivPlnAnd3Lns(vPlnNorm, lnPrv[1], ptInt, lnRef[0], ptCP, &dAng);
							vMajAx = CVec(ptCP, lnPrv[1]);
							CPnt rTmp = Pnt_RotAboutArbPtAndAx(lnPrv[1], ptCP, vPlnNorm, HALF_PI);
							vMinAx = CVec(ptCP, rTmp);
						
							CSeg* pSeg = new CSeg(new CPrimArc(ptCP, vMajAx, vMinAx, dAng));
							pDoc->WorkLayerAddTail(pSeg); 
							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
						}
					}
					app.ModeLineUnhighlightOp(wPrvKeyDwn);
				}
				return 0;

			// mend
			case ID_OP2:	
				pSegSec = detsegs.SelSegAndPrimUsingPoint(ptCurPos);
				if (pSegSec == 0)
				{
					return 0;
				}
				pPrimSec = detsegs.DetPrim();
				pLine = static_cast<CPrimLine*>(pPrimSec);
							
				pLine->GetLine(lnSec);
								
				if (wPrvKeyDwn == 0)
				{
					pSegPrv = pSegSec; 
					pPrimPrv = pPrimSec;
					lnPrv[0] = lnSec[0]; 
					lnPrv[1] = lnSec[1];
					wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP2);
				}
				else if (wPrvKeyDwn == ID_OP1)
				{
					if (!line::Intersection(lnRef, lnSec, ptInt)) // Two lines do not define an intersection
					{
						msgInformation(0);
						return 0;
					}
					pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSegSec);
					if (CVec(pLine->Pt0(), ptInt).Length() < CVec(pLine->Pt1(), ptInt).Length()) 
						pLine->SetPt0(ptInt);
					else
						pLine->SetPt1(ptInt);
					pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSegSec);
				}
				else
				{
					if (!line::Intersection(lnPrv, lnSec, ptInt)) // Two lines do not define an intersection
					{
						msgInformation(0);
						return 0;
					}
					if (wPrvKeyDwn == ID_OP2)
					{
						pLine = static_cast<CPrimLine*>(pPrimPrv);
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSegPrv);
						if (CVec(pLine->Pt0(), ptInt).Length() < CVec(pLine->Pt1(), ptInt).Length()) 
							pLine->SetPt0(ptInt);
						else
							pLine->SetPt1(ptInt);
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSegPrv);
					}
					else if (wPrvKeyDwn == ID_OP3)
					{
						if (CVec(lnPrv[0], ptInt).Length() < CVec(lnPrv[1], ptInt).Length())
							lnPrv[0] = lnPrv[1];
						lnPrv[1] = ptInt;
						if (CVec(lnSec[1], ptInt).Length() < CVec(lnSec[0], ptInt).Length())
							lnSec[1] = lnSec[0];
						lnSec[0] = ptInt;
//						if (pFndCPGivRadAnd4Pts(fixup::dSiz, lnPrv[0], lnPrv[1], lnSec[0], lnSec[1], &ptCP)) 
//						if (pFndCPGivRadAnd4Pts(prefFixup.GetFilletChamfer(), lnPrv[0], lnPrv[1], lnSec[0], lnSec[1], &ptCP)) 
						if (pFndCPGivRadAnd4Pts(fixup::dFilletChamfer, lnPrv[0], lnPrv[1], lnRef[0], lnRef[1], &ptCP)) 
						{
							pLine = static_cast<CPrimLine*>(pPrimPrv);
							lnPrv[1] = lnPrv.ProjPt(ptCP);
							lnSec[0] = lnSec.ProjPt(ptCP);
							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSegPrv);
							pLine->SetPt0(lnPrv[0]); 
							pLine->SetPt1(lnPrv[1]);
							pSegPrv->AddTail(new CPrimLine(lnPrv[1], lnSec[0]));
							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSegPrv);
						}
					}
					else if (wPrvKeyDwn == ID_OP4)
					{
						if (CVec(lnPrv[0], ptInt).Length() < CVec(lnPrv[1], ptInt).Length())
							lnPrv[0] = lnPrv[1];
						lnPrv[1] = ptInt;
						if (CVec(lnSec[1], ptInt).Length() < CVec(lnSec[0], ptInt).Length())
							lnSec[1] = lnSec[0];
						lnSec[0] = ptInt;
//						if (pFndCPGivRadAnd4Pts(fixup::dSiz, lnPrv[0], lnPrv[1], lnSec[0], lnSec[1], &ptCP)) 
//						if (pFndCPGivRadAnd4Pts(prefFixup.GetFilletChamfer(), lnPrv[0], lnPrv[1], lnSec[0], lnSec[1], &ptCP)) 
						if (pFndCPGivRadAnd4Pts(fixup::dFilletChamfer, lnPrv[0], lnPrv[1], lnRef[0], lnRef[1], &ptCP)) 
						{
							double dAng;
							pLine = static_cast<CPrimLine*>(pPrimPrv);
							lnPrv[1] = lnPrv.ProjPt(ptCP);
							lnSec[0] = lnSec.ProjPt(ptCP);
							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSegPrv);
							pLine->SetPt0(lnPrv[0]); 
							pLine->SetPt1(lnPrv[1]);
							CVec rPrvEndInter(lnPrv[1], ptInt);
							CVec rPrvEndSecBeg(lnPrv[1], lnSec[0]);
							vPlnNorm = rPrvEndInter ^ rPrvEndSecBeg;
							vPlnNorm.Normalize();
							pFndSwpAngGivPlnAnd3Lns(vPlnNorm, lnPrv[1], ptInt, lnSec[0], ptCP, &dAng);
							vMajAx = CVec(ptCP, lnPrv[1]);
							CPnt rTmp = Pnt_RotAboutArbPtAndAx(lnPrv[1], ptCP, vPlnNorm, HALF_PI);
							vMinAx = CVec(ptCP, rTmp);
							pSegPrv->AddTail(new CPrimArc(ptCP, vMajAx, vMinAx, dAng));
							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSegPrv);
						}
					}
					pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSegSec);
					pLine = static_cast<CPrimLine*>(pPrimSec);
					if (CVec(pLine->Pt0(), ptInt).Length() < CVec(pLine->Pt1(), ptInt).Length()) 
						pLine->SetPt0(ptInt);
					else
						pLine->SetPt1(ptInt);
					pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSegSec);
					app.ModeLineUnhighlightOp(wPrvKeyDwn);
				}
				return 0;
				
			// chamfer & fillet
			case ID_OP3:
			case ID_OP4:
				pSegSec = detsegs.SelSegAndPrimUsingPoint(ptCurPos);
				pPrimSec = detsegs.DetPrim();

				// mak - return if no line selected
				if(pPrimSec == NULL)
					return 0;

				pLine = static_cast<CPrimLine*>(pPrimSec);
				pLine->GetLine(lnSec);
				
				if (wPrvKeyDwn == 0)
				{
					pSegPrv = pSegSec; pPrimPrv = pPrimSec;
					lnPrv = lnSec; 
					wPrvKeyDwn = app.ModeLineHighlightOp(LOWORD(wParam));
				}
				else
				{
					if (wPrvKeyDwn == ID_OP1)
					{
						pSegPrv = pSegSec; pPrimPrv = pPrimSec;
						lnPrv = lnRef; 
					}				
					if (!line::Intersection(lnPrv, lnSec, ptInt)) // Two lines do not define an intersection
					{
						msgInformation(0);
						return 0;
					}
					if (CVec(lnPrv[0], ptInt).Length() < CVec(lnPrv[1], ptInt).Length())
						lnPrv[0] = lnPrv[1];
					lnPrv[1] = ptInt;
					if (CVec(lnSec[1], ptInt).Length() < CVec(lnSec[0], ptInt).Length())
						lnSec[1] = lnSec[0];
					lnSec[0] = ptInt;
//					if (pFndCPGivRadAnd4Pts(fixup::dSiz, lnPrv[0], lnPrv[1], lnSec[0], lnSec[1], &ptCP)) 
//					if (pFndCPGivRadAnd4Pts(prefFixup.GetFilletChamfer(), lnPrv[0], lnPrv[1], lnSec[0], lnSec[1], &ptCP)) 
					if (pFndCPGivRadAnd4Pts(fixup::dFilletChamfer, lnPrv[0], lnPrv[1], lnRef[0], lnRef[1], &ptCP)) 
					{
						//	   Center point is defined .. determine arc endpoints
						lnPrv[1] = lnPrv.ProjPt(ptCP);
						lnSec[0] = lnSec.ProjPt(ptCP);
						if (wPrvKeyDwn == ID_OP1)
							;
						else if (wPrvKeyDwn == ID_OP2)
						{
							pLine = static_cast<CPrimLine*>(pPrimPrv);
							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSegPrv);
							pLine->SetPt0(lnPrv[0]);
							pLine->SetPt1(ptInt);
							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSegPrv);
						}
						else if (wPrvKeyDwn == ID_OP3 || wPrvKeyDwn == ID_OP4)
						{
							pLine = static_cast<CPrimLine*>(pPrimPrv);
							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSegPrv);
							pLine->SetPt0(lnPrv[0]);
							pLine->SetPt1(lnPrv[1]);
							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSegPrv);
						}
						pLine = static_cast<CPrimLine*>(pPrimSec);
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSegSec);
						pLine->SetPt0(lnSec[0]);
						pLine->SetPt1(lnSec[1]);
						if (LOWORD(wParam) == ID_OP3)
							pSegSec->AddTail(new CPrimLine(lnPrv[1], lnSec[0]));
						else
						{
							double dAng;
							CVec rPrvEndInter(lnPrv[1], ptInt);
							CVec rPrvEndSecBeg(lnPrv[1], lnSec[0]);
							vPlnNorm = rPrvEndInter ^ rPrvEndSecBeg;
							vPlnNorm.Normalize();
							pFndSwpAngGivPlnAnd3Lns(vPlnNorm, lnPrv[1], ptInt, lnSec[0], ptCP, &dAng);
							vMajAx = CVec(ptCP, lnPrv[1]);
							CPnt rTmp = Pnt_RotAboutArbPtAndAx(lnPrv[1], ptCP, vPlnNorm, HALF_PI);
							vMinAx = CVec(ptCP, rTmp);
							pSegSec->AddTail(new CPrimArc(ptCP, vMajAx, vMinAx, dAng));
						}
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSegSec);
					}
					app.ModeLineUnhighlightOp(wPrvKeyDwn);
				}
				return 0;

			// square
			case ID_OP5:
				pSegSec = detsegs.SelSegAndPrimUsingPoint(ptCurPos);
				if (pSegSec != 0)
				{
					pPrimSec = detsegs.DetPrim();
					ptCurPos = detsegs.DetPt();
					if (pPrimSec->Is(CPrim::PRIM_LINE))
					{
						pLine = static_cast<CPrimLine*>(pPrimSec);
						pLine->GetLine(lnSec);
						double dLen = lnSec.Length();
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSegSec);
						lnSec[0] = UserAxis::UserAxisSnapLn(ptCurPos, lnSec[0]);
						lnSec[1] = Pnt_ProjPtTo(lnSec[0], ptCurPos, dLen);
						pLine->SetPt0(grid::UserSnapPt(lnSec[0]));
						pLine->SetPt1(grid::UserSnapPt(lnSec[1]));
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSegSec);
					}
				}
				return 0;
				
			// parallel
			case ID_OP6:
				pSegSec = detsegs.SelSegAndPrimUsingPoint(ptCurPos);
				if (pSegRef != 0 && pSegSec != 0)
				{
					pPrimSec = detsegs.DetPrim();
					if (pPrimSec->Is(CPrim::PRIM_LINE))
					{
						pLine = static_cast<CPrimLine*>(pPrimSec);
				
						lnSec[0] = lnRef.ProjPt(pLine->Pt0());
						lnSec[1] = lnRef.ProjPt(pLine->Pt1());
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSegSec);
						pLine->SetPt0(Pnt_ProjPtTo(lnSec[0], pLine->Pt0(), app.GetDimLen()));
						pLine->SetPt1(Pnt_ProjPtTo(lnSec[1], pLine->Pt1(), app.GetDimLen()));
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSegSec);
					}
				}
				return 0;
				
			case ID_OP7:
				return 0;
		
			case ID_OP8:										// generalized arc
				return 0;
			
			case ID_OP9:										// Generate construction line
				return 0;
			
			case IDM_RETURN:
				if (pSegRef != 0)
				{
					pDoc->UpdateAllViews(NULL, CPegDoc::HINT_PRIM, pPrimRef);
					pSegRef = 0; pPrimRef = 0;
				}
				app.ModeLineUnhighlightOp(wPrvKeyDwn);
				return 0;

			case IDM_ESCAPE:

				if(wPrvKeyDwn != 0)
					app.RubberBandingDisable();
				else
				pDoc->DoEditPrimitiveEscape();

				if (pSegRef != 0)
				{
					pDoc->UpdateAllViews(NULL, CPegDoc::HINT_PRIM, pPrimRef);
					pSegRef = 0; pPrimRef = 0;
				}
				app.ModeLineUnhighlightOp(wPrvKeyDwn);
				return 0;

			break;
		}
	}
	return (CallWindowProc(app.GetMainWndProc(), hwnd, nMsg, wParam, lParam));
}