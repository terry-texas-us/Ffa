#include "..\\PegAeSys\\stdafx.h"

#include "irrXML.h"
#include "irrArray.h"
#include "fast_atof.h"
#include "CXMLReaderImpl.h"

namespace xml
{

//! Implementation of the file read callback for ordinary files
class CFileReadCallBack : public IFileReadCallBack
{
public:

	//! construct from filename
	CFileReadCallBack(const char* filename)
	: File(0), Size(0), Close(true)
	{
		// open file
		File = fopen(filename, "rb");

		if (File)
			getFileSize();
	}

	//! construct from FILE pointer
	CFileReadCallBack(FILE* file)
	: File(file), Size(0), Close(false)
	{
		if (File)
			getFileSize();
	}

	//! destructor
	virtual ~CFileReadCallBack()
	{
		if (Close && File)
			fclose(File);
	}

	//! Reads an amount of bytes from the file.
	virtual int read(void* buffer, int sizeToRead)
	{
		if (!File)
			return 0;

		return (int)fread(buffer, 1, sizeToRead, File);
	}

	//! Returns size of file in bytes
	virtual int getSize() {return Size;}

private:
	//! retrieves the file size of the open file
	void getFileSize()
	{
		fseek(File, 0, SEEK_END);
		Size = ftell(File);
		fseek(File, 0, SEEK_SET);
	}

	FILE* File;
	int Size;
	bool Close;

}; // end class CFileReadCallBack


//! Creates an instance of an UFT-8 or ASCII character xml parser. 
IrrXMLReader* createIrrXMLReader(const char* filename)
{
	return new CXMLReaderImpl(new CFileReadCallBack(filename)); 
}

}