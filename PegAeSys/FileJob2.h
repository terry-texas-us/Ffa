#pragma once

class CFileJob2// : public CFile
{
	private:
		double m_dVersion;

	public:
		CFileJob2() { m_dVersion = 1.0; }

		//void	Close();
		//
		//bool	OpenForRead(const CString& strPathName);
		//bool	OpenForWrite(const CString& strPathName);
		//void	ReadHeader();
		//void	ReadInfo();
		//void	WriteHeader();
		//void	WriteInfo();

	private:
		HFILE	hFile;
};

/************************************************

Header:
	Version Number = 1.0					double

Clip Extents:
	X-Min									LONG
	X-Max									LONG	
	Y-Min									LONG
	Y-Max									LONG

Origin:	
	X										double
	Y										double

Border Files:
	5 - number of border files				int
	border.jb1								CString
	seal.jb1								CString
	.										.
	.										.
	.										.

*************************************************/