#include "stdafx.h"

CModelView::CModelView()
{
	m_dUMin = - 1.;
	m_dVMin = - 1.;
	m_dUMax = 1.;
	m_dVMax = 1.;
}

CModelView::CModelView(const CModelView& src) : CAbstractView(src)
{
	m_dUMin = src.m_dUMin;
	m_dVMin = src.m_dVMin;
	m_dUMax = src.m_dUMax;
	m_dVMax = src.m_dVMax;

	m_tmView = src.m_tmView;
	m_tmViewInverse = src.m_tmViewInverse;
}

CModelView& CModelView::operator=(const CModelView& src)
{
	CAbstractView::operator=(src);

	m_dUMin = src.m_dUMin;
	m_dVMin = src.m_dVMin;
	m_dUMax = src.m_dUMax;
	m_dVMax = src.m_dVMax;

	m_tmView = src.m_tmView;
	m_tmView = src.m_tmViewInverse;

	return *this;
}

void CModelView::AdjustWnd(double dAspectRatio)
{
	double dUExt = m_dUMax - m_dUMin;
	double dVExt = m_dVMax - m_dVMin;
	
	if (dUExt <= DBL_EPSILON || dVExt / dUExt > dAspectRatio) 
	{	// Extend width
		double dX = (dVExt / dAspectRatio - dUExt) * .5;
		m_dUMin -= dX;
		m_dUMax += dX;
	}
	else
	{	// Extend height
		double dY = (dUExt * dAspectRatio - dVExt) * .5;
		m_dVMin -= dY;
		m_dVMax += dY;
	}
	BuildTransformMatrix();
}

void CModelView::BuildTransformMatrix()
{
	m_tmView.Identity();
	
	CVec vN = m_ptEye - m_ptTarget;
	vN.Normalize();

	CVec vU(m_vVwUp ^ vN);
	vU.Normalize();
	
	CVec vV(vN ^ vU);
	vU.Normalize();
	
	m_tmView[0] = CVec4(vU, (ORIGIN - m_ptEye) * vU);
	m_tmView[1] = CVec4(vV, (ORIGIN - m_ptEye) * vV);
	m_tmView[2] = CVec4(vN, (ORIGIN - m_ptEye) * vN);
	m_tmView[3] = CVec4(0., 0., 0., 1.);

//	opengl::LookAt(m_ptEye, m_ptTarget, m_vVwUp);

	double dUExt = m_dUMax - m_dUMin;
	double dVExt = m_dVMax - m_dVMin;
	double dNExt = m_dFarClipDistance - m_dNearClipDistance;

	CTMat tmProj;
	tmProj.Identity();
	
	if (IsPerspectiveEnabled())
	{
//		opengl::Frustum(m_dUMin, m_dUMax, m_dVMin, m_dVMax, m_dNearClipDistance, m_dFarClipDistance);
	
		tmProj[0] = CVec4(2. * m_dNearClipDistance / dUExt, 0., (m_dUMax + m_dUMin) / dUExt, 0.);
		tmProj[1] = CVec4(0., (2. * m_dNearClipDistance) / dVExt, (m_dVMax + m_dVMin) / dVExt, 0.);
		tmProj[2] = CVec4(0., 0., - (m_dFarClipDistance + m_dNearClipDistance) / dNExt,
						- 2. * m_dFarClipDistance * m_dNearClipDistance / dNExt);
		tmProj[3] = CVec4(0., 0., - 1., 0.);
	}
	else 
	{
//		opengl::Ortho(m_dUMin, m_dUMax, m_dVMin, m_dVMax, m_dNearClipDistance, m_dFarClipDistance);
		
		// if oblique projections needed
		
		tmProj[0] = CVec4(2. / dUExt, 0., 0., - (m_dUMax + m_dUMin) / dUExt);
		tmProj[1] = CVec4(0., 2. / dVExt, 0., - (m_dVMax + m_dVMin) / dVExt);
		tmProj[2] = CVec4(0., 0., - 2. / dNExt, - (m_dFarClipDistance + m_dNearClipDistance) / dNExt);
		tmProj[3] = CVec4(0., 0., 0., 1.);
	}
	m_tmView *= tmProj;

	m_tmViewInverse = m_tmView;
	m_tmViewInverse.Inverse();
}

void CModelView::DoTransform(CPnt4& pt) const
{
	mspace.Transform(pt);
	pt = m_tmView * pt;
}

void CModelView::DoTransform(CPnt4s& pta) const
{
	int iPts = (int) pta.GetSize();
	for (int i = 0; i < iPts; i++)
		DoTransform(pta[i]);
}
void CModelView::DoTransform(int iPts, CPnt4* pt) const
{
	for (int i = 0; i < iPts; i++)
		DoTransform(pt[i]);
}

void CModelView::DoTransform(CVec4& v) const
{
	mspace.Transform(v);
	v = m_tmView * v;
}

void CModelView::Initialize()
{
	SetCenteredWnd(44., 34.);
	
	CPnt ptTarget = CPnt(GetUExt() / 2., GetVExt() / 2., 0.);
	CPnt ptEye = ptTarget + (ZDIR * m_dLensLength);
	
	LookAt(ptEye, ptTarget, YDIR);
	
	SetDirection(ZDIR);
	
	SetPerspectiveEnabled(false);
	
	SetNearClipDistance(-1000.);
	SetFarClipDistance(1000.);
	
	BuildTransformMatrix();

	vOver = *this;
}

void CModelView::SizeUpdate()
{
	double a = m_dUMin + (m_dUMax - m_dUMin) / 2;
	double b = m_dVMin + (m_dVMax - m_dVMin) / 2;
	SetCenteredWnd(a, b);
	
	//////////////////////////////////////////////////////////////////////
	//double dVExt = viewport::GetHeight();
	//double dUExt = viewport::GetWidth();
	//double dAspectRatio = viewport::GetAspectRatio();
	//CPnt center = viewport::GetCenter();

	//m_dUMin = center[0] - (dUExt / 8);
	//m_dUMax = center[0] + (dUExt / 8);
	//m_dVMin = center[1] - (dVExt / 8);
	//m_dVMax = center[1] + (dVExt / 8);

	////if (dUExt == 0.)
	////	dUExt = GetUExt();
	////if (dVExt == 0.)
	////	dVExt = GetVExt();
 
	////if(dAspectRatio < dVExt / dUExt)
	////	dUExt = dVExt / dAspectRatio;
	////else
	////	dVExt = dUExt * dAspectRatio;

	////m_dUMin = -dUExt * .5;
	////m_dVMin = -dVExt * .5;
	////m_dUMax = m_dUMin + dUExt;
	////m_dVMax = m_dVMin + dVExt;
 
	//BuildTransformMatrix();
}

void CModelView::SetCenteredWnd(double dUExt, double dVExt)
{	// Sets a window which is centered on the view target after adjusting for aspect
	double dAspectRatio = viewport::GetAspectRatio();
	
	if (dUExt == 0.)
		dUExt = GetUExt();
	if (dVExt == 0.)
		dVExt = GetVExt();

	if (dAspectRatio < dVExt / dUExt)
		dUExt = dVExt / dAspectRatio;
	else
		dVExt = dUExt * dAspectRatio;

	m_dUMin = - dUExt * .5;
	m_dVMin = - dVExt * .5;
	m_dUMax = m_dUMin + dUExt;
	m_dVMax = m_dVMin + dVExt;
 
	BuildTransformMatrix();
}

void CModelView::SetWnd(double dUMin, double dVMin, double dUMax, double dVMax)
{
	m_dUMin = dUMin;
	m_dVMin = dVMin;
	m_dUMax = dUMax;
	m_dVMax = dVMax;

	BuildTransformMatrix();
}

void CModelView::RotateNorm(const CVec& vNorm)
{
	CTMat tm(ORIGIN, vNorm);
	m_tmView *= tm;
}
void CModelView::RotateZ(double dSinAng, double dCosAng)
{
	CTMat tm; tm.RotateZ(dSinAng, dCosAng);
	m_tmView *= tm;
}
void CModelView::Scale(const CVec& v)
{
	CTMat tm; tm.Scale(v);
	m_tmView *= tm;
}

void CModelView::Translate(const CVec& v)
{
	m_tmView.Translate(v); 
}

void view::AdjustWnd(double& dUMin, double& dVMin, double& dUMax, double& dVMax, double dRatio)
{
	double dAspectRatio = viewport::GetAspectRatio();
	
	double dUExt = fabs(dUMax - dUMin);
	double dVExt = fabs(dVMax - dVMin);
	
	double dXExt = 0.;
	double dYExt = 0.;
	
	double dScale = 1. - (viewport::GetWidthInInches() / dUExt) / dRatio;

	if (fabs(dScale) > DBL_EPSILON)
	{
		dXExt = dScale * dUExt;
		dYExt = dScale * dVExt;
	}
	if (dUExt <= DBL_EPSILON || dVExt / dUExt > dAspectRatio)	// Extend x boundaries
		dXExt += (dVExt / dAspectRatio - dUExt) * .5;
	else														// Extend y boundaries
		dYExt += (dUExt * dAspectRatio - dVExt) * .5;
			
	dUMin -= dXExt;
	dUMax += dXExt;
	dVMin -= dYExt;
	dVMax += dYExt;
}

void view::PopActive()
{
	if (!mvs.IsEmpty()) 
		vActive = mvs.RemoveTail();

	vActive.BuildTransformMatrix();
}

void view::PushActive()
{
	mvs.AddTail(vActive);
}