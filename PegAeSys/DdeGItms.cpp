#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysView.h"

#include "ddeGItms.h"

extern double gbl_dExtNum;
extern char gbl_szExtStr[128];

namespace dde
{
	PITEMINFO DimAngZInfo;
	PITEMINFO DimLenInfo;  
	PITEMINFO EngAngZInfo; 
	PITEMINFO EngLenInfo;
	PITEMINFO ExtNumInfo;
	PITEMINFO ExtStrInfo;
	PITEMINFO RelPosXInfo;
	PITEMINFO RelPosYInfo;
	PITEMINFO RelPosZInfo;
	PITEMINFO ScaleInfo;
}

// dimension z axis angle interface
bool dde::DimAngZPoke(UINT, HSZ, HSZ, HDDEDATA hData)
{
	char	sz[32];
	DdeGetData(hData, (LPBYTE) sz, (DWORD) sizeof(sz), (DWORD) 0);
	app.SetDimAngZ(atof(sz));
	app.StatusLineDisplay();				
	return true;
}

HDDEDATA dde::DimAngZRequest(UINT wFmt, HSZ, HSZ hszItem)
{
	char sz[32];
	sprintf(sz, "%f", app.GetDimAngZ());
	return dde::MakeCFText(wFmt, (LPSTR) sz, hszItem);
}

// Dimension length interface
bool dde::DimLenPoke(UINT, HSZ, HSZ, HDDEDATA hData)
{
	char	sz[32];
	DdeGetData(hData, (LPBYTE) sz, (DWORD) sizeof(sz), (DWORD) 0);
	app.SetDimLen(atof(sz));
	app.StatusLineDisplay(DimLen);				

	return true;
}

HDDEDATA dde::DimLenRequest(UINT wFmt, HSZ, HSZ hszItem)
{
	char sz[32];
	sprintf(sz, "%f", app.GetDimLen());
	return dde::MakeCFText(wFmt, (LPSTR) sz, hszItem);
}

// Engaged length interface (no poke)
HDDEDATA dde::EngLenRequest(UINT wFmt, HSZ, HSZ hszItem)
{
	char sz[32];
	sprintf(sz, "%f", app.GetEngLen());
	return dde::MakeCFText(wFmt, (LPSTR) sz, hszItem);
}

// Engaged z axis angle interface (no poke) 
HDDEDATA dde::EngAngZRequest(UINT wFmt, HSZ, HSZ hszItem)
{
	char sz[32];
	sprintf(sz, "%f", app.GetEngAngZ());
	return dde::MakeCFText(wFmt, (LPSTR) sz, hszItem);
}

// Extracted number interface (no poke)
HDDEDATA dde::ExtNumRequest(UINT wFmt, HSZ, HSZ hszItem)
{
	char sz[32];
	sprintf(sz, "%f", gbl_dExtNum);
	return dde::MakeCFText(wFmt, (LPSTR) sz, hszItem);
}

// Extracted string interface (no poke)
HDDEDATA dde::ExtStrRequest(UINT wFmt, HSZ, HSZ hszItem)
{
	return dde::MakeCFText(wFmt, (LPSTR) gbl_szExtStr, hszItem);
}

// Relative x position interface (no poke)
HDDEDATA dde::RelPosXRequest(UINT wFmt, HSZ, HSZ hszItem)
{
	char sz[32];
	sprintf(sz, "%f", CPegView::GetActiveView()->GetRelPos()[0]);
	return dde::MakeCFText(wFmt, (LPSTR) sz, hszItem);
}

// Relative y position interface (no poke)
HDDEDATA dde::RelPosYRequest(UINT wFmt, HSZ, HSZ hszItem)
{
	char sz[32];
	sprintf(sz, "%f", CPegView::GetActiveView()->GetRelPos()[1]);
	return dde::MakeCFText(wFmt, (LPSTR) sz, hszItem);
}

// Relative z position interface (no poke)
HDDEDATA dde::RelPosZRequest(UINT wFmt, HSZ, HSZ hszItem)
{
	char sz[32];
	sprintf(sz, "%f", CPegView::GetActiveView()->GetRelPos()[2]);
	return dde::MakeCFText(wFmt, (LPSTR) sz, hszItem);
}

// Scale interface
bool dde::ScalePoke(UINT, HSZ, HSZ, HDDEDATA hData)
{
	char	sz[32];
	DdeGetData(hData, (LPBYTE) sz, (DWORD) sizeof(sz), (DWORD) 0);
	app.SetScale(atof(sz));
	app.StatusLineDisplay(Scale);				
	return true;
}

HDDEDATA dde::ScaleRequest(UINT wFmt, HSZ, HSZ hszItem)
{
	char sz[32];
	sprintf(sz, "%f", app.GetScale());
	return dde::MakeCFText(wFmt, (LPSTR) sz, hszItem);
}