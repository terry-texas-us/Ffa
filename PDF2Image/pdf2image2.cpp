// pdf2tif.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <commdlg.h>

#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <stdlib.h>

#include <string>
#include <vector>
using namespace std;

extern "C" __declspec(dllexport) 
int __stdcall PDFToImageConverter(const char *szPDFFileName, const char *szOutputName,
			       const char *szUserPassword, const char *szOwnPassword,
			       int xresolution, int yresolution, int bitcount, 
			       int compression, int quality, BOOL grayscale, 
			       BOOL multipage,  int firstPage,int lastPage);
extern "C" __declspec(dllexport) 
int __stdcall PDFToImageConverterEx(const char *szPDFFileName, const char *szOutputName,
			       const char *szUserPassword, const char *szOwnPassword, int nIsWidthAndHeight,
			       int xResORWidth, int yResORHeight, int bitcount, 
			       int compression, int quality, int grayscale, 
			       int multipage,  int firstPage,int lastPage);
extern "C" __declspec(dllexport) 
int __stdcall PDFToImageConverter2(const char *szPDFFileName, const char *szOutputName,
			       const char *szUserPassword, const char *szOwnPassword,
			       int xresolution, int yresolution, int bitcount, 
			       int compression, int quality, BOOL grayscale, 
			       BOOL multipage,  int firstPage,int lastPage);
extern "C" __declspec(dllexport) 
int __stdcall PDFToImageConverter2Ex(const char *szPDFFileName, const char *szOutputName,
			       const char *szUserPassword, const char *szOwnPassword, int nIsWidthAndHeight,
			       int xResORWidth, int yResORHeight, int bitcount, 
			       int compression, int quality, int grayscale, 
			       int multipage,  int firstPage,int lastPage);
extern "C" __declspec(dllexport)
void __stdcall PDFToImageSetParameters(const char *parms);
extern "C" __declspec(dllexport) 
int __stdcall PDFToImageGetPageCount(const char *szPDFFileName);
extern "C" __declspec(dllexport)
HBITMAP __stdcall PDFToImageDrawToHDC(const char *szPDFFileName, HDC hDC, const char *szUserPassword, 
		    const char *szOwnPassword, int xresolution, int yresolution, 
		    int m_iBitCount, BOOL m_bGrayscale, int m_iPageIndex);
extern "C" __declspec(dllexport)
int __stdcall PDFToImageGetPageBox(const char *inputPDFFile, int page, double *extent);
extern "C" __declspec(dllexport)
int __stdcall PDFToImageGetPageWidth(const char *inputPDFFile, int page);
extern "C" __declspec(dllexport)
int __stdcall PDFToImageGetPageHeight(const char *inputPDFFile, int page);
typedef int (__stdcall *PDFToImageProgressCBFunc)(void *, UINT Msg, WPARAM wParam, LPARAM lParam);
extern "C" __declspec(dllexport)
void __stdcall PDFToImageSetProgressCBFunc(PDFToImageProgressCBFunc newFunc);
extern "C" __declspec(dllexport)
void __stdcall PDFToImageSetFileNameSuffix(const char *ptrFileNameSuffix);
extern "C" __declspec(dllexport)
void __stdcall PDFToImageSetCode(const char *lpRegcode);
extern "C" __declspec(dllexport)
void __stdcall PDFToImageEnableErrorDiffusion(BOOL bEnable);


extern "C" __declspec(dllexport)
int __stdcall TIFFImageGetPageCount(char *inputTIFFFile);
extern "C" __declspec(dllexport)
int __stdcall TIFFImageGetColors(char *inputTIFFFile, int pageno);
extern "C" __declspec(dllexport)
int __stdcall TIFFImageGetCompression(char *inputTIFFFile, int pageno);

#define	    COMPRESSION_NONE        1	/* dump mode */
#define	    COMPRESSION_CCITTRLE    2	/* CCITT modified Huffman RLE */
#define	    COMPRESSION_CCITTFAX3   3	/* CCITT Group 3 fax encoding */
#define	    COMPRESSION_CCITTFAX4   4	/* CCITT Group 4 fax encoding */
#define	    COMPRESSION_LZW         5       /* Lempel-Ziv  & Welch */
#define	    COMPRESSION_JPEG        7	/* %JPEG DCT compression */
#define	    COMPRESSION_PACKBITS    32773	/* Macintosh RLE */
#define	    COMPRESSION_CLASSF      88888888
#define	    COMPRESSION_CLASSF196   88888889
///////////////////////////////////////////
//
//  Set options into a TIFF file
//
//  Parameters
//
//  char *inputTIFFFile:  input TIFF file
//  int pageno:  specify page index
//  int colors:  specify colors, it can be 1, 4, 8, 24
//  int compression: 
//#define	    COMPRESSION_NONE		1	/* dump mode */
//#define	    COMPRESSION_CCITTRLE	2	/* CCITT modified Huffman RLE */
//#define	    COMPRESSION_CCITTFAX3	3	/* CCITT Group 3 fax encoding */
//#define     COMPRESSION_CCITT_T4        3       /* CCITT T.4 (TIFF 6 name) */
//#define	    COMPRESSION_CCITTFAX4	4	/* CCITT Group 4 fax encoding */
//#define     COMPRESSION_CCITT_T6        4       /* CCITT T.6 (TIFF 6 name) */
//#define	    COMPRESSION_LZW		5       /* Lempel-Ziv  & Welch */
//#define	    COMPRESSION_OJPEG		6	/* !6.0 JPEG */
//#define	    COMPRESSION_JPEG		7	/* %JPEG DCT compression */
//
//
///////////////////////////////////////////
extern "C" __declspec(dllexport)
int __stdcall TIFFImageSetOptions(char *inputTIFFFile, int pageno, int colors, int compression);

char INIFILENAME[MAX_PATH];
HINSTANCE	hInst		= NULL;		// current instance
vector<string>	m_szPDFFileAry;
VOID	GetFileName(HWND hWnd)
{
	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_PATH];
	char ext[_MAX_EXT];

	m_szPDFFileAry.erase(m_szPDFFileAry.begin(),m_szPDFFileAry.end());

	OPENFILENAME ofn;
	//file type filters
	CHAR szFilterSpec[] = "PDF Files(*.pdf)\0*.pdf\0All Files(*.*)\0*.*\0\0";
	CHAR szFileTitle[MAX_PATH];
	
	strcpy(szFileTitle, "");

	CHAR strInitialDir[MAX_PATH];
	GetPrivateProfileString("Parameter","InitialDir","",strInitialDir,MAX_PATH,INIFILENAME);

	
	char *m_pChBuf = NULL;
	if ( ( m_pChBuf	= ( char * ) malloc( 1024*100 )) == NULL )
		return;
	memset(m_pChBuf,0,1024*100);
	//fill in non-variant fields of OPENFILENAME struct.
	ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner		= hWnd;
	ofn.hInstance		= hInst;
	ofn.lpstrFilter		= szFilterSpec;
	ofn.lpstrCustomFilter	= NULL;
	ofn.nMaxCustFilter	= 0;
	ofn.nFilterIndex	= 0;
	ofn.lpstrFile		= m_pChBuf;
	ofn.nMaxFile		= 20000;
	ofn.lpstrInitialDir	= strInitialDir;
	ofn.lpstrFileTitle	= szFileTitle;
	ofn.nMaxFileTitle	= MAX_PATH;
	ofn.lpstrTitle		= "PDF to TIFF software";
	ofn.lpstrDefExt		= "PDF";
	ofn.Flags		= OFN_FILEMUSTEXIST | OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_EXPLORER;

	//Use standard open dialog
	if (!GetOpenFileName ((LPOPENFILENAME)&ofn))
	{
	}
	else
	{
		char* pChBuf = m_pChBuf;
		pChBuf += strlen( pChBuf ) + 1;
		if ( *pChBuf == 0 )		//only one file
		{
			_splitpath(m_pChBuf, drive, dir, fname, ext );
			_makepath(path_buffer,drive, dir,"","");
			WritePrivateProfileString("Parameter","InitialDir",path_buffer,INIFILENAME);
			m_szPDFFileAry.push_back(m_pChBuf);
		}
		else
		{//multiple files
			while(*pChBuf)
			{
				strcpy(path_buffer,m_pChBuf);
				if(path_buffer[strlen(path_buffer)-1] != '\\\\')
					strcat(path_buffer,"\\\\");
				strcat(path_buffer,pChBuf);
				m_szPDFFileAry.push_back(path_buffer);
				pChBuf += strlen( pChBuf ) + 1;
			}
			WritePrivateProfileString("Parameter","InitialDir",m_pChBuf,INIFILENAME);
		}
	}
	free(m_pChBuf);
	return;
}
void StructureIniFileName()
{
	CHAR	szExeDir[MAX_PATH];
	GetModuleFileName(hInst,szExeDir,MAX_PATH);
	CHAR drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
	_splitpath(szExeDir, drive, dir, fname, ext );
	_makepath(INIFILENAME, drive, dir,"config",".ini");
}
int __stdcall PageProgressCBFunc(void *ptrPDFName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	printf("PDFName=%s,Msg=%d,CurrentPage=%d,LastPage=%d\n",(char*)ptrPDFName,Msg,wParam,lParam);
	return 1;
}
typedef struct PDF2IMAGETHREADINFOtag{
	char szPDFName[MAX_PATH];
	int  dwThreadID;
	int  iResult;
}PDF2IMAGETHREADINFO,*LPPDF2IMAGETHREADINFO;
static DWORD WINAPI LaunchPDF2ImageThread(LPVOID pParam)
{
	LPPDF2IMAGETHREADINFO lpPDF2ImgInfo = (LPPDF2IMAGETHREADINFO)pParam;
	int iThreadID = GetCurrentThreadId();
	char szOutName[MAX_PATH];
	char szFileExt[MAX_PATH];
	CHAR drive[_MAX_DRIVE],dir[_MAX_DIR],fname[MAX_PATH],ext[_MAX_EXT];

	sprintf(szFileExt,"_%d",iThreadID);
	_splitpath(lpPDF2ImgInfo->szPDFName, drive, dir, fname, ext );
	strcat(fname,szFileExt);
	_makepath(szOutName, drive, dir, fname, ".tif");

	int result = PDFToImageConverter(lpPDF2ImgInfo->szPDFName,szOutName,NULL,
		NULL,300,300,1,COMPRESSION_CCITTFAX4,100,FALSE,TRUE,-1,-1);
	printf("ThreadID = %d, OutName = %s, Result = %d\n",iThreadID,szOutName,result);
	return 0;
}
void TestForMultiThread(char *szPDFName)
{
	PDFToImageSetProgressCBFunc(NULL);
	for(int i = 0; i < 10; i++)
	{
		LPPDF2IMAGETHREADINFO lpPDF2ImgInfo = new PDF2IMAGETHREADINFO;
		strcpy(lpPDF2ImgInfo->szPDFName,szPDFName);
		
		DWORD dwThreadId;
		HANDLE hImageOpenThread = CreateThread(NULL,0,LaunchPDF2ImageThread,lpPDF2ImgInfo,0,&dwThreadId);
	}
}

void GetModulePath(char *out_path,char *in_name)
{
	char *p;
	GetModuleFileName(NULL,out_path,256);
	p =strrchr(out_path,'\\');
	p[1]=0;
	strcat(out_path,in_name);
}

typedef int (__stdcall *PDFToImageConverterFunc)(const char *szPDFFileName, const char *szOutputName,
			       const char *szUserPassword, const char *szOwnPassword,
			       int xresolution, int yresolution, int bitcount, 
			       int compression, int quality, BOOL grayscale, 
			       BOOL multipage,  int firstPage,int lastPage);
typedef void (__stdcall *PDFToImageSetCodeFunc)(const char *lpRegcode);
PDFToImageConverterFunc lpPDFToImageConverter = NULL;
PDFToImageSetCodeFunc   lpPDFToImageSetCode = NULL;
BOOL ConvertPDF2ImageDynamic(char *lpPDFFile, char *lpImgFile)
{
	char path[_MAX_PATH];
	GetModulePath(path,"pdf2image.dll");
	HINSTANCE hPDF2Image = LoadLibrary(path);
	if(hPDF2Image == NULL)
		return FALSE;
	lpPDFToImageConverter=(PDFToImageConverterFunc)GetProcAddress(hPDF2Image,"PDFToImageConverter");
	if(lpPDFToImageConverter==NULL)
		return FALSE;
  lpPDFToImageSetCode =(PDFToImageSetCodeFunc)GetProcAddress(hPDF2Image,"PDFToImageSetCode");
  if(lpPDFToImageSetCode ==NULL)
    return FALSE;
  lpPDFToImageSetCode("XXXXXXXXXXXXXXXXXXXXXXX");
	int nResult = lpPDFToImageConverter(lpPDFFile,lpImgFile,NULL,NULL,200,200,24,
		COMPRESSION_PACKBITS,100,FALSE,TRUE,-1,-1);
	FreeLibrary(hPDF2Image);
	return TRUE;
}
int ConvertPDFToEMF(LPCTSTR lpPDFName, LPCTSTR lpEMFName)
{
	PDFToImageSetParameters("AlwaysUseEmbeddedFonts=N");
	PDFToImageSetParameters("AlwaysUseWindowsFonts=Y");
	PDFToImageSetParameters("UseFontGlyphs=N");
	int nResult = PDFToImageConverter(lpPDFName,lpEMFName,NULL,NULL,200,200,24,
			0,100,FALSE,FALSE,-1,-1);
	return nResult;
}
int main()
{
	char drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
	int nResult = 0;
	char szFileName[MAX_PATH];

	hInst = NULL;
	StructureIniFileName();

	if(__argc != 3)
	{
		char path_buffer[_MAX_PATH];
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_PATH];
		char ext[_MAX_EXT];

		GetFileName(GetDesktopWindow());
		int nsize = m_szPDFFileAry.size();
		if(nsize == 0)
			return 0;
		int result = 0;
		for(int i = 0; i < nsize; i++)
		{
			const char *szPDFName = m_szPDFFileAry[i].c_str();
			_splitpath(szPDFName, drive, dir, fname, ext );
			_makepath(path_buffer,drive, dir,fname,".tif");
			
			result = PDFToImageConverter(szPDFName,path_buffer,NULL,NULL,200,200,24,
				COMPRESSION_PACKBITS,70,FALSE,TRUE,-1,-1);
		}
		//MessageBox(NULL,"Conversion OK","PDF2TIFF",MB_OK);
		return 0;
	}
	//Set your registration key at here
	PDFToImageSetCode("XXXXXXXXXXXXXXXXXXXXXXXXXX");
	PDFToImageEnableErrorDiffusion(TRUE);

	char *szPDFFileName = __argv[1];
	char *szOutputName  = __argv[2];
	int pagecount = PDFToImageGetPageCount(szPDFFileName);
	printf("Filename is: %s\n",szPDFFileName);
	printf("Page count is: %d\n",pagecount);
	for(int page = 1; page <= pagecount; page++)
	{
		double m_fPageBox[4];
		PDFToImageGetPageBox(szPDFFileName,page,m_fPageBox);
		printf("Page %3d box is: [%f,%f,%f,%f]\n",page,m_fPageBox[0],m_fPageBox[1],m_fPageBox[2],m_fPageBox[3]);
		int nPageWidth = PDFToImageGetPageWidth(szPDFFileName,page);
		int nPageHeight = PDFToImageGetPageHeight(szPDFFileName,page);

		if(0)
		{
			_splitpath(szOutputName, drive, dir, fname, ext );
			char fname2[MAX_PATH];
			sprintf(fname2,"%s%04d",fname,page);
			char path_buffer[MAX_PATH];
			_makepath(path_buffer,drive, dir,fname2,ext);
			nResult = PDFToImageConverterEx(szPDFFileName,path_buffer,NULL,NULL,1,
				1018,1590,24,COMPRESSION_PACKBITS,70,TRUE,TRUE,page,page);
			//-1,m_iPageHeight/3,24,COMPRESSION_PACKBITS,70,TRUE,TRUE,page,page);
		}
	}
	PDFToImageSetProgressCBFunc(PageProgressCBFunc);
	PDFToImageSetFileNameSuffix(".%02d");
	int iBeginTick = GetTickCount();
	if(1)
	{
		printf("PDFToImageConverter()\n");
		_splitpath(szOutputName, drive, dir, fname, ext );
		char fname2[_MAX_PATH];
		strcpy(fname2,fname);
		strcat(fname2,"_1_");
		_makepath(szFileName,drive, dir,fname2, ext );
		nResult = PDFToImageConverter(szPDFFileName,szFileName,NULL,NULL,200,200,24,
			COMPRESSION_PACKBITS,100,FALSE,TRUE,-1,-1);
		strcpy(fname2,fname);
		strcat(fname2,"_1_Ex");
		_makepath(szFileName,drive, dir,fname2, ext );
		nResult = PDFToImageConverterEx(szPDFFileName,szFileName,NULL,NULL,1,595*2,842*2,24,
			COMPRESSION_PACKBITS,100,FALSE,TRUE,-1,-1);
	}
	if(0)
	{
		//Convert PDF file to 204x98DPI TIFF-F File,
		nResult = PDFToImageConverter(szPDFFileName,szOutputName,NULL,NULL,200,200,24,
			COMPRESSION_CLASSF,100,FALSE,FALSE,-1,-1);
		//Convert PDF file to 204x196DPI TIFF-F File,
		nResult = PDFToImageConverter(szPDFFileName,szOutputName,NULL,NULL,300,300,24,
			COMPRESSION_CLASSF196,100,FALSE,FALSE,-1,-1);
	}
	if(1)
	{
		printf("PDFToImageConverter2()\n");
		_splitpath(szOutputName, drive, dir, fname, ext );
		char fname2[_MAX_PATH];
		strcpy(fname2,fname);
		strcat(fname2,"_2_");
		_makepath(szFileName,drive, dir,fname2, ext );
		nResult = PDFToImageConverter2(szPDFFileName,szFileName,NULL,NULL,200,200,24,
			COMPRESSION_PACKBITS,100,FALSE,TRUE,-1,-1);
		strcpy(fname2,fname);
		strcat(fname2,"_2_Ex");
		_makepath(szFileName,drive, dir,fname2, ext );
		nResult = PDFToImageConverter2Ex(szPDFFileName,szFileName,NULL,NULL,1,595*2,842*2,24,
			COMPRESSION_PACKBITS,100,FALSE,TRUE,-1,-1);
	}
	if(0)
	{
		//Draw a PDF file to HDC directly
		HBITMAP hBitmap = PDFToImageDrawToHDC(szPDFFileName,GetDC(NULL),NULL,NULL,72,72,1,FALSE,1);
		if(hBitmap)
			DeleteObject(hBitmap);
	}
	//result = TIFFImageSetOptions(szOutputName,-1,1,COMPRESSION_NONE);
	int iEndTick = GetTickCount();
	printf("PageCount = %d, Return value is %d, time = %d ms\n",pagecount,nResult,iEndTick-iBeginTick);

	if(0)
		ConvertPDF2ImageDynamic(szPDFFileName,szFileName);
	if(0)
	{
		//Test for multi-thread conversion
		TestForMultiThread(szPDFFileName);
		printf("Press any key to continue...\n");
		getchar();
	}
	if(0)
	{
		_splitpath(szOutputName, drive, dir, fname, ext );
		char fname2[_MAX_PATH];
		strcpy(fname2,fname);
		_makepath(szFileName,drive, dir,fname2, ".emf");
		nResult = ConvertPDFToEMF(szPDFFileName, szFileName);
	}
	return nResult;
}