#include "stdafx.h"

#include "..\\CBitmapEx\\BitmapEx.h"

#include "PegAEsys.h"
#include "PegAEsysView.h"
#include "PegAEsysDoc.h"

#include "FileBitmap.h"

//#include "ddeGItms.h"
//#include "UnitsString.h"

#include "subprocnodal.h"

CPrimBitmap::CPrimBitmap(CPnt ptPos)
{
}

CPrimBitmap::CPrimBitmap(CPnt ptPos, HBITMAP hBitmap)
{
	Init(ptPos, hBitmap);
}

void CPrimBitmap::Init(CPnt ptPos, HBITMAP hBitmap)
{
	m_ptUL[0] = ptPos[0];
	m_ptUL[1] = ptPos[1];
	m_ptUL[2] = 0;

	m_bm.Attach(hBitmap);
	BITMAP bm;
	m_bm.GetBitmap(&bm);

	m_bitmapEx.Load(hBitmap);

	// set LR
	CPnt4 pntUL(m_ptUL, 1.);
	ModelView_DoTransform(pntUL);
	CPoint ptUL = viewport::DoProjection(pntUL);
	double dRatio = vpActive.GetWidthInInches() / view::GetUExt();
	int iWidDst = (int)(bm.bmWidth * dRatio);
	int iHgtDst = (int)(bm.bmHeight * dRatio);
	CPnt pnt22(ptUL.x + iWidDst, ptUL.y + iHgtDst, 0);
	viewport::DoProjectionInverse(pnt22);
	m_ptLR = view::GetMatrixInverse() * pnt22;

	m_ptUR[0] = m_ptLR[0];
	m_ptUR[1] = m_ptUL[1];
	m_ptUR[2] = 0;

	m_ptLL[0] = m_ptUL[0];
	m_ptLL[1] = m_ptLR[1];
	m_ptLL[2] = 0;

	m_dScaleX = 1;
	m_dScaleY = 1;
	m_dRotation = 0;

	m_size.cx = bm.bmWidth;
	m_size.cy = bm.bmHeight;

	m_ptUL_ORIGINAL = m_ptUL;
	m_ptLR_ORIGINAL = m_ptLR;
	m_ptUR_ORIGINAL = m_ptUR;
	m_ptLL_ORIGINAL = m_ptLL;
	m_size_ORIGINAL = m_size;
	m_bm_ORIGINAL.Attach(hBitmap);

	m_bActLikePrim = true;

	BMIH.biSize = sizeof(BITMAPINFOHEADER);
	BMIH.biBitCount = 24;
	BMIH.biPlanes = 1;
	BMIH.biCompression = BI_RGB;
	BMIH.biWidth = m_size.cx;
	BMIH.biHeight = m_size.cy;
	BMIH.biSizeImage = ((((BMIH.biWidth * BMIH.biBitCount) + 31) & ~31) >> 3) * BMIH.biHeight;
}

CPrimBitmap::CPrimBitmap(CPnt ptPos, CString strFilename)
{
	m_ptUL[0] = ptPos[0];
	m_ptUL[1] = ptPos[1];
	m_ptUL[2] = 0;
}

CPrimBitmap::CPrimBitmap(CPnt* pt)
{
	m_ptUL = *pt;
}

CPrimBitmap::CPrimBitmap(const CPrimBitmap& src)
{
	m_ptUL = src.m_ptUL;
	m_ptLR = src.m_ptLR;
	m_ptUR = src.m_ptUR;
	m_ptLL = src.m_ptLL;

	m_size = src.m_size;

	m_dScaleX = src.m_dScaleX;
	m_dScaleY = src.m_dScaleY;

	m_bm.Attach(src.m_bm);
	m_pal.Attach(src.m_pal);

	m_ptUL_ORIGINAL = src.m_ptUL_ORIGINAL;
	m_ptLR_ORIGINAL = src.m_ptLR_ORIGINAL;
	m_ptUR_ORIGINAL = src.m_ptUR_ORIGINAL;
	m_ptLL_ORIGINAL = src.m_ptLL_ORIGINAL;
	m_size_ORIGINAL = src.m_size_ORIGINAL;
	m_bm_ORIGINAL.Attach(src.m_bm_ORIGINAL);

	BMIH = src.BMIH;

	m_strFileName = src.m_strFileName;
}

const CPrimBitmap& CPrimBitmap::operator=(const CPrimBitmap&)// src)
{
	return (*this);
}

CPrimBitmap::~CPrimBitmap()
{
}

bool CPrimBitmap::SelUsingLine(const CLine& ln, CPnts& ptsInt)
{
	ptsInt.RemoveAll();

	CPnts temp;

	polyline::BeginLineStrip();
	polyline::SetVertex(m_ptLR);
	polyline::SetVertex(m_ptLL);
	polyline::SetVertex(m_ptUL);
	polyline::SetVertex(m_ptUR);
	polyline::SetVertex(m_ptLR);
	if(polyline::SelUsingLine(ln, ptsInt))
		temp.Add(ptsInt[0]);

	return (!temp.IsEmpty());
}

CPrim*& CPrimBitmap::Copy(CPrim*& pPrim) const
{
	pPrim = new CPrimBitmap(*this);
	return (pPrim);
}

CPoint CPrimBitmap::UpdateLR(CPoint)// ptUL)
{
	m_ptLR[0] = m_ptUL[0] + m_size.cx;
	m_ptLR[1] = m_ptUL[1] - m_size.cy;

	CPnt4 pntLR(m_ptLR, 1.);
	ModelView_DoTransform(pntLR);
	CPoint ptLR = viewport::DoProjection(pntLR);

	m_ptLR[2] = 0;

	return ptLR;
}

void CPrimBitmap::SetActLikePrim(bool bActLikePrim)
{
	m_bActLikePrim = bActLikePrim;
}

HBITMAP GetRotatedBitmapNT( HBITMAP hBitmap, float radians, COLORREF clrBack )
{
	// Create a memory DC compatible with the display
	CDC sourceDC, destDC;
	sourceDC.CreateCompatibleDC( NULL );
	destDC.CreateCompatibleDC( NULL );

	// Get logical coordinates
	BITMAP bm;
	::GetObject( hBitmap, sizeof( bm ), &bm );

	float cosine = (float)cos(radians);
	float sine = (float)sin(radians);

	// Compute dimensions of the resulting bitmap
	// First get the coordinates of the 3 corners other than origin
	int x1 = (int)(bm.bmHeight * sine);
	int y1 = (int)(bm.bmHeight * cosine);
	int x2 = (int)(bm.bmWidth * cosine + bm.bmHeight * sine);
	int y2 = (int)(bm.bmHeight * cosine - bm.bmWidth * sine);
	int x3 = (int)(bm.bmWidth * cosine);
	int y3 = (int)(-bm.bmWidth * sine);

	int minx = min(0,min(x1, min(x2,x3)));
	int miny = min(0,min(y1, min(y2,y3)));
	int maxx = max(0,max(x1, max(x2,x3)));
	int maxy = max(0,max(y1, max(y2,y3)));

	int w = maxx - minx;
	int h = maxy - miny;

	// Create a bitmap to hold the result
	HBITMAP hbmResult = ::CreateCompatibleBitmap(CClientDC(NULL), w, h);

	HBITMAP hbmOldSource = (HBITMAP)::SelectObject( sourceDC.m_hDC, hBitmap );
	HBITMAP hbmOldDest = (HBITMAP)::SelectObject( destDC.m_hDC, hbmResult );

	// Draw the background color before we change mapping mode
	HBRUSH hbrBack = CreateSolidBrush( clrBack );
	HBRUSH hbrOld = (HBRUSH)::SelectObject( destDC.m_hDC, hbrBack );
	destDC.PatBlt( 0, 0, w, h, PATCOPY );
	::DeleteObject( ::SelectObject( destDC.m_hDC, hbrOld ) );

	// We will use world transform to rotate the bitmap
	SetGraphicsMode(destDC.m_hDC, GM_ADVANCED);
	XFORM xform;
	xform.eM11 = cosine;
	xform.eM12 = -sine;
	xform.eM21 = sine;
	xform.eM22 = cosine;
	xform.eDx = 0;//(float)-minx;
	xform.eDy = 0;//(float)-miny;

	SetWorldTransform( destDC.m_hDC, &xform );

	// Now do the actual rotating - a pixel at a time
	destDC.BitBlt(0,0,bm.bmWidth, bm.bmHeight, &sourceDC, 0, 0, SRCCOPY );

	// Restore DCs
	::SelectObject( sourceDC.m_hDC, hbmOldSource );
	::SelectObject( destDC.m_hDC, hbmOldDest );

	return hbmResult;
}

HBITMAP GetTransformedBitmapNT(HBITMAP hBitmap, CPnt ptUL, CPnt ptUR, CPnt ptLR, CPnt ptLL, 
							   double m_dScaleX, double m_dScaleY, double m_dRotation, COLORREF clrBack,
							   int& nXTrans, int& nYTrans)
{
	// Create a memory DC compatible with the display
	CDC sourceDC, destDC;
	sourceDC.CreateCompatibleDC( NULL );
	destDC.CreateCompatibleDC( NULL );

	// Get logical coordinates
	BITMAP bm;
	::GetObject( hBitmap, sizeof( bm ), &bm );

	float cosine = (float)cos(m_dRotation);
	float sine = (float)sin(m_dRotation);

	// Compute dimensions of the resulting bitmap
	// First get the coordinates of the 3 corners other than origin
	int x1 = (int)(bm.bmHeight * sine);
	int y1 = (int)(bm.bmHeight * cosine);
	int x2 = (int)(bm.bmWidth * cosine + bm.bmHeight * sine);
	int y2 = (int)(bm.bmHeight * cosine - bm.bmWidth * sine);
	int x3 = (int)(bm.bmWidth * cosine);
	int y3 = (int)(-bm.bmWidth * sine);

	int x4 = (int)ptUL[0];
	int y4 = (int)ptUL[1];

	int minx = min(0,min(x1, min(x2, min(x3, x4))));
	int miny = min(0,min(y1, min(y2, min(y3, y4))));
	int maxx = max(0,max(x1, max(x2, max(x3, x4))));
	int maxy = max(0,max(y1, max(y2, max(y3, y4))));

	int w = maxx - minx;
	int h = maxy - miny;

	// Create a bitmap to hold the result
	HBITMAP hbmResult = ::CreateCompatibleBitmap(CClientDC(NULL), w, h);

	HBITMAP hbmOldSource = (HBITMAP)::SelectObject( sourceDC.m_hDC, hBitmap );
	HBITMAP hbmOldDest = (HBITMAP)::SelectObject( destDC.m_hDC, hbmResult );

	// Draw the background color before we change mapping mode
	HBRUSH hbrBack = CreateSolidBrush( clrBack );
	HBRUSH hbrOld = (HBRUSH)::SelectObject( destDC.m_hDC, hbrBack );
	destDC.PatBlt( 0, 0, w, h, PATCOPY );
	::DeleteObject( ::SelectObject( destDC.m_hDC, hbrOld ) );

	// We will use world transform to rotate the bitmap
	SetGraphicsMode(destDC.m_hDC, GM_ADVANCED);
	XFORM xform;

	xform.eM11 = (float)cosine;
	xform.eM12 = (float)-sine;
	xform.eM21 = (float)sine;
	xform.eM22 = (float)cosine;

	xform.eDx = (float)(float)-minx;
	xform.eDy = (float)(float)-miny;

	nXTrans = minx;
	nYTrans = miny;

	SetWorldTransform( destDC.m_hDC, &xform );

	// Now do the actual rotating - a pixel at a time
	destDC.BitBlt(0,0,bm.bmWidth, bm.bmHeight, &sourceDC, 0, 0, SRCCOPY );

	// Restore DCs
	::SelectObject( sourceDC.m_hDC, hbmOldSource );
	::SelectObject( destDC.m_hDC, hbmOldDest );

	return hbmResult;
}

void CPrimBitmap::Display(CDC* pDC, bool bNodeDisplay)
{
	PENCOLOR nPenColor = LogicalPenColor();

	CPnt4 pntUL(m_ptUL, 1.);
	ModelView_DoTransform(pntUL);
	CPoint ptUL = viewport::DoProjection(pntUL);

	CPnt4 pntLR(m_ptLR, 1.);
	ModelView_DoTransform(pntLR);
	CPoint ptLR = viewport::DoProjection(pntLR);

	CPnt4 pntUR(m_ptUR, 1.);
	ModelView_DoTransform(pntUR);
	CPoint ptUR = viewport::DoProjection(pntUR);

	CPnt4 pntLL(m_ptLL, 1.);
	ModelView_DoTransform(pntLL);
	CPoint ptLL = viewport::DoProjection(pntLL);

	CDC dcMem;
	dcMem.CreateCompatibleDC(NULL);
	dcMem.SelectObject(&m_bm);

	double dRatio = vpActive.GetWidthInInches() / view::GetUExt();

	int iWidDst = (int)(m_size.cx * dRatio * m_dScaleX);
	int iHgtDst = (int)(m_size.cy * dRatio * m_dScaleY);

	// draw box around if in edit mode
	if((app.GetPegState() == STATE_BITMAP) && (m_bActLikePrim))
	{
		// create and select a thick, black pen
		CPen penBlack;

		if(nPenColor != mS_nSpecPenColor)
			penBlack.CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
		else
			penBlack.CreatePen(PS_SOLID, 3, RGB(255, 161, 0));

		CPen* pOldPen = pDC->SelectObject(&penBlack);

		pDC->Rectangle(ptUL.x - 2, ptUL.y - 2, ptUL.x + iWidDst + 2, ptUL.y + iHgtDst + 2);

		pDC->SelectObject(pOldPen);
	}

	if(nPenColor != mS_nSpecPenColor)
	{
		_QUAD _dstQuad;
		_dstQuad.p1.x = ptUL.x;
		_dstQuad.p1.y = ptUL.y;
		_dstQuad.p2.x = ptUR.x;
		_dstQuad.p2.y = ptUR.y;
		_dstQuad.p3.x = ptLR.x;
		_dstQuad.p3.y = ptLR.y;
		_dstQuad.p4.x = ptLL.x;
		_dstQuad.p4.y = ptLL.y;

		CBitmapEx bitmap3;
		bitmap3.Create(iWidDst, iHgtDst);
		bitmap3.Draw(_dstQuad, m_bitmapEx);

		pDC->StretchBlt(ptUL.x, ptUL.y, iWidDst, iHgtDst, &dcMem, 0, 0, m_size.cx, m_size.cy, SRCCOPY);
	}
	else
	{
		pDC->StretchBlt(ptUL.x, ptUL.y, iWidDst, iHgtDst, &dcMem, 0, 0, m_size.cx, m_size.cy, NOTSRCCOPY);//MERGEPAINT);
	}

	pDC->SetPixel(ptLR.x, ptLR.y, RGB(0, 255, 0));
	pDC->SetPixel(ptUL.x, ptUL.y, RGB(0, 255, 0));

	pDC->SetPixel(ptUR.x, ptUR.y, RGB(0, 255, 0));
	pDC->SetPixel(ptLL.x, ptLL.y, RGB(0, 255, 0));

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

void CPrimBitmap::DisplayOpenGL(bool)
{

}

void CPrimBitmap::DisRep(const CPnt& ) const
{
	CString strRep("<Bitmap> ");

	strRep += "Upper Left;" + m_ptUL.ToString() + " ";

	strRep += "Lower Right;" + m_ptLR.ToString();
}

void CPrimBitmap::FormatGeometry(CString& str) const
{
	str += "Upper Left;" + m_ptUL.ToString();
	str += "Lower Right;" + m_ptLR.ToString();
}

CString CPrimBitmap::FormatIntStyle() const
{
	CString str;// = (m_nIntStyle >= 0 && m_nIntStyle <= 3) ? strStyle[m_nIntStyle] : "Invalid!";
	return (str);
}

void CPrimBitmap::FormatExtra(CString& str) const
{
	str.Format("Color;%s\tStyle;%s\tWidth;%i\tHeight;%i\tFile;%s", FormatPenColor(), FormatPenStyle(), m_size.cx, m_size.cy, m_strFileName);
}

CPnt CPrimBitmap::GetCtrlPt() const 
{
	return m_ptUL;
}

CPnt CPrimBitmap::GoToNxtCtrlPt() const
{
	return m_ptLR;
}

void CPrimBitmap::Iron()
{
	m_ptUL[2] = 0;
	m_ptLR[2] = 0;
}

bool CPrimBitmap::SelUsingPoint(const CPnt4& ptPic, double dTol, CPnt& ptProj)
{
	bool bReturn1 = false;
	bool bReturn2 = false;
	bool bReturn3 = false;
	bool bReturn4 = false;

	polyline::BeginLineStrip();
	polyline::SetVertex(m_ptUL);
	polyline::SetVertex(m_ptUR);

	bReturn1 = polyline::SelUsingPoint(ptPic, dTol, mS_dRel, ptProj);

	polyline::BeginLineStrip();
	polyline::SetVertex(m_ptUR);
	polyline::SetVertex(m_ptLR);
	bReturn2 = polyline::SelUsingPoint(ptPic, dTol, mS_dRel, ptProj);

	polyline::BeginLineStrip();
	polyline::SetVertex(m_ptLR);
	polyline::SetVertex(m_ptLL);
	bReturn3 = polyline::SelUsingPoint(ptPic, dTol, mS_dRel, ptProj);

	polyline::BeginLineStrip();
	polyline::SetVertex(m_ptLL);
	polyline::SetVertex(m_ptUL);
	bReturn4 = polyline::SelUsingPoint(ptPic, dTol, mS_dRel, ptProj);

	return bReturn1 || bReturn2 || bReturn3 || bReturn4;
}

bool CPrimBitmap::SelUsingRect(const CPnt& pt1, const CPnt& pt2)
{
	CPnts pts;
	pts.Add(m_ptUL);
	pts.Add(m_ptLR);

	return polyline::SelUsingRect(pt1, pt2, pts);
}

void CPrimBitmap::ModifyState()
{
}

void CPrimBitmap::GetState()
{
}

bool CPrimBitmap::PvtOnCtrlPt(const CPnt4&) const
{
	return true;
}

void CPrimBitmap::GetAllPts(CPnts& pts)
{
	pts.SetSize(0);

	pts.Add(m_ptUL);
	pts.Add(m_ptLR);
}

// Determines the extent.
void CPrimBitmap::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
{
	CPnt pt[2];
	pt[0] = m_ptUL;
	pt[1] = m_ptLR;

	for (WORD w = 0; w < 2; w++)
	{
		mspace.Transform(pt[w]);
		pt[w] = tm * pt[w];
		ptMin = Min(ptMin, pt[w]);
		ptMax = Max(ptMax, pt[w]);
	}
}

bool CPrimBitmap::IsInView() const
{
	return true;
}

bool CPrimBitmap::IsPtACtrlPt(const CPnt4&) const
{
	return false;
}

void CPrimBitmap::ReadPeg(HFILE fl)
{
	short sVersion = 0;

	FilePeg_ReadShort2(fl, sVersion);

	m_ptUL_ORIGINAL.Read(fl);

	FilePeg_ReadString2(fl, m_strFileName);
	BitmapLayerLoad();

	HBITMAP hbm;
	m_bitmapEx.Save(hbm);

	Init(m_ptUL_ORIGINAL, hbm);
}

CPnt CPrimBitmap::SelAtCtrlPt(const CPnt4&) const
{
	return m_ptUL;
}

void CPrimBitmap::Transform(const CTMat& tm)
{
	// pre dimensions
	double dw1 = Pnt_DistanceTo_xy(m_ptUL_ORIGINAL, m_ptUR_ORIGINAL);
	double dh1 = Pnt_DistanceTo_xy(m_ptUL_ORIGINAL, m_ptLL_ORIGINAL);

	CPnt ptOrigUL = m_ptUL_ORIGINAL;
	CPnt ptOrigUR = m_ptUR_ORIGINAL;

	// actual transform
	m_ptUL = tm * m_ptUL;
	m_ptLR = tm * m_ptLR;
	m_ptUR = tm * m_ptUR;
	m_ptLL = tm * m_ptLL;

	// post dimensions
	double dw2 = Pnt_DistanceTo_xy(m_ptUL, m_ptUR);
	double dh2 = Pnt_DistanceTo_xy(m_ptUL, m_ptLL);

	// compute scale 
	m_dScaleX = (dw2 / dw1);
	m_dScaleY = (dh2 / dh1);

	CVec vStart(ptOrigUL, ptOrigUR);
	CVec vEnd(m_ptUL, m_ptUR);

	m_dRotation = line::GetAngAboutZAx(CLine(m_ptUL, m_ptUR));

	int nXTrans;
	int nYTrans;

	HBITMAP bmReturn = GetTransformedBitmapNT((HBITMAP)m_bm_ORIGINAL, m_ptUL, m_ptUR, m_ptLR, m_ptLL, 
		m_dScaleX, m_dScaleY, m_dRotation, RGB(255,255,255),
		nXTrans, nYTrans);

	m_bm.Detach();
	m_bm.Attach(bmReturn);											  

	// new sizes
	BITMAP bm;
	m_bm.GetBitmap(&bm);
	m_size.cx = bm.bmWidth;
	m_size.cy = bm.bmHeight;
}

void CPrimBitmap::Translate(const CVec& v)
{
	m_ptUL += v;
	m_ptLR += v;
	m_ptUR += v;
	m_ptLL += v;
}

void CPrimBitmap::TranslateUsingMask(const CVec&, const DWORD)
{
}

bool CPrimBitmap::Vectorize(const CSeg*)
{
	return false;
}

void BitmapLayerSave(CBitmapEx m_bitmapEx, CString filename);

bool CPrimBitmap::WritePeg(HFILE fl) const
{
	short sVersion = 1;

	FilePeg_WriteWord2(fl, PRIM_BITMAP);
	FilePeg_WriteWord2(fl, sVersion);

	m_ptUL_ORIGINAL.Write(fl);

	BitmapLayerSave(m_bitmapEx, m_strFileName);
	FilePeg_WriteString2(fl, m_strFileName);

	return true;
}

void CPrimBitmap::LoadBitmapFile(FILE *pFile) const
{

}

void BitmapLayerSave(CBitmapEx m_bitmapEx, CString filename)
{
	filename = "test00001.bmp";

	m_bitmapEx.Save(filename.GetBuffer());
}

#include <atlimage.h>
#include <Gdiplusimaging.h>

void CPrimBitmap::SaveBitmapFile(FILE *pFile) const
{
}

void CPrimBitmap::BitmapLayerLoad()
{
	m_bitmapEx.Load(m_strFileName.GetBuffer());
}