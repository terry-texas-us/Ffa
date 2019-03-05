// PegAEsysDoc.cpp : implementation of the CPegDoc class

////////////////////////////////////////////////////////////////////////////////////////////////
// INCLUDES
////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <vector>
#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "ddeGItms.h"
#include "DlgProcEditOps.h"
#include "Directory.h"
#include "FileBitmap.h"
#include "FileBlock.h"
#include "FileJob.h"

//#include "FileOpenDWG.h"
#include "FileDWGDirect.h"

#include "Grid.h"
#include "Print.h"
#include "StringExtra.h"
#include "Text.h"
#include "lex.h"

#include "SubProcAnnotate.h"

//#include "commctrl.h"

#include "updialog.h"

#include "Job2State.h"
extern CJob2State job2state;

#include "Undo.h"
extern CUndo undo;

////////////////////////////////////////////////////////////////////////////////////////////////
// DECLARATIONS
////////////////////////////////////////////////////////////////////////////////////////////////
UINT CALLBACK OFNHookProcFileTracing(HWND, UINT, WPARAM, LPARAM);
UINT_PTR CALLBACK OFNHookProcFileSaveAs(HWND, UINT, WPARAM, LPARAM);
UINT_PTR CALLBACK OFNHookProcFileOpen(HWND, UINT, WPARAM, LPARAM);

// Procedures for progress bar dialog
bool FilePegRecoverProc(const CUPDUPDATA* pCUPDUPData);

bool FilePegOpenThreadProc(const CUPDUPDATA* pCUPDUPData);
bool FileFFAOpenThreadProc(const CUPDUPDATA* pCUPDUPData);
bool FileAutoCADOpenThreadProc(const CUPDUPDATA* pCUPDUPData);
bool TracingOpenThreadProc(const CUPDUPDATA* pCUPDUPData);

bool FilePegSaveAsThreadProc(const CUPDUPDATA* pCUPDUPData);
//bool FileAutoCADSaveAsThreadProc(const CUPDUPDATA* pCUPDUPData);
bool FileTracingSaveAsThreadProc(const CUPDUPDATA* pCUPDUPData);

bool FilePegSaveThreadProc(const CUPDUPDATA* pCUPDUPData);
bool FileAutoCADSaveThreadProc(const CUPDUPDATA* pCUPDUPData);
bool FileTracingSaveThreadProc(const CUPDUPDATA* pCUPDUPData);

extern double gbl_dExtNum;
extern char gbl_szExtStr[128];

// for esc released engaged segs
extern CTMat tmEditSeg;

#include "StateInfoFrm.h"
#include "DlgProcPrintPreview.h"
#include "DlgProcPlotPreview.h"
extern CStateFrm stateframe;

#include "DlgProcBitmapEdit.h"
extern CBitmapEdit bitmapEdit;
#include "DlgProcBackgroundImage.h"
extern CBackgroundImage backgroundImage;

void ffaReadFile(const CString& strPathName);
void mfGetAll(CFile& f, const CVec& vTrns);

//UINT AFXAPI HashKey(CString& str)
//{
//	LPCTSTR pStr = (LPCTSTR) str;
//	UINT nHash = 0;
//	while (*pStr)
//	{
//		nHash = (nHash << 5) + nHash + *pStr++;
//	}
//	return nHash;
//}

////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor
////////////////////////////////////////////////////////////////////////////////////////////////
CPegDoc::CPegDoc() 
{
	m_bTrapHighlight = true;
	m_pLayerWork = new CLayer("0");
	m_wOpenFileType = FILE_TYPE_NONE;
	m_wOpenTab = TAB_OPEN_PEN;
	m_pFilePeg = 0;
	LayersAdd(m_pLayerWork);
//	SetOpenFile(FILE_TYPE_NONE, "0");
	m_bSaveShadow = true;
	m_bIsFileReadOnly = false;

//	m_strLastDirectory = "D:\\";
	m_strLastDirectory = "Z:\\_jobs";
	//char szBuffer[MAX_PATH];
	//::GetCurrentDirectory(sizeof(szBuffer), szBuffer);
	//m_strLastDirectory = szBuffer;

	//pBitmapLayer = new CLayer("Bitmap");
	//
	//	pBitmapLayer->ClrStateFlg();
	//	pBitmapLayer->SetStateFlg(CLayer::STATE_FLG_RESIDENT);
	//	pBitmapLayer->SetStateCold();
	//	pBitmapLayer->SetTracingFlg(CLayer::TRACING_FLG_MAPPED);

	//pBitmapLayer->SetStateCold();
//	this->LayersAdd(pBitmapLayer);
//	LayersAdd(pBitmapLayer);
}

CPegDoc::CPegDoc(const CPegDoc &source)
{
	m_strIdentifiedLayerName = source.m_strIdentifiedLayerName;
	m_wOpenFileType = source.m_wOpenFileType;

	//m_PenStyles = source.m_PenStyles;
	for(int i = 0; i < source.m_PenStyles.GetSize(); i++)
		m_PenStyles.Add(source.m_PenStyles.GetAt(i));

	//m_blks = source.m_blks;
	CString strKey;
	CBlock* pPegBlock;

	POSITION pos = source.m_blks.GetStartPosition();
	while(pos != 0)
	{
		source.m_blks.GetNextAssoc(pos, strKey, pPegBlock);
		m_blks.SetAt(strKey, pPegBlock);
	}

	//m_layers = source.m_layers;
	for(int i = 0; i < source.m_layers.GetSize(); i++)
		m_layers.Add(source.m_layers.GetAt(i));

	m_pLayerWork = source.m_pLayerWork;
	m_strPathName = source.m_strPathName;
	m_strTitle = source.m_strTitle;

	m_strLastDirectory = source.m_strLastDirectory;

	//m_BorderLayers = source.m_BorderLayers;
	for(int i = 0; i < source.m_BorderLayers.GetSize(); i++)
		m_BorderLayers.Add(source.m_BorderLayers.GetAt(i));

	m_wOpenTab = source.m_wOpenTab;
	m_wOpenFocus = source.m_wOpenFocus;

	registry = source.registry;

	m_bModified = source.m_bModified;
	m_pFilePeg = source.m_pFilePeg;
	
	//m_segsDeleted = source.m_segsDeleted;
	//for(int i = 0; i < source.m_segsDeleted.GetSize(); i++)
	//	m_segsDeleted.AddTail(source.m_segsDeleted.GetAt(i));

	m_bTrapHighlight= source.m_bTrapHighlight;

	m_bSaveShadow = source.m_bSaveShadow;
	m_bIsFileReadOnly = source.m_bIsFileReadOnly;
}

const CPegDoc& CPegDoc::operator= (const CPegDoc& param)
{
	m_strIdentifiedLayerName = param.m_strIdentifiedLayerName;
	m_wOpenFileType = param.m_wOpenFileType;

	//m_PenStyles = param.m_PenStyles;
	for(int i = 0; i < param.m_PenStyles.GetSize(); i++)
		m_PenStyles.Add(param.m_PenStyles.GetAt(i));

	//m_blks = param.m_blks;
	CString strKey;
	CBlock* pPegBlock;

	POSITION pos = param.m_blks.GetStartPosition();
	while(pos != 0)
	{
		param.m_blks.GetNextAssoc(pos, strKey, pPegBlock);
		m_blks.SetAt(strKey, pPegBlock);
	}

	//m_layers = param.m_layers;
	for(int i = 0; i < param.m_layers.GetSize(); i++)
		m_layers.Add(param.m_layers.GetAt(i));

	m_pLayerWork = param.m_pLayerWork;
	m_strPathName = param.m_strPathName;
	m_strTitle = param.m_strTitle;

	m_strLastDirectory = param.m_strLastDirectory;

	//m_BorderLayers = param.m_BorderLayers;
	for(int i = 0; i < param.m_BorderLayers.GetSize(); i++)
		m_BorderLayers.Add(param.m_BorderLayers.GetAt(i));

	m_wOpenTab = param.m_wOpenTab;
	m_wOpenFocus = param.m_wOpenFocus;

	registry = param.registry;

	m_bModified = param.m_bModified;
	m_pFilePeg = param.m_pFilePeg;
	
	//m_segsDeleted = param.m_segsDeleted;
	//for(int i = 0; i < param.m_segsDeleted.GetSize(); i++)
	//	m_segsDeleted.AddTail(param.m_segsDeleted.GetAt(i));

	m_bTrapHighlight= param.m_bTrapHighlight;

	m_bSaveShadow = param.m_bSaveShadow;
	m_bIsFileReadOnly = param.m_bIsFileReadOnly;
	
	return *this;
}

CPegDoc::~CPegDoc()
{
	// Release resources allocated for line type table
	for (WORD w = 0; w < m_PenStyles.GetSize(); w++) 
		delete m_PenStyles.GetAt(w);
	
	m_PenStyles.RemoveAll();

	BlksRemoveAll();
	LayersRemoveAll();
	m_pLayerWork = NULL;
	DeletedSegsRemoveSegs();

	trapsegs.RemoveAll();
	detsegs.RemoveAll();
	detsegs.DetSeg() = 0;

	if (m_pFilePeg != 0)
		delete m_pFilePeg;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Operations
////////////////////////////////////////////////////////////////////////////////////////////////
//void CPegDoc::AddTextBlock(char* pszText)
//{
//	CPnt ptPvt = app.CursorPosGet();
//	
//	CFontDef fd;
//	pstate.GetFontDef(fd);
//	
//	CCharCellDef ccd;
//	pstate.GetCharCellDef(ccd);
//
//	CRefSys rs(ptPvt, ccd);
//	
//	char* pText = strtok(pszText, "\r");
//	while (pText != 0)
//	{
//		if (strlen(pText) > 0)
//		{
//			CSeg* pSeg = new CSeg(new CPrimText(fd, rs, pText)); 
//			WorkLayerAddTail(pSeg);
//			UpdateAllViews(NULL, HINT_SEG, pSeg);
//		}
//		rs.SetOrigin(text_GetNewLinePos(fd, rs, 1., 0));
//		pText = strtok(0, "\r");
//		if (pText == 0)
//			break;
//		pText++;
//	}
//}

void CPegDoc::TextBlockWordWrap(CFontDef &fd, CRefSys &rs, CString strInput, int nLength)
{
	CString strWrappedText;
	int nLengthOfInput;
	int nCurrentPosition;
	int nCurrentLineStart;
	int nPositionOfLastSpace;

    // Get this once so we don't have to keep checking
	nLengthOfInput = strInput.GetLength();

    // Start both of these at the beginning
	nCurrentPosition = 0;
	nCurrentLineStart = 0;
	nPositionOfLastSpace = 0;

    // Loop through until we get to the end
    while (nCurrentPosition < nLengthOfInput)
	{
		// If the current position is a space, make a note of
        // it's location for later use.
		if(strInput.GetAt(nCurrentPosition) == ' ')
			nPositionOfLastSpace = nCurrentPosition;

		// If we're at what should be the end of a line, we go back
		// to the last space we saw and cut the line there.
		if(nCurrentPosition == nCurrentLineStart + nLength)
		{
			// Append this latest line to our result
			strWrappedText = strInput.Mid(nCurrentLineStart, nPositionOfLastSpace - nCurrentLineStart + 1);
			CSeg* pSeg = new CSeg(new CPrimText(fd, rs, strWrappedText)); 
			WorkLayerAddTail(pSeg);
			UpdateAllViews(NULL, HINT_SEG, pSeg);
			rs.SetOrigin(text_GetNewLinePos(fd, rs, 1., 0));

			// Reset the next line's starting point to the point we
			// used for the last one's end + 1.
			nCurrentLineStart = nPositionOfLastSpace + 1;

			// Remove any leading spaces that might mess up our
			// character count.  If you want to just pull of one,
			// switch this to a simple If conditional instead of
			// looping.
			while (strInput.Mid(nCurrentLineStart, 1) == " ")
				nCurrentLineStart = nCurrentLineStart + 1;
		}

		// Increment our location indicator.
		nCurrentPosition = nCurrentPosition + 1;
	}

    // Since the loop ends before we add the remaining text,
    // add remaining text as the last line.
	strWrappedText = strInput.Mid(nCurrentLineStart);
	CSeg* pSeg = new CSeg(new CPrimText(fd, rs, strWrappedText)); 
	WorkLayerAddTail(pSeg);
	UpdateAllViews(NULL, HINT_SEG, pSeg);
	rs.SetOrigin(text_GetNewLinePos(fd, rs, 1., 0));
}

#include "DlgProcTextWidth.h"
//using namespace textwidth;
//namespace textwidth;

void CPegDoc::AddTextBlock(char* pszText)
{
	CPnt ptPvt = app.CursorPosGet();

	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_TEXT_PASTE), app.GetSafeHwnd(), DlgProcTextPaste);
	
	CFontDef fd;
	pstate.GetFontDef(fd);
	
	CCharCellDef ccd;
	pstate.GetCharCellDef(ccd);

	CRefSys rs(ptPvt, ccd);
	
	char* pText = strtok(pszText, "\r");
	while (pText != 0)
	{
		if (strlen(pText) > 0)
		{
			TextBlockWordWrap(fd, rs, pText, textwidth::nWidth);
		}
		//rs.SetOrigin(text_GetNewLinePos(fd, rs, 1., 0));
		pText = strtok(0, "\r");
		if (pText == 0)
			break;
		pText++;
	}
}

// Locates the layer containing a segment and removes it.
// The segment itself is not deleted.
CLayer* CPegDoc::AnyLayerRemove(CSeg* pSeg)
{	
	for (WORD w = 0; w < m_layers.GetSize(); w++)
	{
		CLayer* pLayer = m_layers.GetAt(w);
		if (pLayer->IsHot() || (pLayer->IsWarm() && !pLayer->IsMapped()))
		{	
			if (pLayer->Remove(pSeg) != 0)
			{
				app.StatusLineDisplay(WorkCnt);
				SetModifiedFlag();	
				return pLayer;
			}
		}
	}
	return 0;
}

CString CPegDoc::LayerFind(CSeg *pSeg)
{
	for (WORD w = 0; w < m_layers.GetSize(); w++)
	{
		CLayer* pLayer = LayersGetAt(w);
		POSITION pos = pLayer->Find(pSeg);
		if (pos != 0)
			return pLayer->GetName();
	}

	return "";
}

int CPegDoc::BlockGetRefCount(const CString& strBlkNam) const
{
	int iCount = 0;

	for (WORD w = 0; w < m_layers.GetSize(); w++)
	{
		CLayer* pLayer = LayersGetAt(w);
		iCount += pLayer->GetBlockRefCount(strBlkNam);
	}
	
	CString strKey;
	CBlock* pBlock;

	POSITION pos = m_blks.GetStartPosition();
	while (pos != NULL)
	{
		m_blks.GetNextAssoc(pos, strKey, pBlock);
		iCount += pBlock->GetBlockRefCount(strBlkNam);
	}
	return iCount;
}

bool CPegDoc::BlksLookup(CString strKey, CBlock*& pBlock) const 
{
	if (m_blks.Lookup(strKey, pBlock))
		return true;
	
	pBlock = NULL;
	return false;
}

///<summary>
/// Removes all blocks and defining primitives.
///</summary>
void CPegDoc::BlksRemoveAll()
{
	CString strKey;
	CBlock* pBlock;

	POSITION pos = m_blks.GetStartPosition();
	while (pos != NULL)
	{
		m_blks.GetNextAssoc(pos, strKey, pBlock);
		pBlock->RemovePrims();
		delete pBlock;
	}
	m_blks.RemoveAll();
}

///<summary>
///Removes all blocks which have no references.
///</summary>
int CPegDoc::BlksRemoveUnused()
{
	int count = 0;
	CString strKey;
	CBlock* pBlock;

	bool done = true;

	do
	{
		done = true;

		POSITION pos = m_blks.GetStartPosition();
		while (pos != NULL)
		{
			m_blks.GetNextAssoc(pos, strKey, pBlock);
			if (BlockGetRefCount(strKey) == 0)
			{
	// TODO: Deletion by key may cause loop problems
				m_blks.RemoveKey(strKey);
				pBlock->RemovePrims();
				delete pBlock;

				count++;
				done = false;
			}
		}
	}while(!done);

	return count;
}

void CPegDoc::UpdateAllViews(CPegView* pView, LPARAM lHint, CObject* pOb)
{
	pView = CPegView::GetActiveView();
	pView->OnUpdate(pView, lHint, pOb);
}

void CPegDoc::ResolveCommands()
{
	// traverse all segments
	for (WORD w = 0; w < LayersGetSize(); w++)
	{
		CLayer* pLayer = LayersGetAt(w);

		POSITION pos = pLayer->GetHeadPosition();
		while(pos != 0)
		{
			CSeg *pSeg = pLayer->GetNext(pos);

			POSITION pos2 = pSeg->GetHeadPosition();
			while(pos2 != 0)
			{
				CPrim *pPrim = pSeg->GetNext(pos2);
				CString text = pPrim->GetText();

				if(text != "")
				{
					// %% commands
					int index = text.Find("%%");
					if(index != -1)
					{
						text.Delete(index, 2);
						pPrim->SetText(text);
					}
					
					// \...; commands
					index = text.Find("\\");
					if(index != -1)
					{
						index = text.Find(";");
						if(index != -1)
						{
							text.Delete(0, index+1);
							pPrim->SetText(text);
						}
					}
				}
			}
		}
	}
}

void CPegDoc::UnblockToCompressed()
{
	//CSegs *pSegs = new CSegs;
	CSegsTrap BlockTrap;

	for (int i = 0; i < LayersGetSize(); i++)
	{
		CLayer* pLayer = LayersGetAt(i);

		POSITION pos = pLayer->GetHeadPosition();
		while(pos != 0)
		{
			CSeg *pSeg = pLayer->GetNext(pos);
//			pSegs->AddTail(pSeg);
			pSeg->BreakSegRefs();
		} 
	} 

	//bool done = true;
	//CString strBName;
	//CBlock *pBlock = new CBlock;

	//POSITION pos2 = pSegs->GetHeadPosition();
	//while(pos2 != 0)
	//{
	//	CSeg *pSeg2 = pSegs->GetNext(pos2);
	//	BlockTrap.AddTail(pSeg2);
	//}

	//do
	//{
	//	done = true;

	//	POSITION pos3 = BlksGetStartPosition();
	//	while(pos3 != 0)
	//	{
	//		BlksGetNextAssoc(pos3, strBName, pBlock);

	//		if(pBlock->GetBlockRefCount(strBName) != 0)
	//			done = false;
	//	}

	//	BlockTrap.BreakSegRefs();
	//}while(!done);
}

void CPegDoc::ConvertFontToStroke()
{
	CFontDef font;
	CCharCellDef ccd;
	font.TextFontSet("simplex.psf");
	font.TextPrecSet(CFontDef::PREC_PEGSTROKEFONT);

	CSegs *pSegs = new CSegs;
	CSegsTrap TextTrap;

	for (int i = 0; i < LayersGetSize(); i++)
	{
		CLayer* pLayer = LayersGetAt(i);

		POSITION pos = pLayer->GetHeadPosition();
		while(pos != 0)
		{
			CSeg *pSeg = pLayer->GetNext(pos);
			pSegs->AddTail(pSeg);
		} 
	} 

	POSITION pos2 = pSegs->GetHeadPosition();
	while(pos2 != 0)
	{
		CSeg *pSeg2 = pSegs->GetNext(pos2);
		TextTrap.AddTail(pSeg2);
	}

	TextTrap.ModifyNotes(font, ccd, 1);
}

void CPegDoc::ConvertByLayerPens()
{
	PENCOLOR curCol, layCol;

	// traverse all segments
	for (WORD w = 0; w < LayersGetSize(); w++)
	{
		CLayer* pLayer = LayersGetAt(w);
		layCol = pLayer->PenColor();

		POSITION pos = pLayer->GetHeadPosition();
		while(pos != 0)
		{
			CSeg *pSeg = pLayer->GetNext(pos);

			POSITION pos2 = pSeg->GetHeadPosition();
			while(pos2 != 0)
			{
				CPrim *pPrim = pSeg->GetNext(pos2);

				// change pen color from bylayer
				curCol = pPrim->PenColor();
				if(curCol == CPrim::PENCOLOR_BYLAYER)
				{
					pPrim->SetPenColor(layCol);
				}
			}
		}
	}
}

void CPegDoc::ConvertPolylines()
{
	for(int i = 0; i < LayersGetSize(); i++)
	{
		CLayer *pLayer = LayersGetAt(i);

		POSITION pos1 = pLayer->GetHeadPosition();
		while(pos1 != 0)
		{
			CSeg *pSeg = pLayer->GetNext(pos1);

			POSITION pos2 = pSeg->GetHeadPosition();
			while(pos2 != 0)
			{
				POSITION pos2Cur = pos2;
				CPrim *pPrim = pSeg->GetNext(pos2);

				if(pPrim->Is(CPrim::PRIM_POLYLINE))
				{
					CPrimPolyline *pPrimPolyline = (CPrimPolyline *)pPrim;
	
					CPnts pnts; 
					pPrimPolyline->GetAllPts(pnts);

					for(int j = 0; j < pnts.GetSize() - 1; j++)
					{
						CPrimLine *pPrimLine = new CPrimLine(pPrimPolyline->PenColor(), pPrimPolyline->PenStyle(), pnts[j], pnts[j+1]);
						pSeg->InsertAfter(pos2Cur, pPrimLine);
					}

					// add final line if closed polyline
					if(pPrimPolyline->IsLooped())
					{
						CPrimLine *pPrimLine = new CPrimLine(pPrimPolyline->PenColor(), pPrimPolyline->PenStyle(), pnts[pnts.GetSize() - 1], pnts[0]);
						pSeg->InsertAfter(pos2Cur, pPrimLine);
					}

					pSeg->RemoveAt(pos2Cur);
				
				}
			}
		}
	}
}

void CPegDoc::ConvertByLayerStyle()
{
	PENSTYLE curSty, laySty;

	// traverse all segments
	for (WORD w = 0; w < LayersGetSize(); w++)
	{
		CLayer* pLayer = LayersGetAt(w);
		laySty = pLayer->PenStyle();

		POSITION pos = pLayer->GetHeadPosition();
		while(pos != 0)
		{
			CSeg *pSeg = pLayer->GetNext(pos);

			POSITION pos2 = pSeg->GetHeadPosition();
			while(pos2 != 0)
			{
				CPrim *pPrim = pSeg->GetNext(pos2);

				// change pen style from bylayer
				curSty = pPrim->PenStyle();
				if(curSty == CPrim::PENSTYLE_BYLAYER)
					pPrim->SetPenStyle(laySty);
			}
		}
	}
}

//Restores the last segment added to the deleted segment list.
void CPegDoc::DeletedSegsRestore()
{
	if (!DeletedSegsIsEmpty())
	{
		// have at least one deleted segment to restore
		CSeg* pSeg = DeletedSegsRemoveTail();
		WorkLayerAddTail(pSeg); 
		UpdateAllViews(NULL, HINT_SEG_SAFE, pSeg);
	}
}

void CPegDoc::DoEditPrimitiveEscape(bool )//bSetMode)
{
	if(app.m_iModeId == ID_MODE_SEG_EDIT)
	{
		CSeg *pSeg;
		
		if(AppGetGinCur(pSeg) != 0)
		{
			tmEditSeg.Inverse();

			UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSeg);
			pSeg->Transform(tmEditSeg);
			UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSeg);
			
			AppSetGinCur();

		//	if(bSetMode)
				app.SetWindowMode(app.m_iPrimModeId);
		}
	}
	else
	{
		CPrim* pPrim;
		
		if(AppGetGinCur(pPrim) != 0)
		{
			tmEditSeg.Inverse();

			UpdateAllViews(NULL, CPegDoc::HINT_PRIM_ERASE_SAFE, pPrim);
			pPrim->Transform(tmEditSeg);
			UpdateAllViews(NULL, CPegDoc::HINT_PRIM_ERASE_SAFE, pPrim);
			
			AppSetGinCur();

		//	if(bSetMode)
				app.SetWindowMode(app.m_iPrimModeId);
			
		}
	}
}

////void CPegDoc::DoEditSegmentEscape()
////{
////	CPrim* pPrim;
////	
////	if (AppGetGinCur(pPrim) != 0)
////	{
////		tmEditSeg.Inverse();
////
////		UpdateAllViews(NULL, CPegDoc::HINT_PRIM_ERASE_SAFE, pPrim);
////		pPrim->Transform(tmEditSeg);
////		UpdateAllViews(NULL, CPegDoc::HINT_PRIM_ERASE_SAFE, pPrim);
////		
////		app.SetWindowMode(app.m_iPrimModeId);
////		AppSetGinCur();
////	}
////}

///<summary>
/// Searches for closest detectible primitive.  
/// If found, primitive is lifted from its segment, inserted into a new segment 
/// which is added to deleted segment list. The primitive resources are not freed.
///</summary>
void CPegDoc::DoPrimDelete(const CPnt& pt)
{
	CSeg* pSeg = detsegs.SelSegAndPrimUsingPoint(pt);
	
	if (pSeg != 0)	
	{
		LPARAM lHint = HINT_SEG_ERASE_SAFE;
		if (pSeg->GetCount() > 1)
		{ // remove primitive from segment
			if (trapsegs.Find(pSeg) != 0)
				lHint |= HINT_TRAP;
			
			CPrim* pPrim = detsegs.DetPrim();
			pSeg->FindAndRemovePrim(pPrim);
			// new segment required to allow primitive to be placed into deleted segment list
			pSeg = new CSeg(pPrim);
		}
		else
		{ // deleting an entire segment
			AnyLayerRemove(pSeg);
			detsegs.Remove(pSeg);
			trapsegs.Remove(pSeg);
		}
		UpdateAllViews(NULL, lHint, pSeg);
		DeletedSegsAddTail(pSeg);
		msgInformation(IDS_MSG_PRIM_ADDED_TO_DEL_SEGS);
	}		
}

void CPegDoc::FileManageUnblockReferences(CString strName)
{
	CSegsTrap BlockTrap;

	for (int i = 0; i < LayersGetSize(); i++)
	{
		CLayer* pLayer = LayersGetAt(i);
		if(pLayer->IsHot() || pLayer->IsWarm())
		{
			POSITION pos = pLayer->GetHeadPosition();
			while(pos != 0)
			{
				CSeg *pSeg = pLayer->GetNext(pos);

				POSITION pos2 = pSeg->GetHeadPosition();
				while(pos2 != 0)
				{
					CPrim *pPrim = pSeg->GetNext(pos2);
			
					if(pPrim->Is(CPrim::PRIM_SEGREF))
					{
						CPrimSegRef *pRef = (CPrimSegRef *)pPrim;

						if(pRef->GetName() == strName)
						{
							trapsegs.Remove(pSeg);

							// mak - i took expand out because it was causing crashes
							// unblock now converts them to compressed segments...
							BlockTrap.AddTail(pSeg);
							BlockTrap.BreakSegRefs();
//							BlockTrap.Expand();
							BlockTrap.RemoveAll();
						}// if block has correct name
					}// if primitive is a block
				}// while in same segment
			}// while in same layer  
		}// layer is hot or warm
	}// layer for loop
}

bool CPegDoc::FilePegOpenExclusive(const CString& strFileName)
{
	CUPDialog Dlg(app.GetSafeHwnd(),FilePegOpenThreadProc,(LPVOID)&strFileName,_T("Loading Peg File..."));
	INT_PTR pReturn = Dlg.DoModal();

	if(pReturn != IDOK)
	{
		app.OnFileNew(false);
	}

	// add error checking
//	VerifyPens();

	if(SaveShadow())
		WriteShadowFile(false);
	
	return true;
}

bool CPegDoc::FilePegRecover(const CString& strFileName)
{
	CUPDialog Dlg(app.GetSafeHwnd(),FilePegRecoverProc,(LPVOID)&strFileName,_T("Recovering Peg File..."));
	Dlg.DoModal();

	if(SaveShadow())
		WriteShadowFile(false);
	
	return true;
}

bool CPegDoc::FileFFAOpenExclusive(CString strFilePath)
{
	//// mak - wait cursor
	//HCURSOR hOldCursor = ::SetCursor(LoadCursor(0, IDC_WAIT));

	//ffaReadFile(strFilePath);
	//strFilePath.TrimRight("ffa");
	//strFilePath += "peg";
	//SetOpenFile(FILE_TYPE_PEG, strFilePath);

	//// mak - old cursor
	//::SetCursor(hOldCursor);

	//return true;

	CUPDialog Dlg(app.GetSafeHwnd(),FileFFAOpenThreadProc,(LPVOID)&strFilePath,_T("Loading FFA File..."));
	Dlg.DoModal();

	if(SaveShadow())
		WriteShadowFile(false);
	
	return true;
}

bool CPegDoc::FileAutoCADOpenExclusive(CString strFilePath)
{
	CUPDialog Dlg(app.GetSafeHwnd(),FileAutoCADOpenThreadProc,(LPVOID)&strFilePath,_T("Loading AutoCAD File..."));
	Dlg.DoModal();

	return true;
//	WORD wFileType = app.GetFileTypeFromPath(strFilePath);
//
//	// MAK - added error handling
//	WIN32_FIND_DATA FindFileData;
//	HANDLE hFind;
//	hFind = FindFirstFile(strFilePath, &FindFileData);
//	if(hFind == INVALID_HANDLE_VALUE) 
//	{
//		msgWarning(IDS_MSG_FILE_NOT_FOUND, strFilePath); 
//		return false;
//	} 
//	FindClose(hFind);
//
//	// open file if it exists
//	CFileDWGDirect fdd;
////	short nError;
//	
////	if(fdd.Initialize())//&nError))
////	{	
//		// mak - wait cursor
//		HCURSOR hOldCursor = ::SetCursor(LoadCursor(0, IDC_WAIT));
//
//		fdd.Load(strFilePath.GetBuffer());//of.lpstrFile);
//		SetOpenFile(wFileType, strFilePath);
//		WorkLayerSet(LayersGetAt(0));
//
//		// mak - old cursor
//		::SetCursor(hOldCursor);
//
//		return true;
////	}	
//
////	return false;
}

bool CPegDoc::FileTypeIsTracing(WORD wType) const
{
	return ((wType == FILE_TYPE_TRACING) || (wType == FILE_TYPE_JOB));
}

CPegDoc* CPegDoc::GetDoc() 
{
	return &doc;
}	

void CPegDoc::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
{
	ptMin(FLT_MAX, FLT_MAX, FLT_MAX);
	ptMax(- FLT_MAX, - FLT_MAX, - FLT_MAX);
	
	for (WORD w = 0; w < m_layers.GetSize(); w++)
	{
		CLayer* pLayer = m_layers.GetAt(w);
		if(pLayer->IsOn()) 
			pLayer->GetExtents(ptMin, ptMax, tm);
	}
}

int CPegDoc::GetHotCount() const
{
	int iCount = 0;

	for (WORD w = 0; w < m_layers.GetSize(); w++)
	{
		CLayer* pLayer = m_layers.GetAt(w);
		if (pLayer->IsHot()) {iCount += pLayer->GetCount();}
	}
	return iCount;
}

int CPegDoc::GetWarmCount() const
{
	int iCount = 0;

	for (WORD w = 0; w < m_layers.GetSize(); w++)
	{
		CLayer* pLayer = m_layers.GetAt(w);
		if (pLayer->IsWarm()) {iCount += pLayer->GetCount();}
	}
	return iCount;
}

void CPegDoc::InitAll()
{
	WorkLayerInit();
	SetOpenFile(FILE_TYPE_NONE, "0");			
	SetFilePeg(0);
	
	for(int i = (int)m_layers.GetSize() - 1; i > 0; i--)
		LayersRemoveAt(i);

//	pBitmapLayer->RemoveAll();

	// update status bar count
	app.StatusLineDisplay(WorkCnt);

	BlksRemoveAll();
	AppSetGinCur();

	SetFileReadOnly(false);
}

void CPegDoc::LayerBlank(const CString& strFileName)
{
	CLayer* pLayer = LayersGet(strFileName);
	
	if (pLayer == 0)
		msgWarning(IDS_LAYER_NOT_LOADED); 
	else if (pLayer->IsResident())
		msgWarning(IDS_MSG_LAYER_IS_RESIDENT, strFileName);
	
	else
	{
		UpdateAllViews(NULL, HINT_LAYER_ERASE, pLayer);
		LayersRemove(strFileName);
	}
}

///<summary>
/// A layer is converted to a tracing or a job file
///</summary>
bool CPegDoc::LayerMelt(CString& strName)
{
	CLayer* pLayer = LayersGet(strName);
	if(pLayer == 0)
		return false;

	bool bRetVal = false;

	CString strPathName = GetPathName();
	CString strWDirectory = strPathName.Left(strPathName.ReverseFind('\\'));

	//char szFilter[55];
	//SecureZeroMemory(szFilter, 55);
	//::LoadString(app.GetInstance(), IDS_OPENFILE_FILTER_TRACINGS, szFilter, sizeof(szFilter));

	//CString strFilter = "PegAEsys Files (.peg .jb1 .tra .ffa)\0*.peg;*.jb1;*.tra;*.ffa\0";
	//strFilter += "Peg Files (.peg)\0*.peg\0";
	//strFilter += "Job Files (.jb1)\0*.jb1\0";
	//strFilter += "Tracing Files (.tra)\0*.tra\0";
	//strFilter += "Archived Files (.ffa)\0*.ffa\0";
	//strFilter += "DWG Files (.dwg)\0*.dwg\0";
	//strFilter += "DXF Files (.dxf)\0*.dxf\0";
	//strFilter += "All Files\0*.*\0";

	//"Job Files (.jb1)\0*.jb1\0Tracing Files (.tra)\0*.tra\0";

	OPENFILENAME of;
		
	::ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lpstrInitialDir = strWDirectory;
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = 0;
	of.hInstance = app.GetInstance();
	//of.lpstrFilter = szFilter;
	of.lpstrFilter = "Job Files (.jb1)\0*.jb1\0Tracing Files (.tra)\0*.tra\0";
	of.lpstrFile = new char[MAX_PATH];
	strcpy(of.lpstrFile, strName);
	of.nMaxFile = MAX_PATH;
	of.lpstrTitle = "Melt As";
	of.Flags = OFN_OVERWRITEPROMPT;
	of.lpstrDefExt = "tra";

	if(GetSaveFileName(&of))
	{
		strName = of.lpstrFile;

		WORD wFileType = app.GetFileTypeFromPath(strName);

		if(FileTypeIsTracing(wFileType))
		{
			if(wFileType == FILE_TYPE_TRACING)
			{
				CFileTracing ft;
				if(ft.OpenForWrite(strName))
				{
					ft.WriteHeader();
					ft.WriteSegs(pLayer);
                    bRetVal = true;
					ft.Close();
				}
			}
			else
			{
				CFileJob fj;
				if(fj.OpenForWrite(strName))
				{
					fj.WriteHeader();
					fj.WriteSegs(pLayer);
					bRetVal = true;
					fj.Close();
				}
			}

			if(bRetVal)
			{
				pLayer->ClrStateFlg();
				pLayer->SetStateFlg(CLayer::STATE_FLG_RESIDENT);
				pLayer->SetStateCold();
				pLayer->SetTracingFlg(CLayer::TRACING_FLG_MAPPED);
			
				strName = strName.Mid(of.nFileOffset);
				pLayer->SetName(strName);
			}
		}
	}
	delete [] of.lpstrFile;
	return (bRetVal);
}

void CPegDoc::LayerMeltAllWithoutSaving(CString strName)
{
	if(!strName.IsEmpty())
	{
		char *szLayers = strName.GetBuffer();
		char *token = strtok(szLayers, ";");
		while(token != NULL)
		{
			CString strLayerName = token;
			strLayerName = strLayerName.Left(strLayerName.GetLength() - 4);

			/////////////////////////////////////////////////////////////////////
				CLayer* pLayer = LayersGet(strLayerName);
				if (pLayer == 0)
					return;

				CString strName = token;

				//mak - this parses the file attribute
				token = strtok(NULL, ";");
				CString strLayerAttr = token;

				WORD wFileType = app.GetFileTypeFromPath(strName);

				if (FileTypeIsTracing(wFileType))
				{
					pLayer->ClrStateFlg();
					pLayer->SetStateFlg(CLayer::STATE_FLG_RESIDENT);
					
					pLayer->SetStateCold();
					
					if(strLayerAttr == "m")
					{
						pLayer->SetTracingFlg(CLayer::TRACING_FLG_MAPPED);
					}
					else if(strLayerAttr == "v")
					{
						pLayer->SetTracingFlg(CLayer::TRACING_FLG_VIEWED);;
					}
					else if(strLayerAttr == "c")
					{
						pLayer->SetStateOff();
						pLayer->SetTracingFlg(CLayer::TRACING_FLG_CLOAKED);
					}
				
					pLayer->SetName(strName);
				}

			token = strtok(NULL, ";");
		}
	}
}

void CPegDoc::LayersSoloHideAll(CString& strLayers)
{
	// mak - hide all layers
	// the string created for reference is in the form of:
	//		layername1;m;layername2;v;layername3;c;...
	//		w - work; a - active; s - static; h - hidden
	//		delimited by ";"
	/////////////////////////////////////////////////////////////////////////////////////	
	for(int i = 0; i < LayersGetSize(); i++)
	{
		CLayer *pLayer = LayersGetAt(i);

		CString strLayerName2 = pLayer->GetName();
		strLayers.Append(strLayerName2);
		strLayers.Append(";");

		if(pLayer->IsInternal())
		{
			if(pLayer->IsHot())
				strLayers.Append("w;");
			else if(pLayer->IsWarm())
				strLayers.Append("a;");
			else if(pLayer->IsCold())
				strLayers.Append("s;");
			else if(pLayer->IsOff())
				strLayers.Append("h;");
		}
		else
		{
			if(pLayer->IsMapped())
				strLayers.Append("m;");
			else if(pLayer->IsViewed())
				strLayers.Append("v;");
			else if(pLayer->IsCloaked())
				strLayers.Append("c;");
		}

		pLayer->SetStateOff();
		if(!pLayer->IsInternal())
			pLayer->SetTracingFlg(CLayer::TRACING_FLG_CLOAKED);
	}
}

CString CPegDoc::LayersGetState()
{
	CString strLayers;

	for(int i = 0; i < LayersGetSize(); i++)
	{
		CLayer *pLayer = LayersGetAt(i);

		CString strLayerName2 = pLayer->GetName();
		strLayers.Append(strLayerName2);
		strLayers.Append(";");

		if(pLayer->IsInternal())
		{
			if(pLayer->IsHot())
				strLayers.Append("w;");
			else if(pLayer->IsWarm())
				strLayers.Append("a;");
			else if(pLayer->IsCold())
				strLayers.Append("s;");
			else if(pLayer->IsOff())
				strLayers.Append("h;");
		}
		else
		{
			if(pLayer->IsMapped())
				strLayers.Append("m;");
			else if(pLayer->IsViewed())
				strLayers.Append("v;");
			else if(pLayer->IsCloaked())
				strLayers.Append("c;");
		}

		//pLayer->SetStateOff();
		//if(!pLayer->IsInternal())
			//pLayer->SetTracingFlg(CLayer::TRACING_FLG_CLOAKED);
	}

	return strLayers;
}

void CPegDoc::LayersSetState(CString strLayers)
{
	if(!strLayers.IsEmpty())
	{
		char *szLayers = strLayers.GetBuffer();
		char *token = strtok(szLayers, ";");
		while(token != NULL)
		{
			CString strLayerName = token;
			//strLayerName = strLayerName.Left(strLayerName.GetLength() - 4);

			/////////////////////////////////////////////////////////////////////
				CLayer* pLayer = LayersGet(strLayerName);
				if (pLayer == 0)
					return;

				CString strName = token;

				//mak - this parses the file attribute
				token = strtok(NULL, ";");
				CString strLayerAttr = token;

				//WORD wFileType = app.GetFileTypeFromPath(strName);

				//if (FileTypeIsTracing(wFileType))
				{
					if(strLayerAttr == "w")
					{
						pLayer->ClrStateFlg(CLayer::STATE_FLG_WARM | CLayer::STATE_FLG_COLD | CLayer::STATE_FLG_OFF);
						pLayer->SetStateFlg(CLayer::STATE_FLG_HOT);
						WorkLayerSet(pLayer);
					}
					else if(strLayerAttr == "a")
					{
						pLayer->ClrStateFlg(CLayer::STATE_FLG_HOT | CLayer::STATE_FLG_COLD | CLayer::STATE_FLG_OFF);
						pLayer->SetStateFlg(CLayer::STATE_FLG_WARM);
					}
					else if(strLayerAttr == "s")
					{
						pLayer->ClrStateFlg(CLayer::STATE_FLG_HOT | CLayer::STATE_FLG_WARM | CLayer::STATE_FLG_OFF);
						pLayer->SetStateFlg(CLayer::STATE_FLG_COLD);
					}
					else if(strLayerAttr == "h")
					{
						pLayer->ClrStateFlg(CLayer::STATE_FLG_HOT | CLayer::STATE_FLG_WARM | CLayer::STATE_FLG_COLD);
						pLayer->SetStateFlg(CLayer::STATE_FLG_OFF);
					}
					else if(strLayerAttr == "m")
					{
						pLayer->ClrStateFlg();
						pLayer->SetStateFlg(CLayer::STATE_FLG_RESIDENT);
						pLayer->SetStateCold();
						pLayer->SetTracingFlg(CLayer::TRACING_FLG_MAPPED);
					}
					else if(strLayerAttr == "v")
					{
						pLayer->ClrStateFlg();
						pLayer->SetStateFlg(CLayer::STATE_FLG_RESIDENT);
						pLayer->SetStateCold();
						pLayer->SetTracingFlg(CLayer::TRACING_FLG_VIEWED);;
					}
					else if(strLayerAttr == "c")
					{
						pLayer->ClrStateFlg();
						pLayer->SetStateFlg(CLayer::STATE_FLG_RESIDENT);
						pLayer->SetStateCold();
						pLayer->SetStateOff();
						pLayer->SetTracingFlg(CLayer::TRACING_FLG_CLOAKED);
					}
				
			//		pLayer->SetName(strName);
				}

			token = strtok(NULL, ";");
		}
	}
}

void CPegDoc::LayersSoloUnHideAll(CString strName)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

//	MessageBox(0, strName, 0, 0);

	if(!strName.IsEmpty())
	{
		char *szLayers = strName.GetBuffer();
		char *token = strtok(szLayers, ";");
		while(token != NULL)
		{
			CString strLayerName = token;

			/////////////////////////////////////////////////////////////////////
			CLayer* pLayer = LayersGet(strLayerName);
			if (pLayer == 0)
				return;

//			bool bRetVal = false;

			CString strName = token;

			//mak - this parses the file attribute
			token = strtok(NULL, ";");
			CString strLayerAttr = token;
						
			if(strLayerAttr == "w")
				pLayer->SetStateHot();
			else if(strLayerAttr == "a")
				pLayer->SetStateWarm();
			else if(strLayerAttr == "s")
				pLayer->SetStateCold();
			else if(strLayerAttr == "h")
				pLayer->SetStateOff();
			else if(strLayerAttr == "m")
				pDoc->TracingMap(pLayer->GetName());
			else if(strLayerAttr == "v")
				pDoc->TracingView(pLayer->GetName());
			else if(strLayerAttr == "c")
			{
				pLayer->ClrStateFlg();
				pLayer->SetStateFlg(CLayer::STATE_FLG_RESIDENT);
			
				pLayer->SetStateCold();
				pLayer->SetStateOff();
				pLayer->SetTracingFlg(CLayer::TRACING_FLG_CLOAKED);
			}

			token = strtok(NULL, ";");
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////
}

void CPegDoc::LayerMeltAll(CString strName)
{
	if(!strName.IsEmpty())
	{
		char *szLayers = strName.GetBuffer();
		char *token = strtok(szLayers, ";");
		while(token != NULL)
		{
			CString strLayerName = token;

			// FIX: trim right bug?
//			strLayerName.TrimRight(".jb1");
//			strLayerName.TrimRight(".tra");
			strLayerName = strLayerName.Left(strLayerName.GetLength() - 4);

			/////////////////////////////////////////////////////////////////////
				CLayer* pLayer = LayersGet(strLayerName);
				if (pLayer == 0)
					return;

				bool bRetVal = false;

				CString strName = token;

				//mak - this parses the file attribute
				token = strtok(NULL, ";");
				CString strLayerAttr = token;

				WORD wFileType = app.GetFileTypeFromPath(strName);

				if (FileTypeIsTracing(wFileType))
				{
					if (wFileType == FILE_TYPE_TRACING)
					{
						CFileTracing ft;
						if (ft.OpenForWrite(strName))
						{
							ft.WriteHeader();
							ft.WriteSegs(pLayer);
							bRetVal = true;
							ft.Close();
						}
					}
					else
					{
						CFileJob fj;
						if (fj.OpenForWrite(strName))
						{
							fj.WriteHeader();
							fj.WriteSegs(pLayer);
							bRetVal = true;
							fj.Close();
						}
					}
					if (bRetVal)
					{
						pLayer->ClrStateFlg();
						pLayer->SetStateFlg(CLayer::STATE_FLG_RESIDENT);
						
						pLayer->SetStateCold();
						
						if(strLayerAttr == "m")
						{
							pLayer->SetTracingFlg(CLayer::TRACING_FLG_MAPPED);
						}
						else if(strLayerAttr == "v")
						{
							pLayer->SetTracingFlg(CLayer::TRACING_FLG_VIEWED);;
						}
						else if(strLayerAttr == "c")
						{
							pLayer->SetStateOff();
							pLayer->SetTracingFlg(CLayer::TRACING_FLG_CLOAKED);
						}
					
						pLayer->SetName(strName);
					}
				}

			token = strtok(NULL, ";");
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////
}

//void CPegDoc::LayersPrintAll(CDC *pDC)
//{
//	CPegView* pView = CPegView::GetActiveView();
//
//	bool bIdentifyTrap = trapsegs.Identify() && !trapsegs.IsEmpty();
//	
//	detsegs.RemoveAll();
//
//	// mak - add tracing layer name text
//	if(printState.GetDispLayer())
//		DisplayTracingLayerText(pDC);
//		
//	if (pDC == 0)
//	{
//		//if (pView->GetViewWireframe())
//		//	opengl::PolygonModeLine();
//		//else
//		//	opengl::PolygonModeFill();
//		//
//		//for (int i = 0; i < LayersGetSize(); i++)
//		//{
//		//	CLayer* pLayer = LayersGetAt(i);
//
//		//	WORD layerState = pLayer->GetStateFlgs();
//		//	pLayer->SetStateHot();
//		//	pLayer->PrintLayer(pDC, bIdentifyTrap);
//		//	pLayer->SetStateFlg(layerState);
//		//}
//		//opengl::Flush();
//	}
//	else
//	{
//		CPrim::SpecPolygonStyle() = (pView->GetViewWireframe()) ? POLYGON_HOLLOW : short(- 1);
//		int iPrimState = pstate.Save();
//		
//		for (WORD w = 0; w < LayersGetSize(); w++)
//		{
//			CLayer* pLayer = LayersGetAt(w);
//
//			if(pLayer->IsOn() && (!pLayer->IsCloaked()))
//			{
//				WORD layerState = pLayer->GetStateFlgs();
//				WORD tracingState = pLayer->GetTracingFlgs();
//
//				pLayer->SetStateHot();
//				pLayer->PrintLayer(pDC, bIdentifyTrap);
//
//				pLayer->ClrStateFlg();
//				pLayer->ClrTracingFlg();
//
//				pLayer->SetStateFlg(layerState);
//				pLayer->SetTracingFlg(tracingState);
//			}
//		}
//		pstate.Restore(pDC, iPrimState);
//		CPrim::SpecPolygonStyle() = - 1;
//	}
//}

void CPegDoc::PrintBox(CDC *pDC)
{
	CPnt ptMin;
	CPnt ptMax;
	CTMat tm;	
	GetExtents(ptMin, ptMax, tm);

	CPnt pt2 = ptMin;

	pt2[0] += .07;
	pt2[1] += .05;
	//CRefSys rs(ptMin, ccd);

	int nSave = pstate.Save();

	CPrimLine *pPrimLine = new CPrimLine(0, 0, ptMin, pt2);
	pPrimLine->Display(pDC, false);

	pstate.Restore(pDC, nSave);

}

void CPegDoc::DisplayTracingLayerText(CDC *pDC)
{
	CCharCellDef ccd(90 * PI / 180, 0, 1, .05);

	PENCOLOR penColor = pstate.PenColor();

	CPnt ptMin;
	CPnt ptMax;
	CTMat tm;	
	GetExtents(ptMin, ptMax, tm);

	ptMin[0] += .07;
	ptMin[1] += .05;
	CRefSys rs(ptMin, ccd);
	
	CFontDef fd;
	fd.Set(CFontDef::PREC_PEGSTROKEFONT, "Simplex.psf", CFontDef::PATH_RIGHT, CFontDef::HOR_ALIGN_LEFT, CFontDef::VER_ALIGN_TOP, 0.);

	CString strLayDisp = GetPathName();
	strLayDisp = strLayDisp.Right(strLayDisp.GetLength() - strLayDisp.ReverseFind('\\') - 1);
	strLayDisp.Append(": ");
	for(int i = 0; i < LayersGetSize(); i++)
	{
		CLayer *pLayer = LayersGetAt(i);
		if(!pLayer->IsInternal())
			strLayDisp.Append(pLayer->GetName() + "; ");
	}
	strLayDisp.Delete(strLayDisp.GetLength() - 2, 5);

	if(strLayDisp.GetLength() == 0)
		strLayDisp = "NewFile";

	CPrimText *pText = new CPrimText(fd, rs, strLayDisp);

	pText->SetPenColor(1);
	pText->Display(pDC, false);

	pstate.SetPenColor(penColor);
}

///<summary>
/// Displays drawing and determines which segments are detectable.
///</summary>
void CPegDoc::LayersDisplayAll(CDC* pDC)
{	
	CPegView* pView = CPegView::GetActiveView();

	bool bIdentifyTrap = trapsegs.Identify() && !trapsegs.IsEmpty();
	
	detsegs.RemoveAll();

	// mak - add tracing layer name text
//	if(printState.GetDispLayer() && pView->IsPrintPreviewOn())
//		DisplayTracingLayerText(pDC);
		
	//if (pDC == 0)
	//{
	//	//if (pView->GetViewWireframe())
	//	//	opengl::PolygonModeLine();
	//	//else
	//	//	opengl::PolygonModeFill();
	//	//
	//	//for (int i = 0; i < LayersGetSize(); i++)
	//	//{
	//	//	CLayer* pLayer = LayersGetAt(i);
	//	//	pLayer->Display(pDC, bIdentifyTrap);
	//	//}
	//	//opengl::Flush();
	//}
	//else
	//{
		CPrim::SpecPolygonStyle() = (pView->GetViewWireframe()) ? POLYGON_HOLLOW : short(-1);
		int iPrimState = pstate.Save();
		
		// display external tracing layers
		for (WORD w = 0; w < LayersGetSize(); w++)
		{
			CLayer* pLayer = LayersGetAt(w);

			if(!pLayer->IsInternal() && (pLayer->GetName() != "Bitmap"))
				pLayer->Display(pDC, bIdentifyTrap);
		}

		// display internal, non-working layers
		for (WORD w = 0; w < LayersGetSize(); w++)
		{
			CLayer* pLayer = LayersGetAt(w);

			if(pLayer->IsInternal() && (pLayer->GetName() != "Bitmap"))
			{
				if(pLayer != CPegDoc::WorkLayerGet())
					pLayer->Display(pDC, bIdentifyTrap);
			}
		}

		// display working layer
		if(WorkLayerGet()->GetName() != "Bitmap")
			CPegDoc::WorkLayerGet()->Display(pDC, bIdentifyTrap);

		pstate.Restore(pDC, iPrimState);
		CPrim::SpecPolygonStyle() = - 1;
//	}
}

///<summary>
/// Displays drawing and determines which segments are detectable.
///</summary>
void CPegDoc::BorderLayersDisplayAll(CDC* pDC)
{	
//	CPegView* pView = CPegView::GetActiveView();

	bool bIdentifyTrap = false;//trapsegs.Identify() && !trapsegs.IsEmpty();
	
//	detsegs.RemoveAll();

	// mak - add tracing layer name text
//	if(printState.GetDispLayer() && pView->IsPrintPreviewOn())
//		DisplayTracingLayerText(pDC);
		
	//if (pDC == 0)
	//{
	//	//if (pView->GetViewWireframe())
	//	//	opengl::PolygonModeLine();
	//	//else
	//	//	opengl::PolygonModeFill();
	//	//
	//	//for (int i = 0; i < LayersGetSize(); i++)
	//	//{
	//	//	CLayer* pLayer = LayersGetAt(i);
	//	//	pLayer->Display(pDC, bIdentifyTrap);
	//	//}
	//	//opengl::Flush();
	//}
	//else
	//{
//		CPrim::SpecPolygonStyle() = (pView->GetViewWireframe()) ? POLYGON_HOLLOW : short(- 1);
		int iPrimState = pstate.Save();
		
		for (WORD w = 0; w < BorderLayersGetSize(); w++)
		{
			CLayer* pLayer = BorderLayersGetAt(w);
			pLayer->Translate(CVec(job2state.m_pntOrigin[0], job2state.m_pntOrigin[1], 0));
			pLayer->Display(pDC, bIdentifyTrap);
			pLayer->Translate(CVec(-job2state.m_pntOrigin[0], -job2state.m_pntOrigin[1], 0));
		}

		pstate.Restore(pDC, iPrimState);
//		CPrim::SpecPolygonStyle() = - 1;

//		job2state.m_pntOrigin[0]);
//			::DlgBoxSetItemDouble(hDlg, IDC_ORIGIN_Y, job2state.m_pntOrigin[1]
//	}
}

CLayers	*CPegDoc::LayersGetAll() 
{ 
	CLayers *layersTemp = new CLayers;
	//for(int i = 0; m_layers.GetCount(); i++)
	//	layersTemp->Add(m_layers.GetAt(i));

	return layersTemp; 
}

void CPegDoc::LayersSetAll(CLayers cLayers) 
{ 
	////CLayer layersTemp;
	//for(int i = 0; cLayers.GetCount(); i++)
	//	m_layers.Add(cLayers.GetAt(i));

	////m_layers = cLayers; 
}

CLayer* CPegDoc::LayersGet(const CString& strName) const
{
	int i = LayersLookup(strName);
	return (i < 0 ? (CLayer*) 0 : m_layers.GetAt(i));
}

CLayer* CPegDoc::LayersGetAt(int i) const 
{
	return (i >= (int) m_layers.GetSize() ? (CLayer*) 0 : m_layers.GetAt(i));
}

CLayer* CPegDoc::BorderLayersGetAt(int i) const 
{
	return (i >= (int) m_BorderLayers.GetSize() ? (CLayer*) 0 : m_BorderLayers.GetAt(i));
}

void CPegDoc::BorderLayersRemove(const CString& strName)
{
	int i = BorderLayersLookup(strName);

	if (i >= 0)
		BorderLayersRemoveAt(i);
}

int CPegDoc::LayersLookup(const CString& strName) const
{
	for (WORD w = 0; w < m_layers.GetSize(); w++)
	{
		CLayer* pLayer = m_layers.GetAt(w);
		if (strName.CompareNoCase(pLayer->GetName()) == 0) 
			return w;
	}
	return -1;
}

int CPegDoc::BorderLayersLookup(const CString& strName) const
{
	for (WORD w = 0; w < m_BorderLayers.GetSize(); w++)
	{
		CLayer* pLayer = m_BorderLayers.GetAt(w);
		if (strName.CompareNoCase(pLayer->GetName()) == 0) 
			return w;
	}
	return -1;
}

void CPegDoc::LayersRemove(const CString& strName)
{
	int i = LayersLookup(strName);

	if (i >= 0)
		LayersRemoveAt(i);
}

void CPegDoc::LayersRemoveAll()
{
	for (WORD w = 0; w < m_layers.GetSize(); w++)
	{
		CLayer* pLayer = LayersGetAt(w);
		pLayer->RemoveSegs();
		delete pLayer;
	}
	m_layers.RemoveAll();
}

void CPegDoc::BorderLayersRemoveAll()
{
	for (WORD w = 0; w < m_BorderLayers.GetSize(); w++)
	{
		CLayer* pLayer = BorderLayersGetAt(w);
		pLayer->RemoveSegs();
		delete pLayer;
	}
	m_BorderLayers.RemoveAll();
}

void CPegDoc::LayersRemoveAt(int i)
{
	CLayer* pLayer = LayersGetAt(i);

	pLayer->RemoveSegs();
	delete pLayer;

	m_layers.RemoveAt(i);
}

void CPegDoc::BorderLayersRemoveAt(int i)
{
	CLayer* pLayer = BorderLayersGetAt(i);

	pLayer->RemoveSegs();
	delete pLayer;

	m_BorderLayers.RemoveAt(i);
}

int CPegDoc::LayersRemoveEmpty()
{
	int count = 0;

	for (int i = m_layers.GetSize() - 1; i > 0; i--)
	{
		CLayer* pLayer = LayersGetAt(i);
		if (pLayer->IsEmpty() && !pLayer->IsHot())
		{
			pLayer->RemoveSegs();
			delete pLayer;
			m_layers.RemoveAt(i);
			count++;
		}
	}

	return count;
}

CLayer* CPegDoc::LayersSelUsingPoint(const CPnt& pt) const
{
	CSeg* pSeg = detsegs.SelSegAndPrimUsingPoint(pt);
	
	if (pSeg != 0)
	{
		for (WORD w = 0; w < m_layers.GetSize(); w++)
		{
			CLayer* pLayer = m_layers.GetAt(w);
			if (pLayer->Find(pSeg)) 
				return (pLayer);
		}
	}	
	for (WORD w = 0; w < m_layers.GetSize(); w++)
	{
		CLayer* pLayer = LayersGetAt(w);

		if (pLayer->SelSegUsingPoint(pt) != 0)
			return (pLayer);
	}
	return 0;
}

void CPegDoc::LayersOmitHidden()
{
	for (WORD w = 0; w < LayersGetSize(); w++)
	{
		CLayer* pLayer = LayersGetAt(w);

		if(pLayer->IsOff())
			CPegDoc::LayersRemoveAt(w);
	}
}

bool CPegDoc::OpenAutoCADImport()
{
	static DWORD nFilterIndex = 1;

	CString strPathName = GetPathName();
	CString strWDirectory = strPathName.Left(strPathName.ReverseFind('\\'));
	
	//char szFilter[75];
	//SecureZeroMemory(szFilter, 75);
	//::LoadString(app.GetInstance(), IDS_IMPORT_FILTER_DWG, szFilter, sizeof(szFilter));

	OPENFILENAME of;
	::ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lpstrInitialDir = strWDirectory;
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = 0;
	of.hInstance = app.GetInstance();
	//of.lpstrFilter = szFilter;
	of.lpstrFilter = "DWG Files (.dwg)\0*.dwg\0";
	of.nFilterIndex = nFilterIndex;
	of.lpstrFile = new char[MAX_PATH];
	of.lpstrFile[0] = 0;
	of.nMaxFile = MAX_PATH;
	of.lpstrTitle = "Import Autocad File";
	of.lpstrDefExt = "dwg";
	of.Flags = OFN_HIDEREADONLY	;
	
	if (GetOpenFileName(&of))
	{
		int iSaveOpenFile = (m_wOpenFileType == FILE_TYPE_NONE) ? IDNO : IDYES;
		if(iSaveOpenFile == IDYES)
			iSaveOpenFile = msgConfirm(IDS_MSG_SAVE_CHANGES, m_strPathName);
		
		if(iSaveOpenFile != IDCANCEL)
		{
			if(iSaveOpenFile == IDYES)
				OpenFileSave();
			
			CString strPathName = of.lpstrFile;
			nFilterIndex = of.nFilterIndex;

			WORD wFileType = app.GetFileTypeFromPath(strPathName);

			if(wFileType == FILE_TYPE_DWG || wFileType == FILE_TYPE_DXF)
			{
				return FileAutoCADOpenExclusive(strPathName);
			}	
		}
		return false;
	}
	delete [] of.lpstrFile;
	return false;
}

bool CPegDoc::OpenPegImport()
{
	static DWORD nFilterIndex = 1;

	CString strPathName = GetPathName();
	CString strWDirectory = strPathName.Left(strPathName.ReverseFind('\\'));
	
	//char szFilter[75];
	//SecureZeroMemory(szFilter, 75);
	//::LoadString(app.GetInstance(), IDS_IMPORT_FILTER_PEG, szFilter, sizeof(szFilter));

	OPENFILENAME of;
	::ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lpstrInitialDir = strWDirectory;
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = 0;
	of.hInstance = app.GetInstance();
	//of.lpstrFilter = szFilter;
	of.lpstrFilter = "Peg Files (.peg)\0*.peg\0";
	of.nFilterIndex = nFilterIndex;
	of.lpstrFile = new char[MAX_PATH];
	of.lpstrFile[0] = 0;
	of.nMaxFile = MAX_PATH;
	of.lpstrTitle = "Import PegAEsys File";
	of.lpstrDefExt = "peg";
	of.Flags = OFN_HIDEREADONLY	;
	
	if(GetOpenFileName(&of))
	{
		int iSaveOpenFile = (m_wOpenFileType == FILE_TYPE_NONE) ? IDNO : IDYES;
		if(iSaveOpenFile == IDYES)
			iSaveOpenFile = msgConfirm(IDS_MSG_SAVE_CHANGES, m_strPathName);
		
		if(iSaveOpenFile != IDCANCEL)
		{
			if(iSaveOpenFile == IDYES)
				OpenFileSave();
			
			CString strPathName = of.lpstrFile;
			nFilterIndex = of.nFilterIndex;

			WORD wFileType = app.GetFileTypeFromPath(strPathName);

			if(wFileType == FILE_TYPE_PEG)
			{
				return FilePegOpenExclusive(strPathName);
			}
		}
		return false;
	}
	delete [] of.lpstrFile;
	
	return false;
}


using std::vector;

void CPegDoc::OnImportSpreadsheet()
{
	CString str;
	char szBuf[256];

	CSeg *pSeg;
	CPnt ptPvt = ORIGIN;

	vector < vector<CString> > strMatrix;

	std::ifstream inFile("C:\\Book1.txt");

	while(!inFile.eof())
	{
		inFile.getline(szBuf, 256);

		int curPos= 0;

		CString resToken;
		str = szBuf;

		vector<CString> strRow;

		resToken = str.Tokenize("\t", curPos);
		while(resToken != "")
		{
//			MessageBox(0, resToken, 0, 0);
			strRow.push_back(resToken);
			resToken= str.Tokenize("\t", curPos);
		}

		strMatrix.push_back(strRow);
		strRow.clear();
	}

	for(int k = 0; k < (int)strMatrix[0].size(); k++)
	{
		int nLength = 0;
		int nIndex = 0;

		for(int i = 0; i < (int)strMatrix.size()-1; i++)
		{
			int nTemp = strMatrix[i][k].GetLength();
			if(nTemp > nLength)
			{
				nLength = nTemp;
				nIndex = i;
			}
		}

		CPnts pntsBox;

		CCharCellDef ccd;
		pstate.GetCharCellDef(ccd);
		CRefSys rs(ORIGIN, ccd);

		CFontDef fd;
		pstate.GetFontDef(fd);

		text_GetBoundingBox(fd, rs, strMatrix[nIndex][k].GetLength() + 2, annotate::dGapSpacing, pntsBox);

		for(int j = 0; j < (int)strMatrix.size()-1; j++)
		{
			CCharCellDef ccd;
			pstate.GetCharCellDef(ccd);
			CRefSys rs(ptPvt, ccd);

			CFontDef fd;
			pstate.GetFontDef(fd);

			CPrim *pPrim = new CPrimText(fd, rs, 0);
			pPrim->SetText(strMatrix[j][k]);
			pSeg = new CSeg(pPrim);

			WorkLayerAddTail(pSeg);

			ptPvt = text_GetNewLinePos(fd, rs, 1., 0);
		}

		//draw lines - vertical
		CPnts pntsBox2, pntsBox3;
		text_GetBoundingBox(fd, rs, strMatrix[0][k].GetLength(), annotate::dGapSpacing, pntsBox2);
		CPnt pt1 = pntsBox2[3];
		pt1[0] = ptPvt[0];
		CPnt pt2a = text_GetNewLinePos(fd, rs, (double)strMatrix.size()-2, 0);
		text_GetBoundingBox(fd, rs, strMatrix[strMatrix.size()-2][k].GetLength(), annotate::dGapSpacing, pntsBox3);
		CPnt pt2;
		pt2[2] = 0;
		pt2[1] = pt2a[1] + pntsBox3[0][1];
		pt2[0] = ptPvt[0];
		CPrimLine* pLine = new CPrimLine(pt1, pt2);
		pSeg = new CSeg(pLine);
		WorkLayerAddTail(pSeg);

//		//draw lines - horizontal
////		CPnt pt1a = text_GetNewLinePos(fd, rs, k+1, 0);
//		text_GetBoundingBox(fd, rs, strMatrix[0][0].GetLength(), annotate::dGapSpacing, pntsBox2);
////		pt1 = pntsBox2[3];
//		pt1[2] = 0;
//		pt1[1] = ptPvt[1] + pt2a[1];
//		pt1[0] = 0;
//		text_GetBoundingBox(fd, rs, strMatrix[0][strMatrix[0].size()-1].GetLength(), annotate::dGapSpacing, pntsBox3);
//		pt2[2] = 0;
////		pt2[1] = pt2a[1] + pntsBox3[0][1];
//		pt2[1] = ptPvt[1];//pt2a[1] + pntsBox3[0][1];
//		pt2[0] = ptPvt[0] + pntsBox3[2][0];
//		CPrimLine* pLine2 = new CPrimLine(pt1, pt2);
//		pSeg = new CSeg(pLine2);
//		WorkLayerAddTail(pSeg);

		ptPvt[2] = 0;
		ptPvt[1] = 0;
		ptPvt[0] += pntsBox[1][0];
	}
}

void CPegDoc::OnImportAutoCAD()
{
	istate.m_bIsImport = true;

	if(ImportAutoCADSettingsConfirm())
	{
		app.ImportFilter();
		::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_IMPORT_REPORT), app.GetSafeHwnd(), DlgProcImportReport);
	}

	istate.m_bIsImport = false;
}

void CPegDoc::OnImportPeg()
{
	istate.m_bIsImport = true;

	if(ImportPegSettingConfirm())
	{
		app.ImportFilter();
		::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_IMPORT_REPORT), app.GetSafeHwnd(), DlgProcImportReport);
	}

	istate.m_bIsImport = false;
}

void CPegDoc::ImportScaleSet()
{
	double scaleValue = istate.GetScaleValue();

	// trap all
	OnEditTrapWorkAndActive();

	//CLayer *pLayer;
	//pLayer->

	// goto homepoint
	CPnt pt(0, 0, 0);
	app.CursorPosSet(pt);

	// edit set pivot
	pt = app.CursorPosGet();
	trapsegs.PvtPt() = pt;

	// edit options set to x = y = z = scale value
	dlgproceditops::SetScale(scaleValue, scaleValue, scaleValue);

	// reduce
	CTMat tm;
	tm.Translate(ORIGIN - trapsegs.PvtPt());
	tm.Scale(dlgproceditops::GetInvertedScale());
	tm.Translate(trapsegs.PvtPt());
	trapsegs.TransformSegs(tm);

	// remove trap
	OnEditTrapQuit();

	// set scale for drawing
	app.SetScale(scaleValue);

	// reset values
	dlgproceditops::SetScale(2, 2, 2);
}

bool CPegDoc::ImportAutoCADSettingsConfirm()
{
//	int iRetVal = IDOK;

//	if(GetPathName() != "0")
//		iRetVal = msgConfirm(IDS_MSG_SAVE_CHANGES, GetPathName());

	/////////////////////////////////////////////////////////////////////////////
	//int iRetVal = IDNO;
	//if((m_wOpenFileType != FILE_TYPE_NONE) || (GetHotCount() > 0))
	//	iRetVal = IDYES;

	//if(iRetVal == IDYES)
	//{
	//	if(!IsFileReadOnly())
	//		iRetVal = msgConfirm(IDS_MSG_SAVE_CHANGES, GetPathName());
	//	else
	//		iRetVal = IDNO;
	//}

	int iRetVal = DisplaySaveMessageConfirmation(IDS_MSG_SAVE_CHANGES);
	////////////////////////////////////////////////////////////////////////////

	if (iRetVal != IDCANCEL)
	{
		OpenFileClose(iRetVal == IDYES);

		int iRetVal2 = ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_IMPORT), app.GetSafeHwnd(), DlgProcOptionsTabImport);

		if(iRetVal2 != IDCANCEL)
		{
			bool status = OpenAutoCADImport();

			if(status)
			{
				CString strPathName = GetPathName();

				// FIX: trim right bug?
//				strPathName.TrimRight(".dwg");
				strPathName = strPathName.Left(strPathName.GetLength() - 4);
				strPathName += ".peg";

//				CFileException e;
				CFilePeg* pFilePegDst = new CFilePeg;
//				pFilePegDst->Open(strPathName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite, &e);
				bool bSuccess = pFilePegDst->Open(strPathName, GENERIC_WRITE, false);

//				if (e.m_cause == CFileException::sharingViolation)
				if(!bSuccess)
				{	
					msgWarning(IDS_MSG_FILE_LOCKED, strPathName);
					delete pFilePegDst;
				}
				else
				{
					CFilePeg* pFile = GetFilePeg();

					if (pFile != 0)
						pFile->Close();

					pFilePegDst->Unload();
					
					SetFilePeg(pFilePegDst);
					SetOpenFile(FILE_TYPE_PEG, strPathName);

					//app.StartAutoSave();
				}	
			}
			return status;
		}
		return false;
	}
	return false;
}

bool CPegDoc::ImportPegSettingConfirm()
{
	int iRetVal = IDOK;

	if(GetPathName() != "0")
		iRetVal = msgConfirm(IDS_MSG_SAVE_CHANGES, GetPathName());

	if (iRetVal != IDCANCEL)
	{
		OpenFileClose(iRetVal == IDYES);

		int iRetVal2 = ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_IMPORT), app.GetSafeHwnd(), DlgProcOptionsTabImport);

		if(iRetVal2 != IDCANCEL)
		{
			if(OpenPegImport())
			{
				//app.StartAutoSave();
				return true;
			}
		}
		return false;
	}
	return false;
}

#include "..\Adobe\acrobat.h"

bool CPegDoc::OnImportPdf()
{
	CString strPathName = GetPathName();
	CString strWDirectory = strPathName.Left(strPathName.ReverseFind('\\'));
	
	//char szFilter[27];
	//SecureZeroMemory(szFilter, 27);
	//::LoadString(app.GetInstance(), ID_IMPORT_PORTABLEDOCUMENTFORMAT, szFilter, sizeof(szFilter));

	CPnt pntCursor = app.CursorPosGet();

	OPENFILENAME	of;
	::ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lpstrInitialDir = strWDirectory;
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = 0;
	of.hInstance = app.GetInstance();
	//of.lpstrFilter = szFilter;
	of.lpstrFilter = "PDF Files (.pdf)\0*.pdf\0";
	of.lpstrFile = new char[MAX_PATH];
	of.lpstrFile[0] = 0;
	of.nMaxFile = MAX_PATH;
	of.lpstrTitle = "Import Portable Document Format";
	of.lpstrDefExt = "pdf";
		
	if (GetOpenFileName(&of))
	{
		// === Initialize OLE libraries. 
		if (!AfxOleInit())	
		{
			AfxMessageBox("OLE initialization failed in BasicIacVc sample.");
			return false;
		}

		// === Create an Acrobat IAC PDDoc object  
		COleException e;
		CAcroApp app;
		CAcroPDDoc *gAcroPdDoc = NULL;
		gAcroPdDoc = new CAcroPDDoc;
		gAcroPdDoc->CreateDispatch("AcroExch.PDDoc", &e);
		if(!gAcroPdDoc) 
		{
			AfxMessageBox("Creating Acrobat IAC object failed in BasicIacVc sample.");
			return false;
		}

		MessageBox(0, of.lpstrFile, 0, 0);
		
		// === Open a pdf file.  
		if(!gAcroPdDoc->Open(of.lpstrFile)) 
		{
			CString message = "Opening the PDF file \"";
			message += of.lpstrFile;
			message += "\" failed in the BasicIacVc sample.";
			AfxMessageBox(message);
			return false;
		}

		// === Get the number of pages, and compose a massage.  
		char str[256];
		sprintf(str ,"PDF document %s is loaded in Acrobat through IAC program.\n", of.lpstrFile);
		long n = gAcroPdDoc->GetNumPages();
		if(n != -1) 
		{
			CAcroPDPage *pdfPage = new CAcroPDPage(gAcroPdDoc->AcquirePage(0));
			
			if(pdfPage == 0)
				return false;

			CAcroPoint *pdfPoint = new CAcroPoint(pdfPage->GetSize());

			if(pdfPoint == 0)
				return false;

			short sX = 100;//pdfPoint->GetX();
			short sY = 100;//pdfPoint->GetY();

			CAcroRect pdfRect = 0;
			pdfRect.SetLeft(0);
			pdfRect.SetRight(sX);
			pdfRect.SetTop(0);
			pdfRect.SetBottom(sY);
			pdfPage->CopyToClipboard(pdfRect, 0, 0, 100);

			OnEditTrapPaste();

			//// Load the pdf image in a Bitmap
			//Bitmap bmpPdf(hBitmap, NULL);

			//// Load the template image in a Bitmap
			//Bitmap bmpTemplate(szTemplateFile);

			//// Create new blank bitmap (+ 7 for template border)
			//Bitmap bmpThumb(sizeThumb.cx + 7, sizeThumb.cy + 7, PixelFormat24bppRGB);

			//// Create graphics object to draw on blank bitmap
			//Graphics gxThumb(&bmpThumb);

			//// Draw pdf image to blank bitmap
			//gxThumb.DrawImage(&bmpPdf, 2, 2, sizeThumb.cx, sizeThumb.cy);

			//// Draw transparent template image to thumbnail bitmap
			//gxThumb.DrawImage(&bmpTemplate, 0, 0);

			//// Save as .png file
			//CLSID clsidPng;
			//GetEncoderClsid(L"image/png", &clsidPng);
			//bmpThumb.Save(szThumbPath, &clsidPng, NULL);
		}
		else
			sprintf(str, "Error in getting page number.");
		
		// message
		AfxEnableControlContainer();
		//AfxMessageBox(str);

		// === Code to finish program.
		gAcroPdDoc->Close();
		delete gAcroPdDoc;
		gAcroPdDoc = NULL;


		//// Render to clipboard, scaled by 100 percent (ie. original size)
		//// Even though we want a smaller image, better for us to scale in .NET
		//// than Acrobat as it would greek out small text
		//// see http://www.adobe.com/support/techdocs/1dd72.htm
		//pdfPage.CopyToClipboard(pdfRect, 0, 0, 100);

		///////////////////////////////////////////////////

//		//CFileBitmap fbmp(of.lpstrFile);
//		//CBitmap		bm;
//		//CPalette	pal;
//		//
//		//fbmp.Load(of.lpstrFile, bm, pal);
//
//		BitmapLayerLoad();
//		BitmapLayerLoadFile(pntCursor, of.lpstrFile);
////		CPrimText *pPrim = new CPrimText;
////		WorkLayerAddTail(new CSeg(pPrim));
	}
	delete [] of.lpstrFile;

	return true;
}

bool CPegDoc::OnImportBmp()
{
	CString strPathName = GetPathName();
	CString strWDirectory = strPathName.Left(strPathName.ReverseFind('\\'));
	
	//char szFilter[27];
	//SecureZeroMemory(szFilter, 27);
	//::LoadString(app.GetInstance(), IDS_OPENFILE_FILTER_BITMAPS, szFilter, sizeof(szFilter));

	CPnt pntCursor = app.CursorPosGet();

	OPENFILENAME	of;
	::ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lpstrInitialDir = strWDirectory;
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = 0;
	of.hInstance = app.GetInstance();
	//of.lpstrFilter = szFilter;
	of.lpstrFilter = "Bitmap Files (.bmp)\0*.bmp\0";
	of.lpstrFile = new char[MAX_PATH];
	of.lpstrFile[0] = 0;
	of.nMaxFile = MAX_PATH;
	of.lpstrTitle = "Import Bitmap Image";
	of.lpstrDefExt = "bmp";
		
	if (GetOpenFileName(&of))
	{
		BitmapLayerLoad();
		BitmapLayerLoadFile(pntCursor, of.lpstrFile);
	}
	delete [] of.lpstrFile;

	return true;
}

int CPegDoc::DisplaySaveMessageConfirmation(UINT uiMsg)
{
	int iSaveOpenFile = IDNO;
	if((m_wOpenFileType != FILE_TYPE_NONE) || (GetHotCount() > 0))
		iSaveOpenFile = IDYES;

	if(iSaveOpenFile == IDYES)
	{
		if(!IsFileReadOnly())
			iSaveOpenFile = msgConfirm(uiMsg, GetPathName());
		else
			iSaveOpenFile = IDNO;
	}

	return iSaveOpenFile;
}

bool CPegDoc::OpenFileRecover()
{
	bool bAddMRU = false;
	static DWORD nFilterIndex = 1;
	
	//char szFilter[250];
	//SecureZeroMemory(szFilter, 250);
	//::LoadString(app.GetInstance(), IDS_OPENFILE_FILTER_PEG, szFilter, sizeof(szFilter));

	OPENFILENAME of;
	::ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = 0;
	of.hInstance = app.GetInstance();
	//of.lpstrFilter = szFilter;
	of.lpstrFilter = "Peg Files (.peg)\0*.peg\0";
	of.nFilterIndex = nFilterIndex;
	of.lpstrFile = new char[MAX_PATH];
	of.lpstrFile[0] = 0;
	of.nMaxFile = MAX_PATH;
	of.lpstrTitle = "Recover";
	// for default only have OFN_HIDEREADONLY flag
	of.Flags = OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLESIZING;
	of.lpstrDefExt = "peg";
	// next two added 
	of.lpfnHook = OFNHookProcFileOpen;
	of.lpTemplateName = MAKEINTRESOURCE(IDD_FILE_OPEN);
	of.lpstrInitialDir = m_strLastDirectory;
	
	if(GetOpenFileName(&of))
	{
		m_strLastDirectory = of.lpstrFile;

		// if a file type is open, prompt user about saving
//		int iSaveOpenFile = (m_wOpenFileType == FILE_TYPE_NONE) ? IDNO : IDYES;
//		if(iSaveOpenFile == IDYES)// && !m_bIsFileReadOnly)
//			iSaveOpenFile = msgConfirm(IDS_MSG_SAVE_CHANGES, m_strPathName);

//		int iSaveOpenFile = IDNO;
//		if((m_wOpenFileType != FILE_TYPE_NONE) || (GetHotCount() > 0))
//			iSaveOpenFile = IDYES;

//		if(iSaveOpenFile == IDYES)
//			iSaveOpenFile = msgConfirm(IDS_MSG_SAVE_CHANGES, GetPathName());

		int iSaveOpenFile = DisplaySaveMessageConfirmation(IDS_MSG_SAVE_CHANGES);
		
		// if the user does not cancel the open command - whether saving or not
		if(iSaveOpenFile != IDCANCEL)
		{
			// if the user decides to save
			if(iSaveOpenFile == IDYES)
				OpenFileSave();
			//close the file before opening whether it was saved first or not
			// mak - multiple open files no longer allowed... current file
			// must be closed before new file is opened
//			OpenFileClose(false);
			app.OnFileNew(false);
			
			CString strPathName = of.lpstrFile;
			nFilterIndex = of.nFilterIndex;

			WORD wFileType = app.GetFileTypeFromPath(strPathName);

//			if(wFileType == FILE_TYPE_BMP)
//				::MessageBox(0, "To load a background image use command:\n\n[File] [Sheet Setup] [Background Image] [Load]", 0, MB_ICONINFORMATION);
//			else if(wFileType == FILE_TYPE_DWG || wFileType == FILE_TYPE_DXF)
//				bAddMRU = FileAutoCADOpenExclusive(strPathName);
//			else if(wFileType == FILE_TYPE_FFA)
//				bAddMRU = FileFFAOpenExclusive(strPathName);
			if(wFileType == FILE_TYPE_PEG)
				bAddMRU = FilePegRecover(strPathName);
//			else if(FileTypeIsTracing(wFileType))
//				bAddMRU = TracingOpen(strPathName);

			// mak - mru registry
//			if(bAddMRU)
//			{
//				if(!strPathName.IsEmpty())
//				{
//					doc.SetRegistry(strPathName);
//					doc.DisplayMRU();
//				}

				//if(m_bIsFileReadOnly)
				//{
				//	::EnableMenuItem(app.GetMenu(), IDM_FILE_SAVE, MF_BYCOMMAND | MF_GRAYED);
				//	::EnableMenuItem(app.GetMenu(), IDM_FILE_SAVEAS, MF_BYCOMMAND | MF_GRAYED);
				//}

//				if((!app.IsThisSoloWindow()))// && (!m_bIsFileReadOnly))
//					app.StartAutoSave();
//			}
		}
	}
	delete [] of.lpstrFile;

	return bAddMRU; 
}

void CPegDoc::OnMruOpen(int iFileNumber)
{
//	int iSaveOpenFile = (m_wOpenFileType == FILE_TYPE_NONE) ? IDNO : IDYES;
//	if(iSaveOpenFile == IDYES)// && !m_bIsFileReadOnly)
//		iSaveOpenFile = msgConfirm(IDS_MSG_SAVE_CHANGES, m_strPathName);
//	
//	if(iSaveOpenFile != IDCANCEL)
//	{
//		if(iSaveOpenFile == IDYES)
//			OpenFileSave();
//
//		app.OnFileNew(false);
//
//		TCHAR szData[1024];
//		DWORD buffSize = 1024;
//		HKEY hKey;
//		RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\MRU", 0, KEY_ALL_ACCESS, &hKey);
//		
//		CString mruList = registry.GetKeyValue();
//		char item = mruList[iFileNumber-1];
//
//		switch(item)
//		{
//			case 'a':				
//				RegQueryValueEx(hKey, "a", NULL, NULL, (LPBYTE)szData, &buffSize);
//				break;
//			case 'b':
//				RegQueryValueEx(hKey, "b", NULL, NULL, (LPBYTE)szData, &buffSize);
//				break;
//			case 'c':
//				RegQueryValueEx(hKey, "c", NULL, NULL, (LPBYTE)szData, &buffSize);
//				break;
//			case 'd':
//				RegQueryValueEx(hKey, "d", NULL, NULL, (LPBYTE)szData, &buffSize);
//				break;
//			case 'e':
//				RegQueryValueEx(hKey, "e", NULL, NULL, (LPBYTE)szData, &buffSize);
//				break;
//		}
//		CString strPathName = szData;
//		RegCloseKey(hKey);
//
//		// set working directory for external files
//		CString strWDirectory = strPathName.Left(strPathName.ReverseFind('\\'));
//		SetCurrentDirectory(strWDirectory);
//
//		WORD wFileType = app.GetFileTypeFromPath(strPathName);
//		bool bAddMRU = false;
//
//		if(wFileType == FILE_TYPE_BMP)
//			::MessageBox(0, "To load a background image use command:\n\n[File] [Sheet Setup] [Background Image] [Load]", 0, MB_ICONINFORMATION);
//		else if(wFileType == FILE_TYPE_DWG || wFileType == FILE_TYPE_DXF)
//			bAddMRU = FileAutoCADOpenExclusive(strPathName);
//		else if(wFileType == FILE_TYPE_FFA)
//			bAddMRU = FileFFAOpenExclusive(strPathName);
//		else if(wFileType == FILE_TYPE_PEG)
//			bAddMRU = FilePegOpenExclusive(strPathName);
//		else if(FileTypeIsTracing(wFileType))
//			bAddMRU = TracingOpen(strPathName);
//
//		// mak - mru registry
//		if(bAddMRU)
//		{
//			if(!strPathName.IsEmpty())
//			{
//				doc.SetRegistry(strPathName);
//				doc.DisplayMRU();
//			}
//
//			//m_bIsFileReadOnly = false;
//			//if(!m_bIsFileReadOnly)
//			//{
//			//	::EnableMenuItem(app.GetMenu(), IDM_FILE_SAVE, MF_BYCOMMAND | MF_ENABLED);
//			//	::EnableMenuItem(app.GetMenu(), IDM_FILE_SAVEAS, MF_BYCOMMAND | MF_ENABLED);
//			//}
//
////			app.StartAutoSave();
//		}
//	}

	////////////////////////////////

//	int iSaveOpenFile = (m_wOpenFileType == FILE_TYPE_NONE) ? IDNO : IDYES;
//	if(iSaveOpenFile == IDYES)// && !m_bIsFileReadOnly)
//		iSaveOpenFile = msgConfirm(IDS_MSG_SAVE_CHANGES, m_strPathName);

	//int iSaveOpenFile = IDNO;
	//if((m_wOpenFileType != FILE_TYPE_NONE) || (GetHotCount() > 0))
	//	iSaveOpenFile = IDYES;

	//if(iSaveOpenFile == IDYES)
	//{
	//	if(!IsFileReadOnly())
	//		iSaveOpenFile = msgConfirm(IDS_MSG_SAVE_CHANGES, GetPathName());
	//	else
	//		iSaveOpenFile = IDNO;
	//}

	int iSaveOpenFile = DisplaySaveMessageConfirmation(IDS_MSG_SAVE_CHANGES);
	
	if(iSaveOpenFile != IDCANCEL)
	{
		if(iSaveOpenFile == IDYES)
			OpenFileSave();

		app.OnFileNew(false);

		CString strPathName = registry.GetItemFilePath(iFileNumber-1);

		// set working directory for external files
		CString strWDirectory = strPathName.Left(strPathName.ReverseFind('\\'));
		SetCurrentDirectory(strWDirectory);

		WORD wFileType = app.GetFileTypeFromPath(strPathName);
		bool bAddMRU = false;

		if(wFileType == FILE_TYPE_BMP)
			::MessageBox(0, "To load a background image use command:\n\n[File] [Sheet Setup] [Background Image] [Load]", 0, MB_ICONINFORMATION);
		else if(wFileType == FILE_TYPE_DWG || wFileType == FILE_TYPE_DXF)
			bAddMRU = FileAutoCADOpenExclusive(strPathName);
		else if(wFileType == FILE_TYPE_FFA)
			bAddMRU = FileFFAOpenExclusive(strPathName);
		else if(wFileType == FILE_TYPE_PEG)
			bAddMRU = FilePegOpenExclusive(strPathName);
		else if(FileTypeIsTracing(wFileType))
			bAddMRU = TracingOpen(strPathName);

		// mak - mru registry
		if(bAddMRU)
		{
			if(!strPathName.IsEmpty())
			{
				doc.SetRegistry(strPathName);
				doc.DisplayMRU();
			}

			CPegView::GetActiveView()->InvalidateRect(0, true);

			//m_bIsFileReadOnly = false;
			//if(!m_bIsFileReadOnly)
			//{
			//	::EnableMenuItem(app.GetMenu(), IDM_FILE_SAVE, MF_BYCOMMAND | MF_ENABLED);
			//	::EnableMenuItem(app.GetMenu(), IDM_FILE_SAVEAS, MF_BYCOMMAND | MF_ENABLED);
			//}

			//app.StartAutoSave();
		}
	}
}

bool CPegDoc::OpenFile()
{
	bool bAddMRU = false;
	static DWORD nFilterIndex = 1;
	
//	char szFilter[250];
//	SecureZeroMemory(szFilter, 250);
//	::LoadString(app.GetInstance(), IDS_OPENFILE_FILTER, szFilter, sizeof(szFilter));
	
	//CString strFilter = "PegAEsys Files (.peg .jb1 .tra .ffa)\0*.peg;*.jb1;*.tra;*.ffa\0";
	//strFilter += "Peg Files (.peg)\0*.peg\0";
	//strFilter += "Job Files (.jb1)\0*.jb1\0";
	//strFilter += "Tracing Files (.tra)\0*.tra\0";
	//strFilter += "Archived Files (.ffa)\0*.ffa\0";
	//strFilter += "DWG Files (.dwg)\0*.dwg\0";
	//strFilter += "DXF Files (.dxf)\0*.dxf\0";
	//strFilter += "All Files\0*.*\0";

	//"PegAEsys Files (.peg .jb1 .tra .ffa)\0*.peg;*.jb1;*.tra;*.ffa\0Peg Files (.peg)\0*.peg\0Job Files (.jb1)\0*.jb1\0Tracing Files (.tra)\0*.tra\0Archived Files (.ffa)\0*.ffa\0DWG Files (.dwg)\0*.dwg\0DXF Files (.dxf)\0*.dxf\0All Files\0*.*\0";

	OPENFILENAME of;
	::ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = 0;
	of.hInstance = app.GetInstance();
	//of.lpstrFilter = szFilter;
	of.lpstrFilter = "PegAEsys Files (.peg .jb1 .tra .ffa)\0*.peg;*.jb1;*.tra;*.ffa\0Peg Files (.peg)\0*.peg\0Job Files (.jb1)\0*.jb1\0Tracing Files (.tra)\0*.tra\0Archived Files (.ffa)\0*.ffa\0DWG Files (.dwg)\0*.dwg\0DXF Files (.dxf)\0*.dxf\0All Files\0*.*\0";
	of.nFilterIndex = nFilterIndex;
	of.lpstrFile = new char[MAX_PATH];
	of.lpstrFile[0] = 0;
	of.nMaxFile = MAX_PATH;
	of.lpstrTitle = "Open";
	// for default only have OFN_HIDEREADONLY flag
	of.Flags = OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLESIZING;
	of.lpstrDefExt = "peg";
	// next two added 
	of.lpfnHook = OFNHookProcFileOpen;
	of.lpTemplateName = MAKEINTRESOURCE(IDD_FILE_OPEN);
	of.lpstrInitialDir = m_strLastDirectory;
	
	if(GetOpenFileName(&of))
	{
		m_strLastDirectory = of.lpstrFile;

		// if a file type is open, prompt user about saving
//		int iSaveOpenFile = (m_wOpenFileType == FILE_TYPE_NONE) ? IDNO : IDYES;
//		if(iSaveOpenFile == IDYES)// && !m_bIsFileReadOnly)
//			iSaveOpenFile = msgConfirm(IDS_MSG_SAVE_CHANGES, m_strPathName);

		//int iSaveOpenFile = IDNO;
		//if((m_wOpenFileType != FILE_TYPE_NONE) || (GetHotCount() > 0))
		//	iSaveOpenFile = IDYES;

		//if(iSaveOpenFile == IDYES)
		//{
		//	if(!IsFileReadOnly())
		//		iSaveOpenFile = msgConfirm(IDS_MSG_SAVE_CHANGES, GetPathName());
		//	else
		//		iSaveOpenFile = IDNO;
		//}

		int iSaveOpenFile = DisplaySaveMessageConfirmation(IDS_MSG_SAVE_CHANGES);
		
		// if the user does not cancel the open command - whether saving or not
		if(iSaveOpenFile != IDCANCEL)
		{
			// if the user decides to save
			if(iSaveOpenFile == IDYES)
				OpenFileSave();
			//close the file before opening whether it was saved first or not
			// mak - multiple open files no longer allowed... current file
			// must be closed before new file is opened
//			OpenFileClose(false);
			app.OnFileNew(false);
			
			CString strPathName = of.lpstrFile;
			nFilterIndex = of.nFilterIndex;

			WORD wFileType = app.GetFileTypeFromPath(strPathName);

			if(wFileType == FILE_TYPE_BMP)
				::MessageBox(0, "To load a background image use command:\n\n[File] [Sheet Setup] [Background Image] [Load]", 0, MB_ICONINFORMATION);
			else if(wFileType == FILE_TYPE_DWG || wFileType == FILE_TYPE_DXF)
				bAddMRU = FileAutoCADOpenExclusive(strPathName);
			else if(wFileType == FILE_TYPE_FFA)
				bAddMRU = FileFFAOpenExclusive(strPathName);
			else if(wFileType == FILE_TYPE_PEG)
				bAddMRU = FilePegOpenExclusive(strPathName);
			else if(FileTypeIsTracing(wFileType))
				bAddMRU = TracingOpen(strPathName);

			// mak - mru registry
			if(bAddMRU)
			{
				if(!strPathName.IsEmpty())
				{
					doc.SetRegistry(strPathName);
					doc.DisplayMRU();
				}

				CPegView::GetActiveView()->InvalidateRect(0, true);

				//if(m_bIsFileReadOnly)
				//{
				//	::EnableMenuItem(app.GetMenu(), IDM_FILE_SAVE, MF_BYCOMMAND | MF_GRAYED);
				//	::EnableMenuItem(app.GetMenu(), IDM_FILE_SAVEAS, MF_BYCOMMAND | MF_GRAYED);
				//}

//				if((!app.IsThisSoloWindow()))// && (!m_bIsFileReadOnly))
					//app.StartAutoSave();
			}
		}
	}
	delete [] of.lpstrFile;

	return bAddMRU;
}

///<summary>
/// Opens the file indicated in a command line argument.
///</summary>
bool CPegDoc::OpenFile(const CString& strCmdLine)
{
	CString strPathName = strCmdLine;
//	Path_UnquoteSpaces(strPathName);

	WORD wFileType = app.GetFileTypeFromPath(strCmdLine);
	bool bAddMRU = false;

	CString strDirectory = strCmdLine;
	strDirectory = strDirectory.Left(strDirectory.ReverseFind('\\'));
	::SetCurrentDirectory(strDirectory);
		
	if(wFileType == FILE_TYPE_DWG || wFileType == FILE_TYPE_DXF)
		bAddMRU = FileAutoCADOpenExclusive(strCmdLine);
	else if(wFileType == FILE_TYPE_PEG)
		bAddMRU = FilePegOpenExclusive(strCmdLine);
	else if(FileTypeIsTracing(wFileType))
		bAddMRU = TracingOpen(strCmdLine);

	// mak - mru registry
	if(bAddMRU)
	{
		if(!strCmdLine.IsEmpty())
		{
			doc.SetRegistry(strCmdLine);
			doc.DisplayMRU();
		}

		CPegView::GetActiveView()->InvalidateRect(0, true);

		//if(m_bIsFileReadOnly)
		//{
		//	::EnableMenuItem(app.GetMenu(), IDM_FILE_SAVE, MF_BYCOMMAND | MF_GRAYED);
		//	::EnableMenuItem(app.GetMenu(), IDM_FILE_SAVEAS, MF_BYCOMMAND | MF_GRAYED);
		//}

	//	if(!app.IsThisSoloWindow())
			//app.StartAutoSave();
	}

	return bAddMRU;
}

bool CPegDoc::OpenFileClose(bool bSaveFirst)
{
	CPegView* pView = CPegView::GetActiveView();

	if(bSaveFirst && !m_bIsFileReadOnly)
		OpenFileSave();

	//FIX: - work?
	if(m_pFilePeg != 0)
		m_pFilePeg->Close();

	InitAll();
	SetOpenFile(FILE_TYPE_NONE, "0");

	// kill timer
	app.KillAutoSave();

//	::EnableMenuItem(app.GetMenu(), IDM_FILE_SAVE, MF_BYCOMMAND | MF_ENABLED);
//	::EnableMenuItem(app.GetMenu(), IDM_FILE_SAVEAS, MF_BYCOMMAND | MF_ENABLED);

	::EnableMenuItem(app.GetMenu(), IDM_FILE_MANAGE, MF_BYCOMMAND | MF_ENABLED); 

	pView->InvalidateRect(NULL, TRUE);
	return true;
}

bool CPegDoc::OnFileExportArchive()
{
	char *szFilter = "Peg Drawing Files (.peg)\000*.peg\000";

	OPENFILENAME of;
	::ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = 0;
	of.hInstance = app.GetInstance();
	of.lpstrFilter = szFilter;
	of.lpstrFile = new char[MAX_PATH];
	strcpy(of.lpstrFile, m_strPathName);
	of.nMaxFile = MAX_PATH;
	of.lpstrTitle = "Save Export As";
	// mak - OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_ENABLEHOOK only for default
	of.Flags = OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLESIZING;
	of.lpfnHook = OFNHookProcFileSaveAs;
	of.lpstrDefExt = "peg";
	of.nFilterIndex = 1;
	// mak - below only for template
	of.lpTemplateName = MAKEINTRESOURCE(IDD_FILE_SAVEAS);

	if(GetSaveFileName(&of))
	{
		CString strPathNameTo = of.lpstrFile;
		WORD wFileTypeTo = app.GetFileTypeFromPath(strPathNameTo);

		if(wFileTypeTo == FILE_TYPE_PEG)
		{
			CFilePeg* pFilePegDst = new CFilePeg;
			if(!pFilePegDst->Open(strPathNameTo, GENERIC_WRITE, false))
			{	
				msgWarning(IDS_MSG_FILE_LOCKED, strPathNameTo);
				delete pFilePegDst;
				return false;
			}
			else
			{
				pFilePegDst->Unload();
				pFilePegDst->Close();
				return true;
			}
		}
	}

	delete [] of.lpstrFile;
	return true;
}

void CPegDoc::OpenFileSaveCleanWarning()
{
	if(PenStylesGetSize() > 128)
	{
		int nReturn = MessageBox(app.GetSafeHwnd(), "This file contains a large quantity of pen styles, which may corrupt the file.  Would you like to remove the unused pen styles?\n\n(Recommended = YES)", "Warning", MB_YESNO | MB_ICONEXCLAMATION);

		if(nReturn == IDYES)
		{
			PenStylesRemoveUnused();
			MessageBox(app.GetSafeHwnd(), "Unused pen styles have been successfully removed", "Pen Style Maintenance", MB_OK | MB_ICONEXCLAMATION);
		}
	}
}

bool CPegDoc::OpenFileSave()
{
	bool bRet = false;

	OpenFileSaveCleanWarning();

	if(m_wOpenFileType == FILE_TYPE_NONE)
		bRet = OpenFileSaveAs();
	else if(m_wOpenFileType == FILE_TYPE_DWG || m_wOpenFileType == FILE_TYPE_DXF)
		bRet = OpenAutoCADSave(m_strPathName, true);
	else if(m_wOpenFileType == FILE_TYPE_PEG)
		bRet = OpenPegSave(m_strPathName, true);
	else if(FileTypeIsTracing(m_wOpenFileType))
		bRet = OpenTracingSave(m_strPathName, true);
	else
		msgWarning(IDS_MSG_NOTHING_TO_SAVE);
	
	//GIG - Start Autosave once file has a name
	app.StartAutoSave();
	return bRet;
}

bool CPegDoc::OpenFileSaveAs()
{
	bool bAddMRU = false;

	CString strPathName = GetPathName();
	CString strWDirectory = strPathName.Left(strPathName.ReverseFind('\\'));

	//char szFilter[170];
	//SecureZeroMemory(szFilter, 170);
	//::LoadString(app.GetInstance(), IDS_OPENFILE_FILTER_SAVEAS, szFilter, sizeof(szFilter));

//	if(IsFileReadOnly())
//		m_strPathName.Append

	OPENFILENAME of;
	::ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lpstrInitialDir = strWDirectory;
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = 0;
	of.hInstance = app.GetInstance();
	//of.lpstrFilter = szFilter;
	of.lpstrFilter = "Peg Files (.peg)\0*.peg\0Job Files (.jb1)\0*.jb1\0Tracing Files (.tra)\0*.tra\0";
	of.nFilterIndex = m_wOpenFileType;
	of.lpstrFile = new char[MAX_PATH];
	strcpy(of.lpstrFile, m_strPathName);
	of.nMaxFile = MAX_PATH;
	of.lpstrTitle = "Save As";
	// mak - OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_ENABLEHOOK only for default
	of.Flags = OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLESIZING;
	of.lpfnHook = OFNHookProcFileSaveAs;
	// mak - below only for template
	of.lpTemplateName = MAKEINTRESOURCE(IDD_FILE_SAVEAS);
	
	switch(m_wOpenFileType)
	{
		case FILE_TYPE_PEG:
			of.lpstrDefExt = "peg";
			break;
	/*	case FILE_TYPE_DXF:
			of.lpstrDefExt = "dxf";
			break;*/
		//case FILE_TYPE_DWG:
		//	of.lpstrDefExt = "dwg";
		//	break;
		case FILE_TYPE_JOB:
			of.lpstrDefExt = "jb1";
			break;
		case FILE_TYPE_TRACING:
			of.lpstrDefExt = "tra";
			break;
		default:
			of.lpstrDefExt = "peg";
			of.nFilterIndex = 1;
	}

	if(GetSaveFileName(&of))
	{
		CString strPathNameTo = of.lpstrFile;
		WORD wFileTypeTo = app.GetFileTypeFromPath(strPathNameTo);

		OpenFileSaveCleanWarning();

		// kill timer to delete temp file
		app.KillAutoSave();

		//if(wFileTypeTo == FILE_TYPE_DWG || wFileTypeTo == FILE_TYPE_DXF)
		//	bAddMRU = OpenAutoCADSaveAs(wFileTypeTo, strPathNameTo, true);
	/*	else */if(wFileTypeTo == FILE_TYPE_PEG)
			bAddMRU = OpenPegSaveAs(strPathNameTo, true);
		else if(FileTypeIsTracing(wFileTypeTo))	
			bAddMRU = OpenTracingSaveAs(wFileTypeTo, strPathNameTo, true);

		// mak - mru registry
		if(bAddMRU && (!strPathNameTo.IsEmpty()))
		{
			doc.SetRegistry(strPathNameTo);
			doc.DisplayMRU();

			SetFileReadOnly(false);

		//	// start autosave
			//app.StartAutoSave();
		}
	}
	delete [] of.lpstrFile;

	//GIG - Start Autosave once file has a name
	app.StartAutoSave();
	return bAddMRU;
}

bool CPegDoc::OpenAutoCADSave(CString strPathName, bool bDispMsg)
{
//	WORD wFileType = app.GetFileTypeFromPath(strPathName);
//
////	CFileOpenDWG fod;
//	CFileDWGDirect fdd;
//
////	short nError;
////	if (fod.Initialize(&nError))
////	{
////		fod.Create(strPathName.GetBuffer(), wFileType);
//		fdd.Create(strPathName.GetBuffer(), wFileType);
//
//		// mak
//		if(bDispMsg)
//			msgInformation(IDS_MSG_DWG_SAVE_SUCCESS, strPathName);
//
//		return true;
////	}

//	return false;

	CUPDialog Dlg(app.GetSafeHwnd(),FileAutoCADSaveThreadProc,(LPVOID)&strPathName,_T("Saving AutoCAD File..."),false);
	INT_PTR nResult = Dlg.DoModal();

	if(nResult == IDOK) 
	{
		// mak
		if(bDispMsg)
			msgInformation(IDS_MSG_DWG_SAVE_SUCCESS, strPathName);

		return true;
	}
	
	return false;

	///////////////////////////////////////////////////////////////////

	//WORD wFileType = app.GetFileTypeFromPath(strPathName);

	//CFileOpenDWG fod;
	//short nError;
	//if (fod.Initialize(&nError))
	//{
	//	fod.Create(strPathName.GetBuffer(), wFileType);

	//	// mak
	//	if(bDispMsg)
	//		msgInformation(IDS_MSG_DWG_SAVE_SUCCESS, strPathName);

	//	return true;
	//}

//	return false;
}

bool CPegDoc::OpenPegSave(CString strPathName, bool bDispMsg)
{
	//CFilePeg* pFile = GetFilePeg();

	//if (pFile != 0)
	//{
	//	if(SaveShadow())
	//		WriteShadowFile(true);

	//	pFile->Unload();

	//	// mak 
	//	if(bDispMsg)
	//		msgInformation(IDS_MSG_PEG_SAVE_SUCCESS, strPathName);
	//	
	//	return true;
	//}

	//return false;

	if(IsFileReadOnly())
	{
		MessageBox(app.GetSafeHwnd(), "File is read-only.  Please save as a new file.", "Warning", 0);
		return false;
	}
	
	CUPDialog Dlg(app.GetSafeHwnd(),FilePegSaveThreadProc,(LPVOID)&strPathName,_T("Saving PegAEsys File..."),false);
	INT_PTR nResult = Dlg.DoModal();

	if(nResult == IDOK) 
	{
		// mak
		if(bDispMsg)
			msgInformation(IDS_MSG_PEG_SAVE_SUCCESS, strPathName);

		return true;
	}
	
	return false;
}

bool CPegDoc::OpenTracingSave(CString strPathName, bool bDispMsg)
{
	CLayer* pLayer = LayersGet(strPathName);
		
	if (pLayer == 0)
		return false;

	if (m_wOpenFileType == FILE_TYPE_JOB)
	{
		CFileJob fj;
		if(fj.OpenForWrite(strPathName))
		{
	//		return false;

			fj.WriteHeader();
			fj.WriteSegs(pLayer);
			fj.Close();
		}
	}
	else if (m_wOpenFileType == FILE_TYPE_TRACING)
	{
		CFileTracing ft;
		if(ft.OpenForWrite(strPathName))
		{
	//		return false;

			ft.WriteHeader();
			ft.WriteSegs(pLayer);
			ft.Close();
		}
	}	

	if(bDispMsg)
		msgInformation(IDS_MSG_TRACING_SAVE_SUCCESS, strPathName);

	return true;


	//CUPDialog Dlg(app.GetSafeHwnd(),FileTracingSaveThreadProc,(LPVOID)&strPathName,_T("Saving Tracing File..."),false);
	//INT_PTR nResult = Dlg.DoModal();

	//if(nResult == IDOK) 
	//{
	//	// mak
	//	if(bDispMsg)
	//		msgInformation(IDS_MSG_TRACING_SAVE_SUCCESS, strPathName);

	//	return true;
	//}
	//
	//return false;
}

bool CPegDoc::OpenAutoCADSaveAs(WORD wFileTypeTo, CString strPathNameTo, bool bDispMsg)
{
	if(OpenAutoCADSave(strPathNameTo, bDispMsg))
	{
//		SetOpenFile(wFileTypeTo, strPathNameTo);
		return true;
	}
	
	return false;

	//CUPDialog Dlg(app.GetSafeHwnd(),FileAutoCADSaveAsThreadProc,(LPVOID)&strPathNameTo,_T("Saving AutoCAD File..."),false);
	//INT_PTR nResult = Dlg.DoModal();

	//if(nResult == IDOK) 
	//{
	//	// mak
	//	if(bDispMsg)
	//		msgInformation(IDS_MSG_DWG_SAVE_SUCCESS, strPathNameTo);

	//	return true;
	//}
	//
	//return false;
}

bool CPegDoc::OpenPegSaveAs(CString strPathNameTo, bool bDispMsg)
{
	CUPDialog Dlg(app.GetSafeHwnd(),FilePegSaveAsThreadProc,(LPVOID)&strPathNameTo,_T("Saving Peg File..."),false);
	INT_PTR nResult = Dlg.DoModal();

	if(nResult == IDOK) 
	{
		// mak
		if(bDispMsg)
			msgInformation(IDS_MSG_PEG_SAVE_SUCCESS, strPathNameTo);

		return true;
	}
	
	return false;
}

//bool CPegDoc::OpenPeg2000SaveAs(CString strPathNameTo)
//{
//	CFileException e;
//	//CFilePeg* pFilePegDst = new CFilePeg;
//	CFilePeg2000 *pFilePegDst = new CFilePeg2000;
//	pFilePegDst->Open(strPathNameTo, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite, &e);
//
//	if(e.m_cause == CFileException::sharingViolation)
//	{	
//		msgWarning(IDS_MSG_FILE_LOCKED, strPathNameTo);
//		delete pFilePegDst;
//		return false;
//	}
//	else
//	{
//		// MAK - saving .jb1 and .tra as .peg requires fusion
//		if(FileTypeIsTracing(m_wOpenFileType))
//		{
//			CLayer *player = LayersGetAt(1);
//			CString title = player->GetName();
//
//			TracingFuse(title);
//			player->SetStateHot();
//		}
//
//		CFilePeg* pFile = GetFilePeg();
//
//	//	if(pFile != 0)
//	//	{
//	//		pFile->Close();
//	//	}
//		pFilePegDst->Unload();
//	//	
//	//	SetFilePeg(pFilePegDst);
//	//	SetOpenFile(FILE_TYPE_PEG, strPathNameTo);
//
//	//	// mak
//	//	if(bDispMsg)
//			msgInformation(IDS_MSG_PEG_SAVE_SUCCESS, strPathNameTo);
//
//		return true;
//	}
//}

bool CPegDoc::OpenTracingSaveAs(WORD, CString strPathNameTo, bool bDispMsg)//wFileTypeTo
{
	WORD wFileTypeTo = app.GetFileTypeFromPath(strPathNameTo);

	if(m_wOpenFileType == FILE_TYPE_NONE)
	{
		CLayer* pLayer = LayersGetAt(0);

		if(wFileTypeTo == FILE_TYPE_JOB)
		{
			CFileJob fj;
			if(fj.OpenForWrite(strPathNameTo))
			{
				fj.WriteHeader();
				fj.WriteSegs(pLayer);
				fj.Close();

				InitAll();
				TracingOpen(strPathNameTo);
			}
			else
				return false;
		}
		else if(wFileTypeTo == FILE_TYPE_TRACING)
		{
			CFileTracing ft;
			if(ft.OpenForWrite(strPathNameTo))
			{
				ft.WriteHeader();
				ft.WriteSegs(pLayer);
				ft.Close();

				InitAll();
				TracingOpen(strPathNameTo);
			}
			else
				return false;
		}

		if(bDispMsg)
			msgInformation(IDS_MSG_TRACING_SAVE_SUCCESS, strPathNameTo);

		return true;
	}
	else if(FileTypeIsTracing(m_wOpenFileType))
	{
		CLayer* pLayer = LayersGet(m_strPathName);
		
		if(wFileTypeTo == FILE_TYPE_JOB)
		{
			CFileJob fj;
			if(fj.OpenForWrite(strPathNameTo))
			{
				fj.WriteHeader();
				fj.WriteSegs(pLayer);
				fj.Close();
				pLayer->SetName(strPathNameTo);
				SetOpenFile(wFileTypeTo, strPathNameTo);
			}
			else
				return false;
		}
		else if(wFileTypeTo == FILE_TYPE_TRACING)
		{
			CFileTracing ft;
			if(ft.OpenForWrite(strPathNameTo))
			{
				ft.WriteHeader();
				ft.WriteSegs(pLayer);
				ft.Close();
				pLayer->SetName(strPathNameTo);
				SetOpenFile(wFileTypeTo, strPathNameTo);
			}
			else
				return false;
		}	

		if(bDispMsg)
			msgInformation(IDS_MSG_TRACING_SAVE_SUCCESS, strPathNameTo);
		return true;
	}
	else
	{
		msgWarning(IDS_MSG_FILE_NOT_A_TRACING, m_strPathName);
		return false;
	}

	//CUPDialog Dlg(app.GetSafeHwnd(),FileTracingSaveAsThreadProc,(LPVOID)&strPathNameTo,_T("Saving Tracing File..."),false);
	//INT_PTR nResult = Dlg.DoModal();

	//if(nResult == IDOK) 
	//{
	//	// mak
	//	if(bDispMsg)
	//		msgInformation(IDS_MSG_TRACING_SAVE_SUCCESS, strPathNameTo);

	//	return true;
	//}
	//
	//return false;
}

//bool CPegDoc::OpenPegAutoSave(CString strPathName)
//{
//	CFileException e;
//	CFilePeg* pFilePegDst = new CFilePeg;
//	pFilePegDst->Open(strPathName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite, &e);
//
//	bool bRetVal = false;
//
//	if(e.m_cause == CFileException::sharingViolation)
//	{	
////		msgWarning(IDS_MSG_FILE_LOCKED, strPathNameTo);
//		delete pFilePegDst;
//		bRetVal = false;
//	}
//	else
//	{
//		pFilePegDst->Unload();
//		bRetVal = true;
//	}
//
//	pFilePegDst->Close();
//	return bRetVal;	
//}

int CPegDoc::PenStyleGetRefCount(PENSTYLE nPenStyle) const
{
	int iCount = 0;

	for (WORD w = 0; w < m_layers.GetSize(); w++)
	{
		CLayer* pLayer = LayersGetAt(w);
		iCount += pLayer->GetPenStyleRefCount(nPenStyle);
	}
	
	CString strKey;
	CBlock* pBlock;

	POSITION pos = m_blks.GetStartPosition();
	while (pos != NULL)
	{
		m_blks.GetNextAssoc(pos, strKey, pBlock);
		iCount += pBlock->GetPenStyleRefCount(nPenStyle);
	}
	return (iCount);
}

int CPegDoc::PenStylesFillCB(HWND hwnd) const
{
	SendMessage(hwnd, CB_RESETCONTENT, 0, 0L);

	int iSize = (int) m_PenStyles.GetSize();
	for (int i = 0; i < iSize; i++)
	{
		CPenStyle* pPenStyle = m_PenStyles.GetAt(i);
		::SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) pPenStyle->GetName());
	}
	return (iSize);
}

///<summary>
/// Gets indicated line type. If invalid returns the last line type
///</summary>
CPenStyle* CPegDoc::PenStylesGetAtSafe(int i)
{
	int iType = (int)m_PenStyles.GetSize() - 1;
	
	if(iType < 0) 
		return 0; // No line types defined

	if(i < iType) 
		iType = i;

	return (m_PenStyles.GetAt(iType));
}

PENSTYLE CPegDoc::PenStylesLookup(const CString& strName) const
{
	//try
	//{

	if (strName.CompareNoCase("ByBlock") == 0) 
		return PENSTYLE(1);
	
	for (WORD w = 0; w < m_PenStyles.GetSize(); w++)
	{
		CPenStyle* pPenStyle = m_PenStyles.GetAt(w);
		//CPenStyle* pPenStyle = PenStylesGetAtSafe(w);

		if (pPenStyle == 0)
			return (PENSTYLE_LookupErr);

		if (strName.CompareNoCase(pPenStyle->GetName()) == 0) 
		{
			return (PENSTYLE(w));
		}
	}

	//}
	//catch(...)
	//{
	//	return (PENSTYLE_LookupErr);
	//}

	return (PENSTYLE_LookupErr);
}

///<summary>
/// Removes line types which have no references. Leaves styles 0-9
///</summary>
int CPegDoc::PenStylesRemoveUnused()
{
	int count = 0;

	int i = m_PenStyles.GetSize();
	while (--i != 0)
	{
		CPenStyle* pPenStyle = m_PenStyles[i];
		PENSTYLE nPenStyle = PenStylesLookup(pPenStyle->GetName());
		if (PenStyleGetRefCount(nPenStyle) != 0 || i < 10)
		{
			m_PenStyles.SetSize(i + 1);
			break;	
		}
		count++;
		delete pPenStyle;
	}
	return count;
}

void CPegDoc::PenTranslation(WORD wCols, PENCOLOR* pColNew, PENCOLOR* pCol)
{
	for(WORD w = 0; w < m_layers.GetSize(); w++)
	{
		CLayer* pLayer = LayersGetAt(w);
		pLayer->PenTranslation(wCols, pColNew, pCol);
	}
}

void CPegDoc::PenStyleTranslation(const PENSTYLE& nStyleNew, const PENSTYLE& nStyleOld)
{
	for(WORD w = 0; w < m_layers.GetSize(); w++)
	{
		CLayer* pLayer = LayersGetAt(w);
		pLayer->PenStyleTranslation(nStyleNew, nStyleOld);
	}
}

void CPegDoc::PenColorsLoadDefault()
{
	CString s4 = app.GetAppPath() + "\\Pens\\Colors\\Default.txt";
	app.PenColorsLoad(s4);
//	app.SetBackGround(crHotCols[0]);
	app.SetBackGround(crHotCols[app.BackgroundColorGet()]);
	UpdateAllViews(NULL, 0L, NULL);
}

void CPegDoc::PenStylesClear()
{
	m_PenStyles.RemoveAll();
}

int CPegDoc::RemoveEmptyNotes()
{
	int iCount = 0;
	
	for (WORD w = 0; w < m_layers.GetSize(); w++)
	{
		CLayer* pLayer = LayersGetAt(w);
		iCount += pLayer->RemoveEmptyNotes();
	}

//	#pragma tasMSG(TODO remove empty notes from blocks)

	//CString strKey;
	//CBlock* pBlock;

	//POSITION pos = m_blks.GetStartPosition();
	//while (pos != NULL)
	//{
	//	m_blks.GetNextAssoc(pos, strKey, pBlock);
	//}

	return (iCount);
}

int CPegDoc::RemoveEmptySegs()
{
	int iCount = 0;
	
	for (WORD w = 0; w < m_layers.GetSize(); w++)
	{
		CLayer* pLayer = LayersGetAt(w);
		iCount += pLayer->RemoveEmptySegs();
	}
	
//	#pragma tasMSG(TODO remove empty segs from blocks)

	//CString strKey;
	//CBlock* pBlock;

	//POSITION pos = m_blks.GetStartPosition();
	//while (pos != NULL)
	//{
	//	m_blks.GetNextAssoc(pos, strKey, pBlock);
	//}
	
	return iCount;
}

void CPegDoc::SetFilePeg(CFilePeg* pFile)
{
	if(m_pFilePeg != 0)
		delete m_pFilePeg;
	
	m_pFilePeg = pFile;
}

void CPegDoc::SetOpenFile(WORD wFileType, const CString& strFileName)
{
	int n;
	for(n = strFileName.GetLength() - 1; n > 0; n--)
	{
		if(strFileName[n] == '\\')
		{
			n++;
			break;
		}
	}
	
	CString strItem = "&File:" + strFileName.Mid(n);

	app.ModifyMenu(0, strItem);

	if(wFileType == FILE_TYPE_NONE)
		WorkLayerSet(LayersGetAt(0));
	
	m_wOpenFileType = wFileType;
	m_strPathName = strFileName;
	::GetFileTitle(strFileName, m_strTitle.GetBuffer(MAX_PATH), MAX_PATH);
	m_strTitle.ReleaseBuffer();
}

void CPegDoc::TracingCloak(const CString& strName)
{
	CLayer* pLayer = LayersGet(strName);

	pLayer->SetStateOff();
	pLayer->SetTracingFlg(CLayer::TRACING_FLG_CLOAKED);

	CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);
}

///<summary>Tracing is converted to a static layer.</summary>
///<remarks>
///This is a cold state meaning the layer is displayed using warm color set,
///is not detectable, and may not have its segments modified or deleted.
/// No new segments may be added to the layer.
///</remarks>
void CPegDoc::TracingFuse(CString& strName)
{	
	CLayer* pLayer = LayersGet(strName);
	if (pLayer != 0)
	{
		char* pTitle = new char[MAX_PATH];
		GetFileTitle(strName, pTitle, MAX_PATH);
		strtok(pTitle, ".");
		strName = pTitle;
		delete [] pTitle;	

		pLayer->ClrTracingFlg();
		pLayer->ClrStateFlg();
		pLayer->SetStateFlg(CLayer::STATE_FLG_RESIDENT | CLayer::STATE_FLG_INTERNAL);
		pLayer->SetStateCold();
	
		pLayer->SetName(strName);
	}
}

void CPegDoc::TracingFuseAll(CString& strLayers)
{
	// mak - Fuse all tracing layers
	// the string created for reference is in the form of:
	//		layername1;m;layername2;v;layername3;c;...
	//		m - mapped; v - viewed; c - cloaked
	//		delimited by ";"
	/////////////////////////////////////////////////////////////////////////////////////	
	for(int i = 0; i < LayersGetSize(); i++)
	{
		CLayer *pLayer = LayersGetAt(i);
		if(!pLayer->IsInternal())
		{
			CString strLayerName2 = pLayer->GetName();
			strLayers.Append(strLayerName2);
			strLayers.Append(";");

			bool bIsCloaked = false;

			if(pLayer->IsMapped())
				strLayers.Append("m;");
			else if(pLayer->IsViewed())
				strLayers.Append("v;");
			else if(pLayer->IsCloaked())
			{
				strLayers.Append("c;");
				bIsCloaked = true;
			}

			TracingFuse(strLayerName2);

			if(bIsCloaked)
				pLayer->SetStateOff();
		}
	}
}

///<summary>
///
///</summary>
bool CPegDoc::TracingLoadLayer(const CString& strPathName, CLayer* pLayer)
{
	WORD wFileType = app.GetFileTypeFromPath(strPathName);
	if (!FileTypeIsTracing(wFileType))
		return false;

	if (pLayer == 0)
		return false;
		
	bool bFileOpen = false;
	
	if (wFileType == FILE_TYPE_TRACING)
	{
		CFileTracing ft;
		bFileOpen = ft.OpenForRead(strPathName);
		if (bFileOpen)
		{
			try
			{
				ft.ReadHeader();
				ft.ReadSegs(pLayer);
				ft.Close();
			}
			catch(char* szMessage)
			{
				msgInformation(szMessage);
			}
		}
	}
	else
	{
		CFileJob fj;
		bFileOpen = fj.OpenForRead(strPathName);
		if (bFileOpen)
		{
			try
			{
				fj.ReadHeader();
				fj.ReadSegs(pLayer);
				fj.Close();
			}
			catch(char* szMessage)
			{
				msgInformation(szMessage);
			}
		}
	}	 
	
	return (bFileOpen);
}

bool CPegDoc::BorderMap(const CString& strPathName)
{
	WORD wFileType = app.GetFileTypeFromPath(strPathName);
	if(!FileTypeIsTracing(wFileType))
		return false;
		
//	bool bFileOpen = false;
	
//	CLayer* pLayer = LayersGet(strPathName);
	
//	if(pLayer != 0)
//	{
//		bFileOpen = true;
//		MessageBox(0, "Already included", 0, 0);
//	}
//	else
//	{
		CLayer *pLayer = new CLayer(strPathName, CLayer::STATE_FLG_COLD);
		
//		bFileOpen = TracingLoadLayer(strPathName, pLayer);
		TracingLoadLayer(strPathName, pLayer);
	
//		if(bFileOpen)
		BorderLayersAdd(pLayer);
//		else
//			delete pLayer;
//	}

//	if (bFileOpen)		
//	{
//		// might not be correct
////		pLayer->SetStateWarm();
//		pLayer->SetStateCold();
//		///////////////////////
//		pLayer->SetTracingFlg(CLayer::TRACING_FLG_MAPPED);
//		UpdateAllViews(NULL, HINT_LAYER_SAFE, pLayer);
//	}
//
//	return (bFileOpen);

		return true;
}

///<summary>
///
///</summary>
bool CPegDoc::TracingMap(const CString& strPathName)
{
	// for bitmap layer
	if(strPathName != "Bitmap")
	{
		WORD wFileType = app.GetFileTypeFromPath(strPathName);
		if(!FileTypeIsTracing(wFileType))
			return false;
	}

	bool bFileOpen = false;
	
	CLayer* pLayer = LayersGet(strPathName);
	
	if(pLayer != 0)
	{
		bFileOpen = true;
	}
	else
	{
		pLayer = new CLayer(strPathName, CLayer::STATE_FLG_COLD);
		
		bFileOpen = TracingLoadLayer(strPathName, pLayer);
		
		if(bFileOpen)
			LayersAdd(pLayer);
		else
			delete pLayer;
	}

	if (bFileOpen)		
	{
		// might not be correct
//		pLayer->SetStateWarm();
		pLayer->SetStateCold();
		///////////////////////
		pLayer->SetTracingFlg(CLayer::TRACING_FLG_MAPPED);
		UpdateAllViews(NULL, HINT_LAYER_SAFE, pLayer);

		CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);
	}

	return (bFileOpen);
}

///<summary>
///
///</summary>
bool CPegDoc::TracingOpen(const CString& strFileName)
{	
	WORD wFileType = app.GetFileTypeFromPath(strFileName);

	// mak - wait cursor
	HCURSOR hOldCursor = ::SetCursor(LoadCursor(0, IDC_WAIT));
	
	CLayer* pLayer = new CLayer(strFileName, CLayer::STATE_FLG_HOT | CLayer::STATE_FLG_WARM);
//	pLayer->SetStateFlg(CLayer::STATE_FLG_INTERNAL);
	LayersAdd(pLayer);
	TracingLoadLayer(strFileName, pLayer);
	detsegs.AddTail(pLayer);

	if(wFileType == FILE_TYPE_TRACING)
		SetOpenFile(FILE_TYPE_TRACING, strFileName);
	else if(wFileType == FILE_TYPE_JOB)
		SetOpenFile(FILE_TYPE_JOB, strFileName);

	WorkLayerSet(pLayer);

	// mak - old cursor
	::SetCursor(hOldCursor);

	::EnableMenuItem(app.GetMenu(), IDM_FILE_MANAGE, MF_BYCOMMAND | MF_GRAYED); 

	app.StatusLineDisplay(WorkCnt);
	UpdateAllViews(NULL, 0L, NULL);
	return true;

	//CUPDialog Dlg(app.GetSafeHwnd(),TracingOpenThreadProc,(LPVOID)&strFileName,_T("Loading Tracing File..."),false);
	//Dlg.DoModal();

	//::EnableMenuItem(app.GetMenu(), IDM_FILE_MANAGE, MF_BYCOMMAND | MF_GRAYED); 

	//UpdateAllViews(NULL, 0L, NULL);
	//return true;
}

///<summary>
///
///</summary>
bool CPegDoc::TracingView(const CString& strPathName)
{
	// for bitmap layer
	if(strPathName != "Bitmap")
	{
		WORD wFileType = app.GetFileTypeFromPath(strPathName);
		if (!FileTypeIsTracing(wFileType))
			return false;
	}
		
	bool bFileOpen = false;
	
	CLayer* pLayer = LayersGet(strPathName);
	
	if (pLayer != 0)
	{
		bFileOpen = true;
	}
	else
	{
		pLayer = new CLayer(strPathName, CLayer::STATE_FLG_COLD);
		
		bFileOpen = TracingLoadLayer(strPathName, pLayer);
		
		if (bFileOpen)
			LayersAdd(pLayer);
		else
			delete pLayer;
	}
	if (bFileOpen)		
	{
		// might not be correct
		pLayer->SetStateCold();
		////////////////////////

		pLayer->SetTracingFlg(CLayer::TRACING_FLG_VIEWED);
		UpdateAllViews(NULL, HINT_LAYER_SAFE, pLayer);
		
		CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);
	}
	return (bFileOpen);
}

void CPegDoc::BitmapLayerLoad()
{
	if(LayersLookup("Bitmap2") == -1)
	{
		CLayer *pLayer = new CLayer("Bitmap2");
		
		pLayer->ClrStateFlg();
		pLayer->SetStateFlg(CLayer::STATE_FLG_RESIDENT);
		pLayer->SetStateCold();
		pLayer->SetTracingFlg(CLayer::TRACING_FLG_MAPPED);

		LayersAdd(pLayer);

		//this->WorkLayerSet(pLayer);
	}
}

void CPegDoc::BitmapLayerLoadFile(CPnt pntCursor, CString strPathName)
{
//	CString szPathName = "C:\\testing___.bmp";
	FILE *pFile = fopen(strPathName, "rb");
    if(pFile == NULL)
    {
//		MessageBox(0, "file not opened", 0, 0);
//        AfxMessageBox(IDS_FILE_CREATE_ERROR_MESSAGE);
        return;
    }

	//if(LayersLookup("Bitmap") == -1)
	//{
	//	CLayer *pLayer = new CLayer("Bitmap");
	//	
	//
	//	pLayer->ClrStateFlg();
	//	pLayer->SetStateFlg(CLayer::STATE_FLG_RESIDENT);
	//	pLayer->SetStateCold();
	//	pLayer->SetTracingFlg(CLayer::TRACING_FLG_MAPPED);

	//	LayersAdd(pLayer);
	//}

	BitmapLayerLoad();

	CLayer *pBitmapLayer = LayersGet("Bitmap");

	if(pBitmapLayer == 0)
		return;

	///////////////////////////////////////////////////////////////////////////

	BITMAPFILEHEADER bitmapFileHeader; //our bitmap file header
	unsigned char *bitmapImage; //store image data

	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,pFile);

	//verify that this is a bmp file by check bitmap id
	if (bitmapFileHeader.bfType !=0x4D42)
	{
		fclose(pFile);
		return;
	}

	BITMAPINFOHEADER bitmapInfoHeader;

	//read the bitmap info header
	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER),1,pFile);

	//move file point to the begging of bitmap data
	fseek(pFile, bitmapFileHeader.bfOffBits, SEEK_SET);

	//allocate enough memory for the bitmap image data
	bitmapImage = (unsigned char*)malloc(bitmapInfoHeader.biSizeImage);

	//verify memory allocation
	if (!bitmapImage)
	{
//		free(bitmapImage);
		fclose(pFile);
		return;
	}

	//read in the bitmap image data
	fread(bitmapImage,bitmapInfoHeader.biSizeImage,1,pFile);

	//make sure bitmap image data was read
	if (bitmapImage == NULL)
	{
		fclose(pFile);
		return;
	}

	BITMAPINFO bmi;
	bmi.bmiHeader = bitmapInfoHeader;

	CDC *pDC = CPegView::GetActiveView()->GetDC();
	HBITMAP hBitmap = CreateDIBitmap(pDC->GetSafeHdc(), &bitmapInfoHeader, CBM_INIT, bitmapImage, &bmi, DIB_RGB_COLORS);

	if(hBitmap != 0)
	{
		
		CPrimBitmap *pPrim = new CPrimBitmap(pntCursor, hBitmap);
		CSeg *pSeg = new CSeg(pPrim);
		//pBitmapLayer->AddTail(new CSeg(pPrim));
		//WorkLayerGet()->AddTail(new CSeg(pPrim));
		//WorkLayerGet()->AddTail(pSeg);

		//// make the prims selectable
		//detsegs.AddTail(pSeg);
		//app.StatusLineDisplay(WorkCnt);

		BitmapLayerAddTail(pSeg);
	}

	fclose(pFile);
}

void CPegDoc::BitmapLayerDisplay(CDC* pDC)
{
	bool bIdentifyTrap = trapsegs.Identify() && !trapsegs.IsEmpty();

	// display working layer
	CLayer *pBitmapLayer = this->LayersGet("Bitmap");

	
//	if(pLayer == 0)
	if(pBitmapLayer == 0)
		return;

//	pLayer->Display(pDC, bIdentifyTrap);
	pBitmapLayer->Display(pDC, bIdentifyTrap);
}

void CPegDoc::BitmapLayerSave()
{
//	CString szPathName = "C:\\testing___.bmp";
	CString szPathName = GetPathName() + "bitmap.bmp";
	FILE *pFile = fopen(szPathName, "wb");
    if(pFile == NULL)
    {
//		MessageBox(0, "file not created", 0, 0);
//        AfxMessageBox(IDS_FILE_CREATE_ERROR_MESSAGE);
        return;
    }

	CLayer *pBitmapLayer = LayersGet("Bitmap");

	if(pBitmapLayer == 0)
		return;

	POSITION pos = pBitmapLayer->GetHeadPosition();
	while(pos != 0)
	{
		CSeg *pSeg = pBitmapLayer->GetNext(pos);

		POSITION pos2 = pSeg->GetHeadPosition();
		while(pos2 != 0)
		{
			CPrim *pPrim = pSeg->GetNext(pos2);
			if(pPrim->Is(CPrim::PRIM_BITMAP))
			{
				CPrimBitmap *pPrimBitmap = (CPrimBitmap*)pPrim;
//				pPrimBitmap->SaveBitmapFile(pFile);
			}
		}
	}

    fclose(pFile);
}

///<summary>
///
///</summary>
void CPegDoc::WorkLayerAddTail(CSeg* pSeg)
{
	m_pLayerWork->AddTail(pSeg);
	detsegs.AddTail(pSeg);
	app.StatusLineDisplay(WorkCnt);
	SetModifiedFlag();	

	// UNDO
	//undo.SaveState();
}

void CPegDoc::OnBitmapEdit()
{
	//MessageBox(0, 0, 0, 0);
	//trapsegs.RemoveAll();

	// enable bitmap edit mode

	if(app.GetPegState() == STATE_NORMAL)
	{
		BitmapEditEnable();
		app.SetPegState(STATE_BITMAP);
	}
	else if(app.GetPegState() == STATE_BITMAP)
	{
		BitmapEditDisable();
		app.SetPegState(STATE_NORMAL);
	}
}

void CPegDoc::BitmapEditEnable()
{
	m_strLayerState = LayersGetState();

	//MessageBox(0, m_strLayerState, 0, 0);

	trapsegs.RemoveAll();

	CLayer *pBitmapLayer = LayersGet("Bitmap2");

	if(pBitmapLayer == 0)
		return;

	WorkLayerSet(pBitmapLayer);
	OnLayersStaticAll();

	app.BackgroundColorSet(255);

	ShowWindow(bitmapEdit.GetHWND(), SW_SHOW);
	CPegView::GetActiveView()->InvalidateRect(0, TRUE);
}

void CPegDoc::BitmapEditDisable()
{
	trapsegs.RemoveAll();

	//CLayer *pZeroLayer = LayersGet("0");
	//if(pZeroLayer == 0)
	//	return;
	//WorkLayerSet(pZeroLayer);

	CLayer *pBitmapLayer = LayersGet("Bitmap2");
	if(pBitmapLayer == 0)
		return;
	pBitmapLayer->SetStateCold();

	ShowWindow(bitmapEdit.GetHWND(), SW_HIDE);

	LayersSetState(m_strLayerState);

	app.BackgroundColorSet(0);

	CPegView::GetActiveView()->InvalidateRect(0, TRUE);
}

void CPegDoc::BitmapLayerAddTail(CSeg* pSeg)
{
	CLayer *pBitmapLayer = LayersGet("Bitmap2");

	if(pBitmapLayer == 0)
		return;

	//CLayer *pLayerWork = WorkLayerGet();
	//WorkLayerSet(pBitmapLayer);
	//WorkLayerAddTail(pSeg);
	//WorkLayerSet(pLayerWork);

	pBitmapLayer->AddTail(pSeg);
	detsegs.AddTail(pSeg);
	app.StatusLineDisplay(WorkCnt);

	//trapsegs.AddTail(pSeg);
}

///<summary>
///
///</summary>
void CPegDoc::WorkLayerAddTail(CSegs* pSegs)
{
	m_pLayerWork->AddTail(pSegs);
	detsegs.AddTail(pSegs);
	app.StatusLineDisplay(WorkCnt);
	SetModifiedFlag();	
}

///<summary>
///
///</summary>
void CPegDoc::WorkLayerClear()
{
//	AppSetGinCur();
	WorkLayerInit();
}

///<summary>
///
///</summary>
CSeg* CPegDoc::WorkLayerGetTail() const
{
	POSITION pos = m_pLayerWork->GetTailPosition();
	return ((CSeg*) (pos != 0 ? m_pLayerWork->GetPrev(pos) : 0));
}

///<summary>
///
///</summary>
void CPegDoc::WorkLayerInit()
{
	UpdateAllViews(NULL, HINT_LAYER_ERASE, m_pLayerWork);
	m_pLayerWork->RemoveSegs();

	trapsegs.RemoveAll();
	detsegs.RemoveAll();	
	detsegs.DetSeg() = 0;
	m_segsDeleted.RemoveSegs();
}

///<summary>
///
///</summary>
void CPegDoc::WorkLayerSet(CLayer* pLayer)
{
	// HACK: m_pLayerWork should be created already. OnNew something, somewhere.
	if(m_pLayerWork == 0)
		m_pLayerWork = pLayer;

	m_pLayerWork->SetStateWarm();
	m_pLayerWork = pLayer;
	m_pLayerWork->SetStateHot();
	
	CString strItem;
	GetMenuString(app.GetMenu(), 0, strItem.GetBuffer(128), 128, MF_BYPOSITION); 
	strItem.ReleaseBuffer();

	int n = strItem.Find('/');
	if(n == - 1)
		n = strItem.GetLength();
	
	strItem.Truncate(n);
	strItem += "/" + m_pLayerWork->GetName();

	WORD wFileType = app.GetFileTypeFromPath(GetPathName());
	if(!FileTypeIsTracing(wFileType))
		app.ModifyMenu(0, strItem);
}

///<summary>
///
///</summary>
void CPegDoc::WriteShadowFile(bool bDispMsg)
{
	if(m_wOpenFileType == FILE_TYPE_PEG)
	{
		CString strShadowFilePath = app.GetShadowDir();
		strShadowFilePath += GetTitle();

		int nExt = strShadowFilePath.ReverseFind('.');
		if(nExt > 0)
		{
			CFileStatus fs;
			CFile::GetStatus(m_strPathName, fs);

			strShadowFilePath.Truncate(nExt);
			strShadowFilePath += fs.m_mtime.Format("_%Y%m%d%H%M");
			strShadowFilePath += ".peg";
			
			// mak - Fuse all tracing layers
			CString strLayers;
			TracingFuseAll(strLayers);

			CFilePeg fp;
			if(fp.Open(strShadowFilePath, GENERIC_WRITE, false))
			{	
				fp.Unload();
				fp.Close();

				if(bDispMsg)
					msgWarning(IDS_MSG_FILE_SHADOWED_AS, strShadowFilePath);
			}
			else
			{
				if(bDispMsg)
					msgWarning(IDS_MSG_SHADOW_FILE_CREATE_FAILURE, app.GetSafeHwnd());
			}

			// mak - Melt all of the layers
			LayerMeltAllWithoutSaving(strLayers);

			////////////////////////////////////////////////////////////////////////////////

			//CFileException e;
			//CFilePeg fp;
			//if(!fp.Open(strShadowFilePath, CFile::modeWrite, &e))
			//{	
			//	fp.Open(strShadowFilePath, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive, &e);
			//	fp.Unload();

			//	if(bDispMsg)
			//		msgWarning(IDS_MSG_FILE_SHADOWED_AS, strShadowFilePath);
			//}
			//else
			//{
			//	if(bDispMsg)
			//		msgWarning(IDS_MSG_SHADOW_FILE_CREATE_FAILURE, app.GetSafeHwnd());
			//}

			//// mak - Melt all of the layers
			//LayerMeltAll(strLayers);
		}
	}
}

void CPegDoc::WriteAutosaveFile()
{
	if(m_wOpenFileType == FILE_TYPE_PEG)
	{
		CString strShadowFilePath = app.GetShadowDir();
		strShadowFilePath += GetTitle();

		int nExt = strShadowFilePath.ReverseFind('.');
		if(nExt > 0)
		{
			CFileStatus fs;
			CFile::GetStatus(m_strPathName, fs);

			strShadowFilePath.Truncate(nExt);
			strShadowFilePath += "-Autosave";
			strShadowFilePath += ".peg";
			int nIndex = strShadowFilePath.ReverseFind('\\');
			strShadowFilePath.Insert(nIndex + 1, "_");

			// mak - Fuse all tracing layers
			CString strLayers;
			TracingFuseAll(strLayers);

			CFilePeg fp;
			if(fp.Open(strShadowFilePath, GENERIC_WRITE, false))
			{	
				fp.Unload();
				fp.Close();
			}

			// mak - Melt all of the layers
			LayerMeltAllWithoutSaving(strLayers);
		}
	}
}


///<summary>
///
///</summary>
void CPegDoc::OnBlocksLoad()
{
	//char szFilter[25];
	//SecureZeroMemory(szFilter, 25);
	//::LoadString(app.GetInstance(), IDS_OPENFILE_FILTER_BLOCKS, szFilter, sizeof(szFilter));

	OPENFILENAME of;
	::ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = 0;
	of.hInstance = app.GetInstance();
	//of.lpstrFilter = szFilter;
	of.lpstrFilter = "Block Files (.blk)\0*.blk\0";
	of.lpstrFile = new char[MAX_PATH];
	of.lpstrFile[0] = 0;
	of.nMaxFile = MAX_PATH;
	of.lpstrTitle = "Load Blocks";
	of.Flags = OFN_HIDEREADONLY;
	of.lpstrDefExt = "blk";
	
	if (GetOpenFileName(&of))
	{
		if ((of.Flags & OFN_EXTENSIONDIFFERENT) == 0)
		{
			CFileBlock fb;
			fb.ReadFile(of.lpstrFile, m_blks);
		}
		else
			msgWarning(IDS_MSG_FILE_TYPE_ERROR);
	}
	delete [] of.lpstrFile;
}

///<summary>
///
///</summary>
void CPegDoc::OnBlocksUnload()
{
	//char szFilter[25];
	//SecureZeroMemory(szFilter, 25);
	//::LoadString(app.GetInstance(), IDS_OPENFILE_FILTER_BLOCKS, szFilter, sizeof(szFilter));

	OPENFILENAME of;
	::ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = 0;
	of.hInstance = app.GetInstance();
	//of.lpstrFilter = szFilter;
	of.lpstrFilter = "Block Files (.blk)\0*.blk\0";
	of.lpstrFile = new char[MAX_PATH];
	of.lpstrFile[0] = 0;
	of.nMaxFile = MAX_PATH;
	of.lpstrTitle = "Unload Blocks As";
	of.Flags = OFN_OVERWRITEPROMPT;
	of.lpstrDefExt = "blk";
	
	if (GetSaveFileName(&of))
	{
		if ((of.Flags & OFN_EXTENSIONDIFFERENT) == 0)
		{
			CFileBlock fb;
			fb.WriteFile(of.lpstrFile, m_blks);
		}
	}
	delete [] of.lpstrFile;
}

///<summary>
///
///</summary>
void CPegDoc::OnClearActiveLayers()
{
	AppSetGinCur();
	for (int i = LayersGetSize() - 1; i > 0; i--)
	{
		CLayer* pLayer = LayersGetAt(i);

		if (pLayer->IsWarm())
		{
			UpdateAllViews(NULL, HINT_LAYER_ERASE, pLayer);
			pLayer->RemoveSegs();
		}
	}
}

///<summary>
///
///</summary>
void CPegDoc::OnClearAllLayers()
{
	AppSetGinCur();
	
	for (int i = LayersGetSize() - 1; i > 0; i--)
	{
		CLayer* pLayer = LayersGetAt(i);

		if (pLayer->IsInternal())
		{
			UpdateAllViews(NULL, HINT_LAYER_ERASE, pLayer);
			pLayer->RemoveSegs();
		}
	}
	UpdateAllViews(NULL, 0L, NULL);
}

///<summary>
///
///</summary>
void CPegDoc::OnClearAllTracings()
{
	AppSetGinCur();
	
	for (int i = LayersGetSize() - 1; i > 0; i--)
	{
		CLayer* pLayer = LayersGetAt(i);

		if (!pLayer->IsInternal())
		{
			UpdateAllViews(NULL, HINT_LAYER_ERASE, pLayer);
			pLayer->RemoveSegs();
		}
	}
	UpdateAllViews(NULL, 0L, NULL);
}

///<summary>
///
///</summary>
void CPegDoc::OnClearMappedTracings()
{
	AppSetGinCur();
	for (int i = LayersGetSize() - 1; i > 0; i--)
	{
		CLayer* pLayer = LayersGetAt(i);

		if (pLayer->IsMapped())
		{
			UpdateAllViews(NULL, HINT_LAYER_ERASE, pLayer);
			
			if (pLayer->IsResident())
			{
				pLayer->ClrTracingFlg(CLayer::TRACING_FLG_MAPPED);
				pLayer->SetStateOff();
			}
			else
				LayersRemoveAt(i);
		}
	}
}

///<summary>
///
///</summary>
void CPegDoc::OnClearViewedTracings()
{
	AppSetGinCur();
	for (int i = LayersGetSize() - 1; i > 0; i--)
	{
		CLayer* pLayer = LayersGetAt(i);

		if (pLayer->IsViewed())
		{
			UpdateAllViews(NULL, HINT_LAYER_ERASE, pLayer);
			
			if (pLayer->IsResident())
			{
				pLayer->ClrTracingFlg(CLayer::TRACING_FLG_VIEWED);
				pLayer->SetStateOff();
			}
			else
				LayersRemoveAt(i);
		}
	}
}

///<summary>
///
///</summary>
void CPegDoc::OnClearWorkingLayer() 
{
	WorkLayerClear();
}

///<summary>
///The current view is copied to the clipboard as an enhanced metafile.
///</summary>
void CPegDoc::OnEditImageToClipboard()
{
	HDC hdcEMF = ::CreateEnhMetaFile(0, 0, 0, 0);
	LayersDisplayAll(CDC::FromHandle(hdcEMF));
	HENHMETAFILE hemf = ::CloseEnhMetaFile(hdcEMF);
	
	::OpenClipboard(NULL);
	::EmptyClipboard();
	::SetClipboardData(CF_ENHMETAFILE, hemf);
	::CloseClipboard();
}

///<summary>
///
///</summary>
void CPegDoc::OnEditSegToWork()
{
	CPnt pt = app.CursorPosGet();
	
	CLayer* pLayer = LayersSelUsingPoint(pt);

	if (pLayer != 0)
	{
		if (pLayer->IsInternal() && pLayer->IsOn())
		{
			CSeg* pSeg = pLayer->SelSegUsingPoint(pt);
	
			if (pSeg != 0)	
			{
				pLayer->Remove(pSeg);
				UpdateAllViews(NULL, HINT_SEG_ERASE_SAFE, pSeg);
				WorkLayerAddTail(pSeg); 
				UpdateAllViews(NULL, HINT_SEG_SAFE, pSeg);
			}
		}
	}
}

///<summary>
/// Pastes clipboard to drawing. Only pegsegs format handled and no translation 
/// is performed.
///</summary>
void CPegDoc::OnEditTrace()
{
	if (::OpenClipboard(NULL))
	{
		char sBuf[16];

		UINT nClipboardFormat;
		UINT nFormat = 0;
		
		while ((nClipboardFormat = EnumClipboardFormats(nFormat)) != 0)
		{
			GetClipboardFormatName(nClipboardFormat, sBuf, 16);
			
			if (strcmp(sBuf, "PegSegs") == 0)
			{
				HGLOBAL hGbl = GetClipboardData(nClipboardFormat);
				if (hGbl != 0)
				{
					CMemFile mf;
					CVec vTrns;

					LPCSTR lpBuffer = (LPCSTR) GlobalLock(hGbl);
					DWORD dwSizeOfBuffer = *((DWORD*) lpBuffer);
					mf.Write(lpBuffer, UINT(dwSizeOfBuffer));
					GlobalUnlock(hGbl);
					
					mf.Seek(96, CFile::begin);
					mfGetAll(mf, vTrns);

					break;
				}		
			}
			nFormat = nClipboardFormat;
		}
		CloseClipboard();
	}
	else
		msgWarning(IDS_MSG_CLIPBOARD_LOCKED);	
}

///<summary>
///
///</summary>
void CPegDoc::OnEditTrapCopy()
{
	DoEditTrapCopy();
}

///<summary>
///
///</summary>
void CPegDoc::OnEditTrapCut()
{
	DoEditTrapCopy();
	trapsegs.DeleteSegs();
	UpdateAllViews(NULL, 0L, NULL);
}

///<summary>
///
///</summary>
void CPegDoc::OnEditTrapDelete()
{
	if(msgConfirm(IDS_MSG_CONFIRM_DELETE) == IDYES)
	{
	//#pragma tasMSG(logic for OnEditTrapDelete poor)
		trapsegs.DeleteSegs();
		UpdateAllViews(NULL, 0L, NULL);
		OnEditTrapQuit();
	}
}

///<summary>
/// Pastes clipboard to drawing. If the clipboard has the PegSegs format, 
/// all other formats are ignored.
///</summary>
void CPegDoc::OnEditTrapPaste()
{
	if (::OpenClipboard(NULL))
	{
		UINT nClipboardFormat = app.GetClipboardFormatPegSegs();

		if (IsClipboardFormatAvailable(nClipboardFormat))
		{
			if (::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_PASTE_POSITION), app.GetSafeHwnd(), DlgProcSetPasteLoc) > 0)
			{
				HGLOBAL hGbl = GetClipboardData(nClipboardFormat);
				if (hGbl != 0)
				{
					CPnt ptMin;

					CPnt ptPvt(app.CursorPosGet());
					trapsegs.PvtPt() = ptPvt;

					LPCSTR lpBuffer = (LPCSTR) GlobalLock(hGbl);

					DWORD dwSizeOfBuffer = *((DWORD*) lpBuffer);

					CMemFile mf;
					mf.Write(lpBuffer, UINT(dwSizeOfBuffer));
						
					mf.Seek(sizeof(DWORD), CFile::begin);
					ptMin.Read(mf);

					CVec vTrns(ptMin,  ptPvt);

					GlobalUnlock(hGbl);

					mf.Seek(96, CFile::begin);
					mfGetAll(mf, vTrns);
				}		
			}
		}
		else if (IsClipboardFormatAvailable(CF_TEXT))
		{
			HGLOBAL hGbl = GetClipboardData(CF_TEXT);
			LPSTR lpText = new char[GlobalSize(hGbl)];

			LPCSTR lpBuffer = (LPCSTR) GlobalLock(hGbl);
			lstrcpy(lpText, lpBuffer);
			GlobalUnlock(hGbl);

			AddTextBlock(lpText);

			delete [] lpText;
		}
		else if(IsClipboardFormatAvailable(CF_BITMAP))
		{
			//Get the clipboard data
			HBITMAP handle = (HBITMAP)GetClipboardData(CF_BITMAP);

			CPnt ptCurPos(app.CursorPosGet());
			CSeg *pSeg = new CSeg(new CPrimBitmap(ptCurPos, handle));
			if(pSeg != 0)
			{
				BitmapLayerLoad();
				BitmapLayerAddTail(pSeg);
				//WorkLayerAddTail(pSeg);
				CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);
			}
		}
		CloseClipboard();
	}
	else
		msgWarning(IDS_MSG_CLIPBOARD_LOCKED);
}

///<summary>
///
///</summary>
void CPegDoc::OnEditTrapQuit()
{
	UpdateAllViews(NULL, HINT_SEGS_SAFE, &trapsegs);
	trapsegs.RemoveAll();
	app.StatusLineDisplay(TrapCnt);

	CPegView *pView = CPegView::GetActiveView();
	pView->InvalidateRect(NULL, TRUE);
}

///<summary>
/// Adds all segments in the work layer to the trap.
///</summary>
void CPegDoc::OnEditTrapWork()
{	
	if(trapsegs.Identify())
		UpdateAllViews(NULL, HINT_SEGS_SAFE, &trapsegs);
	
	trapsegs.RemoveAll();
	trapsegs.AddTail(WorkLayerGet());
	UpdateAllViews(NULL, HINT_SEGS_SAFE_TRAP, &trapsegs);
	app.StatusLineDisplay(TrapCnt);
}

///<summary>
/// Add all segments in all work and active layers to the trap.
///</summary>
void CPegDoc::OnEditTrapWorkAndActive()
{	
	trapsegs.RemoveAll();
	
	for(int i = 0; i < LayersGetSize(); i++)
	{
		CLayer* pLayer = LayersGetAt(i);
		if(pLayer->IsHot() || pLayer->IsWarm())
			trapsegs.AddTail(pLayer);
	}
	UpdateAllViews(NULL, HINT_SEGS_SAFE_TRAP, &trapsegs);
	app.StatusLineDisplay(TrapCnt);
}

///<summary>
///
///</summary>
void CPegDoc::OnFile(HWND hwnd)
{
	CPoint pnt(0, 0);
	::ClientToScreen(hwnd, &pnt);
	HMENU hFilePopupMenu = app.GetSubMenu(0);
	::TrackPopupMenu(hFilePopupMenu, TPM_LEFTALIGN, pnt.x, pnt.y, 0, hwnd, 0);
}

///<summary>
///
///</summary>
int CPegDoc::OnFileClose()
{
//	if(!IsFileReadOnly())
//	{
//		// if a file type is open, prompt user about saving
//		int iRetVal = IDNO;
//		if((m_wOpenFileType != FILE_TYPE_NONE) || (GetHotCount() > 0))
//			iRetVal = IDYES;
//
//		if(iRetVal == IDYES)
//			iRetVal = msgConfirm(IDS_MSG_SAVE_CHANGES, GetPathName());
//
//		if(iRetVal != IDCANCEL)
//		{
//			OpenFileClose(iRetVal == IDYES);
////			SetFileReadOnly(false);
//		}
//
//		return iRetVal;
//	}
//	else
//	{
//		OpenFileClose(false);
//	//	SetFileReadOnly(false);
//		return IDNO;
//	}

	int iRetVal = DisplaySaveMessageConfirmation(IDS_MSG_SAVE_CHANGES);

	if(iRetVal != IDCANCEL)
		OpenFileClose(iRetVal == IDYES);

	return iRetVal;
}

void CPegDoc::OnFileRecover()
{
	OpenFileRecover();
}

void CPegDoc::OnActiveAll()
{
	OnLayersActiveAll();
}

void CPegDoc::OnStaticAll()
{
	OnLayersStaticAll();
}

///<summary>
/// file->manage handler
///</summary>
void CPegDoc::OnFileManage()
{
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_FILE_MANAGE), app.GetSafeHwnd(), DlgProcFileManage);
}

///<summary>
/// file->maintenance handler
///</summary>
void CPegDoc::OnFileClean()
{
	HWND hwnd = app.GetSafeHwnd();
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_FILE_CLEAN), hwnd, DlgProcFileClean);
}

///<summary>
/// file->plot->full handler
///</summary>
void CPegDoc::OnFilePlot()
{
//	print::FilePlot(ID_FILE_PLOT_FULL);
	CPegView::GetActiveView()->OnViewPrintPreview();
}

//void CPegDoc::OnFilePrintCurrentView()
//{
////	print::FilePrintCurrentView();
//
//	CPegView *pView = CPegView::GetActiveView();
////	pView->LoadStatePrint();
//}

//void CPegDoc::OnFilePrintPreview()
//{
////	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_PRINTPREVIEW), app.GetSafeHwnd(), DlgProcPrintPreview);
//	CreateDialog(app.GetInstance(), MAKEINTRESOURCE(IDD_PRINTPREVIEW), app.GetSafeHwnd(), DlgProcPrintPreview);
//}

///<summary>
/// displays the layer and layer settings of the hilighted segment
///</summary>
void CPegDoc::OnFileQuery()
{
	CPnt pt = app.CursorPosGet();

	CLayer* pLayer = LayersSelUsingPoint(pt);

	if(pLayer == 0)
		return;

	if(!pLayer->IsOff())
	{
		CPoint point;
		::GetCursorPos(&point);

		m_strIdentifiedLayerName = pLayer->GetName();

		int iMenu = (pLayer->IsInternal()) ? IDR_LAYER : IDR_TRACING;
		
		HMENU hMenu = ::LoadMenu(app.GetInstance(), MAKEINTRESOURCE(iMenu));
		CMenu* pMenuSub = CMenu::FromHandle(::GetSubMenu(hMenu, 0));
		
		pMenuSub->ModifyMenu(0, MF_BYPOSITION | MF_STRING, 0, m_strIdentifiedLayerName);
		
		if (iMenu == IDR_LAYER)
		{
			pMenuSub->CheckMenuItem(ID_LAYER_WORK, MF_BYCOMMAND | pLayer->IsHot() ? MF_CHECKED : MF_UNCHECKED);
			pMenuSub->CheckMenuItem(ID_LAYER_ACTIVE, MF_BYCOMMAND | pLayer->IsWarm() ? MF_CHECKED : MF_UNCHECKED);
			pMenuSub->CheckMenuItem(ID_LAYER_STATIC, MF_BYCOMMAND | pLayer->IsCold() ? MF_CHECKED : MF_UNCHECKED);
			pMenuSub->CheckMenuItem(ID_LAYER_HIDDEN, MF_BYCOMMAND | pLayer->IsOff() ? MF_CHECKED : MF_UNCHECKED);
		}
		else
		{
			pMenuSub->CheckMenuItem(ID_TRACING_OPEN, MF_BYCOMMAND/* | pLayer->IsOpened()*/ ? MF_CHECKED : MF_UNCHECKED);
			pMenuSub->CheckMenuItem(ID_TRACING_MAP, MF_BYCOMMAND | pLayer->IsMapped() ? MF_CHECKED : MF_UNCHECKED);
			pMenuSub->CheckMenuItem(ID_TRACING_VIEW, MF_BYCOMMAND | pLayer->IsViewed() ? MF_CHECKED : MF_UNCHECKED);
			pMenuSub->CheckMenuItem(ID_TRACING_CLOAK, MF_BYCOMMAND | pLayer->IsOff() ? MF_CHECKED : MF_UNCHECKED);
		}

		pMenuSub->TrackPopupMenu(0, point.x, point.y, CWnd::FromHandle(app.GetSafeHwnd()), 0);
		::DestroyMenu(hMenu);
	}
}

///<summary>
/// file->save handler
///</summary>
void CPegDoc::OnFileSave()
{
	OpenFileSave();
}

///<summary>
/// file->saveas handler
///</summary>
void CPegDoc::OnFileSaveAs()
{
	OpenFileSaveAs();
}

void CPegDoc::OnFileArchive()
{
	HWND hwnd = app.GetSafeHwnd();
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_ARCHIVE), hwnd, DlgProcFileArchive);
}

void CPegDoc::OnExportDWG()
{
	if(::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_ACADEXPORT), app.GetSafeHwnd(), DlgProcExportACAD) != FALSE)
		ExportDWG();
}

bool CPegDoc::ExportDWG()
{
	bool bAddMRU = false;

	CString strPathName = GetPathName();
	CString strWDirectory = strPathName.Left(strPathName.ReverseFind('\\'));

	//char szFilter[170];
	//SecureZeroMemory(szFilter, 170);
	//::LoadString(app.GetInstance(), IDS_OPENFILE_FILTER_SAVEAS, szFilter, sizeof(szFilter));

//	if(IsFileReadOnly())
//		m_strPathName.Append

	OPENFILENAME of;
	::ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lpstrInitialDir = strWDirectory;
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = 0;
	of.hInstance = app.GetInstance();
	//of.lpstrFilter = szFilter;
	of.lpstrFilter = "DWG Files (.dwg)\0*.dwg\0DXF Files (.dxf)\0*.dxf\0";
	of.nFilterIndex = m_wOpenFileType;
	of.lpstrFile = new char[MAX_PATH];
	strcpy(of.lpstrFile, m_strPathName.TrimRight(".peg") + ".dwg");
	of.nMaxFile = MAX_PATH;
	of.lpstrTitle = "Export DWG/DXF";
	// mak - OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_ENABLEHOOK only for default
	of.Flags = OFN_EXPLORER | OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLESIZING;
	of.lpfnHook = OFNHookProcFileSaveAs;
	// mak - below only for template
	//of.lpTemplateName = MAKEINTRESOURCE(IDD_FILE_SAVEAS);
	
	//switch(m_wOpenFileType)
	//{
	//	//case FILE_TYPE_PEG:
	//	//	of.lpstrDefExt = "peg";
	//	//	break;
	//	case FILE_TYPE_DXF:
	//		of.lpstrDefExt = "dxf";
	//		break;
	//	case FILE_TYPE_DWG:
	//		of.lpstrDefExt = "dwg";
	//		break;
	//	//case FILE_TYPE_JOB:
	//	//	of.lpstrDefExt = "jb1";
	//	//	break;
	//	//case FILE_TYPE_TRACING:
	//	//	of.lpstrDefExt = "tra";
	//	//	break;
	//	//default:
	//	//	of.lpstrDefExt = "peg";
	//	//	of.nFilterIndex = 1;
	//}

	if(GetSaveFileName(&of))
	{
		CString strPathNameTo = of.lpstrFile;
		WORD wFileTypeTo = app.GetFileTypeFromPath(strPathNameTo);

		//OpenFileSaveCleanWarning();

		// kill timer to delete temp file
		//app.KillAutoSave();

		if(wFileTypeTo == FILE_TYPE_DWG || wFileTypeTo == FILE_TYPE_DXF)
			bAddMRU = OpenAutoCADSaveAs(wFileTypeTo, strPathNameTo, true);
		//else if(wFileTypeTo == FILE_TYPE_PEG)
		//	bAddMRU = OpenPegSaveAs(strPathNameTo, true);
		//else if(FileTypeIsTracing(wFileTypeTo))	
		//	bAddMRU = OpenTracingSaveAs(wFileTypeTo, strPathNameTo, true);
	}
	delete [] of.lpstrFile;
	
	return bAddMRU;
}


void CPegDoc::OnExportPeg2000()
{
////	bool bAddMRU = false;
//
//	char szFilter[170];
//	SecureZeroMemory(szFilter, 170);
//	::LoadString(app.GetInstance(), IDS_OPENFILE_FILTER_PEG2000, szFilter, sizeof(szFilter));
//
//	OPENFILENAME of;
//	::ZeroMemory(&of, sizeof(OPENFILENAME));
//	of.lStructSize = sizeof(OPENFILENAME);
//	of.hwndOwner = 0;
//	of.hInstance = app.GetInstance();
//	of.lpstrFilter = szFilter;
//	of.nFilterIndex = 1;
//	of.lpstrFile = new char[MAX_PATH];
//	strcpy(of.lpstrFile, m_strPathName + " - Peg2000");
//	of.nMaxFile = MAX_PATH;
//	of.lpstrTitle = "Export Peg 2000";
//	// mak - OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_ENABLEHOOK only for default
//	of.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_ENABLEHOOK;
////	of.lpfnHook = OFNHookProcFileSaveAs;
//	// mak - below only for template
////	of.lpTemplateName = MAKEINTRESOURCE(IDD_FILE_SAVEAS);
//	
////	switch(m_wOpenFileType)
////	{
////		case FILE_TYPE_PEG:
////			of.lpstrDefExt = "peg";
////			break;
////		case FILE_TYPE_DXF:
////			of.lpstrDefExt = "dxf";
////			break;
////		case FILE_TYPE_DWG:
////			of.lpstrDefExt = "dwg";
////			break;
////		case FILE_TYPE_JOB:
////			of.lpstrDefExt = "jb1";
////			break;
////		case FILE_TYPE_TRACING:
////			of.lpstrDefExt = "tra";
////			break;
////		default:
//			of.lpstrDefExt = "peg";
////			of.nFilterIndex = 1;
////	}
//
//	if(GetSaveFileName(&of))
//	{
//		CString strPathNameTo = of.lpstrFile;
//		WORD wFileTypeTo = app.GetFileTypeFromPath(strPathNameTo);
//
////		strPathNameTo.Append(" - Peg2000");
//
////		// kill timer to delete temp file
////		app.KillAutoSave();
//
////		if(wFileTypeTo == FILE_TYPE_DWG || wFileTypeTo == FILE_TYPE_DXF)
////			bAddMRU = OpenAutoCADSaveAs(wFileTypeTo, strPathNameTo, true);
////		else if(wFileTypeTo == FILE_TYPE_PEG)
////			bAddMRU = OpenPegSaveAs(strPathNameTo, true);
////		else if(FileTypeIsTracing(wFileTypeTo))	
////			bAddMRU = OpenTracingSaveAs(wFileTypeTo, strPathNameTo, true);
//
//		if(wFileTypeTo == FILE_TYPE_PEG)
//			OpenPeg2000SaveAs(strPathNameTo);
//
//		//// mak - mru registry
//		//if(bAddMRU && (!strPathNameTo.IsEmpty()))
//		//{
//		//	doc.SetRegistry(strPathNameTo);
//		//	doc.DisplayMRU();
//
//		//	// start autosave
//		//	app.StartAutoSave();
//		//}
//	}
//	delete [] of.lpstrFile;
//	
////	return bAddMRU;
}

///<summary>
///
///</summary>
void CPegDoc::OnFileTracing()
{
	FileTracingInclude();
}

CString CPegDoc::FileTracingInclude()
{
	CString strResult = "failure";

	static DWORD nFilterIndex = 1;

	CString strPathName = GetPathName();
	CString strWDirectory = strPathName.Left(strPathName.ReverseFind('\\'));

	//char szFilter[120];
	//SecureZeroMemory(szFilter, 120);
	//::LoadString(app.GetInstance(), IDS_OPENFILE_FILTER_TRACINGS, szFilter, sizeof(szFilter));

	//of.lpstrFilter = "Job Files (.jb1)\0*.jb1\0Tracing Files (.tra)\0*.tra\0";

	OPENFILENAME of;
	::ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lpstrInitialDir = strWDirectory;
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = 0;
	of.hInstance = app.GetInstance();
	//of.lpstrFilter = szFilter;
	of.lpstrFilter = "Job Files (.jb1)\0*.jb1\0Tracing Files (.tra)\0*.tra\0";
	of.lpstrFile = new char[MAX_PATH]; 
	of.lpstrFile[0] = 0;
	of.nMaxFile = MAX_PATH;
	of.lpstrTitle = "Browse";
	of.Flags = OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	of.lpstrDefExt = "tra";
	of.lpfnHook = OFNHookProcFileTracing;
	of.lpTemplateName = MAKEINTRESOURCE(IDD_TRACING_EX);
	
	if (GetOpenFileName(&of))
	{
		CString strName = of.lpstrFile;
		CString strPath = strName.Left(of.nFileOffset);

		strName = strName.Mid(of.nFileOffset);

		if (LayersGet(strName) == 0)
		{
			if (TracingMap(strName))
			{
				CLayer* pLayer = LayersGet(strName);
				pLayer->SetStateFlg(CLayer::STATE_FLG_RESIDENT);
			}
		}
	}
	
	delete [] of.lpstrFile;

	return strResult;
}

///<summary>
/// file-> ->iron handler
///</summary>
void CPegDoc::OnIronDrawing()
{
	for(int i = 0; i < LayersGetSize(); i++)
	{
		CLayer *pLayer = CPegDoc::LayersGetAt(i);

		POSITION pos = pLayer->GetHeadPosition();
		while(pos != 0)
		{
			CSeg *pSeg = pLayer->GetNext(pos);

			// code to iron the drawing
			POSITION pos2 = pSeg->GetHeadPosition();
			while(pos2 != 0)
			{
				CPrim *pPrim = pSeg->GetNext(pos2);
				pPrim->Iron();
			}
		}
	}

	CPegView *pView = CPegView::GetActiveView();
	pView->InvalidateRect(NULL, TRUE);
	pView->On3dViewsTop();
}

///<summary>
///
///</summary>
void CPegDoc::OnLayerActive()
{	
	CLayer* pLayer = LayersGet(m_strIdentifiedLayerName);
	
	if(pLayer == 0)
		return;

	if (pLayer->IsHot())
	{
		if(pLayer->GetName() != "0")
		{
			WorkLayerSet(LayersGet("0"));
			pLayer->SetStateWarm();
			UpdateAllViews(NULL, HINT_LAYER_SAFE, pLayer);
		}
		else
		{
			msgWarning(IDS_MSG_LAYER_NO_ACTIVE, m_strIdentifiedLayerName, app.GetSafeHwnd());
		}
	}
	else
	{
		pLayer->SetStateWarm();
		UpdateAllViews(NULL, HINT_LAYER_SAFE, pLayer);
	}

	// mak - 
	app.StatusLineDisplay(WorkCnt);
	CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);
}

///<summary>
///
///</summary>
bool CPegDoc::OnLayerHidden(bool bUseInternal, CString strLayerName)
{
	//CString strThisLayer = (bUseInternal) ? m_strIdentifiedLayerName : strLayerName;
	//CLayer* pLayer = LayersGet(strThisLayer);

	//if(pLayer == 0)
	//	return false;

	//if(pLayer->IsHot())
	//{
	//	if(pLayer->GetName() != "0")
	//	{
	//		WorkLayerSet(LayersGet("0"));
	//		pLayer->SetStateOff();
	//		UpdateAllViews(NULL, HINT_LAYER_SAFE, pLayer);
	//	}
	//	else
	//	{
	//		msgWarning(IDS_MSG_LAYER_NO_HIDDEN, strThisLayer, app.GetSafeHwnd());
	//		return false;
	//	}
	//}
	//else
	//{
	//	pLayer->SetStateOff();
	//	UpdateAllViews(NULL, HINT_LAYER_SAFE, pLayer);
	//}

	//// mak - 
	//app.StatusLineDisplay(WorkCnt);
	//CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);
	//return true;

	CString strThisLayer = (bUseInternal) ? m_strIdentifiedLayerName : strLayerName;
	CLayer* pLayer = LayersGet(strThisLayer);
	
	if(pLayer == 0)
		return false;

	if(pLayer->IsHot())
	{
		if(pLayer->GetName() == "0")
		{
			msgWarning(IDS_MSG_LAYER_NO_HIDDEN, strThisLayer, app.GetSafeHwnd());
			return false;
		}
		else
			WorkLayerSet(LayersGet("0"));
	}

	pLayer->RemoveFromSegLists();
	pLayer->SetStateOff();

	UpdateAllViews(NULL, HINT_LAYER_ERASE, pLayer);
	CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);
	app.StatusLineDisplay(WorkCnt);
	return true;
}

///<summary>
///
///</summary>
void CPegDoc::OnLayerMelt()
{
    if(LayerMelt(m_strIdentifiedLayerName))
	{
		CLayer *pLayer = LayersGet(m_strIdentifiedLayerName);
		if(pLayer->IsHot())
		{
			CLayer *pZeroLayer = LayersGet("0");
			if(pZeroLayer != 0)
				WorkLayerSet(pZeroLayer);
		}
	}

	app.StatusLineDisplay(WorkCnt);
}

///<summary>
///
///</summary>
void CPegDoc::OnLayerStatic()
{
	CLayer* pLayer = LayersGet(m_strIdentifiedLayerName);

	if (pLayer == 0)
		return;

	if(pLayer->IsHot())
	{
		if(pLayer->GetName() == "0")
		{
			msgWarning(IDS_MSG_LAYER_NO_STATIC, m_strIdentifiedLayerName, app.GetSafeHwnd());
			return;
		}
		else
		{
			WorkLayerSet(LayersGet("0"));
//			pLayer->SetStateCold();
		}
	}
//	else
//	{
//		pLayer->SetStateCold();
//		UpdateAllViews(NULL, HINT_LAYER_SAFE, pLayer);
//	}

	pLayer->RemoveFromSegLists();
	pLayer->SetStateCold();

	// mak - 
	app.StatusLineDisplay(WorkCnt);
	CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);
	UpdateAllViews(NULL, HINT_LAYER_SAFE, pLayer);
}

///<summary>
///
///</summary>
void CPegDoc::OnLayerWork()
{
	CLayer* pLayer = LayersGet(m_strIdentifiedLayerName);
	WorkLayerSet(pLayer);

	// mak - 
	app.StatusLineDisplay(WorkCnt);
}

///<summary>
///All layers are made active with the exception of the current working layer.
///This is a warm state meaning the layer is displayed using hot color set, is detectable, 
///and may have its segments modified or deleted. No new segments are added to an active layer. 
///Zero or more layers may be active.
///</summary>
void CPegDoc::OnLayersActiveAll()
{	
	for (WORD w = 0; w < m_layers.GetSize(); w++)
	{
		CLayer* pLayer = LayersGetAt(w);

		// mak - i added the internal part
		if(pLayer->IsInternal())
		{
			if(!pLayer->IsHot())
				pLayer->SetStateWarm();
		}
	}
	
	UpdateAllViews(NULL, 0L, NULL);

	// mak - 
	app.StatusLineDisplay(WorkCnt);
}

///<summary>
///
///</summary>
void CPegDoc::OnLayersStaticAll()
{	
	for(WORD w = 0; w < m_layers.GetSize(); w++)
	{
		CLayer* pLayer = LayersGetAt(w);

		// mak - i added the internal part
		if(pLayer->IsInternal())
		{
			if(!pLayer->IsHot())
				pLayer->SetStateCold();
		}
	}
	
	UpdateAllViews(NULL, 0L, NULL);

	// mak - 
	app.StatusLineDisplay(WorkCnt);
}

void CPegDoc::OnLayersHideAll()
{
	for(WORD w = 0; w < m_layers.GetSize(); w++)
	{
		CLayer* pLayer = LayersGetAt(w);

		// mak - i added the internal part
		if(pLayer->IsInternal())
		{
			if(!pLayer->IsHot())
				pLayer->SetStateOff();
		}
	}
	
	UpdateAllViews(NULL, 0L, NULL);

	// mak - 
	app.StatusLineDisplay(WorkCnt);
}

///<summary>
/// removes duplicate segments from all layers.
///</summary>
int CPegDoc::RemoveDuplicateSegments()
{
	int mainCount = 0;

	CQuadTree quadtree;
	quadtree.CalculateTree();
	quadtree.PopulateTree();
	mainCount = quadtree.RemoveDuplicates();
	quadtree.Unload();

	return mainCount;
}

///<summary>
///
///</summary>
void CPegDoc::OnPensLoadColors()
{
	//char szFilter[30];
	//SecureZeroMemory(szFilter, 30);
	//::LoadString(app.GetInstance(), IDS_OPENFILE_FILTER_PENCOLORS, szFilter, sizeof(szFilter));

	OPENFILENAME of;
	::ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = 0;
	of.hInstance = app.GetInstance();
	//of.lpstrFilter = szFilter;
	of.lpstrFilter = "Pen Color Files (.txt)\0*.txt\0";
	of.lpstrFile = new char[MAX_PATH];
	of.lpstrFile[0] = 0;
	of.nMaxFile = MAX_PATH;
	of.lpstrTitle = "Load Pen Colors";
	of.lpstrInitialDir = app.m_strAppPath;
	of.Flags = OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	of.lpstrDefExt = "txt";
	
	if(GetOpenFileName(&of))
	{
		if((of.Flags & OFN_EXTENSIONDIFFERENT) == 0)
		{
			app.PenColorsLoad(of.lpstrFile);
//			app.SetBackGround(crHotCols[0]);
			app.SetBackGround(crHotCols[app.BackgroundColorGet()]);
			UpdateAllViews(NULL, 0L, NULL);

			//mak - print preview
			CString strTemp = of.lpstrFile;
			printStates.SetOldTranslate(strTemp.Right(strTemp.GetLength() - strTemp.ReverseFind('\\')));
		}
		else
			msgWarning(IDS_MSG_FILE_TYPE_ERROR);
	}
	delete [] of.lpstrFile;
}

///<summary>
///
///</summary>
void CPegDoc::OnPensTranslate()
{
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_TRANSLATE_PENS), app.GetSafeHwnd(), DlgProcTranslatePens);
}

///<summary>
///
///</summary>
void CPegDoc::OnPrimBreak()
{
	CSeg* pSeg = detsegs.SelSegAndPrimUsingPoint(app.CursorPosGet());
	if(pSeg != 0 && detsegs.DetPrim() != 0)
	{
		CPrim* pPrim = detsegs.DetPrim();
		
		PENCOLOR nPenColor = pPrim->PenColor();
		PENSTYLE nPenStyle = pPrim->PenStyle();

		if(pPrim->Is(CPrim::PRIM_POLYLINE))
		{
			pSeg->FindAndRemovePrim(pPrim);

			CPrimPolyline* pPolyline = static_cast<CPrimPolyline*>(pPrim);
			
			CPnts pts;
			pPolyline->GetAllPts(pts);
			
			for(WORD w = 0; w < pts.GetSize() - 1; w++)
				pSeg->AddTail(new CPrimLine(nPenColor, nPenStyle, pts[w], pts[w + 1]));
			
			if(pPolyline->IsLooped())
				pSeg->AddTail(new CPrimLine(nPenColor, nPenStyle, pts[pts.GetUpperBound()], pts[0]));
			
			delete pPrim;
			detsegs.DetSeg() = 0;
		}
		else if(pPrim->Is(CPrim::PRIM_SEGREF))
		{
			CPrimSegRef* pSegRef = static_cast<CPrimSegRef*>(pPrim);

			CBlock* pBlock;
	
			if(BlksLookup(pSegRef->GetName(), pBlock) != 0)
			{
				pSeg->FindAndRemovePrim(pPrim);

				CPnt ptBase = pBlock->GetBasePt();

				CTMat tm = pSegRef->BuildTransformMatrix(ptBase);
				
				CSeg* pSegT = new CSeg(*pBlock);
				pSegT->Transform(tm);
				pSeg->AddTail(pSegT);

				delete pPrim;
				detsegs.DetSeg() = 0;
			}
		}
		else if(pPrim->Is(CPrim::PRIM_INSERT))
		{
		
		}
	}
}

///<summary>
/// Searches for closest detectible primitive.  
/// If found, primitive is lifted from its segment, inserted into a new segment 
/// which is added to deleted segment list. The primitive resources are not freed.
///</summary>
void CPegDoc::OnPrimDelete()
{
	DoPrimDelete(app.CursorPosGet());
}

///<summary>
///
///</summary>
void CPegDoc::OnPrimExtractNum()
{
	CPnt pt = app.CursorPosGet();
	
	CSeg* pSeg = detsegs.SelSegAndPrimUsingPoint(pt);
	if(pSeg == 0) 
		return;

	CPrim* pPrim = detsegs.DetPrim();

	CString strChr;

	if (pPrim->Is(CPrim::PRIM_TEXT))
		strChr = static_cast<CPrimText*>(pPrim)->Text();
	else if (pPrim->Is(CPrim::PRIM_DIM))
		strChr = static_cast<CPrimDim*>(pPrim)->Text();
	else
		return;

	double	dVal[32];
	int 	iTyp;
	long	lDef;
				
	int 	iTokId = 0;
				
	try
	{
		lex::Parse(strChr);
		lex::EvalTokenStream((char*) 0, &iTokId, &lDef, &iTyp, (void*) dVal);
		
		char szBuf[64];
		if (iTyp != lex::TOK_LENGTH_OPERAND)
			lex::ConvertValTyp(iTyp, lex::TOK_REAL, &lDef, dVal);
		sprintf(szBuf, "%10.4f ", dVal[0]);
		strcat(szBuf, "was extracted from drawing");
		msgInformation(szBuf);
		gbl_dExtNum = dVal[0];
//		dde::PostAdvise(dde::ExtNumInfo);
	}
	catch(char* szMessage)
	{
		::MessageBox(0, szMessage, 0, MB_ICONWARNING | MB_OK);
	}
}

///<summary>
///
///</summary>
void CPegDoc::OnPrimExtractStr()
{
	CPnt pt = app.CursorPosGet();
	
	if (detsegs.SelSegAndPrimUsingPoint(pt) != 0)
	{
		CPrim* pPrim = detsegs.DetPrim();

		CString strChr;
		
		if (pPrim->Is(CPrim::PRIM_TEXT))
			strChr = static_cast<CPrimText*>(pPrim)->Text();
		else if (pPrim->Is(CPrim::PRIM_DIM))
			strChr = static_cast<CPrimDim*>(pPrim)->Text();
		else
			return;
		
		strcpy(gbl_szExtStr, strChr);

		strChr += " was extracted from drawing";
		msgInformation(strChr);
//		dde::PostAdvise(dde::ExtStrInfo);
	}
}

///<summary>
/// Positions the cursor at a "control" point on the current engaged segment.
///</summary>
void CPegDoc::OnPrimGotoCenterPoint()
{ 	
	if (detsegs.IsSegEngaged())
	{
		CPnt pt = detsegs.DetPrim()->GetCtrlPt();
		app.CursorPosSet(pt);
	}
}

///<summary>
/// Picks a primative and modifies its attributes to current settings.
///</summary>
void CPegDoc::OnPrimModifyAttributes()
{
	CPnt pt = app.CursorPosGet();

	CSeg* pSeg = detsegs.SelSegAndPrimUsingPoint(pt);

	if (pSeg != 0)
	{
		detsegs.DetPrim()->ModifyState();
		UpdateAllViews(NULL, HINT_PRIM_SAFE, detsegs.DetPrim());
	}
}

void CPegDoc::OnPrimGetAttributes()
{
	CPnt pt = app.CursorPosGet();

	CSeg* pSeg = detsegs.SelSegAndPrimUsingPoint(pt);

	if (pSeg != 0)
		detsegs.DetPrim()->GetState();

	app.StatusLineDisplay(TextHeight);
}

///<summary>
///
///</summary>
void CPegDoc::OnPrimSnapToEndPoint()
{
	CPnt4 ptView(app.CursorPosGet(), 1.);
	ModelView_DoTransform(ptView);
	
	if(detsegs.IsSegEngaged())
	{
		CPrim* pPrim = detsegs.DetPrim();
		
		if(pPrim->PvtOnCtrlPt(ptView)) 
		{
			CPnt ptEng = detsegs.DetPt();
			pPrim->DisRep(ptEng);
			app.CursorPosSet(ptEng);
			return;
		}
		// Did not pivot on engaged primitive
		if(pPrim->IsPtACtrlPt(ptView))
			 // still on the control point of the engaged primitive
			 // need to keep pick from re-identifying it
			 CSeg::IgnorePrim() = pPrim;
	}
	if(detsegs.SelSegAndPrimAtCtrlPt(ptView) != 0)
	{
		CPnt ptEng = detsegs.DetPt();
		detsegs.DetPrim()->DisRep(ptEng);
		app.CursorPosSet(ptEng);
	}
	CSeg::IgnorePrim() = 0;
}

///<summary>
///
///</summary>
void CPegDoc::OnSegBreak()
{
	detsegs.BreakPolylines();
	detsegs.BreakSegRefs();
}

///<summary>
/// Searches for closest detectible segment.  If found, segment is removed
/// from all general segment lists and added to deleted segment list.
/// Notes: The segment resources are not freed.
///</summary>
void CPegDoc::OnSegDelete()
{
	CPnt pt = app.CursorPosGet();
	
	CSeg* pSeg = detsegs.SelSegAndPrimUsingPoint(pt);
	
	if (pSeg != 0)	
	{
		AnyLayerRemove(pSeg);
		detsegs.Remove(pSeg);
		trapsegs.Remove(pSeg);
		UpdateAllViews(NULL, HINT_SEG_ERASE_SAFE, pSeg);
		
		DeletedSegsAddTail(pSeg);
		msgInformation(IDS_SEG_DEL_TO_RESTORE);
	}		
}

///<summary>
///
///</summary>
void CPegDoc::OnSegDeleteLast()
{
	detsegs.DelLast();
}

///<summary>
///Exchanges the first and last segments on the deleted segment list.
///</summary>
void CPegDoc::OnSegExchange()
{
	if (!DeletedSegsIsEmpty())
	{
		CSeg* pSegTail = DeletedSegsRemoveTail();
		CSeg* pSegHead = DeletedSegsRemoveHead();
		DeletedSegsAddTail(pSegHead);
		DeletedSegsAddHead(pSegTail);
	}
}

///<summary>
///
///</summary>
void CPegDoc::OnSegInsert()
{
	DeletedSegsRestore();
}

///<summary>
/// Opens options dlg for the hatch options
///</summary>
void CPegDoc::OnSetupFillHatch()
{
	m_wOpenFocus = IDC_LIST2;
	m_wOpenTab = TAB_OPEN_HATCH;
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_OPTIONS_TABS_MAIN), app.GetSafeHwnd(), DlgProcOptions);
}

void CPegDoc::OnProfileSet(ProfileType eProfile)
{
	HKEY hKey;
	DWORD dwDisp = 0;
	LPDWORD lpdwDisp = &dwDisp;
	CString strKey = "SOFTWARE\\Fanning\\Profile";
	CString strProfState;

	// create and/or open key
	RegCreateKeyEx(HKEY_CURRENT_USER, strKey, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 
				   NULL, &hKey, lpdwDisp);

	switch(eProfile)
	{
		case PROFILE_CLASSIC:
			::CheckMenuRadioItem(app.GetSubMenu(5), ID_PROFILES_CLASSIC, ID_PROFILES_CUSTOM2, ID_PROFILES_CLASSIC, MF_BYCOMMAND);
			strProfState = "classic";
			app.ProfileSetState(PROF_CLASSIC);
			break;
		case PROFILE_NEW:
			::CheckMenuRadioItem(app.GetSubMenu(5), ID_PROFILES_CLASSIC, ID_PROFILES_CUSTOM2, ID_PROFILES_NEW, MF_BYCOMMAND);
			strProfState = "new";
			app.ProfileSetState(PROF_NEW);
			break;
		case PROFILE_CUSTOM1:
			::CheckMenuRadioItem(app.GetSubMenu(5), ID_PROFILES_CLASSIC, ID_PROFILES_CUSTOM2, ID_PROFILES_CUSTOM1, MF_BYCOMMAND);
			strProfState = "customone";
			app.ProfileSetState(PROF_CUSTOM1);
			break;
		case PROFILE_CUSTOM2:
			::CheckMenuRadioItem(app.GetSubMenu(5), ID_PROFILES_CLASSIC, ID_PROFILES_CUSTOM2, ID_PROFILES_CUSTOM2, MF_BYCOMMAND);
			strProfState = "customtwo";
			app.ProfileSetState(PROF_CUSTOM2);
			break;
	}

	RegSetValueEx(hKey, "ProfState", 0, REG_SZ, (LPBYTE)strProfState.GetBuffer(strProfState.GetLength()), strProfState.GetLength() + 1);
	RegCloseKey(hKey);

	// load the correct profile
	CString s3 = app.GetAppPath() + "\\Profiles";
	switch(app.ProfileGetState())
	{
		case PROF_NEW:		s3.Append("\\New.txt"); break;
		case PROF_CUSTOM1:	s3.Append("\\Custom1.txt"); break;
		case PROF_CUSTOM2:	s3.Append("\\Custom2.txt"); break;
		case PROF_CLASSIC:	s3.Append("\\Classic.txt"); break;
	}

	app.ProfileLoadFromFile(s3);

//	ShowWindow(stateframe.GetHWND(), SW_HIDE);
}

void CPegDoc::OnProfileManager()
{
//	m_wOpenFocus = IDC_COL_LIST;
	m_wOpenTab = TAB_OPEN_PROFILES;
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_PROFILE_MANAGER), app.GetSafeHwnd(), DlgProcProfileManager);
}

///<summary>
///
///</summary>
void CPegDoc::OnOptions()
{
	m_wOpenFocus = IDC_COL_LIST;
	m_wOpenTab = TAB_OPEN_APPEARANCE;
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_OPTIONS_TABS_MAIN), app.GetSafeHwnd(), DlgProcOptions);
}

///<summary>
///
///</summary>
void CPegDoc::OnSetup()
{
	m_wOpenFocus = IDC_COL_LIST;
	m_wOpenTab = TAB_OPEN_PEN;
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_TABS_MAIN), app.GetSafeHwnd(), DlgProcSetup);
}

///<summary>
///
///</summary>
void CPegDoc::OnSetupFillHollow()
{
	pstate.SetPolygonIntStyle(POLYGON_HOLLOW);
}

///<summary>
///
///</summary>
void CPegDoc::OnSetupFillSolid()
{
    pstate.SetPolygonIntStyle(POLYGON_SOLID);
}

///<summary>
///
///</summary>
void CPegDoc::OnSetupGotoPoint()
{
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_HOME_POINT_GO), app.GetSafeHwnd(), DlgProcHomePointGo);
}

///<summary>
///
///</summary>
void CPegDoc::OnSetupImport()
{
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_IMPORT), app.GetSafeHwnd(), DlgProcOptionsTabImport);
}

///<summary>
///
///</summary>
void CPegDoc::OnSetupMouseButtons()
{
//	m_wOpenFocus = IDC_LEFT_DOWN;
//	m_wOpenTab = TAB_OPEN_SCROLLMOUSE;
//	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_OPTIONS_TABS_MAIN), app.GetSafeHwnd(), DlgProcOptions);
}

///<summary>
///
///</summary>
void CPegDoc::OnSetupNote()
{
	m_wOpenFocus = IDC_TEXT_HEIGHT;
	m_wOpenTab = TAB_OPEN_NOTE;
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_OPTIONS_TABS_MAIN), app.GetSafeHwnd(), DlgProcOptions);
}

///<summary>
///
///</summary>
void CPegDoc::OnSetupPenColor()
{
	m_wOpenFocus = IDC_COL_LIST;
	m_wOpenTab = TAB_OPEN_PEN;
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_TABS_MAIN), app.GetSafeHwnd(), DlgProcSetup);
	app.StatusLineDisplay(Pen);
}

///<summary>
///
///</summary>
void CPegDoc::OnSetupPenStyle()
{
	m_wOpenFocus = IDC_PENSTYLES;
	m_wOpenTab = TAB_OPEN_PEN;
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_TABS_MAIN), app.GetSafeHwnd(), DlgProcSetup);
	app.StatusLineDisplay(Line);
}

///<summary>
///
///</summary>
void CPegDoc::OnSetupSavePoint()
{
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_HOME_POINT_EDIT), app.GetSafeHwnd(), DlgProcHomePointSet);
	CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);
}

///<summary>
///
///</summary>
void CPegDoc::OnTracingCloak()
{
	CLayer* pLayer = LayersGet(m_strIdentifiedLayerName);

	UpdateAllViews(NULL, HINT_LAYER_ERASE, pLayer);
	pLayer->SetStateOff();
	pLayer->SetTracingFlg(CLayer::TRACING_FLG_CLOAKED);
}

///<summary>
///
///</summary>
void CPegDoc::OnTracingFuse()
{
	TracingFuse(m_strIdentifiedLayerName);
}

///<summary>
///
///</summary>
void CPegDoc::OnTracingMap()
{
	TracingMap(m_strIdentifiedLayerName);
}

///<summary>
///
///</summary>
void CPegDoc::OnTracingView()
{
//	TracingView(m_strIdentifiedLayerName);
}

void CPegDoc::OnTracingMapAll()
{
	for(int i = 0; i < LayersGetSize(); i++)
	{
		CLayer *pLayer = LayersGetAt(i);

		if(!pLayer->IsInternal())
			TracingMap(pLayer->GetName());
	}
}

void CPegDoc::OnTracingViewAll()
{
	for(int i = 0; i < LayersGetSize(); i++)
	{
		CLayer *pLayer = LayersGetAt(i);

		if(!pLayer->IsInternal())
			TracingView(pLayer->GetName());
	}
}

void CPegDoc::OnTracingCloakAll()
{
	for(int i = 0; i < LayersGetSize(); i++)
	{
		CLayer *pLayer = LayersGetAt(i);

		if(!pLayer->IsInternal())
			TracingCloak(pLayer->GetName());
	}
}

void CPegDoc::OnTrapCommandsBlock()
{
	if(trapsegs.GetCount() == 0)
		return;
	
	CBlock* pBlock;
	WORD w = BlksSize();
	char szBlkNam[16];
	
	do
	{
		sprintf(szBlkNam, "_%.3i", ++w);
	}
	while(BlksLookup(szBlkNam, pBlock));
	
	pBlock = new CBlock;

	//mak - see below
//	CPnt ptBL(999999, 999999, 0);
	
	POSITION pos = trapsegs.GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = trapsegs.GetNext(pos);

		// mak - see below
//		if(((CPrim *)(pSeg->GetHead()))->GetCtrlPt()[0]
		
		CSeg* pSeg2 = new CSeg(*pSeg);
		pBlock->AddTail(pSeg2);
		pSeg2->RemoveAll();

		delete pSeg2;
	}

	// mak - point being set at (0,0)...
	// this isn't very user-friendly
//	pBlock->SetBasePt(trapsegs.PvtPt());
	pBlock->SetBasePt(((CPrim *)((CSeg *)trapsegs.GetHead())->GetHead())->GetCtrlPt());

	BlksSetAt(szBlkNam, pBlock);
}

///<summary>
///
///</summary>
void CPegDoc::OnTrapCommandsCompress()
{
	trapsegs.Compress();
}

///<summary>
///
///</summary>
void CPegDoc::OnTrapCommandsExpand()
{
	trapsegs.Expand();
}

///<summary>
///
///</summary>
void CPegDoc::OnTrapCommandsFilter()
{
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_TRAP_FILTER), app.GetSafeHwnd(), DlgProcTrapFilter);
}

/////<summary>
/////
/////</summary>
//void CPegDoc::OnTrapCommandsHighlight()
//{
//	m_bTrapHighlight = !m_bTrapHighlight;
//
//	app.CheckMenuItem(ID_TRAPCOMMANDS_HIGHLIGHT, MF_BYCOMMAND | (m_bTrapHighlight ? MF_CHECKED : MF_UNCHECKED));
//	trapsegs.SetIdentify(m_bTrapHighlight);
//
//	LPARAM lHint = m_bTrapHighlight ? HINT_SEGS_SAFE_TRAP : HINT_SEGS_SAFE;
//	UpdateAllViews(NULL, lHint, &trapsegs);
//}

///<summary>
///
///</summary>
void CPegDoc::OnTrapCommandsInvert()
{
	int iTblSize = LayersGetSize();
	for (int i = 0; i < iTblSize; i++)
	{
		CLayer* pLayer = LayersGetAt(i);
		if (pLayer->IsHot() || pLayer->IsWarm())
		{
			POSITION pos = pLayer->GetHeadPosition();
			while (pos != 0)
			{
				CSeg* pSeg = pLayer->GetNext(pos);
				POSITION posSeg = trapsegs.Find(pSeg);
				if (posSeg != 0)
					trapsegs.RemoveAt(posSeg);
				else
					trapsegs.AddTail(pSeg);
			}
		}
	}
	UpdateAllViews(NULL, 0L, NULL);
	app.StatusLineDisplay();
}

///<summary>
///
///</summary>
void CPegDoc::OnTrapCommandsQuery()
{
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_EDIT_TRAPCOMMANDS_QUERY), app.GetSafeHwnd(), DlgProcEditTrap_CommandsQuery);
}

///<summary>
///
///</summary>
void CPegDoc::OnTrapCommandsSquare()
{
	trapsegs.Square();
}

///<summary>
///
///</summary>
void CPegDoc::OnTrapCommandsUnblock()
{
	trapsegs.BreakSegRefs();
}

void CPegDoc::OnTrapCommandsVectorize()
{
	trapsegs.Vectorize();
}

///<summary>
/// The current trap is copied to the clipboard. This is done with two 
/// independent clipboard formats. 
/// The standard enhanced metafile and the private PegSegs which is read 
/// exclusively by Peg.
///</summary>
void DoEditTrapCopy()
{
	::OpenClipboard(NULL);
	::EmptyClipboard();
	
	if (app.GetEditCFText())
	{
		// UNDONE possible
		CString strBuf;

		POSITION pos = trapsegs.GetHeadPosition();
		while (pos != 0) 		
		{
			CSeg* pSeg = trapsegs.GetNext(pos);

			POSITION posPrim = pSeg->GetHeadPosition();
			while (posPrim != 0)
			{
				CPrim* pPrim = pSeg->GetNext(posPrim);
				if (pPrim->Is(CPrim::PRIM_TEXT))
				{
					strBuf += static_cast<CPrimText*>(pPrim)->Text();
					strBuf += "\r\n";
				}
			}
		}
		GLOBALHANDLE hGMem = GlobalAlloc(GHND, strBuf.GetLength() + 1);
		LPSTR lpBuffer = (LPSTR) GlobalLock(hGMem);
		strcpy(lpBuffer, strBuf);
		GlobalUnlock(hGMem);
		::SetClipboardData(CF_TEXT, hGMem);
	}
	if (app.GetEditCFImage())
	{
		int iPrimState = pstate.Save();
		
		HDC hdcEMF = ::CreateEnhMetaFile(0, 0, 0, 0);
		trapsegs.Display(CDC::FromHandle(hdcEMF));
		HENHMETAFILE hemf = ::CloseEnhMetaFile(hdcEMF);
		::SetClipboardData(CF_ENHMETAFILE, hemf);
	
		pstate.Restore(CDC::FromHandle(hdcEMF), iPrimState);
	}
	if (app.GetEditCFSegments())
	{
		CPnt ptMin(FLT_MAX, FLT_MAX, FLT_MAX);
		CPnt ptMax(- FLT_MAX, - FLT_MAX, - FLT_MAX);

		CMemFile mf;				
	
		mf.SetLength(96);
		mf.SeekToEnd();
	
		char* pBuf = new char[CPrim::BUFFER_SIZE];
			
		trapsegs.Write(mf, pBuf);
		//pSeg->WritePeg(hFile);
		//trapsegs.WritePeg(mf, pBuf);
		trapsegs.GetExtents(ptMin, ptMax, view::GetMatrix());

		delete [] pBuf;
	
		ptMin = view::GetMatrixInverse() * ptMin;
	
		ULONGLONG dwSizeOfBuffer = mf.GetLength();
	
		mf.SeekToBegin();
		mf.Write(&dwSizeOfBuffer, sizeof(DWORD));
		ptMin.Write(mf);

		GLOBALHANDLE hGMem = GlobalAlloc(GHND, SIZE_T(dwSizeOfBuffer));
		LPSTR lpBuffer = (LPSTR) GlobalLock(hGMem);
	
		mf.SeekToBegin();
		mf.Read(lpBuffer, UINT(dwSizeOfBuffer));
	
		GlobalUnlock(hGMem);
		::SetClipboardData(app.GetClipboardFormatPegSegs(), hGMem);
	}
	::CloseClipboard();
}

///<summary>
///
///</summary>
void DoEditTrapPopupCommands(HWND hwnd)
{
	CPoint pnt;
	::GetCursorPos(&pnt);
	HMENU hMenu = ::LoadMenu(app.GetInstance(), MAKEINTRESOURCE(IDR_TRAP));
	hMenu = ::GetSubMenu(hMenu, 0);
	::TrackPopupMenu(hMenu, 0, pnt.x, pnt.y, 0, hwnd, 0);
	::DestroyMenu(hMenu);
}