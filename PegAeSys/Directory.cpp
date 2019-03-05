#include "stdafx.h"

void Path_UnquoteSpaces(CString& strPathName)
{
	char* pPathName = new char[MAX_PATH];
	strcpy(pPathName, strPathName);
	int n = strlen(pPathName) - 1;
	while (n != 0 && pPathName[n] != '\\')
		n--;
	pPathName[n] = 0;
    strPathName = &pPathName[1];
	delete [] pPathName;
}

//void Directory_ExamineFile(char* oldfile, char* newfile)
//{
//	//// the default file examine -- if the file isn't pathed to the right place,
//	//// then search the ACAD environment variable for the oldfile */
// // 
//	//const int kExistenceOnly = 0;
//	//
//	//char holdch;
//	//char testpath[256];
//	//  
//	//static char pathchar='\\';
//	//static char oldpathchar='/';
//
//	//// replace path characters, if present, with proper ones for platform
//	//for (int i = 0; i < int(strlen(oldfile)); i++)
//	//{
//	//	if (oldfile[i] == oldpathchar) oldfile[i] = pathchar;
//	//}
//	//if (strchr(oldfile, pathchar) != 0) 
//	//{	// file has some degree of path designation
//	//	if (_access(oldfile, kExistenceOnly) == 0) 
//	//	{
//	//		strcpy(newfile, oldfile);
//	//		return;
//	//	}
//	//	// strip the path
//	//	char* pLast = strrchr(oldfile, pathchar);
//	//	strcpy(oldfile, ++pLast);
//	//}
//	//strcpy(newfile, oldfile);
//
//	//if (_access(oldfile, kExistenceOnly) == 0) 
//	//{	// its in current diretory
//	//	return;
//	//}
//	//else 
//	//{
//	//	if (strchr(oldfile, ':') != 0) 
//	//	{	// path with drive id
//	//		return;
//	//	}
//	//    char* envptr = getenv("ACAD");
//	//	if (envptr == 0) 
//	//	{	// no ACAD environment to search
//	//		return;
//	//	}
//	//	char* cptr = envptr;
//	//	do 
//	//	{
//	//		while (*cptr != ';' && *cptr != 0) 
//	//		{
//	//			cptr++;
//	//		}
//	//		holdch = *cptr;             /* grab terminating character */
//	//		*cptr = 0;                    /* null it out for strcpy */
//	//		strcpy(testpath, envptr);
//	//		*cptr = holdch;               /* put it back */
//	//		int iTest = (int) strlen(testpath);
//	//		if (testpath[iTest - 1] != pathchar) 
//	//		{  /* append / or \ */
//	//			testpath[iTest] = pathchar;
//	//			testpath[iTest + 1] = 0;
//	//		}
//	//		strcat(testpath, oldfile);
//
//	//		if (!_access(testpath, 0)) 
//	//		{  /* true if found */
//	//			strcpy(newfile, testpath);
//	//			return;
//	//		}
//	//		cptr++;
//	//		envptr = cptr;
//	//	} 
//	//	while (holdch == ';');        /* terminator is 0 for end of env string */
//	//}
//}