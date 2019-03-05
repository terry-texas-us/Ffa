#pragma once

#include "updialog.h"

class CFilePeg
{
public:
	static const WORD SECTION_HEADER	= 0x0101;
	static const WORD SECTION_TABLES	= 0x0102;
	static const WORD SECTION_BLOCKS	= 0x0103;
	static const WORD SECTION_SEGMENTS 	= 0x0104;
	static const WORD SECTION_END		= 0x01ff;

	static const WORD TABLE				= 0x0200;
	static const WORD TABLE_VPORT		= 0x0201;
	static const WORD TABLE_LTYPE		= 0x0202;
	static const WORD TABLE_LAYER		= 0x0203;
	static const WORD TABLE_END			= 0x02ff;

public:
	CFilePeg() {};
	
	virtual ~CFilePeg() {}

	// mak
	bool	Open(CString strPathName, DWORD wStyles, bool bReadOnly);
	void	Close();
	void	Seek(LONG lDist, DWORD dwMove);
	ULONGLONG	GetPosition();

	void	Load();
	void	ReadBlocks();
	void	ReadBlocksSection();
	void	ReadEntities();
	void	ReadEntitiesSection();
	void	ReadHeaderSection();
	void	ReadLayerTable();
	void	ReadLnTypsTable();
	void	ReadTablesSection();
	void	ReadVPortTable();

	bool	Load(const CUPDUPDATA* pCUPDUPData);
	void	ReadBlocks(const CUPDUPDATA* pCUPDUPData);
	void	ReadBlocksSection(const CUPDUPDATA* pCUPDUPData);
	void	ReadEntities(const CUPDUPDATA* pCUPDUPData);
	void	ReadEntitiesSection(const CUPDUPDATA* pCUPDUPData);
	void	ReadHeaderSection(const CUPDUPDATA* pCUPDUPData);
	void	ReadLayerTable(const CUPDUPDATA* pCUPDUPData);
	void	ReadLnTypsTable(const CUPDUPDATA* pCUPDUPData);
	void	ReadTablesSection(const CUPDUPDATA* pCUPDUPData);
	void	ReadVPortTable(const CUPDUPDATA* pCUPDUPData);

	void	Unload();
	void	Unload(const CUPDUPDATA* pCUPDUPData);
	void	WriteBlocksSection();
	void	WriteEntitiesSection();
	void	WriteHeaderSection();
	void	WriteLayerTable();
	void	WritePenStyle(CPenStyle*);
	void	WritePenStyleTable();
	void	WriteTablesSection();
	void	WriteVPortTable();

	void	Recover(const CUPDUPDATA* pCUPDUPData = 0);
	void	RecoverHeaderSection(int nOffset);
	void	RecoverVPortTable(int nOffset);
	void	RecoverLayerTable(int nOffset);
	void	RecoverLnTypsTable(int nOffset);
	void	RecoverBlocksSection(int nOffset);
	void	RecoverEntitiesSection(int nOffset);

private:
	HFILE	hFile;
};

void FilePeg_ReadString2(HFILE hFile, CString& str);
bool FilePeg_ReadPrim2(HFILE hFile, CPrim*&);
void FilePeg_ReadDouble2(HFILE hFile, double& d);
void FilePeg_ReadShort2(HFILE hFile, short& i);
void FilePeg_ReadWord2(HFILE hFile, WORD& w);
WORD FilePeg_ReadWord2(HFILE hFile);
void FilePeg_WriteDouble2(HFILE hFile, double d);
void FilePeg_WriteWord2(HFILE hFile, WORD w);
void FilePeg_WriteString2(HFILE hFile, const CString& str);
bool FilePeg_ReadWord3(HFILE hFile, WORD& w);
CString FilePeg_StringCleanup(CString str);