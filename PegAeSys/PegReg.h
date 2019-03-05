#pragma once

#include"PegAEsys.h"
#include"stdafx.h"

//namespace registry
//{
//	
//}

class CPegReg
{
	public:
		CPegReg();

		void SetKey(const CString filename);
		CString GetKeyValue();
		void LoadValues();
		void DisplayMRU();
		void SetMenu() { DefaultMenu = app.GetMenu(); }
		void PriorityUp(CString filename);
		bool Find(CString filename);
		void Initialize();

		CString GetItemFilePath(int nIndex);
		CString GetItemPath(int nIndex);

		// used to set file association
		void SetPeg();
		void SetJB1();
		void SetTra();
		//void SetDWG();
		//void SetDXF();

	private:
		HKEY hKey;
		CString l_strExampleKey;
		CString a, b, c, d, e, mruList;
		LONG iSuccess;
		CString s3, s4;
		int iElements;
		HMENU DefaultMenu;
};