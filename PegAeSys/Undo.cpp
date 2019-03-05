#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "Undo.h"

extern CPrimState pstate;
extern CSegsDet detsegs;
extern CPegDoc doc;

CPrimState*	psSav2[] = {0, 0, 0, 0};
//CPegDoc*	pDocSav[] = {0, 0, 0, 0};
CLayers*	pDocSav[] = {0, 0, 0, 0};
CSegsDet*	pSegsDetSav[] = {0, 0, 0, 0};

CUndo::CUndo() 
{ 
	//psSav = {0, 0, 0, 0};
	//pDocSav = 0
	//pSegsDetSav = 0;

	nDoc = 0;
	nPrim = 0;
	nSegs = 0;

	
}//m_UndoDeque.clear(); }

//void CUndo::SaveState(const WORD state)
void CUndo::SaveState()
{
	//CUndoStruct tempStruct;//(detsegs, pstate, doc);

	//CPegDoc *pDoc = CPegDoc::GetDoc();
	//CPegDoc NewDoc = *pDoc;

//	if(pNewDoc == 0)
//		return;

	//tempStruct.SetPegDoc(doc);
	//tempStruct.SetPrimState(pstate);
	//tempStruct.SetSegsDet(detsegs);

	//tempStruct.m_PegDoc = doc;
	//tempStruct.m_PrimState = pstate;
	//tempStruct.m_SegsDet = detsegs;

	/*switch(state)
	{
		case UNDO_PRIM_STATE:
			tempStruct.SetPrimState(&pstate);
			tempStruct.UndoModePrimState();
			break;
		case UNDO_SEGS_DET:
			tempStruct.SetSegsDet(&detsegs);
			tempStruct.SetPegDoc(CPegDoc::GetDoc());
			tempStruct.UndoModeSegsDet();
			break;
	}*/

	SaveDoc();
	SaveSegs();
	SavePrim();



	//m_UndoDeque.push_back(tempStruct);
}

bool CUndo::RestoreLastState()
{
	/*if((m_UndoDeque.size() - 1) < 0)
		return false;

	CUndoStruct tempStruct = m_UndoDeque[m_UndoDeque.size() - 1];*/

//	if(tempStruct.IsUndoModePrimState())
//		pstate  = *(tempStruct.GetPrimState());
//
//	if(tempStruct.IsUndoModeSegsDet())
//	{
//		detsegs = *(tempStruct.GetSegsDet());
////		doc = *(tempStruct.GetPegDoc());
//	}

	//pstate = tempStruct.GetPrimState();
	////doc = tempStruct.GetPegDoc();
	//detsegs = tempStruct.GetSegsDet();

	//m_UndoDeque.pop_back();

	RestoreDoc();
	RestorePrim();
	RestoreSegs();


	return true;
}

void CUndo::SaveDoc()
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	int iSaveId = sizeof(pDocSav) / sizeof(pDocSav[0]) - 1;
	
	while (iSaveId >= 0 && pDocSav[iSaveId] != 0)
		iSaveId--;

	if (iSaveId < 0) 
		msgWarning(IDS_MSG_SAVE_STATE_LIST_ERROR);
	else
	{
		pDocSav[iSaveId] = new CLayers;

		CLayers layersTemp;

		for(int a = 0; a < pDoc->LayersGetSize(); a++)
		{
			CLayer *pLayer = pDoc->LayersGetAt(a);
			CLayer *pNewLayer = new CLayer;

			POSITION pos1 = pLayer->GetHeadPosition();
			while(pos1 != 0)
			{
				CSeg *pSeg = pLayer->GetNext(pos1);
				pNewLayer->AddTail(pSeg);
			}

			layersTemp.Add(pNewLayer);
		}

		//*pDocSav[iSaveId] = *(CPegDoc::GetDoc()->LayersGetAll());
	}
	// return id to use for restore reference
	//return (iSaveId);
	nDoc = iSaveId;
}

void CUndo::SavePrim()
{
	int iSaveId = sizeof(psSav2) / sizeof(psSav2[0]) - 1;
	
	while (iSaveId >= 0 && psSav2[iSaveId] != 0)
		iSaveId--;

	if (iSaveId < 0) 
		msgWarning(IDS_MSG_SAVE_STATE_LIST_ERROR);
	else
	{
		//SetPolygonIntStyle(pstate.PolygonIntStyle());
		psSav2[iSaveId] = new CPrimState;
		*psSav2[iSaveId] = pstate;
	}
	// return id to use for restore reference
	//return (iSaveId);
	nPrim = iSaveId;
}

void CUndo::SaveSegs()
{
	int iSaveId = sizeof(pSegsDetSav) / sizeof(pSegsDetSav[0]) - 1;
	
	while (iSaveId >= 0 && pSegsDetSav[iSaveId] != 0)
		iSaveId--;

	if (iSaveId < 0) 
		msgWarning(IDS_MSG_SAVE_STATE_LIST_ERROR);
	else
	{
		//SetPolygonIntStyle(pstate.PolygonIntStyle());
		pSegsDetSav[iSaveId] = new CSegsDet;
		*pSegsDetSav[iSaveId] = detsegs;
	}
	// return id to use for restore reference
	//return (iSaveId);
	nSegs = iSaveId;
}

void CUndo::RestoreDoc()
{
	CPegDoc *pDoc = CPegDoc::GetDoc();
	int iSaveId = nDoc;

	if (iSaveId >= sizeof(pDocSav) / sizeof(pDocSav[0]))
		return;
	
	if (pDocSav[iSaveId] != 0) 
	{
		//doc = *pDocSav[iSaveId];	
		//CPegDoc::GetDoc()->LayersSetAll(*pDocSav[iSaveId]);

		//CLayers layersTemp = (*pDocSav[iSaveId]);

		for(int a = 0; a < (*pDocSav[iSaveId]).GetSize(); a++)
		{
			CLayer *pNewLayer = (*pDocSav[iSaveId]).GetAt(a);

			CLayer *pOldLayer = pDoc->LayersGet(pNewLayer->GetName());
			pOldLayer->RemoveAll();

			POSITION pos1 = pNewLayer->GetHeadPosition();
			while(pos1 != 0)
			{
				CSeg *pSeg = pNewLayer->GetNext(pos1);
				pOldLayer->AddTail(pSeg);
			}
		}

		delete pDocSav[iSaveId];
		pDocSav[iSaveId] = 0;
	}
}

void CUndo::RestorePrim()
{
	int iSaveId = nPrim;

	if (iSaveId >= sizeof(psSav2) / sizeof(psSav2[0]))
		return;
	
	if (psSav2[iSaveId] != 0) 
	{
		pstate = *psSav2[iSaveId];
		delete psSav2[iSaveId];
		psSav2[iSaveId] = 0;
	}
}

void CUndo::RestoreSegs()
{
	int iSaveId = nSegs;

	if (iSaveId >= sizeof(pSegsDetSav) / sizeof(pSegsDetSav[0]))
		return;
	
	if (pSegsDetSav[iSaveId] != 0) 
	{
		detsegs = *pSegsDetSav[iSaveId];	
		delete pSegsDetSav[iSaveId];
		pSegsDetSav[iSaveId] = 0;
	}
}

