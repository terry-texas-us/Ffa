#include "stdafx.h"

#include "PegAEsysDoc.h"

#include "FileDWGDirect.h"
#include "FileJob.h"

void ffaReadFile(const CString& strPathName);

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Recover Procedure
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool FilePegRecoverProc(const CUPDUPDATA* pCUPDUPData)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	CFilePeg* pFile = new CFilePeg;

	CString strFileName = *(CString*)pCUPDUPData->GetAppData();

	// mak - added error handling
	if(!pFile->Open(strFileName, GENERIC_READ | GENERIC_WRITE, false))
	{
		msgWarning(IDS_MSG_FILE_LOCKED, strFileName);
		return false;
	}

	pDoc->SetFilePeg(pFile);
	pDoc->SetOpenFile(CPegDoc::FILE_TYPE_PEG, strFileName);

	pFile->Recover(pCUPDUPData);

	pCUPDUPData->SetProgress(_T("Recovery Complete"), 100);	//Set Both Progress Text and Bar

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Open Procedures
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool FilePegOpenThreadProc(const CUPDUPDATA* pCUPDUPData)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	CFilePeg* pFile = new CFilePeg;

	CString strFileName = *(CString*)pCUPDUPData->GetAppData();

	// mak - added error handling
	if(!pFile->Open(strFileName, GENERIC_READ | GENERIC_WRITE, false))
	{
		// allow the file to be opened for read-only access
		int nReturn = MessageBox(app.GetSafeHwnd(), "The file is in use.  Would you like to open a read-only version of the file?", "Warning", MB_YESNO);
//		int nReturn = MessageBox(app.GetSafeHwnd(), "The file is in use.  Please close the file and try again.", "Warning", MB_OK);
		if(nReturn == IDYES)
		{
			if(!pFile->Open(strFileName, GENERIC_READ, true))
			{
				msgWarning(IDS_MSG_FILE_LOCKED, strFileName);
				return false;
			}
			pDoc->SetFileReadOnly(true);
		}
		else
			return false;
	}

	pDoc->SetFilePeg(pFile);
	pDoc->SetOpenFile(CPegDoc::FILE_TYPE_PEG, strFileName);

	try
	{
		pFile->Load(pCUPDUPData);
	}
	catch(char *)
	{
		MessageBox(0, "There was an error opening the file.  You may wish to Recover the file under File->Recover",
			"Error", 0);
		
		//try
		//{
		//	app.OnFileNew(false);
		//}
		//catch(char *)
		//{
			app.OnFileNew(false);
		//	return false;
		//}
		return false;
	}

	pCUPDUPData->SetProgress(_T("Loading Complete"), 100);	

	return true;
}

bool FileFFAOpenThreadProc(const CUPDUPDATA* pCUPDUPData)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	CString strFileName = *(CString*)pCUPDUPData->GetAppData();

	ffaReadFile(strFileName);
	strFileName.TrimRight("ffa");
	strFileName += "peg";
	pDoc->SetOpenFile(CPegDoc::FILE_TYPE_PEG, strFileName);

	pCUPDUPData->SetProgress(_T("Loading Complete"), 100);

	return true;
}

bool FileAutoCADOpenThreadProc(const CUPDUPDATA* pCUPDUPData)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	CString strFileName = *(CString*)pCUPDUPData->GetAppData();

	WORD wFileType = app.GetFileTypeFromPath(strFileName);

	// MAK - added error handling
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFile(strFileName, &FindFileData);
	if(hFind == INVALID_HANDLE_VALUE) 
	{
		msgWarning(IDS_MSG_FILE_NOT_FOUND, strFileName); 
		FindClose(hFind);
		return false;
	} 
	FindClose(hFind);

	// open file if it exists
	CFileDWGDirect fdd;
//	short nError;
	
//	if(fdd.Initialize())//&nError))
	{	
		// mak - wait cursor
//		HCURSOR hOldCursor = ::SetCursor(LoadCursor(0, IDC_WAIT));

		fdd.Load(strFileName.GetBuffer(), pCUPDUPData);
		pDoc->SetOpenFile(wFileType, strFileName);
		pDoc->WorkLayerSet(pDoc->LayersGetAt(0));

		app.StatusLineDisplay(WorkCnt);

//		// mak - old cursor
//		::SetCursor(hOldCursor);
	}	

	pCUPDUPData->SetProgress(_T("Loading Complete"), 100);

	return true;
}

bool TracingOpenThreadProc(const CUPDUPDATA* pCUPDUPData)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	CString strFileName = *(CString*)pCUPDUPData->GetAppData();

	WORD wFileType = app.GetFileTypeFromPath(strFileName);

	CLayer* pLayer = new CLayer(strFileName, CLayer::STATE_FLG_HOT | CLayer::STATE_FLG_WARM);
//	pLayer->SetStateFlg(CLayer::STATE_FLG_INTERNAL);
	pDoc->LayersAdd(pLayer);
	if(!pDoc->TracingLoadLayer(strFileName, pLayer))
		return false;
	detsegs.AddTail(pLayer);

	if(wFileType == CPegDoc::FILE_TYPE_TRACING)
		pDoc->SetOpenFile(CPegDoc::FILE_TYPE_TRACING, strFileName);
	else if(wFileType == CPegDoc::FILE_TYPE_JOB)
		pDoc->SetOpenFile(CPegDoc::FILE_TYPE_JOB, strFileName);

	pDoc->WorkLayerSet(pLayer);

	pCUPDUPData->SetProgress(_T("Loading Complete"), 100);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Save As Procedures
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool FilePegSaveAsThreadProc(const CUPDUPDATA* pCUPDUPData)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	pCUPDUPData->SetProgress(_T("Saving File..."), 0);

	CString strPathNameTo = *(CString*)pCUPDUPData->GetAppData();

	CFilePeg* pFilePegDst = new CFilePeg;
//	pFilePegDst->Open(strPathNameTo, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite | CFile::osSequentialScan, &e);

	if(!pFilePegDst->Open(strPathNameTo, GENERIC_READ | GENERIC_WRITE, false))
	{	
		msgWarning(IDS_MSG_FILE_LOCKED, strPathNameTo);
		delete pFilePegDst;
		return false;
	}
	else
	{
		// MAK - saving .jb1 and .tra as .peg requires fusion
		if(pDoc->FileTypeIsTracing(pDoc->m_wOpenFileType))
		{
			CLayer *player = pDoc->LayersGetAt(1);
			CString title = player->GetName();

			pDoc->TracingFuse(title);
			player->SetStateHot();
		}

		CFilePeg* pFile = pDoc->GetFilePeg();

		if(pFile != 0)
			pFile->Close();
		
		pFilePegDst->Unload(pCUPDUPData);

		pCUPDUPData->SetProgress(_T("Saving Complete"), 100);	//Set Both Progress Text and Bar
		
		pDoc->SetFilePeg(pFilePegDst);
		pDoc->SetOpenFile(CPegDoc::FILE_TYPE_PEG, strPathNameTo);
	}

	return true;
}

//bool FileAutoCADSaveAsThreadProc(const CUPDUPDATA* pCUPDUPData)
//{
//	CPegDoc *pDoc = CPegDoc::GetDoc();
//
//	pCUPDUPData->SetProgress(_T("Saving File..."), 0);
//
//	CString strPathNameTo = *(CString*)pCUPDUPData->GetAppData();
//
//	pCUPDUPData->SetProgress(_T("Saving Complete"), 100);	//Set Both Progress Text and Bar
//
//	return true;
//}

bool FileTracingSaveAsThreadProc(const CUPDUPDATA* pCUPDUPData)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	pCUPDUPData->SetProgress(_T("Saving File..."), 0);

	CString strPathNameTo = *(CString*)pCUPDUPData->GetAppData();

	WORD wFileTypeTo = app.GetFileTypeFromPath(strPathNameTo);

	CString m_strPathName = pDoc->GetPathName();

	//////////////////////////////////////////////////////////////////////////
	if(pDoc->m_wOpenFileType == pDoc->FILE_TYPE_NONE)
	{
		CLayer* pLayer = pDoc->LayersGetAt(0);

		if(wFileTypeTo == CPegDoc::FILE_TYPE_JOB)
		{
			CFileJob fj;
			if(fj.OpenForWrite(strPathNameTo))
			{
				fj.WriteHeader();
				fj.WriteSegs(pLayer);
				fj.Close();

				pDoc->InitAll();
				pDoc->TracingOpen(strPathNameTo);
			}
			else
				return false;
		}
		else if(wFileTypeTo == CPegDoc::FILE_TYPE_TRACING)
		{
			CFileTracing ft;
			if(ft.OpenForWrite(strPathNameTo))
			{
				ft.WriteHeader();
				ft.WriteSegs(pLayer);
				ft.Close();

				pDoc->InitAll();
				pDoc->TracingOpen(strPathNameTo);
			}
			else
				return false;
		}

//		return true;
	}
	else if(pDoc->FileTypeIsTracing(pDoc->m_wOpenFileType))
	{
		CLayer* pLayer = pDoc->LayersGet(m_strPathName);
		
		if(wFileTypeTo == CPegDoc::FILE_TYPE_JOB)
		{
			CFileJob fj;
			if(fj.OpenForWrite(strPathNameTo))
			{
				fj.WriteHeader();
				fj.WriteSegs(pLayer);
				fj.Close();
				pLayer->SetName(strPathNameTo);
				pDoc->SetOpenFile(wFileTypeTo, strPathNameTo);
			}
			else
				return false;
		}
		else if(wFileTypeTo == CPegDoc::FILE_TYPE_TRACING)
		{
			CFileTracing ft;
			if(ft.OpenForWrite(strPathNameTo))
			{
				ft.WriteHeader();
				ft.WriteSegs(pLayer);
				ft.Close();
				pLayer->SetName(strPathNameTo);
				pDoc->SetOpenFile(wFileTypeTo, strPathNameTo);
			}
			else
				return false;
		}	

//		return true;
	}
	else
	{
		msgWarning(IDS_MSG_FILE_NOT_A_TRACING, m_strPathName);
		return false;
	}
	/////////////////////////////////////////////////////////////////////////////////////////

	pCUPDUPData->SetProgress(_T("Saving Complete"), 100);	//Set Both Progress Text and Bar

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Save Procedures
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool FilePegSaveThreadProc(const CUPDUPDATA* pCUPDUPData)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	pCUPDUPData->SetProgress(_T("Saving File..."), 0);

	CString strPathName = *(CString*)pCUPDUPData->GetAppData();

	CFilePeg* pFile = pDoc->GetFilePeg();

	if(pFile != 0)
	{
		if(pDoc->SaveShadow())
			pDoc->WriteShadowFile(false);

		pFile->Unload(pCUPDUPData);

		pCUPDUPData->SetProgress(_T("Saving Complete"), 100);	//Set Both Progress Text and Bar
	}

	return true;
}

bool FileAutoCADSaveThreadProc(const CUPDUPDATA* pCUPDUPData)
{
	pCUPDUPData->SetProgress(_T("Saving File..."), 0);

	CString strPathName = *(CString*)pCUPDUPData->GetAppData();

	WORD wFileType = app.GetFileTypeFromPath(strPathName);

	CFileDWGDirect fdd;
	if(fdd.Create(strPathName.GetBuffer(), wFileType))
	{
		pCUPDUPData->SetProgress(_T("Saving Complete"), 100);	//Set Both Progress Text and Bar
	}
	//CFileDWGDirect::Create(strPathName.GetBuffer(), wFileType);
		
	return true;
}

bool FileTracingSaveThreadProc(const CUPDUPDATA* pCUPDUPData)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	pCUPDUPData->SetProgress(_T("Saving File..."), 0);

	CString strPathName = *(CString*)pCUPDUPData->GetAppData();

	CLayer* pLayer = pDoc->LayersGet(strPathName);
		
	if (pLayer == 0)
		return false;

	if (pDoc->m_wOpenFileType == CPegDoc::FILE_TYPE_JOB)
	{
		CFileJob fj;
		if(!fj.OpenForWrite(strPathName))
			return false;

		fj.WriteHeader();
		fj.WriteSegs(pLayer);
		fj.Close();
	}
	else if (pDoc->m_wOpenFileType == CPegDoc::FILE_TYPE_TRACING)
	{
		CFileTracing ft;
		if(!ft.OpenForWrite(strPathName))
			return false;

		ft.WriteHeader();
		ft.WriteSegs(pLayer);
		ft.Close();
	}	

	pCUPDUPData->SetProgress(_T("Saving Complete"), 100);	//Set Both Progress Text and Bar

	return true;
}