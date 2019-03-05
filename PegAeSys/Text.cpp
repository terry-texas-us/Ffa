#include "Stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysView.h"

#include "StringExtra.h"
#include "Text.h"

void text_Display0(CDC* pDC, const CFontDef& fd, const CRefSys& rs0, const CString& strText)
{
	CPegView* pView = CPegView::GetActiveView();
		
	if (strText.GetLength() > 0) 
	{
		CRefSys rs = rs0;
		bool bTrueType = pDC != 0 && fd.TextPrec() == CFontDef::PREC_TRUETYPEFONT && pView->GetViewTrueTypeFonts();
		int nLen = string_LengthSansEscSeqs(strText);

		CTMat tm(rs.GetTransformMatrix()); tm.Inverse();

		CPnt ptRef;
		text_GetBottomLeft(fd, nLen, ptRef);
		ptRef = tm * ptRef;
		rs.SetOrigin(ptRef);
		tm = rs.GetTransformMatrix(); tm.Inverse();

		int nOff = 0;
		int nEnd = nOff;
		while (nEnd < strText.GetLength())
		{
			char c = strText[nEnd];
			if (c == '\r')
			{
				if (bTrueType)	
					text_Display(pDC, fd, rs, strText.Mid(nOff, nEnd - nOff));
				else
					text_Display(pDC, fd, tm, nOff, nEnd - nOff, strText);
				
				if (strText[++nEnd] == '\n') ++nEnd;
				nOff = nEnd;
				rs.SetOrigin(ptRef);
                rs.SetOrigin(text_GetNewLinePos(fd, rs, 1., 0));
				ptRef = rs.Origin();
				tm = rs.GetTransformMatrix(); tm.Inverse();
			}
			else if (c == '^')
			{
				++nEnd;
				int n = nEnd;
				
				int nCmdEndDel = strText.Find('^', n);
				if (nCmdEndDel != - 1)
				{
					if (strText[n++] == '/')
					{
						int nNumEndDel = strText.Find('/', n);
						if (nNumEndDel != - 1) 
						{
							if (nNumEndDel < nCmdEndDel)
							{
								int nLen = nEnd - nOff - 1;
								if (bTrueType)	
									text_Display(pDC, fd, rs, strText.Mid(nOff, nLen));
								else
									text_Display(pDC, fd, tm, nOff, nLen, strText);
								rs.SetOrigin(text_GetNewLinePos(fd, rs, - .35, nLen * (1 + .32 / .6)));
								ptRef = rs.Origin();
								tm = rs.GetTransformMatrix(); tm.Inverse();

								nLen = nNumEndDel - n;
								if (bTrueType)	
									text_Display(pDC, fd, rs, strText.Mid(n, nLen));
								else
									text_Display(pDC, fd, tm, n, nLen, strText);
								rs.SetOrigin(text_GetNewLinePos(fd, rs, .35, nLen * (1 + .32 / .6) - .72));
								ptRef = rs.Origin();
								tm = rs.GetTransformMatrix(); tm.Inverse();
								
								if (bTrueType)	
									text_Display(pDC, fd, rs, strText.Mid(nNumEndDel, 1));
								else
									text_Display(pDC, fd, tm, nNumEndDel, 1, strText);
								n = nNumEndDel + 1;
								rs.SetOrigin(text_GetNewLinePos(fd, rs, .35, .72));
								ptRef = rs.Origin();
								tm = rs.GetTransformMatrix(); tm.Inverse();
		
								nLen = nCmdEndDel - n;
								if (bTrueType)	
									text_Display(pDC, fd, rs, strText.Mid(n, nLen));
								else
									text_Display(pDC, fd, tm, n, nLen, strText);
								
								rs.SetOrigin(text_GetNewLinePos(fd, rs, - .35, nLen * (1 + .32 / .6)));
								ptRef = rs.Origin();
								tm = rs.GetTransformMatrix(); tm.Inverse();
								nOff = nCmdEndDel + 1;
								nEnd = nOff;
							}
						}
					}
				}
			}
			// TODO autocad escapes
			//else if (c == '\\')
			//{
			//}
			else
				++nEnd;
		}
		if (bTrueType)	
			text_Display(pDC, fd, rs, strText.Mid(nOff, nEnd - nOff));
		else
			text_Display(pDC, fd, tm, nOff, nEnd - nOff, strText);
	}
}

///<summary>Displays a text string using a simple stroke font.</summary>
void text_Display(CDC* pDC, const CFontDef& fd, const CTMat& tm, int nOff, int nLen, const CString& strText)
{
	if (nLen == 0) return;

	long* plStrokeFontDef = (long*) app.StrokeFontGet();
	if (plStrokeFontDef == 0) return;

	long* plStrokeChrDef = plStrokeFontDef + 96;
	double dChrSpac = 1. + (.32 + fd.ChrSpac()) / .6;

	CPnt ptStroke = ORIGIN;
	CPnt ptChrPos = ptStroke;
	CPnt ptLinePos = ptChrPos;

	int n = nOff;

	while (n < nOff + nLen)
	{
		polyline::BeginLineStrip();

		// the +/- character was crashing this function
//		char c = isprint(strText[n]) ? strText[n] : '.';
		char c; 
		if((strText[n] >= 0x20) && (strText[n] <= 0x7E))
			c = strText[n];
		else
			c = '.';
				
		for (int i = (int) plStrokeFontDef[c - 32]; i <= plStrokeFontDef[c - 32 + 1] - 1; i++) 
		{
			int iY = (int) (plStrokeChrDef[i - 1] % 4096L);
			if ((iY & 2048) != 0) 
				iY = - (iY - 2048);
			int iX = (int) ((plStrokeChrDef[i - 1] / 4096L) % 4096L);
			if ((iX & 2048) != 0) 
				iX = - (iX - 2048);
			
			ptStroke += CVec(.01 / .6 * iX, .01 * iY, 0.);
			
			if (plStrokeChrDef[i - 1] / 16777216 == 5)
			{
				polyline::End(pDC, 1);
				polyline::BeginLineStrip();
			}

			// MAK - stray segment fix?
			//polyline::SetVertex(tm * ptStroke);
			//////////////////////////////////////////////////////
			polyline::SetVertex(tm * ptStroke);
		}
		polyline::End(pDC, 1);
		
		switch (fd.TextPath())
		{
			case CFontDef::PATH_LEFT:
				ptChrPos[0] -= dChrSpac;
				break;
			case CFontDef::PATH_UP:
				ptChrPos[1] += dChrSpac;
				break;
			case CFontDef::PATH_DOWN:
				ptChrPos[1] -= dChrSpac;
				break;
			default:
				ptChrPos[0] += dChrSpac;
		}
		ptStroke = ptChrPos;
		n++;
	}
}

///<summary>Attempts to display text is using true type font.</summary>
bool text_Display(CDC* pDC, const CFontDef& fd, const CRefSys& rs, const CString& strText)
{
	int nSiz = strText.GetLength();
	if (nSiz == 0)
		return true;

	//UINT HorAlign, VerAlign;

	//// mak - text fix... hopefully
	//if(fd.TextHorAlign() == CFontDef::HOR_ALIGN_LEFT)
	//	HorAlign = TA_LEFT;
	//else if(fd.TextHorAlign() == CFontDef::HOR_ALIGN_RIGHT)
	//	HorAlign = TA_RIGHT;
	//else //if(fd.TextHorAlign() == CFontDef::HOR_ALIGN_CENTER)
	//	HorAlign = TA_CENTER;

	//if(fd.TextVerAlign() == CFontDef::VER_ALIGN_TOP)
	//	VerAlign = TA_TOP;
	//else if(fd.TextVerAlign() == CFontDef::VER_ALIGN_BOTTOM)
	//	VerAlign = TA_BOTTOM;
	//else //if(fd.TextVerAlign() == CFontDef::VER_ALIGN_MIDDLE)
	//	VerAlign = TA_BASELINE;

	CPnt4 ptPvt(rs.Origin(), 1.);
	CVec4 vXAxRef(rs.DirX(), 0.);
	CVec4 vYAxRef(rs.DirY(), 0.);
	
	ModelView_DoTransform(ptPvt);
	ModelView_DoTransform(vXAxRef);
	ModelView_DoTransform(vYAxRef);
	
	CVec vPlnNorm = CVec(vXAxRef) ^ CVec(vYAxRef);
	vPlnNorm.Normalize();

	if (vPlnNorm != ZDIR)
		return false;

	// text local system plane is parallel to projection plane

	CPoint pnt[4];
	CPnts ptsBox;
	text_GetBoundingBox(fd, rs, strText.GetLength(), 0., ptsBox);

	CPnt4 pt[] = {CPnt4(ptsBox[0], 1.), CPnt4(ptsBox[1], 1.), CPnt4(ptsBox[2], 1.), CPnt4(ptsBox[3], 1.)};
	ModelView_DoTransform(4, pt);
	viewport::DoProjection(pnt, 4, pt);

	CVec vX(double(pnt[1].x - pnt[0].x), double(pnt[1].y - pnt[0].y), 0.); 
	CVec vY(double(pnt[3].x - pnt[0].x), double(pnt[3].y - pnt[0].y), 0.); 
	
	double dHeight = vY.Length();
	if (dHeight == 0.)
	{
		return true;
	}
	double dOffAng = Vec_Angle_xy(vX);
	int iAng = Round(dOffAng / RADIAN * 10.);
			
	LOGFONT logfont;
	logfont.lfHeight = - Round(1.33 * dHeight);
	logfont.lfWidth = 0;
	logfont.lfEscapement = - iAng;
	logfont.lfOrientation = - iAng;
	logfont.lfWeight = FW_THIN;
	logfont.lfItalic = FALSE;
	logfont.lfUnderline = FALSE;
	logfont.lfStrikeOut = FALSE;
	logfont.lfCharSet = DEFAULT_CHARSET; //ANSI_CHARSET;
	logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfQuality = CLEARTYPE_QUALITY;
	logfont.lfPitchAndFamily = DEFAULT_PITCH;
	strcpy(logfont.lfFaceName, fd.TextFont());

	CFont font;
	font.CreateFontIndirect(&logfont);
	CFont* pfntold = (CFont*) pDC->SelectObject(&font);
	
	
//	UINT uTextAlign = pDC->SetTextAlign(HorAlign | VerAlign);
	UINT uTextAlign = pDC->SetTextAlign(TA_BASELINE | TA_LEFT);
	int iBkMode = pDC->SetBkMode(TRANSPARENT);
	
	pDC->TextOut(pnt[0].x, pnt[0].y, (LPCSTR) strText, nSiz);
		
	pDC->SetBkMode(iBkMode);
	pDC->SetTextAlign(uTextAlign);

	pDC->SelectObject(pfntold);

	return true;
}

///<summary>
///Determines the offset to the bottom left alignment position of a string
/// of the specified number of characters and text attributes in the z=0 plane.
///</summary>
void text_GetBottomLeft(const CFontDef& fd, int iChrs, CPnt& pt)
{
	if (iChrs > 0) 
	{
		double dTxtExt = iChrs + (iChrs - 1) * (.32 + fd.ChrSpac()) / .6;
		
		if (fd.TextPath() == CFontDef::PATH_RIGHT || fd.TextPath() == CFontDef::PATH_LEFT)
		{	
			if (fd.TextPath() == CFontDef::PATH_RIGHT)
			{	
				if (fd.TextHorAlign() == CFontDef::HOR_ALIGN_LEFT)
					pt[0] = 0.;
				else if (fd.TextHorAlign() == CFontDef::HOR_ALIGN_CENTER)
					pt[0] = - dTxtExt * .5;
				else if (fd.TextHorAlign() == CFontDef::HOR_ALIGN_RIGHT)
					pt[0] = - dTxtExt;
			}
			else
			{	
				if (fd.TextHorAlign() == CFontDef::HOR_ALIGN_LEFT)
					pt[0] = dTxtExt;
				else if (fd.TextHorAlign() == CFontDef::HOR_ALIGN_CENTER)
					pt[0] = dTxtExt * .5;
				else if (fd.TextHorAlign() == CFontDef::HOR_ALIGN_RIGHT)
					pt[0] = 0.;
				pt[0] = pt[0] - 1.;
			}
			if (fd.TextVerAlign() == CFontDef::VER_ALIGN_BOTTOM)
				pt[1] = 0.;
			else if (fd.TextVerAlign() == CFontDef::VER_ALIGN_MIDDLE)
				pt[1] = - .5;
			else if (fd.TextVerAlign() == CFontDef::VER_ALIGN_TOP)
				pt[1] = - 1.;
		}
		else if (fd.TextPath() == CFontDef::PATH_DOWN || fd.TextPath() == CFontDef::PATH_UP)
		{	
			if (fd.TextHorAlign() == CFontDef::HOR_ALIGN_LEFT)
				pt[0] = 0.;
			else if (fd.TextHorAlign() == CFontDef::HOR_ALIGN_CENTER)
				pt[0] = - .5;
			else if (fd.TextHorAlign() == CFontDef::HOR_ALIGN_RIGHT)
				pt[0] = - 1.;
			if (fd.TextPath() == CFontDef::PATH_UP)
			{	
				if (fd.TextVerAlign() == CFontDef::VER_ALIGN_BOTTOM)
					pt[1] = 0.;
				else if (fd.TextVerAlign() == CFontDef::VER_ALIGN_MIDDLE)
					pt[1] = - dTxtExt * .5;
				else if (fd.TextVerAlign() == CFontDef::VER_ALIGN_TOP)
					pt[1] = - dTxtExt;
			}
			else
			{	
				if (fd.TextVerAlign() == CFontDef::VER_ALIGN_BOTTOM)
					pt[1] = dTxtExt;
				else if (fd.TextVerAlign() == CFontDef::VER_ALIGN_MIDDLE)
					pt[1] = dTxtExt * .5;
				else if (fd.TextVerAlign() == CFontDef::VER_ALIGN_TOP)
					pt[1] = 0.;
				pt[1] = pt[1] - 1.;
			}
		}
	}
	else 
	{
		pt[0] = 0.;
		pt[1] = 0.;
	}
	pt[2] = 0.;
}

///<summary>Returns the region boundaries of a text string applying and optional inflation factor.</summary>
void text_GetBoundingBox(const CFontDef& fd, const CRefSys& cs, int nLen, double dSpacFac, CPnts& ptsBox)
{
	ptsBox.SetSize(4);

	if (nLen > 0) 
	{
		double dChrSpac = (.32 + fd.ChrSpac()) / .6;
		
		CTMat tm = cs.GetTransformMatrix();
		tm.Inverse();
		
		double dTxtHgt = 1.;
		double dTxtWid = 1.;
		double d = (double) nLen + dChrSpac * (double) (nLen - 1);
		
		if (fd.TextPath() == CFontDef::PATH_RIGHT || fd.TextPath() == CFontDef::PATH_LEFT)
			dTxtWid = d;
		else
			dTxtHgt = d;
		
		ptsBox[0] = ORIGIN; 
		ptsBox[1] = ptsBox[0]; 
		ptsBox[2] = ptsBox[0]; 
		ptsBox[3] = ptsBox[0];
	
		if (fd.TextHorAlign() == CFontDef::HOR_ALIGN_LEFT)
			ptsBox[2][0] = dTxtWid;
		else if (fd.TextHorAlign() == CFontDef::HOR_ALIGN_CENTER)
		{
			ptsBox[0][0] = - dTxtWid * .5; 
			ptsBox[2][0] = ptsBox[0][0] + dTxtWid;
		}
		else
			ptsBox[0][0] = - dTxtWid;
		
		if (fd.TextVerAlign() == CFontDef::VER_ALIGN_TOP)
			ptsBox[0][1] = - dTxtHgt;
		else if (fd.TextVerAlign() == CFontDef::VER_ALIGN_MIDDLE)
		{
			ptsBox[0][1] = - dTxtHgt * .5; 
			ptsBox[2][1] = ptsBox[0][1] + dTxtHgt;
		}
		else
			ptsBox[2][1] = dTxtHgt;
		
		if (dSpacFac > DBL_EPSILON) 
		{
			ptsBox[0][0] -= dSpacFac / .6; 
			ptsBox[0][1] -= dSpacFac;
			ptsBox[2][0] += dSpacFac / .6; 
			ptsBox[2][1] += dSpacFac;
		}
		ptsBox[1][0] = ptsBox[2][0]; 
		ptsBox[1][1] = ptsBox[0][1];
		ptsBox[3][0] = ptsBox[0][0]; 
		ptsBox[3][1] = ptsBox[2][1];
		
		for (int n = 0; n < 4; n++)
			ptsBox[n] = tm * ptsBox[n];
	}
	else
		for (int n = 0; n < 4; n++)
			ptsBox[n] = cs.Origin();
}

CPnt text_GetNewLinePos(const CFontDef& fd, const CRefSys& rs, double dLineSpaceFac, double dChrSpaceFac)
{
	CPnt pt = rs.Origin();
	CVec vPath(rs.DirX());

	if (fd.TextPath() == CFontDef::PATH_RIGHT || fd.TextPath() == CFontDef::PATH_LEFT)
	{
		pt += vPath * dChrSpaceFac;
		CVec vRefNorm;
		rs.GetUnitNormal(vRefNorm);
		
		vPath.Normalize();
		vPath *= - (rs.DirY().Length() * dLineSpaceFac);
		vPath.RotAboutArbAx(vRefNorm, HALF_PI);
	}
	return (pt + (vPath * 1.5));
}