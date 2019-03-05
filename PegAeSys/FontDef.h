#pragma once

class CFontDef : public CObject
{
public:
	static const WORD PREC_TRUETYPEFONT = 1;
	static const WORD PREC_PEGSTROKEFONT = 2;

	static const WORD PATH_RIGHT = 0;
	static const WORD PATH_LEFT = 1;
	static const WORD PATH_UP = 2;
	static const WORD PATH_DOWN = 3;

	static const WORD HOR_ALIGN_LEFT = 1;
	static const WORD HOR_ALIGN_CENTER = 2;
	static const WORD HOR_ALIGN_RIGHT = 3;
 
	static const WORD VER_ALIGN_TOP = 2;
	static const WORD VER_ALIGN_MIDDLE = 3;
	static const WORD VER_ALIGN_BOTTOM = 4;

private:
	WORD		m_wTextPrec;		// font precision
	CString		m_strTextFont;		// font name
	WORD		m_wTextPath;		// path between characters
	WORD		m_wTextHorAlign;	// text horizontal alignment
	WORD		m_wTextVerAlign;	// text vertical alignment
	double		m_dChrSpac;			// spacing between character cells

public:
	
	CFontDef();
	
	CFontDef(const CFontDef& fd);

	CFontDef& operator=(const CFontDef& fd);

	double		ChrSpac() const {return (m_dChrSpac);}
	void		ChrSpacSet(double d) {m_dChrSpac = d;}

//	void		Read(CFile& fl);
	void		Read(HFILE hFile);
	void		Set(WORD wPrec, const CString& strFont, WORD wPath, WORD wHorAlign, WORD wVerAlign, double dChrSpac);

	CString		FormatTextHorAlign() const;
	CString		FormatTextPath() const;
	CString		FormatTextPrec() const;
	CString		FormatTextVerAlign() const;
	WORD		TextHorAlign() const {return (m_wTextHorAlign);}
	void		TextHorAlignSet(WORD w) {m_wTextHorAlign = w;}
	CString		TextFont() const {return (m_strTextFont);}
	void		TextFontSet(const CString& str) {m_strTextFont = str;}
	WORD		TextPrec() const {return (m_wTextPrec);}
	void		TextPrecSet(WORD w) {m_wTextPrec = w;}
	WORD		TextPath() const {return (m_wTextPath);}
	void		TextPathSet(WORD w) {m_wTextPath = w;}
	WORD		TextVerAlign() const {return (m_wTextVerAlign);}
	void		TextVerAlignSet(WORD w) {m_wTextVerAlign = w;}

	// mak - fix text orientation
	void		TextNoteModifySet(const CFontDef& fd);

//	void		Write(CFile& fl) const;
	void		Write(HFILE hFile) const;
};