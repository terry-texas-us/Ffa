#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysView.h"

#include "DlgProcTrapModify.h"
#include "Text.h"

#include "subprocnodal.h"
#include "StringExtra.h"

CRefSys *rsNBuff = 0;
CFontDef *fdNBuff = 0;

CPrimText::CPrimText(const CFontDef& fd, const CRefSys& rs, const char* pszText)
{
	m_nPenColor = pstate.PenColor();
	m_fd = fd;
	m_rs = rs;
	m_strText = pszText;
}

CPrimText::CPrimText(const CPrimText& src)
{
	m_nPenColor = src.m_nPenColor;
	m_fd = src.m_fd;
	m_rs = src.m_rs;
	m_strText = src.m_strText;
}

const CPrimText& CPrimText::operator=(const CPrimText& src)
{
	m_nPenColor = src.m_nPenColor;
	m_fd = src.m_fd;
	m_rs = src.m_rs;
	m_strText = src.m_strText;
	
	return (*this);
}

bool CPrimText::SelUsingLine(const CLine& ln, CPnts& ptsInt) 
{
	ptsInt.RemoveAll();

	if (m_strText.GetLength() == 0)
		return false;

	CPnts pts;
	CPnts temp;
	
	text_GetBoundingBox(m_fd, m_rs, m_strText.GetLength(), 0., pts);
	
	CPnt4 pt0[] = {CPnt4(pts[0], 1.), CPnt4(pts[1], 1.), CPnt4(pts[2], 1.), CPnt4(pts[3], 1.)};

	for (int n = 0; n < 4; n++)
	{
		polyline::BeginLineStrip();
		polyline::SetVertex(pt0[n]);
		polyline::SetVertex(pt0[n+1]);
		if(polyline::SelUsingLine(ln, ptsInt))
			temp.Add(ptsInt[0]);
	}

	polyline::BeginLineStrip();
	polyline::SetVertex(pt0[3]);
	polyline::SetVertex(pt0[0]);
	if(polyline::SelUsingLine(ln, ptsInt))
		temp.Add(ptsInt[0]);

	return (!temp.IsEmpty());
}

CPrim*& CPrimText::Copy(CPrim*& pPrim) const
{
	pPrim = new CPrimText(*this);
	return (pPrim);
}

void CPrimText::Display(CDC* pDC, bool bNodeDisplay)
{
	PENCOLOR nPenColor = LogicalPenColor();
	PENSTYLE nPenStyle = pstate.PenStyle();

	pstate.SetPen(pDC, nPenColor, 1);

	text_Display0(pDC, m_fd, m_rs, m_strText);
	
	//pstate.SetPenStyle(nPenStyle);
	pstate.SetPen(pDC, nPenColor, nPenStyle);

	//CPrimLine *pNewLine = new CPrimLine(1, 2, 

	if(bNodeDisplay)
	{
		CPnts pts;
		GetAllPts(pts);

		if((app.m_iModeId == ID_MODE_NODAL) || (app.m_iModeId == ID_MODE_NODALR))
		{
			CPrim *pPrim = (CPrim*)this;
			app.DisplayPoint(pDC, &pts, nodal::GetPrimMask(pPrim));
		}
		else
			app.DisplayPoint(pDC, &pts, 0);
	}
}

void CPrimText::DisplayOpenGL(bool)
{
	//PENCOLOR nPenColor = LogicalPenColor();
	//opengl::SetCurrentColor(app.PenColorsGetHot(nPenColor));

	//text_Display0(0, m_fd, m_rs, m_strText);
}

void CPrimText::DisRep(const CPnt&) const
{
	CString str;
	str.Format("Color: %s Font: %s Precision: %s Path: %s Alignment: (%s,%s)",
		FormatPenColor(), m_fd.TextFont(), m_fd.FormatTextPrec(), m_fd.FormatTextPath(), m_fd.FormatTextHorAlign(), m_fd.FormatTextVerAlign());
	msgInformation(str);
}

void CPrimText::FormatExtra(CString& str) const
{
	str.Format("Text;%s\tColor;%s\tFont;%s\tPrecision;%s\tPath;%s\tAlignment;(%s,%s)\tSpacing;%f\tLength;%d",
		m_strText, FormatPenColor(), m_fd.TextFont(), m_fd.FormatTextPrec(), m_fd.FormatTextPath(), m_fd.FormatTextHorAlign(), 
		m_fd.FormatTextVerAlign(), m_fd.ChrSpac(), m_strText.GetLength());
}

void CPrimText::FormatGeometry(CString& str) const
{
	str += "Origin;" + m_rs.Origin().ToString();
	str += "X Axis;" + m_rs.DirX().ToString();
	str += "Y Axis;" + m_rs.DirY().ToString();
}

///<summary>Get the bounding box of text.</summary>
void CPrimText::GetBoundingBox(CPnts& ptsBox, double dSpacFac) const
{
	text_GetBoundingBox(m_fd, m_rs, m_strText.GetLength(), dSpacFac, ptsBox);
}

CPnt CPrimText::GetCtrlPt() const
{
	return m_rs.Origin();
}

///<summary>Determines the extent.</summary>
void CPrimText::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
{
	CPnts pts;
		
	text_GetBoundingBox(m_fd, m_rs, m_strText.GetLength(), 0., pts);
	
	for (WORD w = 0; w < pts.GetSize(); w++)
	{
		mspace.Transform(pts[w]);
		pts[w] = tm * pts[w];
		ptMin = Min(ptMin, pts[w]);
		ptMax = Max(ptMax, pts[w]);
	}
}

void CPrimText::Iron()
{
	CPnt pnt_ = m_rs.Origin(); 
	pnt_[2] = 0;
	m_rs.SetOrigin(pnt_);

	CVec xDir = m_rs.DirX();
	CVec yDir = m_rs.DirY();
	xDir[2] = 0;
	yDir[2] = 0;
	m_rs.SetDirX(xDir);
	m_rs.SetDirY(yDir);
}

bool CPrimText::IsInView() const
{
	CPnt4 pt[2];
	
	CPnts pts;
	
	text_GetBoundingBox(m_fd, m_rs, m_strText.GetLength(), 0., pts);
	
	for (size_t n = 0; n <= 2; ) 
	{
		pt[0] = pts[n++];
		pt[1] = pts[n++];

		ModelView_DoTransform(2, pt);
		
		if (Pnt4_ClipLine(pt[0], pt[1]))
			return true;
	}
	return false;
}

bool CPrimText::SelUsingRect(const CPnt& pt1, const CPnt& pt2)
{
	CPnts pts;
	text_GetBoundingBox(m_fd, m_rs, m_strText.GetLength(), 0., pts);
	return polyline::SelUsingRect(pt1, pt2, pts);
}

bool CPrimText::IsPtACtrlPt(const CPnt4& ptPic) const
{
	CPnt4 pt(m_rs.Origin(), 1.);
	ModelView_DoTransform(pt);

	return ((Pnt4_DistanceTo_xy(ptPic, pt) < mS_dPicApertSiz) ? true : false);
}

// called when "n" is pressed
void CPrimText::ModifyState()
{
	CPrim::ModifyState();
	
	CFontDef fd;
	pstate.GetFontDef(fd);
	
	CCharCellDef ccd;
	pstate.GetCharCellDef(ccd);

//	m_nPenColor = pstate.PenColor();
	m_fd.TextNoteModifySet(fd);

	m_rs.Rescale(ccd);
}

void CPrimText::GetState()
{
	CPrim::GetState();

	fdNBuff = new CFontDef(m_fd);
	rsNBuff = new CRefSys(m_rs);
}

void CPrimText::ModifyNotes(const CFontDef& fd, const CCharCellDef& ccd, int iAtt)
{
	// trap modify note
	if (iAtt == TM_TEXT_ALL)
	{
		m_nPenColor = pstate.PenColor();

		// mak - text formatting
		m_fd.TextNoteModifySet(fd);

		m_rs.Rescale(ccd);
	}
	// trap modify font
	else if (iAtt == TM_TEXT_FONT)
	{
		m_fd.TextFontSet(fd.TextFont());
		m_fd.TextPrecSet(fd.TextPrec());
	}
	// trap modify height
	else if (iAtt == TM_TEXT_HEIGHT)
	{
		m_fd.ChrSpacSet(fd.ChrSpac());
		m_fd.TextPathSet(fd.TextPath());
		
		m_rs.Rescale(ccd);
	}
}

CPnt CPrimText::SelAtCtrlPt(const CPnt4& ptPicVw) const
{
	mS_wCtrlPt = USHRT_MAX;
	return (ptPicVw);
}

///<summary>Evaluates whether a point lies within the bounding region of text.</summary>
bool CPrimText::SelUsingPoint(const CPnt4& ptView, double, CPnt& ptProj) 
{
	if (m_strText.GetLength() == 0)
		return false;

	CPnts pts;
	text_GetBoundingBox(m_fd, m_rs, m_strText.GetLength(), 0., pts);
	
	CPnt4 pt0[] = {CPnt4(pts[0], 1.), CPnt4(pts[1], 1.), CPnt4(pts[2], 1.), CPnt4(pts[3], 1.)};
	ModelView_DoTransform(4, pt0);

	if((m_rs.DirX()[0] > 0) && (m_rs.DirY()[1] < 0))
	{
		for(size_t n = 0; n < 4; n++)
		{
			if (CLine(pt0[n], pt0[(n + 1) % 4]).DirRelOfPt(ptView) >= 0)
				return false;
		}
		ptProj = ptView;
		return true;
	}
	else if((m_rs.DirX()[0] < 0) && (m_rs.DirY()[1] > 0))
	{
		for(size_t n = 0; n < 4; n++)
		{
			if (CLine(pt0[n], pt0[(n + 1) % 4]).DirRelOfPt(ptView) >= 0)
				return false;
		}
		ptProj = ptView;
		return true;
	}
	else
	{
		for(size_t n = 0; n < 4; n++)
		{
			if (CLine(pt0[n], pt0[(n + 1) % 4]).DirRelOfPt(ptView) < 0)
				return false;
		}
		ptProj = ptView;
		return true;
	}

	///////////////////////////////////////////////////////////////////////////////////
	//if (m_strText.GetLength() == 0)
	//	return false;

	//CPnts pts;
	//
	//text_GetBoundingBox(m_fd, m_rs, m_strText.GetLength(), 0., pts);
	//
	//CPnt4 pt0[] = {CPnt4(pts[0], 1.), CPnt4(pts[1], 1.), CPnt4(pts[2], 1.), CPnt4(pts[3], 1.)};

	//ModelView_DoTransform(4, pt0);

	//for (size_t n = 0; n < 4; n++)
	//{
	//	if (CLine(pt0[n], pt0[(n + 1) % 4]).DirRelOfPt(ptView) < 0)
	//		return false;
	//}
	//ptProj = ptView;

	//return true;
}

void CPrimText::ReadPeg(HFILE hFile)
{
	CPrim::ReadPeg(hFile);
	
	m_fd.Read(hFile);
	m_rs.Read(hFile);
	
	FilePeg_ReadString2(hFile, m_strText);

//	CString strText;
//	FilePeg_ReadString2(hFile, strText);

//	if(m_strText[0] == '4')
//	{
//		int a;
//		a = 0;
//	}

	//for(int i = 0; i < strText.GetLength(); i++)
	//{
	//	if(strText[i] >= 256)
	//		strText.SetAt(i, 'a');
	//}

	//m_strText = strText;
}

void CPrimText::Transform(const CTMat& tm)
{
	m_rs.Transform(tm);
}

void CPrimText::TranslateUsingMask(const CVec& v, const DWORD dwMask)
{
	if (dwMask != 0)
		m_rs.SetOrigin(m_rs.Origin() + v);
}

CSeg *VectorizeCharacter(const CFontDef& fd, const CTMat& tm, int nOff, int nLen, const CString& strText)
{
	if (nLen == 0) return 0;

	long* plStrokeFontDef = (long*) app.StrokeFontGet();
	if (plStrokeFontDef == 0) return 0;

	long* plStrokeChrDef = plStrokeFontDef + 96;
	double dChrSpac = 1. + (.32 + fd.ChrSpac()) / .6;

	CPnt ptStroke = ORIGIN;
	CPnt ptChrPos = ptStroke;
	CPnt ptLinePos = ptChrPos;

	int n = nOff;

	//mak - 
	bool bBegin = true;
	CPnt First, Last;
	CSeg *pSeg = new CSeg;

	while (n < nOff + nLen)
	{
		bBegin = true;
		char c = isprint(strText[n]) ? strText[n] : '.';
				
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
				bBegin = true;
			}

			/////////////////////////////////////////////////
			if(bBegin)
			{
				First = tm*ptStroke;
				bBegin = false;
			}
			else
			{
				Last = tm*ptStroke;
				pSeg->AddTail(new CPrimLine(First, Last));
				First = Last;
			}
		}
		////////////////////////////////////////////////////////////////
		
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

	return pSeg;
}

bool CPrimText::Vectorize(const CSeg *pSeg)
{
	POSITION curPos = pSeg->Find(this);

	if(curPos == 0)
		return false;

	/////////////////////////////////////////////////////////////////////////////////////

	CRefSys rs = m_rs;
	CString strText = m_strText;
	CFontDef fd = m_fd;

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

							rs.SetOrigin(text_GetNewLinePos(fd, rs, - .35, nLen * (1 + .32 / .6)));
							ptRef = rs.Origin();
							tm = rs.GetTransformMatrix(); tm.Inverse();

							nLen = nNumEndDel - n;

							rs.SetOrigin(text_GetNewLinePos(fd, rs, .35, nLen * (1 + .32 / .6) - .72));
							ptRef = rs.Origin();
							tm = rs.GetTransformMatrix(); tm.Inverse();

							n = nNumEndDel + 1;
							rs.SetOrigin(text_GetNewLinePos(fd, rs, .35, .72));
							ptRef = rs.Origin();
							tm = rs.GetTransformMatrix(); tm.Inverse();
	
							nLen = nCmdEndDel - n;
							
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
		else
			++nEnd;
	}

	CSeg *pSegTemp = VectorizeCharacter(fd, tm, nOff, nEnd - nOff, strText);

	//////////////////////////////////////////////////////////////////////////////////////////////

	if(pSegTemp != 0)
	{
		POSITION pos = pSegTemp->GetHeadPosition();
		while(pos != 0)
		{
			CPrim *pPrim = pSegTemp->GetNext(pos);
			pPrim->SetPenColor(m_nPenColor);
			((CObList*)pSeg)->InsertAfter(curPos, pPrim);
		}
	}

	return true;
}

bool CPrimText::WritePeg(HFILE hFile) const
{
	FilePeg_WriteWord2(hFile, PRIM_TEXT);
	FilePeg_WriteWord2(hFile, m_nPenColor);
	FilePeg_WriteWord2(hFile, m_nPenStyle);
	m_fd.Write(hFile);
	m_rs.Write(hFile);
	FilePeg_WriteString2(hFile, m_strText);

	return true;
}