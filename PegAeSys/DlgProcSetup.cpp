#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "DlgProcSetup.h"

void InitWindowText(HWND hDlg, CString strText)
{
	CString s3 = strText + " - Profile: ";

	switch(app.ProfileGetState())
	{
		case PROF_NEW:		s3.Append("New");		break;
		case PROF_CUSTOM1:	s3.Append("Custom1");	break;
		case PROF_CUSTOM2:	s3.Append("Custom2");	break;
		case PROF_CLASSIC:	s3.Append("Classic");	break;
	}

	::SetWindowText(hDlg, s3);
}

void InitGeneral(HWND hDlg)
{
	HTREEITEM hParent = DlgProcOptionsTreeParent(hDlg, "General", CPegDoc::TAB_OPEN_PROFILES);
	
	HTREEITEM hChild = DlgProcOptionsTreeChild(hDlg, hParent, "Profiles", CPegDoc::TAB_OPEN_PROFILES);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_PROFILES);

	hChild = DlgProcOptionsTreeChild(hDlg, hParent, "Appearance", CPegDoc::TAB_OPEN_APPEARANCE);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_APPEARANCE);
}

void InitOptions(HWND hDlg)
{
	HTREEITEM hParent = DlgProcOptionsTreeParent(hDlg, "Options", CPegDoc::TAB_OPEN_DIMENSION);
		
	HTREEITEM hChild = DlgProcOptionsTreeChild(hDlg, hParent, "Dimension", CPegDoc::TAB_OPEN_DIMENSION);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_DIMENSION);

	hChild = DlgProcOptionsTreeChild(hDlg, hParent, "Hatch", CPegDoc::TAB_OPEN_HATCH);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_HATCH);

	hChild = DlgProcOptionsTreeChild(hDlg, hParent, "Constraints", CPegDoc::TAB_OPEN_CONSTRAINTS);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_CONSTRAINTS);

	hChild = DlgProcOptionsTreeChild(hDlg, hParent, "User Axis", CPegDoc::TAB_OPEN_USERAXIS);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_USERAXIS);

	hChild = DlgProcOptionsTreeChild(hDlg, hParent, "Scale", CPegDoc::TAB_OPEN_SCALE);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_SCALE);

	hChild = DlgProcOptionsTreeChild(hDlg, hParent, "Units", CPegDoc::TAB_OPEN_UNITS);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_UNITS);

	hChild = DlgProcOptionsTreeChild(hDlg, hParent, "View", CPegDoc::TAB_OPEN_VIEW);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_VIEW);

	hChild = DlgProcOptionsTreeChild(hDlg, hParent, "Mouse", CPegDoc::TAB_OPEN_MOUSE);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_MOUSE);

	hChild = DlgProcOptionsTreeChild(hDlg, hParent, "Scrollbars", CPegDoc::TAB_OPEN_SCROLL);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_SCROLL);

	hChild = DlgProcOptionsTreeChild(hDlg, hParent, "Registry", CPegDoc::TAB_OPEN_REGISTRY);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_REGISTRY);

	hChild = DlgProcOptionsTreeChild(hDlg, hParent, "Note", CPegDoc::TAB_OPEN_NOTE);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_NOTE);
}

void InitTasks(HWND hDlg)
{
	HTREEITEM hParent = DlgProcOptionsTreeParent(hDlg, "Tasks", CPegDoc::TAB_OPEN_PEN);
	
	HTREEITEM hChild = DlgProcOptionsTreeChild(hDlg, hParent, "Pen", CPegDoc::TAB_OPEN_PEN);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_PEN);

	hChild = DlgProcOptionsTreeChild(hDlg, hParent, "Drawing", CPegDoc::TAB_OPEN_DRAW);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_DRAW);

	hChild = DlgProcOptionsTreeChild(hDlg, hParent, "Edit", CPegDoc::TAB_OPEN_EDIT);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_EDIT);

	hChild = DlgProcOptionsTreeChild(hDlg, hParent, "Annotate", CPegDoc::TAB_OPEN_ANNOTATE);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_ANNOTATE);

	hChild = DlgProcOptionsTreeChild(hDlg, hParent, "Dimension", CPegDoc::TAB_OPEN_SETUPDIMENSION);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_SETUPDIMENSION);

	hChild = DlgProcOptionsTreeChild(hDlg, hParent, "Fixup", CPegDoc::TAB_OPEN_FIXUP);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_FIXUP);

	hChild = DlgProcOptionsTreeChild(hDlg, hParent, "Draw2", CPegDoc::TAB_OPEN_DRAW2);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_DRAW2);

	hChild = DlgProcOptionsTreeChild(hDlg, hParent, "LPD", CPegDoc::TAB_OPEN_LPD);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_LPD);

	hChild = DlgProcOptionsTreeChild(hDlg, hParent, "RLPD", CPegDoc::TAB_OPEN_RLPD);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_RLPD);

	hChild = DlgProcOptionsTreeChild(hDlg, hParent, "Pipe", CPegDoc::TAB_OPEN_PIPE);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_PIPE);
}

void InitProjections(HWND hDlg)
{
	HTREEITEM hParent = DlgProcOptionsTreeParent(hDlg, "Projections", CPegDoc::TAB_OPEN_ISOMETRIC);
	
	HTREEITEM hChild = DlgProcOptionsTreeChild(hDlg, hParent, "Isometric", CPegDoc::TAB_OPEN_ISOMETRIC);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_ISOMETRIC);

	hChild = DlgProcOptionsTreeChild(hDlg, hParent, "Axonometric", CPegDoc::TAB_OPEN_AXONOMETRIC);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_AXONOMETRIC);

	hChild = DlgProcOptionsTreeChild(hDlg, hParent, "Oblique", CPegDoc::TAB_OPEN_OBLIQUE);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_OBLIQUE);

	hChild = DlgProcOptionsTreeChild(hDlg, hParent, "Perspective", CPegDoc::TAB_OPEN_PERSPECTIVE);
	HighlightItem(hDlg, hChild, CPegDoc::TAB_OPEN_PERSPECTIVE);
}

void HighlightItem(HWND hDlg, HTREEITEM hChild, WORD wDocPage)
{
	if(CPegDoc::GetDoc()->GetOptionsPage() == wDocPage)
		TreeView_SelectItem(::GetDlgItem(hDlg, IDC_TREE2), hChild);
}

void OkGeneral(HWND hDlg)
{
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_GENERAL_PROFILES), WM_DIALOG_OK, 0, 0);
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_GENERAL_APPEARANCE), WM_DIALOG_OK, 0, 0);
}

void OkOptions(HWND hDlg)
{
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_OPTIONS_TAB_DIMENSION), WM_DIALOG_OK, 0, 0);
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_OPTIONS_TAB_HATCH), WM_DIALOG_OK, 0, 0);
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_OPTIONS_TAB_CONSTRAINTS), WM_DIALOG_OK, 0, 0);
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_OPTIONS_TAB_USERAXIS), WM_DIALOG_OK, 0, 0);
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_OPTIONS_TAB_SCALE), WM_DIALOG_OK, 0, 0);
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_OPTIONS_TAB_UNITS), WM_DIALOG_OK, 0, 0);
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_OPTIONS_TAB_VIEW), WM_DIALOG_OK, 0, 0);
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_OPTIONS_TAB_MOUSE), WM_DIALOG_OK, 0, 0);
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_OPTIONS_TAB_SCROLLBARS), WM_DIALOG_OK, 0, 0);
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_OPTIONS_TAB_REGISTRY), WM_DIALOG_OK, 0, 0);
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_OPTIONS_TAB_NOTE), WM_DIALOG_OK, 0, 0);
}

void OkTasks(HWND hDlg)
{
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_SETUP_TAB_ANNOTATE), WM_DIALOG_OK, 0, 0);
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_SETUP_TAB_DRAW), WM_DIALOG_OK, 0, 0);
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_SETUP_TAB_DRAW2), WM_DIALOG_OK, 0, 0);
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_SETUP_TAB_EDIT), WM_DIALOG_OK, 0, 0);
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_SETUP_TAB_FIXUP), WM_DIALOG_OK, 0, 0);
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_SETUP_TAB_LPD), WM_DIALOG_OK, 0, 0);
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_SETUP_TAB_PEN), WM_DIALOG_OK, 0, 0);
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_SETUP_TAB_PIPE), WM_DIALOG_OK, 0, 0);
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_SETUP_TAB_RLPD), WM_DIALOG_OK, 0, 0);
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_SETUP_TAB_DIMENSION), WM_DIALOG_OK, 0, 0);
}

void OkProjections(HWND hDlg)
{
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_PROJ_AXONOMETRIC2), WM_DIALOG_OK, 0, 0);
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_PROJ_ISOMETRIC2), WM_DIALOG_OK, 0, 0);
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_PROJ_OBLIQUE2), WM_DIALOG_OK, 0, 0);
	SendNotifyMessage(::GetDlgItem(hDlg, IDD_PROJ_PERSPECTIVE2), WM_DIALOG_OK, 0, 0);
}

HTREEITEM DlgProcOptionsTreeParent(HWND hDlg, char *szText, int lParam)
{ 
	TVITEM item;
	item.mask = TVIF_TEXT | TVIF_STATE | TVIF_PARAM;
	item.state = TVIS_EXPANDED;
	item.stateMask = TVIS_EXPANDED;
	item.pszText = szText;
	item.lParam = lParam;

	TVINSERTSTRUCT insert;
	insert.hParent = NULL;
	insert.hInsertAfter = TVI_LAST;
	insert.item = item;

	LRESULT lResult = SendMessage(::GetDlgItem(hDlg, IDC_TREE2), (UINT) TVM_INSERTITEM, 0, (LPARAM)&insert);

	return (HTREEITEM)lResult; 
}

HTREEITEM DlgProcOptionsTreeChild(HWND hDlg, HTREEITEM hParent, char *szText, int lParam)
{
	TVITEM item;
	item.mask = TVIF_TEXT | TVIF_STATE | TVIF_PARAM;
	item.state = TVIS_EXPANDED;
	item.stateMask = TVIS_EXPANDED;
	item.pszText = szText;
	item.lParam = lParam;

	TVINSERTSTRUCT insert;
	insert.hInsertAfter = TVI_LAST;
	insert.hParent = hParent;
	insert.item = item;

	LRESULT lResult = SendMessage(::GetDlgItem(hDlg, IDC_TREE2), (UINT) TVM_INSERTITEM, 0, (LPARAM)&insert);

	return (HTREEITEM)lResult; 
}

void SetPage(HWND hwndmain, HWND& hwndpage, int nId, DLGPROC dlgProc)
{
	if(!hwndpage)
	{
		hwndpage = CreateDialog((HINSTANCE)GetWindowLong(hwndmain,GWL_HINSTANCE),MAKEINTRESOURCE(nId),hwndmain,dlgProc);
		SetWindowLong(hwndpage,GWL_ID,nId);
	}

	// set window position and focus
	SetWindowPos(hwndpage,0,0,0,0,0,SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	// set focus
	SendNotifyMessage(hwndpage, WM_DIALOG_FOCUS, 0, 0);
}

void DlgProcSetupSetPage(HWND hwnd, int page)
{
	HWND hZero	= GetDlgItem(hwnd, IDD_OPTIONS_TAB_DIMENSION);
	HWND hOne	= GetDlgItem(hwnd, IDD_OPTIONS_TAB_HATCH);
	HWND hTwo	= GetDlgItem(hwnd, IDD_OPTIONS_TAB_CONSTRAINTS);
	HWND hThree = GetDlgItem(hwnd, IDD_OPTIONS_TAB_USERAXIS);
	HWND hFour	= GetDlgItem(hwnd, IDD_OPTIONS_TAB_SCALE);
	HWND hFive	= GetDlgItem(hwnd, IDD_OPTIONS_TAB_UNITS);
	HWND hSix	= GetDlgItem(hwnd, IDD_OPTIONS_TAB_VIEW);
	HWND hSeven	= GetDlgItem(hwnd, IDD_OPTIONS_TAB_MOUSE);
	HWND hEight	= GetDlgItem(hwnd, IDD_OPTIONS_TAB_SCROLLBARS);
	HWND hNine	= GetDlgItem(hwnd, IDD_OPTIONS_TAB_REGISTRY);
	HWND hNote	= GetDlgItem(hwnd, IDD_OPTIONS_TAB_NOTE);

	HWND hPen		= GetDlgItem(hwnd, IDD_SETUP_TAB_PEN);
	HWND hDraw		= GetDlgItem(hwnd, IDD_SETUP_TAB_DRAW);
	HWND hLPD		= GetDlgItem(hwnd, IDD_SETUP_TAB_LPD);
	HWND hRLPD		= GetDlgItem(hwnd, IDD_SETUP_TAB_RLPD);
	HWND hPipe		= GetDlgItem(hwnd, IDD_SETUP_TAB_PIPE);
	HWND hDraw2		= GetDlgItem(hwnd, IDD_SETUP_TAB_DRAW2);
	HWND hAnnotate	= GetDlgItem(hwnd, IDD_SETUP_TAB_ANNOTATE);
	HWND hEdit		= GetDlgItem(hwnd, IDD_SETUP_TAB_EDIT);
	HWND hFixup		= GetDlgItem(hwnd, IDD_SETUP_TAB_FIXUP);
	HWND hDimension = GetDlgItem(hwnd, IDD_SETUP_TAB_DIMENSION);

	HWND hAppearance = GetDlgItem(hwnd, IDD_GENERAL_APPEARANCE);
	HWND hProfiles	= GetDlgItem(hwnd, IDD_GENERAL_PROFILES);

	HWND hIso		= GetDlgItem(hwnd, IDD_PROJ_ISOMETRIC2);
	HWND hAxo		= GetDlgItem(hwnd, IDD_PROJ_AXONOMETRIC2);
	HWND hOblique	= GetDlgItem(hwnd, IDD_PROJ_OBLIQUE2);
	HWND hPersp		= GetDlgItem(hwnd, IDD_PROJ_PERSPECTIVE2);

	if(page == CPegDoc::TAB_OPEN_DIMENSION)
		SetPage(hwnd, hZero, IDD_OPTIONS_TAB_DIMENSION, DlgProcOptionsTabDimension);
	else if(hZero)
		SetWindowPos(hZero,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	if(page == CPegDoc::TAB_OPEN_HATCH)
		SetPage(hwnd, hOne, IDD_OPTIONS_TAB_HATCH, DlgProcOptionsTabHatch);
	else if(hOne)
		SetWindowPos(hOne,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	if(page == CPegDoc::TAB_OPEN_CONSTRAINTS)
		SetPage(hwnd, hTwo, IDD_OPTIONS_TAB_CONSTRAINTS, DlgProcOptionsTabConstraints);
	else if(hTwo)
		SetWindowPos(hTwo,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	if(page == CPegDoc::TAB_OPEN_USERAXIS)
		SetPage(hwnd, hThree, IDD_OPTIONS_TAB_USERAXIS, DlgProcOptionsTabUserAxis);
	else if(hThree)
		SetWindowPos(hThree,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	if(page == CPegDoc::TAB_OPEN_SCALE)
		SetPage(hwnd, hFour, IDD_OPTIONS_TAB_SCALE, DlgProcOptionsTabScale);
	else if(hFour)
		SetWindowPos(hFour,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	if(page == CPegDoc::TAB_OPEN_UNITS)
		SetPage(hwnd, hFive, IDD_OPTIONS_TAB_UNITS, DlgProcOptionsTabUnits);
	else if(hFive)
		SetWindowPos(hFive,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	if(page == CPegDoc::TAB_OPEN_VIEW)
		SetPage(hwnd, hSix, IDD_OPTIONS_TAB_VIEW, DlgProcOptionsTabView);
	else if(hSix)
		SetWindowPos(hSix,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	if(page == CPegDoc::TAB_OPEN_MOUSE)
		SetPage(hwnd, hSeven, IDD_OPTIONS_TAB_MOUSE, DlgProcOptionsTabMouse);
	else if(hSeven)
		SetWindowPos(hSeven,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	if(page == CPegDoc::TAB_OPEN_SCROLL)
		SetPage(hwnd, hEight, IDD_OPTIONS_TAB_SCROLLBARS, DlgProcOptionsTabScrollbars);
	else if(hEight)
		SetWindowPos(hEight,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	if(page == CPegDoc::TAB_OPEN_REGISTRY)
		SetPage(hwnd, hNine, IDD_OPTIONS_TAB_REGISTRY, DlgProcOptionsTabRegistry);
	else if(hNine)
		SetWindowPos(hNine,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	if(page == CPegDoc::TAB_OPEN_PEN)
		SetPage(hwnd, hPen, IDD_SETUP_TAB_PEN, DlgProcSetupTabPen);
	else if(hPen)
		SetWindowPos(hPen,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	if(page == CPegDoc::TAB_OPEN_DRAW)
		SetPage(hwnd, hDraw, IDD_SETUP_TAB_DRAW, DlgProcSetupTabDraw);
	else if(hDraw)
		SetWindowPos(hDraw,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	if(page == CPegDoc::TAB_OPEN_LPD)
		SetPage(hwnd, hLPD, IDD_SETUP_TAB_LPD, DlgProcSetupTabLPD);
	else if(hLPD)
		SetWindowPos(hLPD,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	if(page == CPegDoc::TAB_OPEN_RLPD)
		SetPage(hwnd, hRLPD, IDD_SETUP_TAB_RLPD, DlgProcSetupTabRLPD);
	else if(hRLPD)
		SetWindowPos(hRLPD,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	if(page == CPegDoc::TAB_OPEN_PIPE)
		SetPage(hwnd, hPipe, IDD_SETUP_TAB_PIPE, DlgProcSetupTabPipe);
	else if(hPipe)
		SetWindowPos(hPipe,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	if(page == CPegDoc::TAB_OPEN_DRAW2)
		SetPage(hwnd, hDraw2, IDD_SETUP_TAB_DRAW2, DlgProcSetupTabDraw2);
	else if(hDraw2)
		SetWindowPos(hDraw2,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	if(page == CPegDoc::TAB_OPEN_ANNOTATE)
		SetPage(hwnd, hAnnotate, IDD_SETUP_TAB_ANNOTATE, DlgProcSetupTabAnnotate);
	else if(hAnnotate)
		SetWindowPos(hAnnotate,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	if(page == CPegDoc::TAB_OPEN_SETUPDIMENSION)
		SetPage(hwnd, hDimension, IDD_SETUP_TAB_DIMENSION, DlgProcSetupTabDimension);
	else if(hDimension)
		SetWindowPos(hDimension,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	if(page == CPegDoc::TAB_OPEN_EDIT)
		SetPage(hwnd, hEdit, IDD_SETUP_TAB_EDIT, DlgProcSetupTabEdit);
	else if(hEdit)
		SetWindowPos(hEdit,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	if(page == CPegDoc::TAB_OPEN_FIXUP)
		SetPage(hwnd, hFixup, IDD_SETUP_TAB_FIXUP, DlgProcSetupTabFixup);
	else if(hFixup)
		SetWindowPos(hFixup,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	if(page == CPegDoc::TAB_OPEN_APPEARANCE)//......
		SetPage(hwnd, hAppearance, IDD_GENERAL_APPEARANCE, DlgProcAppearance);
	else if(hAppearance)
		SetWindowPos(hAppearance,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	if(page == CPegDoc::TAB_OPEN_PROFILES)
		SetPage(hwnd, hProfiles, IDD_GENERAL_PROFILES, DlgProcProfile);
	else if(hProfiles)
		SetWindowPos(hProfiles,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	if(page == CPegDoc::TAB_OPEN_NOTE)
		SetPage(hwnd, hNote, IDD_OPTIONS_TAB_NOTE, DlgProcOptionsTabNote);
	else if(hNote)
		SetWindowPos(hNote,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	// Isometric Projection
	if(page == CPegDoc::TAB_OPEN_ISOMETRIC)
		SetPage(hwnd, hIso, IDD_PROJ_ISOMETRIC2, DlgProcProjIsometric);
	else if(hIso)
		SetWindowPos(hIso,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);
	
	// Axonometric Projection
	if(page == CPegDoc::TAB_OPEN_AXONOMETRIC)
		SetPage(hwnd, hAxo, IDD_PROJ_AXONOMETRIC2, DlgProcProjAxonometric);
	else if(hAxo)
		SetWindowPos(hAxo,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	// Oblique Projection
	if(page == CPegDoc::TAB_OPEN_OBLIQUE)
		SetPage(hwnd, hOblique, IDD_PROJ_OBLIQUE2, DlgProcProjOblique);
	else if(hOblique)
		SetWindowPos(hOblique,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	// Perspective Projection
	if(page == CPegDoc::TAB_OPEN_PERSPECTIVE)
		SetPage(hwnd, hPersp, IDD_PROJ_PERSPECTIVE2, DlgProcProjPerspective);
	else if(hPersp)
		SetWindowPos(hPersp,0,0,0,0,0,SWP_HIDEWINDOW|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);
}