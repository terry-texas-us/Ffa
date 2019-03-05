#pragma once

enum ETokClass {Other, Constant, Identifier, BinaryArithOp, BinaryRelatOp, BinaryLogicOp, UnaryLogicOp, AssignOp, OpenParen, CloseParen};
struct CD					// column definition
{
	long lDef;				// data definition
	long lTyp;				// data type
};
struct tokent
{
	int iInComPrio;
	int iInStkPrio;
	ETokClass eClass;
};

namespace lex
{
#include "LexTable.h"

	const int TOKS_MAX = 128;		// maximum number of tokens
	const int VALS_MAX = 256;
	
	const int TOK_UNARY_OPERATOR = 1;
	const int TOK_BINARY_OPERATOR = 2;
	const int TOK_COMPARISON_OPERATOR = 4;
	const int TOK_LOGICAL_OPERATOR = 8;

	const int TOK_ABS				= 1;
	const int TOK_ACOS				= 2;
	const int TOK_ASIN				= 3;
	const int TOK_ATAN				= 4;
	const int TOK_TOSTRING			= 5;
	const int TOK_COS				= 6;
	const int TOK_EXP				= 7;
	const int TOK_TOINTEGER			= 8;
	const int TOK_LN				= 9;
	const int TOK_LOG				= 10;
	const int TOK_SIN				= 11;
	const int TOK_SQRT				= 12;
	const int TOK_TAN				= 13;
	const int TOK_TOREAL			= 14;
	const int TOK_UNARY_PLUS		= 15;
	const int TOK_UNARY_MINUS		= 16;

	const int TOK_INTEGER			= 20;
	const int TOK_REAL				= 21;
	const int TOK_LENGTH_OPERAND	= 22;
	const int TOK_AREA_OPERAND		= 23;
	const int TOK_STRING			= 24;
	const int TOK_IDENTIFIER		= 25;
	const int TOK_EXPONENTIATE		= 26;
	const int TOK_MULTIPLY			= 27;
	const int TOK_DIVIDE			= 28;
	const int TOK_BINARY_PLUS		= 29;
	const int TOK_BINARY_MINUS		= 30;
	const int TOK_EQ				= 31;
	const int TOK_NE				= 32;
	const int TOK_GT				= 33;
	const int TOK_GE				= 34;
	const int TOK_LT				= 35;
	const int TOK_LE				= 36;
	const int TOK_AND				= 37;
	const int TOK_OR				= 38;
	const int TOK_NOT				= 39;
	const int TOK_LPAREN			= 40;
	const int TOK_RPAREN			= 41;
		
	static tokent toktbl[] = 
	{
		{0,		0,		Other},			// unused
		{110,	85,		Other},			// abs
		{110,	85,		Other},			// acos
		{110,	85,		Other},			// asin
		{110,	85,		Other},			// atan
		{110,	85,		Other},			// string
		{110,	85,		Other},			// cos
		{110,	85,		Other},			// exp
		{110,	85,		Other},			// int
		{110,	85,		Other},			// ln
		{110,	85,		Other},			// log
		{110,	85,		Other},			// sin
		{110,	85,		Other},			// sqrt
		{110,	85,		Other},			// tan
		{110,	85,		Other},			// real
		{110,	85,		Other},			// unary+
		{110,	85,		Other},			// unary-
		{0,		0,		Other},			// unused
		{0,		0,		Other},			// unused
		{0,		0,		Other},			// unused
		{0,		0,		Constant},		// integer
		{0,		0,		Constant},		// real
		{0,		0,		Constant},		// length
		{0,		0,		Constant},		// area
		{0,		0,		Constant},		// string
		{0,		0,		Identifier},	// identifier
		{80,	79,		BinaryArithOp},	// **
		{70,	71,		BinaryArithOp},	// *
		{70,	71,		BinaryArithOp},	// /
		{60,	61,		BinaryArithOp},	// +
		{60,	61,		BinaryArithOp},	// -
		{40,	41,		BinaryRelatOp},	// ==
		{40,	41,		BinaryRelatOp},	// !=
		{40,	41,		BinaryRelatOp},	// >
		{40,	41,		BinaryRelatOp},	// >=
		{40,	41,		BinaryRelatOp},	// <
		{40,	41,		BinaryRelatOp},	// <=
		{20,	21,		BinaryLogicOp},	// &
		{10,	11,		BinaryLogicOp},	// |
		{30,	31,		UnaryLogicOp},	// !
		{110,	1,		OpenParen},		// (
		{0,		0,		CloseParen}		// )
	};

	void BreakExpression(int& iLoc, int& iEnd, int* aiTyp, int* aiTokLoc);
	void ConvertStringToVal(int iTyp, long lDef, char* szVal, long* lDefReq, void* p);
	void ConvertValToString(char*, CD*, char*, int*);
	void ConvertValTyp(int, int, long*, void*);		
	void EvalTokenStream(char*, int*, long*, int*, void*);
	void Init();
	void Parse(const char* pszLine);
	void ParseStringOperand(const char* pszTok);
	LPSTR ScanForChar(char c, LPSTR *ppStr);
	LPSTR ScanForString(LPSTR *ppStr, LPSTR pszTerm, LPSTR *ppArgBuf);
	int Scan(char* aszTok, const char* pszLine, int& iLP);
	LPSTR SkipWhiteSpace(LPSTR pszString);
	int TokType(int);
	void UnaryOp(int, int*, long*,	double*);
	void UnaryOp(int, int*, long*, long*);
}
