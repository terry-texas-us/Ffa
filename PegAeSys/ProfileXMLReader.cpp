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

/////////////////////////////////////////////////////////////////////////////////////////
// helper function
/////////////////////////////////////////////////////////////////////////////////////////

bool BoolXML(const char *szText)
{
	if(!strcmp("false", szText))
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

void ProfileXMLAnnotate(xml::IrrXMLReader* xml)
{
	annotate::dArrowSize = xml->getAttributeValueAsFloat("dArrowSize");
	annotate::dBubbleRadius = xml->getAttributeValueAsFloat("dBubbleRadius");;
	annotate::dGapSpacing = xml->getAttributeValueAsFloat("dGapSpacing");;
	annotate::dHookRadius = xml->getAttributeValueAsFloat("dHookRadius");;
	annotate::nArrowType = xml->getAttributeValueAsInt("nArrowType");
	annotate::nBubbleFacets = xml->getAttributeValueAsInt("nBubbleFacets");;

	const char *szBuf = xml->getAttributeValue("strText");
	strcpy(annotate::szDefText, szBuf);
}

void ProfileXMLAppearance(xml::IrrXMLReader* xml)
{
	CPegView *pView = CPegView::GetActiveView();

	app.StatusInfoEnable(BoolXML(xml->getAttributeValue("bDispStatus")));
	app.ModeInfoEnable(BoolXML(xml->getAttributeValue("bDispMode")));
	pView->OdometerEnable(BoolXML(xml->getAttributeValue("bDispOdom")));
	pView->ScrollbarEnable(BoolXML(xml->getAttributeValue("bDispScroll")));
	pView->TTFEnable(BoolXML(xml->getAttributeValue("bDispTTF")));
	pView->OriginEnable(BoolXML(xml->getAttributeValue("bDispOrigin")));
	pView->OriginSetColor1(xml->getAttributeValueAsInt("OriginColor1"));
	pView->OriginSetColor2(xml->getAttributeValueAsInt("OriginColor2"));

	pView->PenWidthsEnable(BoolXML(xml->getAttributeValue("bDispWidths")));
	pView->PenWidthsSetFile(xml->getAttributeValue("strWidthFile"));

	app.SetTransFactor(xml->getAttributeValueAsInt("nInfoTrans"));

	app.BackgroundColorSet(xml->getAttributeValueAsInt("BkgdColor"));
	app.PenColorsSetFile(xml->getAttributeValue("strColorFile"));
}

void ProfileXMLDimension(xml::IrrXMLReader* xml)
{
	app.SetDimLen(xml->getAttributeValueAsFloat("dDistance"));
	app.SetDimAngZ(xml->getAttributeValueAsFloat("dAngle"));
}

void ProfileXMLDraw(xml::IrrXMLReader* xml)
{
	const char *szBuf;
	
	szBuf = xml->getAttributeValue("eArcType");
	if(!strcmp("ARC_THREE_POINT", szBuf))
		draw::eArcType = draw::ARC_THREE_POINT;
	else
		draw::eArcType = draw::ARC_C2_POINT;

	szBuf = xml->getAttributeValue("eCurveType");
	if(!strcmp("CURVE_SPLINE", szBuf))
		draw::eCurveType = draw::CURVE_SPLINE;
	else
		draw::eCurveType = draw::CURVE_FITTED;

	szBuf = xml->getAttributeValue("eCircleType");
	if(!strcmp("CIRCLE_CENTER_RADIUS", szBuf))
		draw::eCircleType = draw::CIRCLE_CENTER_RADIUS;
	else
		draw::eCircleType = draw::CIRCLE_DIAMETER;

	pstate.SetMarkStyle((short)xml->getAttributeValueAsInt("nMarkerType"));
}

void ProfileXMLDraw2(xml::IrrXMLReader* xml)
{
	draw2::DlgProcDraw2_dDistance = xml->getAttributeValueAsFloat("dWidth");
}

void ProfileXMLEdit(xml::IrrXMLReader* xml)
{
	dlgproceditops::SetMirrorScale(xml->getAttributeValueAsFloat("dXMirror"), 
								   xml->getAttributeValueAsFloat("dYMirror"),
								   xml->getAttributeValueAsFloat("dZMirror"));

	dlgproceditops::SetRotAng(xml->getAttributeValueAsFloat("dXRotAng"),
							  xml->getAttributeValueAsFloat("dYRotAng"),
							  xml->getAttributeValueAsFloat("dZRotAng"));

	dlgproceditops::SetScale(xml->getAttributeValueAsFloat("dXScale"),
		                     xml->getAttributeValueAsFloat("dYScale"),
							 xml->getAttributeValueAsFloat("dZScale"));

	dlgproceditops::dVernierSize = xml->getAttributeValueAsFloat("dVernierSize");
	dlgproceditops::dVernierRot = xml->getAttributeValueAsFloat("dVernierRot");
}

void ProfileXMLFixup(xml::IrrXMLReader* xml)
{
	fixup::dFilletChamfer = xml->getAttributeValueAsFloat("dFilletChamfer");
	fixup::dInfluenceAngle = xml->getAttributeValueAsFloat("dInfluenceAngle");
}

void ProfileXMLGrid(xml::IrrXMLReader* xml)
{
	grid::bDotsOn = BoolXML(xml->getAttributeValue("bDotsOn"));
	grid::bLnsOn = BoolXML(xml->getAttributeValue("bLnsOn"));
	grid::bSnapOn = BoolXML(xml->getAttributeValue("bSnapOn"));

	grid::iDotsMax = xml->getAttributeValueAsInt("iDotsMax");

	grid::dXSnapInt = xml->getAttributeValueAsFloat("dXSnapInt");
	grid::dYSnapInt = xml->getAttributeValueAsFloat("dYSnapInt");
	grid::dZSnapInt = xml->getAttributeValueAsFloat("dZSnapInt");

	grid::dXDotInt = xml->getAttributeValueAsFloat("dXDotInt");
	grid::dYDotInt = xml->getAttributeValueAsFloat("dYDotInt");
	grid::dZDotInt = xml->getAttributeValueAsFloat("dZDotInt");

	grid::dXLnInt = xml->getAttributeValueAsFloat("dXLnInt");
	grid::dYLnInt = xml->getAttributeValueAsFloat("dYLnInt");
	grid::dZLnInt = xml->getAttributeValueAsFloat("dZLnInt");
}

void ProfileXMLHatch(xml::IrrXMLReader* xml)
{
	const char *szBuf;
	szBuf = xml->getAttributeValue("eFillType");
	if(!strcmp("hollow", szBuf))
		pstate.SetPolygonIntStyle(POLYGON_HOLLOW);
	else if(!strcmp("solid", szBuf))
		pstate.SetPolygonIntStyle(POLYGON_SOLID);
//	else if(!strcmp("pattern", szBuf))
//		pstate.SetPolygonIntStyle(POLYGON_PATTERN);
	else //"hatch"
		pstate.SetPolygonIntStyle(POLYGON_HATCH);

	pstate.SetPolygonIntStyleId((short)xml->getAttributeValueAsInt("nIndex"));

	hatch::dOffAng = xml->getAttributeValueAsFloat("dRotation");
	hatch::dXAxRefVecScal = xml->getAttributeValueAsFloat("dXScale");
	hatch::dYAxRefVecScal = xml->getAttributeValueAsFloat("dYScale");
}

void ProfileXMLIsometric(xml::IrrXMLReader* xml)
{
	isometric::iLeftRight = xml->getAttributeValueAsInt("nLeftRight");
	isometric::iFrontBack = xml->getAttributeValueAsInt("nFrontBack");
	isometric::iAboveUnder = xml->getAttributeValueAsInt("nAboveUnder");
}

void ProfileXMLLPD(xml::IrrXMLReader* xml)
{
	const char *szBuf = xml->getAttributeValue("eJust");
	if(!strcmp("center", szBuf))
		lpd::eJust = lpd::Center;
	else if(!strcmp("right", szBuf))
		lpd::eJust = lpd::Right;
	else
		lpd::eJust = lpd::Left;

	lpd::bGenTurnVanes = BoolXML(xml->getAttributeValue("bGenTurnVanes"));
	lpd::dSecDep = xml->getAttributeValueAsFloat("dSecDep");
	lpd::dInsRadElFac = xml->getAttributeValueAsFloat("dInsRadElFac");
	lpd::dSecWid = xml->getAttributeValueAsFloat("dSecWid");

	szBuf = xml->getAttributeValue("eElbow");
	if(!strcmp("mittered", szBuf))
		lpd::eElbow = Mittered;
	else
		lpd::eElbow = Radial;
}

void ProfileXMLMouse(xml::IrrXMLReader* xml)
{
	const char *szBuf = xml->getAttributeValue("strLeftDown");
	strcpy(mouse::szLeftMouseDown, szBuf);

	szBuf = xml->getAttributeValue("strLeftUp");
	strcpy(mouse::szLeftMouseUp, szBuf);

	szBuf = xml->getAttributeValue("strRightDown");
	strcpy(mouse::szRightMouseDown, szBuf);

	szBuf = xml->getAttributeValue("strRightUp");
	strcpy(mouse::szRightMouseUp, szBuf);
}

void ProfileXMLNote(xml::IrrXMLReader* xml)
{
	CPegView* pView = CPegView::GetActiveView();
	CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();

	CCharCellDef ccd;
	pstate.GetCharCellDef(ccd);
	ccd.ChrExpFacSet(xml->getAttributeValueAsFloat("nExpansion"));
	ccd.ChrHgtSet(xml->getAttributeValueAsFloat("dHeight"));
	ccd.ChrSlantAngSet(xml->getAttributeValueAsFloat("nInclination"));
	ccd.TextRotAngSet(xml->getAttributeValueAsFloat("nRotation"));
	
	CFontDef fd;
	pstate.GetFontDef(fd);
	fd.ChrSpacSet(xml->getAttributeValueAsFloat("nSpacing"));
	fd.TextFontSet(xml->getAttributeValue("strFont"));

	WORD wPrec = (strcmp(fd.TextFont(), "Simplex.psf") != 0) ? CFontDef::PREC_TRUETYPEFONT : CFontDef::PREC_PEGSTROKEFONT;
	fd.TextPrecSet(wPrec);

	const char *szBuf;
	szBuf = xml->getAttributeValue("eHorzAlign");
	if(!strcmp("left", szBuf))
		fd.TextHorAlignSet(fd.HOR_ALIGN_LEFT);
	else if(!strcmp("center", szBuf))
		fd.TextHorAlignSet(fd.HOR_ALIGN_CENTER);
	else // "right"
		fd.TextHorAlignSet(fd.HOR_ALIGN_RIGHT);

	szBuf = xml->getAttributeValue("eVertAlign");
	if(!strcmp("top", szBuf))
		fd.TextVerAlignSet(fd.VER_ALIGN_TOP);
	else if(!strcmp("middle", szBuf))
		fd.TextVerAlignSet(fd.VER_ALIGN_MIDDLE);
	else // "bottom"
		fd.TextVerAlignSet(fd.VER_ALIGN_BOTTOM);

	szBuf = xml->getAttributeValue("ePath");
	if(!strcmp("right", szBuf))
		fd.TextPathSet(fd.PATH_RIGHT);
	else if(!strcmp("left", szBuf))
		fd.TextPathSet(fd.PATH_LEFT);
	else if(!strcmp("up", szBuf))
		fd.TextPathSet(fd.PATH_UP);
	else // "down"
		fd.TextPathSet(fd.PATH_DOWN);

	pstate.SetFontDef(pDC, fd);
	pstate.SetCharCellDef(ccd);
}

void ProfileXMLPens(xml::IrrXMLReader* xml)
{
	pstate.SetPenColor((PENCOLOR)xml->getAttributeValueAsInt("nColor"));
	pstate.SetPenStyle((PENSTYLE)xml->getAttributeValueAsInt("nStyle"));
}

void ProfileXMLPipe(xml::IrrXMLReader* xml)
{
	pipe::dTicSize = xml->getAttributeValueAsFloat("dTicSize");
	pipe::dCSize = xml->getAttributeValueAsFloat("dCSize");
}

void ProfileXMLRLPD(xml::IrrXMLReader* xml)
{
	const char *szBuf = xml->getAttributeValue("eJust");
	if(!strcmp("center", szBuf))
		rlpd::eJust0 = rlpd::Center0;
	else if(!strcmp("right", szBuf))
		rlpd::eJust0 = rlpd::Right0;
	else
		rlpd::eJust0 = rlpd::Left0;

	rlpd::bGenCenterline = BoolXML(xml->getAttributeValue("bGenCenterline"));
//	rlpd::bContSec = BoolXML(xml->getAttributeValue("bContSec"));
	rlpd::dSecDiam = (double)xml->getAttributeValueAsFloat("dSecDiam");
	rlpd::dInsideRadiusFactor = (double)xml->getAttributeValueAsFloat("dInsideRadius");
}

void ProfileXMLScale(xml::IrrXMLReader* xml)
{
	app.SetScale(xml->getAttributeValueAsFloat("dScale"));
}

void ProfileXMLScroll(xml::IrrXMLReader* xml)
{
	scroll::nHScrollThumbSize = xml->getAttributeValueAsInt("nHorzSize");
	scroll::nHScrollSens = xml->getAttributeValueAsInt("nHorzSensit");
	scroll::nVScrollThumbSize = xml->getAttributeValueAsInt("nVertSize");
	scroll::nVScrollSens = xml->getAttributeValueAsInt("nVertSensit");
}

void ProfileXMLUnits(xml::IrrXMLReader* xml)
{
	const char *szBuf;
	szBuf = xml->getAttributeValue("eType");
	if(!strcmp("architectural", szBuf))
		app.SetUnits(Architectural);
	else if(!strcmp("engineering", szBuf))
		app.SetUnits(Engineering);
	else if(!strcmp("feet", szBuf))
		app.SetUnits(Feet);
	else if(!strcmp("inches", szBuf))
		app.SetUnits(Inches);
	else if(!strcmp("meters", szBuf))
		app.SetUnits(Meters);
	else if(!strcmp("millimeters", szBuf))
		app.SetUnits(Millimeters);
	else if(!strcmp("centimeters", szBuf))
		app.SetUnits(Centimeters);
	else if(!strcmp("decimeters", szBuf))
		app.SetUnits(Decimeters);
	else
		app.SetUnits(Kilometers);

	app.SetUnitsPrec(xml->getAttributeValueAsInt("nPrecision"));
}

void ProfileXMLUserAxis(xml::IrrXMLReader* xml)
{
	UserAxis::UserAxis_dAxOffAng = xml->getAttributeValueAsFloat("dOffsetAngle");
	UserAxis::UserAxis_dInfAng = xml->getAttributeValueAsFloat("dInfluenceAngle");
}

void ProfileXMLView(xml::IrrXMLReader* xml)
{
	view::SetFarClipDistance(xml->getAttributeValueAsFloat("nFarClip"));
	view::SetNearClipDistance(xml->getAttributeValueAsFloat("nNearClip"));
	
	double dAngle = xml->getAttributeValueAsFloat("nUpRotAngle");
	view::SetVwUp(CVec(cos(dAngle * RADIAN), sin(dAngle * RADIAN), 0.));
}

void ProfileXMLPrint(int nIndex, xml::IrrXMLReader* xml)
{
	printStates.SetCurrentState(nIndex-1);

	if((nIndex == 3) || (nIndex == 4))
	{
		const char *szBuf;
		szBuf = xml->getAttributeValue("eSheetSize");
		if(!strcmp("FULL_SHEET", szBuf))
			printStates.SetSheetSize(FULL_SHEET);
		else if(!strcmp("HALF_SHEET", szBuf))
			printStates.SetSheetSize(HALF_SHEET);
		else //if(!strcmp("QUARTER_SHEET", szBuf))
			printStates.SetSheetSize(QUARTER_SHEET);
	}

	printStates.SetDispBkgImage(BoolXML(xml->getAttributeValue("bBackgroundImage")));
	printStates.SetDispLayer(BoolXML(xml->getAttributeValue("bFileTracingNames")));
	printStates.SetPrintMarkers(BoolXML(xml->getAttributeValue("bMarkers")));
	printStates.SetUsePenWidths(BoolXML(xml->getAttributeValue("bPenWidths")));
	printStates.SetTranslatePens(BoolXML(xml->getAttributeValue("bTranslatePens")));

	printStates.SetPenWidthsFile(xml->getAttributeValue("strPenWidthsFile"));
	printStates.SetTranslateFile(xml->getAttributeValue("strTranslatePensFile"));

	printStates.SetMarginX(xml->getAttributeValueAsFloat("dMarginsX"));
	printStates.SetMarginY(xml->getAttributeValueAsFloat("dMarginsY"));
}

void ProfileXMLAxonometric(xml::IrrXMLReader*)// xml)
{

}

void ProfileXMLImport(xml::IrrXMLReader*)// xml)
{

}

void ProfileXMLOblique(xml::IrrXMLReader*)// xml)
{

}

void ProfileXMLPerspective(xml::IrrXMLReader*)// xml)
{

}