#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "SubProcPipe.h"

#include "UserAxis.h"

namespace pipe
{
	double	dTicSize = .03125;
	double	dCSize = .03125;
	WORD	wCurSymId = 0;
}

LRESULT CALLBACK SubProcPipe(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	CPegView* pView = CPegView::GetActiveView();
	CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();

	static CPnt pt[6];
	
	static WORD wPrvKeyDwn = 0;
	
	static CPrimArc* pArcVer;
	static CPrimLine* pLineHor = 0;
	
	static pipe::EJoin eJoinHor = pipe::No;
	static pipe::EJoin eJoinVer = pipe::No;
	
	if (nMsg == WM_COMMAND)
	{
		POSITION pos;
		PENCOLOR nPenColor;
		PENSTYLE nPenStyle;

		pt[1] = app.CursorPosGet();
		
		switch (LOWORD(wParam))
		{
			case ID_OP0:
				pDoc->SetOptionsFocus(IDC_TIC_SIZE);
				pDoc->SetOptionsPage(CPegDoc::TAB_OPEN_PIPE);
				::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_TABS_MAIN), app.GetSafeHwnd(), DlgProcSetup);
				break;
			
			case ID_OP1:	// Join existing component of pipe system
				
				pLineHor = 0;
				eJoinHor = pipe::No;
				
				pArcVer = 0;
				eJoinVer = pipe::No;
				
				pos = detsegs.GetHeadPosition();
				while (eJoinHor == pipe::No && eJoinVer == pipe::No && pos != 0)
				{
					CSeg* pSeg = detsegs.GetNext(pos);
					POSITION posPrim = pSeg->GetHeadPosition();
					while (eJoinHor == pipe::No && eJoinVer == pipe::No && posPrim != 0)
					{
						CPrim* pPrim = pSeg->GetNext(posPrim);
						if (pPrim->Is(CPrim::PRIM_ARC))
						{	// Primitive is potentially an existing vertical pipe section
							CPrimArc* pArc = static_cast<CPrimArc*>(pPrim);
							if (fabs(pArc->GetSwpAng() - TWOPI) <= DBL_EPSILON) 
							{	
								if (CVec(pArc->Center(), pt[1]).Length() <= pipe::dCSize)
								{
									pArcVer = pArc;
									eJoinVer = (wPrvKeyDwn == 0) ? pipe::Begin : pipe::End;
									pt[1] = pArc->Center();
								}
							}
						}
						else if (pPrim->Is(CPrim::PRIM_LINE))
						{	// Primitive is potentially an existing horizontal pipe section
							CPrimLine* pLine = static_cast<CPrimLine*>(pPrim);

							CPnt4 ptView(pt[1], 1.);
							ModelView_DoTransform(ptView);

							CPnt ptProj;

							if (pLine->SelUsingPoint(ptView, .02, ptProj))
							{
								pLineHor = pLine;
								eJoinHor = (wPrvKeyDwn == 0) ? pipe::Begin : pipe::End;
								pt[1] = ptProj;
								pt[1] = view::GetMatrixInverse() * pt[1];
							}
						}
					}
				}
				if (eJoinHor == pipe::No && eJoinVer == pipe::No)
				{
					wPrvKeyDwn = 0;
					// error
				}
				break;
				
			case ID_OP2:
				if (wPrvKeyDwn == 0) 
					pt[0] = pt[1];
				else 
				{
					pt[1] = UserAxis::UserAxisSnapLn(pt[0], pt[1]);
					
					if (CVec(pt[0], pt[1]).Length() > pipe::dCSize) 
						pipe::GenFits(wPrvKeyDwn, pt[0], ID_OP2, pt[1]);
					else 
						;	// error
					
					pt[0] = pt[1];
				}
				pLineHor = 0;
				eJoinHor = pipe::No;
				
				pArcVer = 0;
				eJoinVer = pipe::No;
				
				wPrvKeyDwn = ID_OP2;
				app.RubberBandingStartAtEnable(pt[0], Lines);
				break;
	
			case ID_OP3:
				if (eJoinHor != pipe::No) 
				{	// On an existing horizontal pipe section
					nPenColor = pstate.PenColor();
					nPenStyle = pstate.PenStyle();
					pstate.SetPen(pDC, pLineHor->PenColor(), pLineHor->PenStyle());
					
					CPnt ptBeg = pLineHor->Pt0();
					CPnt ptEnd = pLineHor->Pt1();
					
					pt[1] = CLine(ptBeg, ptEnd).ProjPt(pt[1]);

					pLineHor->SetPt1(pt[1]);
					
					pipe::GenTicMark(pt[1], ptBeg, pipe::dCSize);
					
					CSeg* pSeg = new CSeg(new CPrimLine(pt[1], ptEnd));
					pDoc->WorkLayerAddTail(pSeg);
					pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
					
					pipe::GenTicMark(pt[1], ptEnd, pipe::dCSize);
					
					pstate.SetPen(pDC, nPenColor, nPenStyle);
					
					if (eJoinHor == pipe::End)
						// Ending on an existing horizontal pipe section
						pipe::GenFits(wPrvKeyDwn, pt[0], ID_OP3, pt[1]);
					
					wPrvKeyDwn = ID_OP3;
					
					pLineHor = 0;
					eJoinHor = pipe::No;
				}
				else if(eJoinVer != pipe::No) 
				{	// On an existing vertical pipe section
					if (eJoinVer == pipe::End)
						// Ending on an existing vertical pipe section
						pipe::GenFits(wPrvKeyDwn, pt[0], ID_OP5, pt[1]);
					wPrvKeyDwn = ID_OP4;
					pArcVer = 0;
					eJoinVer = pipe::No;
				}
				else 
				{
					if (wPrvKeyDwn != 0) 
					{
						pt[1] = UserAxis::UserAxisSnapLn(pt[0], pt[1]);
						pipe::GenFits(wPrvKeyDwn, pt[0], ID_OP3, pt[1]);
					}
					wPrvKeyDwn = ID_OP3;
				}
				pt[0] = pt[1];
				app.RubberBandingStartAtEnable(pt[0], Lines);
				break;

			case ID_OP4:

				if (eJoinHor != pipe::No) 
				{		// On an existing horizontal pipe section
					nPenColor = pstate.PenColor();
					nPenStyle = pstate.PenStyle();
					pstate.SetPen(pDC, pLineHor->PenColor(), pLineHor->PenStyle());
					
					CPnt ptBeg = pLineHor->Pt0();
					CPnt ptEnd = pLineHor->Pt1();
					
					pt[1] = CLine(ptBeg, ptEnd).ProjPt(pt[1]);

					if (eJoinHor == pipe::End) 
					{	// Dropping into an existing horizontal pipe section
						pt[3] = Pnt_ProjPtTo(pt[1], ptBeg, pipe::dCSize);
						pt[4] = Pnt_ProjPtTo(pt[1], ptEnd, pipe::dCSize);
						
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_PRIM_ERASE_SAFE, pLineHor); 
						pLineHor->SetPt1(pt[3]);
						pLineHor->Display(pDC, true); 
						
						pipe::GenTicMark(pt[3], ptBeg, pipe::dCSize);
						
						CSeg* pSeg = new CSeg(new CPrimLine(pt[4], ptEnd));
						pDoc->WorkLayerAddTail(pSeg);
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);

						pipe::GenTicMark(pt[4], ptEnd, pipe::dCSize);
						
						pstate.SetPen(pDC, 1, 1);
						
						pipe::GenDrop(pt[1]);
						
						pstate.SetPen(pDC, nPenColor, nPenStyle);
						pipe::GenFits(wPrvKeyDwn, pt[0], ID_OP4, pt[1]);
					}
					else 
					{	// Dropping from an existing horizontal pipe section
						pLineHor->SetPt1(pt[1]);
						
						pipe::GenTicMark(pt[1], ptBeg, 2. * pipe::dCSize);
						
						CSeg* pSeg = new CSeg(new CPrimLine(pt[1], ptEnd));
						pDoc->WorkLayerAddTail(pSeg);
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);

						pipe::GenTicMark(pt[1], ptEnd, 2. * pipe::dCSize);
						
						pstate.SetPen(pDC, 1, 1);
						
						pipe::GenDrop(pt[1]);
						
						pstate.SetPen(pDC, nPenColor, nPenStyle);
					}
					wPrvKeyDwn = ID_OP4;
					
					pLineHor = 0;
					eJoinHor = pipe::No;
				}
				else if (eJoinVer != pipe::No) 
				{		// On an existing vertical pipe section
					if (eJoinVer == pipe::End)
						// Dropping into an existing vertical pipe section
						pipe::GenFits(wPrvKeyDwn, pt[0], ID_OP4, pt[1]);
					wPrvKeyDwn = ID_OP4;
					pArcVer = 0;
					eJoinVer = pipe::No;
				}
				else 
				{
					nPenColor = pstate.PenColor();
					nPenStyle = pstate.PenStyle();
					
					if (wPrvKeyDwn != 0) 
					{
						pt[1] = UserAxis::UserAxisSnapLn(pt[0], pt[1]);
						pipe::GenFits(wPrvKeyDwn, pt[0], ID_OP4, pt[1]);
					}
					pstate.SetPen(pDC, 1, 1);
					
					pipe::GenDrop(pt[1]);
					
					pstate.SetPen(pDC, nPenColor, nPenStyle);
					wPrvKeyDwn = ID_OP4;
				}
				pt[0] = pt[1];
				app.RubberBandingStartAtEnable(pt[0], Lines);
				break;
			
			case ID_OP5:
				if (eJoinHor != pipe::No) 
				{	// On an existing horizontal pipe section
					nPenColor = pstate.PenColor();
					nPenStyle = pstate.PenStyle();
					pstate.SetPen(pDC, pLineHor->PenColor(), pLineHor->PenStyle());
				
					CPnt ptBeg = pLineHor->Pt0();
					CPnt ptEnd = pLineHor->Pt1();

					pt[1] = CLine(ptBeg, ptEnd).ProjPt(pt[1]);

					if (eJoinHor == pipe::End)
					{	// Rising into an existing horizontal pipe section
						pLineHor->SetPt1(pt[1]);
						pipe::GenTicMark(pt[1], ptBeg, 2. * pipe::dCSize);
					
						CSeg* pSeg = new CSeg(new CPrimLine(pt[1], ptEnd));
						pDoc->WorkLayerAddTail(pSeg);
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
						
						pipe::GenTicMark(pt[1], ptEnd, 2. * pipe::dCSize);
						pstate.SetPen(pDC, 1, 1);
					
						pipe::GenRise(pt[1]);

						pstate.SetPen(pDC, nPenColor, nPenStyle);
						pipe::GenFits(wPrvKeyDwn, pt[0], ID_OP5, pt[1]);
					}
					else 
					{	// Rising from an existing horizontal pipe section
						pt[3] = Pnt_ProjPtTo(pt[1], ptBeg, pipe::dCSize);
						pt[4] = Pnt_ProjPtTo(pt[1], ptEnd, pipe::dCSize);
					
						pLineHor->SetPt1(pt[3]);
						pipe::GenTicMark(pt[3], ptBeg, pipe::dCSize);
					
						CSeg* pSeg = new CSeg(new CPrimLine(pt[4], ptEnd));
						pDoc->WorkLayerAddTail(pSeg);
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);					
						
						pipe::GenTicMark(pt[4], ptEnd, pipe::dCSize);
						pstate.SetPen(pDC, 1, 1);
						
						pipe::GenRise(pt[1]);

						pstate.SetPen(pDC, nPenColor, nPenStyle);
					}
					wPrvKeyDwn = ID_OP5;
				
					pLineHor = 0;
					eJoinHor = pipe::No;
				}
				else if (eJoinVer != pipe::No) 
				{		// On an existing vertical pipe section
					if (eJoinVer == pipe::End)
						// Rising into an existing vertical pipe section
						pipe::GenFits(wPrvKeyDwn, pt[0], ID_OP5, pt[1]);
					wPrvKeyDwn = ID_OP5;
					pArcVer = 0;
					eJoinVer = pipe::No;
				}
				else 
				{	
					nPenColor = pstate.PenColor();
					nPenStyle = pstate.PenStyle();
					if (wPrvKeyDwn != 0) 
					{
						pt[1] = UserAxis::UserAxisSnapLn(pt[0], pt[1]);
						pipe::GenFits(wPrvKeyDwn, pt[0], ID_OP5, pt[1]);
					}
					pstate.SetPen(pDC, 1, 1);
					pipe::GenRise(pt[1]);
					pstate.SetPen(pDC, nPenColor, nPenStyle);
					wPrvKeyDwn = ID_OP5;
				}
				pt[0] = pt[1];
				app.RubberBandingStartAtEnable(pt[0], Lines);
				break;
		
			case ID_OP8:
				if (wPrvKeyDwn != 0) 
					app.RubberBandingDisable();
				wPrvKeyDwn = 0;
				pLineHor = 0;
				eJoinHor = pipe::No;
				pArcVer = 0;
				eJoinVer = pipe::No;
			
				pipe::GenSyms(pDC, pt[1]);
				break;
	
			case ID_OP9:	// Generate wye
				if (eJoinHor == pipe::End)
				{	// Save location of point on existing horizontal pipe
					pt[2] = pt[1];
					pt[1] = pLineHor->Ln().ProjPt(pt[0]);
					
					double d1 = CVec(pt[0], pt[1]).Length();
				
					if (d1 >= .25) 
					{
						nPenColor = pstate.PenColor();
						nPenStyle = pstate.PenStyle();
											
						CPnt ptBeg = pLineHor->Pt0();
						CPnt ptEnd = pLineHor->Pt1();

						double d2 = CVec(pt[1], pt[2]).Length();
						
						if (fabs(d2 - d1) <= .25) 
						{
							pt[4] = Pnt_ProjPtTo(pt[1], pt[2], d1);
							
							pLineHor->SetPt1(pt[4]);
							pipe::GenTicMark(pt[4], ptBeg, pipe::dCSize);
							
							pipe::GenFits(ID_OP3, pt[4], ID_OP2, ptEnd);
							pstate.SetPen(pDC, nPenColor, nPenStyle);
							pipe::GenFits(wPrvKeyDwn, pt[0], ID_OP3, pt[4]);
						}
						else 
						{
							double d3;
							if (d2 - .25 <= d1) 
							{
								d3 = .125;
								if (d2 > .25) d3 = d2;
								pt[3] = Pnt_ProjPtTo(pt[1], pt[0], d3);
								pt[4] = Pnt_ProjPtTo(pt[1], pt[2], d3);
							}
							else 
							{
								pt[3] = Pnt_ProjPtTo(pt[1], pt[2], d2 - d1);
								
								pt[3][0] = pt[3][0] + pt[0][0] - pt[1][0];
								pt[3][1] = pt[3][1] + pt[0][1] - pt[1][1];
//Note .. what about p4z
								pt[4] = pt[2];
								d3 = d2 - d1;
							}
							pLineHor->SetPt1(pt[4]);
							pipe::GenTicMark(pt[4], ptBeg, pipe::dCSize);
							
							pipe::GenFits(ID_OP3, pt[4], ID_OP2, ptEnd);
							pstate.SetPen(pDC, nPenColor, nPenStyle);
							pipe::GenFits(wPrvKeyDwn, pt[0], ID_OP3, pt[3]);
							pipe::GenFits(ID_OP3, pt[3], ID_OP3, pt[4]);
						}
						pt[0] = pt[4];
						pLineHor = 0;
						eJoinHor = pipe::No;
						wPrvKeyDwn = ID_OP3;
						app.RubberBandingStartAtEnable(pt[0], Lines);
					}
					else 
						// error
						pt[1] = pt[2];
				}
				else 
					// error
					pt[1] = pt[2];

				break;

			case IDM_RETURN:
			case IDM_ESCAPE:
				if (wPrvKeyDwn != 0) 
					app.RubberBandingDisable();
				else
					pDoc->DoEditPrimitiveEscape();

				wPrvKeyDwn = 0;
				pLineHor = 0;
				eJoinHor = pipe::No;
				pArcVer = 0;
				eJoinVer = pipe::No;

				pt[0] = pt[1];
				break;
		}	
	}
	return (CallWindowProc(app.GetMainWndProc(), hwnd, nMsg, wParam, lParam));
}

///<summary>
///Adds a fitting indication to horizontal pipe section as required by previous fitting type.
///</summary>
void pipe::GenFits(WORD wPrvKeyDwn, const CPnt& PP, WORD CUR, const CPnt& PC)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CPnt pt1 = PP;
	CPnt pt2 = PC;
	
	if (wPrvKeyDwn == ID_OP3) 
		// Previous fitting is an elbow or side tee
		pipe::GenTicMark(PP, PC, pipe::dCSize);
	else if (wPrvKeyDwn == ID_OP4) 
	{	// Previous fitting is an elbow down, riser down or bottom tee
		pt1 = Pnt_ProjPtTo(PP, PC, pipe::dCSize);
		pipe::GenTicMark(pt1, PC, pipe::dCSize);
	}
	else if (wPrvKeyDwn == ID_OP5) 
		// Previous fitting is an elbow up, riser up or top tee
		pipe::GenTicMark(PP, PC, 2. * pipe::dCSize);
	
	if (CUR == ID_OP3) 
		// Current fitting is an elbow or side tee
		pipe::GenTicMark(PC, PP, pipe::dCSize);
	else if (CUR == ID_OP4) 
		// Current fitting is an elbow down, riser down or bottom tee
		pipe::GenTicMark(PC, PP, 2. * pipe::dCSize);
	else if (CUR == ID_OP5) 
	{	// Current fitting is an elbow up, riser up or top tee
		pt2 = Pnt_ProjPtTo(PC, PP, pipe::dCSize);
		pipe::GenTicMark(PC, PP, 2. * pipe::dCSize);
	}
	CSeg* pSeg = new CSeg(new CPrimLine(pt1, pt2));
	
	pDoc->WorkLayerAddTail(pSeg);
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
}

///<summary>Generates a piping symbol at point specified if pipe section located.</summary>
void pipe::GenSyms(CDC* pDC, const CPnt& P0)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	double dSymSiz[] =
	{
		.09375, .09375, .09375, .09375, .125, .125, .125, .125, .125, .125, 
		.125, .125, .125, .125, .125, 0., 0., .09375
	};
	double dTicDis[] =
	{
		.125, .125, .125, .125, .15625, .15625, .15625, .15625, .15625, .15625,
		.15625, .15625, .15625, .15625, .15625, .03125, .03125, .125
	};

	CPnt ptIns;
	
	CPrimLine* pLine = 0;
	
	POSITION pos = detsegs.GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = detsegs.GetNext(pos);
		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim != 0)
		{
			CPrim* pPrim = pSeg->GetNext(posPrim);
			if (pPrim->Is(CPrim::PRIM_LINE))
			{	// Primitive is potentially an existing horizontal pipe section
				pLine = static_cast<CPrimLine*>(pPrim);

				CPnt4 ptView(P0, 1.);
				ModelView_DoTransform(ptView);

				if (pLine->SelUsingPoint(ptView, .02, ptIns))
				{
					ptIns = view::GetMatrixInverse() * ptIns;

					goto FoundIt;
				}
			}	
		}
	}
	// error
	return;
				
FoundIt:

	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SUBPROC_PIPE_SYMBOL), app.GetSafeHwnd(), DlgProcPipeSymbol);

	CPnt pt[5];
	
	CPnt ptBeg = pLine->Pt0();
	CPnt ptEnd = pLine->Pt1();

	CLine lnInsBeg(ptIns, ptBeg);
	CLine lnInsEnd(ptIns, ptEnd);

	PENCOLOR nPenColor = pstate.PenColor();
	PENSTYLE nPenStyle = pstate.PenStyle();
	pstate.SetPen(pDC, pLine->PenColor(), pLine->PenStyle());
	
	// Regenerate pipe section with gap for insertion of fitting specified

	CSeg* pSeg;

	pt[2] = Pnt_ProjPtTo(ptIns, ptBeg, dSymSiz[pipe::wCurSymId]);
	pLine->SetPt1(pt[2]);
	
	pipe::GenTicMark(ptIns, ptBeg, dTicDis[pipe::wCurSymId]);
	
	pt[3] = Pnt_ProjPtTo(ptIns, ptEnd, dSymSiz[pipe::wCurSymId]);
	
	pSeg = new CSeg(new CPrimLine(pt[3], ptEnd));
	pDoc->WorkLayerAddTail(pSeg);
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);	
	
	pipe::GenTicMark(ptIns, ptEnd, dTicDis[pipe::wCurSymId]);
	
	pstate.SetPen(pDC, nPenColor, nPenStyle);
	
	double dTicSize = pipe::dTicSize;

	pSeg = new CSeg;

	switch (pipe::wCurSymId)
	{
		case 0:		// Generate flow switch
			pSeg->AddTail(new CPrimArc(ptIns, pt[2]));
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[0], - dSymSiz[0] * 1.5, &pt[0]);
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[0], - dSymSiz[0] * 2., &pt[1]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[0], dSymSiz[0] * 1.5, &pt[2]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[0], dSymSiz[0] * 2., &pt[3]);
			pSeg->AddTail(new CPrimLine(pt[1], pt[3]));
			pSeg->AddTail(new CPrimLine(pt[3], pt[2]));
			pSeg->AddTail(new CPrimLine(pt[2], pt[0]));
			pSeg->AddTail(new CPrimLine(pt[0], pt[1]));
			pSeg->AddTail(new CPrimLine(pt[1], pt[2]));
			break;
		
		case 1:		// Generate float and thermostatic trap
			pSeg->AddTail(new CPrimArc(ptIns, pt[2]));
			pt[0] = Pnt_RotateAboutPt_xy(pt[2], ptIns, PI / 4.);
			pt[1] = Pnt_RotateAboutPt_xy(pt[0], ptIns, PI);
			pSeg->AddTail(new CPrimLine(pt[0], pt[1]));
			pt[0] = Pnt_RotateAboutPt_xy(pt[2], ptIns, 3. * PI / 4.);
			pt[1] = Pnt_RotateAboutPt_xy(pt[0], ptIns, PI);
			pSeg->AddTail(new CPrimLine(pt[0], pt[1]));
			break;

		case 2:
			pSeg->AddTail(new CPrimArc(ptIns, pt[2]));
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[2], dSymSiz[2] * 1.5, &pt[0]);
			line::ProjPtFrom_xy(lnInsEnd, 0., dSymSiz[2] * 1.5, &pt[1]);
			pSeg->AddTail(new CPrimLine(pt[0], pt[1]));
			line::ProjPtFrom_xy(lnInsEnd, 0., dSymSiz[2], &pt[2]);
			pSeg->AddTail(new CPrimLine(pt[1], pt[2]));
			break;

		case 3: 
			pSeg->AddTail(new CPrimArc(ptIns, pt[2]));
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[3], dSymSiz[3] * 1.5, &pt[0]);
			line::ProjPtFrom_xy(lnInsEnd, 0., dSymSiz[3] * 1.5, &pt[1]);
			pSeg->AddTail(new CPrimLine(pt[0], pt[1]));
			line::ProjPtFrom_xy(lnInsBeg, 0., dSymSiz[3], &pt[2]);
			pSeg->AddTail(new CPrimLine(pt[1], pt[2]));
			break;

		case 4:
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[4], dSymSiz[4] * .5, &pt[0]);
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[4], - dSymSiz[4] * .5, &pt[1]);
			pSeg->AddTail(new CPrimLine(pt[0], pt[1]));
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[4], - dSymSiz[4] * .5, &pt[2]);
			pSeg->AddTail(new CPrimLine(pt[1], pt[2]));
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[4], dSymSiz[4] * .5, &pt[3]);
			pSeg->AddTail(new CPrimLine(pt[2], pt[3]));
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[4], - dSymSiz[4] * .3, &pt[0]);
			pSeg->AddTail(new CPrimArc(pt[2], pt[0]));
			break;

		case 5:
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[5], dSymSiz[5] * .5, &pt[0]);
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[5], - dSymSiz[5] * .5, &pt[1]);
			pSeg->AddTail(new CPrimLine(pt[0], pt[1]));
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[5], - dSymSiz[5] * .5, &pt[2]);
			pSeg->AddTail(new CPrimLine(pt[1], pt[2]));
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[5], dSymSiz[5] * .5, &pt[3]);
			pSeg->AddTail(new CPrimLine(pt[2], pt[3]));
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[5], - dSymSiz[5] * .3, &pt[0]);
			pSeg->AddTail(new CPrimArc(pt[2], pt[0]));
			pSeg->AddTail(new CPrimLine(pt[3], ptIns));
			break;

		case 6:		// Generate gate valve
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[6], dSymSiz[6] * .5, &pt[0]);
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[6], - dSymSiz[6] * .5, &pt[1]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[6], - dSymSiz[6] * .5, &pt[2]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[6], dSymSiz[6] * .5, &pt[3]);
			pSeg->AddTail(new CPrimLine(pt[0], pt[1]));
			pSeg->AddTail(new CPrimLine(pt[1], pt[2]));
			pSeg->AddTail(new CPrimLine(pt[2], pt[3]));
			pSeg->AddTail(new CPrimLine(pt[3], pt[0]));
			break;

		case 7:		// Generate globe valve
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[7], dSymSiz[7] * .5, &pt[0]);
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[7], - dSymSiz[7] * .5, &pt[1]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[7], - dSymSiz[7] * .5, &pt[2]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[7], dSymSiz[7] * .5, &pt[3]);
			pSeg->AddTail(new CPrimLine(pt[0], pt[1]));
			pSeg->AddTail(new CPrimLine(pt[1], pt[2]));
			pSeg->AddTail(new CPrimLine(pt[2], pt[3]));
			pSeg->AddTail(new CPrimLine(pt[3], pt[0]));
			pt[0] = Pnt_ProjPtTo(ptIns, ptEnd, dSymSiz[7] * .25);
			pSeg->AddTail(new CPrimArc(ptIns, pt[0]));
			break;

		case 8:		// Generate stop check valve
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[8], dSymSiz[8] * .5, &pt[0]);
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[8], - dSymSiz[8] * .5, &pt[1]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[8], - dSymSiz[8] * .5, &pt[2]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[8], dSymSiz[8] * .5, &pt[3]);
			pSeg->AddTail(new CPrimLine(pt[0], pt[1]));
			pSeg->AddTail(new CPrimLine(pt[1], pt[2]));
			pSeg->AddTail(new CPrimLine(pt[2], pt[3]));
			pSeg->AddTail(new CPrimLine(pt[3], pt[0]));
			pt[0] = Pnt_ProjPtTo(ptIns, ptEnd, dSymSiz[8] * .25);
			pSeg->AddTail(new CPrimArc(ptIns, pt[0]));
			line::ProjPtFrom_xy(lnInsEnd, 0., dSymSiz[8], &pt[0]);
			pSeg->AddTail(new CPrimLine(ptIns, pt[0]));
			pipe::dTicSize = dSymSiz[8] * .25;
			pipe::GenTicMark(ptIns, pt[0], dSymSiz[8] * .75);
			break;

		case 9:	// Generate pressure reducing valve
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[9], dSymSiz[9] * .5, &pt[0]);
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[9], - dSymSiz[9] * .5, &pt[1]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[9], - dSymSiz[9] * .5, &pt[2]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[9], dSymSiz[9] * .5, &pt[3]);
			pSeg->AddTail(new CPrimLine(pt[0], pt[1]));
			pSeg->AddTail(new CPrimLine(pt[1], pt[2]));
			pSeg->AddTail(new CPrimLine(pt[2], pt[3]));
			pSeg->AddTail(new CPrimLine(pt[3], pt[0]));
			pt[0] = Pnt_ProjPtTo(ptIns, ptEnd, dSymSiz[9] * .25);
			pSeg->AddTail(new CPrimArc(ptIns, pt[0]));
			line::ProjPtFrom_xy(lnInsEnd, 0., dSymSiz[9], &pt[0]);
			pSeg->AddTail(new CPrimLine(ptIns, pt[0]));
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[9] * .5, dSymSiz[9] * .75, &pt[1]);
			pSeg->AddTail(new CPrimLine(pt[0], pt[1]));
			line::ProjPtFrom_xy(lnInsEnd, 0., dSymSiz[9] * .5, &pt[2]);
			pSeg->AddTail(new CPrimLine(pt[1], pt[2]));
			break;

		case 10:
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[10], dSymSiz[10] * .5, &pt[0]);
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[10], - dSymSiz[10] * .5, &pt[1]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[10], - dSymSiz[10] * .5, &pt[2]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[10], dSymSiz[10] * .5, &pt[3]);
			pSeg->AddTail(new CPrimLine(pt[0], pt[1]));
			pSeg->AddTail(new CPrimLine(pt[1], pt[2]));
			pSeg->AddTail(new CPrimLine(pt[2], pt[3]));
			pSeg->AddTail(new CPrimLine(pt[3], pt[0]));
			line::ProjPtFrom_xy(lnInsEnd, 0., dSymSiz[10] * .5, &pt[0]);
			pSeg->AddTail(new CPrimLine(ptIns, pt[0]));
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[10] * .25, dSymSiz[10] * .5, &pt[0]);
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[10] * .25, dSymSiz[10] * .75, &pt[1]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[10] * .25, - dSymSiz[10] * .75, &pt[2]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[10] * .25, - dSymSiz[10] * .5, &pt[3]);
			pSeg->AddTail(new CPrimLine(pt[0], pt[1]));
			pSeg->AddTail(new CPrimLine(pt[1], pt[2]));
			pSeg->AddTail(new CPrimLine(pt[2], pt[3]));
			pSeg->AddTail(new CPrimLine(pt[3], pt[0]));
			break;

		case 11:	// Generate automatic 3-way valve
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[11], dSymSiz[11] * .5, &pt[0]);
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[11], - dSymSiz[11] * .5, &pt[1]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[11], - dSymSiz[11] * .5, &pt[2]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[11], dSymSiz[11] * .5, &pt[3]);
			pSeg->AddTail(new CPrimLine(pt[0], pt[1]));
			pSeg->AddTail(new CPrimLine(pt[1], pt[2]));
			pSeg->AddTail(new CPrimLine(pt[2], pt[3]));
			pSeg->AddTail(new CPrimLine(pt[3], pt[0]));
			line::ProjPtFrom_xy(lnInsEnd, 0., dSymSiz[11] * .5, &pt[0]);
			pSeg->AddTail(new CPrimLine(ptIns, pt[0]));
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[11] * .25, dSymSiz[11] * .5, &pt[0]);
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[11] * .25, dSymSiz[11] * .75, &pt[1]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[11] * .25, - dSymSiz[11] * .75, &pt[2]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[11] * .25, - dSymSiz[11] * .5, &pt[3]);
			pSeg->AddTail(new CPrimLine(pt[0], pt[1]));
			pSeg->AddTail(new CPrimLine(pt[1], pt[2]));
			pSeg->AddTail(new CPrimLine(pt[2], pt[3]));
			pSeg->AddTail(new CPrimLine(pt[3], pt[0]));
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[11] * .5, - dSymSiz[11], &pt[0]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[11] * .5, dSymSiz[11], &pt[1]);
			pSeg->AddTail(new CPrimLine(ptIns, pt[0]));
			pSeg->AddTail(new CPrimLine(pt[0], pt[1]));
			pSeg->AddTail(new CPrimLine(pt[1], ptIns));
			break;

		case 12:	// Generate self operated valve
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[12], dSymSiz[12] * .5, &pt[0]);
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[12], - dSymSiz[12] * .5, &pt[1]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[12], - dSymSiz[12] * .5, &pt[2]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[12], dSymSiz[12] * .5, &pt[3]);
			pSeg->AddTail(new CPrimLine(pt[0], pt[1]));
			pSeg->AddTail(new CPrimLine(pt[1], pt[2]));
			pSeg->AddTail(new CPrimLine(pt[2], pt[3]));
			pSeg->AddTail(new CPrimLine(pt[3], pt[0]));
			line::ProjPtFrom_xy(lnInsEnd, 0., dSymSiz[12] * .5, &pt[0]);
			pSeg->AddTail(new CPrimLine(ptIns, pt[0]));
			line::ProjPtFrom_xy(lnInsEnd, dSymSiz[12] * .25, dSymSiz[12] * .5, &pt[1]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[12] * .25, - dSymSiz[12] * .5, &pt[2]);
			pSeg->AddTail(new CPrimLine(pt[1], pt[2]));
//Note .. end is undefined
			// P_ADD_ARC_TO_SEG_STO(SEG_STO_FRE, pt[0], - PI, pt[2]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[12] * 1.25, - dSymSiz[12] * .5, &pt[0]);
			
			// save line type and set at 2

			pSeg->AddTail(new CPrimLine(pt[2], pt[0]));
			
			// restore line type

			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[12] * 1.25, - dSymSiz[12] * .75, &pt[1]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[12] * 2., - dSymSiz[12] * .75, &pt[2]);
			line::ProjPtFrom_xy(lnInsBeg, dSymSiz[12] * 2., - dSymSiz[12] * .5, &pt[3]);
			pSeg->AddTail(new CPrimLine(pt[0], pt[1]));
			pSeg->AddTail(new CPrimLine(pt[1], pt[2]));
			pSeg->AddTail(new CPrimLine(pt[2], pt[3]));
			pSeg->AddTail(new CPrimLine(pt[3], pt[0]));
			break;

		case 13:
			line::ProjPtFrom_xy(lnInsEnd, 0., - dSymSiz[13], &pt[0]);
			line::ProjPtFrom_xy(lnInsEnd, 0., dSymSiz[13], &pt[1]);
			pSeg->AddTail(new CPrimLine(pt[3], pt[1]));
			pSeg->AddTail(new CPrimLine(pt[1], pt[2]));
			pSeg->AddTail(new CPrimLine(pt[2], pt[0]));
			pSeg->AddTail(new CPrimLine(pt[0], pt[3]));
			break;

		case 14:
			line::ProjPtFrom_xy(lnInsEnd, 0., - dSymSiz[14], &pt[0]);
			line::ProjPtFrom_xy(lnInsEnd, 0., dSymSiz[14], &pt[1]);
			pSeg->AddTail(new CPrimLine(pt[3], pt[1]));
			pSeg->AddTail(new CPrimLine(pt[1], pt[2]));
			pSeg->AddTail(new CPrimLine(pt[2], pt[0]));
			pSeg->AddTail(new CPrimLine(pt[0], pt[3]));
			pSeg->AddTail(new CPrimLine(pt[0], pt[1]));
			break;

		case 15:
			line::ProjPtFrom_xy(lnInsEnd, 0., - .250, &pt[0]);
			pSeg->AddTail(new CPrimLine(ptIns, pt[0]));
			pipe::GenTicMark(ptIns, pt[0], dTicDis[15]);
			line::ProjPtFrom_xy(lnInsBeg, .0625, .1875, &pt[1]);
			line::ProjPtFrom_xy(lnInsEnd, .0625, - .1875, &pt[2]);
			line::ProjPtFrom_xy(lnInsEnd, .0625, - .125, &pt[3]);
			pSeg->AddTail(new CPrimLine(pt[1], pt[2]));
			pSeg->AddTail(new CPrimLine(pt[2], pt[3]));
			break;

		case 16:
			line::ProjPtFrom_xy(lnInsEnd, 0., - .250, &pt[0]);
			pSeg->AddTail(new CPrimLine(ptIns, pt[0]));
			pipe::GenTicMark(ptIns, pt[0], dTicDis[16]);
			line::ProjPtFrom_xy(lnInsBeg, .0625, .1875, &pt[1]);
			line::ProjPtFrom_xy(lnInsEnd, .0625, - .1875, &pt[2]);
			line::ProjPtFrom_xy(lnInsEnd, .0625, - .125, &pt[3]);
			pSeg->AddTail(new CPrimLine(pt[1], pt[2]));
			pSeg->AddTail(new CPrimLine(pt[2], pt[3]));
			pt[1] = Pnt_ProjPtTo(ptIns, pt[0], .28125);
			pSeg->AddTail(new CPrimArc(pt[1], pt[0]));
			break;

		case 17:	// Generate union
			pipe::dTicSize = dSymSiz[17];
			pipe::GenTicMark(ptIns, ptBeg, dSymSiz[17]);
			pipe::GenTicMark(ptIns, ptEnd, dSymSiz[17]);
			pipe::dTicSize *= 2;
			pipe::GenTicMark(ptIns, ptBeg, 0.);
			break;
	}
	
	pipe::dTicSize = dTicSize;

	pDoc->WorkLayerAddTail(pSeg);
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
}

//	Effect: Draws tic mark at a point dDis from begin point on the line defined by
//			begin and end points.
void pipe::GenTicMark(const CPnt& ptBeg, const CPnt& ptEnd, double dDis)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CPnt ptProj = Pnt_ProjPtTo(ptBeg, ptEnd, dDis);
	
	double dProjEnd = CVec(ptProj, ptEnd).Length();
	
	if (dProjEnd > DBL_EPSILON)
	{	// Tic does not intersect line at end point
		
		CVec vProjEnd(ptProj, ptEnd);
		vProjEnd *= pipe::dTicSize / dProjEnd;
		
		CPnt pt1(ptProj);
		pt1 += CVec(vProjEnd[1], - vProjEnd[0], 0.);
				
		CPnt pt2(ptProj);
		pt2 += CVec(- vProjEnd[1], vProjEnd[0], 0.);
		
		CSeg* pSeg = new CSeg(new CPrimLine(1, 1, pt1, pt2));
		pDoc->WorkLayerAddTail(pSeg);
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
	}
}

void pipe::GenRise(const CPnt& ptCent)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CPnt ptBeg = ptCent;
	
	ptBeg[0] = ptBeg[0] + pipe::dCSize;

	CVec vPlnNorm = - view::GetDirection();
	CVec vXAx(ptCent, ptBeg);
	CVec vYAx = vXAx;
	vYAx.RotAboutArbAx(vPlnNorm, HALF_PI);
						
	CSeg* pSeg = new CSeg(new CPrimArc(ptCent, vXAx, vYAx, TWOPI));
	pDoc->WorkLayerAddTail(pSeg);
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
}

void pipe::GenDrop(const CPnt& ptCent)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CPnt ptBeg = ptCent;
	
	ptBeg[0] = ptBeg[0] + pipe::dCSize;

	CVec vPlnNorm = - view::GetDirection();
	CVec vXAx(ptCent, ptBeg);
	CVec vYAx = vXAx;
	vYAx.RotAboutArbAx(vPlnNorm, HALF_PI);
						
	CSeg* pSeg = new CSeg(new CPrimArc(ptCent, vXAx, vYAx, TWOPI));
	pDoc->WorkLayerAddTail(pSeg);
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
}