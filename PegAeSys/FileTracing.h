#pragma once

class CLayer;

class CFileTracing
{
public:
	CFileTracing() {};
	
	virtual ~CFileTracing() {}

	// mak
	bool	OpenForWrite(CString strPathName);
	bool	OpenForRead(CString strPathName);
	void	Close();
	void	Seek(LONG lDist, DWORD dwMove);
	ULONGLONG	GetPosition();

	bool	ReadSegs(CLayer* pLayer);
	void	ReadHeader();
	void	WriteSegs(CLayer* pLayer);
	void	WriteHeader();

private:
	HFILE hFile;
};