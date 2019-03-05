#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "FileJob.h"
#include "Text.h"

///<summary>Get the drawing using a file opened somewhere else.</summary>
void mfGetAll(CFile& f, const CVec& vTrns)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	
	if (!trapsegs.IsEmpty())
	{
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEGS_SAFE, &trapsegs);
		trapsegs.RemoveAll();
	}
	char* pBuf = new char[CPrim::BUFFER_SIZE];

	CSeg *pSeg;
	while (filejob_GetNextSeg(f, 3, pBuf, pSeg))
	{
		pDoc->WorkLayerAddTail(pSeg); 
		trapsegs.AddTail(pSeg);
	}
	delete [] pBuf;

	trapsegs.Translate(vTrns);
	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEGS_SAFE_TRAP, &trapsegs);
}