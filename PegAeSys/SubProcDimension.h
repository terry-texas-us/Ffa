#pragma once

LRESULT CALLBACK SubProcDimension(HWND, UINT, WPARAM, LPARAM);

namespace dimension
{
	CPnt AddArrowHead(CPnt);
	CPnt AddRadius(const CPnt& pt);
	CPnt AddDiameter(const CPnt& pt);
	CPnt CvtPrim(CPnt);
	CPnt ProjPtToLn(CPnt arPt);
}