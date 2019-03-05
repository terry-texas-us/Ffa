#include "stdafx.h"

CModelTransform::CModelTransform()
{
	m_wDepth = 0;
	m_p_tmCompositeModeling = new CTMat;
}

CModelTransform::~CModelTransform()
{
	delete m_p_tmCompositeModeling;
}

void CModelTransform::InvokeNew()
{
	m_wDepth++;
	m_TMList.AddTail(m_p_tmCompositeModeling);
	m_p_tmCompositeModeling = new CTMat(*m_p_tmCompositeModeling);

//	opengl::PushModelViewMatrix();
}

void CModelTransform::Return()
{
	delete m_p_tmCompositeModeling;
	
	m_p_tmCompositeModeling = (CTMat*) m_TMList.RemoveTail();
	
	m_wDepth--;

//	opengl::PopModelViewMatrix();
}
void CModelTransform::SetLocalTM(const CTMat& tm)
{
	*m_p_tmCompositeModeling = tm * *m_p_tmCompositeModeling;

//	opengl::MultModelViewMatrix(tm);
}

void CModelTransform::Transform(CPnt& pt) const 
{
	if (m_wDepth == 0) return;
	pt = *m_p_tmCompositeModeling * pt;
}

void CModelTransform::Transform(CPnt4& pt) const 
{
	if (m_wDepth == 0) return;
	pt = *m_p_tmCompositeModeling * pt;
}

void CModelTransform::Transform(CVec& v) const 
{
	if (m_wDepth == 0) return;
	v = *m_p_tmCompositeModeling * v;
}
	
void CModelTransform::Transform(CVec4& v) const 
{
	if (m_wDepth == 0) return;
	v = *m_p_tmCompositeModeling * v;
}
	