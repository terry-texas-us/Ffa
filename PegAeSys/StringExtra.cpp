#include "stdafx.h"

using namespace std;

char* string_TrimLeadingSpace(char* szString)
{
	char* p = szString;
	
	while (p && *p && isspace(*p))
		p++;

	return p;
}		

///<summary>
///Determines the count of characters in string excluding embedded command characters.
///</summary>
int string_LengthSansEscSeqs(const CString& str)
{
	int nCount = str.GetLength();
	int n = 0;
	
	while (n < nCount && str[n] != '\0') 
	{
		int n2 = n + 1;
		
		if (str[n] == '^' && str[n2] == '/') 
		{
			while (str[n2] != '\0' && str[n2] != '^') 
				n2++;
			
			if (str[n2] == '^') 
			{	// End delimiter found .. Resume scan at character following end delimiter
				n = n2;
				nCount -= 4;
			}
		}
		n++;
	}
	return nCount;
}