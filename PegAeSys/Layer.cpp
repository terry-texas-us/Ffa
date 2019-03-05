#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "FileJob.h"

CLayer::CLayer(const CString& strName, WORD wStateFlgs)
{
	m_strName = strName;
	m_wTracingFlgs = 0;
	m_wStateFlgs = wStateFlgs;
	m_nPenColor = 1;
	m_strPenStyleName = "Continuous";
}

const CLayer& CLayer::operator=(const CLayer& src)
{
	m_strName = src.m_strName;
	m_wTracingFlgs = src.m_wTracingFlgs;
	m_wStateFlgs = src.m_wStateFlgs;
	m_nPenColor = src.m_nPenColor;
	m_strPenStyleName = src.m_strPenStyleName;

	return (*this);
}

void CLayer::Display(CDC* pDC)
{
	CPrim::LayerPenColor() = PenColor();
	CPrim::LayerPenStyle() = PenStyle();

	COLORREF* pCurColTbl = pColTbl;
	
	// mak - tracing open remove
//	pColTbl = (/*IsOpened() || */IsHot() || IsWarm()) ? crHotCols : crWarmCols;
	pColTbl = (IsHot() || IsWarm()) ? crHotCols : crWarmCols;

	CSegs::Display(pDC);
	pColTbl = pCurColTbl;
}

//void CLayer::PrintLayer(CDC *pDC, bool bIdentifyTrap)
//{
//	if (IsOn())
//	{
//		CPrim::LayerPenColor() = PenColor();
//		CPrim::LayerPenStyle() = PenStyle();
//		
//		COLORREF* pCurColTbl = pColTbl;
//		
//		//mak - tracing open remove
//		bool bDetSegs = /*IsOpened() || */IsHot() || IsWarm();//) && !IsMapped());
//		
//		// disable warm colors for print routine, not screen
////		pColTbl = bDetSegs ? crHotCols : crWarmCols;
//		pColTbl = crHotCols;
//
//		POSITION pos = GetHeadPosition();
//		while (pos != 0)
//		{
//			CSeg* pSeg = GetNext(pos);
//	
//			if (pSeg->IsInView())
//			{
//				if (bDetSegs)// && !IsMapped())
//					detsegs.AddTail(pSeg);
//				
//				if (bIdentifyTrap && trapsegs.Find(pSeg) != 0)
//				{
//					CPrim::SpecPenColor() = trapsegs.PenColor();
//					pSeg->Display(pDC, false);
//					CPrim::SpecPenColor() = 0;
//				}
//				else
//				{
//					pSeg->Display(pDC, false);
//				}					
//			}
//		}
//		pColTbl = pCurColTbl;
//	}
//}

void CLayer::Display(CDC* pDC, bool bIdentifyTrap)
{
	if (IsOn())
	{
		CPrim::LayerPenColor() = PenColor();
		CPrim::LayerPenStyle() = PenStyle();
		
		COLORREF* pCurColTbl = pColTbl;
		
		// mak - tracing open remove
		bool bDetSegs = /*IsOpened() || */IsHot() || IsWarm();
//		bool bDetSegs = ((IsOpened() || IsHot() || IsWarm()) && !IsMapped());
		
		pColTbl = bDetSegs ? crHotCols : crWarmCols;

		POSITION pos = GetHeadPosition();
		while (pos != 0)
		{
			CSeg* pSeg = GetNext(pos);
	
			if (pSeg->IsInView())
			{
				if (bDetSegs)// && !IsMapped())
					detsegs.AddTail(pSeg);
				
				if (bIdentifyTrap && trapsegs.Find(pSeg) != 0)
				{
					CPrim::SpecPenColor() = trapsegs.PenColor();
					pSeg->Display(pDC, IsHot() || IsWarm());
					CPrim::SpecPenColor() = 0;
				}
				else
				{
					pSeg->Display(pDC, IsHot() || IsWarm());
				}					
			}
		}
		pColTbl = pCurColTbl;
	}
}

PENSTYLE CLayer::PenStyle() const
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	return pDoc->PenStylesLookup(m_strPenStyleName);
}

void CLayer::PenTranslation(WORD wCols, PENCOLOR* pColNew, PENCOLOR* pCol)
{
	for (int i = 0; i < wCols; i++)
	{
		if(m_nPenColor == pCol[i])
		{
			m_nPenColor = pColNew[i];
			break;
		}
	}

	CSegs::PenTranslation(wCols, pColNew, pCol);
}

void CLayer::PenStyleTranslation(const PENSTYLE& nStyleNew, const PENSTYLE& nStyleOld)
{
//	if(m_strPenStyleName == CPegDoc::GetDoc()->penstpStyleOld[i])
//		m_nPenStyle = pStyleNew[i];

	CSegs::PenStyleTranslation(nStyleNew, nStyleOld);
}

void CLayer::RemoveFromSegLists()
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
	
		if(pSeg != 0)
		{
			detsegs.Remove(pSeg);
			trapsegs.Remove(pSeg);
		}
	}
}

void CLayer::SetStateCold()
{
	ClrStateFlg(STATE_FLG_HOT | STATE_FLG_WARM | STATE_FLG_OFF);
	SetStateFlg(STATE_FLG_COLD);
}

void CLayer::SetStateHot()
{
	ClrStateFlg(STATE_FLG_WARM | STATE_FLG_COLD | STATE_FLG_OFF);
	SetStateFlg(STATE_FLG_HOT);
}

void CLayer::SetStateOff()
{
	ClrStateFlg(STATE_FLG_HOT | STATE_FLG_WARM | STATE_FLG_COLD);
	SetStateFlg(STATE_FLG_OFF);
}

void CLayer::SetStateWarm()
{
	ClrStateFlg(STATE_FLG_HOT | STATE_FLG_COLD | STATE_FLG_OFF);
	SetStateFlg(STATE_FLG_WARM);
}