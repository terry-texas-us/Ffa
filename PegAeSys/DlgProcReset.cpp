#include "stdafx.h"
//
//#include "PegAEsys.h"
//#include "PegAEsysDoc.h"
//#include "PegAEsysView.h"
//
//#include "dde.h"
//#include "ddeGItms.h"
//
//#include "UnitsString.h"
//#include "UserAxis.h"
//
//#include "SubProcLPD.h"
//#include "SubProcRLPD.h"
//#include "SubProcPipe.h"
//#include "SubProcAnnotate.h"
//#include "SubProcFixup.h"
//
//#include "DlgProcEditOps.h"
//
//#include "Hatch.h"
//#include "scroll.h"
//#include "Grid.h"
//
//void DlgProcResetOk(HWND hDlg);
//void DlgProcResetAll();
//
//// tasks
//void DlgProcReset_Draw();
//void DlgProcReset_Draw2();
//void DlgProcReset_Annotate();
//void DlgProcReset_Pens();
//void DlgProcReset_Pipe();
//void DlgProcReset_Fixup();
//void DlgProcReset_LPD();
//void DlgProcReset_RLPD();
//void DlgProcReset_Edit();
//
//// options
//void DlgProcReset_Import();
//void DlgProcReset_Note();
//void DlgProcReset_Units();
//void DlgProcReset_Scale();
//void DlgProcReset_View();
//void DlgProcReset_Hatch();
//void DlgProcReset_Mouse();
//void DlgProcReset_Const();
//void DlgProcReset_Dim();
//void DlgProcReset_Scroll();
//void DlgProcReset_Page();
//
//// other
//void DlgProcReset_Zoom();
//void DlgProcReset_Colors();
//void DlgProcReset_Mode();
//void DlgProcReset_Proj();
//
//// draw 2
////extern double DlgProcDraw2_dDistance;
//
//// mouse
//extern char	szLeftMouseDown[60];
//extern char	szRightMouseDown[60]; 
//extern char	szLeftMouseUp[60];
//extern char	szRightMouseUp[60];
//
//// constraints - axis
//extern double UserAxis_dInfAng2;
//extern double UserAxis_dAxOffAng2;
//
//// dimension 
//extern double DlgProcDistance_dDistance;
//
//BOOL CALLBACK DlgProcReset(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
//{
//	switch(nMsg)
//	{
//		case WM_INITDIALOG:
//		{
//			return (TRUE);
//		}
//		case WM_COMMAND:
//		{
//			UINT bState;
//
//			switch (LOWORD(wParam)) 
//			{
//				case IDC_TASKS_ALL:
//				{					
//					bState = ::IsDlgButtonChecked(hDlg, IDC_TASKS_ALL);
//					if(bState == BST_CHECKED)
//					{
//						::CheckDlgButton(hDlg, IDC_TASK_DRAW, BST_CHECKED);
//						::CheckDlgButton(hDlg, IDC_TASK_DRAW2, BST_CHECKED);
//						::CheckDlgButton(hDlg, IDC_TASK_ANNOTATE, BST_CHECKED);
//						::CheckDlgButton(hDlg, IDC_TASK_PENS, BST_CHECKED);
//						::CheckDlgButton(hDlg, IDC_TASK_PIPE, BST_CHECKED);
//						::CheckDlgButton(hDlg, IDC_TASK_FIXUP, BST_CHECKED);
//						::CheckDlgButton(hDlg, IDC_TASK_EDIT, BST_CHECKED);
//						::CheckDlgButton(hDlg, IDC_TASK_LPD, BST_CHECKED);
//						::CheckDlgButton(hDlg, IDC_TASK_RLPD, BST_CHECKED);
//					}
//					else if(bState == BST_UNCHECKED)
//					{
//						::CheckDlgButton(hDlg, IDC_TASK_DRAW, BST_UNCHECKED);
//						::CheckDlgButton(hDlg, IDC_TASK_DRAW2, BST_UNCHECKED);
//						::CheckDlgButton(hDlg, IDC_TASK_ANNOTATE, BST_UNCHECKED);
//						::CheckDlgButton(hDlg, IDC_TASK_PENS, BST_UNCHECKED);
//						::CheckDlgButton(hDlg, IDC_TASK_PIPE, BST_UNCHECKED);
//						::CheckDlgButton(hDlg, IDC_TASK_FIXUP, BST_UNCHECKED);
//						::CheckDlgButton(hDlg, IDC_TASK_EDIT, BST_UNCHECKED);
//						::CheckDlgButton(hDlg, IDC_TASK_LPD, BST_UNCHECKED);
//						::CheckDlgButton(hDlg, IDC_TASK_RLPD, BST_UNCHECKED);
//					}
//
//						break;
//				}
//				case IDC_OPTIONS_ALL:
//				{					
//					bState = ::IsDlgButtonChecked(hDlg, IDC_OPTIONS_ALL);
//					if(bState == BST_CHECKED)
//					{
//						::CheckDlgButton(hDlg, IDC_OPT_IMPORT, BST_CHECKED);
//						::CheckDlgButton(hDlg, IDC_OPT_NOTE, BST_CHECKED);
//						::CheckDlgButton(hDlg, IDC_OPT_UNITS, BST_CHECKED);
//						::CheckDlgButton(hDlg, IDC_OPT_SCALE, BST_CHECKED);
//						::CheckDlgButton(hDlg, IDC_OPT_VIEW, BST_CHECKED);
//						::CheckDlgButton(hDlg, IDC_OPT_HATCH, BST_CHECKED);
//						::CheckDlgButton(hDlg, IDC_OPT_MOUSE, BST_CHECKED);
//						::CheckDlgButton(hDlg, IDC_OPT_CONST, BST_CHECKED);
//						::CheckDlgButton(hDlg, IDC_OPT_DIM, BST_CHECKED);
//						::CheckDlgButton(hDlg, IDC_OPT_SCROLL, BST_CHECKED);
//						::CheckDlgButton(hDlg, IDC_OPT_PAGE, BST_CHECKED);
//					}
//					else if(bState == BST_UNCHECKED)
//					{
//						::CheckDlgButton(hDlg, IDC_OPT_IMPORT, BST_UNCHECKED);
//						::CheckDlgButton(hDlg, IDC_OPT_NOTE, BST_UNCHECKED);
//						::CheckDlgButton(hDlg, IDC_OPT_UNITS, BST_UNCHECKED);
//						::CheckDlgButton(hDlg, IDC_OPT_SCALE, BST_UNCHECKED);
//						::CheckDlgButton(hDlg, IDC_OPT_VIEW, BST_UNCHECKED);
//						::CheckDlgButton(hDlg, IDC_OPT_HATCH, BST_UNCHECKED);
//						::CheckDlgButton(hDlg, IDC_OPT_MOUSE, BST_UNCHECKED);
//						::CheckDlgButton(hDlg, IDC_OPT_CONST, BST_UNCHECKED);
//						::CheckDlgButton(hDlg, IDC_OPT_DIM, BST_UNCHECKED);
//						::CheckDlgButton(hDlg, IDC_OPT_SCROLL, BST_UNCHECKED);
//						::CheckDlgButton(hDlg, IDC_OPT_PAGE, BST_UNCHECKED);
//					}
//
//						break;
//				}
//				case IDC_OTHER_ALL:
//				{					
//					bState = ::IsDlgButtonChecked(hDlg, IDC_OTHER_ALL);
//					if(bState == BST_CHECKED)
//					{
//						::CheckDlgButton(hDlg, IDC_OTHER_ZOOM, BST_CHECKED);
//						::CheckDlgButton(hDlg, IDC_OTHER_COLORS, BST_CHECKED);
//						::CheckDlgButton(hDlg, IDC_OTHER_MODE, BST_CHECKED);
//						::CheckDlgButton(hDlg, IDC_OTHER_PROJ, BST_CHECKED);
//					}
//					else if(bState == BST_UNCHECKED)
//					{
//						::CheckDlgButton(hDlg, IDC_OTHER_ZOOM, BST_UNCHECKED);
//						::CheckDlgButton(hDlg, IDC_OTHER_COLORS, BST_UNCHECKED);
//						::CheckDlgButton(hDlg, IDC_OTHER_MODE, BST_UNCHECKED);
//						::CheckDlgButton(hDlg, IDC_OTHER_PROJ, BST_UNCHECKED);
//					}
//
//						break;
//				}
//				case IDOK:
//				{
//					DlgProcResetOk(hDlg);
//					::EndDialog(hDlg, TRUE);
//					return (TRUE);
//				}
//				case IDCANCEL:
//				{
//					::EndDialog(hDlg, IDCANCEL);
//					return (FALSE);
//				}
//			}
//			break;
//		}
//	}
//	return (FALSE); 
//}
//
//void DlgProcResetOk(HWND hDlg)
//{
//	UINT bState;
//
//	HCURSOR hOldCursor = ::SetCursor(LoadCursor(0, IDC_WAIT));
//
//	//////////////////////////////////////////////////////////////////////
//	// tasks
//	//////////////////////////////////////////////////////////////////////
//	bState = ::IsDlgButtonChecked(hDlg, IDC_TASK_DRAW);
//	if(bState == BST_CHECKED) 
//		DlgProcReset_Draw();
//
//	bState = ::IsDlgButtonChecked(hDlg, IDC_TASK_PENS);
//	if(bState == BST_CHECKED)
//		DlgProcReset_Pens();
//
//	bState = ::IsDlgButtonChecked(hDlg, IDC_TASK_DRAW2);
//	if(bState == BST_CHECKED) 
//		DlgProcReset_Draw2();
//
//	bState = ::IsDlgButtonChecked(hDlg, IDC_TASK_PIPE);
//	if(bState == BST_CHECKED)
//		DlgProcReset_Pipe();
//
//	bState = ::IsDlgButtonChecked(hDlg, IDC_TASK_LPD);
//	if(bState == BST_CHECKED) 
//		DlgProcReset_LPD();
//
//	bState = ::IsDlgButtonChecked(hDlg, IDC_TASK_RLPD);
//	if(bState == BST_CHECKED)
//		DlgProcReset_RLPD();
//
//	bState = ::IsDlgButtonChecked(hDlg, IDC_TASK_EDIT);
//	if(bState == BST_CHECKED) 
//		DlgProcReset_Edit();
//
//	bState = ::IsDlgButtonChecked(hDlg, IDC_TASK_FIXUP);
//	if(bState == BST_CHECKED)
//		DlgProcReset_Fixup();
//
//	bState = ::IsDlgButtonChecked(hDlg, IDC_TASK_ANNOTATE);
//	if(bState == BST_CHECKED) 
//		DlgProcReset_Annotate();
//	//////////////////////////////////////////////////////////////////////
//
//	//////////////////////////////////////////////////////////////////////
//	// options
//	//////////////////////////////////////////////////////////////////////
//	bState = ::IsDlgButtonChecked(hDlg, IDC_OPT_IMPORT);
//	if(bState == BST_CHECKED) 
//		DlgProcReset_Import();
//
//	bState = ::IsDlgButtonChecked(hDlg, IDC_OPT_NOTE);
//	if(bState == BST_CHECKED) 
//		DlgProcReset_Note();
//
//	bState = ::IsDlgButtonChecked(hDlg, IDC_OPT_UNITS);
//	if(bState == BST_CHECKED) 
//		DlgProcReset_Units();
//
//	bState = ::IsDlgButtonChecked(hDlg, IDC_OPT_SCALE);
//	if(bState == BST_CHECKED) 
//		DlgProcReset_Scale();
//
//	bState = ::IsDlgButtonChecked(hDlg, IDC_OPT_VIEW);
//	if(bState == BST_CHECKED) 
//		DlgProcReset_View();
//
//	bState = ::IsDlgButtonChecked(hDlg, IDC_OPT_HATCH);
//	if(bState == BST_CHECKED) 
//		DlgProcReset_Hatch();
//
//	bState = ::IsDlgButtonChecked(hDlg, IDC_OPT_MOUSE);
//	if(bState == BST_CHECKED) 
//		DlgProcReset_Mouse();
//
//	bState = ::IsDlgButtonChecked(hDlg, IDC_OPT_CONST);
//	if(bState == BST_CHECKED) 
//		DlgProcReset_Const();
//
//	bState = ::IsDlgButtonChecked(hDlg, IDC_OPT_DIM);
//	if(bState == BST_CHECKED) 
//		DlgProcReset_Dim();
//
//	bState = ::IsDlgButtonChecked(hDlg, IDC_OPT_SCROLL);
//	if(bState == BST_CHECKED) 
//		DlgProcReset_Scroll();
//
//	bState = ::IsDlgButtonChecked(hDlg, IDC_OPT_PAGE);
//	if(bState == BST_CHECKED) 
//		DlgProcReset_Page();
//	//////////////////////////////////////////////////////////////////////
//
//	//////////////////////////////////////////////////////////////////////
//	// other
//	//////////////////////////////////////////////////////////////////////
//	bState = ::IsDlgButtonChecked(hDlg, IDC_OTHER_ZOOM);
//	if(bState == BST_CHECKED) 
//		DlgProcReset_Zoom();
//
//	bState = ::IsDlgButtonChecked(hDlg, IDC_OTHER_PROJ);
//	if(bState == BST_CHECKED) 
//		DlgProcReset_Proj();
//
//	bState = ::IsDlgButtonChecked(hDlg, IDC_OTHER_COLORS);
//	if(bState == BST_CHECKED) 
//		DlgProcReset_Colors();
//
//	bState = ::IsDlgButtonChecked(hDlg, IDC_OTHER_MODE);
//	if(bState == BST_CHECKED) 
//		DlgProcReset_Mode();
//	//////////////////////////////////////////////////////////////////////
//
//	::SetCursor(hOldCursor);
//}
//
//// tasks
//void DlgProcReset_Draw()
//{
//	// scale
//	app.SetScale(96);
//	dde::PostAdvise(dde::ScaleInfo);
//	app.StatusLineDisplay(Scale);
//}
//
//void DlgProcReset_Draw2()
//{
//	// width - 1'
////	DlgProcDraw2_dDistance = 1.0/8.0;
//}
//
//void DlgProcReset_Annotate()
//{
//	//strcpy(annotate::szDefTxt, "");
//	//annotate::dGapSpaceFac = .5;
//	//annotate::dHookRad = .03125;
//
//	//annotate::iArrowTyp = 1;
//
//	//annotate::dArrowSiz = .1;
//	//annotate::dBubRad = .125;
//	//annotate::wBubFacets = 0;
//}
//
//void DlgProcReset_Pens()
//{
//	// pen color
//	pstate.SetPenColor(1);
//
//	// pen style
//	pstate.SetPenStyle(1);
//}
//
//void DlgProcReset_Pipe()
//{
//	pipe::dTicSize = .03125;
//	pipe::dCSize = .03125;
//}
//
//void DlgProcReset_Fixup()
//{
//	//fixup::dSiz = .25;
//	//fixup::dAxTol = 2;
//	//UserAxisSetInfAng(fixup::dAxTol);
//}
//
//void DlgProcReset_LPD()
//{
//	lpd::dInsRadElFac = 1.5;
//	lpd::dSecWid = UnitsString_ParseLength(max(app.GetUnits(), Inches), "12\"");
//	lpd::dSecDep = UnitsString_ParseLength(max(app.GetUnits(), Inches), "6\"");
//	lpd::bGenTurnVanes = true;
//	lpd::eElbow = Mittered;
//	lpd::eJust = lpd::Center;
//
//	lpd::dWid[1] = Max(0., lpd::dSecWid);
//	lpd::dDep[1] = Max(0., lpd::dSecDep);
//}
//
//void DlgProcReset_RLPD()
//{
//	rlpd::dInsideRadiusFactor = 1.5;
//	rlpd::dSecDiam = UnitsString_ParseLength(max(app.GetUnits(), Inches), "12\"");
//	rlpd::bGenCenterline = false;
//	rlpd::eJust0 = rlpd::Center0;
//
//	rlpd::dDiam[2]= Max(0., rlpd::dSecDiam);
//}
//
//void DlgProcReset_Edit()
//{
//	//dlgproceditops::SetScale(2, 2, 2);
//	//dlgproceditops::SetRotAng();
//	//dlgproceditops::SetMirrorScale();
//}
//
//
//// options
//void DlgProcReset_Import()
//{
//	istate.SetTranslatePens(true);
//	istate.SetModifyFont(true);
//	istate.SetUnblockInserts(true);
//	istate.SetPurgeBlocks(true);
//	istate.SetScale(true);
//	istate.SetKillLayers(true);
//	istate.SetOmitLayers(true);
//	istate.SetResolveCommands(true);
//	istate.SetHardPens(true);
//	istate.SetHardStyle(true);
//	istate.SetScaleValue(96.0);
//	istate.SetTranslateFile("Default.txt");
//}
//
//void DlgProcReset_Note()
//{
//	CCharCellDef ccd;
//	pstate.GetCharCellDef(ccd);
//
//	ccd.ChrHgtSet(.1);
//	ccd.TextRotAngSet(0);
//	ccd.ChrExpFacSet(1);
//	ccd.ChrSlantAngSet(0);
//	
//	CFontDef fd;
//	pstate.GetFontDef(fd);
//	
//	fd.ChrSpacSet(0);
//	
//	WORD wHorAlign = IDC_TEXT_ALIGN_HOR_LEFT;
//	wHorAlign = WORD(1 - IDC_TEXT_ALIGN_HOR_LEFT + wHorAlign);
//	
//	WORD wVerAlign = IDC_TEXT_ALIGN_VER_BOT;
//	wVerAlign = WORD(4 + IDC_TEXT_ALIGN_VER_BOT - wVerAlign);
//	
//	fd.TextHorAlignSet(wHorAlign);
//	fd.TextVerAlignSet(wVerAlign);
//						
//	WORD wPath = IDC_PATH_RIGHT;
//	wPath = WORD(wPath - IDC_PATH_RIGHT);
//	
//	fd.TextPathSet(wPath);
//
//	fd.TextFontSet("Simplex.psf");	
//	fd.TextPrecSet(CFontDef::PREC_PEGSTROKEFONT);
//
//	CDC *pDC = CPegView::GetActiveView()->GetDC();
//
//	pstate.SetFontDef(pDC, fd);
//	pstate.SetCharCellDef(ccd);
//}
//
//void DlgProcReset_Units()
//{
//	//app.SetUnits(Engineering);
//	//app.SetUnitsPrec(8);
//}
//
//void DlgProcReset_Scale()
//{
//	// scale
//	app.SetScale(96);
//	dde::PostAdvise(dde::ScaleInfo);
//	app.StatusLineDisplay(Scale);
//}
//
//void DlgProcReset_View()
//{
//	double dAngle = 90;
//
//	view::SetNearClipDistance(-1000);
//	view::SetFarClipDistance(1000);
//
//	view::SetVwUp(CVec(cos(dAngle * RADIAN), sin(dAngle * RADIAN), 0.));
//	CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);
//}
//
//void DlgProcReset_Hatch()
//{
//	pstate.SetPolygonIntStyleId(1);
//
//	hatch::dXAxRefVecScal = .1;
//	hatch::dYAxRefVecScal = .1;
//	hatch::dOffAng = Arc(0);
//
//	pstate.SetPolygonIntStyle(POLYGON_HOLLOW);
//}
//
//void DlgProcReset_Mouse()
//{
//	strcpy(szLeftMouseDown, "");
//	strcpy(szRightMouseDown, ""); 
//	strcpy(szLeftMouseUp, "{13}");
//	strcpy(szRightMouseUp, "{27}");
//}
//
//void DlgProcReset_Const()
//{
//	// grid
//	grid::dXSnapInt = UnitsString_ParseLength(app.GetUnits(), "0\'.0125\"");
//	grid::dYSnapInt = UnitsString_ParseLength(app.GetUnits(), "0\'.0125\"");
//	grid::dZSnapInt = UnitsString_ParseLength(app.GetUnits(), "0\'.0125\"");
//	
//	grid::dXDotInt = UnitsString_ParseLength(app.GetUnits(), "8\'0\"");
//	grid::dYDotInt = UnitsString_ParseLength(app.GetUnits(), "8\'0\"");
//	grid::dZDotInt = UnitsString_ParseLength(app.GetUnits(), "8\'0\"");
//	
//	grid::dXLnInt = UnitsString_ParseLength(app.GetUnits(), "32\'0\"");
//	grid::dYLnInt = UnitsString_ParseLength(app.GetUnits(), "32\'0\"");
//	grid::dZLnInt = UnitsString_ParseLength(app.GetUnits(), "32\'0\"");
//	
//	grid::SetSnapState(false);
//	grid::SetDotsState(false);
//	grid::SetLinesState(false);
//
//	// axis
//	UserAxis_dInfAng2 = 0;
//	UserAxis_dAxOffAng2 = 0;
//
//	CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);
//}
//
//void DlgProcReset_Dim()
//{
//	// distance
//	DlgProcDistance_dDistance = UnitsString_ParseLength(app.GetUnits(), "1\'0\"");
//	app.SetDimLen(DlgProcDistance_dDistance);
//	dde::PostAdvise(dde::DimLenInfo);
//	app.StatusLineDisplay(DimLen);
//
//	// angle
//	app.SetDimAngZ(45.0);
//	dde::PostAdvise(dde::DimAngZInfo);
//}
//
//void DlgProcReset_Scroll()
//{
//	scroll::nHScrollThumbSize = 150;						  
//	scroll::nHScrollSens = 3;
//	scroll::nVScrollThumbSize = 200;
//	scroll::nVScrollSens = 4;
//	CPegView::GetActiveView()->UpdateScroll();
//}
//
//void DlgProcReset_Page()
//{
//	CPegView *pView = CPegView::GetActiveView();
//	
//	// F2
//	if(!pView->IsTTFontsOn())
//		pView->OnViewTrueTypeFonts();
//
//	// F3 - state info
//	if(!app.IsStateInfoOn())
//		app.OnViewStateInformation();
//
//	// mode info
//	if(!app.IsModeInfoOn())
//		app.OnViewModeInformation();
//
//	// F4
//	if(pView->IsOdometerOn())
//		pView->OnViewOdometer();
//
//	// F6
//	if(!pView->IsScrollbarsOn())
//		pView->OnScrollBars();
//
//	// F7
//	if(pView->IsPenWidthsOn())
//		pView->OnViewPenWidths();
//
//	// F8
//	if(pView->IsPrintPreviewOn())
//		pView->OnViewPrintPreview();
//
//	// F9
//	if(pView->IsBackgroundImageOn())
//		pView->OnViewBackgroundImage();
//}
//
//
//// other
//void DlgProcReset_Zoom()
//{
//	view::Initialize();
//	CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);
//}
//
//void DlgProcReset_Colors()
//{
//	app.PenUseDefaults();
//}
//
//void DlgProcReset_Mode()
//{
//	app.SetWindowMode(ID_MODE_DRAW);
//}
//
//void DlgProcReset_Proj()
//{
//	CPegView::GetActiveView()->On3dViewsTop();
//}
//
//void DlgProcResetAll()
//{
//	// tasks
//	DlgProcReset_Draw();
//	DlgProcReset_Draw2();
//	DlgProcReset_Annotate();
//	DlgProcReset_Pens();
//	DlgProcReset_Pipe();
//	DlgProcReset_Fixup();
//	DlgProcReset_LPD();
//	DlgProcReset_RLPD();
//	DlgProcReset_Edit();
//
//	// options
//	DlgProcReset_Import();
//	DlgProcReset_Note();
//	DlgProcReset_Units();
//	DlgProcReset_Scale();
//	DlgProcReset_View();
//	DlgProcReset_Hatch();
//	DlgProcReset_Mouse();
//	DlgProcReset_Const();
//	DlgProcReset_Dim();
//	DlgProcReset_Scroll();
//	DlgProcReset_Page();
//
//	// other
//	DlgProcReset_Zoom();
//	DlgProcReset_Colors();
//	DlgProcReset_Mode();
//	DlgProcReset_Proj();
//}