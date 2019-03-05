#pragma once

namespace grid
{
	// Sign transfer - returns |A| if B >= 0, and -|A| if B < 0
	#define fsign(A, B) (B >= 0. ? fabs(A) : - fabs(A)) 

	extern bool bDotsOn;
	extern bool	bLnsOn;
	extern bool	bSnapOn;

	extern int		iDotsMax;
	extern double	dXSnapInt;
	extern double	dYSnapInt;
	extern double	dZSnapInt;
	extern double	dXDotInt;
	extern double	dYDotInt;
	extern double	dZDotInt;
	extern double	dXLnInt;
	extern double	dYLnInt;
	extern double	dZLnInt;
	extern double	dXOrig;
	extern double	dYOrig;
	extern double	dZOrig;

	void Display(CDC* pDC);

	CPnt GetOrign();
	CPnt SetOrign(const CPnt&);

	int GetDotsMax();
	void SetDotsMax(int);
	
	bool GetDotsState();
	void SetDotsState(bool);
	
	bool GetLinesState();
	void SetLinesState(bool);
	
//	void Init();
	bool IsSnapEnabled();
	void SetSnapState(bool);

	CPnt UserSnapPt(const CPnt&);

	inline bool GetDotsState()			{return bDotsOn;}
	inline bool GetLinesState()			{return bLnsOn;}
	inline bool IsSnapEnabled()			{return bSnapOn;}
	inline void SetDotsMax(int i)		{iDotsMax = i;}
	inline void SetDotsState(bool b)	{bDotsOn  = b;}
	inline void SetLinesState(bool b)	{bLnsOn   = b;}
	inline void SetSnapState(bool b)	{bSnapOn  = b;}
	inline int  GetDotsMax()			{return iDotsMax;}
}