#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

// SECTION_HEADER sentinel						WORD 0x0101
//		{0 or more key-value pairs}
// SECTION_END sentinel							WORD 0x01ff
// SECTION_TABLES sentinel						WORD 0x0102
//		TABLE_VPORT sentinel					WORD 0x0201
//			{0 or more viewport definitions}	WORD
//		TABLE_END sentinel						WORD 0x02ff
//		TABLE_LTYPE sentinel					WORD 0x0202
//			Number of linetypes					WORD
//			{0 or more linetype definitions}
//		TABLE_END sentinel						WORD 0x02ff
//		TABLE_LAYER sentinel					WORD 0x0203
//			Number of layers (resident only)	WORD
//			{0 or more layer definitions}
//		TABLE_END sentinel						WORD 0x02ff
// SECTION_END sentinel							WORD 0x01ff
// SECTION_BLOCKS sentinel						WORD 0x0103
//		Number of blocks						WORD
//		{0 or more block definitions}
// SECTION_END sentinel							WORD 0x01ff
// SECTION_SEGMENTS sentinel					WORD 0x0104
//		Number of segments						WORD
//		{0 or more segment definitions}
// SECTION_END sentinel							WORD 0x01ff

// Layer definition
//		Layers
//			Name								string
//			Tracing flags						WORD
//			State								WORD
//			Layer pen color						WORD
//			Layer pen style						string
//		Tracings
//			?? nothing

// Line style definition
//		Name									string
//		Flags (always 0)						WORD
//		Description								string
//		Definition length						WORD		
//		Pattern length							double
//		if (definition length > 0)
//			1 or more dash length				double...

// Block definition
//		Number of primitives					WORD
//		Name									string
//		Flags									WORD
//		Base point								Pnt
//		{0 or more primitive definitions}

// Segment definition
//		Number of primitives					WORD
//		{0 or more primitive definitions}

// Primitive definition
//		Mark primitive
//			PRIM_MARK type code					WORD 0x0100
//			Pen color							WORD
//			Mark style							WORD
//			Point								Pnt
//			Number of data values				WORD
//			{0 or more data values}
//		Insert primitive
//			PRIM_INSERT type code				WORD 0x0101
//			Pen color							WORD
//			Pen style							WORD
//			Insertion point						Pnt
//			Local reference x-axis				Vec
//			Local reference y-axis				Vec
//			Local reference z-axis				Vec
//			Number of columns					WORD
//			Number of rows						WORD
//			Column spacing						double
//			Row spacing							double
//		SegRef primitive
//			PRIM_SEGR type code					WORD 0x0102
//			Pen color							WORD
//			Pen style							WORD
//			Segment name						string
//			Insertion point						Pnt
//			Local normal vector					Vec
//			Scale factors(x, y, z)				Vec
//			Rotatation							double
//			Number of columns					WORD
//			Number of rows						WORD
//			Column spacing						double
//			Row spacing							double
//		Line primitive
//			PRIM_LINE type code					WORD 0x0200
//			Pen color							WORD
//			Pen style							WORD
//			Line								Ln
//		Polygon primitive
//			PRIM_POLYGON type code				WORD 0x0400
//			Pen color							WORD
//			Polygon style						WORD
//			Polygon Style Index					WORD
//			Number of verticies					WORD
//			Hatch origin						Pnt
//			Hatch/pattern reference x-axis		Vec
//			Hatch/pattern reference y-axis		Vec
//			{0 or more points}	
//		Arc primitive
//			PRIM_ARC type code					WORD 0x1003
//			Pen color							WORD
//			Pen style							WORD
//			Center point						Pnt
//			Major axis							Vec
//			Minor axis							Vec
//			Sweep angle							double
//		BSpline primitive
//			PRIM_BSPLINE type code				WORD 0x2000
//			Pen color							WORD
//			Pen style							WORD
//			Number of control points			WORD
//			{0 or more control points}
//		CSpline primitive
//			PRIM_CSPLINE type code				WORD 0x2001
//			Pen color							WORD
//			Pen style							WORD
//			m_wPtsS								WORD
//			Number of control points			WORD
//			End condition Type					WORD
//			Begin point tangent vector			Vec
//			End point tangent vector			Vec
//			{0 or more control points}
//		Polyline primitive (not ready for io)
//			PRIM_POLYLINE type code				WORD 0x2002
//			Pen color							WORD
//			Pen style							WORD
//			Number of points					WORD
//			{0 or more points}			
//		Text primitive
//			PRIM_TEXT type code					WORD 0x4000
//			Pen color							WORD
//			Pen style							WORD
//			Text precision						WORD
//			Text font name						string
//			Text path							WORD
//			Horizontal alignment				WORD
//			Vertical alignment					WORD
//			Character spacing					double
//			Insertion point						Pnt
//			Local reference x-axis				Vec
//			Local reference y-axis				Vec
//			Text								string
//		Tag primitive
//			PRIM_TAG type code					WORD 0x4100
//			Pen color							WORD
//			Pen style							WORD
//			Point								Pnt
//		Dim primitive 
//			(could be a problem. primdim not overloaded. 
//			 only its base primtext data has been streamed.
//			 has this always been so on tra/peg files.)

bool CFilePeg::Open(CString strPathName, DWORD wStyles, bool bReadOnly)
{
	DWORD dwAccess = wStyles;
	DWORD dwAttr = FILE_ATTRIBUTE_NORMAL;
	DWORD dwShare = 0;
	DWORD dwDisposition = OPEN_ALWAYS;

	if(bReadOnly)
	{
		dwShare = FILE_SHARE_READ | FILE_SHARE_WRITE;
	}
	else
		dwShare = FILE_SHARE_READ;

	hFile = (HFILE)::CreateFile(strPathName.GetBuffer(), dwAccess, dwShare, NULL, dwDisposition, dwAttr, NULL);

	return ((HANDLE)hFile != INVALID_HANDLE_VALUE);
}

void CFilePeg::Close()
{
	CloseHandle((HANDLE)hFile);
}

void CFilePeg::Seek(LONG lDist, DWORD dwMove)
{
	SetFilePointer((HANDLE)hFile, lDist, NULL, dwMove);
}

ULONGLONG CFilePeg::GetPosition()
{
	return SetFilePointer((HANDLE)hFile, 0, NULL, FILE_CURRENT);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CFilePeg::Load()
{
	try
	{
		ReadHeaderSection();
		ReadTablesSection();
		ReadBlocksSection();
		ReadEntitiesSection();
	}
	catch(char* szMessage)
	{
		msgInformation(szMessage);
		return;
	}
}

void CFilePeg::ReadBlocksSection()
{
	if(FilePeg_ReadWord2(hFile) != SECTION_BLOCKS)
		throw "Exception CFilePeg: Expecting sentinel SECTION_BLOCKS.";

	CPrim* pPrim;
	CString strName;
	char szXRefPathNam[32];

	WORD wTblSize = FilePeg_ReadWord2(hFile);

	for (WORD w = 0; w < wTblSize; w++)
	{
		WORD wPrimCount = FilePeg_ReadWord2(hFile);
		
		FilePeg_ReadString2(hFile, strName);
		WORD wBlkTypFlgs = FilePeg_ReadWord2(hFile);
		CPnt pt;
		pt.Read(hFile);
		
		CBlock* pBlock = new CBlock(wBlkTypFlgs, pt, szXRefPathNam);
		CPegDoc::GetDoc()->BlksSetAt(strName, pBlock);

		for (WORD wPrim = 0; wPrim < wPrimCount; wPrim++)
		{
			FilePeg_ReadPrim2(hFile, pPrim);
			pBlock->AddTail(pPrim); 
		}
	}
	
	if(FilePeg_ReadWord2(hFile) != SECTION_END)
		throw "Exception CFilePeg: Expecting sentinel SECTION_END.";
}

void CFilePeg::ReadEntitiesSection()
{
	if(FilePeg_ReadWord2(hFile) != SECTION_SEGMENTS)
		throw "Exception CFilePeg: Expecting sentinel SECTION_SEGMENTS.";

	CPrim* pPrim;

	WORD wTblSize = FilePeg_ReadWord2(hFile);

	for (WORD w = 0; w < wTblSize; w++)
	{
		CLayer* pLayer = CPegDoc::GetDoc()->LayersGetAt(w);
		
		if (pLayer == 0)
			return;

		WORD wSegCount = FilePeg_ReadWord2(hFile);

		if (pLayer->IsInternal())
		{
			for (WORD wSeg = 0; wSeg < wSegCount; wSeg++)
			{
				CSeg* pSeg = new CSeg;
				WORD wPrimCount = FilePeg_ReadWord2(hFile);
				for (WORD wPrim = 0; wPrim < wPrimCount; wPrim++)
				{
					FilePeg_ReadPrim2(hFile, pPrim);
					pSeg->AddTail(pPrim);
				}
				pLayer->AddTail(pSeg); 
			}
		}
		else
			CPegDoc::GetDoc()->TracingLoadLayer(pLayer->GetName(), pLayer);
	}
	
	if (FilePeg_ReadWord2(hFile) != SECTION_END)
		throw "Exception CFilePeg: Expecting sentinel SECTION_END.";
}

void CFilePeg::ReadHeaderSection()
{
	if (FilePeg_ReadWord2(hFile) != SECTION_HEADER)
		throw "Exception CFilePeg: Expecting sentinel SECTION_HEADER.";

	// 	with addition of info here will loop key-value pairs till SECTION_END sentinel
	
	if (FilePeg_ReadWord2(hFile) != SECTION_END)
		throw "Exception CFilePeg: Expecting sentinel SECTION_END.";
}

void CFilePeg::ReadLayerTable()
{
	if (FilePeg_ReadWord2(hFile) != TABLE_LAYER)
		throw "Exception CFilePeg: Expecting sentinel TABLE_LAYER.";
	
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CString strName;
	CString strPenStyleName;
 
	WORD wTblSize = FilePeg_ReadWord2(hFile);

	for (WORD w = 0; w < wTblSize; w++)
	{
		FilePeg_ReadString2(hFile, strName);
		WORD wTracingFlgs = FilePeg_ReadWord2(hFile);
		WORD wStateFlgs = FilePeg_ReadWord2(hFile);
		
		wStateFlgs |= CLayer::STATE_FLG_RESIDENT;	
		
		if ((wStateFlgs & CLayer::STATE_FLG_INTERNAL) != CLayer::STATE_FLG_INTERNAL)
		{	
			// HACK is this correct
			if (strName.Find('.') == -1)
				strName += ".jb1";
		}
		PENCOLOR nPenColor = short(FilePeg_ReadWord2(hFile));
		FilePeg_ReadString2(hFile, strPenStyleName);
		
		CLayer *pLayer;

		if (pDoc->LayersLookup(strName) < 0)
		{
			pLayer = new CLayer(strName, wStateFlgs);
			pDoc->LayersAdd(pLayer);
		}
		else
		{
			pLayer = pDoc->LayersGetAt(pDoc->LayersLookup(strName));
			pLayer->ClrStateFlg();
			pLayer->ClrTracingFlg();
		}
			
		if(pLayer == 0)
			continue;

		
		//if (pLayer->GetStateFlgs() == 0)
		//	MessageBox(0, pLayer->GetName() + " state clear", 0, 0);

		//if (pLayer->GetTracingFlgs() == 0)
		//	MessageBox(0, pLayer->GetName() + " tracing clear", 0, 0);

		//if (pLayer->IsHot())
		//	MessageBox(0, pLayer->GetName() + " is hot.11", 0, 0);
		//else if(pLayer->IsWarm())
		//	MessageBox(0, pLayer->GetName() + " is warm.11", 0, 0);
		//else if(pLayer->IsCold())
		//	MessageBox(0, pLayer->GetName() + " is cold.11", 0, 0);

		pLayer->SetTracingFlg(wTracingFlgs);
		
		// mak - fix?
		pLayer->SetStateFlg(wStateFlgs);

		pLayer->SetPenColor(nPenColor);
		pLayer->SetPenStyleName(strPenStyleName);

		//if (pLayer->IsHot())
		//	MessageBox(0, pLayer->GetName() + " is hot.22", 0, 0);
		//else if(pLayer->IsWarm())
		//	MessageBox(0, pLayer->GetName() + " is warm.22", 0, 0);
		//else if(pLayer->IsCold())
		//	MessageBox(0, pLayer->GetName() + " is cold.22", 0, 0);

		//if (pDoc->LayersLookup(strName) < 0)
		//{
		//	pLayer = new CLayer(strName, wStateFlgs);
		//	
		//	pLayer->SetTracingFlg(wTracingFlgs);
		//	
		//	// mak - fix?
		//	pLayer->SetStateFlg(wStateFlgs);

		//	pLayer->SetPenColor(nPenColor);
		//	pLayer->SetPenStyleName(strPenStyleName);
		//	pDoc->LayersAdd(pLayer);
		//}
		//else
		//	pLayer = pDoc->LayersGetAt(pDoc->LayersLookup(strName));

		if(pLayer->IsHot())
			pDoc->WorkLayerSet(pLayer);
	}
	
	if (FilePeg_ReadWord2(hFile) != TABLE_END)
		throw "Exception CFilePeg: Expecting sentinel TABLE_END.";
}

void CFilePeg::ReadLnTypsTable()
{
	if (FilePeg_ReadWord2(hFile) != TABLE_LTYPE)
		throw "Exception CFilePeg: Expecting sentinel TABLE_LTYPE.";

	CString strName;
	CString strDescr;
	WORD wDefLen;
	WORD iStdFlgs;
	double* dDash = new double[32];

	double dPatternLen;

	WORD wTblSize = FilePeg_ReadWord2(hFile);

	//if(wTblSize > 128)
	//	wTblSize = 128;

	for (WORD w = 0; w < wTblSize; w++)
	{
		FilePeg_ReadString2(hFile, strName);
		// standard flag values 	??
		iStdFlgs = FilePeg_ReadWord2(hFile);
		FilePeg_ReadString2(hFile, strDescr);
		
		wDefLen = FilePeg_ReadWord2(hFile);
		FilePeg_ReadDouble2(hFile, dPatternLen);
 
		for (WORD wDash = 0; wDash < wDefLen; wDash++)
			FilePeg_ReadDouble2(hFile, dDash[wDash]);		
		
		if (CPegDoc::GetDoc()->PenStylesLookup(strName) == PENSTYLE_LookupErr)
			CPegDoc::GetDoc()->PenStylesAdd(new CPenStyle(strName, strDescr, wDefLen, dDash));
	}
	delete [] dDash;
	
	if (FilePeg_ReadWord2(hFile) != TABLE_END)
		throw "Exception CFilePeg: Expecting sentinel TABLE_END.";
}

void CFilePeg::ReadTablesSection()
{
	if (FilePeg_ReadWord2(hFile) != SECTION_TABLES)
		throw "Exception CFilePeg: Expecting sentinel SECTION_TABLES.";

	try
	{
		ReadVPortTable();
		ReadLnTypsTable();
		ReadLayerTable();
	}
	catch (char* szMessage)
	{
		msgInformation(szMessage);
		return;
	}

	if (FilePeg_ReadWord2(hFile) != SECTION_END)
		throw "Exception CFilePeg: Expecting sentinel SECTION_END.";
}

void CFilePeg::ReadVPortTable()
{
	if (FilePeg_ReadWord2(hFile) != TABLE_VPORT)
		throw "Exception CFilePeg: Expecting sentinel TABLE_VPORT.";

	FilePeg_ReadWord2(hFile);

	if(FilePeg_ReadWord2(hFile) != TABLE_END)
		throw "Exception CFilePeg: Expecting sentinel TABLE_END.";
}

bool CFilePeg::Load(const CUPDUPDATA* pCUPDUPData)
{
	try
	{
		pCUPDUPData->SetProgress(_T("Loading Header..."),0);
		ReadHeaderSection(pCUPDUPData);
		if(pCUPDUPData->ShouldTerminate())
			return false;

		pCUPDUPData->SetProgress(_T("Loading Tables..."),25);
		ReadTablesSection(pCUPDUPData);
		if(pCUPDUPData->ShouldTerminate())
			return false;

		pCUPDUPData->SetProgress(_T("Loading Blocks..."),50);
		ReadBlocksSection(pCUPDUPData);
		if(pCUPDUPData->ShouldTerminate())
			return false;

		pCUPDUPData->SetProgress(_T("Loading Entities..."),75);
		ReadEntitiesSection(pCUPDUPData);
		if(pCUPDUPData->ShouldTerminate())
			return false;
	}
	catch(char* szMessage)
	{
		msgInformation(szMessage);
		return false;
	}

	return true;
}

void CFilePeg::ReadBlocksSection(const CUPDUPDATA* pCUPDUPData)
{
	if(FilePeg_ReadWord2(hFile) != SECTION_BLOCKS)
		throw "Exception CFilePeg: Expecting sentinel SECTION_BLOCKS.";

	CPrim* pPrim;
	CString strName;
	char szXRefPathNam[32];

	WORD wTblSize = FilePeg_ReadWord2(hFile);

	for (WORD w = 0; w < wTblSize; w++)
	{
		WORD wPrimCount = FilePeg_ReadWord2(hFile);
		
		FilePeg_ReadString2(hFile, strName);
		WORD wBlkTypFlgs = FilePeg_ReadWord2(hFile);
		CPnt pt;
		pt.Read(hFile);
		
		CBlock* pBlock = new CBlock(wBlkTypFlgs, pt, szXRefPathNam);
		CPegDoc::GetDoc()->BlksSetAt(strName, pBlock);

		for (WORD wPrim = 0; wPrim < wPrimCount; wPrim++)
		{
			FilePeg_ReadPrim2(hFile, pPrim);
			pBlock->AddTail(pPrim); 

			//MAK
			if(pCUPDUPData->ShouldTerminate())
				return;
		}
	}
	
	if(FilePeg_ReadWord2(hFile) != SECTION_END)
		throw "Exception CFilePeg: Expecting sentinel SECTION_END.";
}

void CFilePeg::ReadEntitiesSection(const CUPDUPDATA* pCUPDUPData)
{
	if(FilePeg_ReadWord2(hFile) != SECTION_SEGMENTS)
		throw "Exception CFilePeg: Expecting sentinel SECTION_SEGMENTS.";

	CPrim* pPrim;

	WORD wTblSize = FilePeg_ReadWord2(hFile);

	for (WORD w = 0; w < wTblSize; w++)
	{
		CLayer* pLayer = CPegDoc::GetDoc()->LayersGetAt(w);
		
		if (pLayer == 0)
			return;

		WORD wSegCount = FilePeg_ReadWord2(hFile);

		if (pLayer->IsInternal())
		{
			for (WORD wSeg = 0; wSeg < wSegCount; wSeg++)
			{
				CSeg* pSeg = new CSeg;
				WORD wPrimCount = FilePeg_ReadWord2(hFile);

				//MAK
					if(pCUPDUPData->ShouldTerminate())
					{
						delete pSeg;
						return;
					}

				for (WORD wPrim = 0; wPrim < wPrimCount; wPrim++)
				{
					//MAK
					if(pCUPDUPData->ShouldTerminate())
					{
						delete pSeg;
						return;
					}

					FilePeg_ReadPrim2(hFile, pPrim);

					//MAK
					if(pCUPDUPData->ShouldTerminate())
					{
						delete pSeg;
						return;
					}

					pSeg->AddTail(pPrim);

					
				}
				pLayer->AddTail(pSeg); 
			}
		}
		else
			CPegDoc::GetDoc()->TracingLoadLayer(pLayer->GetName(), pLayer);
	}
	
	if (FilePeg_ReadWord2(hFile) != SECTION_END)
		throw "Exception CFilePeg: Expecting sentinel SECTION_END.";
}

void CFilePeg::ReadHeaderSection(const CUPDUPDATA*)
{
	if (FilePeg_ReadWord2(hFile) != SECTION_HEADER)
		throw "Exception CFilePeg: Expecting sentinel SECTION_HEADER.";

	// 	with addition of info here will loop key-value pairs till SECTION_END sentinel
	
	if (FilePeg_ReadWord2(hFile) != SECTION_END)
		throw "Exception CFilePeg: Expecting sentinel SECTION_END.";
}

void CFilePeg::ReadLayerTable(const CUPDUPDATA* pCUPDUPData)
{
	if (FilePeg_ReadWord2(hFile) != TABLE_LAYER)
		throw "Exception CFilePeg: Expecting sentinel TABLE_LAYER.";
	
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CString strName;
	CString strPenStyleName;
 
	WORD wTblSize = FilePeg_ReadWord2(hFile);

	for (WORD w = 0; w < wTblSize; w++)
	{
		FilePeg_ReadString2(hFile, strName);
		WORD wTracingFlgs = FilePeg_ReadWord2(hFile);
		WORD wStateFlgs = FilePeg_ReadWord2(hFile);
		
		wStateFlgs |= CLayer::STATE_FLG_RESIDENT;	
		
		if ((wStateFlgs & CLayer::STATE_FLG_INTERNAL) != CLayer::STATE_FLG_INTERNAL)
		{	
			// HACK is this correct
			if (strName.Find('.') == -1)
				strName += ".jb1";
		}
		PENCOLOR nPenColor = short(FilePeg_ReadWord2(hFile));
		FilePeg_ReadString2(hFile, strPenStyleName);
		
		CLayer *pLayer;

		if (pDoc->LayersLookup(strName) < 0)
		{
			pLayer = new CLayer(strName, wStateFlgs);
			
			pLayer->SetTracingFlg(wTracingFlgs);
			
			// mak - fix?
			pLayer->SetStateFlg(wStateFlgs);

			pLayer->SetPenColor(nPenColor);
			pLayer->SetPenStyleName(strPenStyleName);
			pDoc->LayersAdd(pLayer);
		}
		else
			pLayer = pDoc->LayersGetAt(pDoc->LayersLookup(strName));

		if(pLayer->IsHot())
			pDoc->WorkLayerSet(pLayer);

		//MAK
		if(pCUPDUPData->ShouldTerminate())
			return;
	}
	
	if (FilePeg_ReadWord2(hFile) != TABLE_END)
		throw "Exception CFilePeg: Expecting sentinel TABLE_END.";
}

void CFilePeg::ReadLnTypsTable(const CUPDUPDATA* pCUPDUPData)
{
	if (FilePeg_ReadWord2(hFile) != TABLE_LTYPE)
		throw "Exception CFilePeg: Expecting sentinel TABLE_LTYPE.";

	CString strName;
	CString strDescr;
	WORD wDefLen;
	WORD iStdFlgs;
	double* dDash = new double[32];

	double dPatternLen;

	WORD wTblSize = FilePeg_ReadWord2(hFile);

	for (WORD w = 0; w < wTblSize; w++)
	{
		FilePeg_ReadString2(hFile, strName);
		// standard flag values 	??
		iStdFlgs = FilePeg_ReadWord2(hFile);
		FilePeg_ReadString2(hFile, strDescr);
		
		wDefLen = FilePeg_ReadWord2(hFile);
		FilePeg_ReadDouble2(hFile, dPatternLen);
 
		for (WORD wDash = 0; wDash < wDefLen; wDash++)
			FilePeg_ReadDouble2(hFile, dDash[wDash]);		
		
		if (CPegDoc::GetDoc()->PenStylesLookup(strName) == PENSTYLE_LookupErr)
			CPegDoc::GetDoc()->PenStylesAdd(new CPenStyle(strName, strDescr, wDefLen, dDash));

		//MAK
			if(pCUPDUPData->ShouldTerminate())
				return;
	}
	delete [] dDash;
	
	if (FilePeg_ReadWord2(hFile) != TABLE_END)
		throw "Exception CFilePeg: Expecting sentinel TABLE_END.";
}

void CFilePeg::ReadTablesSection(const CUPDUPDATA*)
{
	if (FilePeg_ReadWord2(hFile) != SECTION_TABLES)
		throw "Exception CFilePeg: Expecting sentinel SECTION_TABLES.";

	try
	{
		ReadVPortTable();
		ReadLnTypsTable();
		ReadLayerTable();
	}
	catch (char* szMessage)
	{
		msgInformation(szMessage);
		return;
	}

	if (FilePeg_ReadWord2(hFile) != SECTION_END)
		throw "Exception CFilePeg: Expecting sentinel SECTION_END.";
}

void CFilePeg::ReadVPortTable(const CUPDUPDATA*)
{
	if (FilePeg_ReadWord2(hFile) != TABLE_VPORT)
		throw "Exception CFilePeg: Expecting sentinel TABLE_VPORT.";

	FilePeg_ReadWord2(hFile);

	if(FilePeg_ReadWord2(hFile) != TABLE_END)
		throw "Exception CFilePeg: Expecting sentinel TABLE_END.";
}

////////////////////////////////////////////////////////////////////////////////////
void CFilePeg::Unload()
{
	Seek(0, FILE_BEGIN);
	SetEndOfFile((HANDLE)hFile);
	
	WriteHeaderSection();
	WriteTablesSection();
	WriteBlocksSection();
	WriteEntitiesSection();
	FilePeg_WriteString2(hFile, "EOF");	
}

void CFilePeg::Unload(const CUPDUPDATA* pCUPDUPData)
{
	Seek(0, FILE_BEGIN);
	SetEndOfFile((HANDLE)hFile);

	pCUPDUPData->SetProgress(_T("Saving Header..."),0);
	WriteHeaderSection();

	pCUPDUPData->SetProgress(_T("Saving Tables..."),25);
	WriteTablesSection();

	pCUPDUPData->SetProgress(_T("Saving Blocks..."),50);
	WriteBlocksSection();

	pCUPDUPData->SetProgress(_T("Saving Entities..."),75);
	WriteEntitiesSection();

	FilePeg_WriteString2(hFile, "EOF");
}

void CFilePeg::WriteBlocksSection()
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	FilePeg_WriteWord2(hFile, SECTION_BLOCKS);

	WORD wTblSize = pDoc->BlksSize();
	FilePeg_WriteWord2(hFile, wTblSize);

	CString strKey;
	CBlock* pBlock;

	POSITION pos = pDoc->BlksGetStartPosition();
	while (pos != 0)
	{
		pDoc->BlksGetNextAssoc(pos, strKey, pBlock);
		
		DWORD dwCountPosition = SetFilePointer((HANDLE)hFile, 0, NULL, FILE_CURRENT);
		FilePeg_WriteWord2(hFile, 0);	
		WORD iPrimCount = 0;
		
		FilePeg_WriteString2(hFile, strKey);
		FilePeg_WriteWord2(hFile, pBlock->GetBlkTypFlgs());
		pBlock->GetBasePt().Write(hFile);
		
// TODO: if block is an xref add a pathname

		POSITION posPrim = pBlock->GetHeadPosition();
		while (posPrim != 0)
		{
			CPrim* pPrim = pBlock->GetNext(posPrim);
			if (pPrim->WritePeg(hFile))
				iPrimCount++;
		}
		DWORD dwPosition = SetFilePointer((HANDLE)hFile, 0, NULL, FILE_CURRENT);
		Seek(dwCountPosition, FILE_BEGIN);
		FilePeg_WriteWord2(hFile, iPrimCount);
		Seek(dwPosition, FILE_BEGIN);
	}

	FilePeg_WriteWord2(hFile, SECTION_END);
}

void CFilePeg::WriteEntitiesSection()
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	FilePeg_WriteWord2(hFile, SECTION_SEGMENTS);

	int iTblSize = pDoc->LayersGetSize();
	FilePeg_WriteWord2(hFile, WORD(iTblSize));

	for (int i = 0; i < iTblSize; i++)
	{
		CLayer* pLayer = pDoc->LayersGetAt(i);
		if (pLayer->IsInternal())
		{
			FilePeg_WriteWord2(hFile, (WORD)pLayer->GetCount());

			POSITION pos = pLayer->GetHeadPosition();
			while (pos != 0)
			{
				CSeg* pSeg = pLayer->GetNext(pos);
				pSeg->WritePeg(hFile);
			}
		}
		else
			FilePeg_WriteWord2(hFile, 0);	
	}
	FilePeg_WriteWord2(hFile, SECTION_END);
}

void CFilePeg::WriteHeaderSection()
{
	FilePeg_WriteWord2(hFile, SECTION_HEADER);

	// header variable items go here
	
	FilePeg_WriteWord2(hFile, SECTION_END);
}

void CFilePeg::WriteLayerTable()
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	int iTblSize = pDoc->LayersGetSize();

	FilePeg_WriteWord2(hFile, TABLE_LAYER);

	DWORD dwTblSizePosition = SetFilePointer((HANDLE)hFile, 0, NULL, FILE_CURRENT);
	FilePeg_WriteWord2(hFile, WORD(iTblSize));

	for(int i = 0; i < pDoc->LayersGetSize(); i++)
	{
		CLayer* pLayer = pDoc->LayersGetAt(i);
		if (pLayer->IsResident())
		{
			FilePeg_WriteString2(hFile, pLayer->GetName());
			FilePeg_WriteWord2(hFile, pLayer->GetTracingFlgs());
			FilePeg_WriteWord2(hFile, pLayer->GetStateFlgs());
			FilePeg_WriteWord2(hFile, pLayer->PenColor());
			FilePeg_WriteString2(hFile, pLayer->GetPenStyleName());
		}
		else
			iTblSize--;
	}
	FilePeg_WriteWord2(hFile, TABLE_END);
	
	if (iTblSize != pDoc->LayersGetSize())
	{
		DWORD dwPosition = SetFilePointer((HANDLE)hFile, 0, NULL, FILE_CURRENT);
		Seek(dwTblSizePosition, FILE_BEGIN);
		FilePeg_WriteWord2(hFile, WORD(iTblSize));
		Seek(dwPosition, FILE_BEGIN);
	}
}

void CFilePeg::WritePenStyleTable()
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	WORD wTblSize = WORD(pDoc->PenStylesGetSize());

	FilePeg_WriteWord2(hFile, TABLE_LTYPE);
	FilePeg_WriteWord2(hFile, wTblSize);

	for (int i = 0; i < wTblSize; i++)
	{
		CPenStyle* pPenStyle = pDoc->PenStylesGetAt(i);
		
		FilePeg_WriteString2(hFile, pPenStyle->GetName());
		// standard flag values 	??
		FilePeg_WriteWord2(hFile, 0);
		FilePeg_WriteString2(hFile, pPenStyle->GetDescription());
		
		WORD wDefLen = pPenStyle->GetDefLen();
		FilePeg_WriteWord2(hFile, wDefLen);
		double dPatternLen = pPenStyle->GetPatternLen();
		FilePeg_WriteDouble2(hFile, dPatternLen);

		if (wDefLen > 0)
		{
			double* dDash = new double[wDefLen];
			pPenStyle->GetDashLen(dDash);
			for (WORD w = 0; w < wDefLen; w++)
				FilePeg_WriteDouble2(hFile, dDash[w]);

			delete [] dDash;
		}
	}
	FilePeg_WriteWord2(hFile, TABLE_END);
}

void CFilePeg::WriteTablesSection()
{
	FilePeg_WriteWord2(hFile, SECTION_TABLES);
	
	WriteVPortTable();
	WritePenStyleTable();
	WriteLayerTable();
	FilePeg_WriteWord2(hFile, SECTION_END);
}

void CFilePeg::WriteVPortTable()
{
	FilePeg_WriteWord2(hFile, TABLE_VPORT);
	FilePeg_WriteWord2(hFile, 0);
	FilePeg_WriteWord2(hFile, TABLE_END);
}

///////////////////////////////////////////////////////////////////////////
void CFilePeg::Recover(const CUPDUPDATA* pCUPDUPData)
{
	pCUPDUPData->SetProgress(_T("Pass 1: Recovering Header..."),0);
	RecoverHeaderSection(0);

	pCUPDUPData->SetProgress(_T("Pass 2: Recovering Viewport Table..."),17);
	RecoverVPortTable(0);

	pCUPDUPData->SetProgress(_T("Pass 3: Recovering Line Styles Table..."),33);
//	RecoverLnTypsTable(0);

	pCUPDUPData->SetProgress(_T("Pass 4: Recovering Layer Table..."),50);
	RecoverLayerTable(0);
	RecoverLayerTable(1);

	pCUPDUPData->SetProgress(_T("Pass 5: Recovering Blocks..."),67);
	RecoverBlocksSection(0);
	RecoverBlocksSection(1);

	pCUPDUPData->SetProgress(_T("Pass 6: Recovering Entities..."),83);
	RecoverEntitiesSection(0);
	RecoverEntitiesSection(1);
}

void CFilePeg::RecoverHeaderSection(int nOffset)
{
	Seek(nOffset, FILE_BEGIN);

	WORD w = 0;
	while(w != SECTION_HEADER)
	{
		if(FilePeg_ReadWord3(hFile, w))
			return;
	}

	// 	with addition of info here will loop key-value pairs till SECTION_END sentinel
}

void CFilePeg::RecoverVPortTable(int nOffset)
{
	Seek(nOffset, FILE_BEGIN);

	WORD w = 0;
	while(w != TABLE_VPORT)
	{
		if(FilePeg_ReadWord3(hFile, w))
			return;
	}

	FilePeg_ReadWord2(hFile);
}

void CFilePeg::RecoverLnTypsTable(int nOffset)
{
	Seek(nOffset, FILE_BEGIN);

	WORD w = 0;
	while(w != TABLE_LTYPE)
	{
		if(FilePeg_ReadWord3(hFile, w))
			return;
	}

	CString strName;
	CString strDescr;
	WORD wDefLen;
	WORD iStdFlgs;
	double* dDash = new double[32];

	double dPatternLen;

	WORD wTblSize = FilePeg_ReadWord2(hFile);

	for (WORD w = 0; w < wTblSize; w++)
	{
		FilePeg_ReadString2(hFile, strName);
		// standard flag values 	??
		iStdFlgs = FilePeg_ReadWord2(hFile);
		FilePeg_ReadString2(hFile, strDescr);
		
		wDefLen = FilePeg_ReadWord2(hFile);
		FilePeg_ReadDouble2(hFile, dPatternLen);
 
		for (WORD wDash = 0; wDash < wDefLen; wDash++)
			FilePeg_ReadDouble2(hFile, dDash[wDash]);

		if(strName == "")
		{
			//strName = " ";
			//continue;
			return;
		}
		
		if (CPegDoc::GetDoc()->PenStylesLookup(strName) == PENSTYLE_LookupErr)
			CPegDoc::GetDoc()->PenStylesAdd(new CPenStyle(strName, strDescr, wDefLen, dDash));
	}
	delete [] dDash;
}

void CFilePeg::RecoverLayerTable(int nOffset)
{
	Seek(nOffset, FILE_BEGIN);

	WORD w = 0;
	while(w != TABLE_LAYER)
	{
		if(FilePeg_ReadWord3(hFile, w))
			return;
	}

	CPegDoc* pDoc = CPegDoc::GetDoc();

	CString strName;
	CString strPenStyleName;
 
	WORD wTblSize = FilePeg_ReadWord2(hFile);

	for (WORD w = 0; w < wTblSize; w++)
	{
		if(strName == "")
		{
			//strName = "_MAK";
			//return;
		}

		FilePeg_ReadString2(hFile, strName);
		WORD wTracingFlgs = FilePeg_ReadWord2(hFile);
		WORD wStateFlgs = FilePeg_ReadWord2(hFile);
		
		wStateFlgs |= CLayer::STATE_FLG_RESIDENT;	
		
		if ((wStateFlgs & CLayer::STATE_FLG_INTERNAL) != CLayer::STATE_FLG_INTERNAL)
		{	
			// HACK is this correct
			if (strName.Find('.') == -1)
				strName += ".jb1";
		}
		PENCOLOR nPenColor = short(FilePeg_ReadWord2(hFile));
		FilePeg_ReadString2(hFile, strPenStyleName);
		
		CLayer *pLayer;

		if (pDoc->LayersLookup(strName) < 0)
		{
			pLayer = new CLayer(strName, wStateFlgs);
			
			pLayer->SetTracingFlg(wTracingFlgs);
			
			// mak - fix?
			pLayer->SetStateFlg(wStateFlgs);

			pLayer->SetPenColor(nPenColor);
			pLayer->SetPenStyleName(strPenStyleName);
			pDoc->LayersAdd(pLayer);
		}
		else
			pLayer = pDoc->LayersGetAt(pDoc->LayersLookup(strName));

		if(pLayer->IsHot())
			pDoc->WorkLayerSet(pLayer);
	}
}

void CFilePeg::RecoverBlocksSection(int nOffset)
{
	Seek(nOffset, FILE_BEGIN);

	WORD w = 0;
	while(w != SECTION_BLOCKS)
	{
		if(FilePeg_ReadWord3(hFile, w))
			return;
	}

	CPrim* pPrim;
	CString strName;
	char szXRefPathNam[32];

	WORD wTblSize = FilePeg_ReadWord2(hFile);

	for (WORD w = 0; w < wTblSize; w++)
	{
		WORD wPrimCount = FilePeg_ReadWord2(hFile);
		
		FilePeg_ReadString2(hFile, strName);
		WORD wBlkTypFlgs = FilePeg_ReadWord2(hFile);
		CPnt pt;
		pt.Read(hFile);
		
		CBlock* pBlock = new CBlock(wBlkTypFlgs, pt, szXRefPathNam);
		CPegDoc::GetDoc()->BlksSetAt(strName, pBlock);

		for (WORD wPrim = 0; wPrim < wPrimCount; wPrim++)
		{
			FilePeg_ReadPrim2(hFile, pPrim);
			pBlock->AddTail(pPrim); 
		}
	}
}

void CFilePeg::RecoverEntitiesSection(int nOffset)
{
	Seek(nOffset, FILE_BEGIN);

	WORD w = 0;
	while(w != SECTION_SEGMENTS)
	{
		if(FilePeg_ReadWord3(hFile, w))
			return;
	}

	CPrim* pPrim;

	WORD wTblSize = FilePeg_ReadWord2(hFile);

	for (WORD w = 0; w < wTblSize; w++)
//	for (WORD w = 0; w < 1; w++)
	{
		CLayer* pLayer = CPegDoc::GetDoc()->LayersGetAt(w);
		//CLayer* pLayer = CPegDoc::GetDoc()->WorkLayerGet();
		
		// MAK - hack..... recover
		if(pLayer == 0)
		{
//			pLayer = CPegDoc::GetDoc()->WorkLayerGet();
//			return;
			continue;
		}

		WORD wSegCount = FilePeg_ReadWord2(hFile);

		// recovery
		if(wSegCount < 0)
			continue;

		if(pLayer->IsInternal())
		{
			for (WORD wSeg = 0; wSeg < wSegCount; wSeg++)
			{
				CSeg* pSeg = new CSeg;
				WORD wPrimCount = FilePeg_ReadWord2(hFile);

				// recovery
				if(wPrimCount < 0)
					continue;

				for (WORD wPrim = 0; wPrim < wPrimCount; wPrim++)
				{
					if(!FilePeg_ReadPrim2(hFile, pPrim))
					{
//						w++;
						return;
//						goto layer;
					}
					pSeg->AddTail(pPrim);
				}

				pLayer->AddTail(pSeg); 
			}
		}
		else
			CPegDoc::GetDoc()->TracingLoadLayer(pLayer->GetName(), pLayer);
	}
}

/////////////////////////////////////////////////////////////////////////////////
bool FilePeg_ReadPrim2(HFILE hFile, CPrim*& pPrim)
{
	WORD wEntityTyp = FilePeg_ReadWord2(hFile);

	switch (wEntityTyp)
	{
		case CPrim::PRIM_MARK:
			pPrim = new CPrimMark;
			break;
		case CPrim::PRIM_INSERT:
			pPrim = new CPrimInsert;
			break;
		case CPrim::PRIM_SEGREF:
			pPrim = new CPrimSegRef;
			break;
		case CPrim::PRIM_LINE:
			pPrim = new CPrimLine;
			break;
		case CPrim::PRIM_POLYGON:
			pPrim = new CPrimPolygon;
			break;
		case CPrim::PRIM_ARC:
			pPrim = new CPrimArc;
			break;
		case CPrim::PRIM_BSPLINE:
			pPrim = new CPrimBSpline;
			break;
		case CPrim::PRIM_CSPLINE:
			pPrim = new CPrimCSpline;
			break;
		case CPrim::PRIM_POLYLINE:
			pPrim = new CPrimPolyline;
			break;
		case CPrim::PRIM_TEXT:
			pPrim = new CPrimText;
			break;
		case CPrim::PRIM_TAG:
			pPrim = new CPrimTag;
			break;
		case CPrim::PRIM_DIM:
			pPrim = new CPrimDim;
			break;
		case CPrim::PRIM_BITMAP:
			pPrim = new CPrimBitmap;
			break;
		default:
			pPrim = new CPrimMark;
			// mak - add "skip all" button
//			static int nSkipAll = 0;
//			
//			if(nSkipAll != 3)
//				nSkipAll = ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_INVALID_PRIM), app.GetSafeHwnd(), DlgProcInvalidPrim);

//			CString strPrimType;
//			strPrimType.Format(wEntityTyp);

//			msgWarning(IDS_MSG_BAD_PRIM_TYPE);//, strPrimType.GetBuffer());
			return false;
	}
	
	pPrim->ReadPeg(hFile);
	return true;
}

void FilePeg_ReadString2(HFILE hFile, CString& str)
{
	DWORD dwRead;

	str.Empty();
	char c;
	bool escape = false;
	//while(ReadFile((HANDLE)hFile, &c, 1, &dwRead, NULL) == 1)
	//{
	//	if (c == '\t') return;
	//	str += c;
	//}

	////
	//GIG - 07/11
	//Modified to detect and handle escape sequences
	while(ReadFile((HANDLE)hFile, &c, 1, &dwRead, NULL) == 1)
	{
		if (c == '\x1b')
		{
			escape = true;
			continue;
		}
		else
		{
			if (!escape && (c == '\t'))
				return;
			str += c;
			escape = false;
		}
	}
}

void FilePeg_WriteString2(HFILE hFile, const CString& str)
{
	DWORD dwWrite;
	CString cleanstr = FilePeg_StringCleanup(str);
	//CString cleanstr = str;	//uncomment to stop escaping tabs mid-string

	WriteFile((HANDLE)hFile, cleanstr, (UINT)cleanstr.GetLength(), &dwWrite, NULL);
	WriteFile((HANDLE)hFile, "\t", 1, &dwWrite, NULL);
}

CString FilePeg_StringCleanup(CString str)
{
	CString retval = str;
	
	for (int c = 0; c < retval.GetLength(); c++)
	{
		//::MessageBox(0, (LPCSTR)c, (char *)retval.GetAt(c), 0);
		if(retval.GetAt(c) == '\t')
		{
			retval.Insert(c, '\x1b');
			c++;
		}
//		::MessageBox(0, "", "", 0);
	}

	return retval;
}

void FilePeg_ReadDouble2(HFILE hFile, double& d) 
{
	DWORD dwRead;
	ReadFile((HANDLE)hFile, &d, sizeof(double), &dwRead, NULL);
}

void FilePeg_ReadShort2(HFILE hFile, short& i) 
{
	DWORD dwRead;
	ReadFile((HANDLE)hFile, &i, sizeof(short), &dwRead, NULL);
}

void FilePeg_ReadWord2(HFILE hFile, WORD& w) 
{
	DWORD dwRead;
	ReadFile((HANDLE)hFile, &w, sizeof(WORD), &dwRead, NULL);
}

WORD FilePeg_ReadWord2(HFILE hFile) 
{
	WORD w; 
	DWORD dwRead; 
	ReadFile((HANDLE)hFile, &w, sizeof(WORD), &dwRead, NULL);
	return w;
}

bool FilePeg_ReadWord3(HFILE hFile, WORD& w)
{
	DWORD dwRead; 
	BOOL bResult = ReadFile((HANDLE)hFile, &w, sizeof(WORD), &dwRead, NULL);

	// eof
	if (bResult && dwRead == 0) 
		return true;
		
	return false;
}

void FilePeg_WriteDouble2(HFILE hFile, double d) 
{
	DWORD dwWrite;
	WriteFile((HANDLE)hFile, &d, sizeof(double), &dwWrite, NULL);
}

void FilePeg_WriteWord2(HFILE hFile, WORD w) 
{
	DWORD dwWrite;
	WriteFile((HANDLE)hFile, &w, sizeof(WORD), &dwWrite, NULL);
}
