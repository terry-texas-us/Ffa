#pragma once

namespace UserAxis
{
	extern double UserAxis_dInfAng;
	extern double UserAxis_dAxOffAng;

//	void UserAxisInit();
	void UserAxisSetInfAng(double);
	CPnt UserAxisSnapLn(const CPnt&, const CPnt&);
}