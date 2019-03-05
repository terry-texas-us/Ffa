#pragma once

#include "PegAEsys.h"

#include "StringExtra.h"

void	UnitsString_FormatAngle(char*, double, int = 6, int = 2);
void	UnitsString_FormatLength(char*, EUnits, double, int iPrec, int iScal);
int		UnitsString_GCD(int aiNmb1, int aiNmb2);	   
double  UnitsString_ParseLength(char*);
double	UnitsString_ParseLength(EUnits, char*);