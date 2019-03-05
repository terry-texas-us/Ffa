#pragma once

int msgConfirm(UINT uiMsgId);
int msgConfirm(UINT uiMsgId, const CString& strVal);

void msgInformation(const CString& strMes);
void msgInformation(UINT = 0);
void msgInformation(UINT uiId, const CString& strVal);

void msgWarning(UINT uiMsgId, HWND hDlgWnd = 0);
void msgWarning(UINT uiMsgId, const CString& strVal, HWND hDlgWnd = 0);