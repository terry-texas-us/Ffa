#include "stdafx.h"

CBlock::CBlock(WORD wBlkTypFlgs, const CPnt& ptBase)
{
	m_wBlkTypFlgs = wBlkTypFlgs;
	m_ptBase = ptBase;
}

CBlock::CBlock(WORD wBlkTypFlgs, const CPnt& ptBase, const CString& strXRefPathName)
{
	m_wBlkTypFlgs = wBlkTypFlgs;
	m_ptBase = ptBase;
	m_strXRefPathName = strXRefPathName;
}

void CBlock::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = GetNext(pos);
		pPrim->GetExtents(ptMin, ptMax, tm);
	}
	
	// mak
	CPrimMark marker(1, 1, m_ptBase);
	marker.GetExtents(ptMin, ptMax, tm);
}