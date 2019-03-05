#include "stdafx.h"

CFontDef::CFontDef()
{
	m_wTextPrec = PREC_PEGSTROKEFONT;
	m_strTextFont = "Simplex.psf";
	m_wTextPath = PATH_RIGHT;
	m_wTextHorAlign = HOR_ALIGN_LEFT;
	m_wTextVerAlign = VER_ALIGN_BOTTOM;
	m_dChrSpac = 0.;
}

CFontDef::CFontDef(const CFontDef& fd) 
{
	m_wTextPrec = fd.m_wTextPrec;
	m_strTextFont = fd.m_strTextFont;
	m_wTextPath = fd.m_wTextPath;
	m_wTextHorAlign = fd.m_wTextHorAlign;
	m_wTextVerAlign = fd.m_wTextVerAlign;
	m_dChrSpac = fd.m_dChrSpac;
}

CFontDef& CFontDef::operator=(const CFontDef& fd)
{
	m_wTextPrec = fd.m_wTextPrec;
	m_strTextFont = fd.m_strTextFont;
	m_wTextPath = fd.m_wTextPath;
	m_wTextHorAlign = fd.m_wTextHorAlign;
	m_wTextVerAlign = fd.m_wTextVerAlign;
	m_dChrSpac = fd.m_dChrSpac;

	return (*this);
}

// mak - fix text orientation
void CFontDef::TextNoteModifySet(const CFontDef& fd)
{
	m_wTextPrec = fd.m_wTextPrec;
	m_strTextFont = fd.m_strTextFont;
	m_wTextPath = fd.m_wTextPath;
	m_dChrSpac = fd.m_dChrSpac;

	// mak - taken out to prevent text from jumping
//	m_wTextHorAlign = fd.m_wTextHorAlign;
//	m_wTextVerAlign = fd.m_wTextVerAlign;
}

CString CFontDef::FormatTextHorAlign() const
{
	CString strAlign[] = {"Left", "Center", "Right"};
	CString str = (m_wTextHorAlign >= 1 && m_wTextHorAlign <= 3) ? strAlign[m_wTextHorAlign - 1] : "Invalid!";

	return (str);
}

CString CFontDef::FormatTextPath() const
{
	CString strPath[] = {"Right", "Left", "Up", "Down"};
	CString str = (m_wTextPath >= 0 && m_wTextPath <= 3) ? strPath[m_wTextPath] : "Invalid!";

	return (str);
}

CString CFontDef::FormatTextPrec() const
{
	CString strPrec[] = {"True Type", "Stroke"};
	CString str = (m_wTextPrec >= 1 && m_wTextPrec <= 2) ? strPrec[m_wTextPrec - 1] : "Invalid!";

	return (str);
}

CString CFontDef::FormatTextVerAlign() const
{
	CString strAlign[] = {"Top", "Middle", "Bottom"};
	CString str = (m_wTextVerAlign >= 2 && m_wTextVerAlign <= 4) ? strAlign[m_wTextVerAlign - 2] : "Invalid!";

	return (str);
}

//void CFontDef::Read(CFile& fl)
//{
//	FilePeg_ReadWord(fl, m_wTextPrec);
//	FilePeg_ReadString(fl, m_strTextFont);
//	FilePeg_ReadWord(fl, m_wTextPath);
//	FilePeg_ReadWord(fl, m_wTextHorAlign);
//	FilePeg_ReadWord(fl, m_wTextVerAlign);
//	FilePeg_ReadDouble(fl, m_dChrSpac);
//}

void CFontDef::Read(HFILE hFile)
{
	FilePeg_ReadWord2(hFile, m_wTextPrec);
	FilePeg_ReadString2(hFile, m_strTextFont);
	FilePeg_ReadWord2(hFile, m_wTextPath);
	FilePeg_ReadWord2(hFile, m_wTextHorAlign);
	FilePeg_ReadWord2(hFile, m_wTextVerAlign);
	FilePeg_ReadDouble2(hFile, m_dChrSpac);

	//WORD w;
	//CString str;
	//double d;
	//FilePeg_ReadWord2(hFile, w);
	//FilePeg_ReadString2(hFile, str);
	//FilePeg_ReadWord2(hFile, w);
	//FilePeg_ReadWord2(hFile, w);
	//FilePeg_ReadWord2(hFile, w);
	//FilePeg_ReadDouble2(hFile, d);
}

void CFontDef::Set(WORD wPrec, const CString& strFont, WORD wPath, WORD wHorAlign, WORD wVerAlign, double dChrSpac)
{
	m_wTextPrec = wPrec;
	m_strTextFont = strFont;
	m_wTextPath = wPath;
	m_wTextHorAlign = wHorAlign;
	m_wTextVerAlign = wVerAlign;
	m_dChrSpac = dChrSpac;
}

//void CFontDef::Write(CFile& fl) const
//{
//	FilePeg_WriteWord(fl, m_wTextPrec);
//	FilePeg_WriteString(fl, m_strTextFont);
//	FilePeg_WriteWord(fl, m_wTextPath);
//	FilePeg_WriteWord(fl, m_wTextHorAlign);
//	FilePeg_WriteWord(fl, m_wTextVerAlign);
//	FilePeg_WriteDouble(fl, m_dChrSpac);
//}

void CFontDef::Write(HFILE hFile) const
{
	FilePeg_WriteWord2(hFile, m_wTextPrec);
	FilePeg_WriteString2(hFile, m_strTextFont);
	FilePeg_WriteWord2(hFile, m_wTextPath);
	FilePeg_WriteWord2(hFile, m_wTextHorAlign);
	FilePeg_WriteWord2(hFile, m_wTextVerAlign);
	FilePeg_WriteDouble2(hFile, m_dChrSpac);
}