#include "stdafx.h"

#include "PegAEsys.h"

#include "ddeGItms.h"
#include "DlgProcSetLength.h"
#include "DlgProcEditOps.h"
#include "FileJob.h"
#include "UnitsString.h"

extern CTMat tmEditSeg;

double DlgBoxGetItemDouble(HWND hDlg, int iCtrlId)
{
	char szBuf[32];

	::GetDlgItemText(hDlg, iCtrlId, (LPSTR) szBuf, 32);
	return (atof(szBuf));
}				

double DlgBoxGetItemUnitsText(HWND hDlg, int iCtrlId)
{
	char szBuf[32];

	::GetDlgItemText(hDlg, iCtrlId, (LPSTR) szBuf, 32);
	return (UnitsString_ParseLength(app.GetUnits(), szBuf));
}				

void DlgBoxSetItemDouble(HWND hDlg, int iCtrlId, double dVal)
{
	CString str;
	str.Format("%f", dVal);	
	SetDlgItemText(hDlg, iCtrlId, str);
}

void DlgBoxSetItemUnitsText(HWND hDlg, int iCtrlId, double dVal)
{
	char szBuf[32];
	UnitsString_FormatLength(szBuf, app.GetUnits(), dVal, 16, 4);
	CString temp = szBuf;
	SetDlgItemText(hDlg, iCtrlId, temp.TrimLeft());
}

LONG pGetLayerName()
{
	return ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_GET_LAYER_NAME), app.GetSafeHwnd(), DlgProcGetLayerName);
}