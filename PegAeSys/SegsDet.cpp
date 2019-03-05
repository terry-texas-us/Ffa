#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

CSegsDet::CSegsDet()
{
	m_pDetSeg = 0;
	m_pDetPrim = 0;
	m_dPicApertSiz = .005;
}

CSegsDet::CSegsDet(const CSegsDet &source)
{
	m_ptDet = source.m_ptDet;
	m_dPicApertSiz = source.m_dPicApertSiz;
	m_pDetSeg = source.m_pDetSeg;
	m_pDetPrim = source.m_pDetPrim;
}

CSegsDet& CSegsDet::operator= (const CSegsDet& param)
{
	m_ptDet = param.m_ptDet;
	m_dPicApertSiz = param.m_dPicApertSiz;
	m_pDetSeg = param.m_pDetSeg;
	m_pDetPrim = param.m_pDetPrim;
	return *this;
}

void CSegsDet::Build()
{
	// Determines which segments are currently visible
	CPegDoc* pDoc = CPegDoc::GetDoc();

	RemoveAll();

	for (WORD w = 0; w < pDoc->LayersGetSize(); w++)
	{
		CLayer* pLayer = pDoc->LayersGetAt(w);
		
		if (pLayer->IsHot() || pLayer->IsWarm())
		{
			POSITION pos = pLayer->GetHeadPosition();
			while (pos != 0)
			{
				CSeg* pSeg = pLayer->GetNext(pos);
	
				if (pSeg->IsInView())
				{
					AddTail(pSeg);
				}
			}
		}
	}
}

///<summary> Deletes last detectible segment.</summary>
void CSegsDet::DelLast()
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	
	if (IsEmpty())
	{
		msgInformation(IDS_MSG_NO_DET_SEGS_IN_VIEW);
	}
	else
	{
		CSeg* pSeg = RemoveTail();
		
		pDoc->AnyLayerRemove(pSeg);
		trapsegs.Remove(pSeg);
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSeg);
		pDoc->DeletedSegsAddTail(pSeg);
		msgInformation(IDS_SEG_DEL_TO_RESTORE);
	}
}

CSeg* CSegsDet::SelLineUsingPoint(const CPnt& pt)
{
	m_pDetSeg = 0;
	m_pDetPrim = 0;

	CPnt ptEng;
	
	CPnt4 ptView(pt, 1.);
	ModelView_DoTransform(ptView);
	
	double tol = m_dPicApertSiz;

	CTMat tm = view::GetMatrixInverse();
	
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim != 0)
		{
			CPrim* pPrim = pSeg->GetNext(posPrim);
			if (pPrim->Is(CPrim::PRIM_LINE))
			{
				if (pPrim->SelUsingPoint(ptView, tol, ptEng))
				{	
					tol = Pnt4_DistanceTo_xy(ptView, CPnt4(ptEng, 1.));
					
					m_ptDet = ptEng;
					m_ptDet = tm * m_ptDet;
					m_pDetSeg = pSeg;
					m_pDetPrim = pPrim;
				}
			}
		}
	}
	return (m_pDetSeg);
}

CPrimText* CSegsDet::SelTextUsingPoint(const CPnt& ptPic)
{
	CPnt4 ptView(ptPic, 1.);
	ModelView_DoTransform(ptView);

	POSITION pos = GetHeadPosition();	
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim !=0)
		{
			CPrim* pPrim = pSeg->GetNext(posPrim);
			if (pPrim->Is(CPrim::PRIM_TEXT))
			{
				CPnt ptProj;
				if (static_cast<CPrimText*>(pPrim)->SelUsingPoint(ptView, 0., ptProj))
					return static_cast<CPrimText*>(pPrim);			
			}
		}
	}
	return 0;
}

CSeg* CSegsDet::SelSegAndPrimUsingPoint(const CPnt& pt)
{
	CPnt ptEng;
	
	m_pDetSeg = 0;
	m_pDetPrim = 0;

	CPnt4 ptView(pt, 1.);
	ModelView_DoTransform(ptView);	
	
	CTMat tm = view::GetMatrixInverse();
	
	double dPicApert = m_dPicApertSiz;	
	
	CPrimPolygon::EdgeToEvaluate() = 0; 	
	CPrimPolyline::EdgeToEvaluate() = 0;

	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		CPrim* pPrim = pSeg->SelPrimUsingPoint(ptView, dPicApert, ptEng);
		if (pPrim != 0)
		{
			m_ptDet = ptEng;
			m_ptDet = tm * m_ptDet;
			m_pDetSeg = pSeg;
			m_pDetPrim = pPrim;
			return (pSeg);
		}
	}
	return 0; 	
}

CSeg* CSegsDet::SelSegAndPrimAtCtrlPt(const CPnt4& pt)
{
	CPrim* pPrim;
	CPnt ptEng;

	m_pDetSeg = 0;
	m_pDetPrim = 0;

	CTMat tm = view::GetMatrixInverse();

	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		pPrim = pSeg->SelPrimAtCtrlPt(pt, &ptEng);
		if (pPrim != 0)
		{
			m_ptDet = ptEng;
			m_ptDet = tm * m_ptDet;
			m_pDetSeg = pSeg;
			m_pDetPrim = pPrim;
		}
	}
	return (m_pDetSeg);
}	