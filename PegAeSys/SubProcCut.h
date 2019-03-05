#pragma once

LRESULT CALLBACK SubProcCut(HWND, UINT, WPARAM, LPARAM);

namespace cut
{
	void CutPrimAt2Pts(CDC* pDC, CPnt, CPnt);
	void CutPrimsAtPt(CPnt pt);
	void CutPrimsByLn(CPnt, CPnt);
	void CutSegsByArea(CDC* pDC, CPnt, CPnt);
}