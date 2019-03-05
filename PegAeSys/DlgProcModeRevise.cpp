#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

//#include "AltNBuffer.h"

#include "Text.h"

extern CRefSys *rsNBuff;
extern CFontDef *fdNBuff;

///<remarks>
///Text related attributes for all notes generated will be same as those of the text last picked.
///Upon exit attributes restored to their entry values.
///</remarks>
BOOL CALLBACK DlgProcModeRevise(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	static CFontDef fd;
	static CRefSys rs;
	static CPrimText* pText = 0;
	//static CPegApp* app = CPegApp::g

	// mak - norris text color fix
	static PENCOLOR pReviseColor = -1;
	
	HWND hWndTextCtrl = ::GetDlgItem(hDlg, IDC_TEXT);

	switch (nMsg)
	{
		case WM_INITDIALOG:
			pText = detsegs.SelTextUsingPoint(app.CursorPosGet());
			if (pText != 0)
			{
				pText->GetFontDef(fd);
				pText->GetRefSys(rs);
				::SetWindowText(hWndTextCtrl, pText->Text());
				pReviseColor = pText->PenColor();
			}
			else
				::EndDialog(hDlg, TRUE);
			
			return (TRUE);
			
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{	
				case IDC_TEXT:
				{
					int iLen = ::GetWindowTextLength(hWndTextCtrl);
					char* pszText = new char[iLen + 1];
					::GetWindowText(hWndTextCtrl, pszText, iLen + 1);

					//CPnts textPts;
					//pText->GetBoundingBox(textPts, 0);
					//RECT textRect;
					//textRect.top = (LONG)textPts[0][0];
					//textRect.left = (LONG)textPts[0][1];
					//textRect.right = (LONG)textPts[1][0];
					//textRect.bottom = (LONG)textPts[1][1];
					
					pText->SetText(pszText);

					//pDoc->WorkLayerAddTail(pSeg);
					//pDoc->UpdateAllViews(NULL, CPegDoc::HINT_PRIM, pText);
					//::InvalidateRect(app.GetSafeHwnd(), &textRect, false);
					//CPegView::GetActiveView()->InvalidateRect(NULL, true);
					//pDoc->UpdateAllViews(NULL, CPegDoc::HINT_PRIM_ERASE_SAFE, pText);

					delete [] pszText;
					
					return (TRUE);
				}
				break;
				case IDOK:
				{
					int iLen = ::GetWindowTextLength(hWndTextCtrl);
					
					char* pszNew = new char[iLen + 1];
					::GetWindowText(hWndTextCtrl, pszNew, iLen + 1);
					
					if (pText != 0)
					{
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_PRIM_ERASE_SAFE, pText);
						pText->SetText(pszNew);
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_PRIM_SAFE, pText);
					}
					else
					{
						CPrim *pPrim;
						if((rsNBuff != 0) && (fdNBuff != 0))
							pPrim = new CPrimText(*fdNBuff, *rsNBuff, pszNew);
						else
							pPrim = new CPrimText(fd, rs, pszNew); 

						if(pReviseColor != -1)
							pPrim->SetPenColor(pReviseColor);

						CSeg *pSeg = new CSeg(pPrim);
						pDoc->WorkLayerAddTail(pSeg);
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
					}					
					delete [] pszNew;

					rs.SetOrigin(text_GetNewLinePos(fd, rs, 1., 0));
					
					pText = detsegs.SelTextUsingPoint(rs.Origin());
					if (pText != 0)
					{
						pText->GetFontDef(fd);
						pText->GetRefSys(rs);
						::SetWindowText(hWndTextCtrl, pText->Text());
						::SendMessage(hWndTextCtrl, EM_SETSEL, (WPARAM) 0, (LPARAM) -1);
					}
					else
						SetWindowText(hWndTextCtrl, "");
					
					::SetFocus(hWndTextCtrl);
					return (TRUE);
				}				
				case IDCANCEL:
					::EndDialog(hDlg, TRUE);
					return (TRUE);
			}
			break;
			
		case WM_SIZE:
			::SetWindowPos(hWndTextCtrl, 0, 0, 0, LOWORD(lParam), HIWORD(lParam), SWP_NOZORDER | SWP_NOMOVE);
			break;
	}
	return (FALSE);
}