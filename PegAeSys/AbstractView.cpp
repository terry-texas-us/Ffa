#include "stdafx.h"

#include "grid.h"

CAbstractView::CAbstractView()
{
	//m_ptEye = CPnt(0., 0., 60.);
	//m_ptTarget = ORIGIN;
	//m_vDirection = ZDIR;
	//m_vVwUp = YDIR;
	//SetNearClipDistance(-1000.);
	//SetFarClipDistance(1000.);

	m_dLensLength = 60.;
	
	SetPerspectiveEnabled(false);
	SetNearClippingEnabled(false);
	SetFarClippingEnabled(false);
}

CAbstractView::CAbstractView(const CAbstractView& src)
{
	m_nViewMode = src.m_nViewMode; 
	m_nRenderMode = src.m_nRenderMode;
	
	m_sUcsOrthoViewType = src.m_sUcsOrthoViewType;
	m_ptUCSOrigin = src.m_ptUCSOrigin;
	m_vUCSXAxis = src.m_vUCSXAxis;
	m_vUCSYAxis = src.m_vUCSYAxis;
	m_dElevation = src.m_dElevation;
	
	m_ptEye = src.m_ptEye;
	m_ptTarget = src.m_ptTarget;
	m_vDirection = src.m_vDirection;
	m_vVwUp = src.m_vVwUp;
	m_dViewTwist = src.m_dViewTwist;
	m_dHeight = src.m_dHeight;
	m_dWidth = src.m_dWidth;
	m_dCenter[0] = src.m_dCenter[0]; m_dCenter[1] = src.m_dCenter[1];
	m_dLensLength = src.m_dLensLength;
	m_dNearClipDistance = src.m_dNearClipDistance;
	m_dFarClipDistance = src.m_dFarClipDistance;
}

CAbstractView::CAbstractView(OdDbViewportPtr pView)
{
	OdDbObjectId OdUCS = pView->ucsName();
//	OdUCS.
//	OdDbUCSTableRecordPtr pUCS = OdUCS.safeOpenObject();//.openObject();
//	OdDbObjectPtr pObj = OdUCS.safeOpenObject();
	
//	OdDbUCSTableRecordPtr pUCS = pObj;

//	m_ptUCSOrigin = CPnt(pUCS->origin().x, pUCS->origin().y, pUCS->origin().z);
//	m_vUCSXAxis = CVec(pUCS->xAxis().x, pUCS->xAxis().y, pUCS->xAxis().z);
//	m_vUCSYAxis = CVec(pUCS->yAxis().x, pUCS->yAxis().y, pUCS->yAxis().z);
//	m_sUcsOrthoViewType = pVP->ucsorthoviewtype;
	m_dElevation = pView->elevation();

	m_ptTarget = CPnt(pView->viewTarget().x, pView->viewTarget().y, pView->viewTarget().z);
	m_vDirection = CVec(pView->viewDirection().x, pView->viewDirection().y, pView->viewDirection().z);

	m_dViewTwist = pView->twistAngle();
	m_dHeight = pView->viewHeight();
	m_dWidth = pView->viewHeight() * pView->width() / pView->height();
	m_dCenter[0] = pView->viewCenter().x;
	m_dCenter[1] = pView->viewCenter().y;
	m_dLensLength = pView->lensLength();
	m_dNearClipDistance = -1000.;
	m_dFarClipDistance = 1000.;
	
	m_ptEye = m_ptTarget + m_vDirection * m_dLensLength;
	m_vVwUp(0., 1., 0.);
}

CAbstractView& CAbstractView::operator=(const CAbstractView& src)
{
	m_nViewMode = src.m_nViewMode; 
	m_nRenderMode = src.m_nRenderMode;
	
	m_sUcsOrthoViewType = src.m_sUcsOrthoViewType;
	m_ptUCSOrigin = src.m_ptUCSOrigin;
	m_vUCSXAxis = src.m_vUCSXAxis;
	m_vUCSYAxis = src.m_vUCSYAxis;
	m_dElevation = src.m_dElevation;
	
	m_ptEye = src.m_ptEye;
	m_ptTarget = src.m_ptTarget;
	m_vDirection = src.m_vDirection;
	m_vVwUp = src.m_vVwUp;
	m_dViewTwist = src.m_dViewTwist;
	m_dHeight = src.m_dHeight;
	m_dWidth = src.m_dWidth;
	m_dCenter[0] = src.m_dCenter[0]; m_dCenter[1] = src.m_dCenter[1];
	m_dLensLength = src.m_dLensLength;
	m_dNearClipDistance = src.m_dNearClipDistance;
	m_dFarClipDistance = src.m_dFarClipDistance;

	return *this;
}

void CAbstractView::LookAt(const CPnt& ptEye, const CPnt& ptTarget, const CVec& vVwUp) 
{
	m_ptEye = ptEye;
	m_ptTarget = ptTarget;
	SetVwUp(vVwUp);
}

void CAbstractView::SetDirection(const CVec& v)
{
	if (v.Length() > DBL_EPSILON)
	{
		m_vDirection = v;
		m_vDirection.Normalize();
	}
}

void CAbstractView::SetEye(const CVec& v) 
{
	m_ptEye = m_ptTarget + (v * m_dLensLength);
}

void CAbstractView::SetFarClippingEnabled(bool bEnabled)
{
	if (bEnabled) 
		m_nViewMode |= AV_FARCLIPPING;
	else 
		m_nViewMode &= ~AV_FARCLIPPING;
}

void CAbstractView::SetNearClippingEnabled(bool bEnabled)
{
	if (bEnabled) 
		m_nViewMode |= AV_NEARCLIPPING;
	else 
		m_nViewMode &= ~AV_NEARCLIPPING;
}

void CAbstractView::SetPerspectiveEnabled(bool bEnabled)
{
	if (bEnabled) 
		m_nViewMode |= AV_PERSPECTIVE;
	else 
		m_nViewMode &= ~AV_PERSPECTIVE;
}

void CAbstractView::SetVwUp(const CVec& v)
{
	if (v.Length() > DBL_EPSILON)
	{
		m_vVwUp = v;
		m_vVwUp.Normalize();
	}
}