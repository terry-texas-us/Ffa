#pragma once

bool	ParLines_CleanCorners(CLine*, CLine*);
bool	ParLines_GenPts(double, double, const CLine&, CLine*);
int		ParLines_GenPtsAndClean(double, double, const CLine&, CLine*, bool);