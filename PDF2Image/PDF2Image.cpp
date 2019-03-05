#include "..\\PegAeSys\\stdafx.h"

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