#include "stdafx.h"
//
//#include "PegAEsys.h"
//#include "PegAEsysDoc.h"
//#include "PegAEsysView.h"
//
////#include "Directory.h"
//#include "FileOpenDWG.h"
////#include "StringExtra.h"
////#include "Text.h"
////
////AD_OBJHANDLE CFileOpenDWG::ms_objecthandle = {0, 0, 0, 0, 0, 0, 0, 0};
////
////#if defined(_DEBUG)
////void myloadometer(short percent) {TRACE("Load Progress: %d\n", percent);}
////#endif // _DEBUG
//
//CFileOpenDWG::CFileOpenDWG()
//{
//	//m_henhd = (PAD_ENT_HDR) new char[sizeof(AD_ENT_HDR)]; 
//	//m_hen = (PAD_ENT) new char[sizeof(AD_ENT)]; 
//	//m_htb = (PAD_TB) new char[sizeof(AD_TB)];
//	//m_hxd = (PAD_XD) new char[sizeof(AD_XD)];
//}
//
//CFileOpenDWG::~CFileOpenDWG()
//{
//	//if (m_bInitialized)
//	//{
//	//	if(m_hdb != 0)
//	//		adCloseFile(m_hdb);
//
//	//	adCloseAd2();
//	//}
//	//delete [] m_hxd;
//	//delete [] m_htb;
//	//delete [] m_hen;
//	//delete [] m_henhd;
//
//	//m_hdb = 0;
//}
//
///////<summary>Creates a new DWG or DXF file.</summary>
////bool CFileOpenDWG::Create(char* szPathKey, WORD wType)
////{
////	// adNewFile adds the normal entries for a default "empty file" in AutoCAD, including layer 0, 
////	// linetypes CONTINUOUS, BYLAYER and BYBLOCK, dimstyle STANDARD, shapefile STANDARD, regapp ACAD, 
////	// and the standard objects. 
////	
////	if (!m_bInitialized)
////		return false;
////	
////	m_hdb = adNewFile(0, 0, 0, AD_ACAD2000);
////
////	WritePenStyleTable();
////	WriteLayerTable();
////	WriteBlocksSection();
////	WriteEntities();
////
////	char cFileType = wType == CPegDoc::FILE_TYPE_DWG ? AD_DWG : AD_DXF;
////	adSaveFile(m_hdb, szPathKey, cFileType, AD_ACAD2000, 0, 6, 1, 1);
////
////	return true;
////}
//
////bool CFileOpenDWG::Initialize(short* nError)
////{
////	m_bInitialized = (adInitAd2(app.m_strAppPath + "\\adinit.dat", nError) == 1);
////	if (m_bInitialized)
////	{
////#if defined(_DEBUG)
////		adSetAd2LoadometerFn(myloadometer);
////#endif // _DEBUG
////		adSetupDwgRead();
////		adSetupDxfRead();
////	}
////	else
////	{
////// TODO: Embed adinit.dat
////		msgInformation("Initialization data not loaded. (adinit.dat file must reside in same directory as application.)");
////	}
////	return m_bInitialized;
////}
////void CFileOpenDWG::Load(char* szPathKey)
////{
////	if (!m_bInitialized)
////		return;
////	
////#if defined(_DEBUG)
////	short nRet = adDwgSmellsBad(szPathKey);
////	if (nRet == - 1) 
////		TRACE("File: %s is pre-Release 11\n", szPathKey);
////	else if (nRet == 0) 
////		TRACE("File: %s passed pre-test\n", szPathKey);
////	else 
////		TRACE("File: %s has error(s) -> %s\n", szPathKey, adErrorStr(adError()));
////#endif // _DEBUG
////
////	m_hdb = adLoadFile(szPathKey, AD_PRELOAD_ALL, 1);
////
////	if (m_hdb == 0)
////	{	
////		TRACE("Error opening file %s\n", szPathKey);
////		TRACE("  error code is %d: %s\n", adError(), adErrorStr(adError()));
////		return;
////	}
////	m_dhd = adHeaderPointer(m_hdb);
////	
////	ReadHeaderSection();
////	ReadVPortTable();
////	ReadLnTypsTable();
////	ReadLayerTable();
////	
////	ReadBlockTable(m_hdb);
////	
////	ReadBlocks(m_hdb);
////	ReadEntities();
////}
//
////void CFileOpenDWG::ReadBlock(AD_DB_HANDLE hdb, PAD_BLKH blkh, PAD_ENT_HDR henhd)
////{
////	CBlock* pBlock;
////	CPegDoc::GetDoc()->BlksLookup(blkh->name, pBlock);
////
////	TRACE("Loading Block %s entity definitions ...\n", blkh->name);
////			
////	if ((blkh->flag & AD_BLOCK_XREF) == AD_BLOCK_XREF)
////	{
////		// external reference blocks have no entities. It points to a block in an external
////		// drawing. either the external drawing's *Model_space or any non-layout block.
////	}
////			
////	short nSuccess;
////	do
////	{
////		nSuccess = adGetEntity(blkh->entitylist, henhd, m_hen);
////		if (nSuccess)
////		{
////			AD_LAY lay;
////			adSeekLayer(hdb, henhd->entlayerobjhandle, &lay);
////						
////			if (henhd->xdblob != AD_VMNULL) 
////				TRACE("Extended data in block not loaded\n");
////			
////			CPrim* pPrim = ReadEntity(blkh->entitylist);
////			
//////#pragma tasMSG(TODO: There may be more to entity)
////				
////			if (henhd->enttype != AD_ENT_ENDBLK)
////			{	
////				// insert primitive altered position in block header table..
////				// restore position for next block
////				
////				if (henhd->enttype == AD_ENT_INSERT)
////					adSeekBlockheader(hdb, blkh->objhandle, blkh);
////					
////				// entity not loaded
////				if (pPrim == 0)
////					TRACE("Entity %d not loaded\n", henhd->enttype);
////				else
////				{
////					PENCOLOR nPenColor = henhd->entcolor;
////
////					if (nPenColor == AD_COLOR_BYBLOCK) {nPenColor = 7;}
////					else if (nPenColor == AD_COLOR_BYLAYER) {nPenColor = CPrim::PENCOLOR_BYLAYER;}
////					
////					pPrim->PenColor() = nPenColor;
////							
////					if (henhd->enttype != AD_ENT_FACE3D && henhd->enttype != AD_ENT_SOLID &&
////						henhd->enttype != adHatchEnttype(hdb))
////					{
////						AD_LTYPE ltype;
////						if (adSeekLinetype(hdb, henhd->entltypeobjhandle, &ltype))
////						{
////							if (Prim_IsByBlockPenStyle(ltype.name)) 
////								pPrim->PenStyle() = 1;
////							else if (Prim_IsByLayerPenStyle(ltype.name)) 
////								pPrim->PenStyle() = CPrim::PENSTYLE_BYLAYER;
////							else 
////								pPrim->PenStyle() = CPegDoc::GetDoc()->PenStylesLookup(ltype.name);
////						}
////					}
////					pBlock->AddTail(pPrim);
////				}
////			}
////		}
////	}
////	while (nSuccess && henhd->enttype != AD_ENT_ENDBLK);
////}
//
////void CFileOpenDWG::ReadBlocks(AD_DB_HANDLE hdb)
////{
////	adStartBlockheaderGet(hdb);
////	long lNumBlockHeaders = adNumBlockheaders(hdb);
////	
////	for (long lBlockHeader = 0; lBlockHeader < lNumBlockHeaders - 2; lBlockHeader++)
////	{
////		if (adGetBlockheader(hdb, &m_htb->blkh) == 1)
////		{
////			if (!m_htb->blkh.purgedflag) 
////			{
////				if (adStartEntityGet(m_htb->blkh.entitylist) == 0)
////					continue;					
////				if (adGetEntity(m_htb->blkh.entitylist, m_henhd, m_hen) == 0)
////					continue;
////				if (m_henhd->enttype != AD_ENT_BLOCK)
////					continue;
////			
////				ReadBlock(hdb, &m_htb->blkh, m_henhd);
////			}
////		}
////	}
////}
//
///////<summary>
///////Load all block containers, local or external. An external reference contains the
///////name and the filename of the external drawing. Local blocks containers are an unordered list
///////drawing entities. The two type of local block containers are layout and non-layout.
///////</summary>
////void CFileOpenDWG::ReadBlockTable(AD_DB_HANDLE hdb)
////{
////	if (adStartBlockheaderGet(hdb) == 0)
////	{
////		TRACE("adStartBlockheaderGet error: %s\n", adErrorStr(adError()));
////		return;
////	}
////	long lNumBlockHeaders = adNumBlockheaders(hdb);
////	
////	for (long lBlockHeader = 0; lBlockHeader < lNumBlockHeaders - 2; lBlockHeader++)
////	{
////		if (adGetBlockheader(hdb, &m_htb->blkh) == 0)
////		{
////			TRACE("adGetBlockheader error: %s\n", adErrorStr(adError()));
////			continue;
////		}
////		if (!m_htb->blkh.purgedflag) 
////		{
////			if (adStartEntityGet(m_htb->blkh.entitylist) == 0)
////			{
////				TRACE("adStartEntityGet error: %s\n", adErrorStr(adError()));
////				continue;					
////			}
////			if (adGetEntity(m_htb->blkh.entitylist, m_henhd, m_hen) == 0)
////			{
////				TRACE("adGetEntity error: %s\n", adErrorStr(adError()));
////				continue;
////			}
////			if (m_henhd->enttype != AD_ENT_BLOCK)
////			{
////				TRACE("Block table error: First entity is not a block entity\n");
////				continue;
////			}			
////			if ((m_htb->blkh.flag & AD_BLOCK_ANONYMOUS) == AD_BLOCK_ANONYMOUS)
////			{
////				char szBuf[8];
////				strcat(m_htb->blkh.name, _itoa(lBlockHeader, szBuf, 10));
////				adReplaceBlockheader(hdb, &m_htb->blkh);
////			}
////			if ((m_htb->blkh.flag & AD_BLOCK_XREF) == AD_BLOCK_XREF)
////			{
////				char szBuf[256];
////				Directory_ExamineFile(m_hen->block.xrefpath, szBuf);
////				strcpy(m_hen->block.xrefpath, szBuf);
////			}
////			CBlock* pBlock;
////			if (CPegDoc::GetDoc()->BlksLookup(m_htb->blkh.name, pBlock))
////			{
////				char szBuf[8];
////				strcat(m_htb->blkh.name, _itoa(lBlockHeader, szBuf, 10));
////				adReplaceBlockheader(hdb, &m_htb->blkh);
////			}
////			CPnt ptBase(m_hen->block.base[0], m_hen->block.base[1], m_hen->block.base[2]);
////			pBlock = new CBlock(m_htb->blkh.flag, ptBase, m_hen->block.xrefpath);
////			CPegDoc::GetDoc()->BlksSetAt(m_htb->blkh.name, pBlock);
////		}
////	}
////}
//
////void CFileOpenDWG::ReadEntities()
////{
////	short nBlockType[] = {AD_PAPERSPACE_HANDLE, AD_MODELSPACE_HANDLE};
////	const CString strBlockName[] = {"Paperspace", "Modelspace"};
////
////	AD_OBJHANDLE blkobjhandle;
////	
////	for (short entset = 0; entset < 2; entset++) 
////	{
////		adGetBlockHandle(m_hdb, blkobjhandle, nBlockType[entset]);
////
////		TRACE("Loading %s entity definitions ...\n", strBlockName[entset]);
////
////		int iExtendedData = 0;
////		long lNumEntities = adNumEntities(m_hdb, blkobjhandle);
////		int iAttribEnts = 0;
////		int iSeqEndEnts = 0;
////		int iDimensionEnts = 0;
////		
////		AD_VMADDR entlist = adEntityList(m_hdb, blkobjhandle);
////		
////		adStartEntityGet(entlist);
////		
////		short nSuccess;
////		do 
////		{
////			nSuccess = adGetEntity(entlist, m_henhd, m_hen); 
////			if (nSuccess) 
////			{
////				AD_LAY lay;
////				if (!adSeekLayer(m_hdb, m_henhd->entlayerobjhandle, &lay))
////					continue;
////				CLayer* pLayer = CPegDoc::GetDoc()->LayersGet(lay.name);
////			
////				if (lay.xrefindex != - 1)
////					TRACE("Entity from a xRef\n");
////				
////				CPrim* pPrim = ReadEntity(entlist);
////
////				if (m_henhd->xdblob != AD_VMNULL) {iExtendedData++;}
////				if (pPrim == 0)
////				{	// entity not loaded
////					if (m_henhd->enttype == AD_ENT_ATTRIB) {iAttribEnts++;}
////					else if (m_henhd->enttype == AD_ENT_SEQEND) {iSeqEndEnts++;}
////					else if (m_henhd->enttype == AD_ENT_DIMENSION) {iDimensionEnts++;}
////					else {TRACE("Entity %d not loaded\n", m_henhd->enttype);}
////
////					lNumEntities--;
////				}
////				else
////				{
////					PENCOLOR nPenColor = m_henhd->entcolor;
////					if (nPenColor == AD_COLOR_BYBLOCK) {nPenColor = 7;}
////					else if (nPenColor == AD_COLOR_BYLAYER) 
////					{
////						nPenColor = CPrim::PENCOLOR_BYLAYER;
////					}
////				
////					pPrim->PenColor() = nPenColor;
////				
////					if (m_henhd->enttype != AD_ENT_FACE3D && m_henhd->enttype != AD_ENT_SOLID &&
////						m_henhd->enttype != adHatchEnttype(m_hdb))
////					{
////						AD_LTYPE ltype;
////						if (adSeekLinetype(m_hdb, m_henhd->entltypeobjhandle, &ltype))
////						{
////							PENSTYLE nPenStyle;
////							if (Prim_IsByBlockPenStyle(ltype.name)) 
////								nPenStyle = 1;
////							else if (Prim_IsByLayerPenStyle(ltype.name)) 
////								nPenStyle = CPrim::PENSTYLE_BYLAYER;
////							else 
////								nPenStyle = CPegDoc::GetDoc()->PenStylesLookup(ltype.name);
////							
////							pPrim->PenStyle() = nPenStyle;
////						}
////					}
////					if (m_dhd->tilemode == 0 && entset == 1)
////					{
////						for (int i = 1; i < m_iVpCount; i++)
////						{
////							view::PushActive();
////							view::SetMatrix(m_tmView[i]);
////							if (pPrim->IsInView())
////							{
////								CPrim* pPrimCopy;
////								pPrim->Copy(pPrimCopy); 
////								pPrimCopy->Transform(m_tm[i]);
////								pLayer->AddTail(new CSeg(pPrimCopy));
////							}
////							view::PopActive();
////						}
////						delete pPrim;
////					}
////					else
////					{
////						pLayer->AddTail(new CSeg(pPrim));
////					}
////				}
////			}
////		}
////		while (nSuccess == 1);
////		
////		TRACE("%d Extended data in entity header not loaded\n", iExtendedData);
////		TRACE("%d of %d entitities loaded\n", lNumEntities, adNumEntities(m_hdb, blkobjhandle));
////		TRACE("      %d Attrib entities not loaded\n", iAttribEnts);
////		TRACE("      %d SeqEnd entities not loaded\n", iSeqEndEnts);
////		TRACE("      %d Dimension entities not loaded\n", iDimensionEnts);
////	}
////}
//
////CPrim* CFileOpenDWG::ReadEntity(AD_VMADDR entlist)
////{
////	CPrim* pPrim;
////	
////	switch (m_henhd->enttype) 
////	{
////		case AD_ENT_LINE:
////			pPrim = new CPrimLine(m_hen);
////			break;
////		
////		case AD_ENT_POINT:
////			pPrim = new CPrimMark(m_hen);
////			break;
////		
////		case AD_ENT_CIRCLE:
////		case AD_ENT_ELLIPSE:
////		case AD_ENT_ARC:
////			pPrim = new CPrimArc(m_henhd, m_hen);
////			break;
////
////		case AD_ENT_TEXT:
////		case AD_ENT_MTEXT:
////			pPrim = new CPrimText(m_hdb, m_henhd, m_hen);
////			break;
////		
////		case AD_ENT_INSERT:
////			pPrim = new CPrimSegRef(m_hdb, m_henhd, m_hen);
////			break;
////		
////		case AD_ENT_FACE3D:
////		case AD_ENT_SOLID:
////			pPrim = new CPrimPolygon(m_hdb, m_henhd, m_hen);
////			break;
////
////		case AD_ENT_POLYLINE:
////			pPrim = ReadEntityPolyline(entlist);
////			break;
////
////		case AD_ENT_VIEWPORT:
////			return ((CPrim*) 0);
////
////		case AD_ENT_ATTRIB:
////		case AD_ENT_ATTDEF:
////		case AD_ENT_ENDBLK:
////		case AD_ENT_SEQEND:
////			return ((CPrim*) 0);
////
////		default:	// proxies
////			
////			if(m_henhd->enttype == adOle2frameEnttype(m_hdb)) 
////			{
////				TRACE("AD_OLE2FRAME entity\n");
////				if(m_hen->ole2frame.databytes != 0L) 
////				{
////				}
////			}
////			else if(m_henhd->enttype == adLwplineEnttype(m_hdb)) 
////			{
////				pPrim = new CPrimPolyline(m_hen);
////				break;
////			}
////			else if(m_henhd->enttype == adHatchEnttype(m_hdb)) 
////			{
////				pPrim = new CPrimPolygon(m_hdb, m_henhd, m_hen);
////				CPrimPolygon* pPolygon = (CPrimPolygon*) pPrim;
////				if (pPolygon->GetPts() > 0)
////					break;
////				delete pPrim;	
////			}
////			else if(m_henhd->enttype == adImageEnttype(m_hdb)) 
////			{
////				TRACE("AD_IMAGE entity\n");
////			}
////			else 
////			{  // regular proxy
////				TRACE("entclassid: %d\n", m_hen->proxyent.entclassid);
////				TRACE("appclassid: %d\n", m_hen->proxyent.appclassid);
////				TRACE("entdatabits: %ld\n", m_hen->proxyent.entdatabits);
////				if(m_hen->proxyent.entdatabits != 0L) 
////				{
////					/* writebindatablob(m_hen->proxyent.entdatablob);*/
////				}
////				TRACE("objids: %ld\n", m_hen->proxyent.numobjids);
////				if(m_hen->proxyent.numobjids != 0L) 
////				{
////				/*
////					PAD_BLOB_CTRL bcptr = adStartBlobRead(m_hen->proxyent.objidblob);
////					for (int i = 0; i < (short) m_hen->proxyent.numobjids; i++) 
////					{
////						AD_TYPEDOBJHANDLE objidhandle;
////						adReadProxyEntityObjid(bcptr, &objidhandle);
////						TRACE("obj ID handle: \n");
////						// locprinthandle(objidhandle.typedhandle);
////					}
////					adEndBlobRead(bcptr);
////				*/
////				}
////			}
////  			return ((CPrim*) 0);
////		
////	}
////	return (pPrim);
////}
//
////CPrim* CFileOpenDWG::ReadEntityPolyline(AD_VMADDR entlist)
////{
////	WORD wFlag = m_hen->pline.polyflag;
////	
////	CPnts pts;
////	CPnt pt;
////	short nSuccess;
////	do
////	{
////		nSuccess = adGetEntity(entlist, m_henhd, m_hen);
////		
////		if (!nSuccess) {return ((CPrim*) 0);}
////		
////		if (m_henhd->enttype == AD_ENT_VERTEX)
////		{
////			if (m_hen->vertex.startwidthvalid != 0) {}
////			if (m_hen->vertex.endwidthvalid != 0) {}
////			pt(m_hen->vertex.pt0[0], m_hen->vertex.pt0[1], m_hen->vertex.pt0[2]);
////			pts.Add(pt);
////		}
////		else if (m_henhd->enttype == AD_ENT_SEQEND)
////		{
////			CPrimPolyline* pPolyline = new CPrimPolyline(pts);
////			pPolyline->SetFlag(wFlag);
////	
////			return (pPolyline);
////		}
////		else
////		{
////		
////		}
////	}
////	while (nSuccess == 1);
////
////	return ((CPrim*) 0);
////}
//
////void CFileOpenDWG::ReadHeaderSection()
////{
////	TRACE("Loading header section ..\n");
////	
////	if (m_dhd->tilemode == 1)
////	{
////		// Use tiled viewports only .. displaying only those entities in model space
////		TRACE("Old style tiled viewports of modelspace\n");
////	}
////	else
////	{
////		// Use floating viewports using the active layout (*paper_space) 
////
////		AD_OBJHANDLE blkobjhandle;
////	
////		adGetBlockHandle(m_hdb, blkobjhandle, AD_PAPERSPACE_HANDLE);
////		
////		AD_BLKH blkh;
////	
////		adSeekBlockheader(m_hdb, blkobjhandle, &blkh);
////
////		// paperspace blocks should contain at least one viewport entity. The first
////		// being the viewport for the entities on the paperspace block itself.
////		// additional viewports define the view of modelspace entities.
////		
////		m_iVpCount = 0;
////		adStartEntityGet(blkh.entitylist);
////		while (adGetEntity(blkh.entitylist, m_henhd, m_hen))
////		{
////			if (m_henhd->enttype == AD_ENT_VIEWPORT)
////			{
////				m_av[m_iVpCount] = CAbstractView(&m_hen->vpent);
////				m_vp[m_iVpCount] = CViewport(&m_hen->vpent);
////				
////				view::PushActive();
////				view::LookAt(m_av[m_iVpCount].GetEye(), m_av[m_iVpCount].GetTarget(), m_av[m_iVpCount].GetVwUp());
////				view::SetNearClipDistance(m_av[m_iVpCount].GetNearClipDistance());
////				view::SetFarClipDistance(m_av[m_iVpCount].GetFarClipDistance());
////				view::SetWnd(m_av[m_iVpCount].GetUMin(), m_av[m_iVpCount].GetVMin(), m_av[m_iVpCount].GetUMax(), m_av[m_iVpCount].GetVMax());
////				m_tmView[m_iVpCount] = view::GetMatrix();
////				view::PopActive();
////
////				double dXCenter;
////				double dYCenter;
////				m_av[m_iVpCount].GetCenter(dXCenter, dYCenter);
////				m_tm[m_iVpCount].Translate(CVec(- dXCenter, - dYCenter, 0.));
////				
////				double dScale = m_vp[m_iVpCount].GetHeight() / m_av[m_iVpCount].GetHeight();
////				m_tm[m_iVpCount].Scale(CVec(dScale, dScale, 1.));
////				CPnt ptCenter = m_vp[m_iVpCount].GetCenter();
////				m_tm[m_iVpCount].Translate(CVec(ptCenter[0], ptCenter[1], 0.));
////				m_iVpCount++;
////			}
////		}
////	}
////}
//
////void CFileOpenDWG::ReadLayerTable()
////{
////	TRACE("Loading layer definitions ...\n");
////	
////	adStartLayerGet(m_hdb);
////	
////	for (int i = 0; i < int(adNumLayers(m_hdb)); i++) 
////	{
////		adGetLayer(m_hdb, &m_htb->lay);
////		
////		if (m_htb->lay.purgedflag)
////			continue;
////			
////		TRACE("  %s\n", m_htb->lay.name);
////
////		if (CPegDoc::GetDoc()->LayersLookup(m_htb->lay.name) < 0)
////		{
////			CLayer* pLayer = new CLayer(m_htb->lay.name);
////			
////			pLayer->SetPenColor(WORD(abs(m_htb->lay.color)));
////			
////			AD_LTYPE adltp;
////			adSeekLinetype(m_hdb, m_htb->lay.linetypeobjhandle, &adltp);
////			
////			pLayer->SetPenStyleName(adltp.name);
////
////			if (m_htb->lay.color < 0 || (m_htb->lay.flag & AD_LAYER_FROZEN) != 0) 
////				pLayer->SetStateOff();
////			
////			CPegDoc::GetDoc()->LayersAdd(pLayer);
////
////			if (m_htb->lay.xdblob != AD_VMNULL) { /* extended data ?? */ }
////		}
////	}
////}
////
////void CFileOpenDWG::ReadLnTypsTable()
////{
////	TRACE("Loading line type definitions ...\n");
////	
////	adStartLinetypeGet(m_hdb);
////	
////	for (int i = 0; i < int(adNumLinetypes(m_hdb)); i++) 
////	{
////		adGetLinetype(m_hdb, &m_htb->ltype);
////		
////		if (m_htb->ltype.purgedflag)
////			continue;
////		
////		if (CPegDoc::GetDoc()->PenStylesLookup(m_htb->ltype.name) != PENSTYLE_LookupErr)
////			continue;	
////			
////		short nDashLens = m_htb->ltype.numlinetypesegs;
////			
////		double* dDashLen = new double[nDashLens];
////			
////		//	segdata values do not account for a scale factor
////
////		for (short n = 0; n < nDashLens; n++)
////			dDashLen[n] = m_htb->ltype.segdata[n].val;
////
////		CPenStyle* pPenStyle = new CPenStyle(m_htb->ltype.name, m_htb->ltype.text, nDashLens, dDashLen);
////		CPegDoc::GetDoc()->PenStylesAdd(pPenStyle);
////			
////		delete [] dDashLen;
////		
////		if (m_htb->ltype.xdblob != AD_VMNULL) {	/* extended data ?? */	}
////	}
////}
////
////void CFileOpenDWG::ReadVPortTable()
////{
////	TRACE("Loading view port definitions ...\n");
////	
////	adStartVportGet(m_hdb);
////	
////	for (int i = 0; i < int(adNumVports(m_hdb)); i++) 
////	{
////		adGetVport(m_hdb, &m_htb->vport);
////		if (!m_htb->vport.purgedflag)
////		{
////			TRACE("Viewport %s\n", m_htb->vport.name);
////
////			if (m_htb->vport.xdblob != AD_VMNULL) 
////			{
////				AD_XD m_hxd;
////				PAD_BLOB_CTRL bcptr = adStartBlobRead(m_htb->vport.xdblob);
////				while (adReadExtendeddata(bcptr, &m_hxd)) 
////				{
////				
////				}
////				adEndBlobRead(bcptr);
////			}
////		}
////    }
////}
////
////void CFileOpenDWG::WriteBlocksSection()
////{
////	CPegDoc* pDoc = CPegDoc::GetDoc();
////						
////	CString strKey;
////	CBlock* pBlock;
////	
////	POSITION pos = pDoc->BlksGetStartPosition();
////	while (pos != 0)
////	{
////        pDoc->BlksGetNextAssoc(pos, strKey, pBlock);
////
////		adSetDefaultBlockheader(&m_htb->blkh);
////		strcpy(m_htb->blkh.name, strKey);
////		adGenerateObjhandle(m_hdb, m_htb->blkh.objhandle);
////		adAddBlockheader(m_hdb, &m_htb->blkh); 
////	}
////	pos = pDoc->BlksGetStartPosition();
////	while (pos != 0)
////	{
////        pDoc->BlksGetNextAssoc(pos, strKey, pBlock);
////
////		char szName[64];
////		strcpy(szName, strKey);
////			
////		AD_OBJHANDLE blkobjhandle;
////		if (adFindBlockheaderByName(m_hdb, szName, blkobjhandle) == 0)
////			continue;
////
////		AD_VMADDR entlist = adEntityList(m_hdb, blkobjhandle);
////		if (entlist != 0)
////		{
////			m_henhd->enttype = AD_ENT_BLOCK;
////			adSetEntityDefaults(m_hdb, m_henhd, m_hen);
////			adGenerateObjhandle(m_hdb, m_henhd->enthandle);
////	
////			CPnt pt = pBlock->GetBasePt();
////			pt.Get(m_hen->block.base);
////			strcpy(m_hen->block.name2, strKey);
////			
////			adAddEntityToList(m_hdb, entlist, m_henhd, m_hen);
////		}
////		pBlock->Write(m_hdb, entlist, m_henhd, m_hen);
////		
////		m_henhd->enttype = AD_ENT_ENDBLK;
////		adSetEntityDefaults(m_hdb, m_henhd, m_hen);
////		adGenerateObjhandle(m_hdb, m_henhd->enthandle);
////		adAddEntityToList(m_hdb, entlist, m_henhd, m_hen);
////	}
////}
////
////void CFileOpenDWG::WriteEntities()
////{
////	CPegDoc* pDoc = CPegDoc::GetDoc();
////						
////	AD_OBJHANDLE blkobjhandle;
////	adFindBlockheaderByName(m_hdb, "*MODEL_SPACE", blkobjhandle);
////	AD_VMADDR mspaceentlist = adEntityList(m_hdb, blkobjhandle);
////				
////	int iTblSize = pDoc->LayersGetSize();
////
////	for (int i = 0; i < iTblSize; i++)
////	{
////		CLayer* pLayer = pDoc->LayersGetAt(i);
////		
////		char szName[64];
////		strcpy(szName, pLayer->GetName());
////		adFindLayerByName(m_hdb, szName, CFileOpenDWG::ms_objecthandle);
////	
////		POSITION pos = pLayer->GetHeadPosition();
////		while (pos != 0)
////		{
////			CSeg* pSeg = pLayer->GetNext(pos);
////			pSeg->Write(m_hdb, mspaceentlist, m_henhd, m_hen);
////		}
////	}
////}
////
////void CFileOpenDWG::WriteLayerTable()
////{
////	CPegDoc* pDoc = CPegDoc::GetDoc();
////						
////	// layer "0" table entry created by toolkit .. not added here .. modify some defaults
////	CLayer* pLayer = pDoc->LayersGetAt(0);
////	
////	adStartLayerGet(m_hdb);
////	adGetLayer(m_hdb, &m_htb->lay);
////	
////	m_htb->lay.color = pLayer->PenColor();
////
////	////////////////////////////////////////////////////////////////////////////////////////////
////	////////////////////////////////////////////////////////////////////////////////////////////
////
////	// mak - removed and replaced with code below
////	for (int i = 1; i < pDoc->LayersGetSize(); i++)
////	{
////		pLayer = pDoc->LayersGetAt(i);
////		WriteLayer(pLayer->GetName(), pLayer->PenColor());
////	}
////
////	////////////////////////////////////////////////////////////////////////////////////////////
////	////////////////////////////////////////////////////////////////////////////////////////////
////
////	//// MAK - added code to fix the layer name when a tracing file is saved as ACAD file
////	//// Peg tracing layer names are fine... Tracings were including the full path
////	//for (int i = 1; i < pDoc->LayersGetSize(); i++)
////	//{
////	//	pLayer = pDoc->LayersGetAt(i);
////
////	//	// MAK - added the code below to set the tracing layer name to just the filename
////	//	// not the entire pathname
////	//	CString strFilePath = pLayer->GetName();
////	//	WORD wFileType = app.GetFileTypeFromPath(strFilePath);
////
////	//	if(pDoc->FileTypeIsTracing(wFileType))
////	//	{
////	//		CString strName = strFilePath;
////	//		strName = strName.Right(strName.GetLength() - strName.ReverseFind('\\'));
////	//		strName.TrimLeft('\\');
////	//		pLayer->SetName(strName);
////	//	}
////
////	//	WriteLayer(strFilePath, pLayer->PenColor());
////	//}
////}
////
////void CFileOpenDWG::WriteLayer(const CString& strName, PENCOLOR nColor)
////{
////	adSetDefaultLayer(m_hdb, &m_htb->lay);
////	
////	strcpy(m_htb->lay.name, strName);
////	m_htb->lay.color = nColor;
////	
//////#pragma tasMSG(TODO: Need to modify default pen style)
////	
////	adGenerateObjhandle(m_hdb, m_htb->lay.objhandle);
////	adAddLayer(m_hdb, &m_htb->lay);
////}
////
////void CFileOpenDWG::WritePenStyleTable()
////{
////	CPegDoc* pDoc = CPegDoc::GetDoc();
////						
////	// linetype Continuous, ByBlock & ByLayer table entry created by toolkit
////	
////	for (int i = 2; i < pDoc->PenStylesGetSize(); i++)
////	{
////		CPenStyle* pPenStyle = pDoc->PenStylesGetAt(i);
////		WritePenStyle(pPenStyle);
////	}
////}
////void CFileOpenDWG::WritePenStyle(CPenStyle* pPenStyle)
////{
////	adSetDefaultLinetype(&m_htb->ltype);
////  
////	strcpy(m_htb->ltype.name, pPenStyle->GetName());
////	strcpy(m_htb->ltype.text, pPenStyle->GetDescription());
////	
////	WORD wDefLen = pPenStyle->GetDefLen();
////	
////	m_htb->ltype.numlinetypesegs = wDefLen;
////	m_htb->ltype.patternlength = pPenStyle->GetPatternLen();
////	
////	if (wDefLen > 0)
////	{
////		double* dDash = new double[wDefLen];
////		pPenStyle->GetDashLen(dDash);
////
////		for (WORD w = 0; w < wDefLen; w++)
////		{
////			m_htb->ltype.segdata[w].val = dDash[w];
////			adHanclear(m_htb->ltype.segdata[w].shapeentryobjhandle);
////			m_htb->ltype.segdata[w].complexshapecode = 0;
////			m_htb->ltype.segdata[w].xoffset = 0.;
////			m_htb->ltype.segdata[w].yoffset = 0.;
////			m_htb->ltype.segdata[w].rotang = 0.;
////			m_htb->ltype.segdata[w].scale = 0.;
////			m_htb->ltype.segdata[w].shapeflag = 0;
////			m_htb->ltype.segdata[w].stroffset = 0;
////		}
////		delete [] dDash;
////	}
////	adGenerateObjhandle(m_hdb, m_htb->ltype.objhandle);
////
////	if (!adAddLinetype(m_hdb, &m_htb->ltype)) 
////	{
////	
////	}
////}
//
////CPrimArc::CPrimArc(PAD_ENT_HDR henhd, PAD_ENT hen)
////{
////	CVec vNorm(henhd->extrusion[0], henhd->extrusion[1], henhd->extrusion[2]);
////	CVec vXAx = Prim_ComputeArbitraryAxis(vNorm);
////	CVec vYAx = vNorm ^ vXAx;
////	
////	if (henhd->enttype == AD_ENT_CIRCLE)
////	{ 
////		m_ptCenter(hen->circle.pt0[0], hen->circle.pt0[1], hen->circle.pt0[2]);
////
////		vXAx *= hen->circle.radius;
////		vYAx *= hen->circle.radius;
////		
////		m_vMajAx = vXAx;
////		m_vMinAx = vYAx;
////		m_dSwpAng = TWOPI;
////	}
////	else if (henhd->enttype == AD_ENT_ELLIPSE)
////	{
////		// local z-axis is perpendicular to the plane on which the ellipse is drawn.
////		// local x-axis is parallel to the major axis.
////		// local origin the center of ellipse
////		// within the lcs the representation of an ellipse is:
////		//      E = (a * cos(u), b * sin(u), 0), u ranges form 0 to 2pi
////		//			where a is half the major axis length and b is half the minor axis length
////		// this entity does not use arbitrary axis algorithim
////		// Conversion of parameter value to angle only matters if minor axis ratio is not 1. (non radial ellipses)
////				
////		// Can only properly represent ellipse which is radial or non radials which have a start parameter of 0.
////		
////		m_ptCenter(hen->ellipse.pt0[0], hen->ellipse.pt0[1], hen->ellipse.pt0[2]);
////
////		m_vMajAx(hen->ellipse.pt1offset[0], hen->ellipse.pt1offset[1], hen->ellipse.pt1offset[2]);
////
//////		hen->ellipse.
////
////		m_vMinAx = vNorm ^ m_vMajAx;
////		m_vMinAx *= hen->ellipse.minortomajorratio;
////		
////		// Conversion of parameter 0 - 2pi to angle 0 - 2pi
////		
////		double dMajLen = m_vMajAx.Length();
////		double dMinLen = m_vMinAx.Length();
////
////		CVec vStart(dMajLen / 2. * cos(hen->ellipse.startparam), dMinLen / 2 * sin(hen->ellipse.startparam), 0.);
////		double dStAng = Vec_Angle_xy(vStart);
////		
////		CVec vEnd(dMajLen / 2. * cos(hen->ellipse.endparam), dMinLen / 2 * sin(hen->ellipse.endparam), 0.);
////		double dEndAng = Vec_Angle_xy(vEnd);
////
//////		//swapping ish
//////		CVec temp = vStart;
//////		vStart = vEnd;
//////		vEnd = temp;
////		
////		// Rectify the start and end angles .. Arcs are always drawn counterclockwise
////		// from the start angle to the end angle
////		
////		if (dStAng >= TWOPI)
////		{	// need to rationalize angs to first period angles in range on (0 to twopi)
////			dStAng -= TWOPI;
////			dEndAng -= TWOPI;
////		}
////		m_dSwpAng = dEndAng - dStAng;
////		if (m_dSwpAng <= FLT_EPSILON) {m_dSwpAng += TWOPI;}
////		if (dStAng != 0.)
////		{
////			m_vMajAx.RotAboutArbAx(vNorm, dStAng);
////			m_vMinAx.RotAboutArbAx(vNorm, dStAng);
////			if (hen->ellipse.minortomajorratio != 1.)
////			{
////				TRACE("Ellipse: Non radial with start parameter not 0\n");
////			}
////		}
////
////		/*if((m_vMajAx ^ m_vMinAx)[2] < 0)
////			MessageBox(0, 0, 0, 0);*/
////
////		//// mak - hack to get arcs drawing correctly
////		//// PROBLEM:
////		//// clockwise > 180 degrees:   min axis negated and plane normal negated
////		//// counterck < 180 degrees:   min axis negated and plane normal negated
////		//if(m_dSwpAng > TWOPI)
////		//{
////		//	MessageBox(0, 0, 0, 0);
////
////		//	m_vMinAx[0] *= -1;
////		//	m_vMinAx[1] *= -1;
////		//	m_vMinAx[2] *= -1;
////		//}
////	}
////	else
////	{
////		CPnt ptCP(hen->arc.pt0[0], hen->arc.pt0[1], hen->arc.pt0[2]);
////	
////		vXAx *= hen->arc.radius;
////		vYAx *= hen->arc.radius;
////
////		// Rectify the start and end angles .. Arcs are always drawn counterclockwise
////		// from the start angle to the end angle
////
////		double dStAng = hen->arc.stang;
////		double dEndAng = hen->arc.endang;
////
////		if (dStAng >= TWOPI)
////		{	// need to rationalize angs to first period angles in range on (0 to twopi)
////			dStAng -= TWOPI;
////			dEndAng -= TWOPI;
////		}
////		double dAng = dEndAng - dStAng;
////	
////		if (dAng <= FLT_EPSILON)
////			dAng += TWOPI;
////		
////		CPnt ptStart = Pnt_RotAboutArbPtAndAx(ptCP + vXAx, ptCP, vNorm, dStAng);	
////	
////		CVec vMajAx(ptCP, ptStart);
////		CVec vMinAx = vNorm ^ vMajAx;
////	
////		m_ptCenter = ptCP;
////		m_vMajAx = vMajAx;
////		m_vMinAx = vMinAx;
////		m_dSwpAng = dAng; 
////
////		// mak - added to make arcs in correct place
////		if((m_vMajAx ^ m_vMinAx)[2] < 0)
////		{
////			m_ptCenter[0] *= -1;
////		}
////	}
////}
////bool CPrimArc::Write(AD_DB_HANDLE hdb, AD_VMADDR entlist, PAD_ENT_HDR henhd, PAD_ENT hen)
////{
////	henhd->enttype = AD_ENT_ELLIPSE;
////	adSetEntityDefaults(hdb, henhd, hen);
////	adHancpy(henhd->entlayerobjhandle, CFileOpenDWG::ms_objecthandle);
////	adGenerateObjhandle(hdb, henhd->enthandle);
////	
////	henhd->entcolor = m_nPenColor;
////	
////	char szName[64];
////	if (m_nPenStyle == PENSTYLE_BYLAYER)
////	{
////		strcpy(szName, "ByLayer");	
////	}
////	else
////	{
////		CPenStyle* pPenStyle = CPegDoc::GetDoc()->PenStylesGetAt(m_nPenStyle);
////		strcpy(szName, pPenStyle->GetName());
////	}
////	adFindLinetypeByName(hdb, szName, henhd->entltypeobjhandle);
////	
////	m_ptCenter.Get(hen->ellipse.pt0);
////	m_vMajAx.Get(hen->ellipse.pt1offset);
////	
////	CVec vNorm(m_vMajAx ^ m_vMinAx);
////	vNorm.Normalize();
////	vNorm.Get(henhd->extrusion);
////
////	hen->ellipse.minortomajorratio = min(m_vMinAx.Length() / m_vMajAx.Length(), 1.);
////
////	hen->ellipse.startparam = 0.;
////	hen->ellipse.endparam = m_dSwpAng;
////
////	// mak
////	if((m_vMajAx ^ m_vMinAx)[2] < 0)
////	{
////		hen->ellipse.startparam = TWOPI - m_dSwpAng;
////
////		while(hen->ellipse.startparam < 0)
////			hen->ellipse.startparam += 360.;
////
////		hen->ellipse.endparam = 0;
////	}
////
////	adAddEntityToList(hdb, entlist, henhd, hen);
////
////	return true;
////}
////bool CPrimBSpline::Write(AD_DB_HANDLE, AD_VMADDR, PAD_ENT_HDR, PAD_ENT)
////{
////	return false;
////}
////
////bool CPrimDim::Write(AD_DB_HANDLE, AD_VMADDR, PAD_ENT_HDR, PAD_ENT)
////{
////	return false;
////}
////CPrimInsert::CPrimInsert(AD_DB_HANDLE hdb, PAD_ENT_HDR henhd, PAD_ENT hen)
////{
////	AD_BLKH blkh;
////	adSeekBlockheader(hdb, hen->insert.blockheaderobjhandle, &blkh);
////
////	m_strName = blkh.name;
////	m_pt(hen->insert.pt0[0], hen->insert.pt0[1], hen->insert.pt0[2]);
////	m_vZ(henhd->extrusion[0], henhd->extrusion[1], henhd->extrusion[2]);
////	m_vScale(hen->insert.xscale, hen->insert.yscale, hen->insert.zscale);
////	m_dRotation = hen->insert.rotang;
////
////	if (henhd->extrusion[2] < 0.) {TRACE("Insert (block - %s) Entity Norm[2] = %f\n", blkh.name, henhd->extrusion[2]);}
////
////	m_vX = Prim_ComputeArbitraryAxis(m_vZ);
////	m_vY = m_vZ ^ m_vX;
////
////	m_vX *= hen->insert.xscale;
////	m_vY *= hen->insert.yscale;
////	m_vZ *= hen->insert.zscale;
////
////	//double dSinAng = sin(hen->insert.rotang);
////	//double dCosAng = cos(hen->insert.rotang);
////	//m_vX.RotAboutZAx(dSinAng, dCosAng);
////	//m_vY.RotAboutZAx(dSinAng, dCosAng);
////
////	CTMat tm(ORIGIN, m_vZ, hen->insert.rotang);
////	m_vX = tm * m_vX;
////	m_vY = tm * m_vY;
////}
////bool CPrimInsert::Write(AD_DB_HANDLE hdb, AD_VMADDR entlist, PAD_ENT_HDR henhd, PAD_ENT hen)
////{
////	henhd->enttype = AD_ENT_INSERT;
////	adSetEntityDefaults(hdb, henhd, hen);
////	adHancpy(henhd->entlayerobjhandle, CFileOpenDWG::ms_objecthandle);
////	adGenerateObjhandle(hdb, henhd->enthandle);
////	
////	henhd->entcolor = m_nPenColor;
////	
////	char szName[64];
////	if (m_nPenStyle == PENSTYLE_BYLAYER)
////	{
////		strcpy(szName, "ByLayer");	
////	}
////	else 
////	{
////		CPenStyle* pPenStyle = CPegDoc::GetDoc()->PenStylesGetAt(m_nPenStyle);
////		strcpy(szName, pPenStyle->GetName());
////	}
////	adFindLinetypeByName(hdb, szName, henhd->entltypeobjhandle);
////	
////	strcpy(szName, m_strName);
////	
////	adFindBlockheaderByName(hdb, szName, hen->insert.blockheaderobjhandle);
////	m_pt.Get(hen->insert.pt0);
////
////	hen->insert.xscale = m_vX.Length();
////	hen->insert.yscale = m_vY.Length();
////	hen->insert.zscale = m_vZ.Length();
////
////	hen->insert.rotang = Vec_Angle_xy(m_vX);
////	
////	hen->insert.numcols = m_wColCnt;
////	hen->insert.numrows = m_wRowCnt;
////	hen->insert.coldist = m_dColSpac;
////	hen->insert.rowdist = m_dRowSpac;
////
////	adAddEntityToList(hdb, entlist, henhd, hen);
////
////	return true;
////}
////CPrimLine::CPrimLine(PAD_ENT hen)
////{
////	m_ln[0](hen->line.pt0[0], hen->line.pt0[1], hen->line.pt0[2]);
////	m_ln[1](hen->line.pt1[0], hen->line.pt1[1], hen->line.pt1[2]);
////}
////
////bool CPrimLine::Write(AD_DB_HANDLE hdb, AD_VMADDR entlist, PAD_ENT_HDR henhd, PAD_ENT hen)
////{
////	henhd->enttype = AD_ENT_LINE;
////	adSetEntityDefaults(hdb, henhd, hen);
////	adHancpy(henhd->entlayerobjhandle, CFileOpenDWG::ms_objecthandle);
////	adGenerateObjhandle(hdb, henhd->enthandle);
////	
////	henhd->entcolor = m_nPenColor;
////	
////	char szName[64];
////	if (m_nPenStyle == PENSTYLE_BYLAYER)
////	{
////		strcpy(szName, "ByLayer");	
////	}
////	else 
////	{
////		CPenStyle* pPenStyle = CPegDoc::GetDoc()->PenStylesGetAt(m_nPenStyle);
////		strcpy(szName, pPenStyle->GetName());
////	}
////	adFindLinetypeByName(hdb, szName, henhd->entltypeobjhandle);
////	
////	hen->line.pt0[0] = m_ln[0][0];
////	hen->line.pt0[1] = m_ln[0][1];
////	hen->line.pt0[2] = m_ln[0][2];
////
////	hen->line.pt1[0] = m_ln[1][0];
////	hen->line.pt1[1] = m_ln[1][1];
////	hen->line.pt1[2] = m_ln[1][2];
////
////	adAddEntityToList(hdb, entlist, henhd, hen);
////
////	return true;
////}
////CPrimMark::CPrimMark(PAD_ENT hen)
////{
////	m_nMarkStyle = m_nPenStyle;
////	
////	m_pt(hen->point.pt0[0], hen->point.pt0[1], hen->point.pt0[2]);
////	m_Dats = 0;
////	m_dDat = 0;
////}
////bool CPrimMark::Write(AD_DB_HANDLE hdb, AD_VMADDR entlist, PAD_ENT_HDR henhd, PAD_ENT hen)
////{
////	henhd->enttype = AD_ENT_POINT;
////	adSetEntityDefaults(hdb, henhd, hen);
////	adHancpy(henhd->entlayerobjhandle, CFileOpenDWG::ms_objecthandle);
////	adGenerateObjhandle(hdb, henhd->enthandle);
////	
////	henhd->entcolor = m_nPenColor;
////	
////	m_pt.Get(hen->point.pt0);
////
////	adAddEntityToList(hdb, entlist, henhd, hen);
////
////	return true;
////}
////CPrimPolygon::CPrimPolygon(AD_DB_HANDLE	hdb, PAD_ENT_HDR henhd, PAD_ENT hen)
////{
////	if (henhd->enttype == AD_ENT_FACE3D)
////	{	
////		// Four sided, not necessarily planar, surface. It hides other objects and fills with solid color.
////		// No support for individual edge visibilty
////
////		m_nIntStyle = POLYGON_SOLID;
////		m_nIntStyleId = 0;
////		
////		CPnts pta;
////		CPnt pt(hen->face3d.pt0[0], hen->face3d.pt0[1], hen->face3d.pt0[2]);
////		pta.Add(pt);
////		pt(hen->face3d.pt1[0], hen->face3d.pt1[1], hen->face3d.pt1[2]);
////		if (pta[0] != pt) {pta.Add(pt);}
////		pt(hen->face3d.pt2[0], hen->face3d.pt2[1], hen->face3d.pt2[2]);
////		if (pta[pta.GetUpperBound()] != pt) {pta.Add(pt);}
////		pt(hen->face3d.pt3[0], hen->face3d.pt3[1], hen->face3d.pt3[2]);
////		if (pta[pta.GetUpperBound()] != pt && pta[0] != pt) {pta.Add(pt);}
////
////		m_wPts = (WORD) pta.GetSize();
////		m_Pt = new CPnt[m_wPts];
////		
////		for (WORD w = 0; w < m_wPts; w++) {m_Pt[w] = pta[w];}
////
////		m_ptOrig = m_Pt[0];
////		m_vPosXAx = XDIR;
////		m_vPosYAx = YDIR;
////	}
////	else if (henhd->enttype == AD_ENT_SOLID)
////	{
////		// The first two points define one edge of the polygon. 
////		// The third point is diagonally opposite the second 
////		// If the fourth point coincides with third result is a filled triangle.
////		// else fourth point creates a quadrilateral area. 
////
////		m_nIntStyle = POLYGON_SOLID;
////		m_nIntStyleId	= 0;
////			
////		m_wPts = WORD((hen->solid.pt2[0] == hen->solid.pt3[0] && 
////				  hen->solid.pt2[1] == hen->solid.pt3[1] &&
////				  hen->solid.pt2[2] == hen->solid.pt3[2]) ? 3 : 4);
////
////		m_Pt = new CPnt[m_wPts];
////			
////		m_Pt[0] = CPnt(hen->solid.pt0[0], hen->solid.pt0[1], hen->solid.pt0[2]);
////		m_Pt[1] = CPnt(hen->solid.pt1[0], hen->solid.pt1[1], hen->solid.pt1[2]);
////		m_Pt[2] = CPnt(hen->solid.pt3[0], hen->solid.pt3[1], hen->solid.pt3[2]);
////		
////		if (m_wPts == 4)
////		{
////			m_Pt[3] = CPnt(hen->solid.pt2[0], hen->solid.pt2[1], hen->solid.pt2[2]);
////		}
////		m_ptOrig = m_Pt[0];
////		m_vPosXAx = XDIR;
////		m_vPosYAx = YDIR;
////	}
////	else if (henhd->enttype == adHatchEnttype(hdb))
////	{
////		CPnts pta;
////		CPnt pt;
////		m_ptOrig = CPnt(hen->hatch.pt0[0], hen->hatch.pt0[1], hen->hatch.pt0[2]);
////		
////		CVec vNorm(henhd->extrusion[0], henhd->extrusion[1], henhd->extrusion[2]);
////		m_vPosXAx = Prim_ComputeArbitraryAxis(vNorm);
////		m_vPosYAx = vNorm ^ m_vPosXAx;
////	
////		m_nIntStyle = hen->hatch.solidfill == 1 ? POLYGON_SOLID : POLYGON_HOLLOW;
////		m_nIntStyleId = 0;
////
////		PAD_BLOB_CTRL bcptrAssocObj = 0;
////		
////		if (hen->hatch.boundaryobjblob != AD_VMNULL)
////		{
////			bcptrAssocObj = adStartBlobRead(hen->hatch.boundaryobjblob);
////		}
////		PAD_BLOB_CTRL bcptr = adStartBlobRead(hen->hatch.pathblob);
////
////		// Hatched region is bounded by a number of paths (or loops). Loops must
////		// be closed and may not intersect themselves or other loops. 
////		// There is always at least one loop. Only processing one loop now.
////
////		for (int iPath = 0; iPath < 1 /* hen->hatch.numpaths */; iPath++) 
////		{
////			long lLoopType;
////			adReadBlobLong(bcptr, &lLoopType);
////			
////			if ((lLoopType & AD_HATCH_PATHFLAG_POLYLINE) == AD_HATCH_PATHFLAG_POLYLINE) 
////			{  
////				// Polyline-like loops - similar to a polyline entity. Consists of an arbitrary
////				// number of vertices and option bulge factors. 
////				// All vertices are 2d points within the lcs of the hatch.
////				
////				short nBulgesPresent;
////				adReadBlobShort(bcptr, &nBulgesPresent);
////				short nClosed;
////				adReadBlobShort(bcptr, &nClosed);
////				long lNumPathSegs;
////				adReadBlobLong(bcptr, &lNumPathSegs);
////				
////				m_wPts = WORD(lNumPathSegs);
////				m_Pt = new CPnt[m_wPts];
////
////				for (int iPathSeg = 0; iPathSeg < lNumPathSegs; iPathSeg++) 
////				{
////					double dVer[2];
////					adReadBlob2Double(bcptr, dVer);
////					m_Pt[iPathSeg](dVer[0], dVer[1], 0.);
////
////					if (nBulgesPresent) 
////					{
////						double dBulge;
////						adReadBlobDouble(bcptr,&dBulge);
////					}
////				}
////			}
////		    else
////			{
////				// Hatch contour composed of a series of edges.
////				
////				long lNumPathSegs;
////				adReadBlobLong(bcptr, &lNumPathSegs);
////							
////				for (int iPathSeg = 0; iPathSeg < lNumPathSegs; iPathSeg++) 
////				{
////					char bEdgeType;
////					adReadBlobByte(bcptr, &bEdgeType);
////					
////					if (bEdgeType == AD_HATCH_PATHTYPE_LINE) 
////					{
////						double dPt[2];
////						adReadBlob2Double(bcptr, dPt);
////						pt(dPt[0], dPt[1], 0.);
////						
////						if (pta.GetSize() > 1) 
////						{
////							if (pta[pta.GetUpperBound()] != pt) {pta.Add(pt);}
////						}						
////						adReadBlob2Double(bcptr, dPt);
////						pt(dPt[0], dPt[1], 0.);
////						pta.Add(pt);
////					}
////					else if (bEdgeType == AD_HATCH_PATHTYPE_CIRCARC) 
////					{
////						TRACE("  Edge Type: Circular Arc\n");
////						double dCenter[2];
////						double dRadius;
////						double dStartAng;
////						double dEndAng;
////						short nIsClockWise;
////						adReadBlob2Double(bcptr, dCenter);
////						adReadBlobDouble(bcptr, &dRadius);
////						adReadBlobDouble(bcptr, &dStartAng);
////						adReadBlobDouble(bcptr, &dEndAng);
////						adReadBlobShort(bcptr, &nIsClockWise);
////					}
////					else if (bEdgeType==AD_HATCH_PATHTYPE_ELLIPARC) 
////					{
////						TRACE("  Edge Type: Elliptical Arc\n");
////						double dCenter[2];
////						double dMajorAxis[2];
////						double minmajratio;
////						double dStartAng;
////						double dEndAng;
////						short nIsClockWise;
////						adReadBlob2Double(bcptr, dCenter);
////						adReadBlob2Double(bcptr, dMajorAxis);
////						adReadBlobDouble(bcptr, &minmajratio);
////						adReadBlobDouble(bcptr, &dStartAng);
////						adReadBlobDouble(bcptr, &dEndAng);
////						adReadBlobShort(bcptr, &nIsClockWise);
////					}
////					else if (bEdgeType==AD_HATCH_PATHTYPE_SPLINE) 
////					{
////						TRACE("  Edge Type: Spline\n");
////						long dDegree;
////						short nIsRational;
////						short nIsPeriodic;
////						long lNumKnots;
////						long lNumControlPoints;
////						adReadBlobLong(bcptr, &dDegree);
////						adReadBlobShort(bcptr, &nIsRational);
////						adReadBlobShort(bcptr, &nIsPeriodic);
////						adReadBlobLong(bcptr, &lNumKnots);
////						adReadBlobLong(bcptr, &lNumControlPoints);
////						
////						for (int iKnot = 0L; iKnot < lNumKnots; iKnot++) 
////						{
////							double dKnot;
////							adReadBlobDouble(bcptr, &dKnot);
////						}
////						for (int iControlPoint = 0; iControlPoint < lNumControlPoints; iControlPoint++) 
////						{
////							double dPt[2];
////							adReadBlob2Double(bcptr,dPt);
////							if (nIsRational) 
////							{
////								double dWeight;
////								adReadBlobDouble(bcptr, &dWeight);
////							}
////						}
////					}
////				}
////				m_wPts = WORD(pta.GetSize());
////				m_Pt = new CPnt[m_wPts];
////						
////				for (WORD w = 0; w < m_wPts; w++) {m_Pt[w] = pta[w];}
////			}
////			
////			long lNumAssocObjs;
////			adReadBlobLong(bcptr, &lNumAssocObjs);
////		    for (int iAssocObj = 0; iAssocObj < lNumAssocObjs; iAssocObj++) 
////			{
////				char handle[8];
////   				adReadBlobBytes(bcptrAssocObj, handle, 8);
////			}
////		}
////		adEndBlobRead(bcptr);
////
////		if (hen->hatch.boundaryobjblob != AD_VMNULL)
////		{
////			adEndBlobRead(bcptrAssocObj);
////		}
////		
////	}
////}
////
////bool CPrimPolygon::Write(AD_DB_HANDLE hdb, AD_VMADDR entlist, PAD_ENT_HDR henhd, PAD_ENT hen)
////{
////	henhd->enttype = adLwplineEnttype(hdb);
////	adSetEntityDefaults(hdb, henhd, hen);
////	adHancpy(henhd->entlayerobjhandle, CFileOpenDWG::ms_objecthandle);
////	adGenerateObjhandle(hdb, henhd->enthandle);
////	
////	hen->lwpline.numpoints = m_wPts;
////	hen->lwpline.flag = AD_LWPLINE_IS_CLOSED;
////	
////	hen->lwpline.ldblob = adCreateBlob();
////
////	PAD_BLOB_CTRL bcptr = adStartBlobWrite(hen->lwpline.ldblob);
////	
////	double dPt[3];
////
////	for (WORD w = 0; w < m_wPts; w++) 
////	{
////		m_Pt[w].Get(dPt);
////		adWriteBlob2Double(bcptr, dPt);
////	}
////	adEndBlobWrite(bcptr);
////	adAddEntityToList(hdb, entlist, henhd, hen);
////
////	return true;
////}
////CPrimPolyline::CPrimPolyline(PAD_ENT hen)
////{
////	// Note: this is a lightweight polyline. will not succeed if adConvertPlinesOnLoad(0) is
////	// called suppressing conversion of the old style polylines
////
////	double dver[2];
////	double dbulge;
////	double dwidth[2];
////
////	WORD wPts = WORD(hen->lwpline.numpoints);
////	m_wFlags = hen->lwpline.flag;
////		
////	PAD_BLOB_CTRL bcptr = adStartBlobRead(hen->lwpline.ldblob);
////	
////	for (WORD w = 0; w < wPts; w++) 
////	{
////		adReadBlob2Double(bcptr, dver);
////		CPnt pt(dver[0], dver[1], hen->lwpline.elevation);
////		m_pts.Add(pt);
////		
////		dbulge = dwidth[0] = dwidth[1] = 0.0;
////		
////		if (hen->lwpline.flag & AD_LWPLINE_HAS_BULGES) 
////			adReadBlobDouble(bcptr, &dbulge);
////		if (hen->lwpline.flag & AD_LWPLINE_HAS_WIDTHS) 
////			adReadBlob2Double(bcptr, dwidth);
////	}
////	adEndBlobRead(bcptr);
////}
////bool CPrimPolyline::Write(AD_DB_HANDLE hdb, AD_VMADDR entlist, PAD_ENT_HDR henhd, PAD_ENT hen)
////{
////	henhd->enttype = adLwplineEnttype(hdb);
////	adSetEntityDefaults(hdb, henhd, hen);
////	adHancpy(henhd->entlayerobjhandle, CFileOpenDWG::ms_objecthandle);
////	adGenerateObjhandle(hdb, henhd->enthandle);
////	
////	hen->lwpline.numpoints = m_pts.GetSize();
////	hen->lwpline.flag = m_wFlags;
////	
////	hen->lwpline.ldblob = adCreateBlob();
////
////	PAD_BLOB_CTRL bcptr = adStartBlobWrite(hen->lwpline.ldblob);
////	
////	double dPt[3];
////	double dBulge = 0.;
////	double dWidth[2] = {0., 0.};
////
////	for (WORD w = 0; w < m_pts.GetSize(); w++) 
////	{
////		m_pts[w].Get(dPt);
////		adWriteBlob2Double(bcptr, dPt);
////		
////		if (m_wFlags & AD_LWPLINE_HAS_BULGES)
////			adWriteBlobDouble(bcptr, &dBulge);
////		if (m_wFlags & AD_LWPLINE_HAS_WIDTHS)
////			adWriteBlob2Double(bcptr, dWidth);
////	}
////	adEndBlobWrite(bcptr);
////	adAddEntityToList(hdb, entlist, henhd, hen);
////
////	return true;
////}
////CPrimSegRef::CPrimSegRef(AD_DB_HANDLE hdb, PAD_ENT_HDR henhd, PAD_ENT hen)
////{
////	AD_BLKH blkh;
////	adSeekBlockheader(hdb, hen->insert.blockheaderobjhandle, &blkh);
////
////	m_strName = blkh.name;
////	m_pt(hen->insert.pt0[0], hen->insert.pt0[1], hen->insert.pt0[2]);
////	m_vZ(henhd->extrusion[0], henhd->extrusion[1], henhd->extrusion[2]);
////	m_vScale(hen->insert.xscale, hen->insert.yscale, hen->insert.zscale);
////	m_dRotation = hen->insert.rotang;
////
////	m_wColCnt = hen->insert.numcols;
////	m_wRowCnt = hen->insert.numrows;
////	m_dColSpac = hen->insert.coldist;
////	m_dRowSpac = hen->insert.rowdist;
////}
////bool CPrimSegRef::Write(AD_DB_HANDLE hdb, AD_VMADDR entlist, PAD_ENT_HDR henhd, PAD_ENT hen)
////{
////	henhd->enttype = AD_ENT_INSERT;
////	adSetEntityDefaults(hdb, henhd, hen);
////	adHancpy(henhd->entlayerobjhandle, CFileOpenDWG::ms_objecthandle);
////	adGenerateObjhandle(hdb, henhd->enthandle);
////	
////	henhd->entcolor = m_nPenColor;
////	
////	char szName[64];
////	if (m_nPenStyle == PENSTYLE_BYLAYER) {strcpy(szName, "ByLayer");}
////	else 
////	{
////		CPenStyle* pPenStyle = CPegDoc::GetDoc()->PenStylesGetAt(m_nPenStyle);
////		strcpy(szName, pPenStyle->GetName());
////	}
////	adFindLinetypeByName(hdb, szName, henhd->entltypeobjhandle);
////	m_vZ.Get(henhd->extrusion);
////	
////	strcpy(szName, m_strName);
////	
////	adFindBlockheaderByName(hdb, szName, hen->insert.blockheaderobjhandle);
////	m_pt.Get(hen->insert.pt0);
////	
////	hen->insert.xscale = m_vScale[0];
////	hen->insert.yscale = m_vScale[1];
////	hen->insert.zscale = m_vScale[2];
////
////	hen->insert.rotang = m_dRotation;
////	
////	hen->insert.numcols = m_wColCnt;
////	hen->insert.numrows = m_wRowCnt;
////	hen->insert.coldist = m_dColSpac;
////	hen->insert.rowdist = m_dRowSpac;
////
////	adAddEntityToList(hdb, entlist, henhd, hen);
////
////	return true;
////}
////bool CPrimTag::Write(AD_DB_HANDLE, AD_VMADDR, PAD_ENT_HDR, PAD_ENT)
////{
////	return false;
////}
///////<summary>Constructs a text primative using OpenDWG library.</summary>
////CPrimText::CPrimText(AD_DB_HANDLE hdb, PAD_ENT_HDR henhd, PAD_ENT hen)
////{
////	AD_SHPTB shptb;
////	if (henhd->enttype == AD_ENT_TEXT) 
////		adSeekShapefile(hdb, hen->text.tdata.shapefileobjhandle, &shptb);
////	else
////		adSeekShapefile(hdb, hen->mtext.shapefileobjhandle, &shptb);
////		
////	CString strFont = shptb.file;
////	int nExt = strFont.ReverseFind('.');
////	if (nExt != - 1)
////	{
////		if (strFont.Mid(nExt).CompareNoCase(".shx") == 0)
////		{		
////			strFont.Truncate(nExt);
////			for (int n = nExt; n < 8; n++)
////				strFont += '_';
////			
////			strFont += ".ttf";
////		}
////	}
////	WORD wHorAlign;
////	WORD wVerAlign;
////	if (henhd->enttype == AD_ENT_TEXT)
////	{
////		/*switch (hen->text.tdata.vertalignment)
////		{
////			case AD_TEXT_VALIGN_MIDDLE:
////				wVerAlign = FD_VER_ALIGN_MIDDLE; break;
////			case AD_TEXT_VALIGN_TOP:
////				wVerAlign = FD_VER_ALIGN_TOP; break;
////			default:
////				wVerAlign = FD_VER_ALIGN_BOTTOM; break;
////		}
////		switch (hen->text.tdata.justification)
////		{
////			case AD_TEXT_JUST_CENTER:
////			case AD_TEXT_JUST_MIDDLE:
////				wHorAlign = FD_HOR_ALIGN_CENTER; break;
////			case AD_TEXT_JUST_RIGHT:
////			case AD_TEXT_JUST_ALIGNED:
////			case AD_TEXT_JUST_FIT:
////				wHorAlign = FD_HOR_ALIGN_RIGHT; break;
////			default:
////				wHorAlign = FD_HOR_ALIGN_LEFT; break;
////		}	
////		m_fd.Set(FD_PREC_TRUETYPEFONT, sFont, FD_PATH_RIGHT, wHorAlign, wVerAlign, 0.);
////		*/
////		m_fd.Set(CFontDef::PREC_TRUETYPEFONT, strFont, CFontDef::PATH_RIGHT, CFontDef::HOR_ALIGN_LEFT, CFontDef::VER_ALIGN_BOTTOM, 0.);
////		CCharCellDef ccd(hen->text.tdata.rotang, hen->text.tdata.oblique, hen->text.tdata.widthfactor, hen->text.tdata.height);
////		
////		CVec vDirX;
////		CVec vDirY;
////		CharCellDef_EncdRefSys(ccd, vDirX, vDirY);
////		m_rs.SetDirX(vDirX);
////		m_rs.SetDirY(vDirY);
////
////		m_rs.SetOrigin(CPnt(hen->text.pt0[0], hen->text.pt0[1], hen->text.pt0[2]));
////		m_strText = hen->text.textstr;
////	}
////	else if (henhd->enttype == AD_ENT_MTEXT)
////	{
////		switch (hen->mtext.attachpt)
////		{
////			case AD_MTEXT_ATTACH_TOPLEFT: 
////				wHorAlign = CFontDef::HOR_ALIGN_LEFT; 
////				wVerAlign = CFontDef::VER_ALIGN_TOP; 
////				break;
////			case AD_MTEXT_ATTACH_TOPCENTER:
////				wHorAlign = CFontDef::HOR_ALIGN_CENTER; 
////				wVerAlign = CFontDef::VER_ALIGN_TOP; 
////				break;
////			case AD_MTEXT_ATTACH_TOPRIGHT:
////				wHorAlign = CFontDef::HOR_ALIGN_RIGHT; 
////				wVerAlign = CFontDef::VER_ALIGN_TOP; 
////				break;
////			case AD_MTEXT_ATTACH_MIDDLELEFT:
////				wHorAlign = CFontDef::HOR_ALIGN_LEFT; 
////				wVerAlign = CFontDef::VER_ALIGN_MIDDLE; 
////				break;
////			case AD_MTEXT_ATTACH_MIDDLECENTER:
////				wHorAlign = CFontDef::HOR_ALIGN_CENTER; 
////				wVerAlign = CFontDef::VER_ALIGN_MIDDLE; 
////				break;
////			case AD_MTEXT_ATTACH_MIDDLERIGHT:
////				wHorAlign = CFontDef::HOR_ALIGN_RIGHT; 
////				wVerAlign = CFontDef::VER_ALIGN_MIDDLE; 
////				break;
////			case AD_MTEXT_ATTACH_BOTTOMCENTER:
////				wHorAlign = CFontDef::HOR_ALIGN_CENTER; 
////				wVerAlign = CFontDef::VER_ALIGN_BOTTOM; 
////				break;
////			case AD_MTEXT_ATTACH_BOTTOMRIGHT:
////				wHorAlign = CFontDef::HOR_ALIGN_RIGHT; 
////				wVerAlign = CFontDef::VER_ALIGN_BOTTOM; 
////				break;
////			default:
////				wHorAlign = CFontDef::HOR_ALIGN_LEFT; 
////				wVerAlign = CFontDef::VER_ALIGN_BOTTOM;
////		}
////		m_fd.Set(CFontDef::PREC_TRUETYPEFONT, strFont, CFontDef::PATH_RIGHT, wHorAlign, wVerAlign, 0.);
////	
////		CVec vXDir(hen->mtext.xdir[0], hen->mtext.xdir[1], hen->mtext.xdir[2]); 
////		
////		CCharCellDef ccd(Vec_Angle_xy(vXDir), 0., 1., hen->mtext.ht);
////				
////		CVec vDirX;
////		CVec vDirY;
////		CharCellDef_EncdRefSys(ccd, vDirX, vDirY);
////		m_rs.SetDirX(vDirX);
////		m_rs.SetDirY(vDirY);
////
////		m_rs.SetOrigin(CPnt(hen->mtext.pt0[0], hen->mtext.pt0[1], hen->mtext.pt0[2]));
////		if (hen->mtext.ldblob == AD_VMNULL)
////			m_strText = hen->mtext.textstr;
////		else
////		{
////			PAD_BLOB_CTRL bcptr = adStartBlobRead(hen->mtext.ldblob);
////			while (adReadMtextBlock(bcptr, hen->mtext.textstr)) 
////				m_strText += hen->mtext.textstr;
////			adEndBlobRead(bcptr);
////		}
////		int n = 0;
//////#pragma tasMSG(Possible CString subscript bounds error)
////		while (n + 1 < m_strText.GetLength())
////		{
////			if (m_strText[n] == '\\')
////			{
////				if (m_strText[n + 1] == 'P')
////				{
////					m_strText.SetAt(n, '\r');
////					m_strText.SetAt(n + 1, '\n');
////					n++;
////				}
////			}
////			n++;
////		}
////	}
////}
////bool CPrimText::Write(AD_DB_HANDLE hdb, AD_VMADDR entlist, PAD_ENT_HDR henhd, PAD_ENT hen)
////{
////	henhd->enttype = AD_ENT_TEXT;
////	adSetEntityDefaults(hdb, henhd, hen);
////	adHancpy(henhd->entlayerobjhandle, CFileOpenDWG::ms_objecthandle);
////	adGenerateObjhandle(hdb, henhd->enthandle);
////	
////	henhd->entcolor = m_nPenColor;
////	
////	char szName[64];
////	if (m_nPenStyle == PENSTYLE_BYLAYER)
////	{
////		strcpy(szName, "ByLayer");	
////	}
////	else 
////	{
////		CPenStyle* pPenStyle = CPegDoc::GetDoc()->PenStylesGetAt(m_nPenStyle);
////		strcpy(szName, pPenStyle->GetName());
////	}
////	adFindLinetypeByName(hdb, szName, henhd->entltypeobjhandle);
////
////	CPnts ptsPvt;
////
////	text_GetBoundingBox(m_fd, m_rs, m_strText.GetLength(), 0., ptsPvt);
////
////	ptsPvt[0].Get(hen->text.pt0);
////	
////	hen->text.tdata.height = m_rs.DirY().Length();
////	hen->text.tdata.rotang = Vec_Angle_xy(m_rs.DirX());
////	strcpy(hen->text.textstr, m_strText);
////	
////	adAddEntityToList(hdb, entlist, henhd, hen);
////	
////	return true;
////}