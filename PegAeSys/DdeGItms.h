#pragma once

#include "dde.h"

namespace dde
{
	extern PITEMINFO DimAngZInfo;
	extern PITEMINFO DimLenInfo;  
	extern PITEMINFO EngLenInfo;
	extern PITEMINFO EngAngZInfo; 
	extern PITEMINFO ExtNumInfo;
	extern PITEMINFO ExtStrInfo;
	extern PITEMINFO RelPosXInfo;
	extern PITEMINFO RelPosYInfo;
	extern PITEMINFO RelPosZInfo;
	extern PITEMINFO ScaleInfo;
	
	bool		DimAngZPoke(UINT, HSZ, HSZ, HDDEDATA);
	HDDEDATA	DimAngZRequest(UINT, HSZ, HSZ);
	bool		DimLenPoke(UINT, HSZ, HSZ, HDDEDATA);
	HDDEDATA	DimLenRequest(UINT, HSZ, HSZ);
	HDDEDATA	EngAngZRequest(UINT, HSZ, HSZ);
	HDDEDATA	EngLenRequest(UINT, HSZ, HSZ);
	HDDEDATA	ExtNumRequest(UINT, HSZ, HSZ);
	HDDEDATA	ExtStrRequest(UINT, HSZ, HSZ);
	HDDEDATA	RelPosXRequest(UINT, HSZ, HSZ);
	HDDEDATA	RelPosYRequest(UINT, HSZ, HSZ);
	HDDEDATA	RelPosZRequest(UINT, HSZ, HSZ);
	bool		ScalePoke(UINT, HSZ, HSZ, HDDEDATA);
	HDDEDATA	ScaleRequest(UINT, HSZ, HSZ);
}