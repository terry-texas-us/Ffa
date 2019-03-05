#pragma once

LRESULT CALLBACK SubProcPipe(HWND, UINT, WPARAM, LPARAM);

namespace pipe
{
	enum EJoin {Begin, End, No};

	extern double dTicSize;
	extern double dCSize;
	extern WORD	wCurSymId;

	void GenDrop(const CPnt&);
	void GenFits(WORD, const CPnt&, WORD, const CPnt&);
	void GenRise(const CPnt&);
	void GenSyms(CDC* pDC, const CPnt&);
	void GenTicMark(const CPnt&, const CPnt&, double);
}