#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "DlgProcTrapModify.h"
#include "Hatch.h"

///<summary>Modifies attributes of all segment primatives in current trap tocurrent settings.</summary>
///<remarks>Trap color index is not modified.</remarks>
BOOL CALLBACK DlgProcTrapModify(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	switch (anMsg)
	{
		case WM_INITDIALOG: 								
			return (TRUE);
	
		case WM_COMMAND:								
			switch (LOWORD(wParam)) 
			{
				case IDOK:
					DlgProcTrapModifyDoOK(hDlg);				
					pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEGS_SAFE_TRAP, &trapsegs);

				case IDCANCEL:
					::EndDialog(hDlg, TRUE);
					return (TRUE);
			}
	}
	return (FALSE); 		
}

void DlgProcTrapModifyDoOK(HWND hDlg)
{
	if (::SendDlgItemMessage(hDlg, IDC_MOD_MARKER, BM_GETCHECK, 0, 0L))
		trapsegs.ModifyMarkers();
	if (::SendDlgItemMessage(hDlg, IDC_MOD_PEN, BM_GETCHECK, 0, 0L))
		trapsegs.ModifyPenColor(pstate.PenColor());
	if (::SendDlgItemMessage(hDlg, IDC_MOD_LINE, BM_GETCHECK, 0, 0L))
		trapsegs.ModifyPenStyle(pstate.PenStyle());
	if (::SendDlgItemMessage(hDlg, IDC_MOD_FILL, BM_GETCHECK, 0, 0L))
		DlgProcTrapModifyPolygons();
	
	CCharCellDef ccd;
	pstate.GetCharCellDef(ccd);
	
	CFontDef fd;
	pstate.GetFontDef(fd);
	
	if (::SendDlgItemMessage(hDlg, IDC_MOD_NOTE, BM_GETCHECK, 0, 0L))
		trapsegs.ModifyNotes(fd, ccd, TM_TEXT_ALL);
	else if (::SendDlgItemMessage(hDlg, IDC_FONT, BM_GETCHECK, 0, 0L))
		trapsegs.ModifyNotes(fd, ccd, TM_TEXT_FONT);
	else if (::SendDlgItemMessage(hDlg, IDC_HEIGHT, BM_GETCHECK, 0, 0L))
		trapsegs.ModifyNotes(fd, ccd, TM_TEXT_HEIGHT);

}

void DlgProcTrapModifyPolygons()
{
	POSITION pos = trapsegs.GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = trapsegs.GetNext(pos);
		
		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim != 0)
		{
			CPrim* pPrim = pSeg->GetNext(posPrim);
			
			if (pPrim->Is(CPrim::PRIM_POLYGON))
			{
				CPrimPolygon* pPolygon = static_cast<CPrimPolygon*>(pPrim);
				pPolygon->SetIntStyle(pstate.PolygonIntStyle());
				pPolygon->SetIntStyleId(pstate.PolygonIntStyleId());
				pPolygon->SetHatRefVecs(hatch::dOffAng, hatch::dXAxRefVecScal, hatch::dYAxRefVecScal);
			}
		}
	}
}