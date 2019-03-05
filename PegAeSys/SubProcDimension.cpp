#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "SubProcAnnotate.h"
#include "SubProcDimension.h"
#include "UnitsString.h"
#include "UserAxis.h"

//namespace dimension
//{
//
//}

LRESULT CALLBACK SubProcDimension(HWND hwnd, UINT anMsg, WPARAM wParam, LPARAM lParam)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	CPegView* pView = CPegView::GetActiveView();
	CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();

	static CPnt rProjPt[2];
	static CLine ln;
	static CPnt ptCen;
	static CPnt ptPrv;
	static int iLns;
	static WORD wPrvOp = 0;
	

	if (anMsg == WM_COMMAND)
	{
		long lResult = 0;
		CPnt ptCur = app.CursorPosGet();
		
		switch (LOWORD(wParam))
		{
			case IDM_RETURN:
				if (wPrvOp != 0)
				{
					app.RubberBandingDisable();
					app.ModeLineUnhighlightOp(wPrvOp);
				}
				ptPrv = ptCur;
				break;
				
			case ID_OP0:
				pDoc->SetOptionsFocus(IDC_DIM_CONST);
				pDoc->SetOptionsPage(CPegDoc::TAB_OPEN_SETUPDIMENSION);
				::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_TABS_MAIN), app.GetSafeHwnd(), DlgProcSetup);
				// I don't know what this was doing... I don't think anything
				/*if (wPrvOp != 0)
				{
					app.RubberBandingDisable();
					app.ModeLineUnhighlightOp(wPrvOp);
				}
				ptPrv = ptCur;*/
				break;
				
			case ID_OP1:	// Generate arrow head
				if (wPrvOp != 0)
				{
					app.RubberBandingDisable();
					app.ModeLineUnhighlightOp(wPrvOp);
				}
				ptPrv = dimension::AddArrowHead(ptCur);
				break;
				
			case ID_OP2:	// Generate simple line
				app.RubberBandingDisable();
				if (wPrvOp != ID_OP2) 
				{
					wPrvOp = app.ModeLineHighlightOp(ID_OP2);
					ptPrv = ptCur;
				}
				else
				{
					//ptCur = UserAxis::UserAxisSnapLn(ptPrv, ptCur);
					ptCur = annotate::SnapLn(ptPrv, ptCur);

					if (ptPrv != ptCur) 
					{
						CSeg* pSeg = new CSeg(new CPrimLine(1, 1, ptPrv, ptCur));
						pDoc->WorkLayerAddTail(pSeg);
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
					}
					ptPrv = ptCur;
				}
				app.RubberBandingStartAtEnable(ptCur, Lines);
				break;
				
			case ID_OP3:	// Line with dimension
				if (wPrvOp == ID_OP3 || wPrvOp == ID_OP4)
				{
					ptCur = annotate::SnapLn(ptPrv, ptCur);

					app.RubberBandingDisable();
					if (ptPrv != ptCur)
					{
						CSeg* pSeg = new CSeg;
						
						if (wPrvOp == ID_OP4)
						{
							//annotate::GenArrowHead(1, .1, ptCur, ptPrv, pSeg);
							annotate::GenArrowHead(annotate::nArrowType, annotate::dArrowSize, ptCur, ptPrv, pSeg);
							app.ModeLineUnhighlightOp(wPrvOp);
							wPrvOp = app.ModeLineHighlightOp(ID_OP3);
						}
						CPrimDim* pDim = new CPrimDim(1, 1, CLine(ptPrv, ptCur));
						pDim->SetTextPenColor(5);
						pDim->SetTextHorAlign(CFontDef::HOR_ALIGN_CENTER);
						pDim->SetTextVerAlign(CFontDef::VER_ALIGN_MIDDLE);
						
						pSeg->AddTail(pDim); 
						pDoc->WorkLayerAddTail(pSeg); 
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);	
						
						ptPrv = ptCur;
					}
				}
				else 
				{
					if (wPrvOp != 0)
					{
						app.RubberBandingDisable();
						app.ModeLineUnhighlightOp(wPrvOp);
					}				
					wPrvOp = app.ModeLineHighlightOp(ID_OP3);
					ptPrv = ptCur;
				}
				app.CursorPosSet(ptCur);
				app.RubberBandingStartAtEnable(ptCur, Lines);
				break;
				
			case ID_OP4:
				if (wPrvOp == 0) 
				{
					wPrvOp = app.ModeLineHighlightOp(ID_OP4);
					ptPrv = ptCur;
				}	
				else if (wPrvOp == ID_OP3 || wPrvOp == ID_OP4)
				{
					ptCur = annotate::SnapLn(ptPrv, ptCur);

					app.RubberBandingDisable();
					if (ptPrv != ptCur)
					{
						CSeg* pSeg = new CSeg;
						if (wPrvOp == ID_OP4) 
							annotate::GenArrowHead(annotate::nArrowType, annotate::dArrowSize, ptCur, ptPrv, pSeg);
							//annotate::GenArrowHead(1, .1, ptCur, ptPrv, pSeg);
						else
						{
							app.ModeLineUnhighlightOp(wPrvOp);
							wPrvOp = app.ModeLineHighlightOp(ID_OP4);
						}
						CPrimDim *pDim = new CPrimDim(1, 1, CLine(ptPrv, ptCur));
						pDim->SetTextPenColor(5);
						pDim->SetTextHorAlign(CFontDef::HOR_ALIGN_CENTER);
						pDim->SetTextVerAlign(CFontDef::VER_ALIGN_MIDDLE);
						
						pSeg->AddTail(pDim); 
						//annotate::GenArrowHead(1, .1, ptPrv, ptCur, pSeg);
						annotate::GenArrowHead(annotate::nArrowType, annotate::dArrowSize, ptPrv, ptCur, pSeg);
						pDoc->WorkLayerAddTail(pSeg); 
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
						
						ptPrv = ptCur;
					}
					else
						msgInformation(0);
				}
				else
					// error finish prior op first
				
				app.CursorPosSet(ptCur);
				app.RubberBandingStartAtEnable(ptCur, Lines);
				break;
				
			case ID_OP5:	// Batter board
				if (wPrvOp != ID_OP5) 
				{
					app.RubberBandingDisable();
					ptPrv = dimension::ProjPtToLn(ptCur);
					app.ModeLineUnhighlightOp(wPrvOp);
					wPrvOp = app.ModeLineHighlightOp(ID_OP5);
				}
				else
				{
					ptCur = dimension::ProjPtToLn(ptCur);
					if (ptPrv != ptCur)
					{
						ptCur = Pnt_ProjPtTo(ptCur, ptPrv, - .1875);
						ptPrv = Pnt_ProjPtTo(ptPrv, ptCur, .0625);
						
						CSeg* pSeg = new CSeg(new CPrimLine(1, 1, ptPrv, ptCur));
						pDoc->WorkLayerAddTail(pSeg); 
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
					}
					ptPrv = ptCur;
					app.ModeLineUnhighlightOp(wPrvOp);
				}
				break;
				
			case ID_OP6:	// Radius
				ptPrv = dimension::AddRadius(ptCur);
				break;
			
			case ID_OP7:	// Diameter
				ptPrv = dimension::AddDiameter(ptCur);
				break;
							
			case ID_OP8:
				if (wPrvOp != ID_OP8) 
				{
					app.RubberBandingDisable();
					app.ModeLineUnhighlightOp(wPrvOp);
					
					if (detsegs.SelLineUsingPoint(ptCur) != 0)
					{
						CPrimLine* pLine = static_cast<CPrimLine*>(detsegs.DetPrim());
						
						rProjPt[0] = detsegs.DetPt();
						pLine->GetLine(ln);
						wPrvOp = app.ModeLineHighlightOp(ID_OP8);
						msgInformation("Select the second line.");				
						iLns = 1;
					}
				}
				else
				{
					if (iLns == 1)
					{
						if (detsegs.SelLineUsingPoint(ptCur) != 0)
						{
							CPrimLine* pLine = static_cast<CPrimLine*>(detsegs.DetPrim());
							
							rProjPt[1] = detsegs.DetPt();
							if (line::Intersection(ln, pLine->Ln(), ptCen)) 
							{
								iLns++;
								msgInformation("Specify the location for the dimension arc.");				
							}
						}
					}
					else
					{
						char szNote[32];
						double dAng;
						
						CVec vPlnNorm(CVec(ptCen, rProjPt[0]) ^ CVec(ptCen, ptCur));
						vPlnNorm.Normalize();
						if (pFndSwpAngGivPlnAnd3Lns(vPlnNorm, rProjPt[0], ptCur, rProjPt[1], ptCen, &dAng)) 
						{
							double dRad = CVec(ptCen, ptCur).Length();
							
							ln[0] = Pnt_ProjPtTo(ptCen, rProjPt[0], dRad);
							ln[1] = Pnt_RotAboutArbPtAndAx(ln[0], ptCen, vPlnNorm, dAng);
							
							CVec vXAx = CVec(ptCen, ln[0]);
							CPnt ptRot(Pnt_RotAboutArbPtAndAx(ln[0], ptCen, vPlnNorm, HALF_PI));
							CVec vYAx = CVec(ptCen, ptRot);
							CPnt ptArrow = Pnt_RotAboutArbPtAndAx(ln[0], ptCen, vPlnNorm, RADIAN);
							
							CSeg* pSeg = new CSeg;
							//annotate::GenArrowHead(1, .1, ptArrow, ln[0], pSeg);
							annotate::GenArrowHead(annotate::nArrowType, annotate::dArrowSize, ptArrow, ln[0], pSeg);
							pSeg->AddTail(new CPrimArc(1, 1, ptCen, vXAx, vYAx, dAng));
							ptArrow = Pnt_RotAboutArbPtAndAx(ln[0], ptCen, vPlnNorm, dAng - RADIAN);
							//annotate::GenArrowHead(1, .1, ptArrow, ln[1], pSeg);
							annotate::GenArrowHead(annotate::nArrowType, annotate::dArrowSize, ptArrow, ln[1], pSeg);
							
							int iPrimState = pstate.Save();
							
							CFontDef fd;
							pstate.GetFontDef(fd);
							fd.TextHorAlignSet(CFontDef::HOR_ALIGN_CENTER);
							fd.TextVerAlignSet(CFontDef::VER_ALIGN_MIDDLE);
							pstate.SetFontDef(pDC, fd);
							
							CCharCellDef ccd;
							pstate.GetCharCellDef(ccd);
							ccd.TextRotAngSet(0.);
							ccd.ChrHgtSet(.1);
							pstate.SetCharCellDef(ccd);
							
							CPnt ptPvt = Pnt_ProjPtTo(ptCur, ptCen, - .25);
							CharCellDef_EncdRefSys(ccd, vXAx, vYAx);
							CRefSys rs(ptPvt, vXAx, vYAx);
							UnitsString_FormatAngle(szNote, dAng, 8, 3); 
							pSeg->AddTail(new CPrimText(fd, rs, szNote));
							((CPrim*)pSeg->GetTail())->SetPenColor(5);
							//pDim->SetTextPenColor(5);
							pDoc->WorkLayerAddTail(pSeg); 
							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
							pstate.Restore(pDC, iPrimState);
						}
						app.ModeLineUnhighlightOp(wPrvOp);
						msgInformation();
					}
				}
				break;
				
			case ID_OP9:
				if (wPrvOp != 0) 
				{
					app.RubberBandingDisable();
					app.ModeLineUnhighlightOp(wPrvOp);
				}
				ptPrv = dimension::CvtPrim(ptCur);
				pView->InvalidateRect(NULL, TRUE);
				break;
			
			case IDM_ESCAPE:
				if (wPrvOp != 0) 
					app.RubberBandingDisable();
				else
					pDoc->DoEditPrimitiveEscape();

				app.ModeLineUnhighlightOp(wPrvOp);
				
			default:
				lResult = !0;
		}
		if (lResult == 0)
			return (lResult);
	}
	return(CallWindowProc(app.GetMainWndProc(), hwnd, anMsg, wParam, lParam));
}	
CPnt dimension::AddArrowHead(CPnt arPt)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	POSITION pos = detsegs.GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = detsegs.GetNext(pos);

		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim != 0)
		{
			CPrim* pPrim = pSeg->GetNext(posPrim);
			CLine ln;
			if (pPrim->Is(CPrim::PRIM_LINE))
				static_cast<CPrimLine*>(pPrim)->GetLine(ln);
			else if (pPrim->Is(CPrim::PRIM_DIM))
				ln = static_cast<CPrimDim*>(pPrim)->Line();
			else
				continue;
			
			CPnt ptProj;
			double dRel[2];
			double dDis;
			
			if (line::EvalPtProx_xy(ln, arPt, .05, ptProj, dRel, &dDis)) 
			{
				CPnt pt;
					
				CSeg* pSeg = new CSeg;
				if (*dRel <= .5) 
				{
					//annotate::GenArrowHead(1, .1, ln[1], ln[0], pSeg);
					annotate::GenArrowHead(annotate::nArrowType, annotate::dArrowSize, ln[1], ln[0], pSeg);
					pt = ln[0];
				}
				else
				{
					//annotate::GenArrowHead(1, .1, ln[0], ln[1], pSeg);
					annotate::GenArrowHead(annotate::nArrowType, annotate::dArrowSize, ln[0], ln[1], pSeg);
					pt = ln[1];
				}
				pDoc->WorkLayerAddTail(pSeg); 
				pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);					
									
				app.CursorPosSet(pt);
				return (pt);
			}
		}
	}
	return (arPt);
}							
CPnt dimension::AddDiameter(const CPnt& pt)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	if (detsegs.SelSegAndPrimUsingPoint(pt) != 0)
	{
		CPnt ptEnd = detsegs.DetPt();

		if ((detsegs.DetPrim())->Is(CPrim::PRIM_ARC))
		{
			CPrimArc* pArc = static_cast<CPrimArc*>(detsegs.DetPrim());

			CPnt ptBeg = Pnt_ProjPtTo(ptEnd, pArc->Center(), 2. * pArc->GetMajAx().Length());
						
			CSeg* pSeg = new CSeg;
			
			//annotate::GenArrowHead(1, .1, ptEnd, ptBeg, pSeg);
			annotate::GenArrowHead(annotate::nArrowType, annotate::dArrowSize, ptEnd, ptBeg, pSeg);
			
			CPrimDim* pDim = new CPrimDim(1, 1, CLine(ptBeg, ptEnd));
			pDim->SetText("D" + pDim->Text());
			pDim->SetDefaultNote();
			
			pDim->SetTextPenColor(5);
			pDim->SetTextHorAlign(CFontDef::HOR_ALIGN_CENTER);
			pDim->SetTextVerAlign(CFontDef::VER_ALIGN_MIDDLE);
			pSeg->AddTail(pDim); 
	
			//annotate::GenArrowHead(1, .1, ptBeg, ptEnd, pSeg);
			annotate::GenArrowHead(annotate::nArrowType, annotate::dArrowSize, ptBeg, ptEnd, pSeg);
			pDoc->WorkLayerAddTail(pSeg);
			pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
						
			return(ptEnd);
		}
	}
	return (pt);
}
CPnt dimension::AddRadius(const CPnt& arPt)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	if (detsegs.SelSegAndPrimUsingPoint(arPt) != 0)
	{
		CPnt ptEnd = detsegs.DetPt();

		if ((detsegs.DetPrim())->Is(CPrim::PRIM_ARC))
		{
			CPrimArc* pArc = static_cast<CPrimArc*>(detsegs.DetPrim());

			CPnt ptBeg = pArc->Center();
			
			CSeg* pSeg = new CSeg;
			
			CPrimDim* pDim = new CPrimDim(1, 1, CLine(ptBeg, ptEnd));
			pDim->SetText("R" + pDim->Text());
			pDim->SetDefaultNote();

			pDim->SetTextPenColor(5);
			pDim->SetTextHorAlign(CFontDef::HOR_ALIGN_CENTER);
			pDim->SetTextVerAlign(CFontDef::VER_ALIGN_MIDDLE);
			
			pSeg->AddTail(pDim); 
	
			//annotate::GenArrowHead(1, .1, ptBeg, ptEnd, pSeg);
			annotate::GenArrowHead(annotate::nArrowType, annotate::dArrowSize, ptBeg, ptEnd, pSeg);
			pDoc->WorkLayerAddTail(pSeg);
			pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
			
			return(ptEnd);
		}
	}
	return (arPt);
	// error arc not identified
}
CPnt dimension::CvtPrim(CPnt pt)
{	
	CSeg* pSeg;
	CPrim* pPrim;
	CPnt ptProj;

	POSITION posPrimCur;

	CPnt4 ptView(pt, 1.);
	ModelView_DoTransform(ptView);

	double dPicApertSiz = detsegs.PicApertSiz();

	POSITION pos = detsegs.GetHeadPosition();
	while (pos != 0)
	{
		pSeg = detsegs.GetNext(pos);

		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim != 0)
		{
			posPrimCur = posPrim;
			pPrim = pSeg->GetNext(posPrim);
			if (pPrim->SelUsingPoint(ptView, dPicApertSiz, ptProj))
			{
				CLine ln;
				
				if (pPrim->Is(CPrim::PRIM_LINE))
				{
					// if the segment that is being converted to a dim is grouped with a single text string,
					// assume that that text string is the text to associate, otherwise, create
					if(pSeg->GetCount() == 2)
					{
						if(( ((CPrim*)(pSeg->GetHead()))->Is(CPrim::PRIM_TEXT) ) && ( ((CPrim*)(pSeg->GetTail()))->Is(CPrim::PRIM_LINE) ))
							pSeg->RemoveHead();
						else if(( ((CPrim*)(pSeg->GetTail()))->Is(CPrim::PRIM_TEXT) ) && ( ((CPrim*)(pSeg->GetHead()))->Is(CPrim::PRIM_LINE) ))
							pSeg->RemoveTail();
					}

					CPrimLine* pPrimLine = static_cast<CPrimLine*>(pPrim);
					pPrimLine->GetLine(ln);
					CPrimDim* pPrimDim = new CPrimDim(pPrimLine->PenColor(), pPrimLine->PenStyle(), ln);
					pPrimDim->SetTextPenColor(5);
					pPrimDim->SetTextHorAlign(CFontDef::HOR_ALIGN_CENTER);
					pPrimDim->SetTextVerAlign(CFontDef::VER_ALIGN_MIDDLE);
					pSeg->InsertAfter(posPrimCur, pPrimDim);
					pSeg->RemoveAt(posPrimCur);
					delete pPrim;
					return ptProj;
				}
				else if (pPrim->Is(CPrim::PRIM_DIM))
				{
					CPrimDim* pPrimDim = static_cast<CPrimDim*>(pPrim);
					CRefSys rs;
					pPrimDim->GetRefSys(rs);
					CPrimLine* pPrimLine = new CPrimLine(pPrimDim->PenColor(), pPrimDim->PenStyle(), pPrimDim->Line());
					CPrimText* pPrimText = new CPrimText(pPrimDim->FontDef(), rs, pPrimDim->Text());
					pPrimText->PenColor() = pPrimDim->TextPenColor();
					pSeg->InsertAfter(posPrimCur, pPrimLine);
					pSeg->InsertAfter(posPrimCur, pPrimText);
					pSeg->RemoveAt(posPrimCur);
					delete pPrim;
					return ptProj;
				}				
			}
		}
	}
	return (pt);
}				
CPnt dimension::ProjPtToLn(CPnt pt)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CLine ln;
	CPnt ptProj;

	double dDis;
	double dRel[2];

	POSITION pos = pDoc->WorkLayerGetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = pDoc->WorkLayerGetNext(pos);

		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim != 0)
		{
			CPrim* pPrim = pSeg->GetNext(posPrim);

			if (pPrim->Is(CPrim::PRIM_LINE))
				static_cast<CPrimLine*>(pPrim)->GetLine(ln);
			else if (pPrim->Is(CPrim::PRIM_DIM))
				ln = static_cast<CPrimDim*>(pPrim)->Line();
			else
				continue;
			
			if (line::EvalPtProx_xy(ln, pt, .05, ptProj, dRel, &dDis)) 
				return (*dRel <= .5) ? ln[0] : ln[1];
		}
	}
	return (pt);
}				

