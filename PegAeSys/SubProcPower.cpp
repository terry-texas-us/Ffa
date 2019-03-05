#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "UserAxis.h"

enum EJoinPower {Begin, End, No};
 
int		SubProcPowerFndCirc(CLine& ln, const CPnt&, CPnt&, double*);
void	SubProcPowerGenGround(const CPnt&, const CPnt&);
void	SubProcPowerGenHot(const CPnt&, const CPnt&);
void	SubProcPowerGenNeutral(const CPnt&, const CPnt&);
void	SubProcPowerGenSwitchLeg(const CPnt&, const CPnt&);

LRESULT CALLBACK SubProcPower(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	static CPnt pt[5];
	static CPnt pt1;
	static CPnt pt2;
	static CPnt pt3;

	static CPrimLine* pLinePrv = 0;
	static CSeg* pSegPrv = 0;

	static EJoinPower eJoin = No;

	static WORD wPrvKeyDwn = 0;
	static bool bArrow = false;
	static bool bConductor = false;
	
	static double dRad[2];

	if (nMsg == WM_COMMAND)
	{
		pt[1] = app.CursorPosGet();
		
		switch (LOWORD(wParam))
		{
			case ID_OP0:
				// Options
				break;

			case ID_OP1:
			{	// Looking for electrical symbol (actually any circle)
				bArrow = false;
				bConductor = false;
				
				eJoin = No;
				
				POSITION pos = detsegs.GetHeadPosition();
				while (eJoin == No && pos != 0)
				{
					CSeg* pSeg = detsegs.GetNext(pos);
					POSITION posPrim = pSeg->GetHeadPosition();
					while (eJoin == No && posPrim != 0)
					{
						CPrim* pPrim = pSeg->GetNext(posPrim);
						if (pPrim->Is(CPrim::PRIM_ARC))
						{
							CPrimArc* pArc = static_cast<CPrimArc*>(pPrim);

							if (fabs(pArc->GetSwpAng() - TWOPI) <= DBL_EPSILON) 
							{	// Arc is closed
								if (Pnt_DistanceTo_xy(pt[1], pArc->Center()) <= .1)
								{
									pt[1] = pArc->Center();
									eJoin = (wPrvKeyDwn == 0) ? Begin : End;
									dRad[eJoin] = (pArc->GetMajAx().Length() + pArc->GetMinAx().Length()) * .5;
								}
							}
						}
					}
				}
				break;
			}
			case ID_OP2:
				bArrow = false;
				bConductor = false;
				
				app.RubberBandingDisable();
				
				if (wPrvKeyDwn == ID_OP2)
				{
					pt1 = pLinePrv->Pt0();
					pt2 = pLinePrv->Pt1();
						
					pt2 = CLine(pt1, pt2).ProjPt(pt[1]);
					
					double dDis = Pnt_DistanceTo_xy(pLinePrv->Pt1(), pt2);
						
					if (dDis > DBL_EPSILON && dDis / Max(DBL_EPSILON, Pnt_DistanceTo_xy(pt[0], pt[1])) <= .258819)
					{
						pLinePrv->SetPt1(pt2);
						pt[0] = pt2;
					}
				}
				else if (wPrvKeyDwn == ID_OP3)
				{	// Circuit from a symbol
					if (Pnt_DistanceTo_xy(pt[0], pt[1]) > dRad[Begin])
					{
						pt[1] = UserAxis::UserAxisSnapLn(pt[0], pt[1]);
						pt[0] = Pnt_ProjPtTo(pt[0], pt[1], dRad[Begin]);
					}
				}
				if (eJoin == No)
				{	// Not on a symbol
					if (wPrvKeyDwn == 0) 
						// Begin circuit generation
						wPrvKeyDwn = ID_OP1;			
					else
					{
						if (wPrvKeyDwn == ID_OP1)
							// Generate first circuit
							pt[1] = UserAxis::UserAxisSnapLn(pt[0], pt[1]);
					   
						if (pt[0] != pt[1])
						{
							pSegPrv = new CSeg;
							pLinePrv = new CPrimLine(pt[0], pt[1]);
							pSegPrv->AddTail(pLinePrv);
							pDoc->WorkLayerAddTail(pSegPrv); 
							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSegPrv);							
							wPrvKeyDwn = ID_OP2;
						}
					}
				}
				else
				{	// Circuit to or from a symbol
					if (eJoin == End) 
					{	// Ending circuit at symbol
						if (Pnt_DistanceTo_xy(pt[0], pt[1]) > dRad[End]) 
						{
							pt2 = pt[1];
							pt2 = Pnt_ProjPtTo(pt[1], pt[0], dRad[End]);
							pSegPrv = new CSeg;
					
							pSegPrv->AddTail(new CPrimLine(pt[0], pt2));
							pDoc->WorkLayerAddTail(pSegPrv); 
							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSegPrv);	
							dRad[Begin] = dRad[End];
						}
					}
					wPrvKeyDwn = ID_OP3;
					eJoin = No;
				}
				pt[0] = pt[1];
				app.RubberBandingStartAtEnable(pt[0], Lines);
				break;
					
			case ID_OP4:
			case ID_OP5:
			case ID_OP6:
			case ID_OP7:
				bArrow = false;
				eJoin = No;
				
				wPrvKeyDwn = 0;
				
				app.RubberBandingDisable();
				
				if (!bConductor || Pnt_DistanceTo_xy(pt1, pt[1]) > .05) 
				{
					CLine ln;
					double dRel;
					
					if (SubProcPowerFndCirc(ln, pt[1], pt1, &dRel)) 
					{
						CPnt ptBeg = ln[0];
						CPnt ptEnd = ln[1];
						
						pt2 = pt1;
							
						if (fabs(ptEnd[0] - ptBeg[0]) > .025) 
						{
							if (ptBeg[0] > ptEnd[0]) 
								pt3 = ptBeg;
							else
								pt3 = ptEnd;
						}
						else if (ptBeg[1] > ptEnd[1]) 
							pt3 = ptBeg;
						else
							pt3 = ptEnd;
						
						bConductor = true;
						goto l400;
					}
					bConductor = false;
					// error
					pt[0] = pt[1];
				}
				else if (Pnt_DistanceTo_xy(pt2, pt3) > .04) 
				{	// Room for additional conductors on identified line
l400:				switch (LOWORD(wParam))
					{
						case ID_OP4:
							SubProcPowerGenGround(pt2, pt3);
							break;
				
						case ID_OP5:
							SubProcPowerGenHot(pt2, pt3);
							break;

						case ID_OP6:
							SubProcPowerGenSwitchLeg(pt2, pt3);
							break;
							
						case ID_OP7:
							SubProcPowerGenNeutral(pt2, pt3);
							break;
					}
					pt2 = Pnt_ProjPtTo(pt2, pt3, .04);
				}
				else		
				{	// No more room for additional conductors on identified line
					bConductor = false;
					// error 
					pt[0] = pt3;
				}
				break;

			case ID_OP8:
				// Generate circuit run to home arrow
				bConductor = false;
				eJoin = No;
				wPrvKeyDwn = 0;
				
				app.RubberBandingDisable();
				
				if (!bArrow || Pnt_DistanceTo_xy(pt1, pt[1]) > .05) 
				{
					CLine ln;
					double dRel;
					if (SubProcPowerFndCirc(ln, pt[1], pt1, &dRel)) 
					{
						if (dRel <= .5) 
						{
							pt3 = ln[1];
							if (Pnt_DistanceTo_xy(pt1, ln[0]) <= .1) 
								pt1 = ln[0];
						}
						else
						{
							pt3 = ln[0];
							if (Pnt_DistanceTo_xy(pt1, ln[1]) <= .1) 
								pt1 = ln[1];
						}
						pt2 = pt1;
						bArrow = true;
						goto l300;
					}
					bArrow = false;
					// error
					pt[0] = pt[1];
				}
				else if (Pnt_DistanceTo_xy(pt2, pt3) > .04) 
				{	// Room for additional circuit run to home arrows on identified line
l300:				pt[2] = Pnt_ProjPtTo(pt2, pt3, .05);
					line::ProjPtFrom_xy(CLine(pt[2], pt3), 0., - .075, &pt[3]);
					line::ProjPtFrom_xy(CLine(pt[2], pt3), 0., .075, &pt[4]);
					CSeg* pSeg = new CSeg;
					pSeg->AddTail(new CPrimLine(2, 1, pt[3], pt2));
					pSeg->AddTail(new CPrimLine(2, 1, pt2, pt[4]));
					pDoc->WorkLayerAddTail(pSeg);
					pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
					pt2 = Pnt_ProjPtTo(pt2, pt3, .04);
				}
				else
				{	// No more room for additional circuit run to home arrows on identified line
					bArrow = false;
					// error 
					pt[0] = pt3;
				}
				break;

			case IDM_RETURN:
			case IDM_ESCAPE:
				if (wPrvKeyDwn != 0) 
					app.RubberBandingDisable();
				else
					pDoc->DoEditPrimitiveEscape();

				bArrow = false;
				bConductor = false;
				wPrvKeyDwn = 0;
				eJoin = No;
				
				break;
		}
	}
	return (CallWindowProc(app.GetMainWndProc(), hwnd, nMsg, wParam, lParam));
}

int SubProcPowerFndCirc(CLine& ln, const CPnt& pt, CPnt& ptProj, double* dRel)
{
	double dDis;

	POSITION pos = detsegs.GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = detsegs.GetNext(pos);
		
		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim != 0)
		{
			CPrim* pPrim = pSeg->GetNext(posPrim);
			if (pPrim->Is(CPrim::PRIM_LINE))
			{
				CPrimLine* pLine = static_cast<CPrimLine*>(pPrim);

				pLine->GetLine(ln);
				
				if (line::EvalPtProx_xy(ln, pt, .05, ptProj, dRel, &dDis))
					return (1);
			}
		}
	}
	return 0;
}
///<summary>Generate ground circuit conductor</summary>
void SubProcPowerGenGround(const CPnt& pt2, const CPnt& pt3)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CPnt pt[3];
	
	line::ProjPtFrom_xy(CLine(pt2, pt3), 0., - .1, &pt[0]);
	line::ProjPtFrom_xy(CLine(pt2, pt3), 0., .075, &pt[1]);
	line::ProjPtFrom_xy(CLine(pt2, pt3), 0., .0875, &pt[2]);
												
	CVec vPlnNorm = - view::GetDirection();
	CVec vXAx(pt[2], pt[1]);
	CVec vYAx = vXAx;
	vYAx.RotAboutArbAx(vPlnNorm, HALF_PI);
							
	CSeg* pSeg = new CSeg;
	pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[1]));
	pSeg->AddTail(new CPrimArc(1, 1, pt[2], vXAx, vYAx, TWOPI));
	pDoc->WorkLayerAddTail(pSeg);
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
}
///<summary>Generate hot circuit conductor</summary>
void SubProcPowerGenHot(const CPnt& pt2, const CPnt& pt3)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CPnt pt[2];
	
	line::ProjPtFrom_xy(CLine(pt2, pt3), 0., - .1, &pt[0]);
	line::ProjPtFrom_xy(CLine(pt2, pt3), 0., .1, &pt[1]);
							
	CSeg* pSeg = new CSeg(new CPrimLine(1, 1, pt[0], pt[1]));
	pDoc->WorkLayerAddTail(pSeg);
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
}
void SubProcPowerGenNeutral(const CPnt& pt2, const CPnt& pt3)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CPnt pt[2];

	line::ProjPtFrom_xy(CLine(pt2, pt3), 0., - .05, &pt[0]);
	line::ProjPtFrom_xy(CLine(pt2, pt3), 0., .05, &pt[1]);
							
	CSeg* pSeg = new CSeg(new CPrimLine(1, 1, pt[0], pt[1]));
	pDoc->WorkLayerAddTail(pSeg);
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
}
///<summary>Generate switch leg circuit conductor</summary>
void SubProcPowerGenSwitchLeg(const CPnt& pt2, const CPnt& pt3)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CPnt pt[5];

	line::ProjPtFrom_xy(CLine(pt2, pt3), 0., - .1, &pt[0]);
	line::ProjPtFrom_xy(CLine(pt2, pt3), 0., .05, &pt[1]);
	
	pt[2] = Pnt_ProjPtTo(pt2, pt3, .025);
	
	line::ProjPtFrom_xy(CLine(pt[2], pt3), 0., .075, &pt[3]);
	line::ProjPtFrom_xy(CLine(pt2, pt3), 0., .1, &pt[4]);
													
	CSeg* pSeg = new CSeg;
	pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[1]));
	pSeg->AddTail(new CPrimLine(1, 1, pt[1], pt[3]));
	pSeg->AddTail(new CPrimLine(1, 1, pt[3], pt[4]));
	pDoc->WorkLayerAddTail(pSeg);
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
}