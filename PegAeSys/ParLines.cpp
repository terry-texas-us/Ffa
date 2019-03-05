#include "stdafx.h"

///<summary>Cleans up the corners of two sets of parallel lines.</summary>
// Returns: false	parallel lines
//			true otherwise (success)
bool ParLines_CleanCorners(CLine* ln1, CLine* ln2)
{	
	CPnt ptInt;
	
	for (WORD w = 0; w < 2; w++)
	{
		if (ln1[w] || ln2[w])
			return false;
		
		line::Intersection_xy(ln1[w], ln2[w], ptInt);

		ln1[w][1] = ptInt;
		ln2[w][0] = ptInt;
	}
	return true;
}

///<summary>Generates coordinate sets for parallel lines.</summary>
// Notes:	Eccentricity is a function of the width of the line.
//			"Left" is dEcc * dWid to the left of the lead and 
//			"right" is dWid right of left.
// Parameters:	dEcc	eccentricity of lines to lead line (see note)
//				dWid	distance between lines
//				lnLead	lead line
//				pLns	parallel lines endpoints
// Returns: true	otherwise (success)
//			false	null length lead line
bool ParLines_GenPts(double dEcc, double dWid, const CLine& lnLead, CLine* pLns)
{
	pLns[0] = lnLead;
	pLns[1] = lnLead;

	CVec vLead(lnLead);
	double dLen = vLead.Length();
	
	if (dLen > DBL_EPSILON)
	{
		double dX = vLead[1] * dWid / dLen;
		double dY = vLead[0] * dWid / dLen;
		
		CVec v(- dX * dEcc, dY * dEcc, 0.);
		
		pLns[0] += v;
		pLns[1] -= v;

		return true;
	}
	return false;
}

///<summary>
///Generates coordinate sets for parallel lines and modifies previously generated coordinate
///sets to properly close corners.
///</summary>
// Notes:	The first of the two parallel lines lies to the left of lead
//			line, and the second to the right.
// Returns: 2 null length lead line
//			4 lead line parallel to line set being continued
//			1 otherwise (success)
// Parameters:	dEcc		eccentricity of lines to lead line
//				dWid		distance between lines
//				lnLead	lead line
//				pLns, 
//				abCont		sequence continuation status
int ParLines_GenPtsAndClean(double dEcc, double dWid, const CLine& lnLead, CLine* pLns, bool abCont)
{
	CVec vLead(lnLead);
	
	double dLen = vLead.Length();
	
	if (dLen <= DBL_EPSILON)	// Null length lead line
		return (2);

	if (abCont) 
	{
		pLns[0] = pLns[2];
		pLns[1] = pLns[3];
	}
	
	double dX = vLead[1] * dWid / dLen;
	double dY = vLead[0] * dWid / dLen;

	pLns[2] = lnLead + CVec(- dX * dEcc, dY * dEcc, 0.);
	pLns[3] = lnLead + CVec(dX * (1. - dEcc), - dY * (1. - dEcc), 0.);

	if (abCont) 
	{
		CPnt ptInt;
		
		for (int i = 0; i < 2; i++)
		{
			if (line::Intersection_xy(pLns[i], pLns[i + 2], ptInt)) // Two lines are not parallel
			{
				pLns[i][1] = ptInt;
				pLns[i + 2][0] = ptInt;
			}
			else
				return (4);
		}
	}
	return (1);
}