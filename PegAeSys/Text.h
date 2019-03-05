#pragma once

// At present one embedded command is supported.
// It allows for fractions and the following should be noted:
//	  Formated as folows:
//		^/nn/dd^	where
//			nn is a numerator of zero or more characters
//			dd is a denominator of zero or more characters
//	  The numerator and denominator are each offset in y by 50 / 1000 
//    iregardless of character size attributes.
//	  Horizontal spacing of resulting string is 3 short and must be
//	  accounted for for proper extent determination.

// Control characters (x00 - x1f) (except return/linefeed pair) are replaced by periods.

// Font stroke table encoded as follows:
//	  b0 - b11  relative y displacement
//	  b12 - b23 relative x displacement
//	  b24 - b31 operation code (5 for line, else move)
	
void text_Display(CDC* pDC, const CFontDef& fd, const CTMat& tm, int nOff, int nLen, const CString& strText);
bool text_Display(CDC* pDC, const CFontDef& fd, const CRefSys& rs, const CString& strText);
void text_Display0(CDC* pDC, const CFontDef& fd, const CRefSys& rs, const CString& strText);
void text_GetBottomLeft(const CFontDef& fd, int iChrs, CPnt& pt);
void text_GetBoundingBox(const CFontDef& fd, const CRefSys& cs, int nLen, double dSpacFac,  CPnts& ptsBox);
CPnt text_GetNewLinePos(const CFontDef& fd, const CRefSys& rs, double dLineSpaceFac, double dChrSpaceFac);