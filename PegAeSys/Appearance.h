#pragma once

namespace appearance
{
	bool bDispStatus = true;
	bool bDispMode = true;
	bool bDispOdom = false;
	bool bDispScroll = true;
	bool bDispTTF = true;
	bool bDispOrigin = true;

	COLORREF OriginColor1;
	COLORREF OriginColor2;

	COLORREF InfoColor1;
	COLORREF InfoColor2;
	COLORREF InfoColor3;

	int nInfoTrans = 200;

	bool bDispWidths = false;
	CString strWidthFile;

	CString strColorFile;
}