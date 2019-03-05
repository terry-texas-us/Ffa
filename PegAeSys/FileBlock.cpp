#include "stdafx.h"

#include "FileBlock.h"

// SECTION_HEADER sentinel						WORD 0x0101
//		{0 or more key-value pairs}
// SECTION_END sentinel							WORD 0x01ff
// SECTION_BLOCKS sentinal						WORD 0x0103
//		Number of blocks						WORD
//		{0 or more block definitions}
// SECTION_END sentinel							WORD 0x01ff

//bool CFileBlock::Open(CString strPathName, WORD wStyles)
bool CFileBlock::Open(CString, ULONG)
{
	//OFSTRUCT sOf;
	//sOf.cBytes = sizeof(OFSTRUCT);

	//hFile = OpenFile(strPathName.GetBuffer(), &sOf, wStyles);

	//return (hFile != HFILE_ERROR);

	return false;
}

void CFileBlock::Close()
{
	CloseHandle((HANDLE)hFile);
}

void CFileBlock::Seek(LONG lDist, DWORD dwMove)
{
	SetFilePointer((HANDLE)hFile, lDist, NULL, dwMove);
}

DWORD CFileBlock::GetPosition()
{
	return SetFilePointer((HANDLE)hFile, 0, NULL, FILE_CURRENT);
}

//////////////////////////////////////////////////////////////////////////////////////////

CFileBlock::CFileBlock(const CString& strPathName)
{
//	CFile::Open(strPathName, modeReadWrite | shareDenyNone);
	Open(strPathName, GENERIC_READ | GENERIC_WRITE);
}

void CFileBlock::ReadBlocks(CBlocks& blks)
{
	if(FilePeg_ReadWord2(hFile) != CFilePeg::SECTION_BLOCKS)
//	if(FilePeg_ReadWord(CFile((HANDLE)hFile)) != CFilePeg::SECTION_BLOCKS)
		throw "Exception CFileBlock: Expecting sentinel SECTION_BLOCKS.";  
	
	CString strName;

	CPrim* pPrim;

	WORD wTblSize = FilePeg_ReadWord2(hFile);
//	WORD wTblSize = FilePeg_ReadWord(CFile);
	for (int i = 0; i < wTblSize; i++)
	{
		int iPrims = FilePeg_ReadWord2(hFile);

		FilePeg_ReadString2(hFile, strName);
		WORD wBlkTypFlgs = FilePeg_ReadWord2(hFile);
	
		CBlock* pBlock = new CBlock(wBlkTypFlgs, ORIGIN);

		for (int i = 0; i < iPrims; i++)
		{
			FilePeg_ReadPrim2(hFile, pPrim);
			pBlock->AddTail(pPrim); 
		}
		blks[strName] = pBlock;
	}
	if (FilePeg_ReadWord2(hFile) != CFilePeg::SECTION_END)
		throw "Exception CFileBlock: Expecting sentinel SECTION_END.";  
}

void CFileBlock::ReadFile(const CString& strPathName, CBlocks& blks)
{
//	CFileException e;

//	if (CFile::Open(strPathName, modeRead | shareDenyNone, &e))
	if(Open(strPathName, GENERIC_READ))
	{
		try
		{
			ReadHeader();
			ReadBlocks(blks);
		}
		catch(char* szMessage)
		{
			msgInformation(szMessage);
		}
	}
}

void CFileBlock::ReadHeader()
{
	if (FilePeg_ReadWord2(hFile) != CFilePeg::SECTION_HEADER)
		throw "Exception CFileBlock: Expecting sentinel SECTION_HEADER.";
	
	// 	with addition of info where will loop key-value pairs till SECTION_END sentinel

	if (FilePeg_ReadWord2(hFile) != CFilePeg::SECTION_END)
		throw "Exception CFileBlock: Expecting sentinel SECTION_END.";
}

void CFileBlock::WriteBlock(const CString& strName, CBlock* pBlock)
{
	WORD wPrims = 0;
	
	DWORD dwCountPosition = GetPosition();
	
	FilePeg_WriteWord2(hFile, wPrims);	
	FilePeg_WriteString2(hFile, strName);
	FilePeg_WriteWord2(hFile, pBlock->GetBlkTypFlgs());
	
	POSITION pos = pBlock->GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = pBlock->GetNext(pos);
		if (pPrim->WritePeg(hFile))
			wPrims++;
	}
	DWORD dwPosition = GetPosition();
	Seek(dwCountPosition, FILE_BEGIN);
	FilePeg_WriteWord2(hFile, wPrims);
	Seek(dwPosition, FILE_BEGIN);
}

void CFileBlock::WriteBlocks(CBlocks& blks)
{
	FilePeg_WriteWord2(hFile, CFilePeg::SECTION_BLOCKS);
	FilePeg_WriteWord2(hFile, (WORD) blks.GetSize());

	CString strKey;
	CBlock* pBlock;
	
	POSITION pos = blks.GetStartPosition();
	while (pos != NULL)
	{
		blks.GetNextAssoc(pos, strKey, pBlock);
		WriteBlock(strKey, pBlock);
	}
	FilePeg_WriteWord2(hFile, CFilePeg::SECTION_END);
}

void CFileBlock::WriteFile(const CString& strPathName, CBlocks& blks)
{
//	CFileException e;
	
//	CFile::Open(strPathName, modeCreate | modeWrite, &e);
	Open(strPathName, GENERIC_WRITE);

	WriteHeader();
	WriteBlocks(blks);
}

void CFileBlock::WriteHeader()
{
	FilePeg_WriteWord2(hFile, CFilePeg::SECTION_HEADER);

	FilePeg_WriteWord2(hFile, CFilePeg::SECTION_END);
}

//void CFileBlock::ReadBlocks(CBlocks& blks)
//{
//	if(FilePeg_ReadWord2(hFile) != CFilePeg::SECTION_BLOCKS)
////	if(FilePeg_ReadWord(*this) != CFilePeg::SECTION_BLOCKS)
//		throw "Exception CFileBlock: Expecting sentinel SECTION_BLOCKS.";  
//	
//	CString strName;
//
//	CPrim* pPrim;
//
//	WORD wTblSize = FilePeg_ReadWord2(hFile);
////	WORD wTblSize = FilePeg_ReadWord(*this);
//	for (int i = 0; i < wTblSize; i++)
//	{
//		int iPrims = FilePeg_ReadWord(*this);
//
//		FilePeg_ReadString(*this, strName);
//		WORD wBlkTypFlgs = FilePeg_ReadWord(*this);
//	
//		CBlock* pBlock = new CBlock(wBlkTypFlgs, ORIGIN);
//
//		for (int i = 0; i < iPrims; i++)
//		{
//			FilePeg_ReadPrim(*this, pPrim);
//			pBlock->AddTail(pPrim); 
//		}
//		blks[strName] = pBlock;
//	}
//	if (FilePeg_ReadWord(*this) != CFilePeg::SECTION_END)
//		throw "Exception CFileBlock: Expecting sentinel SECTION_END.";  
//}
//
//void CFileBlock::ReadFile(const CString& strPathName, CBlocks& blks)
//{
//	CFileException e;
//
//	if (CFile::Open(strPathName, modeRead | shareDenyNone, &e))
//	{
//		ReadHeader();
//		ReadBlocks(blks);
//	}
//}
//
//void CFileBlock::ReadHeader()
//{
//	if (FilePeg_ReadWord(*this) != CFilePeg::SECTION_HEADER)
//		throw "Exception CFileBlock: Expecting sentinel SECTION_HEADER.";
//	
//	// 	with addition of info where will loop key-value pairs till SECTION_END sentinel
//
//	if (FilePeg_ReadWord(*this) != CFilePeg::SECTION_END)
//		throw "Exception CFileBlock: Expecting sentinel SECTION_END.";
//}
//
//void CFileBlock::WriteBlock(const CString& strName, CBlock* pBlock)
//{
//	WORD wPrims = 0;
//	
//	ULONGLONG dwCountPosition = GetPosition();
//	
//	FilePeg_WriteWord(*this, wPrims);	
//	FilePeg_WriteString(*this, strName);
//	FilePeg_WriteWord(*this, pBlock->GetBlkTypFlgs());
//	
//	POSITION pos = pBlock->GetHeadPosition();
//	while (pos != 0)
//	{
//		CPrim* pPrim = pBlock->GetNext(pos);
//		if (pPrim->Write(*this))
//			wPrims++;
//	}
//	ULONGLONG dwPosition = GetPosition();
//	Seek(dwCountPosition, begin);
//	FilePeg_WriteWord(*this, wPrims);
//	Seek(dwPosition, begin);
//}
//
//void CFileBlock::WriteBlocks(CBlocks& blks)
//{
//	FilePeg_WriteWord(*this, CFilePeg::SECTION_BLOCKS);
//	FilePeg_WriteWord(*this, (WORD) blks.GetSize());
//
//	CString strKey;
//	CBlock* pBlock;
//	
//	POSITION pos = blks.GetStartPosition();
//	while (pos != NULL)
//	{
//		blks.GetNextAssoc(pos, strKey, pBlock);
//		WriteBlock(strKey, pBlock);
//	}
//	FilePeg_WriteWord(*this, CFilePeg::SECTION_END);
//}
//
//void CFileBlock::WriteFile(const CString& strPathName, CBlocks& blks)
//{
//	CFileException e;
//	
//	CFile::Open(strPathName, modeCreate | modeWrite, &e);
//
//	WriteHeader();
//	WriteBlocks(blks);
//}
//
//void CFileBlock::WriteHeader()
//{
//	FilePeg_WriteWord(*this, CFilePeg::SECTION_HEADER);
//	FilePeg_WriteWord(*this, CFilePeg::SECTION_END);
//}