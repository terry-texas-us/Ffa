#pragma once

LRESULT CALLBACK SubProcAnnotate(HWND, UINT, WPARAM, LPARAM);

namespace annotate
{
	extern int		nArrowType;
	extern double	dArrowSize;
	extern double	dBubbleRadius;
	extern int		nBubbleFacets;
	extern double	dGapSpacing;
	extern double	dHookRadius;

	extern double	dConstraint;
	
	extern char 	szDefText[16];
	extern char		szCurTxt[16];

	void GenArrowHead(int, double, const CPnt&, const CPnt&, CSeg*);
	CPnt SnapLn(const CPnt& ptBeg, const CPnt& ptEnd);
}