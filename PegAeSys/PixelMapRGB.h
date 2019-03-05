#pragma once

class CRGB : public CObject
{
public:
	BYTE m_bR;
	BYTE m_bG;
	BYTE m_bB;

public: // Contructors and destructor
	CRGB() {m_bR = m_bG = m_bB = 0;}
	CRGB(const CRGB& rgb) {m_bR = rgb.m_bR; m_bG = rgb.m_bG; m_bB = rgb.m_bB;}
	CRGB(BYTE bR, BYTE bG, BYTE bB) {m_bR = bR; m_bG = bG; m_bB = bB;}

	~CRGB() {}

public: // Operators
	CRGB& operator=(const CRGB& src) {m_bR = src.m_bR; m_bG = src.m_bG; m_bB = src.m_bB; return *this;}

public:	// Methods
	void Set(BYTE bR, BYTE bG, BYTE bB) {m_bR = bR; m_bG = bG; m_bB = bB;}
};

//class CPixelMapRGB
//{
//private:
//	int		m_nRows;		// dimensions of the bitmap
//	int		m_nCols;
//	CRGB*	m_rgbPixel;
//
//public: // Constructors and destructor
//	CPixelMapRGB() {m_nRows = m_nCols = 0; m_rgbPixel = 0;}
//	CPixelMapRGB(int nRows, int nCols);
//
//public: // Operators
//	CPixelMapRGB& operator=(const CPixelMapRGB& src);
//
//public: // Methods
//	//void Copy(int iX, int iY, int iWidth, int iHeight) {glCopyPixels(iX, iY, iWidth, iHeight, GL_COLOR);}
//	void Draw();
//	CRGB GetPixel(int iX, int iY);
//	void FreeIt() {delete [] m_rgbPixel; m_nRows = m_nCols = 0;}
//	int Read(CRect rect);
//	int Read(int iX, int iY, int iWidth, int iHeight);
//	void SetPixel(int iX, int iY, CRGB rgb);
//	void WriteBMPFile(char* pFileName);
//};