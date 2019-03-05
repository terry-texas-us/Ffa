#include "stdafx.h"
//
//#include "PegAEsys.h"
//#include "PegAEsysDoc.h"
//
//void CFilePeg2000::Load()
//{
//	ReadHeaderSection();
//	ReadTablesSection();
//	ReadBlocksSection();
//	ReadEntitiesSection();
//}
//void CFilePeg2000::ReadBlocksSection()
//{
//	//if (FilePeg_ReadWord(*this) != SECTION_BLOCKS)
//	//	throw "Exception CFilePeg: Expecting sentinel SECTION_BLOCKS.";
//
//	//CPrim* pPrim;
//	//CString strName;
//	//char szXRefPathNam[32];
//
//	//WORD wTblSize = FilePeg_ReadWord(*this);
//
//	//for (WORD w = 0; w < wTblSize; w++)
//	//{
//	//	WORD wPrimCount = FilePeg_ReadWord(*this);
//	//	
//	//	FilePeg_ReadString(*this, strName);
//	//	WORD wBlkTypFlgs = FilePeg_ReadWord(*this);
//	//	CPnt pt;
//	//	pt.Read(*this);
//	//	
//	//	CBlock* pBlock = new CBlock(wBlkTypFlgs, pt, szXRefPathNam);
//	//	CPegDoc::GetDoc()->BlksSetAt(strName, pBlock);
//
//	//	for (WORD wPrim = 0; wPrim < wPrimCount; wPrim++)
//	//	{
//	//		FilePeg_ReadPrim(*this, pPrim);
//	//		pBlock->AddTail(pPrim); 
//	//	}
//	//}
//	//if (FilePeg_ReadWord(*this) != SECTION_END)
//	//	throw "Exception CFilePeg: Expecting sentinel SECTION_END.";
//
//	//////////////////////////////////////////////////////////////////////////
//
//	VERIFY(ReadWord() == SECTION_BLOCKS);	// Section.Blocks
//
//	CPrim* pPrim;
//	char szNam[32];
//	char szXRefPathNam[32];
//
//	WORD wTblSize = ReadWord();
//
//	for (WORD w = 0; w < wTblSize; w++)
//	{
//		WORD wPrimCount = ReadWord();
//		
//		ReadDelimitedString(szNam, 32);
//		WORD wBlkTypFlgs = ReadWord();
//		CPnt pt;
//		pt.Read(*this);
//		
//		CBlock* pBlock = new CBlock(wBlkTypFlgs, pt, szXRefPathNam);
//		CPegDoc::GetDoc()->BlksSetAt(szNam, pBlock);
////		dwg.BlockSetAt(szNam, pBlock);
//
//		for (WORD wPrim = 0; wPrim < wPrimCount; wPrim++)
//		{
//			primRead(*this, pPrim);
//			pBlock->AddTail(pPrim); 
//		}
//	}
//	VERIFY(ReadWord() == SECTION_END); // Section.End
//}
//
//void CFilePeg2000::ReadEntitiesSection()
//{
//	//if (FilePeg_ReadWord(*this) != SECTION_SEGMENTS)
//	//	throw "Exception CFilePeg: Expecting sentinel SECTION_SEGMENTS.";
//
//	//CPrim* pPrim;
//
//	//WORD wTblSize = FilePeg_ReadWord(*this);
//
//	//for (WORD w = 0; w < wTblSize; w++)
//	//{
//	//	CLayer* pLayer = CPegDoc::GetDoc()->LayersGetAt(w);
//	//	
//	//	if (pLayer == 0)
//	//		return;
//
//	//	WORD wSegCount = FilePeg_ReadWord(*this);
//
//	//	if (pLayer->IsInternal())
//	//	{
//	//		for (WORD wSeg = 0; wSeg < wSegCount; wSeg++)
//	//		{
//	//			CSeg* pSeg = new CSeg;
//	//			WORD wPrimCount = FilePeg_ReadWord(*this);
//	//			for (WORD wPrim = 0; wPrim < wPrimCount; wPrim++)
//	//			{
//	//				FilePeg_ReadPrim(*this, pPrim);
//	//				pSeg->AddTail(pPrim);
//	//			}
//	//			pLayer->AddTail(pSeg); 
//	//		}
//	//	}
//	//	else
//	//		CPegDoc::GetDoc()->TracingLoadLayer(pLayer->GetName(), pLayer);
//	//}
//	//if (FilePeg_ReadWord(*this) != SECTION_END)
//	//	throw "Exception CFilePeg: Expecting sentinel SECTION_END.";
//
//	////////////////////////////////////////////////////////////////////////////
//
//		VERIFY(ReadWord() == SECTION_ENTITIES);
//	CPrim* pPrim;
//
//	WORD wTblSize = ReadWord();
//
//	for (WORD w = 0; w < wTblSize; w++)
//	{
////		CLayer* pLayer = dwg.LayerGetAt(w);
//		CLayer* pLayer = CPegDoc::GetDoc()->LayersGetAt(w);
//		
//		if (pLayer == NULL)
//			return;// (FALSE);
//
//		WORD wSegCount = ReadWord();
//
//		if (pLayer->IsInternal())
//		{
//			for (WORD wSeg = 0; wSeg < wSegCount; wSeg++)
//			{
//				CSeg* pSeg = new CSeg;
//				WORD wPrimCount = ReadWord();
//				for (WORD wPrim = 0; wPrim < wPrimCount; wPrim++)
//				{
//					primRead(*this, pPrim);
//					pSeg->AddTail(pPrim);
//				}
////				pLayer->SegAdd(pSeg); 
//				pLayer->AddTail(pSeg);
//			}
//		}
//		else
//			//dwg.TracingLoadLayer(pLayer->GetName(), pLayer);
//			CPegDoc::GetDoc()->TracingLoadLayer(pLayer->GetName(), pLayer);
//	}
//	VERIFY(ReadWord() == SECTION_END);
//}
//
//void CFilePeg2000::ReadHeaderSection()
//{
//	//if (FilePeg_ReadWord(*this) != SECTION_HEADER)
//	//	throw "Exception CFilePeg: Expecting sentinel SECTION_HEADER.";
//
//	//// 	with addition of info here will loop key-value pairs till SECTION_END sentinel
//	//
//	//if (FilePeg_ReadWord(*this) != SECTION_END)
//	//	throw "Exception CFilePeg: Expecting sentinel SECTION_END.";
//
//	///////////////////////////////////////////////////////////////////////////////////
//
//	VERIFY(ReadWord() == SECTION_HEADER);	// Section.Header
//
//	// header variable items go here
//	
//	VERIFY(ReadWord() == SECTION_END); // Section.End
//}
//
//void CFilePeg2000::ReadLayerTable()
//{
//	//if (FilePeg_ReadWord(*this) != TABLE_LAYER)
//	//	throw "Exception CFilePeg: Expecting sentinel TABLE_LAYER.";
//	//
//	CPegDoc* pDoc = CPegDoc::GetDoc();
//
//	//CString strName;
//	//CString strPenStyleName;
// //
//	//WORD wTblSize = FilePeg_ReadWord(*this);
//
//	//for (WORD w = 0; w < wTblSize; w++)
//	//{
//	//	FilePeg_ReadString(*this, strName);
//	//	WORD wTracingFlgs = FilePeg_ReadWord(*this);
//	//	WORD wStateFlgs = FilePeg_ReadWord(*this);
//	//	
//	//	wStateFlgs |= CLayer::STATE_FLG_RESIDENT;	
//	//	
//	//	if ((wStateFlgs & CLayer::STATE_FLG_INTERNAL) != CLayer::STATE_FLG_INTERNAL)
//	//	{	
//	//		// HACK is this correct
//	//		if (strName.Find('.') == - 1)
//	//			strName += ".jb1";
//	//	}
//	//	PENCOLOR nPenColor = PENCOLOR(FilePeg_ReadWord(*this));
//	//	FilePeg_ReadString(*this, strPenStyleName);
//	//	
//	//	CLayer *pLayer;
//
//	//	if (pDoc->LayersLookup(strName) < 0)
//	//	{
//	//		pLayer = new CLayer(strName, wStateFlgs);
//	//		
//	//		pLayer->SetTracingFlg(wTracingFlgs);
//	//		pLayer->SetPenColor(nPenColor);
//	//		pLayer->SetPenStyleName(strPenStyleName);
//	//		pDoc->LayersAdd(pLayer);
//	//	}
//	//	else
//	//		pLayer = pDoc->LayersGetAt(pDoc->LayersLookup(strName));
//
//	//	if(pLayer->IsHot())
//	//		pDoc->WorkLayerSet(pLayer);
//	//}
//	//if (FilePeg_ReadWord(*this) != TABLE_END)
//	//	throw "Exception CFilePeg: Expecting sentinel TABLE_END.";
//
//	//////////////////////////////////////////////////////////////////////////
//
//		VERIFY(ReadWord() == TABLE_LAYER); // Table.Layer
//	
//	char* pName = new char[MAX_PATH];
//	
//	char szLnTypName[32];
// 
//	WORD wTblSize = ReadWord();
//
//	for (WORD w = 0; w < wTblSize; w++)
//	{
//		ReadDelimitedString(pName, 256);
//		WORD wTracingFlgs = ReadWord();
//		WORD wStateFlgs = ReadWord();
//		
//		wStateFlgs |= STATE_FLG_RESIDENT;	
//		
//		if ((wStateFlgs & STATE_FLG_INTERNAL) != STATE_FLG_INTERNAL)
//		{	
//			if (strchr(pName, '.') == NULL)
//				strcat(pName, ".jb1");
//		}
//		
//		PENCOLOR nPenColor = PENCOLOR(ReadWord());
//		ReadDelimitedString(szLnTypName, 32);
//
////		if (dwg.LayerLookup(pName) < 0)
//		if(pDoc->LayersLookup(pName) < 0)
//		{
//			CLayer* pLayer = new CLayer(pName, wStateFlgs);
//			
////			pLayer->SetTracingFlg(wTracingFlgs);
////			pLayer->SetColId(nPenColor);
////			pLayer->SetLnTypNam(szLnTypName);
////			dwg.LayerAdd(pLayer);
//			pLayer->SetTracingFlg(wTracingFlgs);
//			pLayer->SetPenColor(nPenColor);
//			pLayer->SetPenStyleName(szLnTypName);
//			pDoc->LayersAdd(pLayer);
//			
//			if (pLayer->IsHot())
////				dwg.WorkLayerSet(pLayer);
//				pDoc->WorkLayerSet(pLayer);
//		}
//	}
//	delete [] pName;
//	VERIFY(ReadWord() == TABLE_END);	// Table.End
//}
//
//void CFilePeg2000::ReadLnTypsTable()
//{
//	//if (FilePeg_ReadWord(*this) != TABLE_LTYPE)
//	//	throw "Exception CFilePeg: Expecting sentinel TABLE_LTYPE.";
//
//	//CString strName;
//	//CString strDescr;
//	//WORD wDefLen;
//	//WORD iStdFlgs;
//	//double* dDash = new double[32];
//
//	//double dPatternLen;
//
//	//WORD wTblSize = FilePeg_ReadWord(*this);
//
//	//for (WORD w = 0; w < wTblSize; w++)
//	//{
//	//	FilePeg_ReadString(*this, strName);
//	//	// standard flag values 	??
//	//	iStdFlgs = FilePeg_ReadWord(*this);
//	//	FilePeg_ReadString(*this, strDescr);
//	//	
//	//	wDefLen = FilePeg_ReadWord(*this);
//	//	FilePeg_ReadDouble(*this, dPatternLen);
// //
//	//	for (WORD wDash = 0; wDash < wDefLen; wDash++)
//	//		FilePeg_ReadDouble(*this, dDash[wDash]);		
//	//	
//	//	if (CPegDoc::GetDoc()->PenStylesLookup(strName) == PENSTYLE_LookupErr)
//	//		CPegDoc::GetDoc()->PenStylesAdd(new CPenStyle(strName, strDescr, wDefLen, dDash));
//	//}
//	//delete [] dDash;
//	//
//	//if (FilePeg_ReadWord(*this) != TABLE_END)
//	//	throw "Exception CFilePeg: Expecting sentinel TABLE_END.";
//
//	/////////////////////////////////////////////////////////////////////////////
//
//		VERIFY(ReadWord() == TABLE_LTYPE); 	// Table.Ltype
//
//	char szDescr[128];
//	char szName[32];
//	WORD wDefLen;
//	WORD iStdFlgs;
//	double* dDash = new double[32];
//
//	double dPatternLen;
//
////	CLineTypes*	pLineTypes = dwg.GetLineTypes();
//
//	WORD wTblSize;
//	
//	wTblSize = ReadWord();
//
//	for (WORD w = 0; w < wTblSize; w++)
//	{
//		ReadDelimitedString(szName, sizeof(szName));
//		// standard flag values 	??
//		iStdFlgs = ReadWord();
//		ReadDelimitedString(szDescr, sizeof(szDescr));
//		
//		wDefLen = ReadWord();
//		CFile::Read(&dPatternLen, sizeof(double));
// 
//		for (WORD wDash = 0; wDash < wDefLen; wDash++)
//		{
//			CFile::Read(&dDash[wDash], sizeof(double));		
//		}
////		if (pLineTypes->FindNam(szName) == PENSTYLE_LookupErr)
////			pLineTypes->Add(new CLineType(szName, szDescr, wDefLen, dDash));
//		if (CPegDoc::GetDoc()->PenStylesLookup(szName) == PENSTYLE_LookupErr)
//			CPegDoc::GetDoc()->PenStylesAdd(new CPenStyle(szName, szDescr, wDefLen, dDash));
//	}
//	delete [] dDash;
//	VERIFY(ReadWord() == TABLE_END);	// Table.End
//}
//void CFilePeg2000::ReadTablesSection()
//{
//	//if (FilePeg_ReadWord(*this) != SECTION_TABLES)
//	//	throw "Exception CFilePeg: Expecting sentinel SECTION_TABLES.";
//
//	//ReadVPortTable();
//	//ReadLnTypsTable();
//	//ReadLayerTable();
//
//	//if (FilePeg_ReadWord(*this) != SECTION_END)
//	//	throw "Exception CFilePeg: Expecting sentinel SECTION_END.";
//
//	////////////////////////////////////////////////////////////////////////////
//
//	VERIFY(ReadWord() == SECTION_TABLES);	// Section.Tables
//	ReadVPortTable();
//	ReadLnTypsTable();
//	ReadLayerTable();
//	VERIFY(ReadWord() == SECTION_END); // Section.End
//}
//void CFilePeg2000::ReadVPortTable()
//{
//	//if (FilePeg_ReadWord(*this) != TABLE_VPORT)
//	//	throw "Exception CFilePeg: Expecting sentinel TABLE_VPORT.";
//
//	//FilePeg_ReadWord(*this);
//
//	//if(FilePeg_ReadWord(*this) != TABLE_END)
//	//	throw "Exception CFilePeg: Expecting sentinel TABLE_END.";
//
//	/////////////////////////////////////////////////////////////////////////////
//
//	VERIFY(ReadWord() == TABLE_VPORT); // Table.Vport
//	ReadWord();
//	VERIFY(ReadWord() == TABLE_END);	// Table.End
//}	
////void CFilePeg::Unload()
////{
////	CFile::SetLength(0);
////	CFile::SeekToBegin();
////	
////	WriteHeaderSection();
////	WriteTablesSection();
////	WriteBlocksSection();
////	WriteEntitiesSection();
////	FilePeg_WriteString(*this, "EOF");
////	
////	CFile::Flush();
////}
////void CFilePeg2000::WriteBlocksSection()
////{
////	CPegDoc* pDoc = CPegDoc::GetDoc();
////
////	FilePeg_WriteWord(*this, SECTION_BLOCKS);
////
////	WORD wTblSize = pDoc->BlksSize();
////	FilePeg_WriteWord(*this, wTblSize);
////
////	CString strKey;
////	CBlock* pBlock;
////
////	POSITION pos = pDoc->BlksGetStartPosition();
////	while (pos != 0)
////	{
////		pDoc->BlksGetNextAssoc(pos, strKey, pBlock);
////		
////		ULONGLONG dwCountPosition = CFile::GetPosition();
////		FilePeg_WriteWord(*this, 0);	
////		WORD iPrimCount = 0;
////		
////		FilePeg_WriteString(*this, strKey);
////		FilePeg_WriteWord(*this, pBlock->GetBlkTypFlgs());
////		pBlock->GetBasePt().Write(*this);
////		
////// TODO: if block is an xref add a pathname
////
////		POSITION posPrim = pBlock->GetHeadPosition();
////		while (posPrim != 0)
////		{
////			CPrim* pPrim = pBlock->GetNext(posPrim);
////			if (pPrim->Write(*this))
////				iPrimCount++;
////		}
////		ULONGLONG dwPosition = CFile::GetPosition();
////		CFile::Seek(dwCountPosition, CFile::begin);
////		FilePeg_WriteWord(*this, iPrimCount);
////		CFile::Seek(dwPosition, CFile::begin);
////	}
////
////	FilePeg_WriteWord(*this, SECTION_END);
////}
////void CFilePeg2000::WriteEntitiesSection()
////{
////	CPegDoc* pDoc = CPegDoc::GetDoc();
////
////	FilePeg_WriteWord(*this, SECTION_SEGMENTS);
////
////	int iTblSize = pDoc->LayersGetSize();
////	FilePeg_WriteWord(*this, WORD(iTblSize));
////
////	for (int i = 0; i < iTblSize; i++)
////	{
////		CLayer* pLayer = pDoc->LayersGetAt(i);
////		if (pLayer->IsInternal())
////		{
////			FilePeg_WriteWord(*this, (WORD) pLayer->GetCount());
////		
////			POSITION pos = pLayer->GetHeadPosition();
////			while (pos != 0)
////			{
////				CSeg* pSeg = pLayer->GetNext(pos);
////				pSeg->Write(*this);
////			}
////		}
////		else
////			FilePeg_WriteWord(*this, 0);	
////	}
////	FilePeg_WriteWord(*this, SECTION_END);
////}
////void CFilePeg2000::WriteHeaderSection()
////{
////	FilePeg_WriteWord(*this, SECTION_HEADER);
////
////	// header variable items go here
////	
////	FilePeg_WriteWord(*this, SECTION_END);
////}
////void CFilePeg2000::WriteLayerTable()
////{
////	CPegDoc* pDoc = CPegDoc::GetDoc();
////
////	int iTblSize = pDoc->LayersGetSize();
////
////	FilePeg_WriteWord(*this, TABLE_LAYER);
////
////	ULONGLONG dwTblSizePosition = CFile::GetPosition();
////	FilePeg_WriteWord(*this, WORD(iTblSize));
////
////	for (int i = 0; i < pDoc->LayersGetSize(); i++)
////	{
////		CLayer* pLayer = pDoc->LayersGetAt(i);
////		if (pLayer->IsResident())
////		{
////			FilePeg_WriteString(*this, pLayer->GetName());
////			FilePeg_WriteWord(*this, pLayer->GetTracingFlgs());
////			FilePeg_WriteWord(*this, pLayer->GetStateFlgs());
////			FilePeg_WriteWord(*this, pLayer->PenColor());
////			FilePeg_WriteString(*this, pLayer->GetPenStyleName());
////		}
////		else
////			iTblSize--;
////	}
////	FilePeg_WriteWord(*this, TABLE_END);
////	
////	if (iTblSize != pDoc->LayersGetSize())
////	{
////		ULONGLONG dwPosition = CFile::GetPosition();
////		CFile::Seek(dwTblSizePosition, CFile::begin);
////		FilePeg_WriteWord(*this, WORD(iTblSize));
////		CFile::Seek(dwPosition, CFile::begin);
////	}
////}
////void CFilePeg2000::WritePenStyleTable()
////{
////	//CPegDoc* pDoc = CPegDoc::GetDoc();
////
////	//WORD wTblSize = WORD(pDoc->PenStylesGetSize());
////
////	//FilePeg_WriteWord(*this, TABLE_LTYPE);
////	//FilePeg_WriteWord(*this, wTblSize);
////
////	//for (int i = 0; i < wTblSize; i++)
////	//{
////	//	CPenStyle* pPenStyle = pDoc->PenStylesGetAt(i);
////	//	
////	//	FilePeg_WriteString(*this, pPenStyle->GetName());
////	//	// standard flag values 	??
////	//	FilePeg_WriteWord(*this, 0);
////	//	FilePeg_WriteString(*this, pPenStyle->GetDescription());
////	//	
////	//	WORD wDefLen = pPenStyle->GetDefLen();
////	//	FilePeg_WriteWord(*this, wDefLen);
////	//	double dPatternLen = pPenStyle->GetPatternLen();
////	//	FilePeg_WriteDouble(*this, dPatternLen);
////
////	//	if (wDefLen > 0)
////	//	{
////	//		double* dDash = new double[wDefLen];
////	//		pPenStyle->GetDashLen(dDash);
////	//		for (WORD w = 0; w < wDefLen; w++)
////	//			FilePeg_WriteDouble(*this, dDash[w]);
////
////	//		delete [] dDash;
////	//	}
////	//}
////	//FilePeg_WriteWord(*this, TABLE_END);
////
////	/////////////////////////////////////////////////////////////////////////////////////////////
////
////		CLineTypes*	pLineTypes = dwg.GetLineTypes();
////	WORD wTblSize = pLineTypes->GetSize();
////
////	WriteWord(TABLE_LTYPE);
////	WriteWord(wTblSize);
////
////	for (int i = 0; i < wTblSize; i++)
////	{
////		CLineType* pLineType = (CLineType*) pLineTypes->GetAt(i);
////		
////		WriteString(pLineType->GetName());
////		// standard flag values 	??
////		WriteWord(0);
////		WriteString(pLineType->GetDescription());
////		
////		WORD wDefLen = pLineType->GetDefLen();
////		WriteWord(wDefLen);
////		double dPatternLen = pLineType->GetPatternLen();
////		CFile::Write(&dPatternLen, sizeof(double));
////
////		if (wDefLen > 0)
////		{
////			double* dDash = new double[wDefLen];
////			pLineType->GetDashLen(dDash);
////			for (WORD w = 0; w < wDefLen; w++)
////				CFile::Write(&dDash[w], sizeof(double));
////
////			delete [] dDash;
////		}
////	}
////	WriteWord(TABLE_END);
////}
////void CFilePeg2000::WriteTablesSection()
////{
////	FilePeg_WriteWord(*this, SECTION_TABLES);
////	
////	WriteVPortTable();
////	WritePenStyleTable();
////	WriteLayerTable();
////	FilePeg_WriteWord(*this, SECTION_END);
////}
////void CFilePeg2000::WriteVPortTable()
////{
////	FilePeg_WriteWord(*this, TABLE_VPORT);
////	FilePeg_WriteWord(*this, 0);
////	FilePeg_WriteWord(*this, TABLE_END);
////}
//
//////////////////////////////////////////////////////////////////////////////////////////////////////
//void CFilePeg2000::ReadDelimitedString(char* szBuf, UINT nCount)
//{
//	for (WORD w = 0; w < nCount; w++)
//	{
//		CFile::Read(&szBuf[w], 1);
//		if (szBuf[w] == '\t')
//		{
//			szBuf[w] = NULL;
//			break;
//		}
//	}
//}
//
//WORD CFilePeg2000::ReadWord()
//{
//	WORD wBuf;
//	CFile::Read(&wBuf, sizeof(WORD));
//	return (wBuf);
//}
//
//void CFilePeg2000::Unload()
//{
//	CFile::SetLength(0);
//	CFile::SeekToBegin();
//	
//	WriteHeaderSection();
//	WriteTablesSection();
//	WriteBlocksSection();
//	WriteEntitiesSection();
//	WriteString("EOF");
//	
//	CFile::Flush();
//}
//
//void CFilePeg2000::WriteBlocksSection()
//{
//	CPegDoc *pDoc = CPegDoc::GetDoc();
//
//	WriteWord(SECTION_BLOCKS);
//
////	WORD wTblSize = dwg.BlocksGetCount();
//	WORD wTblSize = pDoc->BlksSize();
//	WriteWord(wTblSize);
//
//	CString sNam;
//	CString strKey;
//	CBlock* pBlock;
//
////	POSITION pos = dwg.BlocksGetStartPosition();
//	POSITION pos = pDoc->BlksGetStartPosition();
//	while (pos != NULL)
//	{
//		DWORD dwCountPosition = CFile::GetPosition();
//		WriteWord(0);	
//		WORD iPrimCount = 0;
//		
////		pBlock = dwg.BlocksGetNext(pos, sNam);
//		pDoc->BlksGetNextAssoc(pos, strKey, pBlock);
//
//		WriteString(sNam);
//		WriteWord(pBlock->GetBlkTypFlgs());
//		pBlock->GetBasePt().Write(*this);
//		// if block is an xref add a pathname
//
//		POSITION posPrim = pBlock->GetHeadPosition();
//		while (posPrim != NULL)
//		{
//			CPrim* pPrim = pBlock->GetNext(posPrim);//GetNextPrim(posPrim);
//			if (pPrim->Write(*this))
//				iPrimCount++;
//		}
//		DWORD dwPosition = CFile::GetPosition();
//		CFile::Seek(dwCountPosition, CFile::begin);
//		WriteWord(iPrimCount);
//		CFile::Seek(dwPosition, CFile::begin);
//	}
//
//	WriteWord(SECTION_END);
////	return (true);
//}
//
//void CFilePeg2000::WriteEntitiesSection()
//{
//	CPegDoc *pDoc = CPegDoc::GetDoc();
//
//	WriteWord(SECTION_ENTITIES);
//
////	WORD wTblSize = dwg.GetLayerTblSize();
//	WORD wTblSize = pDoc->LayersGetSize();
//	WriteWord(wTblSize);
//
//	for (int i = 0; i < wTblSize; i++)
//	{
////		CLayer* pLayer = dwg.LayerGetAt(i);
//		CLayer *pLayer = pDoc->LayersGetAt(i);
//		if (pLayer->IsInternal())
//		{
//			dwgWriteWord(*this, pLayer->GetCount());
//		
//			POSITION pos = pLayer->GetHeadPosition();
//			while (pos != NULL)
//			{
////				CSeg* pSeg = pLayer->SegGetNext(pos);
//				CSeg *pSeg = pLayer->GetNext(pos);
////				pSeg->WritePeg2000(*this);
//
//			}
//		}
//		else
//			WriteWord(0);	
//	}
//	WriteWord(SECTION_END);
////	return (true);
//}
//
//void CFilePeg2000::WriteHeaderSection()
//{
//	WriteWord(SECTION_HEADER);
//
//	// header variable items go here
//	
//	WriteWord(SECTION_END);
////	return (true);
//}
//
//void CFilePeg2000::WriteLayerTable()
//{
//	CPegDoc *pDoc = CPegDoc::GetDoc();
//
////	WORD wTblSize = dwg.GetLayerTblSize();
//	WORD wTblSize = pDoc->LayersGetSize();
//
//	WriteWord(TABLE_LAYER);
//
//	DWORD dwTblSizePosition = CFile::GetPosition();
//	WriteWord(wTblSize);
//
////	for (int i = 0; i < dwg.GetLayerTblSize(); i++)
//	for (int i = 0; i < pDoc->LayersGetSize(); i++)
//	{
////		CLayer* pLayer = dwg.LayerGetAt(i);
//		CLayer* pLayer = pDoc->LayersGetAt(i);
//		if (pLayer->IsResident())
//		{
//			WriteString(pLayer->GetName());
//			WriteWord(pLayer->GetTracingFlgs());
//			WriteWord(pLayer->GetStateFlgs());
////			dwgWriteWord(*this, pLayer->GetColId());
//			dwgWriteWord(*this, pLayer->PenColor());
////			WriteString(pLayer->GetLnTypNam());
//			WriteString(pLayer->GetPenStyleName());
//		}
//		else
//			wTblSize--;
//	}
//	WriteWord(TABLE_END);
//	
////	if (wTblSize != dwg.GetLayerTblSize())
//	if (wTblSize != pDoc->LayersGetSize())
//	{
//		DWORD dwPosition = CFile::GetPosition();
//		CFile::Seek(dwTblSizePosition, CFile::begin);
//		WriteWord(wTblSize);
//		CFile::Seek(dwPosition, CFile::begin);
//	}
////	return (true);
//}
//
//void CFilePeg2000::WritePenStyleTable()
//{
//	CPegDoc* pDoc = CPegDoc::GetDoc();
//
//	//WORD wTblSize = WORD(pDoc->PenStylesGetSize());
//
//	//FilePeg_WriteWord(*this, TABLE_LTYPE);
//	//FilePeg_WriteWord(*this, wTblSize);
//
//	//for (int i = 0; i < wTblSize; i++)
//	//{
//	//	CPenStyle* pPenStyle = pDoc->PenStylesGetAt(i);
//	//	
//	//	FilePeg_WriteString(*this, pPenStyle->GetName());
//	//	// standard flag values 	??
//	//	FilePeg_WriteWord(*this, 0);
//	//	FilePeg_WriteString(*this, pPenStyle->GetDescription());
//	//	
//	//	WORD wDefLen = pPenStyle->GetDefLen();
//	//	FilePeg_WriteWord(*this, wDefLen);
//	//	double dPatternLen = pPenStyle->GetPatternLen();
//	//	FilePeg_WriteDouble(*this, dPatternLen);
//
//	//	if (wDefLen > 0)
//	//	{
//	//		double* dDash = new double[wDefLen];
//	//		pPenStyle->GetDashLen(dDash);
//	//		for (WORD w = 0; w < wDefLen; w++)
//	//			FilePeg_WriteDouble(*this, dDash[w]);
//
//	//		delete [] dDash;
//	//	}
//	//}
//	//FilePeg_WriteWord(*this, TABLE_END);
//
//	//////////////////////////////////////////////////////////////////////////////
//
////	CLineTypes*	pLineTypes = dwg.GetLineTypes();
////	WORD wTblSize = pLineTypes->GetSize();
//	WORD wTblSize = WORD(pDoc->PenStylesGetSize());
//
//	WriteWord(TABLE_LTYPE);
//	WriteWord(wTblSize);
//
//	for (int i = 0; i < wTblSize; i++)
//	{
////		CLineType* pLineType = (CLineType*) pLineTypes->GetAt(i);
//		CPenStyle* pPenStyle = pDoc->PenStylesGetAt(i);
//		
////		WriteString(pLineType->GetName());
//		WriteString(pPenStyle->GetName());
//		// standard flag values 	??
//		WriteWord(0);
////		WriteString(pLineType->GetDescription());
//		WriteString(pPenStyle->GetDescription());
//		
////		WORD wDefLen = pLineType->GetDefLen();
//		WORD wDefLen = pPenStyle->GetDefLen();
//		WriteWord(wDefLen);
////		double dPatternLen = pLineType->GetPatternLen();
//		double dPatternLen = pPenStyle->GetPatternLen();
//		CFile::Write(&dPatternLen, sizeof(double));
//
//		if (wDefLen > 0)
//		{
//			double* dDash = new double[wDefLen];
////			pLineType->GetDashLen(dDash);
//			pPenStyle->GetDashLen(dDash);
//			for (WORD w = 0; w < wDefLen; w++)
//				CFile::Write(&dDash[w], sizeof(double));
//
//			delete [] dDash;
//		}
//	}
//	WriteWord(TABLE_END);
//
////	return (true);
//}
//
////bool CFilePeg2000::WriteShadowFile()
////{
////	CString sShadowFilePath = app.GetShadowDir() + CFile::GetFileName();
////	
////	int iExt = sShadowFilePath.Find('.');
////	if (iExt > 0)
////	{
////		CFileStatus fs;
////		CFile::GetStatus(fs);
////
////		char szInfo[16];
////		strcpy(szInfo, fs.m_mtime.Format("_%Y%m%d%H%M"));
////		
////		sShadowFilePath = sShadowFilePath.Left(iExt);
////		sShadowFilePath += szInfo;
////		sShadowFilePath += ".peg";
////		
////		CFileException e;
////		CFilePeg fpShadow;
////		if (!fpShadow.Open(sShadowFilePath, CFile::modeWrite, &e))
////		{	
////			fpShadow.Open(sShadowFilePath, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive, &e);
////			fpShadow.Unload();
////			msgWarning(IDS_MSG_FILE_SHADOWED_AS, sShadowFilePath);
////			return (true);
////		}
////		msgWarning(IDS_MSG_SHADOW_FILE_CREATE_FAILURE);
////	}
////	return (false);
////}
//
//void CFilePeg2000::WriteTablesSection()
//{
//	WriteWord(SECTION_TABLES);
//	
//	WriteVPortTable();
////	WriteLineTypeTable();
//	WritePenStyleTable();
//	WriteLayerTable();
//	WriteWord(SECTION_END);
//}
//
//void CFilePeg2000::WriteVPortTable()
//{
//	WriteWord(TABLE_VPORT);
//	WriteWord(0);
//	WriteWord(TABLE_END);
//}
//
/////////////////////////////////////////////////////////////////////////////////////////////////
//
////void dwgReadDelimitedString(CFile& fl, char* szBuf, UINT nCount)
////{
////	for (WORD w = 0; w < nCount; w++)
////	{
////		fl.Read(&szBuf[w], 1);
////		if (szBuf[w] == '\t')
////		{
////			szBuf[w] = NULL;
////			break;
////		}
////	}
////}
//
//bool primRead(CFile& fl, CPrim*& pPrim)
//{
//	WORD wEntityTyp = dwgReadWord(fl);
//
//	switch (wEntityTyp)
//	{
//		case CFilePeg2000::BSPLINE_PRIMITIVE:
//			pPrim = new CPrimBSpline;
//			break;
//
//		case CFilePeg2000::ARC_PRIMITIVE:
//			pPrim = new CPrimArc;
//			break;
//
//		case CFilePeg2000::CSPLINE_PRIMITIVE:
//			pPrim = new CPrimCSpline;
//			break;
//
//		case CFilePeg2000::POLYGON_PRIMITIVE:
//			pPrim = new CPrimPolygon;
//			break;
//
//		case CFilePeg2000::INSERT_PRIMITIVE:
//			pPrim = new CPrimInsert;
//			break;
//
//		case CFilePeg2000::LINE_PRIMITIVE:
//			pPrim = new CPrimLine;
//			break;
//
//		case CFilePeg2000::MARK_PRIMITIVE:
//			pPrim = new CPrimMark;
//			break;
//
//		case CFilePeg2000::NOTE_PRIMITIVE:
//			pPrim = new CPrimText;
//			break;
//
//		case CFilePeg2000::POLYLINE_PRIMITIVE:
//			pPrim = new CPrimPolyline;
//			break;
//		
//		case CFilePeg2000::TAG_PRIMITIVE:
//			pPrim = new CPrimTag;
//			break;
//
//		default:
//			msgWarning(IDS_MSG_BAD_PRIM_TYPE);
//			return (false);
//	}
//	pPrim->Read(fl);
//	return (true);
//}
//
//WORD dwgReadWord(CFile& fl)
//{
//	WORD wBuf;
//	fl.Read(&wBuf, sizeof(WORD));
//	return (wBuf);
//}
//
//void dwgWriteWord(CFile& fl, int iGroupValue)
//{
//	WORD wGroupValue = WORD(iGroupValue);
//
//	fl.Write(&wGroupValue, sizeof(WORD));
//}
//
//void dwgWriteWord(CFile& fl, WORD iGroupValue)
//{
//	fl.Write(&iGroupValue, sizeof(WORD));
//}
//
////void dwgWriteString(CFile& fl, const CString& sGroupValue)
////{
////	fl.Write(sGroupValue + "\t", sGroupValue.GetLength() + 1);
////}