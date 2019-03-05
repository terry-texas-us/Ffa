#include "stdafx.h"

#include "pegaesysdoc.h"
#include "solo.h"

CSolo::CSolo() {}

bool CSolo::SaveSoloFileBlock()
{
	CPegDoc::GetDoc()->OnEditTrapWorkAndActive();
	OnSoloTrapBlock();
	CPegDoc::GetDoc()->LayersRemove("__blockedit__");

	TCHAR szPath[MAX_PATH];
	GetTempPath(MAX_PATH, szPath);
	CString strPathName = szPath;
	strPathName += "__MaKtEmP - block.peg";

	CFilePeg* pFilePegDst = new CFilePeg;
	bool bSuccess = pFilePegDst->Open(strPathName, GENERIC_WRITE, false);

//	CFileException e;
//	CFilePeg* pFilePegDst = new CFilePeg;
//	pFilePegDst->Open(strPathName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite, &e);
	CFilePeg* pFile;

//	if (e.m_cause == CFileException::sharingViolation)
	if(!bSuccess)
	{	
		msgWarning(IDS_MSG_FILE_LOCKED, strPathName);
		delete pFilePegDst;
		return false;
	}
	else
	{
		pFile = CPegDoc::GetDoc()->GetFilePeg();

		if(pFile != 0)
			pFile->Close();

		pFilePegDst->Unload();
		pFilePegDst->Close();

		// write pathname to registry
		CString s1 = "SOFTWARE\\Fanning\\Solo";
		HKEY fKey;
		DWORD dwDisp = 0;
		LPDWORD lpdwDisp = &dwDisp;

		if(RegCreateKeyEx(HKEY_CURRENT_USER, s1, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &fKey, lpdwDisp) == ERROR_SUCCESS)
		{
			RegSetValueEx(fKey, "pathnameblock", 0, REG_SZ, (LPBYTE)strPathName.GetBuffer(strPathName.GetLength()), strPathName.GetLength() + 1);
			RegCloseKey(fKey);
		}
	}

	return true;
}

bool CSolo::SaveSoloFileTracing()
{
	return CPegDoc::GetDoc()->OpenFileSave();
}

bool CSolo::SaveSoloFileLayer()
{
	return CPegDoc::GetDoc()->OpenFileSave();
}

///<summary>
///
///</summary>
void CSolo::OnSoloTrapBlock()
{
	if (trapsegs.GetCount() == 0)
		return;
	
	CBlock* pBlock;	
	pBlock = new CBlock;
	
	POSITION pos = trapsegs.GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = trapsegs.GetNext(pos);
		CSeg* pSeg2 = new CSeg(*pSeg);

		pBlock->AddTail(pSeg2);
		pSeg2->RemoveAll();

		delete pSeg2;
	}
	pBlock->SetBasePt(trapsegs.PvtPt());
	CPegDoc::GetDoc()->BlksSetAt(m_strBlockName, pBlock);
}