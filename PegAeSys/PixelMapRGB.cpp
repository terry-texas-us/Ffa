#include "stdafx.h"

#include "FileBitmap.h"
#include "PixelMapRGB.h"

//CPixelMapRGB::CPixelMapRGB(int nRows, int nCols) 
//{
//	m_nRows = nRows;
//	m_nCols = nCols;
//	m_rgbPixel = new CRGB[nRows * nCols];
//}
//CPixelMapRGB& CPixelMapRGB::operator=(const CPixelMapRGB& src)
//{
//	m_nRows = src.m_nRows;
//	m_nCols = src.m_nCols;
//	
//	if (m_rgbPixel != 0)
//		delete [] m_rgbPixel;
//
//	m_rgbPixel = new CRGB[m_nRows * m_nCols];
//	
//	for (int i = 0; i < m_nRows * m_nCols; i++)
//		m_rgbPixel[i] = src.m_rgbPixel[i];
//
//	return *this;
//}
//void CPixelMapRGB::Draw()
//{	// Drawn at current position
//	if (m_nRows == 0 || m_nCols == 0) {return;}
//
//	// do not align on 4-byte boundaries in memory
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//	glDrawPixels(m_nCols, m_nRows, GL_RGB, GL_UNSIGNED_BYTE, m_rgbPixel);
//}
//
//CRGB CPixelMapRGB::GetPixel(int iX, int iY) 
//{
//	CRGB rgbBad(255, 255, 255);
//	ASSERT(iX >= 0 && iX < m_nCols);
//	ASSERT(iY >= 0 && iY < m_nRows);
//
//	return m_rgbPixel[m_nCols * iY + iX];
//}
//int CPixelMapRGB::Read(int iX, int iY, int iWidth, int iHeight)
//{
//	m_nRows = iHeight;
//	m_nCols = iWidth;
//	m_rgbPixel = new CRGB[m_nRows * m_nCols];
//	if (!m_rgbPixel)
//		return - 1;
//	// do not align on 4-byte boundaries in memory
//	glPixelStorei(GL_PACK_ALIGNMENT, 1);
//	glReadPixels(iX, iY, m_nCols, m_nRows, GL_RGB, GL_UNSIGNED_BYTE, m_rgbPixel);
//	return 0;
//}
//
//int CPixelMapRGB::Read(CRect rect)
//{
//	m_nRows = rect.top - rect.bottom;
//	m_nCols = rect.right - rect.left;
//	m_rgbPixel = new CRGB[m_nRows * m_nCols];
//	if (!m_rgbPixel)
//		return - 1;
//	// do not align on 4-byte boundaries in memory
//	glPixelStorei(GL_PACK_ALIGNMENT, 1);
//	glReadPixels(rect.left, rect.bottom, m_nCols, m_nRows, GL_RGB, GL_UNSIGNED_BYTE, m_rgbPixel);
//	return 0;
//}
//
//void CPixelMapRGB::SetPixel(int iX, int iY, CRGB rgb)
//{
//	if (iX >= 0 && iX < m_nCols && iY >= 0 && iY < m_nRows)
//		m_rgbPixel[m_nCols * iY + iX] =  rgb;
//}