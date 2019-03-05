#include "stdafx.h"

#include "FileBitmap.h"

CFileBitmap::CFileBitmap(const CString& strFileName)
{
	CString temp = strFileName;
	
	DWORD dwAccess = GENERIC_READ; 
	DWORD dwAttr = FILE_ATTRIBUTE_NORMAL;
	DWORD dwShare = FILE_SHARE_READ;
	DWORD dwDisposition = OPEN_ALWAYS;

	hFile = (HFILE)::CreateFile(temp.GetBuffer(), dwAccess, dwShare, NULL, dwDisposition, dwAttr, NULL);
}

void CFileBitmap::Close()
{
	CloseHandle((HANDLE)hFile);
}

bool CFileBitmap::Load(const CString& strFileName, CBitmap& bmReference, CPalette& palReference)
{
	HBITMAP hBitmap = (HBITMAP) ::LoadImage(0, strFileName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	if (hBitmap == NULL) 
		return false;

	bmReference.Attach(hBitmap);

	// Return now if device does not support palettes

	CClientDC dc(NULL);
	if ((dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE) == 0)
		return true;

	DIBSECTION ds;
	bmReference.GetObject(sizeof(DIBSECTION), &ds);

	int nColors;
	
	if (ds.dsBmih.biClrUsed != 0)
		nColors = ds.dsBmih.biClrUsed;
	else
		nColors = 1 << ds.dsBmih.biBitCount;
	
	// Create a halftone palette if the DIB section contains more than 256 colors
	if (nColors > 256)
	{
		palReference.CreateHalftonePalette(&dc);
	}		
	else
	{	// Create a custom palette from the DIB section's color table
		RGBQUAD* pRGB = new RGBQUAD[nColors];
		
		CDC dcMem;
		dcMem.CreateCompatibleDC(&dc);

		CBitmap* pBitmap = dcMem.SelectObject(&bmReference);
		::GetDIBColorTable((HDC) dcMem, 0, nColors, pRGB);
		dcMem.SelectObject(pBitmap);

		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * (nColors - 1));

		LOGPALETTE* pLogPal = (LOGPALETTE*) new BYTE[nSize];
		
		pLogPal->palVersion = 0x300;
		pLogPal->palNumEntries = (WORD) nColors;
		
		for (int i = 0; i < nColors; i++)
		{
			pLogPal->palPalEntry[i].peRed = pRGB[i].rgbRed;
			pLogPal->palPalEntry[i].peGreen = pRGB[i].rgbGreen;
			pLogPal->palPalEntry[i].peBlue = pRGB[i].rgbBlue;
			pLogPal->palPalEntry[i].peFlags = 0;
		}
		palReference.CreatePalette(pLogPal);

		delete [] pLogPal;
		delete [] pRGB;
	}
	Close();

	return true;
}