#include "stdafx.h"

#include "UserAxis.h"

namespace UserAxis
{
	double UserAxis_dInfAng = 2.;
	double UserAxis_dAxOffAng = 0.;
}

/////<summary>Set Axis constraint tolerance angle and offset axis constraint offset angle</summary>
//void UserAxisInit()
//{	
//	UserAxis_dInfAng  = 2.;
//	UserAxis_dAxOffAng = 0.;
//}

void UserAxis::UserAxisSetInfAng(double dAng)
{
	UserAxis_dInfAng = dAng;
}

///<summary>Constrains a line to nearest axis pivoting on first endpoint.</summary>
// Notes:	Offset angle only support about z-axis
// Returns: Point after snap
CPnt UserAxis::UserAxisSnapLn(const CPnt& ptBeg, const CPnt& ptEnd)
{
	return (line::ConstrainToAx(CLine(ptBeg, ptEnd), UserAxis_dInfAng, UserAxis_dAxOffAng));
}