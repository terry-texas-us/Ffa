#pragma once

// Prototypes for all procedure called directly by Windows

// Dialog procedures
BOOL CALLBACK DlgProcAbout (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcAnnotateEnterText(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcBlockInsert(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcEditTrap_CommandsQuery(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcFileBlock(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcFileManage(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcFileShadow(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcFileTracing(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcFixupOptions(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcGetLayerName(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcHomePointGo(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM);
BOOL CALLBACK DlgProcHomePointSet(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM);
BOOL CALLBACK DlgProcModeLetter(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcModeRevise(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcPipeSymbol(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcSetPasteLoc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcTrapFilter(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcTrapModify(HWND, UINT, WPARAM, LPARAM);
//BOOL CALLBACK DlgProcViewLighting(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcViewZoom(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcImportReport(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcExportACAD(HWND, UINT, WPARAM, LPARAM);

//mak
BOOL CALLBACK DlgProcFileClean(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcFileArchive(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcOptions(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcSetup(HWND, UINT, WPARAM, LPARAM);
//BOOL CALLBACK DlgProcPrintPreview(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcStatePreview(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcSoloStatePreview(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcPlotPreview(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcTranslatePens(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcInvalidPrim(HWND, UINT, WPARAM, LPARAM);
//BOOL CALLBACK DlgProcProgress(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcLayoutSelect(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcTextPaste(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcBitmapEdit(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcBackgroundImage(HWND, UINT, WPARAM, LPARAM);

//General
BOOL CALLBACK DlgProcProfileManager(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcProfile(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcAppearance(HWND, UINT, WPARAM, LPARAM);

//Projections
BOOL CALLBACK DlgProcProjection(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcProjAxonometric(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcProjIsometric(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcProjOblique(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcProjPerspective(HWND, UINT, WPARAM, LPARAM);

//Options Tabs
BOOL CALLBACK DlgProcOptionsTabNote(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcOptionsTabImport(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcOptionsTabUnits(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcOptionsTabView(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcOptionsTabScale(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcOptionsTabConstraints(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcOptionsTabUserAxis(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcOptionsTabHatch(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcOptionsTabDimension(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcOptionsTabScrollbars(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcOptionsTabMouse(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcOptionsTabRegistry(HWND, UINT, WPARAM, LPARAM);

// Setup Tabs
BOOL CALLBACK DlgProcSetupTabPen(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcSetupTabDraw(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcSetupTabLPD(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcSetupTabRLPD(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcSetupTabPipe(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcSetupTabDraw2(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcSetupTabAnnotate(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcSetupTabEdit(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcSetupTabFixup(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProcSetupTabDimension(HWND, UINT, WPARAM, LPARAM);

double	DlgBoxGetItemDouble(HWND hDlg, int iCtrlId);
double	DlgBoxGetItemUnitsText(HWND hDlg, int iCtrlId);
void	DlgBoxSetItemDouble(HWND hDlg, int iCtrlId, double dVal);
void	DlgBoxSetItemUnitsText(HWND hDlg, int iCtrlId, double dVal);