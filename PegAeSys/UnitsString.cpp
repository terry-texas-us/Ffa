#include "stdafx.h"

#include "PegAEsys.h"

#include "Lex.h"

///<summary>Finds the greatest common divisor of arbitrary integers.</summary>
// Returns: First number if second number is zero, greatest 
//			common divisor otherwise.
// Parameters:	aiNmb1	first number
//				aiNmb2	second number
int UnitsString_GCD(int aiNmb1, int aiNmb2)	   
{
	int iRetVal = abs(aiNmb1);
	int iNmb = abs(aiNmb2);
	while (iNmb != 0)
	{
		int iRem = iRetVal % iNmb;
		iRetVal = iNmb;
		iNmb = iRem;
	}
	return (iRetVal);
}

void UnitsString_FormatAngle(char* szBuf, double dAng, int iPrec, int iScal) 
{
	char szFmt[24];

	strcpy(szFmt, "%");
	strcat(szFmt, _itoa(iPrec, szBuf, 10));
	strcat(szFmt, ".");
	strcat(szFmt, _itoa(iScal, szBuf, 10));
	strcat(szFmt, "f");

	sprintf(szBuf, strcat(szFmt, "d"), dAng / RADIAN);
}

void UnitsString_FormatLength(char* aszBuf, EUnits eUnits, double adVal, int aiPrec, int aiScal)		
{
	// Produces a string formatted to type eUnits from a "length" value
	// Architectural units formatted as follows:
	//	<feet>'<inches>.<fraction numerator>/<fraction denominator>"
	// Engineering units formatted as follows:
	//	<feet>'<inches>.<decimal inches>"
	// All other units formatted using floating decimal.
	
	char szBuf[16];

	// Reduce length to number of scaled inches
	double dLen = adVal * app.GetScale();			
	
	if (eUnits == Architectural)
	{
		// Determine how many whole feet and inches
		int iNmbFt = int(dLen / 12.);
		int iNmbIn = abs(int(fmod(dLen, 12.)));
		int iUnitsPrec = app.GetUnitsPrec();
		int iNum = int(fabs(fmod(dLen, 1.)) * (double) (iUnitsPrec) + .5);	// Numerator of fractional component of inches
		
		if (iNum == iUnitsPrec)
		{	// Tricky wholes
			if (iNmbIn == 11)
			{
				iNmbFt++;
				iNmbIn = 0;
			}
			else
				iNmbIn++;
			
			iNum = 0;
		}
		aszBuf[0] = '\0';
		if (iNmbFt == 0 && dLen < 0.)
		{
			aszBuf[0] = '-';
			aszBuf[1] = '\0';
		}
		_itoa(iNmbFt, szBuf, 10);
		strcat(aszBuf, szBuf);
		strcat(aszBuf, "'");
		_itoa(iNmbIn, szBuf, 10);
		strcat(aszBuf, szBuf);
		if (iNum > 0)				// Significant decimal inch component
		{
			strcat(aszBuf, "^/");
			int	iGrtComDivisor = UnitsString_GCD(iNum, iUnitsPrec);
			iNum = iNum / iGrtComDivisor;
			int iDen = iUnitsPrec / iGrtComDivisor; // Add fractional component of inches
			_itoa(iNum, szBuf, 10);
			strcat(aszBuf, szBuf);
			strcat(aszBuf, "/");
			_itoa(iDen, szBuf, 10);
			strcat(aszBuf, szBuf);
			strcat(aszBuf, "^");
		}
		strcat(aszBuf, "\"");
	}
	else if (eUnits == Engineering)
	{
		int iScal = aiScal;
		if (fabs(dLen) >= 1.)			 
			iScal = aiScal - int(log10(fabs(dLen))) - 1;
		if (iScal >= 0)
		{											 
			double dScalFac = pow(10., (double) iScal);
			
			dLen = fabs(dLen) + .5 /  dScalFac; 				// Round it
			aszBuf[0] = ' ';
			aszBuf[1] = '\0';
			_itoa(int(dLen / 12.), szBuf, 10);
			strcat(aszBuf, szBuf);
			dLen = fmod(dLen, 12.);
			if (adVal < 0.) 
				aszBuf[0] = '-';
			strcat(aszBuf, "'");
			_itoa(int(dLen), szBuf, 10);
			strcat(aszBuf, szBuf);
			strcat(aszBuf, ".");
			dLen = fmod(dLen, 1.) * dScalFac;
			_itoa(int(dLen), szBuf, 10);
			strcat(aszBuf, szBuf);
			strcat(aszBuf, "\"");
		}	
		else
			aszBuf[0] = '\0';
		
	}
	else
	{
		char szFmt[12];
		
		strcpy(szFmt, "%");
		strcat(szFmt, _itoa(aiPrec, szBuf, 10));
		strcat(szFmt, ".");
		strcat(szFmt, _itoa(aiScal, szBuf, 10));
		strcat(szFmt, "f");
		
		if (eUnits == Feet) 
			sprintf(aszBuf, strcat(szFmt, "'"), dLen / 12.);
		else if (eUnits == Inches)
			sprintf(aszBuf, strcat(szFmt, "\""), dLen);
		else if (eUnits == Meters)
			sprintf(aszBuf, strcat(szFmt, "m"), dLen * .0254);
		else if (eUnits == Millimeters) 
			sprintf(aszBuf, strcat(szFmt, "mm"), dLen * 25.4);
		else if (eUnits == Centimeters)
			sprintf(aszBuf, strcat(szFmt, "cm"), dLen * 2.54);
		else if (eUnits == Decimeters)
			sprintf(aszBuf, strcat(szFmt, "dm"), dLen * .254);
		else if (eUnits == Kilometers)
			sprintf(aszBuf, strcat(szFmt, "km"), dLen * .0000254);
		else
			aszBuf[0] = '\0';
	}
}

double UnitsString_ParseLength(char* aszLen)
{
	char*	szEndPtr;
	
	double dRetVal = strtod(aszLen, &szEndPtr);
	
	switch (toupper((int) szEndPtr[0]))
	{
		case '\'':												// Feet and maybe inches
			dRetVal *= 12.; 										// Reduce to inches
			dRetVal += strtod(&szEndPtr[1], &szEndPtr); 			// Begin scan for inches at character following foot delimeter
			break;
	
		case 'M': 
			if (toupper((int) szEndPtr[1]) == 'M')
				dRetVal *= .03937007874015748;
			else
				dRetVal *= 39.37007874015748;
			break;
	
		case 'C':
			dRetVal *= .3937007874015748;
			break;
			 
		case 'D':
			dRetVal *= 3.937007874015748;
			break;
		
		case 'K':
			dRetVal *= 39370.07874015748;
	
	}
	return (dRetVal / app.GetScale());
}

double UnitsString_ParseLength(EUnits eUnits, char* aszLen)
{	// Convert length expression to double value.
	try
	{
		int iTokId = 0;
		long lDef;
		int iTyp;
		double dVal[32];
		
		lex::Parse(aszLen);
		lex::EvalTokenStream((char*) 0, &iTokId, &lDef, &iTyp, (void*) dVal);
		
		if (iTyp == lex::TOK_LENGTH_OPERAND)
			return (dVal[0]);
		else
		{
			lex::ConvertValTyp(iTyp, lex::TOK_REAL, &lDef, dVal);
			
			switch (eUnits)
			{
				case Architectural:
				case Engineering:
				case Feet:
					dVal[0] *= 12.;
					break;

				case Meters:
					dVal[0] *= 39.37007874015748;
					break;

				case Millimeters:
					dVal[0] *= .03937007874015748;
					break;

				case Centimeters:
					dVal[0] *= .3937007874015748;
					break;
				
				case Decimeters:
					dVal[0] *= 3.937007874015748;
					break;

				case Kilometers:
					dVal[0] *= 39370.07874015748;
			}				
			dVal[0] /= app.GetScale();
		}
		return (dVal[0]);
	}
	catch(char* szMessage)
	{
		::MessageBox(0, szMessage, 0, MB_ICONWARNING | MB_OK);
		return (0.);
	}
}