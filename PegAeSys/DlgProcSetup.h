#pragma once

void DlgProcSetupSetPage(HWND hwnd, int page);
HTREEITEM DlgProcOptionsTreeParent(HWND hDlg, char *szText, int lParam);
HTREEITEM DlgProcOptionsTreeChild(HWND hDlg, HTREEITEM hParent, char *szText, int lParam);

void InitWindowText(HWND hDlg, CString strText);

void InitGeneral(HWND hDlg);
void InitOptions(HWND hDlg);
void InitTasks(HWND hDlg);
void InitProjections(HWND hDlg);

void HighlightItem(HWND hDlg, HTREEITEM hChild, WORD wDocPage);

void OkGeneral(HWND hDlg);
void OkOptions(HWND hDlg);
void OkTasks(HWND hDlg);
void OkProjections(HWND hDlg);