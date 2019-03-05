#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

void DlgProcSetupNoteInit(HWND hDlg);
void DlgProcSetupNotePaint(HWND hDlg, RECT dimen);
void DlgProcEnableControls(HWND hDlg);

extern CRefSys *rsNBuff;
extern CFontDef *fdNBuff;

CString strFontName = "simplex.psf";

BOOL CALLBACK DlgProcOptionsTabNote(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();
	CPegView* pView = CPegView::GetActiveView();
	CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();

	RECT dimen;
	dimen.top = 2;
	dimen.bottom = 33;
//	dimen.left = 20;
	dimen.left = 185;
	dimen.right = 375;

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			DlgProcSetupNoteInit(hDlg);

//			SetFocus(::GetDlgItem(hDlg, IDC_TEXT_HEIGHT));
//			SendDlgItemMessage(hDlg, IDC_TEXT_HEIGHT, EM_SETSEL, 0, -1);

			return (TRUE);
		}
		case WM_PAINT:
		{
			if((rsNBuff == 0) && (fdNBuff == 0))
				DlgProcSetupNotePaint(hDlg, dimen);

			return (TRUE);
		}
		case WM_DIALOG_FOCUS:
		{
			switch(pDoc->GetOptionsFocus())
			{
				case IDC_TEXT_HEIGHT:
					SetFocus(::GetDlgItem(hDlg, IDC_TEXT_HEIGHT));
					SendDlgItemMessage(hDlg, IDC_TEXT_HEIGHT, EM_SETSEL, 0, -1);
					break;
				default:					
					SetFocus(GetWindow(hDlg,GW_CHILD));
					break;
			}
			return TRUE;
		}	
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{
				case IDC_NBUF_CLEAR:
				{
					if(HIWORD(wParam) == BN_CLICKED)
					{
						fdNBuff = 0;
						rsNBuff = 0;
						
						DlgProcEnableControls(hDlg);
					}

					return (TRUE);
				}
				case IDC_FONT_NAME:
				{
					if (HIWORD(wParam) == LBN_SELCHANGE)
					{
						// code to get name of font
						LRESULT lrSel = ::SendDlgItemMessage(hDlg, IDC_FONT_NAME, CB_GETCURSEL, 0, 0L);
						if (lrSel != CB_ERR)
						{
							char szBuf[32];

							::SendDlgItemMessage(hDlg, IDC_FONT_NAME, CB_GETLBTEXT, (WPARAM) lrSel, (LPARAM) ((LPSTR) szBuf));
							strFontName = szBuf;

							InvalidateRect(hDlg, &dimen, TRUE);
							SendMessage(hDlg, WM_PAINT, 0, 0);
						}
					}
					return (TRUE);
				}
			}
			break;
		}
		case WM_DIALOG_OK:
		{
			app.SetEditSegBeg(app.CursorPosGet());
			
			CCharCellDef ccd;
			pstate.GetCharCellDef(ccd);
			
			char szBuf[32];

			::GetDlgItemText(hDlg, IDC_TEXT_HEIGHT, szBuf, sizeof(szBuf));
			ccd.ChrHgtSet(atof(szBuf));
			::GetDlgItemText(hDlg, IDC_TEXT_ROTATION, szBuf, sizeof(szBuf));
			ccd.TextRotAngSet(atof(szBuf) * RADIAN);
			::GetDlgItemText(hDlg, IDC_TEXT_EXP_FAC, szBuf, sizeof(szBuf));
			ccd.ChrExpFacSet(atof(szBuf));
			::GetDlgItemText(hDlg, IDC_TEXT_INCLIN, szBuf, sizeof(szBuf));
			ccd.ChrSlantAngSet(atof(szBuf) * RADIAN);
			
			CFontDef fd;
			pstate.GetFontDef(fd);
			
			::GetDlgItemText(hDlg, IDC_TEXT_SPACING, szBuf, sizeof(szBuf));
			fd.ChrSpacSet(atof(szBuf));
			
			WORD wHorAlign = IDC_TEXT_ALIGN_HOR_LEFT;
			while (!IsDlgButtonChecked(hDlg, wHorAlign))
				wHorAlign++;
			wHorAlign = WORD(1 - IDC_TEXT_ALIGN_HOR_LEFT + wHorAlign);
			
			WORD wVerAlign = IDC_TEXT_ALIGN_VER_BOT;
			while (!IsDlgButtonChecked(hDlg, wVerAlign))
				wVerAlign++;
			wVerAlign = WORD(4 + IDC_TEXT_ALIGN_VER_BOT - wVerAlign);
			
			fd.TextHorAlignSet(wHorAlign);
			fd.TextVerAlignSet(wVerAlign);
								
			WORD wPath = IDC_PATH_RIGHT;
			while (!IsDlgButtonChecked(hDlg, wPath))
				wPath++;
			wPath = WORD(wPath - IDC_PATH_RIGHT);
			
			fd.TextPathSet(wPath);

			LRESULT lrSel = ::SendDlgItemMessage(hDlg, IDC_FONT_NAME, CB_GETCURSEL, 0, 0L);
			if (lrSel != CB_ERR)
			{
				::SendDlgItemMessage(hDlg, IDC_FONT_NAME, CB_GETLBTEXT, (WPARAM) lrSel, (LPARAM) ((LPSTR) szBuf));
				fd.TextFontSet(szBuf);	
				
				WORD wPrec = (strcmp(szBuf, "Simplex.psf") != 0) ? CFontDef::PREC_TRUETYPEFONT : CFontDef::PREC_PEGSTROKEFONT;
				fd.TextPrecSet(wPrec);
			}
			pstate.SetFontDef(pDC, fd);
			pstate.SetCharCellDef(ccd);
			
			break;
		}
	}
	return (FALSE); 											
}

void DlgProcSetupNotePaint(HWND hDlg, RECT dimen)
{
	COLORREF old;
	PAINTSTRUCT ps;
	CDC dc;
	dc.Attach(::BeginPaint(hDlg, &ps));

	CDC m_dcMem;
	m_dcMem.CreateCompatibleDC( NULL );

	HFONT newfont;
	newfont = CreateFont(0, 0, 0, 0, FW_DONTCARE, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
						 CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, 
						 strFontName);
	dc.SelectObject(newfont);

	old = dc.SetBkColor(RGB(255, 255, 255));

	dc.Rectangle(dimen.left - 1, dimen.top - 1, dimen.right + 1, dimen.bottom + 1);
	dc.DrawText("12345AaBbCcDdEe", &dimen, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	
	dc.SetBkColor(old);
	dc.Detach();

	DeleteObject(newfont);

	::EndPaint(hDlg, &ps);
}

int CALLBACK EnumFontFamProc(const LPLOGFONT lplf, const LPTEXTMETRIC, int, LPARAM lParam)
{
	HWND hDlg = HWND(lParam);

	const LPENUMLOGFONT elf = (const LPENUMLOGFONT) lplf;
	::SendDlgItemMessage(hDlg, IDC_FONT_NAME, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) elf->elfFullName);

	return 1;
}

void DlgProcSetupNoteInit(HWND hDlg)
{
	CPegView* pView = CPegView::GetActiveView();
	CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();
	
	LOGFONT lf;
	::ZeroMemory(&lf, sizeof(lf));

	lf.lfFaceName[0] = '\0';
	lf.lfCharSet = ANSI_CHARSET;			
	::EnumFontFamiliesEx(pDC->GetSafeHdc(), &lf, (FONTENUMPROC) EnumFontFamProc, LPARAM (hDlg), 0);

	lf.lfCharSet = SYMBOL_CHARSET;
	::EnumFontFamiliesEx(pDC->GetSafeHdc(), &lf, (FONTENUMPROC) EnumFontFamProc, LPARAM (hDlg), 0);

	::SendDlgItemMessage(hDlg, IDC_FONT_NAME, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) "Simplex.psf");
	
	CCharCellDef ccd;
	pstate.GetCharCellDef(ccd);

	DlgBoxSetItemDouble(hDlg, IDC_TEXT_HEIGHT, ccd.ChrHgtGet());
	DlgBoxSetItemDouble(hDlg, IDC_TEXT_ROTATION, ccd.TextRotAngGet() / RADIAN);
	DlgBoxSetItemDouble(hDlg, IDC_TEXT_EXP_FAC, ccd.ChrExpFacGet());
	DlgBoxSetItemDouble(hDlg, IDC_TEXT_INCLIN, ccd.ChrSlantAngGet() / RADIAN);
	
	CFontDef fd;
	pstate.GetFontDef(fd);
			
	char szBuf[32];
	strcpy(szBuf, fd.TextFont());
	::SendDlgItemMessage(hDlg, IDC_FONT_NAME, CB_SELECTSTRING, (WPARAM) - 1, (LPARAM) ((LPSTR) szBuf));
			
	DlgBoxSetItemDouble(hDlg, IDC_TEXT_SPACING, fd.ChrSpac());
			
	::CheckRadioButton(hDlg, IDC_PATH_RIGHT, IDC_PATH_DOWN, IDC_PATH_RIGHT + fd.TextPath());
	::CheckRadioButton(hDlg, IDC_TEXT_ALIGN_HOR_LEFT, IDC_TEXT_ALIGN_HOR_RIGHT, IDC_TEXT_ALIGN_HOR_LEFT + fd.TextHorAlign() - 1);
	::CheckRadioButton(hDlg, IDC_TEXT_ALIGN_VER_BOT, IDC_TEXT_ALIGN_VER_TOP, IDC_TEXT_ALIGN_VER_BOT - fd.TextVerAlign() + 4);

	DlgProcEnableControls(hDlg);
}

void DlgProcEnableControls(HWND hDlg)
{
	if((rsNBuff != 0) && (fdNBuff != 0))
	{
		::ShowWindow(::GetDlgItem(hDlg, IDC_HEIGHT_TEXT), false);
		::ShowWindow(::GetDlgItem(hDlg, IDC_TEXT_HEIGHT), false);

		::ShowWindow(::GetDlgItem(hDlg, IDC_EXP_TEXT), false);
		::ShowWindow(::GetDlgItem(hDlg, IDC_TEXT_EXP_FAC), false);

		::ShowWindow(::GetDlgItem(hDlg, IDC_ROTATION_TEXT), false);
		::ShowWindow(::GetDlgItem(hDlg, IDC_TEXT_ROTATION), false);

		::ShowWindow(::GetDlgItem(hDlg, IDC_SPACING_TEXT), false);
		::ShowWindow(::GetDlgItem(hDlg, IDC_TEXT_SPACING), false);
		
		::ShowWindow(::GetDlgItem(hDlg, IDC_INC_TEXT), false);
		::ShowWindow(::GetDlgItem(hDlg, IDC_TEXT_INCLIN), false);

		::ShowWindow(::GetDlgItem(hDlg, IDC_FONT_TEXT), false);
		::ShowWindow(::GetDlgItem(hDlg, IDC_FONT_NAME), false);

		::ShowWindow(::GetDlgItem(hDlg, IDC_ALIGN_TEXT), false);
		::ShowWindow(::GetDlgItem(hDlg, IDC_HORZ_TEXT), false);
		::ShowWindow(::GetDlgItem(hDlg, IDC_VERT_TEXT), false);
		::ShowWindow(::GetDlgItem(hDlg, IDC_TEXT_ALIGN_HOR_LEFT), false);
		::ShowWindow(::GetDlgItem(hDlg, IDC_TEXT_ALIGN_HOR_CEN), false);
		::ShowWindow(::GetDlgItem(hDlg, IDC_TEXT_ALIGN_HOR_RIGHT), false);
		::ShowWindow(::GetDlgItem(hDlg, IDC_TEXT_ALIGN_VER_BOT), false);
		::ShowWindow(::GetDlgItem(hDlg, IDC_TEXT_ALIGN_VER_MID), false);
		::ShowWindow(::GetDlgItem(hDlg, IDC_TEXT_ALIGN_VER_TOP), false);

		::ShowWindow(::GetDlgItem(hDlg, IDC_PATH_TEXT), false);
		::ShowWindow(::GetDlgItem(hDlg, IDC_PATH_RIGHT), false);
		::ShowWindow(::GetDlgItem(hDlg, IDC_PATH_LEFT), false);
		::ShowWindow(::GetDlgItem(hDlg, IDC_PATH_UP), false);
		::ShowWindow(::GetDlgItem(hDlg, IDC_PATH_DOWN), false);

		::ShowWindow(::GetDlgItem(hDlg, IDC_NBUF_GROUP), true);
		::ShowWindow(::GetDlgItem(hDlg, IDC_NBUF_TEXT), true);
		::ShowWindow(::GetDlgItem(hDlg, IDC_NBUF_CLEAR), true);
	}
	else
	{
		::ShowWindow(::GetDlgItem(hDlg, IDC_HEIGHT_TEXT), true);
		::ShowWindow(::GetDlgItem(hDlg, IDC_TEXT_HEIGHT), true);

		::ShowWindow(::GetDlgItem(hDlg, IDC_EXP_TEXT), true);
		::ShowWindow(::GetDlgItem(hDlg, IDC_TEXT_EXP_FAC), true);

		::ShowWindow(::GetDlgItem(hDlg, IDC_ROTATION_TEXT), true);
		::ShowWindow(::GetDlgItem(hDlg, IDC_TEXT_ROTATION), true);

		::ShowWindow(::GetDlgItem(hDlg, IDC_SPACING_TEXT), true);
		::ShowWindow(::GetDlgItem(hDlg, IDC_TEXT_SPACING), true);
		
		::ShowWindow(::GetDlgItem(hDlg, IDC_INC_TEXT), true);
		::ShowWindow(::GetDlgItem(hDlg, IDC_TEXT_INCLIN), true);

		::ShowWindow(::GetDlgItem(hDlg, IDC_FONT_TEXT), true);
		::ShowWindow(::GetDlgItem(hDlg, IDC_FONT_NAME), true);

		::ShowWindow(::GetDlgItem(hDlg, IDC_ALIGN_TEXT), true);
		::ShowWindow(::GetDlgItem(hDlg, IDC_HORZ_TEXT), true);
		::ShowWindow(::GetDlgItem(hDlg, IDC_VERT_TEXT), true);
		::ShowWindow(::GetDlgItem(hDlg, IDC_TEXT_ALIGN_HOR_LEFT), true);
		::ShowWindow(::GetDlgItem(hDlg, IDC_TEXT_ALIGN_HOR_CEN), true);
		::ShowWindow(::GetDlgItem(hDlg, IDC_TEXT_ALIGN_HOR_RIGHT), true);
		::ShowWindow(::GetDlgItem(hDlg, IDC_TEXT_ALIGN_VER_BOT), true);
		::ShowWindow(::GetDlgItem(hDlg, IDC_TEXT_ALIGN_VER_MID), true);
		::ShowWindow(::GetDlgItem(hDlg, IDC_TEXT_ALIGN_VER_TOP), true);

		::ShowWindow(::GetDlgItem(hDlg, IDC_PATH_TEXT), true);
		::ShowWindow(::GetDlgItem(hDlg, IDC_PATH_RIGHT), true);
		::ShowWindow(::GetDlgItem(hDlg, IDC_PATH_LEFT), true);
		::ShowWindow(::GetDlgItem(hDlg, IDC_PATH_UP), true);
		::ShowWindow(::GetDlgItem(hDlg, IDC_PATH_DOWN), true);

		::ShowWindow(::GetDlgItem(hDlg, IDC_NBUF_GROUP), false);
		::ShowWindow(::GetDlgItem(hDlg, IDC_NBUF_TEXT), false);
		::ShowWindow(::GetDlgItem(hDlg, IDC_NBUF_CLEAR), false);
	}
}