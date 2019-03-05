#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "SubProcTrapAdd.h"

///<summary>Add segments to trap</summary>
// Notes:  Convex polygon trap (operation 3) not implemented
//	   Area trap (operation 4) needs to be generalized to quad.
//	   Making trap invisible marks segments for potential deletion elsewhere.
LRESULT CALLBACK SubProcTrapAdd(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	static int	iPts;
	static WORD wPrvKeyDwn = 0;
	static CPnt pt[8];

	char szKey[] = "TRAP";
	
	switch (nMsg)
	{
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case ID_HELP_KEY:
					WinHelp(hwnd, "peg.hlp", HELP_KEY, (DWORD) (LPSTR) szKey);
					return 0;
				
				case ID_OP0:
					app.OnTrapCommandsAddSegments();
					return 0;

				case ID_OP1:
					pt[0] = app.CursorPosGet();
					trapsegs.AddSegsAtPt(&detsegs, pt[0]);					
					return 0;

				case ID_OP2:
					if (wPrvKeyDwn != ID_OP2) 
					{
						pt[0] = app.CursorPosGet();
						app.RubberBandingStartAtEnable(pt[0], Lines);
						wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP2);
					}
					else 
					{
						pt[1] = app.CursorPosGet();
						trap::AddByLn(&detsegs, pt[0], pt[1]);
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
						pt[0] = app.CursorPosGet();
						app.RubberBandingStartAtEnable(pt[0], Rectangles);
						wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP4);
					}
					else 
					{
						pt[1] = app.CursorPosGet();
						trap::AddByArea(&detsegs, pt[0], pt[1]);
						app.RubberBandingDisable();
						app.ModeLineUnhighlightOp(wPrvKeyDwn);
					}
					app.StatusLineDisplay(TrapCnt);
					return 0;

				case ID_OP5:
					trap::AddLast();
					return 0;

				case ID_OP6:		// Check engaged segment
					if (detsegs.IsSegEngaged())
					{
						POSITION pos = pDoc->WorkLayerFind(detsegs.DetSeg());
						
						CSeg* pSeg = pDoc->WorkLayerGetNext(pos);
						
						if (trapsegs.Find(pSeg) == 0)
						{
							trapsegs.AddTail(pSeg);
							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE_TRAP, pSeg);
							app.StatusLineDisplay(TrapCnt);
						}
					}
					else {msgInformation(0);}
					
					return 0;

				case ID_OP7:		// compress trap and release all
					trapsegs.Compress();
					trapsegs.RemoveAll();
					app.StatusLineDisplay(TrapCnt);
					pDoc->UpdateAllViews(NULL);
					return 0;

				case ID_OP8:
					DoEditTrapPopupCommands(hwnd);
					return 0;
				
				case ID_OP9:
					if (!trapsegs.IsEmpty())		// Trap is not empty
						::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_TRAP_MODIFY), hwnd, DlgProcTrapModify);
					else {msgInformation(0);}
					return 0;


				case IDM_RETURN:
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
		
		case WM_LBUTTONDBLCLK:
			pt[0] = app.CursorPosGet();
			trapsegs.AddSegsAtPt(&detsegs, pt[0]);					
			return 0;
	}
	return (CallWindowProc(app.GetMainWndProc(), hwnd, nMsg, wParam, lParam));
}

// Identifies segments which lie wholly or partially within a rectangular area.
void trap::AddByArea(CSegs*	pSegs, CPnt pt0, CPnt pt1)		
{	
	CPegDoc* pDoc = CPegDoc::GetDoc();
	
	CPnt4 pt[] = {CPnt4(pt0, 1.), CPnt4(pt1, 1.)};

	ModelView_DoTransform(2, pt);

	if (CVec(pt[0], pt[1]).IsNull()) 
		return;

	line::Extents_xy(CLine(pt[0], pt[1]), pt0, pt1);
	
	POSITION pos = pSegs->GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = pSegs->GetNext(pos);
		
		if (trapsegs.Find(pSeg) != 0)					// already in trap
			continue;

		if (pSeg->SelUsingRect(pt0, pt1))
		{
			trapsegs.AddTail(pSeg);
			pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE_TRAP, pSeg);
		}
	}
	app.StatusLineDisplay(TrapCnt);
}

// Identifies segments which intersect with a line and adds them to the trap.
void trap::AddByLn(CSegs* pSegs, CPnt ptBeg, CPnt ptEnd)
{	
	CPegDoc* pDoc = CPegDoc::GetDoc();
	
	if (CVec(ptBeg, ptEnd).IsNull()) 
		return;
	
	CPnt4 ptView[] = {CPnt4(ptBeg, 1.), CPnt4(ptEnd, 1.)};
	
	ModelView_DoTransform(2, ptView);
	
	CPnts ptsInt;

	POSITION pos = pSegs->GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = pSegs->GetNext(pos);
		
		if (trapsegs.Find(pSeg) != 0)// already in trap
			continue;
	
		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim != 0)
		{
			CPrim* pPrim = pSeg->GetNext(posPrim);
			if (pPrim->SelUsingLine(CLine(ptView[0], ptView[1]), ptsInt))
			{
				trapsegs.AddTail(pSeg);
				pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE_TRAP, pSeg);
				break;
			}
		}
	}
	app.StatusLineDisplay(TrapCnt);
}

void trap::AddLast()
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	// adds last editable segment to trap
	POSITION pos = pDoc->WorkLayerGetTailPosition();
	while (pos != 0)
	{
		CSeg* pSeg = pDoc->WorkLayerGetPrev(pos);
					
		if (!trapsegs.Find(pSeg))
		{	// not already in trap
			trapsegs.AddTail(pSeg);
			pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE_TRAP, pSeg);
			CPrim *pPrim = (CPrim *)pSeg->GetHead();
			CPnt pt = pPrim->GetCtrlPt();
			app.CursorPosSet(pt);
			break;
		}
	}
	app.StatusLineDisplay(TrapCnt);
}
