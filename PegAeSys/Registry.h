#pragma once

class CPegReg
{
	public:
		CPegReg();
		void SetKey();

	private:
		HKEY hKey;
		CString l_strExampleKey;
};

CPegReg::CPegReg()
{
	DWORD dwDisp = 0;
	LPDWORD lpdwDisp = &dwDisp;
	l_strExampleKey = "SOFTWARE\\Fanning\\MRU";

	LONG iSuccess = RegCreateKeyEx( HKEY_CURRENT_USER, l_strExampleKey, 0L,NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey,lpdwDisp);

	if(iSuccess == ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
	}
}

void CPegReg::SetKey()
{
	DWORD dwDisp = 0;
	LPDWORD lpdwDisp = &dwDisp;
//	CString l_strExampleKey = "SOFTWARE\\Microsoft\\Exchange\\MSExchangeAdminCommon";
	CString l_strStringSample = "StringSample";
	CString l_strStringSampleVal = "String Value";;

	LONG iSuccess = RegCreateKeyEx( HKEY_CURRENT_USER, l_strExampleKey, 0L,NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey,lpdwDisp);

	if(iSuccess == ERROR_SUCCESS)
	{
		RegSetValueEx(hKey, l_strStringSample, 0, REG_SZ, (LPBYTE) l_strStringSampleVal.GetBuffer(l_strStringSampleVal.GetLength()), l_strStringSampleVal.GetLength()+ 1);
	}
}