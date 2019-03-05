#include "stdafx.h"

#include "PegAEsys.h"
#include "PegaesysDoc.h"
#include "PegAEsysView.h"

#include "DlgProcEditOps.h"

namespace dlgproceditops
{
	double dXRotAng = 0.0;
	double dYRotAng = 0.0;
	double dZRotAng = 45.0;

	double dXScale = 2.0;
	double dYScale = 2.0;
	double dZScale = 2.0;

	double dXMirrorScale = true;
	double dYMirrorScale = false;
	double dZMirrorScale = false;

	double dVernierSize = 0.01;
	double dVernierRot = 0.05;

	int iRotOrd[3] = {0, 1, 2};

	// functions
	CVec	GetMirrorScale();
	CVec	GetRotAng();
	CTMat	GetInvertedRotTrnMat();
	CTMat	GetRotTrnMat();
	CVec	GetScale();
	CVec	GetInvertedScale();
	
	CVec	GetVernierInvertedScale();
	CVec	GetVernierScale();
	CTMat	GetVernierInvertedRotTrnMat();
	CTMat	GetVernierRotTrnMat();

	void	SetMirrorScale(double dX, double dY, double dZ);
	void	SetRotAng(double dX, double dY, double dZ);
	void	SetRotOrd(int i0, int i1, int i2);
	void	SetScale(double dX, double dY, double dZ);
}

// Edits segments in trap
LRESULT CALLBACK SubProcEdit(HWND hwnd, UINT anMsg, WPARAM wParam, LPARAM lParam)
{
	static CPnt pt;

	static int	iPts;
	static WORD wPrvKeyDwn = 0;

	CPegDoc *pDoc = CPegDoc::GetDoc();

	if(anMsg == WM_COMMAND)
	{
		CTMat tm;
		char szKey[] = "EDIT";
		
		switch (LOWORD(wParam))
		{
			case ID_HELP_KEY:
				WinHelp(hwnd, "peg.hlp", HELP_KEY, (DWORD) (LPSTR) szKey);
				return 0;
			
			case ID_OP0:
				pDoc->SetOptionsFocus(IDC_EDIT_OP_SIZING);
				pDoc->SetOptionsPage(CPegDoc::TAB_OPEN_EDIT);
				::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_TABS_MAIN), app.GetSafeHwnd(), DlgProcSetup);
				return 0;

			case ID_OP1:		// Reposition trap pivot point
				pt = app.CursorPosGet();
				trapsegs.PvtPt() = pt;
				CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);
				return 0;

			case ID_OP2:
				trapsegs.PvtPt() = app.CursorPosGet();
				tm.Translate(ORIGIN - trapsegs.PvtPt());
				tm *= dlgproceditops::GetRotTrnMat();
				tm.Translate(trapsegs.PvtPt());
				trapsegs.TransformSegs(tm);
				return 0;

			case ID_OP2_CTRL:
				trapsegs.PvtPt() = app.CursorPosGet();
				tm.Translate(ORIGIN - trapsegs.PvtPt());
				tm *= dlgproceditops::GetVernierRotTrnMat();
				tm.Translate(trapsegs.PvtPt());
				trapsegs.TransformSegs(tm);
				return 0;
			
			case ID_OP3:
				trapsegs.PvtPt() = app.CursorPosGet();
				tm.Translate(ORIGIN - trapsegs.PvtPt());
				tm *= dlgproceditops::GetInvertedRotTrnMat();
				tm.Translate(trapsegs.PvtPt());
				trapsegs.TransformSegs(tm);
				return 0;

			case ID_OP3_CTRL:
				trapsegs.PvtPt() = app.CursorPosGet();
				tm.Translate(ORIGIN - trapsegs.PvtPt());
				tm *= dlgproceditops::GetVernierInvertedRotTrnMat();
				tm.Translate(trapsegs.PvtPt());
				trapsegs.TransformSegs(tm);
				return 0;
				
			case ID_OP4:
				pt = app.CursorPosGet();
				if (wPrvKeyDwn != ID_OP4) 
				{
					wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP4);
					app.RubberBandingStartAtEnable(pt, Lines);
				}
				else 
				{
					app.ModeLineUnhighlightOp(wPrvKeyDwn);
					app.RubberBandingDisable();
					tm.Translate(CVec(trapsegs.PvtPt(), pt));
					trapsegs.TransformSegs(tm);
				}
				trapsegs.PvtPt() = pt;
				return 0;
	
			case ID_OP5:
				pt = app.CursorPosGet();
				if (wPrvKeyDwn != ID_OP5) 
				{
					wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP5);
					app.RubberBandingStartAtEnable(pt, Lines);
				}
				else 
				{
					app.ModeLineUnhighlightOp(wPrvKeyDwn);
					app.RubberBandingDisable();
					trapsegs.Copy(CVec(trapsegs.PvtPt(), pt));
				}		
				trapsegs.PvtPt() = pt;
				return 0;

			case ID_OP6:
				tm.Translate(ORIGIN - trapsegs.PvtPt());
				tm.Scale(dlgproceditops::GetMirrorScale());
				tm.Translate(trapsegs.PvtPt());
				trapsegs.TransformSegs(tm);
				return 0;

			case ID_OP7:
				tm.Translate(ORIGIN - trapsegs.PvtPt());
				tm.Scale(dlgproceditops::GetInvertedScale());
				tm.Translate(trapsegs.PvtPt());
				trapsegs.TransformSegs(tm);
				return 0;

			case ID_OP7_CTRL:
				tm.Translate(ORIGIN - trapsegs.PvtPt());
				tm.Scale(dlgproceditops::GetVernierInvertedScale());
				tm.Translate(trapsegs.PvtPt());
				trapsegs.TransformSegs(tm);	
				return 0;
			
			case ID_OP8:
				tm.Translate(ORIGIN - trapsegs.PvtPt());
				tm.Scale(dlgproceditops::GetScale());
				tm.Translate(trapsegs.PvtPt());
				trapsegs.TransformSegs(tm);
				return 0;

			case ID_OP8_CTRL:
				tm.Translate(ORIGIN - trapsegs.PvtPt());
				tm.Scale(dlgproceditops::GetVernierScale());
				tm.Translate(trapsegs.PvtPt());
				trapsegs.TransformSegs(tm);
				return 0;

			case IDM_RETURN:
				return 0;

			case IDM_ESCAPE:

				if(wPrvKeyDwn!=0)
					app.RubberBandingDisable();
				else
				pDoc->DoEditPrimitiveEscape();				

				if (wPrvKeyDwn == ID_OP4 || wPrvKeyDwn == ID_OP5) 
				{
					app.ModeLineUnhighlightOp(wPrvKeyDwn);
					app.RubberBandingDisable();
				}

				return 0;
		}
	}
	return (CallWindowProc(app.GetMainWndProc(), hwnd, anMsg, wParam, lParam));
}

// edit functions
CVec dlgproceditops::GetMirrorScale()
{
	return (CVec(dlgproceditops::dXMirrorScale, dlgproceditops::dYMirrorScale, dlgproceditops::dZMirrorScale));
}

CVec dlgproceditops::GetRotAng()
{
	return (CVec(dlgproceditops::dXRotAng, dlgproceditops::dYRotAng, dlgproceditops::dZRotAng));
}

CTMat dlgproceditops::GetInvertedRotTrnMat()
{
	CTMat tm(dlgproceditops::iRotOrd, dlgproceditops::GetRotAng());
	tm.Inverse();
	return (tm);
}

CTMat dlgproceditops::GetRotTrnMat()
{
	return (CTMat(dlgproceditops::iRotOrd, dlgproceditops::GetRotAng()));
}

CTMat dlgproceditops::GetVernierInvertedRotTrnMat()
{
	CVec vec = (GetRotAng() * (1.0+dVernierRot)) - GetRotAng();
	CTMat tm(dlgproceditops::iRotOrd, vec);
	tm.Inverse();
	return (tm);
}

CTMat dlgproceditops::GetVernierRotTrnMat()
{
	CVec vec = (GetRotAng() * (1.0+dVernierRot)) - GetRotAng();
	return (CTMat(dlgproceditops::iRotOrd, vec));
}

CVec dlgproceditops::GetScale()
{
	return (CVec(dlgproceditops::dXScale, dlgproceditops::dYScale, dlgproceditops::dZScale));
}

CVec dlgproceditops::GetInvertedScale()
{
	double dX = fabs(dlgproceditops::dXScale) > DBL_EPSILON ? 1. / dlgproceditops::dXScale : 1.;
	double dY = fabs(dlgproceditops::dYScale) > DBL_EPSILON ? 1. / dlgproceditops::dYScale : 1.;
	double dZ = fabs(dlgproceditops::dZScale) > DBL_EPSILON ? 1. / dlgproceditops::dZScale : 1.;
	
	return (CVec(dX, dY, dZ));
}

CVec dlgproceditops::GetVernierInvertedScale()
{
	return (CVec(1.0-(dXScale * dVernierSize), 1.0-(dYScale * dVernierSize), 1.0-(dZScale * dVernierSize)));
}

CVec dlgproceditops::GetVernierScale()
{
	return (CVec(1.0+(dXScale * dVernierSize), 1.0+(dYScale * dVernierSize), 1.0+(dZScale * dVernierSize)));
}

void dlgproceditops::SetMirrorScale(double dX, double dY, double dZ) 
{
	dlgproceditops::dXMirrorScale = dX;
	dlgproceditops::dYMirrorScale = dY;
	dlgproceditops::dZMirrorScale = dZ;
}

void dlgproceditops::SetRotAng(double dX, double dY, double dZ) 
{
	dlgproceditops::dXRotAng = dX;
	dlgproceditops::dYRotAng = dY;
	dlgproceditops::dZRotAng = dZ;
}

void dlgproceditops::SetRotOrd(int i0, int i1, int i2)
{
	dlgproceditops::iRotOrd[0] = i0;
	dlgproceditops::iRotOrd[1] = i1;
	dlgproceditops::iRotOrd[2] = i2;
}

void dlgproceditops::SetScale(double dX, double dY, double dZ) 
{
	dlgproceditops::dXScale = dX;
	dlgproceditops::dYScale = dY;
	dlgproceditops::dZScale = dZ;
}