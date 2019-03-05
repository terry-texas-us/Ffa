#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

DWORD	dwMendPrimPtId;
CPrim*	pPrimToMend;
CPrim*	pPrimToMendCopy;
CPnt	ptMendPrimBeg;

///<summary>Mends primatives.</summary>
LRESULT CALLBACK SubProcMendPrim(HWND hwnd, UINT anMsg, WPARAM wParam, LPARAM lParam)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	if(anMsg == WM_MOUSEMOVE)
	{
		CPnt ptCurPos = app.CursorPosGet();
		CVec vTrns(ptMendPrimBeg, ptCurPos);
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_PRIM_ERASE_SAFE, pPrimToMendCopy);
		pPrimToMendCopy->TranslateUsingMask(vTrns, dwMendPrimPtId);
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_PRIM_SAFE, pPrimToMendCopy);
		ptMendPrimBeg = ptCurPos;
	}		
	else if(anMsg == WM_COMMAND) 
	{
		WORD wId = LOWORD(wParam);
		
		if(wId == IDM_RETURN || wId == IDM_ESCAPE)
		{
			pDoc->UpdateAllViews(NULL, CPegDoc::HINT_PRIM_ERASE_SAFE, pPrimToMendCopy);
		
			if(wId == IDM_RETURN)
				pPrimToMend->Assign(pPrimToMendCopy);
			
			pDoc->UpdateAllViews(NULL, CPegDoc::HINT_PRIM_SAFE, pPrimToMend);
		
			delete pPrimToMendCopy;
			
			app.SetWindowMode(app.m_iPrimModeId);
			return 0;
		}
	}
	return(CallWindowProc(app.GetMainWndProc(), hwnd, anMsg, wParam, lParam));
}