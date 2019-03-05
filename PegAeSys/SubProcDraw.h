#pragma once

LRESULT CALLBACK SubProcDraw(HWND, UINT, WPARAM, LPARAM);

namespace draw
{
	enum EArcGen {StartPoint = 1, CenterPoint};
	enum ESplnGen {BSpline, CSpline};
	enum ESplnEndCnd {Fixed, Relaxed, Cyclic, Anticyclic};

	enum ArcType {ARC_THREE_POINT, ARC_C2_POINT};
	enum CircleType {CIRCLE_CENTER_RADIUS, CIRCLE_DIAMETER};
	enum CurveType {CURVE_FITTED, CURVE_SPLINE};

	extern ArcType eArcType;
	extern CurveType eCurveType;
	extern CircleType eCircleType;

	WORD	EndFillAreaDef(WORD, CPnt*);
	WORD	EndPolylineDef(WORD wPts, CPnt* pPts);
	int		EndSplineDef(ESplnGen, int, CPnt*);
	int		EndCSplineDef(ESplnGen, int, CPnt*);
}