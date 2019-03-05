#include "stdafx.h"

#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

PENCOLOR	CPrim::mS_nLayerPenColor = 1;
PENSTYLE	CPrim::mS_nLayerPenStyle = 1;
PENSTYLE	CPrim::mS_nSpecPenStyle = 0;
PENCOLOR	CPrim::mS_nSpecPenColor = 0;
short		CPrim::mS_nSpecPolygonStyle = - 1;

WORD		CPrim::mS_wCtrlPt = USHRT_MAX;
double		CPrim::mS_dRel = 0.;
double		CPrim::mS_dPicApertSiz = .02;

CString CPrim::FormatPenColor() const
{
	CString str;	
	if (m_nPenColor == PENCOLOR_BYLAYER)
		str = "ByLayer";
	else if (m_nPenColor == PENCOLOR_BYBLOCK) 
		str = "ByBlock";
	else 
	{
		char szBuf[16]; 
		str = _itoa(m_nPenColor, szBuf, 10);
	}
	return str;
}	

CString CPrim::FormatPenStyle() const
{
	CString str;
	if (m_nPenStyle == PENSTYLE_BYLAYER) 
		str = "ByLayer";
	else if (m_nPenStyle == PENSTYLE_BYBLOCK) 
		str = "ByBlock";
	else 
	{
		char szBuf[16];
		str = _itoa(m_nPenStyle, szBuf, 10);
	}
	return str;
}	

PENCOLOR CPrim::LogicalPenColor() const
{
	PENCOLOR nPenColor = m_nPenColor;

	if(CPegView::GetActiveView()->IsColorByLayerOn())
		nPenColor = mS_nLayerPenColor;
	else if(mS_nSpecPenColor != 0)
		nPenColor = mS_nSpecPenColor;

	if(nPenColor == PENCOLOR_BYLAYER) 
		nPenColor = mS_nLayerPenColor;
	else if(nPenColor == PENCOLOR_BYBLOCK)
		nPenColor = 7;

	return (nPenColor);

	//////////////////////////////////////////////////////////////////////////////

	//nPenColor = mS_nSpecPenColor == 0 ? m_nPenColor : mS_nSpecPenColor;

	//if (nPenColor == PENCOLOR_BYLAYER) 
	//	nPenColor = mS_nLayerPenColor;
	//else if (nPenColor == PENCOLOR_BYBLOCK)
	//	nPenColor = 7;

	//return (nPenColor);
}

PENSTYLE CPrim::LogicalPenStyle() const
{
	PENSTYLE nPenStyle = mS_nSpecPenStyle == 0 ? m_nPenStyle : mS_nSpecPenStyle;
	
	if (nPenStyle == PENSTYLE_BYLAYER) 
		nPenStyle = mS_nLayerPenStyle;
	else if (nPenStyle == PENSTYLE_BYBLOCK) 
		nPenStyle = 1;
	
	return (nPenStyle);
}

void CPrim::ModifyState() 
{
	m_nPenColor = pstate.PenColor();
	m_nPenStyle = pstate.PenStyle();
}

void CPrim::GetState()
{
	if(m_nPenColor != 0)
	{
		pstate.SetPenColor(m_nPenColor);
		app.StatusLineDisplay(Pen);
	}

	if(m_nPenStyle != 0)
	{
		pstate.SetPenStyle(m_nPenStyle);
		app.StatusLineDisplay(Line);
	}	
}

void CPrim::ReadPeg(HFILE hFile)
{
	short sColor, sStyle;

//	FilePeg_ReadShort2(hFile, m_nPenColor);
//	FilePeg_ReadShort2(hFile, m_nPenStyle);

	FilePeg_ReadShort2(hFile, sColor);
	FilePeg_ReadShort2(hFile, sStyle);

	if((sColor < 256) && (sColor >= 0))
		m_nPenColor = sColor;
	else
		m_nPenColor = 1;

	if((sStyle < CPegDoc::GetDoc()->PenStylesGetSize()) && (sStyle >= 0))
		m_nPenStyle = sStyle;
	else
		m_nPenStyle = 1;
}

CVec Prim_ComputeArbitraryAxis(const CVec& vNorm)
{
	// Compute a not so arbitrary axis for AutoCAD entities
	const double e = 1. / 64;
	
	if ((fabs(vNorm[0]) < e) && (fabs(vNorm[1]) < e))
		return (YDIR ^ vNorm);
	else
		return (ZDIR ^ vNorm);
}

bool CPrim::IsSupportedTyp(int iTyp) 
{
//	return(iTyp <= 7 && iTyp != 3 && iTyp != 4 && iTyp != 5);
	return (iTyp == 1);
}