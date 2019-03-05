#include "stdafx.h"

namespace scroll
{
	// thumb size / nPage - 272 : for standard style
	int nVScrollThumbSize = 200;
	int nHScrollThumbSize = 150;

	// nMax : from 0 to nMax - 1;
	ULONG nVScrollMax = 500;
	ULONG nHScrollMax = 500;

	double tempPos = ((nVScrollMax - nVScrollThumbSize) / 2);

	int nVScrollSens = 4;
	int nHScrollSens = 3;
}