#pragma once

class CBlock;

class CFileBlock// : public CFile
{
public:
	CFileBlock() {}
	CFileBlock(const CString& strPathName);
	
	virtual ~CFileBlock() {}

	// mak
	bool	Open(CString strPathName, ULONG lStyles);
	void	Close();
	void	Seek(LONG lDist, DWORD dwMove);
	DWORD	GetPosition();
	
	void	ReadFile(const CString&, CBlocks& blks);
	void	ReadBlocks(CBlocks& blks);
	void	ReadHeader();
	void	WriteBlock(const CString& strName, CBlock* pBlock);
	void	WriteBlocks(CBlocks& blks);
	void	WriteFile(const CString& strPathName, CBlocks& blks);
	void	WriteHeader();

private:
	HFILE hFile;
};