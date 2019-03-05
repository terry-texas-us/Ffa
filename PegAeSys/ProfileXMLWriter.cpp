#include "stdafx.h"

#include "PegAEsysView.h"

#include "..\\src\\irrXML.h"

#include "SubProcDraw.h"
#include "SubProcDraw2.h"
#include "SubProcRLPD.h"
#include "SubProcLPD.h"
#include "SubProcPipe.h"
#include "SubProcAnnotate.h"
#include "DlgProcEditOps.h"
#include "SubProcFixup.h"
#include "UserAxis.h"
#include "Grid.h"
#include "Hatch.h"
#include "Scroll.h"
#include "Mouse.h"
#include "Projections.h"

#include "StateInfoFrm.h"
extern CInfoStatusFrm statusframe;
extern CInfoModeFrm modeframe;
extern CInfoOdomFrm odomframe;

using std::endl;

void PadXML(ofstream &outFile, int nSpaces)
{
	for(int i = 0; i < nSpaces; i++)
		outFile << " ";
}

char *StrBoolXML(bool bValue)
{
	if(bValue)
		return "true";
	
	return "false";
}

void ProfileXMLAnnotateWriter(ofstream &outFile)
{
	char szBuf[32];

	PadXML(outFile, 4);
	outFile << "<annotate nArrowType=\"";
	itoa(annotate::nArrowType, szBuf, 10);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dArrowSize=\"";
	gcvt(annotate::dArrowSize, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dBubbleRadius=\"";
	gcvt(annotate::dBubbleRadius, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "nBubbleFacets=\"";
	itoa(annotate::nBubbleFacets, szBuf, 10);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dGapSpacing=\"";
	gcvt(annotate::dGapSpacing, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dHookRadius=\"";
	gcvt(annotate::dHookRadius, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "strText=\"" << annotate::szDefText << "\"/>" << endl;
}

void ProfileXMLAppearanceWriter(ofstream &outFile)
{
	CPegView *pView = CPegView::GetActiveView();

	char szBuf[32];

	PadXML(outFile, 4);
	outFile << "<appearance bDispStatus=\"";
	outFile << StrBoolXML(app.IsStateInfoOn());
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "bDispMode=\"";
	outFile << StrBoolXML(app.IsModeInfoOn());
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "bDispOdom=\"";
	outFile << StrBoolXML(pView->IsOdometerOn());
	outFile << "\"" << endl;
	
	PadXML(outFile, 10);
	outFile << "bDispScroll=\"";
	outFile << StrBoolXML(pView->IsScrollbarsOn());
	outFile << "\"" << endl;
	
	PadXML(outFile, 10);
	outFile << "bDispTTF=\"";
	outFile << StrBoolXML(pView->IsTTFontsOn());
	outFile << "\"" << endl;
	
	PadXML(outFile, 10);
	outFile << "bDispOrigin=\"";
	outFile << StrBoolXML(pView->IsOriginMarkerOn());
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "OriginColor1=\"";
	itoa(pView->OriginGetColor1(), szBuf, 10);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "OriginColor2=\"";
	itoa(pView->OriginGetColor2(), szBuf, 10);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "BkgdColor=\"";
	itoa(app.BackgroundColorGet(), szBuf, 10);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "nInfoTrans=\"";
	itoa(app.GetTransFactor(), szBuf, 10);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "bDispWidths=\"";
	outFile << StrBoolXML(pView->IsPenWidthsOn());
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "strWidthFile=\"";
	outFile << pView->PenWdithsGetFile();
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "strColorFile=\"";
	outFile << app.PenColorsGetFile();
	outFile << "\"/>" << endl;
}

void ProfileXMLAxonometricWriter(ofstream &outFile)
{
	PadXML(outFile, 4);
	outFile << "<axonometric />" << endl;
}

void ProfileXMLDimensionWriter(ofstream &outFile)
{
	char szBuf[32];

	PadXML(outFile, 4);
	outFile << "<dimension dDistance=\"";
	gcvt(app.GetDimLen(), 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dAngle=\"";
	gcvt(app.GetDimAngZ(), 7, szBuf);
	outFile << szBuf << "\"/>" << endl;
}

void ProfileXMLDrawWriter(ofstream &outFile)
{
	PadXML(outFile, 4);
	outFile << "<draw eArcType=\"";
	switch(draw::eArcType)
	{
		case draw::ARC_THREE_POINT:	outFile << "ARC_THREE_POINT";	break;
		case draw::ARC_C2_POINT:	outFile << "ARC_C2_POINT";		break;
	}
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "eCurveType=\"";
	switch(draw::eCurveType)
	{
		case draw::CURVE_SPLINE:	outFile << "CURVE_SPLINE";	break;
		case draw::CURVE_FITTED:	outFile << "CURVE_FITTED";	break;
	}
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "eCircleType=\"";
	switch(draw::eCircleType)
	{
		case draw::CIRCLE_CENTER_RADIUS:	outFile << "CIRCLE_CENTER_RADIUS";	break;
		case draw::CIRCLE_DIAMETER:			outFile << "CIRCLE_DIAMETER";		break;
	}
	outFile << "\"" << endl;

	char szBuf[32];

	PadXML(outFile, 10);
	outFile << "nMarkerType=\"";
	itoa(pstate.MarkStyle(), szBuf, 10);
	outFile << szBuf << "\"/>" << endl;
}

void ProfileXMLDraw2Writer(ofstream &outFile)
{
	char szBuf[32];

	PadXML(outFile, 4);
	outFile << "<draw2 dWidth=\"";
	gcvt(draw2::DlgProcDraw2_dDistance, 7, szBuf);
	outFile << szBuf << "\"/>" << endl;
}

void ProfileXMLEditWriter(ofstream &outFile)
{
	char szBuf[32];

	PadXML(outFile, 4);
	outFile << "<edit dXRotAng=\"";
	gcvt(dlgproceditops::dXRotAng, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dYRotAng=\"";
	gcvt(dlgproceditops::dYRotAng, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dZRotAng=\"";
	gcvt(dlgproceditops::dZRotAng, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dXScale=\"";
	gcvt(dlgproceditops::dXScale, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dYScale=\"";
	gcvt(dlgproceditops::dYScale, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dZScale=\"";
	gcvt(dlgproceditops::dZScale, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dXMirror=\"";
	gcvt(dlgproceditops::dXMirrorScale, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dYMirror=\"";
	gcvt(dlgproceditops::dYMirrorScale, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dZMirror=\"";
	gcvt(dlgproceditops::dZMirrorScale, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dVernierSize=\"";
	gcvt(dlgproceditops::dVernierSize, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dVernierRot=\"";
	gcvt(dlgproceditops::dVernierRot, 7, szBuf);
	outFile << szBuf << "\"/>" << endl;
}

void ProfileXMLFixupWriter(ofstream &outFile)
{
	char szBuf[32];

	PadXML(outFile, 4);
	outFile << "<fixup dInfluenceAngle=\"";
	gcvt(fixup::dInfluenceAngle, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dFilletChamfer=\"";
	gcvt(fixup::dFilletChamfer, 7, szBuf);
	outFile << szBuf << "\"/>" << endl;
}

void ProfileXMLGridWriter(ofstream &outFile)
{
	char szBuf[32];

	PadXML(outFile, 4);
	outFile << "<grid bDotsOn=\"";
	outFile << StrBoolXML(grid::bDotsOn);
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "bLnsOn=\"";
	outFile << StrBoolXML(grid::bLnsOn);
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "bSnapOn=\"";
	outFile << StrBoolXML(grid::bSnapOn);
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "iDotsMax=\"";
	itoa(grid::iDotsMax, szBuf, 10);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dXSnapInt=\"";
	gcvt(grid::dXSnapInt, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dYSnapInt=\"";
	gcvt(grid::dYSnapInt, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dZSnapInt=\"";
	gcvt(grid::dZSnapInt, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dXDotInt=\"";
	gcvt(grid::dXDotInt, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dYDotInt=\"";
	gcvt(grid::dYDotInt, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dZDotInt=\"";
	gcvt(grid::dZDotInt, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dXLnInt=\"";
	gcvt(grid::dXLnInt, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dYLnInt=\"";
	gcvt(grid::dYLnInt, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dZLnInt=\"";
	gcvt(grid::dZLnInt, 7, szBuf);
	outFile << szBuf << "\"/>" << endl;
}

void ProfileXMLHatchWriter(ofstream &outFile)
{
	char szBuf[32];

	PadXML(outFile, 4);
	outFile << "<hatch nIndex=\"";
	itoa(pstate.GetPolygonIntStyleId(), szBuf, 10);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dXScale=\"";
	gcvt(hatch::dXAxRefVecScal, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dYScale=\"";
	gcvt(hatch::dYAxRefVecScal, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dRotation=\"";
	gcvt(hatch::dOffAng, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "eFillType=\"";
	switch(pstate.GetPolygonIntStyle())
	{
		case POLYGON_HOLLOW:	outFile << "hollow";	break;
		case POLYGON_SOLID:		outFile << "solid";		break;
		case POLYGON_HATCH:		outFile << "hatch";		break;
	}
	outFile << "\"/>" << endl;
}

void ProfileXMLImportWriter(ofstream &outFile)
{
	PadXML(outFile, 4);
	outFile << "<import />" << endl;
}

void ProfileXMLIsometricWriter(ofstream &outFile)
{
	char szBuf[32];

	PadXML(outFile, 4);
	outFile << "<isometric nLeftRight=\"";
	itoa(isometric::iLeftRight, szBuf, 10);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "nFrontBack=\"";
	itoa(isometric::iFrontBack, szBuf, 10);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "nAboveUnder=\"";
	itoa(isometric::iAboveUnder, szBuf, 10);
	outFile << szBuf << "\"/>" << endl;
}

void ProfileXMLLPDWriter(ofstream &outFile)
{
	char szBuf[32];

	PadXML(outFile, 4);
	outFile << "<lpd eJust=\"";
	switch(lpd::eJust)
	{
		case lpd::Center:	outFile << "center";	break;
		case lpd::Right:	outFile << "right";		break;
		case lpd::Left:		outFile << "left";		break;
	}
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "eElbow=\"";
	switch(lpd::eElbow)
	{
		case Mittered:	outFile << "mittered";	break;
		case Radial:	outFile << "radial";	break;
	}
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "bGenTurnVanes=\"";
	outFile << StrBoolXML(lpd::bGenTurnVanes);
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dSecWid=\"";
	gcvt(lpd::dSecWid, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dSecDep=\"";
	gcvt(lpd::dSecDep, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dInsRadElFac=\"";
	gcvt(lpd::dInsRadElFac, 7, szBuf);
	outFile << szBuf << "\"/>" << endl;
}

void ProfileXMLMouseWriter(ofstream &outFile)
{
	PadXML(outFile, 4);
	outFile << "<mouse strLeftDown=\"";
	outFile << mouse::szLeftMouseDown;
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "strLeftUp=\"";
	outFile << mouse::szLeftMouseUp;
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "strRightDown=\"";
	outFile << mouse::szRightMouseDown;
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "strRightUp=\"";
	outFile << mouse::szRightMouseUp;
	outFile << "\"/>" << endl;
}

void ProfileXMLNoteWriter(ofstream &outFile)
{
	char szBuf[32];

	CCharCellDef ccd;
	pstate.GetCharCellDef(ccd);

	CFontDef fd;
	pstate.GetFontDef(fd);

	PadXML(outFile, 4);
	outFile << "<note strFont=\"";
	outFile << fd.TextFont();
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dHeight=\"";
	gcvt(ccd.ChrHgtGet(), 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "nExpansion=\"";
	gcvt(ccd.ChrExpFacGet(), 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "nRotation=\"";
	gcvt(ccd.TextRotAngGet(), 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "nSpacing=\"";
	gcvt(fd.ChrSpac(), 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "nInclination=\"";
	gcvt(ccd.ChrSlantAngGet(), 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "eHorzAlign=\"";
	switch(fd.TextHorAlign())
	{
		case fd.HOR_ALIGN_LEFT:		outFile << "left";		break;
		case fd.HOR_ALIGN_CENTER:	outFile << "center";	break;
		case fd.HOR_ALIGN_RIGHT:	outFile << "right";		break;
	}
	outFile << "\"" << endl;
	
	PadXML(outFile, 10);
	outFile << "eVertAlign=\"";
	switch(fd.TextVerAlign())
	{
		case fd.VER_ALIGN_TOP:		outFile << "top";		break;
		case fd.VER_ALIGN_MIDDLE:	outFile << "middle";	break;
		case fd.VER_ALIGN_BOTTOM:	outFile << "bottom";	break;
	}
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "ePath=\"";
	switch(fd.TextPath())
	{
		case fd.PATH_RIGHT:	outFile << "right";		break;
		case fd.PATH_LEFT:	outFile << "left";		break;
		case fd.PATH_UP:	outFile << "up";		break;
		case fd.PATH_DOWN:	outFile << "down";		break;
	}
	outFile << "\"/>" << endl;
}

void ProfileXMLObliqueWriter(ofstream &outFile)
{
	PadXML(outFile, 4);
	outFile << "<oblique />" << endl;
}

void ProfileXMLPensWriter(ofstream &outFile)
{
	char szBuf[32];

	PadXML(outFile, 4);
	outFile << "<pens nColor=\"";
	itoa(pstate.PenColor(), szBuf, 10);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "nStyle=\"";
	itoa(pstate.PenStyle(), szBuf, 10);
	outFile << szBuf << "\"/>" << endl;
}

void ProfileXMLPerspectiveWriter(ofstream &outFile)
{
	PadXML(outFile, 4);
	outFile << "<perspective />" << endl;
}	

void ProfileXMLPipeWriter(ofstream &outFile)
{
	char szBuf[32];

	PadXML(outFile, 4);
	outFile << "<pipe dTicSize=\"";
	gcvt(pipe::dTicSize, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dCSize=\"";
	gcvt(pipe::dCSize, 7, szBuf);
	outFile << szBuf << "\"/>" << endl;
}

void ProfileXMLPrintWriter(int nIndex, ofstream &outFile)
{
	char szBuf[32];

	printStates.SetCurrentState(nIndex-1);

	PadXML(outFile, 4);
	switch(nIndex)
	{
		case 1:	outFile << "<print1 ";	break;
		case 2:	outFile << "<print2 ";	break;
		case 3:	outFile << "<plot3 ";	break;
		case 4:	outFile << "<plot4 ";	break;
	}

	if((nIndex == 3) || (nIndex == 4))
	{
		outFile << "eSheetSize=\"";
		switch(printStates.GetSheetSize())
		{
			case FULL_SHEET:		outFile << "FULL_SHEET";		break;
			case HALF_SHEET:		outFile << "HALF_SHEET";		break;
			case QUARTER_SHEET:		outFile << "QUARTER_SHEET";		break;
		}
		outFile << "\"" << endl;
		PadXML(outFile, 10);
	}

	outFile << "bFileTracingNames=\"";
	outFile << StrBoolXML(printStates.GetDispLayer());
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "bBackgroundImage=\"";
	outFile << StrBoolXML(printStates.GetDispBkgImage());
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "bMarkers=\"";
	outFile << StrBoolXML(printStates.GetPrintMarkers());
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "bPenWidths=\"";
	outFile << StrBoolXML(printStates.GetUsePenWidths());
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "strPenWidthsFile=\"";
	outFile << printStates.GetPenWidthsFile();
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "bTranslatePens=\"";
	outFile << StrBoolXML(printStates.GetTranslatePens());
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "strTranslatePensFile=\"";
	outFile << printStates.GetTranslateFile();
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dMarginsX=\"";
	gcvt(printStates.GetMarginX(), 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dMarginsY=\"";
	gcvt(printStates.GetMarginY(), 7, szBuf);
	outFile << szBuf << "\"/>" << endl;
}

void ProfileXMLRLPDWriter(ofstream &outFile)
{
	char szBuf[32];

	PadXML(outFile, 4);
	outFile << "<rlpd eJust=\"";
	switch(rlpd::eJust0)
	{
		case rlpd::Center0:		outFile << "center";	break;
		case rlpd::Right0:		outFile << "right";		break;
		case rlpd::Left0:		outFile << "left";		break;
	}
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "bGenCenterline=\"";
	outFile << StrBoolXML(rlpd::bGenCenterline);
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dSecDiam=\"";
	gcvt(rlpd::dSecDiam, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dInsideRadius=\"";
	gcvt(rlpd::dInsideRadiusFactor, 7, szBuf);
	outFile << szBuf << "\"/>" << endl;
}

void ProfileXMLScaleWriter(ofstream &outFile)
{
	char szBuf[32];

	PadXML(outFile, 4);
	outFile << "<scale dScale=\"";
	gcvt(app.GetScale(), 7, szBuf);
	outFile << szBuf << "\"/>" << endl;
}

void ProfileXMLScrollWriter(ofstream &outFile)
{
	char szBuf[32];

	PadXML(outFile, 4);
	outFile << "<scroll nHorzSize=\"";
	itoa(scroll::nHScrollThumbSize, szBuf, 10);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "nHorzSensit=\"";
	itoa(scroll::nHScrollSens, szBuf, 10);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "nVertSize=\"";
	itoa(scroll::nVScrollThumbSize, szBuf, 10);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "nVertSensit=\"";
	itoa(scroll::nVScrollSens, szBuf, 10);
	outFile << szBuf << "\"/>" << endl;
}

void ProfileXMLUnitsWriter(ofstream &outFile)
{
	char szBuf[32];

	PadXML(outFile, 4);
	outFile << "<units eType=\"";
	switch(app.GetUnits())
	{
		case Architectural:	outFile << "architectural";	break;
		case Engineering:	outFile << "engineering";	break;
		case Feet:			outFile << "feet";			break;
		case Inches:		outFile << "inches";		break;
		case Meters:		outFile << "meters";		break;
		case Millimeters:	outFile << "millimeters";	break;
		case Centimeters:	outFile << "centimeters";	break;
		case Decimeters:	outFile << "decimeters";	break;
		case Kilometers:	outFile << "kilometers";	break;
	}
	outFile << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "nPrecision=\"";
	itoa(app.GetUnitsPrec(), szBuf, 10);
	outFile << szBuf << "\"/>" << endl;
}

void ProfileXMLUserAxisWriter(ofstream &outFile)
{
	char szBuf[32];

	PadXML(outFile, 4);
	outFile << "<useraxis dInfluenceAngle=\"";
	gcvt(UserAxis::UserAxis_dInfAng, 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "dOffsetAngle=\"";
	gcvt(UserAxis::UserAxis_dAxOffAng, 7, szBuf);
	outFile << szBuf << "\"/>" << endl;
}

void ProfileXMLViewWriter(ofstream &outFile)
{
	char szBuf[32];

	PadXML(outFile, 4);
	outFile << "<view nNearClip=\"";
	gcvt(view::GetNearClipDistance(), 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "nFarClip=\"";
	gcvt(view::GetFarClipDistance(), 7, szBuf);
	outFile << szBuf << "\"" << endl;

	PadXML(outFile, 10);
	outFile << "nUpRotAngle=\"";
	gcvt(Vec_Angle_xy(view::GetVwUp()) / RADIAN, 7, szBuf);
	outFile << szBuf << "\"/>" << endl;
}