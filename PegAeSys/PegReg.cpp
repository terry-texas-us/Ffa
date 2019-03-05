#include "stdafx.h"

#include "PegAEsys.h"

CPegReg::CPegReg()
{
	Initialize();

	// get application path
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(app.GetInstance(), szPath, MAX_PATH);
	s3 = szPath;
	s3 += " %1";

	// get icon path
	TCHAR szPath2[MAX_PATH];
	char *pStr;
	GetModuleFileName(app.GetInstance(), szPath2, MAX_PATH);
	pStr = strrchr(szPath2, '\\');
	if (pStr != NULL)
	*(++pStr)='\0';
	s4 = szPath2;
	s4 += "\\PegAEsys.ico";
}

void CPegReg::Initialize()
{
	iElements = 0;

	DWORD dwDisp = 0;
	LPDWORD lpdwDisp = &dwDisp;
	l_strExampleKey = "SOFTWARE\\Fanning\\MRU";

	TCHAR szData[1024];
	DWORD buffSize = 1024;
	CString firstRun = "FALSE";

	// create and/or open key
	iSuccess = RegCreateKeyEx(HKEY_CURRENT_USER, l_strExampleKey, 0L,
								NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 
								NULL, &hKey, lpdwDisp);

	LONG lResult = RegQueryValueEx(hKey, "FirstRun", NULL, NULL, (LPBYTE)szData, &buffSize);
	if(lResult != ERROR_SUCCESS)
	{
		RegSetValueEx(hKey, "FirstRun", 0, REG_SZ, (LPBYTE)firstRun.GetBuffer(firstRun.GetLength()), firstRun.GetLength() + 1);

		mruList = "abcde";
		a = "File";
		b = "File";
		c = "File";
		d = "File";
		e = "File";
		
		// set initial values in registry
		RegSetValueEx(hKey, "a", 0, REG_SZ, (LPBYTE)a.GetBuffer(a.GetLength()), a.GetLength() + 1);
		RegSetValueEx(hKey, "b", 0, REG_SZ, (LPBYTE)b.GetBuffer(b.GetLength()), b.GetLength() + 1);
		RegSetValueEx(hKey, "c", 0, REG_SZ, (LPBYTE)c.GetBuffer(c.GetLength()), c.GetLength() + 1);
		RegSetValueEx(hKey, "d", 0, REG_SZ, (LPBYTE)d.GetBuffer(d.GetLength()), d.GetLength() + 1);
		RegSetValueEx(hKey, "e", 0, REG_SZ, (LPBYTE)e.GetBuffer(e.GetLength()), e.GetLength() + 1);
		RegSetValueEx(hKey, "mruList", 0, REG_SZ, (LPBYTE)mruList.GetBuffer(mruList.GetLength()), mruList.GetLength() + 1);
	}

	LoadValues();
}

void CPegReg::PriorityUp(CString filename)
{
	LoadValues();

	filename = filename.Right(filename.GetLength() - filename.ReverseFind('\\') - 1);

	char file;
	if(a == filename)
		file = 'a';
	else if(b == filename)
		file = 'b';
	else if(c == filename)
		file = 'c';
	else if(d == filename)
		file = 'd';
	else if(e == filename)
		file = 'e';
	else
		return;

	CString list = mruList;

	int i = 0;
	while(list[i] != file)
		i++;

	for(int j = i; j > 0; j--)
		list.SetAt(j, list[j-1]);

	list.SetAt(0, file);

	mruList = list;

	if(iSuccess == ERROR_SUCCESS)
		RegSetValueEx(hKey, "mruList", 0, REG_SZ, (LPBYTE) mruList.GetBuffer(mruList.GetLength()), mruList.GetLength()+ 1);

	LoadValues();
}

bool CPegReg::Find(CString filename)
{
	CString pathname = filename;
	filename = filename.Right(filename.GetLength() - filename.ReverseFind('\\') - 1);

	char chMatch = 'x';

	// see if filenames match
	if(a == filename)
		chMatch = 'a';
	else if(b == filename)
		chMatch = 'b';
	else if(c == filename) 
		chMatch = 'c';
	else if(d == filename)
		chMatch = 'd';
	else if(e == filename)
		chMatch = 'e';
	else
		return false;

	// if filenames match, check if pathnames also match
	TCHAR szData[1024];
	DWORD buffSize = 1024;
	HKEY hKey;
	RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\MRU", 0, KEY_ALL_ACCESS, &hKey);

	switch(chMatch)
	{
		case 'a':				
			RegQueryValueEx(hKey, "a", NULL, NULL, (LPBYTE)szData, &buffSize);
			break;
		case 'b':
			RegQueryValueEx(hKey, "b", NULL, NULL, (LPBYTE)szData, &buffSize);
			break;
		case 'c':
			RegQueryValueEx(hKey, "c", NULL, NULL, (LPBYTE)szData, &buffSize);
			break;
		case 'd':
			RegQueryValueEx(hKey, "d", NULL, NULL, (LPBYTE)szData, &buffSize);
			break;
		case 'e':
			RegQueryValueEx(hKey, "e", NULL, NULL, (LPBYTE)szData, &buffSize);
			break;
	}
	CString strPathName = szData;
	RegCloseKey(hKey);

	if(strPathName == pathname)
		return true;

	return false;

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

	//filename = filename.Right(filename.GetLength() - filename.ReverseFind('\\') - 1);

	//if((a == filename) || (b == filename) || (c == filename) ||
	//   (d == filename) || (e == filename))
	//   return true;

	//return false;
}

void CPegReg::SetKey(const CString filename)
{
	// return if solo file
	if(filename.Find("__MaKtEmP.peg") != -1)
		return;

	if(filename.Find("__MaKtEmP - layer.jb1") != -1)
		return;

	if(filename.Find("__MaKtEmP - block.peg") != -1)
		return;

	if(filename.Find("(Recovered)") != -1)
		return;

	// reset registry keys if previously cleaned
	HKEY initKey;
	LRESULT lResult = RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\MRU", 0, KEY_ALL_ACCESS, &initKey);
	if(lResult != ERROR_SUCCESS)
	{
		Initialize();
	}
	RegCloseKey(initKey);

	// main part of function
	TCHAR szData[1024];
	DWORD buffSize = 1024;
	CString current;
	CString letterfile;

	if(Find(filename))
	{
		PriorityUp(filename);
		return;
	}

	char list[6];

	RegQueryValueEx(hKey, "mruList", NULL, NULL, (LPBYTE)szData, &buffSize);
	list[4] = szData[3];
	list[3] = szData[2];
	list[2] = szData[1];
	list[1] = szData[0];
	list[0] = szData[4];

	letterfile = list[0];

	if(list[0] == 'a')
	{
		a = filename;
		current = a;
	}
	else if(list[0] == 'b')
	{
		b = filename;
		current = b;
	}
	else if(list[0] == 'c')
	{
		c = filename;
		current = c;
	}
	else if(list[0] == 'd')
	{
		d = filename;
		current = d;
	}
	else if(list[0] == 'e')
	{
		e = filename;
		current = e;
	}

	mruList = list;

	if(iSuccess == ERROR_SUCCESS)
	{
		RegSetValueEx(hKey, "mruList", 0, REG_SZ, (LPBYTE) mruList.GetBuffer(mruList.GetLength()), mruList.GetLength()+ 1);
		RegSetValueEx(hKey, letterfile, 0, REG_SZ, (LPBYTE) current.GetBuffer(current.GetLength()), current.GetLength()+ 1);
	}

	LoadValues();
}

CString CPegReg::GetKeyValue()
{
	return mruList;
}

void CPegReg::LoadValues()
{
	iElements = 0;

	LONG lResult;
	TCHAR szDatalist[1024];
	TCHAR szDataa[1024];
	TCHAR szDatab[1024];
	TCHAR szDatac[1024];
	TCHAR szDatad[1024];
	TCHAR szDatae[1024];
	DWORD buffSize = 1024;
	HKEY hKeya;
	HKEY hKeyb;
	HKEY hKeyc;
	HKEY hKeyd;
	HKEY hKeye;
	HKEY hKeylist;

	buffSize = 1024;
	lResult = RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\MRU", 0, KEY_ALL_ACCESS, &hKeya);
	if(lResult == ERROR_SUCCESS)
	{
		RegQueryValueEx(hKeya, "a", NULL, NULL, (LPBYTE)szDataa, &buffSize);
		RegCloseKey(hKeya);

		a = szDataa;
		a = a.Right(a.GetLength() - a.ReverseFind('\\') - 1);

		if(a != "File")
			iElements++;
	}

	buffSize = 1024;
	lResult = RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\MRU", 0, KEY_ALL_ACCESS, &hKeyb);
	if(lResult == ERROR_SUCCESS)
	{
		RegQueryValueEx(hKeyb, "b", NULL, NULL, (LPBYTE)szDatab, &buffSize);
		RegCloseKey(hKeyb);
		
		b = szDatab;
		b = b.Right(b.GetLength() - b.ReverseFind('\\') - 1);

		if(b != "File")
			iElements++;
	}

	buffSize = 1024;
	lResult = RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\MRU", 0, KEY_ALL_ACCESS, &hKeyc);
	if(lResult == ERROR_SUCCESS)
	{
		RegQueryValueEx(hKeyc, "c", NULL, NULL, (LPBYTE)szDatac, &buffSize);
		RegCloseKey(hKeyc);

		c = szDatac;
		c = c.Right(c.GetLength() - c.ReverseFind('\\') - 1);

		if(c != "File")
			iElements++;
	}

	buffSize = 1024;
	lResult = RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\MRU", 0, KEY_ALL_ACCESS, &hKeyd);
	if(lResult == ERROR_SUCCESS)
	{
		RegQueryValueEx(hKeyd, "d", NULL, NULL, (LPBYTE)szDatad, &buffSize);
		RegCloseKey(hKeyd);
		
		d = szDatad;
		d = d.Right(d.GetLength() - d.ReverseFind('\\') - 1);

		if(d != "File")
			iElements++;
	}

	buffSize = 1024;
	lResult = RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\MRU", 0, KEY_ALL_ACCESS, &hKeye);
	if(lResult == ERROR_SUCCESS)
	{
		RegQueryValueEx(hKeye, "e", NULL, NULL, (LPBYTE)szDatae, &buffSize);
		RegCloseKey(hKeye);

		e = szDatae;
		e = e.Right(e.GetLength() - e.ReverseFind('\\') - 1);

		if(e != "File")
			iElements++;
	}

	buffSize = 1024;
	lResult = RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\MRU", 0, KEY_ALL_ACCESS, &hKeylist);
	if(lResult == ERROR_SUCCESS)
	{
		RegQueryValueEx(hKeylist, "mruList", NULL, NULL, (LPBYTE)szDatalist, &buffSize);
		RegCloseKey(hKeylist);
		mruList = szDatalist;
	}
}

void CPegReg::DisplayMRU()
{
	UINT menuLoc = ID_FILE_MRU1;

	MENUITEMINFO minfo;
	minfo.cbSize = sizeof(MENUITEMINFO);
	minfo.fMask = MIIM_ID;

	HMENU submenu = app.GetSubMenu(0);

	::RemoveMenu(app.GetMenu(), ID_FILE_MRU1, MF_BYCOMMAND);
	::RemoveMenu(app.GetMenu(), ID_FILE_MRU2, MF_BYCOMMAND);
	::RemoveMenu(app.GetMenu(), ID_FILE_MRU3, MF_BYCOMMAND);
	::RemoveMenu(app.GetMenu(), ID_FILE_MRU4, MF_BYCOMMAND);
	::RemoveMenu(app.GetMenu(), ID_FILE_MRU5, MF_BYCOMMAND);

	for(int i = 0; i < iElements; i++)
	{
		CString number;
		switch(i)
		{
			case 0:		number = "&1 "; break;
			case 1:		number = "&2 "; break;
			case 2:		number = "&3 "; break;
			case 3:		number = "&4 "; break;
			case 4:		number = "&5 "; break;
			default:	number = "&0 "; break;
		}

		minfo.wID = menuLoc + i;
		::InsertMenuItem(submenu , 16+i, TRUE, &minfo);

		if(mruList[i] == 'a')
			::ModifyMenu(app.GetMenu(), menuLoc + i, MF_BYCOMMAND | MF_STRING, menuLoc + i, number + a); 
		else if(mruList[i] == 'b')
			::ModifyMenu(app.GetMenu(), menuLoc + i, MF_BYCOMMAND | MF_STRING, menuLoc + i, number + b);
		else if(mruList[i] == 'c')
			::ModifyMenu(app.GetMenu(), menuLoc + i, MF_BYCOMMAND | MF_STRING, menuLoc + i, number + c);
		else if(mruList[i] == 'd')
			::ModifyMenu(app.GetMenu(), menuLoc + i, MF_BYCOMMAND | MF_STRING, menuLoc + i, number + d);
		else if(mruList[i] == 'e')
			::ModifyMenu(app.GetMenu(), menuLoc + i, MF_BYCOMMAND | MF_STRING, menuLoc + i, number + e);
	}

	::DrawMenuBar(app.GetSafeHwnd());
}

CString CPegReg::GetItemFilePath(int nIndex)
{
		TCHAR szData[1024];
		DWORD buffSize = 1024;
		HKEY hKey;
		RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\MRU", 0, KEY_ALL_ACCESS, &hKey);
		
		CString mruList = GetKeyValue();
		char item = mruList[nIndex];

		switch(item)
		{
			case 'a':				
				RegQueryValueEx(hKey, "a", NULL, NULL, (LPBYTE)szData, &buffSize);
				break;
			case 'b':
				RegQueryValueEx(hKey, "b", NULL, NULL, (LPBYTE)szData, &buffSize);
				break;
			case 'c':
				RegQueryValueEx(hKey, "c", NULL, NULL, (LPBYTE)szData, &buffSize);
				break;
			case 'd':
				RegQueryValueEx(hKey, "d", NULL, NULL, (LPBYTE)szData, &buffSize);
				break;
			case 'e':
				RegQueryValueEx(hKey, "e", NULL, NULL, (LPBYTE)szData, &buffSize);
				break;
		}
		CString strPathName = szData;
		RegCloseKey(hKey);

		return strPathName;
}

CString CPegReg::GetItemPath(int nIndex)
{
	CString strPathName = GetItemFilePath(nIndex);

	int nLastOcc = strPathName.ReverseFind('\\');
	return strPathName.Mid(0, nLastOcc);
}

// file association
void CPegReg::SetPeg()
{
	HKEY fKey;
	DWORD dwDisp = 0;
	LPDWORD lpdwDisp = &dwDisp;

	CString s1 = "PegAEsys.peg";
	CString s2 = "PegAEsys Peg File";

	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, ".peg", 0L, NULL, REG_OPTION_NON_VOLATILE,
		              KEY_ALL_ACCESS, NULL, &fKey, lpdwDisp) == ERROR_SUCCESS)
	{
		RegSetValueEx(fKey, "", 0, REG_SZ, (LPBYTE)s1.GetBuffer(s1.GetLength()), 
					  s1.GetLength() + 1);
		RegCloseKey(fKey);
	}

	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, s1, 0L, NULL, REG_OPTION_NON_VOLATILE, 
					  KEY_ALL_ACCESS, NULL, &fKey, lpdwDisp) == ERROR_SUCCESS)
	{
		RegSetValueEx(fKey, "", 0, REG_SZ, (LPBYTE)s2.GetBuffer(s2.GetLength()), 
					  s2.GetLength() + 1);
		RegCloseKey(fKey);
	}

	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, "PegAEsys.peg\\Shell\\Open\\command", 0L, 
					  NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &fKey, 
					  lpdwDisp) == ERROR_SUCCESS)
	{
		RegSetValueEx(fKey, "", 0, REG_SZ, (LPBYTE)s3.GetBuffer(s3.GetLength()), 
					  s3.GetLength() + 1);
		RegCloseKey(fKey);
	}

	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, "PegAEsys.peg\\DefaultIcon", 0L, NULL, 
					  REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &fKey, 
					  lpdwDisp) == ERROR_SUCCESS)
	{
		RegSetValueEx(fKey, "", 0, REG_SZ, (LPBYTE)s4.GetBuffer(s4.GetLength()), 
					  s4.GetLength() + 1);
		RegCloseKey(fKey);
	}
}

void CPegReg::SetJB1()
{
	HKEY fKey;
	DWORD dwDisp = 0;
	LPDWORD lpdwDisp = &dwDisp;

	CString s1 = "PegAEsys.jb1";
	CString s2 = "PegAEsys Job File";

	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, ".jb1", 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &fKey, lpdwDisp) == ERROR_SUCCESS)
	{
		RegSetValueEx(fKey, "", 0, REG_SZ, (LPBYTE)s1.GetBuffer(s1.GetLength()), s1.GetLength() + 1);
		RegCloseKey(fKey);
	}

	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, s1, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &fKey, lpdwDisp) == ERROR_SUCCESS)
	{
		RegSetValueEx(fKey, "", 0, REG_SZ, (LPBYTE)s2.GetBuffer(s2.GetLength()), s2.GetLength() + 1);
		RegCloseKey(fKey);
	}

	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, "PegAEsys.jb1\\Shell\\Open\\command", 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &fKey, lpdwDisp) == ERROR_SUCCESS)
	{
		RegSetValueEx(fKey, "", 0, REG_SZ, (LPBYTE)s3.GetBuffer(s3.GetLength()), s3.GetLength() + 1);
		RegCloseKey(fKey);
	}

	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, "PegAEsys.jb1\\DefaultIcon", 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &fKey, lpdwDisp) == ERROR_SUCCESS)
	{
		RegSetValueEx(fKey, "", 0, REG_SZ, (LPBYTE)s4.GetBuffer(s4.GetLength()), s4.GetLength() + 1);
		RegCloseKey(fKey);
	}
}

void CPegReg::SetTra()
{
	HKEY fKey;
	DWORD dwDisp = 0;
	LPDWORD lpdwDisp = &dwDisp;

	CString s1 = "PegAEsys.tra";
	CString s2 = "PegAEsys Tra File";

	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, ".tra", 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &fKey, lpdwDisp) == ERROR_SUCCESS)
	{
		RegSetValueEx(fKey, "", 0, REG_SZ, (LPBYTE)s1.GetBuffer(s1.GetLength()), s1.GetLength() + 1);
		RegCloseKey(fKey);
	}

	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, s1, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &fKey, lpdwDisp) == ERROR_SUCCESS)
	{
		RegSetValueEx(fKey, "", 0, REG_SZ, (LPBYTE)s2.GetBuffer(s2.GetLength()), s2.GetLength() + 1);
		RegCloseKey(fKey);
	}

	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, "PegAEsys.tra\\Shell\\Open\\command", 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &fKey, lpdwDisp) == ERROR_SUCCESS)
	{
		RegSetValueEx(fKey, "", 0, REG_SZ, (LPBYTE)s3.GetBuffer(s3.GetLength()), s3.GetLength() + 1);
		RegCloseKey(fKey);
	}

	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, "PegAEsys.tra\\DefaultIcon", 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &fKey, lpdwDisp) == ERROR_SUCCESS)
	{
		RegSetValueEx(fKey, "", 0, REG_SZ, (LPBYTE)s4.GetBuffer(s4.GetLength()), s4.GetLength() + 1);
		RegCloseKey(fKey);
	}
}

//void CPegReg::SetDWG()
//{
//	HKEY fKey;
//	DWORD dwDisp = 0;
//	LPDWORD lpdwDisp = &dwDisp;
//
//	CString s1 = "PegAEsys.dwg";
//	CString s2 = "PegAEsys DWG File";
//
//	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, ".dwg", 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &fKey, lpdwDisp) == ERROR_SUCCESS)
//	{
//		RegSetValueEx(fKey, "", 0, REG_SZ, (LPBYTE)s1.GetBuffer(s1.GetLength()), s1.GetLength() + 1);
//		RegCloseKey(fKey);
//	}
//
//	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, s1, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &fKey, lpdwDisp) == ERROR_SUCCESS)
//	{
//		RegSetValueEx(fKey, "", 0, REG_SZ, (LPBYTE)s2.GetBuffer(s2.GetLength()), s2.GetLength() + 1);
//		RegCloseKey(fKey);
//	}
//
//	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, "PegAEsys.dwg\\Shell\\Open\\command", 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &fKey, lpdwDisp) == ERROR_SUCCESS)
//	{
//		RegSetValueEx(fKey, "", 0, REG_SZ, (LPBYTE)s3.GetBuffer(s3.GetLength()), s3.GetLength() + 1);
//		RegCloseKey(fKey);
//	}
//
//	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, "PegAEsys.dwg\\DefaultIcon", 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &fKey, lpdwDisp) == ERROR_SUCCESS)
//	{
//		RegSetValueEx(fKey, "", 0, REG_SZ, (LPBYTE)s4.GetBuffer(s4.GetLength()), s4.GetLength() + 1);
//		RegCloseKey(fKey);
//	}
//}
//
//void CPegReg::SetDXF()
//{
//	HKEY fKey;
//	DWORD dwDisp = 0;
//	LPDWORD lpdwDisp = &dwDisp;
//
//	CString s1 = "PegAEsys.dxf";
//	CString s2 = "PegAEsys DXF File";
//
//	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, ".dxf", 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &fKey, lpdwDisp) == ERROR_SUCCESS)
//	{
//		RegSetValueEx(fKey, "", 0, REG_SZ, (LPBYTE)s1.GetBuffer(s1.GetLength()), s1.GetLength() + 1);
//		RegCloseKey(fKey);
//	}
//
//	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, s1, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &fKey, lpdwDisp) == ERROR_SUCCESS)
//	{
//		RegSetValueEx(fKey, "", 0, REG_SZ, (LPBYTE)s2.GetBuffer(s2.GetLength()), s2.GetLength() + 1);
//		RegCloseKey(fKey);
//	}
//
//	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, "PegAEsys.dxf\\Shell\\Open\\command", 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &fKey, lpdwDisp) == ERROR_SUCCESS)
//	{
//		RegSetValueEx(fKey, "", 0, REG_SZ, (LPBYTE)s3.GetBuffer(s3.GetLength()), s3.GetLength() + 1);
//		RegCloseKey(fKey);
//	}
//
//	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, "PegAEsys.dxf\\DefaultIcon", 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &fKey, lpdwDisp) == ERROR_SUCCESS)
//	{
//		RegSetValueEx(fKey, "", 0, REG_SZ, (LPBYTE)s4.GetBuffer(s4.GetLength()), s4.GetLength() + 1);
//		RegCloseKey(fKey);
//	}
//}