#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "SubProcAnnotate.h"
#include "UserAxis.h"

void Polygon_GenPts(const CPnt&, const CVec&, const CVec&, int, CPnt*);

namespace annotate
{
	int		nArrowType		= 1;
	double	dArrowSize		= .1;
	double	dBubbleRadius	= .125;
	int		nBubbleFacets	= 0;
	double  dGapSpacing		= .5;
	double  dHookRadius		= .03125;

	double	dConstraint		= 2.00;

	char 	szDefText[16];
	char	szCurTxt[16];
}

LRESULT CALLBACK SubProcAnnotate(HWND hwnd, UINT anMsg, WPARAM wParam, LPARAM lParam)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	CPegView* pView = CPegView::GetActiveView();
	CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();
	
	static WORD wPrvKeyDwn = 0;
	static CPnt pt[16];

	CSeg*	pSeg;
	CPrimText*	pText;
	double	dD12;
	int 	i;
	CVec	vPlnNorm, vXAx, vYAx;

	switch (anMsg)
	{
		case WM_COMMAND:
			switch (LOWORD(wParam)) 
			{
				case ID_OP0:
					pDoc->SetOptionsFocus(IDC_ANN_DEF_TXT);
					pDoc->SetOptionsPage(CPegDoc::TAB_OPEN_ANNOTATE);
					::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_TABS_MAIN), app.GetSafeHwnd(), DlgProcSetup);
					return 0;

				case ID_OP1:	// unused - existing bubble
				{
					pt[1] = app.CursorPosGet();
					
					pSeg = new CSeg;
					if (wPrvKeyDwn == 0 || wPrvKeyDwn == ID_OP4 || wPrvKeyDwn == ID_OP1)	// No operation pending
						wPrvKeyDwn = ID_OP1;
					else											// Operation pending
					{
						app.RubberBandingDisable();
						dD12 = CVec(pt[0], pt[1]).Length();
						if (dD12 > DBL_EPSILON) 
						{
							if (wPrvKeyDwn == ID_OP2) 
							{
								if (dD12 > 0.095699 + DBL_EPSILON) 
								{
									pt[2] = Pnt_ProjPtTo(pt[1], pt[0], 0.095699);
									pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[2]));
								}
							}
							else if (wPrvKeyDwn == ID_OP3)
							{
								annotate::GenArrowHead(annotate::nArrowType, annotate::dArrowSize, pt[1], pt[0], pSeg);
								if (dD12 > 0.095699 + DBL_EPSILON) 
								{
									pt[2] = Pnt_ProjPtTo(pt[1], pt[0], 0.095699);
									pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[2]));
								}
							}
							else 
							{
								if (dD12 > annotate::dHookRadius + DBL_EPSILON) 
								{
									pt[0] = Pnt_ProjPtTo(pt[0], pt[1], annotate::dHookRadius);
									if (CVec(pt[0], pt[1]).Length() > 0.095699 + DBL_EPSILON) 
									{
										pt[2] = Pnt_ProjPtTo(pt[1], pt[0], 0.095699);
										pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[2]));
									}
								}
							}
							app.ModeLineUnhighlightOp(wPrvKeyDwn);
							wPrvKeyDwn = ID_OP1;
						}
					}
					app.ModeLineHighlightOp(ID_OP1);

					pt[2] = pt[1];
					vPlnNorm = view::GetDirection();
					vYAx = view::GetVwUp();
					vYAx *= .1;
					vXAx = vYAx;
					vXAx.RotAboutArbAx(vPlnNorm, - HALF_PI);
					vXAx *= .6;
					CRefSys rs(pt[1], vXAx, vYAx);
					
					vXAx *= annotate::dBubbleRadius / .06;
					vYAx *= annotate::dBubbleRadius / .1;
											
					{	// Bubble is a circle
						pSeg->AddTail(new CPrimArc(2, 1, pt[1], CVec(0.0, 0.095699, 0.0), CVec(-0.095699, 0.0, 0.0), TWOPI));
						
						CPnt tmpPnts[12];
						tmpPnts[0] = CPnt(pt[1][0] + 0.094950, pt[1][1] + -0.005129, 0.0);
						tmpPnts[1] = CPnt(pt[1][0] + 0.083843, pt[1][1] + -0.046248, 0.0);
						tmpPnts[2] = CPnt(pt[1][0] + 0.060770, pt[1][1] + -0.072845, 0.0);
						tmpPnts[3] = CPnt(pt[1][0] + 0.027344, pt[1][1] + -0.091642, 0.0);
						tmpPnts[4] = CPnt(pt[1][0] + 0.0, pt[1][1] + -0.095057, 0.0);
						tmpPnts[5] = CPnt(pt[1][0] + 0.0, pt[1][1] + 0.095057, 0.0);
						tmpPnts[6] = CPnt(pt[1][0] + -0.030869, pt[1][1] + 0.090785, 0.0);
						tmpPnts[7] = CPnt(pt[1][0] + -0.059067, pt[1][1] + 0.076260, 0.0);
						tmpPnts[8] = CPnt(pt[1][0] + -0.080425, pt[1][1] + 0.051373, 0.0);
						tmpPnts[9] = CPnt(pt[1][0] + -0.093349, pt[1][1] + 0.022216, 0.0);
						tmpPnts[10] = CPnt(pt[1][0] + -0.095915, pt[1][1] + -0.005129, 0.0);
						tmpPnts[11] = CPnt(pt[1][0] + 0.095804, pt[1][1] + -0.005129, 0.0);

						CPrimPolygon *pPoly = new CPrimPolygon(12, pt[1], CVec(0.002043, -0.003556, 0.0), CVec(0.003556, 0.002043, 0.0), tmpPnts);

						pPoly->SetIntStyle(3);
						pPoly->SetIntStyleId(1);
						pPoly->SetPenColor(2);

						pSeg->AddTail(pPoly);
					}
					
					pDoc->WorkLayerAddTail(pSeg);
					pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);

					pt[0] = pt[1];
					app.CursorPosSet(pt[0]);
					app.RubberBandingStartAtEnable(pt[0], Lines);
					
					return 0;	
				}

				case ID_OP2:	// Define and generate one or more lines
					pt[1] = app.CursorPosGet();
					if (wPrvKeyDwn == 0) 					// No operation pending
					{
						pt[0] = pt[1];
						wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP2);
					}
					else									// Operation pending
					{
						pt[1] = annotate::SnapLn(pt[0], pt[1]);

						app.RubberBandingDisable();
						dD12 = CVec(pt[0], pt[1]).Length();
						if (dD12 > DBL_EPSILON) 			// Points do not coincide
						{
							pSeg = new CSeg;
							if (wPrvKeyDwn == ID_OP2) 
								pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[1]));
							else 
							{
								if (wPrvKeyDwn == ID_OP3) 
								{
									annotate::GenArrowHead(annotate::nArrowType, annotate::dArrowSize, pt[1], pt[0], pSeg);
									pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[1]));
								}
								else if(wPrvKeyDwn == ID_OP1)
								{
									if (dD12 > 0.095699 + DBL_EPSILON) 
									{
										pt[0] = Pnt_ProjPtTo(pt[0], pt[1], 0.095699);
										pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[1]));
									}
								}
								else if (wPrvKeyDwn == ID_OP4) 
								{
									if (dD12 > annotate::dBubbleRadius + DBL_EPSILON) 
									{
										pt[0] = Pnt_ProjPtTo(pt[0], pt[1], annotate::dBubbleRadius);
										pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[1]));
									}
								}
								else 
								{
									if (dD12 > annotate::dHookRadius + DBL_EPSILON) 
									{
										pt[0] = Pnt_ProjPtTo(pt[0], pt[1], annotate::dHookRadius);
										pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[1]));
									}
								}
								app.ModeLineUnhighlightOp(wPrvKeyDwn);
								wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP2);
							}
							if (pSeg->IsEmpty())
								delete pSeg;
							else
							{
								pDoc->WorkLayerAddTail(pSeg);
								pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
							}
							pt[0] = pt[1];
						}
						else
							msgInformation(0);
					}
					// app.CursorPosSet(pt[0]);
					app.RubberBandingStartAtEnable(pt[0], Lines);
					return 0;

				case ID_OP3:	// Generate line with arrow head
					pt[1] = app.CursorPosGet();
					if (wPrvKeyDwn == 0) 						// No operation pending
					{
						pt[0] = pt[1];
						wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP3);
					}
					else											// Operation pending
					{
						pt[1] = annotate::SnapLn(pt[0], pt[1]);

						app.RubberBandingDisable();
						dD12 = CVec(pt[0], pt[1]).Length();
						if (dD12 > DBL_EPSILON)
						{	// Two points do not coincide
							pSeg = new CSeg;
							if (wPrvKeyDwn == ID_OP3) 
							{
								annotate::GenArrowHead(annotate::nArrowType, annotate::dArrowSize, pt[1], pt[0], pSeg);
								pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[1]));
								annotate::GenArrowHead(annotate::nArrowType, annotate::dArrowSize, pt[0], pt[1], pSeg);
							}
							else 
							{
								if (wPrvKeyDwn == ID_OP2) 
								{
									pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[1]));
									annotate::GenArrowHead(annotate::nArrowType, annotate::dArrowSize, pt[0], pt[1], pSeg);
								}
								else if(wPrvKeyDwn == ID_OP1)
								{
									if (dD12 > 0.095699 + DBL_EPSILON) 
									{
										pt[0] = Pnt_ProjPtTo(pt[0], pt[1], 0.095699);
										pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[1]));
										annotate::GenArrowHead(annotate::nArrowType, annotate::dArrowSize, pt[0], pt[1], pSeg);
									}
								}
								else if (wPrvKeyDwn == ID_OP4) 
								{
									if (dD12 > annotate::dBubbleRadius + DBL_EPSILON)
									{
										pt[0] = Pnt_ProjPtTo(pt[0], pt[1], annotate::dBubbleRadius);
										pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[1]));
										annotate::GenArrowHead(annotate::nArrowType, annotate::dArrowSize, pt[0], pt[1], pSeg);
									}
								}
								else 
								{ 
									if (dD12 > annotate::dHookRadius + DBL_EPSILON) 
									{
										pt[0] = Pnt_ProjPtTo(pt[0], pt[1], annotate::dHookRadius);
										pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[1]));
										annotate::GenArrowHead(annotate::nArrowType, annotate::dArrowSize, pt[0], pt[1], pSeg);
									}
								}
								app.ModeLineUnhighlightOp(wPrvKeyDwn);
								wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP3);
							}
							if (pSeg->IsEmpty())
								delete pSeg;
							else
							{
								pDoc->WorkLayerAddTail(pSeg);
								pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
							}
							pt[0] = pt[1];
						}
						else		// Two points coincide
							msgInformation(0);
					}
					// app.CursorPosSet(pt[0]);
					app.RubberBandingStartAtEnable(pt[0], Lines);
					return 0;

				case ID_OP4:	// Generate a bubble
				{
					pt[1] = app.CursorPosGet();
					::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_ANNOTATE_ENTER_TEXT), app.GetSafeHwnd(), DlgProcAnnotateEnterText);
					pSeg = new CSeg;
					if (wPrvKeyDwn == 0 || wPrvKeyDwn == ID_OP4)// || wPrvKeyDwn == ID_OP1)	// No operation pending
						wPrvKeyDwn = ID_OP4;
					else											// Operation pending
					{
						pt[1] = annotate::SnapLn(pt[0], pt[1]);

						app.RubberBandingDisable();
						dD12 = CVec(pt[0], pt[1]).Length();
						if (dD12 > DBL_EPSILON) 
						{
							if (wPrvKeyDwn == ID_OP2) 
							{
								if (dD12 > annotate::dBubbleRadius + DBL_EPSILON)
								{
									pt[2] = Pnt_ProjPtTo(pt[1], pt[0], annotate::dBubbleRadius);
									pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[2]));
								}
							}
							else if (wPrvKeyDwn == ID_OP3)
							{
								annotate::GenArrowHead(annotate::nArrowType, annotate::dArrowSize, pt[1], pt[0], pSeg);
								if (dD12 > annotate::dBubbleRadius + DBL_EPSILON) 
								{
									pt[2] = Pnt_ProjPtTo(pt[1], pt[0], annotate::dBubbleRadius);
									pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[2]));
								}
							}
							else 
							{
								if (dD12 > annotate::dHookRadius + DBL_EPSILON) 
								{
									pt[0] = Pnt_ProjPtTo(pt[0], pt[1], annotate::dHookRadius);
									if (CVec(pt[0], pt[1]).Length() > annotate::dBubbleRadius + DBL_EPSILON)
									{
										pt[2] = Pnt_ProjPtTo(pt[1], pt[0], annotate::dBubbleRadius);
										pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[2]));
									}
								}
							}
							app.ModeLineUnhighlightOp(wPrvKeyDwn);
							wPrvKeyDwn = ID_OP4;
						}
					}
					app.ModeLineHighlightOp(ID_OP4);

					pt[2] = pt[1];
					vPlnNorm = view::GetDirection();
					vYAx = view::GetVwUp();
					vYAx *= .1;
					vXAx = vYAx;
					vXAx.RotAboutArbAx(vPlnNorm, - HALF_PI);
					vXAx *= .6;
					CRefSys rs(pt[1], vXAx, vYAx);
					
					if (strlen(annotate::szCurTxt) > 0) 
					{
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
						
						pSeg->AddTail(new CPrimText(fd, rs, annotate::szCurTxt));
						pSeg->Vectorize();
						pstate.Restore(pDC, iPrimState);
					}

					vXAx *= annotate::dBubbleRadius / .06;
					vYAx *= annotate::dBubbleRadius / .1;
					
					if (annotate::nBubbleFacets == 0)
					{	// Bubble is a circle
						pSeg->AddTail(new CPrimArc(1, 1, pt[1], vXAx, vYAx, TWOPI));
					}
					else					
					{	// Bubble is a polygon
						int iPrimState = pstate.Save();
						pstate.SetPen(pDC, 1, 1);
						pstate.SetPolygonIntStyle(POLYGON_HOLLOW);
						Polygon_GenPts(pt[1], vXAx, vYAx, annotate::nBubbleFacets + 1, &pt[3]);
						pSeg->AddTail(new CPrimPolygon(WORD(annotate::nBubbleFacets + 1), pt[1], vXAx, vYAx, &pt[3]));
						pstate.Restore(pDC, iPrimState);
					}
					pDoc->WorkLayerAddTail(pSeg);
					pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);

					pt[0] = pt[1];
					app.CursorPosSet(pt[0]);
					app.RubberBandingStartAtEnable(pt[0], Lines);
					
					return 0;
				}
				case ID_OP5:	// Generate a hook
					pt[1] = app.CursorPosGet();

					pSeg = new CSeg;

					if (wPrvKeyDwn == 0)
						wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP5);
					else
					{
						pt[1] = annotate::SnapLn(pt[0], pt[1]);

						app.RubberBandingDisable();
						dD12 = CVec(pt[0], pt[1]).Length();
						if (dD12 > DBL_EPSILON) 
						{
							if (wPrvKeyDwn == ID_OP5) 
							{
								if (dD12 > annotate::dHookRadius + DBL_EPSILON) 
								{
									pt[0] = Pnt_ProjPtTo(pt[0], pt[1], annotate::dHookRadius);
									if (CVec(pt[0], pt[1]).Length() > annotate::dHookRadius + DBL_EPSILON)
									{
										pt[2] = Pnt_ProjPtTo(pt[1], pt[0], annotate::dHookRadius);
										pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[2]));
									}
								}
							}
							else 
							{
								if (wPrvKeyDwn == ID_OP2) 
								{
									if (dD12 > annotate::dHookRadius + DBL_EPSILON) 
									{
										pt[2] = Pnt_ProjPtTo(pt[1], pt[0], annotate::dHookRadius);
										pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[2]));
									}
								}
								else if (wPrvKeyDwn == ID_OP3) 
								{
									annotate::GenArrowHead(annotate::nArrowType, annotate::dArrowSize, pt[1], pt[0], pSeg);
									if (dD12 > annotate::dHookRadius + DBL_EPSILON) 
									{
										pt[2] = Pnt_ProjPtTo(pt[1], pt[0], annotate::dHookRadius);
										pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[2]));
									}
								}
								else if (wPrvKeyDwn == ID_OP4) 
								{
									if (dD12 > annotate::dBubbleRadius + DBL_EPSILON) 
									{
										pt[0] = Pnt_ProjPtTo(pt[0], pt[1], annotate::dBubbleRadius);
										if (CVec(pt[0], pt[1]).Length() > annotate::dHookRadius + DBL_EPSILON)
										{
											pt[2] = Pnt_ProjPtTo(pt[1], pt[0], annotate::dHookRadius);
											pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[2]));
										}
									}
								}
								app.ModeLineUnhighlightOp(wPrvKeyDwn);
								wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP5);
							}
						}
					}
					pt[2] = pt[1];
					vPlnNorm = view::GetDirection();
					vYAx = view::GetVwUp();
					vYAx *= annotate::dHookRadius;
					//	Rotate view up vector - 90 degress about plane normal vector to get positive x-axis
					vXAx = vYAx;
					vXAx.RotAboutArbAx(vPlnNorm, - HALF_PI);
					pSeg->AddTail(new CPrimArc(1, 1, pt[1], vXAx, vYAx, TWOPI));
					pDoc->WorkLayerAddTail(pSeg);
					pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
					
					pt[0] = pt[1];
					app.RubberBandingStartAtEnable(pt[0], Lines);
					return 0;
			
				case ID_OP6:
					pt[1] = app.CursorPosGet();
					if (wPrvKeyDwn != 0) 
					{
						app.RubberBandingDisable();
						app.ModeLineUnhighlightOp(wPrvKeyDwn);
					}
					pText = detsegs.SelTextUsingPoint(pt[1]);
					if (pText != 0)
					{
						CPnts ptsBox;
						pText->GetBoundingBox(ptsBox, annotate::dGapSpacing);
						
						pSeg = new CSeg;
						pSeg->AddTail(new CPrimLine(pstate.PenColor(), 1, ptsBox[0], ptsBox[1]));
							
						pDoc->WorkLayerAddTail(pSeg);
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);

						app.CursorPosSet(pt[1]);
					}
					pt[0] = pt[1];
					return 0;

				case ID_OP7:
					pt[1] = app.CursorPosGet();
					if (wPrvKeyDwn != ID_OP7) 
					{
						if (wPrvKeyDwn != 0) 
						{
							app.RubberBandingDisable();
							app.ModeLineUnhighlightOp(wPrvKeyDwn);
						}
						wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP7);
						pt[0] = pt[1];
					}
					else 
					{
						CPnts ptsBox1;
						CPnts ptsBox2;
						bool bG1Flg = false;
						bool bG2Flg = false;
						pText = detsegs.SelTextUsingPoint(pt[0]);
						if (pText != 0) 
						{
							pText->GetBoundingBox(ptsBox1, annotate::dGapSpacing);
							bG1Flg = true;
						}
						pText = detsegs.SelTextUsingPoint(pt[1]);
						if (pText != 0) 
						{
							pText->GetBoundingBox(ptsBox2, annotate::dGapSpacing);
							bG2Flg = true;
						}
						if (bG1Flg && bG2Flg) 
						{
							pt[0] = ptsBox1[0];
							pt[2] = ptsBox1[0];
							for (i = 1; i < 4; i++) 
							{
								pt[0][0] = Min(pt[0][0], ptsBox1[i][0]);
								pt[2][0] = Max(pt[2][0], ptsBox1[i][0]);
								pt[0][1] = Min(pt[0][1], ptsBox1[i][1]);
								pt[2][1] = Max(pt[2][1], ptsBox1[i][1]);
							}
							for (i = 0; i < 4; i++) 
							{
								pt[0][0] = Min(pt[0][0], ptsBox2[i][0]);
								pt[2][0] = Max(pt[2][0], ptsBox2[i][0]);
								pt[0][1] = Min(pt[0][1], ptsBox2[i][1]);
								pt[2][1] = Max(pt[2][1], ptsBox2[i][1]);
							}
							pt[1][0] = pt[2][0];
							pt[1][1] = pt[0][1];
							pt[3][0] = pt[0][0];
							pt[3][1] = pt[2][1];
							
							pSeg = new CSeg;
					
							for (int i = 0; i < 4; i++)
								pSeg->AddTail(new CPrimLine(1, 1, pt[i], pt[(i + 1) % 4]));
							
							pDoc->WorkLayerAddTail(pSeg); 
							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
						}
						app.CursorPosSet(pt[1]);
						app.ModeLineUnhighlightOp(wPrvKeyDwn);
					}
					return 0;

				case ID_OP8:
				{
					pt[1] = app.CursorPosGet();
										
					CSeg* pSeg = detsegs.SelLineUsingPoint(pt[1]);
					if (pSeg != 0)
					{
						CPrimLine* pLine = static_cast<CPrimLine*>(detsegs.DetPrim());
						
						pt[1] = detsegs.DetPt();
						
						annotate::szCurTxt[0] = '\0';
						
						::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_ANNOTATE_ENTER_TEXT), app.GetSafeHwnd(), DlgProcAnnotateEnterText);
					
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSeg);

						int iPrimState = pstate.Save();
					
						if (strlen(annotate::szCurTxt) > 0) 
						{
							double dAng = pLine->GetAngAboutZAx();
							if (dAng > .25 * TWOPI && dAng <  .75 * TWOPI)
								dAng += PI;

							vPlnNorm = view::GetDirection();
							vYAx = view::GetVwUp();
							vYAx *= .1;
							vYAx.RotAboutArbAx(vPlnNorm, dAng);
							vXAx = vYAx;
							vXAx.RotAboutArbAx(vPlnNorm, - HALF_PI);
							vXAx *= .6;
							CRefSys rs(pt[1], vXAx, vYAx);
							PENCOLOR nPenColor = pstate.PenColor();
							pstate.SetPenColor(2);
							
							CFontDef fd;
							pstate.GetFontDef(fd);
							fd.TextHorAlignSet(CFontDef::HOR_ALIGN_CENTER);
							fd.TextVerAlignSet(CFontDef::VER_ALIGN_MIDDLE);
														
							CCharCellDef ccd;
							pstate.GetCharCellDef(ccd);
							ccd.TextRotAngSet(0.);
							pstate.SetCharCellDef(ccd);
							
							CPrimText* pText = new CPrimText(fd, rs, annotate::szCurTxt); 
							pstate.SetPenColor(nPenColor);

							pSeg->AddTail(pText);
							CPnts ptsBox;
							pText->GetBoundingBox(ptsBox, annotate::dGapSpacing);
							
							double dGap = CVec(ptsBox[0], ptsBox[1]).Length();

							ptsBox[0] = Pnt_ProjPtTo(pt[1], pLine->Pt0(), dGap / 2.);
							ptsBox[1] = Pnt_ProjPtTo(pt[1], pLine->Pt1(), dGap / 2.);
										
							double dRel[2];
								
							dRel[0] = pLine->RelOfPt(ptsBox[0]);
							dRel[1] = pLine->RelOfPt(ptsBox[1]);

							if (dRel[0] > DBL_EPSILON && dRel[1] < 1. - DBL_EPSILON)
							{
								CPrimLine* pLineNew = new CPrimLine(*pLine);
								pLine->SetPt1(ptsBox[0]);
								pLineNew->SetPt0(ptsBox[1]);
								pSeg->AddTail(pLineNew);
							}
							else if (dRel[0] <= DBL_EPSILON)
								pLine->SetPt0(ptsBox[1]);
							else if (dRel[1] >= 1. - DBL_EPSILON)
								pLine->SetPt1(ptsBox[0]);

							pt[0] = pt[1];
							app.CursorPosSet(pt[0]);
						}
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG, pSeg);
						pstate.Restore(pDC, iPrimState);
					}

					return 0;
				}
				case ID_OP9: // Generate construction line
					if (wPrvKeyDwn != ID_OP9) 
					{
						pt[0] = app.CursorPosGet();
						wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP9);
					}
					else
					{
						pt[1] = app.CursorPosGet();
						pt[1] = UserAxis::UserAxisSnapLn(pt[0], pt[1]);
					
						pt[2] = Pnt_ProjPtTo(pt[0], pt[1], 48.);
						pt[3] = Pnt_ProjPtTo(pt[2], pt[0], 96.);
					
						pSeg = new CSeg(new CPrimLine(15, 2, pt[2], pt[3]));
						pDoc->WorkLayerAddTail(pSeg);
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
						app.CursorPosSet(pt[0]);
						app.ModeLineUnhighlightOp(wPrvKeyDwn);
					}
					return 0;
				
				case IDM_RETURN:
					app.RubberBandingDisable();
					app.ModeLineUnhighlightOp(wPrvKeyDwn);
					return 0;

				case IDM_ESCAPE:
					if (wPrvKeyDwn != 0) 
						app.RubberBandingDisable();
					else
						pDoc->DoEditPrimitiveEscape();

					app.ModeLineUnhighlightOp(wPrvKeyDwn);
					return 0;

			break;
		}
	}
	return (CallWindowProc(app.GetMainWndProc(), hwnd, anMsg, wParam, lParam));
}

///<summary>Generates arrow heads for annotation mode.</summary>
// Parameters:	aiArrowTyp	type of arrowhead
//				adArrowSiz,
//				ptTail		tail of line segment defining arrow head
//				ptHead		head of line segment defining arrow head
//				pSeg
void annotate::GenArrowHead(int aiArrowTyp, double adArrowSiz, const CPnt& ptTail, const CPnt& ptHead, CSeg* pSeg)
{
	CVec vVwPlnNorm = view::GetDirection();
	if (aiArrowTyp == 1 || aiArrowTyp == 2) 
	{
		double dAng = .244978663127;
		double dLen = adArrowSiz / .970142500145;
		CPnt pt3 = Pnt_ProjPtTo(ptHead, ptTail, dLen);
		CPnt pt4 = Pnt_RotAboutArbPtAndAx(pt3, ptHead, vVwPlnNorm, dAng);
		CPnt pt5 = Pnt_RotAboutArbPtAndAx(pt3, ptHead, vVwPlnNorm, - dAng);
		pSeg->AddTail(new CPrimLine(1, 1, ptHead, pt4));
		if (aiArrowTyp == 2)
			pSeg->AddTail(new CPrimLine(1, 1, pt4, pt5));
		pSeg->AddTail(new CPrimLine(1, 1, pt5, ptHead));
	}
	else if (aiArrowTyp == 3) 
	{
		double dAng = 9.96686524912e-2;
		double dLen = adArrowSiz / .99503719021;
		CPnt pt3 = Pnt_ProjPtTo(ptHead, ptTail, dLen);
		CPnt pt4 = Pnt_RotAboutArbPtAndAx(pt3, ptHead, vVwPlnNorm, dAng);
		pSeg->AddTail(new CPrimLine(1, 1, pt4, ptHead));
	}
	else if (aiArrowTyp == 4) 
	{
		double dAng = .785398163397;
		double dLen = .5 * adArrowSiz / .707106781187;
		CPnt pt3 = Pnt_ProjPtTo(ptHead, ptTail, dLen);
		CPnt pt4 = Pnt_RotAboutArbPtAndAx(pt3, ptHead, vVwPlnNorm, dAng);
		CPnt pt5 = Pnt_RotAboutArbPtAndAx(pt4, ptHead, vVwPlnNorm, PI);
		pSeg->AddTail(new CPrimLine(1, 1, pt4, pt5));
	}
}

CPnt annotate::SnapLn(const CPnt& ptBeg, const CPnt& ptEnd)
{
	return (line::ConstrainToAx(CLine(ptBeg, ptEnd), annotate::dConstraint, 0.));
}