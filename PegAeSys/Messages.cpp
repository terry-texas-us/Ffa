#include "stdafx.h"

#include "PegAEsys.h"

int msgConfirm(UINT uiMsgId)
{
	char szMsg[256];
	SecureZeroMemory(szMsg, 256);
	::LoadString(app.GetInstance(), uiMsgId, szMsg, sizeof(szMsg));
	
	char* pMsg = strtok(szMsg, "\t");
	char* pCap = strtok(0, "\n");
	
	return (MessageBox(0, pMsg, pCap, MB_ICONINFORMATION | MB_YESNO | MB_DEFBUTTON2));
}

int msgConfirm(UINT uiMsgId, const CString& strVal)
{
	char szFmt[256];
	char szMsg[256];
	SecureZeroMemory(szFmt, 256);
	::LoadString(app.GetInstance(), uiMsgId, szFmt, sizeof(szFmt));
	
	sprintf(szMsg, szFmt, strVal);
	
	char* pMsg = strtok(szMsg, "\t");
	char* pCap = strtok(0, "\n");
	
	return (MessageBox(0, pMsg, pCap, MB_ICONINFORMATION | MB_YESNOCANCEL | MB_DEFBUTTON2));
}

void msgWarning(UINT uiMsgId, HWND hDlgWnd)
{
	char szMsg[256];
	SecureZeroMemory(szMsg, 256);
	::LoadString(app.GetInstance(), uiMsgId, szMsg, sizeof(szMsg));
	
	char* pMsg = strtok(szMsg, "\t");
	char* pCap = strtok(0, "\n");
	
	MessageBox(hDlgWnd, pMsg, pCap, MB_ICONWARNING | MB_OK);
}

void msgWarning(UINT uiMsgId, const CString& strVal, HWND hDlgWnd)
{
	char szFmt[256];
	char szMsg[256];
	
	::LoadString(app.GetInstance(), uiMsgId, szFmt, sizeof(szFmt));
	
	sprintf(szMsg, szFmt, strVal);
	
	char* pMsg = strtok(szMsg, "\t");
	char* pCap = strtok(0, "\n");
	
	MessageBox(hDlgWnd, pMsg, pCap, MB_ICONWARNING | MB_OK | MB_APPLMODAL);
}

void msgInformation(const CString& strMes)
{
	::SetWindowText(app.GetSafeHwnd(), strMes);
}

void msgInformation(UINT nId)
{
	char szMsg[256];
	SecureZeroMemory(szMsg, 256);
	CString ver;
	ver += "PegAEsys BETA 4.08.03";
#ifdef DEV_BRANCH
	ver += " ";
	ver += DEV_BRANCH;
#endif

	if (nId == 0)
	{
		strcpy(szMsg,ver);
//		strcpy(szMsg,"PegAEsys - 10/31/2005");

		// mak - displays mode in title bar
		strcat(szMsg, " - [");
		int iLen = strlen(szMsg);
		char* pMode = &szMsg[iLen];

		::LoadString(app.GetInstance(), app.m_iModeId, pMode, sizeof(szMsg) - iLen);
		strtok(szMsg, "\n");

		strcat(szMsg, "]");

//#ifdef ENABLE_UNDO
//		strcat(szMsg, "                            UNDO");
//#endif
	}	
	else
	{
		::LoadString(app.GetInstance(), nId, szMsg, sizeof(szMsg));
	}
	msgInformation(szMsg);
}

void msgInformation(UINT nId, const CString& strVal)
{
	char szFmt[256];
	char szMsg[256];
	SecureZeroMemory(szFmt, 256);
	::LoadString(app.GetInstance(), nId, szFmt, sizeof(szFmt));
	
	sprintf(szMsg, szFmt, strVal);
	
	msgInformation(szMsg);
}