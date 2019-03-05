#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysView.h"

SPrintState::SPrintState()
{
	strTranslateFile = "Black on White.txt";
	strOldTranslate = "Default.txt";
	bTranslatePens = true;

	bUsePenWidths = true;
	strPenWidthsFile = "Plotter.txt";
	bOldPenWidths = false;
	
	dMarginX = .000;
	dMarginY = .000;

	bDispLayer = true;
	bDispClipRect = true;
	bDispBkgImage = false;
	bPrintMarkers = false;
	bUseBorders = true;

	eSheetSize = FULL_SHEET;
}