#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "Hatch.h"
#include "Polyline.h"
#include "SubProcDraw.h"
#include "UserAxis.h"

namespace draw
{
	ArcType eArcType		= ARC_THREE_POINT;
	CurveType eCurveType	= CURVE_SPLINE;
	CircleType eCircleType	= CIRCLE_CENTER_RADIUS;

	EArcGen eArcGenId = StartPoint;
	ESplnGen eSplnGenId = BSpline;
	ESplnEndCnd eSplnEndCndId = Relaxed;
	int iSplnPntsS = 8;
}

LRESULT CALLBACK SubProcDraw(HWND hwnd, UINT anMsg, WPARAM wParam, LPARAM lParam)
{
	static WORD wPts;
	static WORD wPrvOp = 0;
	static CPnt pt[64];
	
	CPegDoc* pDoc = CPegDoc::GetDoc();
	CPegView* pView = CPegView::GetActiveView();
	CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();
	
	if (anMsg == WM_COMMAND)
	{
		CSeg *pSeg;	 
		long lResult = 0;
		
		CPnt ptCurPos = app.CursorPosGet();
		
		switch (LOWORD(wParam)) 
		{
			case ID_HELP_KEY:
			{
				HtmlHelp(hwnd, app.GetAppPath() + "\\PegAeSys.chm::/Draw.htm", HH_DISPLAY_TOPIC, NULL);
				return 0;
			}
			case ID_OP0:
			{
				pDoc->SetOptionsFocus(IDC_INFL_ANGLE);
				pDoc->SetOptionsPage(CPegDoc::TAB_OPEN_DRAW);
				::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_TABS_MAIN), hwnd, DlgProcSetup);
				app.StatusLineDisplay();
				break;
			}	
			case ID_OP1:
			{
				pSeg = new CSeg(new CPrimMark(pstate.PenColor(), pstate.MarkStyle(), ptCurPos));
				pDoc->WorkLayerAddTail(pSeg); 
				pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
				break;
			}	
			// line
			case ID_OP2:		
			{
				if (wPrvOp != ID_OP2) 
				{
					app.ModeLineUnhighlightOp(wPrvOp);
					wPrvOp = app.ModeLineHighlightOp(ID_OP2);
					pt[0] = ptCurPos;
				}
				else
				{
					app.RubberBandingDisable();
					pt[1] = UserAxis::UserAxisSnapLn(pt[0], ptCurPos);
					
					pSeg = new CSeg(new CPrimLine(pt[0], pt[1]));
					pDoc->WorkLayerAddTail(pSeg); 

					//#ifdef ENABLE_UNDO
					//	undo.SaveState(CUndo::UNDO_SEGS_DET);
					//#endif

					pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
					pt[0] = pt[1];
				}

				app.RubberBandingStartAtEnable(pt[0], Lines);
				break;
			}
			//polygon
			case ID_OP3:
			{
				if(wPrvOp != ID_OP3) 
				{
					app.ModeLineUnhighlightOp(wPrvOp);
					wPrvOp = app.ModeLineHighlightOp(ID_OP3);
					wPts = 1;
					pt[0] = ptCurPos;
					app.RubberBandingStartAtEnable(ptCurPos, Lines);
				}
				else
				{
					if(wPts < sizeof(pt) / sizeof(pt[0]))
					{
						app.RubberBandingDisable();
						pt[wPts] = ptCurPos;
						if(pt[wPts - 1] != pt[wPts]) 
						{
							CLine(pt[wPts - 1], pt[wPts]).Display(pDC);
							app.RubberBandingStartAtEnable(pt[wPts], Lines);
							wPts++;
						}
						else
						{
							draw::EndFillAreaDef(wPts, pt);
							app.ModeLineUnhighlightOp(wPrvOp);
							wPts = 0;
							pView->InvalidateRect(NULL, TRUE);
						}
					}
					else
						msgInformation(IDS_MSG_TOO_MANY_PTS);

				}
				break;
			}
			// quad
			case ID_OP4:
			{
				if (wPrvOp != ID_OP4) 
				{
					app.ModeLineUnhighlightOp(wPrvOp);
					wPrvOp = app.ModeLineHighlightOp(ID_OP4);
					wPts = 1;
					pt[0] = ptCurPos;
					app.RubberBandingStartAtEnable(pt[0], Lines);
				}
				else
				{
					pt[wPts] = ptCurPos;
					if (pt[wPts - 1] != pt[wPts]) 
					{
						if (wPts == 1) 
						{
							app.RubberBandingEnable(Quads);
							wPts++;
						}
						else 
						{
							app.RubberBandingDisable();
							pt[3] = pt[0] + CVec(pt[1], pt[2]);
							
							pSeg = new CSeg;
					
							for (int i = 0; i < 4; i++)
								pSeg->AddTail(new CPrimLine(pt[i], pt[(i + 1) % 4]));
							
							pDoc->WorkLayerAddTail(pSeg); 
							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
							app.ModeLineUnhighlightOp(wPrvOp);
							wPts = 0;
						}
					}
					else
					{
						app.RubberBandingDisable();
						msgInformation(IDS_MSG_PTS_COINCIDE);
					}
				}
				break;
			}
			// arc
			case ID_OP5:
			{
				if(draw::eArcType == draw::ARC_THREE_POINT)
				{
					if (wPrvOp != ID_OP5) 
					{
						app.ModeLineUnhighlightOp(wPrvOp);
						wPrvOp = app.ModeLineHighlightOp(ID_OP5);
						wPts = 1;
						pt[0] = ptCurPos;
						app.RubberBandingStartAtEnable(ptCurPos, Lines);
					}
					else
					{
						pt[wPts] = ptCurPos;
						if (pt[wPts - 1] != pt[wPts]) 
						{
							if (wPts == 1) 
							{
								app.RubberBandingDisable();
								pt[++wPts] = pt[1];
							}
							else 
							{
								if (draw::eArcGenId == draw::StartPoint) 
								{
									CPrimArc* pArc = new CPrimArc(pt[0], pt[1], pt[2]);
									if (pArc->GetSwpAng() != 0.)
									{
										pSeg = new CSeg(pArc);
										pDoc->WorkLayerAddTail(pSeg); 
										pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
									}
									else
									{
										delete pArc;
										msgInformation(IDS_MSG_PTS_COLINEAR);
									}							
								}
								app.ModeLineUnhighlightOp(wPrvOp);
								wPts = 0;
							}
						}
						else
							msgInformation(IDS_MSG_PTS_COINCIDE);
					}
					break;
				}
				// alternate arc type
				else //if(draw::eArcType == draw::ARC_C2_POINT)
				{
					if (wPrvOp != ID_OP5) 
					{
						app.ModeLineUnhighlightOp(wPrvOp);
						wPrvOp = app.ModeLineHighlightOp(ID_OP5);
						wPts = 1;
						pt[0] = ptCurPos;
						app.RubberBandingStartAtEnable(ptCurPos, Lines);
					}
					else
					{
						pt[wPts] = ptCurPos;
						if (pt[wPts - 1] != pt[wPts]) 
						{
							if (wPts == 1) 
							{
								app.RubberBandingDisable();
								pt[++wPts] = pt[1];
							}
							else 
							{
								if (draw::eArcGenId == draw::StartPoint) 
								{
									CPrimArc* pArc = new CPrimArc(pt[0], pt[1], pt[2]);
									if (pArc->GetSwpAng() != 0.)
									{
										pSeg = new CSeg(pArc);
										pDoc->WorkLayerAddTail(pSeg); 
										pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
									}
									else
									{
										delete pArc;
										msgInformation(IDS_MSG_PTS_COLINEAR);
									}							
								}
								app.ModeLineUnhighlightOp(wPrvOp);
								wPts = 0;
							}
						}
						else
							msgInformation(IDS_MSG_PTS_COINCIDE);
					}
					break;
				}
			}
			// spline
			case ID_OP6:
			{
				if(draw::eCurveType == draw::CURVE_SPLINE)
				{
					if(wPrvOp != ID_OP6) 
					{	// Intitial control point
						app.ModeLineUnhighlightOp(wPrvOp);
						wPrvOp = app.ModeLineHighlightOp(ID_OP6);
						wPts = 1;
						pt[0] = ptCurPos;
						app.RubberBandingStartAtEnable(ptCurPos, Lines);
					}
					else 
					{
						if(wPts < sizeof(pt) / sizeof(pt[0]))
						{
							if (wPts == 1)
								app.RubberBandingDisable();
							else
							{	// rubber banding for splines required display 
								// in main window proc. It is erased here and displayed there.
								int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
								CPrimBSpline BSp(WORD(wPts + 1), pt);
								BSp.Display(pDC, false);
								pstate.SetROP2(pDC, iDrawMode);
							}
							pt[wPts] = ptCurPos;
							if (pt[wPts - 1] != pt[wPts]) 
							{
								wPts++;
								pt[wPts] = pt[wPts - 1];
							}
							else
							{
								draw::EndSplineDef(draw::eSplnGenId, wPts, pt);
								app.ModeLineUnhighlightOp(wPrvOp);
								wPts = 0;
							}
							
						}
						else // Too many control points
							msgInformation(IDS_MSG_TOO_MANY_PTS);
					}
					break;
				}
				else if(draw::eCurveType == draw::CURVE_FITTED)
				{
					if(wPrvOp != ID_OP6) 
					{	// Intitial control point
						app.ModeLineUnhighlightOp(wPrvOp);
						wPrvOp = app.ModeLineHighlightOp(ID_OP6);
						wPts = 1;
						pt[0] = ptCurPos;
						app.RubberBandingStartAtEnable(ptCurPos, Lines);
					}
					else 
					{
						if(wPts < sizeof(pt) / sizeof(pt[0]))
						{
							if (wPts == 1)
								app.RubberBandingDisable();
							else
							{	// rubber banding for splines required display 
								// in main window proc. It is erased here and displayed there.
								int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
								CPrimCSpline CSp(wPts + 1, 0, new CVec(0, 0, 0), pt);
								CSp.Display(pDC, false);
								pstate.SetROP2(pDC, iDrawMode);
							}
							pt[wPts] = ptCurPos;
							if (pt[wPts - 1] != pt[wPts]) 
							{
								wPts++;
								pt[wPts] = pt[wPts - 1];
							}
							else
							{
								draw::EndCSplineDef(draw::eSplnGenId, wPts, pt);
								app.ModeLineUnhighlightOp(wPrvOp);
								wPts = 0;
							}
							
						}
						else // Too many control points
							msgInformation(IDS_MSG_TOO_MANY_PTS);
					}
					break;
				}
			}
			// circle
			case ID_OP7:
			{
				if(draw::eCircleType == draw::CIRCLE_CENTER_RADIUS)
				{
					if (wPrvOp != ID_OP7) 
					{
						app.ModeLineUnhighlightOp(wPrvOp);
						wPrvOp = app.ModeLineHighlightOp(ID_OP7);
						pt[0] = ptCurPos;
						app.RubberBandingStartAtEnable(ptCurPos, Circles);
					}
					else 
					{
						pt[1] = ptCurPos;
						app.RubberBandingDisable();
						if (pt[0] != ptCurPos)
						{	// Radius not zero
							CVec vPlnNorm;
							vPlnNorm = view::GetDirection();
							CVec vMajAx(pt[0], ptCurPos);
							pt[1] = Pnt_RotAboutArbPtAndAx(ptCurPos, pt[0], vPlnNorm, HALF_PI);
							CVec vMinAx(pt[0], pt[1]);
							
							pSeg = new CSeg(new CPrimArc(pt[0], vMajAx, vMinAx, TWOPI));
							pDoc->WorkLayerAddTail(pSeg); 
							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
						}
						app.ModeLineUnhighlightOp(wPrvOp);
					}
					break;
				}
				else //if(draw::eCircleType == draw::CIRCLE_DIAMETER)
				{
					if (wPrvOp != ID_OP7) 
					{
						app.ModeLineUnhighlightOp(wPrvOp);
						wPrvOp = app.ModeLineHighlightOp(ID_OP7);
						pt[0] = ptCurPos;
						app.RubberBandingStartAtEnable(ptCurPos, Circles);
					}
					else 
					{
						pt[1] = ptCurPos;
						app.RubberBandingDisable();
						if (pt[0] != ptCurPos)
						{	// Radius not zero
							CVec vPlnNorm;
							vPlnNorm = view::GetDirection();
							//CVec vMajAx(pt[0], ptCurPos);
							//pt[1] = Pnt_RotAboutArbPtAndAx(ptCurPos, pt[0], vPlnNorm, HALF_PI);
							CPrimLine tempLine(pt[0], ptCurPos);
							pt[1] = tempLine.GetCtrlPt();
							CVec vMajAx(pt[1], pt[0]);
							pt[0] = Pnt_RotAboutArbPtAndAx(pt[0], pt[1], vPlnNorm, HALF_PI);
							CVec vMinAx(pt[1], pt[0]);
							
							pSeg = new CSeg(new CPrimArc(pt[1], vMajAx, vMinAx, TWOPI));
							pDoc->WorkLayerAddTail(pSeg); 
							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
						}
						app.ModeLineUnhighlightOp(wPrvOp);
					}
					break;
				}
			}
			case ID_OP8:	// generalized arc
			{
				if (wPrvOp != ID_OP8) 
				{
					app.ModeLineUnhighlightOp(wPrvOp);
					wPrvOp = app.ModeLineHighlightOp(ID_OP8);
					wPts = 1;
					pt[0] = ptCurPos;
					app.RubberBandingStartAtEnable(ptCurPos, Lines);
				}
				else
				{
					pt[wPts] = ptCurPos;
					if (pt[wPts - 1] != ptCurPos) 
					{
						if (wPts == 1) 
						{
							pt[2] = pt[0];
							app.RubberBandingDisable();
							app.CursorPosSet(pt[0]);
							wPts++;
						}
						else 
						{
							CVec vMajAx(pt[0], pt[1]); 
							CVec vMinAx(pt[0], pt[2]); 
							
							pSeg = new CSeg(new CPrimArc(pt[0], vMajAx, vMinAx, TWOPI));
							pDoc->WorkLayerAddTail(pSeg); 
							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
							app.ModeLineUnhighlightOp(wPrvOp);
							wPts = 0;
						}
					}
					else
						msgInformation(IDS_MSG_PTS_COINCIDE);
				}
				break;
			}
			case ID_OP9:
			{
				if (pDoc->BlksSize() > 0)
					::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_INSERT_BLOCK), hwnd, DlgProcBlockInsert);
				else
					MessageBox(app.GetSafeHwnd(), "There are no blocks in this drawing", "Warning", MB_ICONWARNING);

				break;
			}	
			case IDM_RETURN:
			{
				app.RubberBandingDisable();
				if (wPrvOp == ID_OP3) 
					draw::EndFillAreaDef(wPts, pt);
				else if (wPrvOp == ID_OP6)
				{
					if(draw::eCurveType == draw::CURVE_SPLINE)
						draw::EndSplineDef(draw::eSplnGenId, wPts, pt);
					else
						draw::EndCSplineDef(draw::eSplnGenId, wPts, pt);
				}

				app.ModeLineUnhighlightOp(wPrvOp);
				wPts = 0;
				break;
			}
			case IDM_ESCAPE:
			{
				if (wPrvOp != 0) 
					app.RubberBandingDisable();
				else
					pDoc->DoEditPrimitiveEscape();
				
				//if (wPrvOp == ID_OP2) 
				//{
				//}
				//else if (wPrvOp == ID_OP3) 
				//{
				//	// erase tmp lines of poly
				//}
				//else if (wPrvOp == ID_OP6) 
				//{
				//	// erase tmp lines of poly
				//}
			
				app.ModeLineUnhighlightOp(wPrvOp);
				break;	
			}
			default:
			{
				lResult = !lResult;
			}
		}

		if (lResult == 0)
			return (lResult);
	}
	else if (anMsg == WM_MOUSEMOVE)
	{
		if (wPrvOp == ID_OP5 && wPts == 2) 
		{
			int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
			if (draw::eArcGenId == draw::StartPoint) 
			{
				CPrimArc(pt[0], pt[1], pt[2]).Display(pDC, false);
				pt[2] = app.CursorPosGet();
				CPrimArc(pt[0], pt[1], pt[2]).Display(pDC, false);
			}
			pstate.SetROP2(pDC, iDrawMode);
		}
		else if (wPrvOp == ID_OP6 && wPts >= 2) 
		{
			if(draw::eCurveType == draw::CURVE_SPLINE)
			{
				int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
				
				CPrimBSpline BSp(WORD(wPts +  1), pt);
				if (pt[wPts - 1] != pt[wPts])
					BSp.Display(pDC, false);
				pt[wPts] = app.CursorPosGet();
				BSp.SetPt(wPts, pt[wPts]);
				if (pt[wPts - 1] != pt[wPts])
					BSp.Display(pDC, false);
				
				pstate.SetROP2(pDC, iDrawMode);
			}
			else
			{
				int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
				
				CPrimCSpline CSp(WORD(wPts +  1), 0, new CVec(0, 0, 0), pt);
				if (pt[wPts - 1] != pt[wPts])
					CSp.Display(pDC, false);
				pt[wPts] = app.CursorPosGet();
				CSp.SetPt(wPts, pt[wPts]);
				if (pt[wPts - 1] != pt[wPts])
					CSp.Display(pDC, false);
				
				pstate.SetROP2(pDC, iDrawMode);
			}
		}
		else if (wPrvOp == ID_OP8 && wPts == 2) 
		{
			int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);

			CVec vMajAx(pt[0], pt[1]);
			CVec vMinAx(pt[0], pt[2]);
			CPrimArc arc(pt[0], vMajAx, vMinAx, TWOPI);
			if (!vMinAx.IsNull())
				arc.Display(pDC, false);
			pt[2] = app.CursorPosGet();
			vMinAx = CVec(pt[0], pt[2]);			
			if (!vMinAx.IsNull())
			{
				arc.SetMinAx(vMinAx);
				arc.Display(pDC, false);
			}

			pstate.SetROP2(pDC, iDrawMode);
		}
	}
	return (CallWindowProc(app.GetMainWndProc(), hwnd, anMsg, wParam, lParam));
}

WORD draw::EndFillAreaDef(WORD wPts, CPnt* pt)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	if (wPts >= 3)
	{	// Determine plane in which fill area lies
		CVec vXAx(pt[0], pt[1]);
		CVec vYAx(pt[0], pt[2]);
		CVec vPlnNorm(vXAx ^ vYAx);
		vPlnNorm.Normalize();

		if (vPlnNorm[2] < 0)
			vPlnNorm = - vPlnNorm;

		vXAx.Normalize();
		vXAx.RotAboutArbAx(vPlnNorm, hatch::dOffAng);
		vYAx = vXAx;
		vYAx.RotAboutArbAx(vPlnNorm, HALF_PI);
		vXAx *= hatch::dXAxRefVecScal;
		vYAx *= hatch::dYAxRefVecScal;

		// Project reference origin to plane

		CSeg* pSeg = new CSeg(new CPrimPolygon(wPts, pt[wPts], vXAx, vYAx, pt));
		pDoc->WorkLayerAddTail(pSeg); 
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
	}
	return (wPts);
}

WORD draw::EndPolylineDef(WORD wPts, CPnt* pt)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CSeg* pSeg = new CSeg(new CPrimPolyline(wPts, pt));
	pDoc->WorkLayerAddTail(pSeg); 
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);

	return (wPts);
}

int draw::EndSplineDef(ESplnGen eSplnGenId, int iPts, CPnt* pt)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	if (iPts > 2) 
	{
		if (eSplnGenId == draw::BSpline) 
		{
			CSeg* pSeg = new CSeg(new CPrimBSpline(WORD(iPts), pt));
			pDoc->WorkLayerAddTail(pSeg); 
			pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
		}
	}
	return (iPts);
}

int draw::EndCSplineDef(ESplnGen, int iPts, CPnt* pt)
//int draw::EndCSplineDef(ESplnGen eSplnGenId, int iPts, CPnt* pt)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	if (iPts > 2) 
	{
//		if (eSplnGenId == draw::BSpline) 
		{
			CSeg* pSeg = new CSeg(new CPrimCSpline(WORD(iPts), 0, new CVec(0, 0, 0), pt));
			pDoc->WorkLayerAddTail(pSeg); 
			pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
		}
	}
	return (iPts);
}