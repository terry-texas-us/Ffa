#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

void pTrapRemByArea(CPnt, CPnt);
void pTrapRemByLn(CPnt, CPnt);

///<summary>Remove segments from trap</summary>
// Notes:  Convex polygon trap (operation 3) not implemented
//	   Area trap (operation 4) needs to be generalized to quad.
//	   Making trap invisible marks segments for potential deletion elsewhere.
LRESULT CALLBACK SubProcTrapRemove(HWND hwnd, UINT anMsg, WPARAM wParam, LPARAM lParam)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CSeg*		pSeg;
	static int	iPts;
	static WORD wPrvKeyDwn = 0;
	static CPnt rPt[8];

	char	szKey[] = "TRAP";
		
	switch (anMsg)
	{
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case ID_HELP_KEY:
                    HtmlHelp(hwnd, app.GetAppPath() + "\\PegAeSys.chm::/Trap.htm", HH_DISPLAY_TOPIC, NULL);
					return 0;
	
				case ID_OP0:
					app.OnTrapCommandsAddSegments();
					return 0;

				case ID_OP1:
					rPt[0] = app.CursorPosGet();
					trapsegs.RemoveSegsAtPt(rPt[0]);
					app.StatusLineDisplay(TrapCnt);
					return 0;

				case ID_OP2:
					if (wPrvKeyDwn != ID_OP2) 
					{
						rPt[0] = app.CursorPosGet();
						app.RubberBandingStartAtEnable(rPt[0], Lines);
						wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP2);
					}
					else 
					{
						rPt[1] = app.CursorPosGet();
						pTrapRemByLn(rPt[0], rPt[1]);
						app.RubberBandingDisable();
						app.ModeLineUnhighlightOp(wPrvKeyDwn);
					}
					app.StatusLineDisplay(TrapCnt);
					return 0;
				
				case ID_OP3:
					return 0;

				case ID_OP4:
					if (wPrvKeyDwn != ID_OP4) 
					{
						rPt[0] = app.CursorPosGet();
						app.RubberBandingStartAtEnable(rPt[0], Rectangles);
						wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP4);
					}
					else 
					{
						rPt[1] = app.CursorPosGet();
						pTrapRemByArea(rPt[0], rPt[1]);
						app.RubberBandingDisable();
						app.ModeLineUnhighlightOp(wPrvKeyDwn);
					}
					app.StatusLineDisplay(TrapCnt);
					return 0;

				case ID_OP5:
					if (!trapsegs.IsEmpty())
					{
						pSeg = trapsegs.RemoveTail();
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
					}
					app.StatusLineDisplay(TrapCnt);
					return 0;

				case ID_OP7:
					return 0;

				case ID_OP8:
					DoEditTrapPopupCommands(hwnd);
					return 0;

				case ID_OP9:
					if (!trapsegs.IsEmpty())
						::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_TRAP_MODIFY), hwnd, DlgProcTrapModify);
					else 
						msgInformation(0);
					return 0;

				case IDM_RETURN:
					app.StatusLineDisplay(TrapCnt);
					return 0;

				case IDM_ESCAPE:
					if(wPrvKeyDwn!=0)
					app.RubberBandingDisable();
					else
					{
					pDoc->DoEditPrimitiveEscape();
					app.ModeLineUnhighlightOp(wPrvKeyDwn);
					}
					return 0;
			}
			break;
	}
	return (CallWindowProc(app.GetMainWndProc(), hwnd, anMsg, wParam, lParam));
}

///<summary>Identifies segments which lie wholly or partially within a orthoganal rectangular area.</summary>
// Notes: This routine fails in all but top view. !!
// Parameters:	pt1 	one corner of the area
//				pt2 	other corner of the area
void pTrapRemByArea(CPnt pt1, CPnt pt2)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CPnt4 pt[] = {CPnt4(pt1, 1.), CPnt4(pt2, 1.)};
	ModelView_DoTransform(2, pt);

	if(CVec(pt[0], pt[1]).IsNull())
		return;
	
	line::Extents_xy(CLine(pt[0], pt[1]), pt1, pt2);
		
	POSITION pos = trapsegs.GetHeadPosition();
	while(pos != 0)
	{
		CSeg *pSeg = trapsegs.GetNext(pos);

		if (pSeg->SelUsingRect(pt1, pt2))
		{
			trapsegs.RemoveAt(trapsegs.Find(pSeg));
			pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
		}
	} 
}

///<summary>Identifies segments which intersect with a line and removes them from the trap.</summary>
void pTrapRemByLn(CPnt ptBeg, CPnt ptEnd)		
{	
	CPegDoc* pDoc = CPegDoc::GetDoc();

	if (CVec(ptBeg, ptEnd).IsNull()) 
		return;
	
	CPnt4 ptView[] = {CPnt4(ptBeg, 1.), CPnt4(ptEnd, 1.)};
	
	ModelView_DoTransform(2, ptView);
	
	CPnts ptsInt;

	POSITION pos = trapsegs.GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = trapsegs.GetNext(pos);
		
		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim != 0)
		{
			CPrim* pPrim = pSeg->GetNext(posPrim);
	
			if (pPrim->SelUsingLine(CLine(ptView[0], ptView[1]), ptsInt))
			{
				trapsegs.RemoveAt(trapsegs.Find(pSeg));
				pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
				break;
			}
		}
	}
}