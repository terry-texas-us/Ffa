#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "DlgProcEditOps.h"

extern CTMat tmEditSeg;

void DoCopy();
void DoTransform(WORD);

// Edits segment
LRESULT CALLBACK SubProcEditSegment(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if(nMsg != WM_COMMAND) 
		return (CallWindowProc(app.GetMainWndProc(), hwnd, nMsg, wParam, lParam));

	CSeg*	pSeg;
	CPegDoc *pDoc = CPegDoc::GetDoc();
	static WORD wPrvKeyDwn = 0;

	if(AppGetGinCur(pSeg) == 0)
	{
		app.SetWindowMode(app.m_iPrimModeId);
		AppSetGinCur();
		return 0;
	}
	
	WORD wId = LOWORD(wParam);
		
	switch(wId)
	{
		case ID_OP0:
			pDoc->SetOptionsFocus(IDC_EDIT_OP_SIZING);
			pDoc->SetOptionsPage(CPegDoc::TAB_OPEN_EDIT);
			::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_TABS_MAIN), app.GetSafeHwnd(), DlgProcSetup);
			return 0;

		case ID_OP1:
		case ID_OP9:
			return 0;
		
		case ID_OP2:
		case ID_OP2_CTRL:
		case ID_OP3:
		case ID_OP3_CTRL:
		case ID_OP6:
		case ID_OP7:
		case ID_OP7_CTRL:
		case ID_OP8:
		case ID_OP8_CTRL:
			DoTransform(wId);
			return 0;

		case ID_OP5:
			DoCopy();
			return 0;
			
		case IDM_ESCAPE:
			if(wPrvKeyDwn!=0)
				app.RubberBandingDisable();
			else
				pDoc->DoEditPrimitiveEscape();
			return 0;
							
		case ID_OP4:
		case IDM_RETURN:
			AppSetGinCur();
			app.SetWindowMode(app.m_iPrimModeId);
			return 0;
	}
	return (CallWindowProc(app.GetMainWndProc(), hwnd, nMsg, wParam, lParam));
}

void DoCopy()
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	CSeg*	pSeg;

	if(AppGetGinCur(pSeg) != 0)
	{
		CSeg* pSegNew = new CSeg(*pSeg);
		pDoc->WorkLayerAddTail(pSegNew); 
		AppSetGinCur(pSegNew);

		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSegNew);
	}
}

void DoTransform(WORD wId)
{
	CSeg* pSeg;

	if(AppGetGinCur(pSeg) != 0)
	{
		CTMat tm;
		tm.Translate(ORIGIN - app.GetEditSegBeg());
		
		if(wId == ID_OP2)
			tm *= dlgproceditops::GetRotTrnMat();
		else if(wId == ID_OP2_CTRL)
			tm *= dlgproceditops::GetVernierRotTrnMat();
		else if(wId == ID_OP3)
			tm *= dlgproceditops::GetInvertedRotTrnMat();
		else if(wId == ID_OP3_CTRL)
			tm *= dlgproceditops::GetVernierInvertedRotTrnMat();
		else if(wId == ID_OP6)
			tm.Scale(dlgproceditops::GetMirrorScale());
		else if(wId == ID_OP7) 
			tm.Scale(dlgproceditops::GetInvertedScale());
		else if(wId == ID_OP7_CTRL)
			tm.Scale(dlgproceditops::GetVernierInvertedScale());
		else if(wId == ID_OP8)
			tm.Scale(dlgproceditops::GetScale());
		else
			tm.Scale(dlgproceditops::GetVernierScale());
	
		tm.Translate(app.GetEditSegBeg());
				
		CPegDoc* pDoc = CPegDoc::GetDoc();
				
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSeg);
		pSeg->Transform(tm);
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSeg); 				

		tmEditSeg *= tm;
	}
}