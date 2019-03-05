#include "stdafx.h"

// SECTION_HEADER sentinel						WORD 0x0101
//		{0 or more key-value pairs}
// SECTION_END sentinel							WORD 0x01ff
// SECTION_SEGMENTS sentinel					WORD 0x0104
//		Number of segments						WORD
//		{0 or more segment definitions}
// SECTION_END sentinel							WORD 0x01ff

bool CFileTracing::OpenForRead(CString strPathName)
{
	DWORD dwAccess = GENERIC_READ;
	DWORD dwAttr = FILE_ATTRIBUTE_NORMAL;
	DWORD dwShare = FILE_SHARE_READ | FILE_SHARE_WRITE;
	DWORD dwDisposition = OPEN_ALWAYS;

	hFile = (HFILE)::CreateFile(strPathName.GetBuffer(), dwAccess, dwShare, NULL, dwDisposition, dwAttr, NULL);

	if((HANDLE)hFile == INVALID_HANDLE_VALUE)
	{
		msgWarning(IDS_MSG_FILE_LOCKED, strPathName);
		return false;
	}

	return true;
}

bool CFileTracing::OpenForWrite(CString strPathName)
{
	DWORD dwAccess = GENERIC_WRITE;
	DWORD dwAttr = FILE_ATTRIBUTE_NORMAL;
	DWORD dwShare = FILE_SHARE_READ | FILE_SHARE_WRITE;
	DWORD dwDisposition = OPEN_ALWAYS;

	hFile = (HFILE)::CreateFile(strPathName.GetBuffer(), dwAccess, dwShare, NULL, dwDisposition, dwAttr, NULL);

	if((HANDLE)hFile == INVALID_HANDLE_VALUE)
	{
		msgWarning(IDS_MSG_TRACING_WRITE_FAILURE, strPathName);
		return false;
	}

	return true;
}

void CFileTracing::Close()
{
	CloseHandle((HANDLE)hFile);
}

void CFileTracing::Seek(LONG lDist, DWORD dwMove)
{
	SetFilePointer((HANDLE)hFile, lDist, NULL, dwMove);
}

ULONGLONG CFileTracing::GetPosition()
{
	return SetFilePointer((HANDLE)hFile, 0, NULL, FILE_CURRENT);
}

//////////////////////////////////////////////////////////////////////////////////////////
void CFileTracing::ReadHeader()
{
	if (FilePeg_ReadWord2(hFile) != CFilePeg::SECTION_HEADER)
		throw "Exception CFileTracing: Expecting sentinel SECTION_HEADER.";
	
	// 	with addition of info here will loop key-value pairs till SECTION_END sentinel

	if (FilePeg_ReadWord2(hFile) != CFilePeg::SECTION_END)
		throw "Exception CFileTracing: Expecting sentinel SECTION_END.";
}

bool CFileTracing::ReadSegs(CLayer* pLayer)
{
	if (FilePeg_ReadWord2(hFile) != CFilePeg::SECTION_SEGMENTS)
		throw "Exception CFileTracing: Expecting sentinel SECTION_SEGMENTS.";
	
	CPrim* pPrim;

	WORD wSegCount = FilePeg_ReadWord2(hFile);

	for (WORD wSeg = 0; wSeg < wSegCount; wSeg++)
	{
		int iPrims = FilePeg_ReadWord2(hFile);

		CSeg* pSeg = new CSeg;
		
		for (int i = 0; i < iPrims; i++)
		{
			FilePeg_ReadPrim2(hFile, pPrim);
			pSeg->AddTail(pPrim);
		}
		pLayer->AddTail(pSeg); 
	}
	if (FilePeg_ReadWord2(hFile) != CFilePeg::SECTION_END)
		throw "Exception CFileTracing: Expecting sentinel SECTION_END.";

	return true;
}

void CFileTracing::WriteHeader()
{
	FilePeg_WriteWord2(hFile, CFilePeg::SECTION_HEADER);

	FilePeg_WriteWord2(hFile, CFilePeg::SECTION_END);
}

void CFileTracing::WriteSegs(CLayer* pLayer)
{
	FilePeg_WriteWord2(hFile, CFilePeg::SECTION_SEGMENTS);

	FilePeg_WriteWord2(hFile, (WORD) pLayer->GetCount());
		
	POSITION pos = pLayer->GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = pLayer->GetNext(pos);
		pSeg->WritePeg(hFile);
	}
	FilePeg_WriteWord2(hFile, CFilePeg::SECTION_END);
	msgInformation(IDS_MSG_TRACING_SAVE_SUCCESS, pLayer->GetName());
}