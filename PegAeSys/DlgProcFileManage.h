#pragma once

void	DlgProcFileManageDoLayerActive(HWND);
void	DlgProcFileManageDoLayerDelete(HWND);
void	DlgProcFileManageDoLayerHidden(HWND);
void	DlgProcFileManageDoLayerMelt(HWND);
void	DlgProcFileManageDoLayerRename(HWND);
void	DlgProcFileManageDoLayerStatic(HWND);
void	DlgProcFileManageDoLayerWork(HWND);
void	DlgProcFileManageDoTracingCloak(HWND);
void	DlgProcFileManageDoTracingExclude(HWND);
void	DlgProcFileManageDoTracingFuse(HWND);
void	DlgProcFileManageDoTracingInclude(HWND);
void	DlgProcFileManageDoTracingMap(HWND);
void	DlgProcFileManageDoTracingView(HWND);

// MAK - Solo
void	DlgProcFileManageDoSolo(HWND);
void	DlgProcFileManageDoSoloTracing(CString strName);
void	DlgProcFileManageDoSoloLayer(CString strName);
void	DlgProcFileManageDoSoloBlock(CString strName);

void	DlgProcFileManageDoSoloLoadHwnd();
void	DlgProcFileManageDoSoloCreateProcess(HWND &rHwnd);
void	DlgProcFileManageDoSoloTracingSpawn(CString strName);
void	DlgProcFileManageDoSoloLayerSpawn(CString strName);
void	DlgProcFileManageDoSoloBlockSpawn(CString strName);

void	DlgProcFileManageInit(HWND);

LRESULT	GetCurSel(HWND hDlg, int iListId, CString& strName);
LONG	pGetLayerName();