#include "stdafx.h"

#include "Lex.h"
#include "UnitsString.h"
using namespace lex;

namespace lex
{
	int iTokenType[TOKS_MAX];	// token type identifier
	int iToks;					// number of tokens in current token stream
	int iValsCount;				// number of values
	int iValLoc[TOKS_MAX];		// location of value
	long lValues[VALS_MAX];
}

///<summary>
///Converts a stream of tokens into a postfix stack for evaluation.
///</summary>
///<returns>
///true	successful completion
///false	empty postfix stack (but no errors really) or unbalanced parenthesis
///</returns>
///<parameters>
///aiLoc location of first token in stream to consider
///		 (out) location of first token not part of expression
///ref iEnd	number of tokens on stack
///aiTyp type of tokens on stack
///aiTokLoc	location of tokens on stack
///</parameters>
void lex::BreakExpression(int& iLoc, int& iEnd, int* aiTyp, int* aiTokLoc)		
{
	int iOpnPars = 0;
	int iPrvTyp = 0;
	
	int iOpStkTyp[32];
	
	int iOpStkTop = 1;
	iOpStkTyp[iOpStkTop] = TOK_IDENTIFIER;
	
	iEnd = 0;

	int iTyp = TokType(iLoc);
	while (iTyp != - 1) 
	{
		switch (toktbl[iTyp].eClass)
		{
			case Constant:
				aiTyp[iEnd] = iTyp;
				aiTokLoc[iEnd++] = iLoc;
				break;
			
			case OpenParen: 
				iOpStkTyp[++iOpStkTop] = iTyp;	// Push to operator stack
				iOpnPars++;
				break;
			
			case CloseParen:
				if (iOpnPars == 0) 
					break;
			
				while (iOpStkTyp[iOpStkTop] != TOK_LPAREN)	// Move operator to token stack
					aiTyp[iEnd++] = iOpStkTyp[iOpStkTop--];
			
				iOpStkTop--;		// Discard open parentheses
				iOpnPars--; 		// One less open parentheses
				break;
		
			case BinaryArithOp:
			case Other:
				if (iTyp == TOK_BINARY_PLUS || iTyp == TOK_BINARY_MINUS)
				{
					ETokClass eClassPrv = toktbl[iPrvTyp].eClass;
					if (eClassPrv != Constant && eClassPrv != Identifier && eClassPrv != CloseParen) 
						iTyp = (iTyp == TOK_BINARY_PLUS) ? TOK_UNARY_PLUS : TOK_UNARY_MINUS;
				}
				// Pop higher priority operators from stack
				while (toktbl[iOpStkTyp[iOpStkTop]].iInStkPrio >= toktbl[iTyp].iInComPrio)
					aiTyp[iEnd++] = iOpStkTyp[iOpStkTop--]; 	
				// Push new operator onto stack
				iOpStkTyp[++iOpStkTop] = iTyp;	
				break;
		
			// TODO .. classes of tokens which might be implemented
			case Identifier:
			case BinaryRelatOp:
			case BinaryLogicOp:
			case UnaryLogicOp:
			case AssignOp:
			
			default:
				break;
		}
		iPrvTyp = iTyp;
		iTyp = TokType(++iLoc);
	}
	if (iOpnPars > 0) 
		throw "Unbalanced parentheses";
				
	while (iOpStkTop > 1) aiTyp[iEnd++] = iOpStkTyp[iOpStkTop--];
	
	if (iEnd == 0)
		throw "Syntax error";
}

void lex::ConvertValToString(char* acVal, CD* arCD, char* acPic, int* aiLen)
{
	long lTyp = arCD->lTyp;
	int iDim = LOWORD(arCD->lDef);
	
	if (lTyp == TOK_STRING) 
	{
		*aiLen = iDim;
		acPic[0] = '\'';
		memmove(&acPic[1], acVal, *aiLen);
		acPic[++*aiLen] = '\'';
		acPic[++*aiLen] = '\0';
	}
	else 
	{
		char cVal[32];
		long* lVal = (long*) cVal;
		double* dVal = (double*) cVal;
	
		char* szpVal;
		int iLoc;
		 
		int iVLen = 0;
		int iValId = 0;
		int iLnLoc = 0;
		int iLen = HIWORD(arCD->lDef);
		
		if (lTyp != TOK_INTEGER) iLen = iLen / 2;
		
		if (iDim != iLen)
		{	// Matrix
			acPic[0] = '[';
			iLnLoc++;
		}
		for (int i1 = 0; i1 < iLen; i1++) 
		{
			iLnLoc++;
			if (iLen != 1 && (i1 % iDim) == 0)
				acPic[iLnLoc++] = '[';
			if (lTyp == TOK_INTEGER) 
			{
				memcpy(lVal, &acVal[iValId], 4);
				iValId += 4;
				_ltoa(*lVal, &acPic[iLnLoc], 10);
				iVLen = (int) strlen(&acPic[iLnLoc]);
				iLnLoc += iVLen;
			}
			else 
			{
				memcpy(dVal, &acVal[iValId], 8);
				iValId += 8;
				if (lTyp == TOK_REAL) 
				{
					iLoc = 1;
					// pCvtDoubToFltDecTxt(*dVal, 7, iLoc, cVal);
					szpVal = strtok(cVal, " ");
					strcpy(&acPic[iLnLoc], szpVal);
					iLnLoc += (int) strlen(szpVal);
				}
				else if (lTyp == TOK_LENGTH_OPERAND) 
				{
// TODO: Length to length string
					iLnLoc += iVLen;
				}
				else if (lTyp == TOK_AREA_OPERAND) 
				{
					// pCvtWrldToUsrAreaStr(*dVal, &acPic[iLnLoc], iVLen);
					iLnLoc += iVLen;
				}
			}
			if (iLen != 1 && (i1 % iDim) == iDim - 1)
				acPic[iLnLoc++] = ']';
		}
		if (iDim == iLen) 
			*aiLen = iLnLoc - 1;
		else 
		{
			acPic[iLnLoc] = ']';
			*aiLen = iLnLoc;
		}
	}
}

///<summary>Does value type conversion
// Parameters:	aiTyp		type of value(s)
//				aiTypReq	type of value(s) required
//				alDef		dimension (lo word) and length (hi word) of result
//				apVal		value(s)
void lex::ConvertValTyp(int aiTyp, int aiTypReq, long* alDef, void* apVal)		
{
	if (aiTyp == aiTypReq) return;
	
	double* pdVal = (double*) apVal;
	long*	piVal = (long*) apVal;
	
	if (aiTyp == TOK_STRING) 
	{
		char	szVal[256];
		
		strcpy(szVal, (char*) apVal);
		if (aiTypReq == TOK_INTEGER) 
		{
			piVal[0] = atoi(szVal);
			*alDef = MAKELONG(1, 1);
		}
		else
		{
			pdVal[0] = atof(szVal);
			*alDef = MAKELONG(1, 2);
		}
	}
	else if (aiTyp == TOK_INTEGER) 
	{
		if (aiTypReq == TOK_STRING) 
		{
			// integer to string
		}
		else
		{
			pdVal[0] = (double) piVal[0];
			*alDef = MAKELONG(1, 2);
		}
	}
	else
	{
		if (aiTypReq == TOK_STRING) 
		{
			// real to string
		}
		else if (aiTypReq == TOK_INTEGER) 
		{
			// real to integer
		}
	}
}

///<summary>Converts a literal user units string to a double precision value.</summary>
// Notes:	Assumes that a valid liter user units string is passed with no suffix
//			characters evaluated.
// Effect:
// Parameters:	aiTyp		type of value(s) required
//				alDef		dimension (lo word) and length (hi word) of string
//				aszVal		string to convert
//				alDefReq	dimension (lo word) and length (hi word) of result
//				aVal		result
void lex::ConvertStringToVal(int aiTyp, long alDef, char* aszVal, long* alDefReq, void* aVal)			
{
	if (LOWORD(alDef) <= 0) throw "Empty string";

	char szTok[64];
	int iNxt = 0;
	
	int iTyp = Scan(szTok, aszVal, iNxt);
	if (aiTyp == TOK_INTEGER)									// Conversion to integer
	{
		long *pVal = (long *) aVal;
		
		if (iTyp == TOK_INTEGER)
			*pVal = atol(szTok);
		else if (iTyp == TOK_REAL)
			*pVal = (long) atof(szTok);
		else
			throw "String format conversion error";
		*alDefReq = MAKELONG(1, 1);
	}
	else 
	{
		double *pVal = (double *) aVal;
		
		if (iTyp == TOK_INTEGER)
			*pVal = (double) atoi(szTok);
		else if (iTyp == TOK_REAL)
			*pVal = atof(szTok);
		else
			throw "String format conversion error";
		*alDefReq = MAKELONG(1, 2);
	}
}

///<summary> Evaluates an expression.</summary>
// Returns: 1 infix expression successfully evaluated
//			0 unspecified syntax error
// Parameters:	aiTokId 	
//				alDef		dimension (lo word) and length (hi word) of result
//				aiTyp		type of result
//				apOp		result
void lex::EvalTokenStream(char*, int* aiTokId, long* alDef, int* aiTyp, void* apOp)	
{
	char szTok[256];

	int iDim;
	int iTyp;

	long lDef1 = MAKELONG(1, 1);
	int iDim1;
	int iLen1;
	int iTyp1 = TOK_INTEGER;
	
	long lDef2;
	int iDim2;
	int iLen2;
	int iTyp2;
	
	int iExprEnd;
	int iExprTokTyp[32];
	int iExprTokLoc[32];

	BreakExpression(*aiTokId, iExprEnd, iExprTokTyp, iExprTokLoc);
	
	int iOpStkTyp[32];
	long lOpStk[32][32];
	long lOpStkDef[32];
	
	char* cOp1 = (char*) apOp;
	double* dOp1 = (double*) apOp;
	long* lOp1 = (long*) apOp;

	char cOp2[256];
	double* dOp2 = (double*) cOp2;
	long* lOp2 = (long*) cOp2;
		
	int iOpStkTop = 0;												// Empty operand stack
	int iTokStkId = 0;												// Start with first token
	
	while (iTokStkId < iExprEnd) 
	{
		int iTokTyp = iExprTokTyp[iTokStkId];
		int iTokLoc = iExprTokLoc[iTokStkId];
		if (toktbl[iTokTyp].eClass == Identifier)
		{
			// symbol table stuff if desired
			throw "Identifier token class not implemented";
		}
		else if (toktbl[iTokTyp].eClass == Constant) 
		{
			iTyp1 = iTokTyp;
			lDef1 = lValues[iValLoc[iTokLoc]];
			memcpy(cOp1, &lValues[iValLoc[iTokLoc] + 1], HIWORD(lDef1) * 4);
		}
		else													
		{	// Token is an operator .. Pop an operand from operand stack
			if (iOpStkTop == 0) throw "Operand stack is empty";
			
			iTyp1 = iOpStkTyp[iOpStkTop];
			lDef1 = lOpStkDef[iOpStkTop];
			iLen1 = HIWORD(lDef1);
			memcpy(cOp1, &lOpStk[iOpStkTop--][0], iLen1 * 4);
			
			if (toktbl[iTokTyp].eClass == Other)
			{	// intrinsics and oddball unary minus/plus
				if (iTyp1 == TOK_STRING) 
				{
					iDim1 = LOWORD(lDef1);
					strcpy(szTok, cOp1);
					if (iTokTyp == TOK_TOINTEGER) 
					{
						iTyp1 = TOK_INTEGER;
						ConvertStringToVal(TOK_INTEGER, lDef1, szTok, &lDef1, cOp1);
					}
					else if (iTokTyp == TOK_REAL) 
					{
						iTyp1 = TOK_REAL;
						ConvertStringToVal(TOK_REAL, lDef1, szTok, &lDef1, cOp1);
					}
					else if (iTokTyp == TOK_STRING) 
						;
					else
						throw "String operand conversions error: unknown";
				}
				else if (iTyp1 == TOK_INTEGER)
					UnaryOp(iTokTyp, &iTyp1, &lDef1, lOp1);
				else 
					UnaryOp(iTokTyp, &iTyp1, &lDef1, dOp1);
			}
			else if (toktbl[iTokTyp].eClass == BinaryArithOp)
			{	// Binary arithmetic operator 
				if (iOpStkTop == 0) throw "Binary Arithmetic: Only one operand.";
				iTyp2 = iOpStkTyp[iOpStkTop];				// Pop second operand from operand stack
				lDef2 = lOpStkDef[iOpStkTop];
				iLen2 = HIWORD(lDef2);
				memcpy(cOp2, &lOpStk[iOpStkTop--][0], iLen2 * 4);
				iTyp = Min(iTyp2, TOK_REAL);
				if (iTyp1 < iTyp)						// Convert first operand
				{
					ConvertValTyp(iTyp1, iTyp, &lDef1, lOp1);
					iTyp1 = iTyp;
					iLen1 = HIWORD(lDef1);
				}
				else 
				{
					iTyp = Min(iTyp1, TOK_REAL);
					if (iTyp2 < iTyp)					// Convert second operand
					{
						ConvertValTyp(iTyp2, iTyp, &lDef2, lOp2);
						iTyp2 = iTyp;
						iLen2 = HIWORD(lDef2);
					}
				}
				if (iTokTyp == TOK_BINARY_PLUS) 
				{
					if (iTyp1 == TOK_STRING) 
					{
						iDim1 = LOWORD(lDef1);
						iDim2 = LOWORD(lDef2);
						iDim = iDim2 + iDim1;
						strcpy(cOp1, strcat(cOp2, cOp1));
						iLen1 = 1 + (iDim - 1) / 4;
						lDef1 = MAKELONG(iDim, iLen1);
					}
					else 
					{
						if (iTyp1 == TOK_INTEGER) 
							lOp1[0] += lOp2[0];
						else 
							dOp1[0] += dOp2[0];
					}
				}
				else if (iTokTyp == TOK_BINARY_MINUS) 
				{
					if (iTyp1 == TOK_STRING) throw "Can not subtract strings";
					if (iTyp1 == TOK_INTEGER) 
						lOp1[0] = lOp2[0] - lOp1[0];
					else 
						dOp1[0] = dOp2[0] - dOp1[0];
				}
				else if (iTokTyp == TOK_MULTIPLY) 
				{
					if (iTyp1 == TOK_STRING) throw "Can not mutiply strings";
					if (iTyp1 == TOK_INTEGER) 
						lOp1[0] *= lOp2[0];
					else 
					{
						if (iTyp1 == TOK_REAL) 
							iTyp1 = iTyp2;
						else if (iTyp2 == TOK_REAL) 
							;
						else if (iTyp1 == TOK_LENGTH_OPERAND && iTyp2 == TOK_LENGTH_OPERAND) 
							iTyp1 = TOK_AREA_OPERAND;
						else
							throw "Invalid mix of multiplicands";
						
						dOp1[0] *= dOp2[0];
					}
				}
				else if (iTokTyp == TOK_DIVIDE) 
				{
					if (iTyp1 == TOK_STRING) throw "Can not divide strings";
					if (iTyp1 == TOK_INTEGER) 
					{
						if (lOp1[0] == 0) throw "Attempting to divide by 0";
						lOp1[0] = lOp2[0] / lOp1[0];
					}
					else if (iTyp1 <= iTyp2) 
					{
						if (dOp1[0] == 0.) throw "Attempting to divide by 0.";
						if (iTyp1 == iTyp2) 
							iTyp1 = TOK_REAL;
						else if (iTyp1 == TOK_REAL) 
							iTyp1 = iTyp2;
						else 
							iTyp1 = TOK_LENGTH_OPERAND;
						dOp1[0] = dOp2[0] / dOp1[0];
					}
					else
						throw "Division type error";
				}
				else if (iTokTyp == TOK_EXPONENTIATE) 
				{
					if (iTyp1 == TOK_INTEGER) 
					{
						if ((lOp1[0] >= 0 && lOp1[0] > DBL_MAX_10_EXP) || (lOp1[0] < 0 && lOp1[0] < DBL_MIN_10_EXP))
							throw "Exponentiation error";
						
						lOp1[0] = (int) pow((long double)lOp2[0], (long double)lOp1[0]);
					}	
					else if (iTyp1 == TOK_REAL) 
					{
						int iExp = (int) dOp1[0];
												
						if ((iExp >= 0 && iExp > DBL_MAX_10_EXP) || (iExp < 0 && iExp < DBL_MIN_10_EXP))
							throw "Exponentiation error";
						dOp1[0] = pow(dOp2[0], dOp1[0]);
					}
				}
			}
			else if (toktbl[iTokTyp].eClass == BinaryRelatOp) 
			{
				// if support for binary relational operators desired (== != > >= < <=)
				throw "Binary relational operators not implemented";
			}
			else if (toktbl[iTokTyp].eClass == BinaryLogicOp) 
			{
				// if support for binary logical operators desired (& |)
				throw "Binary logical operators not implemented";
			}
			else if (toktbl[iTokTyp].eClass == UnaryLogicOp) 
			{
				// if support for unary logical operator desired (!)
				throw "Unary logical operator not implemented";
			}
		}
		iOpStkTop++;						// Increment opernad stack pointer
		iOpStkTyp[iOpStkTop] = iTyp1;		// Push operand onto operand stack
		lOpStkDef[iOpStkTop] = lDef1;
		memcpy(&lOpStk[iOpStkTop][0], cOp1, HIWORD(lDef1) * 4);
		iTokStkId++;
	}
	*aiTyp = iTyp1;
	*alDef = lDef1;
}

void lex::Init()
{
	lex::iToks = 0;
	iValsCount = 0;
}

///<summary>Parses line into tokens.</summary>
void lex::Parse(const char* szLine)
{
	iToks = 0;
	iValsCount = 0;
	
	char szTok[256];
	
	int iBeg = 0;
	int iLnLen = (int) strlen(szLine);
	
	while (iBeg < iLnLen) 
	{
		int iTyp = Scan(szTok, szLine, iBeg);
		
		if (iTyp == - 1) return;
		if (iToks == TOKS_MAX) return;

		iTokenType[iToks] = iTyp;
		int iLen = (int) strlen(szTok);
		int iDim;
		double dVal;

		switch (iTyp)
		{
			case TOK_IDENTIFIER:
				iDim = (int) strlen(szTok);
				iLen = 1 + (iDim - 1) /  4;
			
				iValLoc[iToks] = iValsCount + 1;
				lValues[iValsCount + 1] = iDim + iLen * 65536;
				memcpy(&lValues[iValsCount + 2], szTok, iDim);
				iValsCount = iValsCount + 1 + iLen;
				break;
		
			case TOK_STRING: 
				ParseStringOperand(szTok);
				break;

			case TOK_INTEGER:
				iValLoc[iToks] = iValsCount;
				lValues[iValsCount++] = MAKELONG(1, 1);
				lValues[iValsCount++] = atoi(szTok);
				break;
		
			case TOK_REAL:
			case TOK_LENGTH_OPERAND:
				dVal = (iTyp == TOK_REAL) ? atof(szTok) : UnitsString_ParseLength(szTok);
			
				iValLoc[iToks] = iValsCount;
				lValues[iValsCount++] = MAKELONG(1, 2);
				memcpy(&lValues[iValsCount++], &dVal, sizeof(double));
				iValsCount++;
				break;
		}
		iToks++;
	}
}

void lex::ParseStringOperand(const char* pszTok)
{
	if (strlen(pszTok) < 3) {msgInformation(IDS_MSG_ZERO_LENGTH_STRING); return;}
	
	char* pszValues = (char*) &lValues[iValsCount + 2];
	
	int iDim = 0;
	int iNxt = 1;
	while (pszTok[iNxt] != '\0') 
	{
		if (pszTok[iNxt] == '"' && pszTok[iNxt + 1] == '"') iNxt++;
		pszValues[iDim++] = pszTok[iNxt++];
	}
	pszValues[--iDim] = '\0';
	int iLen = 1 + (iDim - 1) /  4;
	iValLoc[iToks] = ++iValsCount;
	lValues[iValsCount] = MAKELONG(iDim, iLen);
	iValsCount += iLen;
}

int lex::Scan(char* aszTok, const char* szLine, int& iLP)
{
	int iLen;

	while (szLine[iLP] == ' ') {iLP++;}
	
	int iBegLoc = iLP;
	int iTokLoc = iLP;
	int iRetVal = - 1;
	int iS = 1;
	
	bool bDone = false;
	while (!bDone) 
	{
		int iAddr = iBase[iS] + szLine[iLP];
		
		if (iCheck[iAddr] == iS)
		{
			iS = iNext[iAddr];
			if (iTokVal[iS] != 0) 
			{
				iRetVal = iTokVal[iS];
				iTokLoc = iLP;
			}
			iLP++;
		}
		else if (iDefault[iS] != 0)
			iS = iDefault[iS];
		else
			bDone = true;
	}
	
	iLen = iTokLoc - iBegLoc + 1;
	strncpy(aszTok, &szLine[iBegLoc], iLen);
	aszTok[iLen] = '\0';
	TRACE("LinePointer = %d, TokenID = %d\n", iLP, iRetVal);	
	if (iRetVal == - 1) {iLP = iBegLoc + 1;}
	return (iRetVal);
}

///<summary> Fetches specified tokens type from current token stream.</summary>
// Returns:  token type
//		 - 1 if token identifier out of range
int lex::TokType(int aiTokId)
{
	return (aiTokId >= 0 && aiTokId < lex::iToks) ? iTokenType[aiTokId] : - 1;
}

void lex::UnaryOp(int aiTokTyp, int* aiTyp, long* alDef, double* adOp)
{
	CD		cd;
	char	szTok[32];
	int 	i;
	
	int iDim = LOWORD(*alDef);
	int iLen = HIWORD(*alDef);
	
	switch (aiTokTyp)
	{
		case TOK_UNARY_MINUS:
			for (i = 0; i < iLen / 2; i++)
				adOp[i] = - adOp[i];
			break;
		
		case TOK_UNARY_PLUS:
			break;
	
		case TOK_ABS: 
			adOp[0] = fabs(adOp[0]);
			break;
		
		case TOK_ACOS: 
			if (fabs(adOp[0]) > 1.) 
				throw "Math error: acos of a value greater than 1.";
			else
				adOp[0] = acos(adOp[0]) / RADIAN;
			break;
	
		case TOK_ASIN: 
			if (fabs(adOp[0]) > 1.)
				throw "Math error: asin of a value greater than 1.";
			else
				adOp[0] = asin(adOp[0]) / RADIAN;
			break;
	
		case TOK_ATAN:
			adOp[0] = atan(adOp[0]) / RADIAN;
			break;
			
		case TOK_COS:
			adOp[0] = cos(adOp[0] * RADIAN);
			break;
		
		case TOK_TOREAL: 
			break;
	
		case TOK_EXP: 
			adOp[0] = exp(adOp[0]);
			break;
			
		case TOK_TOINTEGER:		// Conversion to integer
			ConvertValTyp(TOK_REAL, TOK_INTEGER, alDef, (void*) adOp);
			*aiTyp = TOK_INTEGER;
			break;
	
		case TOK_LN: 
			if (adOp[0] <= 0.)
				throw "Math error: ln of a non-positive number";
			else
				adOp[0] = log(adOp[0]);
			break;
	
		case TOK_LOG: 
			if (adOp[0] <= 0.) 
				throw "Math error: log of a non-positive number";
			else
				adOp[0] = log10(adOp[0]);
			break;
		
		case TOK_SIN:
			adOp[0] = sin(adOp[0] * RADIAN);
			break;
			
		case TOK_SQRT: 
			if (adOp[0] < 0.)
				throw "Math error: sqrt of a negative number";
			else
				adOp[0] = sqrt(adOp[0]);
			break;
		
		case TOK_TAN:
			adOp[0] = tan(adOp[0] * RADIAN);
			break;
			
		case TOK_TOSTRING:	// Conversion to string
			*aiTyp = TOK_STRING;
			cd.lTyp = TOK_REAL;
			cd.lDef = *alDef;
			ConvertValToString((char*) adOp, &cd, szTok, &iDim);
			iLen = 1 + (iDim - 1) / 4;
			strcpy((char*) adOp, szTok);
			*alDef = MAKELONG(iDim, iLen);
			break;
		
		default:
			throw "Unknown operation";
	}
}

void lex::UnaryOp(int aiTokTyp, int* aiTyp, long* alDef, long* alOp)
{
	CD		cd;
	char	szTok[32];
	
	int iDim = LOWORD(*alDef);
	int iLen = HIWORD(*alDef);
	
	switch (aiTokTyp)
	{
		case TOK_UNARY_MINUS:
			alOp[0] = - alOp[0];
			break;
		
		case TOK_UNARY_PLUS:
			break;
				
		case TOK_ABS:
			alOp[0] = labs(alOp[0]);
			break;
				
		case TOK_TOINTEGER:
			break;
		
		case TOK_TOREAL:
			ConvertValTyp(TOK_INTEGER, TOK_REAL, alDef, (void*) alOp);
			*aiTyp = TOK_REAL;
			break;
			
		case TOK_TOSTRING:
			*aiTyp = TOK_STRING;
			cd.lTyp = TOK_INTEGER;
			cd.lDef = *alDef;
			ConvertValToString((char*) alOp, &cd, szTok, &iDim);
			iLen = 1 + (iDim - 1) / 4;
			strcpy((char*) alOp, szTok);
			*alDef = MAKELONG(iDim, iLen);
			break;
		
		default:
			throw "Unknown operation";
	}
}

///<summary>Scan a buffer for a given character.</summary>
// Notes:	If the character is found the scan pointer is updated
//			to point to the character following the one found.
// Returns: Pointer to the character if found,	0 if not.
LPSTR lex::ScanForChar(
	char c, 		// The character to look for.
	LPSTR *ppStr)	// Pointer to the current scan pointer.
{
	LPSTR p = lex::SkipWhiteSpace(*ppStr);
	
	if (*p == c) 
	{
		*ppStr = p + 1;
		return p;
	}
	return 0; // not found
}

///<summary>Skip over any white space characters.</summary>
// Returns: Pointer to the first non-white character.
LPSTR lex::SkipWhiteSpace(
	LPSTR pszString)	// Pointer to the current buffer position.
{
	while (pszString && *pszString && isspace(*pszString)) 
		pszString++;
	
	return pszString;
}

///<summary>Scan for a string.</summary>
// Notes:	The scan pointer is updated to point past the string.  The
//			arg buffer pointer is updated to point to the next free character.
// Returns: Pointer tot he string or 0 if an error occurs.
LPSTR lex::ScanForString(
	LPSTR *ppStr,		// Pointer to the current scan pointer.
	LPSTR pszTerm,		// Pointer to a location to receive the character which terminates the string.
	LPSTR *ppArgBuf)	// Pointer to the current arg buffer pointer.
{
	LPSTR pIn = lex::SkipWhiteSpace(*ppStr);
	LPSTR pStart = *ppArgBuf;
	LPSTR pOut = pStart;

	bool bInQuotes = *pIn == '"';
	
	if (bInQuotes)											
		pIn++;

	do
	{
		if (bInQuotes) 
		{
			if ((*pIn == '"') && (*(pIn + 1) != '"'))
			{	// Skip over the quote
				pIn++;	
				break;
			}
		} 
		else if (isalnum(*pIn))
			;
		else
		{	// allow some peg specials
			if (!(*pIn == '_' || *pIn == '$' || *pIn == '.' || *pIn == '-' || *pIn == ':' || *pIn == '\\'))
				break;
		}
		if ((*pIn == '"') && (*(pIn + 1) == '"'))
			// Skip the escaping first quote
			pIn++; 
		
		if (*pIn == '\\' && *(pIn + 1) == '\\')
			// Skip the escaping backslash
			pIn++; 
		
		*pOut++ = *pIn++;				// the char to the arg buffer

	} while(*pIn);

	*pOut++ = '\0'; 					// Set up the terminating char and update the scan pointer
	*pszTerm = *pIn;
	if (*pIn) 
		*ppStr = pIn+1;
	else 
		*ppStr = pIn;
	
	*ppArgBuf = pOut;					// Update the arg buffer to the next free bit
	
	return pStart;
}