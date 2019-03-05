#include "stdafx.h"

CViewport::CViewport(const CViewport& src) 
{
	m_ptCenter = src.m_ptCenter;
	m_dWidth = src.m_dWidth;
	m_dHeight = src.m_dHeight;
	m_iDeviceHeightInPixels = src.m_iDeviceHeightInPixels;
	m_dDeviceHeightInInches = src.m_dDeviceHeightInInches;
	m_iDeviceWidthInPixels = src.m_iDeviceWidthInPixels;
	m_dDeviceWidthInInches = src.m_dDeviceWidthInInches;
}

CViewport::CViewport(OdDbViewportPtr pView)
{
	m_ptCenter = CPnt(pView->centerPoint().x, pView->centerPoint().y, pView->centerPoint().z);
	m_dWidth = pView->width();
	m_dHeight = pView->height();
}

CViewport& CViewport::operator=(const CViewport& src) 
{
	m_ptCenter = src.m_ptCenter;
	m_dWidth = src.m_dWidth;
	m_dHeight = src.m_dHeight;
	m_iDeviceHeightInPixels = src.m_iDeviceHeightInPixels;
	m_dDeviceHeightInInches = src.m_dDeviceHeightInInches;
	m_iDeviceWidthInPixels = src.m_iDeviceWidthInPixels;
	m_dDeviceWidthInInches = src.m_dDeviceWidthInInches;
	
	return *this;
}

///<summary>Transforms a point from view coordinates to point in window coordinates.</summary>
// Notes:	Window coordinates are rounded to nearest whole number.
CPoint CViewport::DoProjection(const CPnt4& pt) const
{	
	CPoint pnt;
	
	pnt.x = Round((pt[0] / pt[3] + 1.) * ((m_dWidth - 1.) / 2.));
	pnt.y = Round((- pt[1] / pt[3] + 1.) * ((m_dHeight - 1.) / 2.));

	return pnt;
}

///<summary>Transforms a point from view coordinates to a point in window coordinates.</summary>
// Window coordinates are rounded to nearest whole number.
// Perspective division to yield normalized device coordinates
void CViewport::DoProjection(CPoint* pnt, int iPts, CPnt4* pt) const
{	
	for (int i = 0; i < iPts; i++)
	{
		pt[i] /= pt[i][3];
		
		pnt[i].x = Round((pt[i][0] + 1.) * ((m_dWidth - 1.) / 2.));
		pnt[i].y = Round((- pt[i][1] + 1.) * ((m_dHeight - 1.) / 2.));
	}	 
}

///<summary>Transforms a point from view coordinates to a point in window coordinates.</summary>
// Window coordinates are rounded to nearest whole number.
// Perspective division to yield normalized device coordinates
void CViewport::DoProjection(CPoint* pnt, CPnt4s& pta) const
{	
	int iPts = (int) pta.GetSize();

	for (int i = 0; i < iPts; i++)
	{
		pta[i] /= pta[i][3];
		
		pnt[i].x = Round((pta[i][0] + 1.) * ((m_dWidth - 1.) / 2.));
		pnt[i].y = Round((- pta[i][1] + 1.) * ((m_dHeight - 1.) / 2.));
	}	 
}

void CViewport::DoProjectionInverse(CPnt& pt) const
{	
	pt[0] = (pt[0] * 2.) / (m_dWidth - 1.) - 1.;
	pt[1] = - ((pt[1] * 2.) / (m_dHeight - 1.) - 1.);
}

void CViewport::Set(double dWidth, double dHeight)
{	
	m_dWidth = dWidth;
	m_dHeight = dHeight;

//	opengl::Viewport(0, 0, Round(dWidth), Round(dHeight));
}

void viewport::PopActive()
{
	if (!vps.IsEmpty()) 
		vpActive = vps.RemoveTail();

//	opengl::Viewport(0, 0, int(vpActive.GetWidth()), int(vpActive.GetHeight()));
}

void viewport::PushActive()
{
	vps.AddTail(vpActive);
}