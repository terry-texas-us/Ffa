#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

void DlgProcSetupImportInit(HWND hDlg);
void DlgProcSetupImportOk(HWND hDlg);

BOOL CALLBACK DlgProcOptionsTabImport(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
{
//	CPegDoc *pDoc = CPegDoc::GetDoc();

	switch (nMsg) 
	{
		case WM_INITDIALOG:
		{
			DlgProcSetupImportInit(hDlg);
			return (TRUE);
		}
		// used with options dialog
		case WM_DIALOG_FOCUS:
		{
			SetFocus(GetWindow(hDlg,GW_CHILD));
			return TRUE;
		}
		// used with options dialog
		case WM_DIALOG_OK:
		{
			DlgProcSetupImportOk(hDlg);
			::EndDialog(hDlg, TRUE);
			return (TRUE);
		}
		// used in file->import menu
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{
				case IDC_IMPORT_TRANSLATE:
				{
					if(::IsDlgButtonChecked(hDlg, IDC_IMPORT_TRANSLATE))
						::EnableWindow(::GetDlgItem(hDlg, IDC_IMPORT_TRANSLATELIST), true);
					else
						::EnableWindow(::GetDlgItem(hDlg, IDC_IMPORT_TRANSLATELIST), false);
	
					break;
				}
				case IDOK:
					DlgProcSetupImportOk(hDlg);
					::EndDialog(hDlg, TRUE);
					return (TRUE);
				case IDCANCEL:
					::EndDialog(hDlg, IDCANCEL);
					return (FALSE);
			}
			break;
		}
	}
	return (FALSE); 											
}

void DlgProcSetupImportOk(HWND hDlg)
{
	UINT bState;
	bool bValue;

	/*bState = ::IsDlgButtonChecked(hDlg, IDC_IMPORT_TRANSLATE);
	bValue = (bState == BST_CHECKED) ? true : false;
	istate.SetTranslatePens(bValue);*/

	bState = ::IsDlgButtonChecked(hDlg, IDC_IMPORT_MODIFYFONT);
	bValue = (bState == BST_CHECKED) ? true : false;
	istate.SetModifyFont(bValue);

	bState = ::IsDlgButtonChecked(hDlg, IDC_IMPORT_UNBLOCK);
	bValue = (bState == BST_CHECKED) ? true : false;
	istate.SetUnblockInserts(bValue);

	bState = ::IsDlgButtonChecked(hDlg, IDC_IMPORT_PURGEBLOCKS);
	bValue = (bState == BST_CHECKED) ? true : false;
	istate.SetPurgeBlocks(bValue);

	bState = ::IsDlgButtonChecked(hDlg, IDC_IMPORT_SCALE);
	bValue = (bState == BST_CHECKED) ? true : false;
	istate.SetScale(bValue);

	bState = ::IsDlgButtonChecked(hDlg, IDC_IMPORT_KILLLAYERS);
	bValue = (bState == BST_CHECKED) ? true : false;
	istate.SetKillLayers(bValue);

	bState = ::IsDlgButtonChecked(hDlg, IDC_IMPORT_OMITLAYERS);
	bValue = (bState == BST_CHECKED) ? true : false;
	istate.SetOmitLayers(bValue);

	bState = ::IsDlgButtonChecked(hDlg, IDC_IMPORT_COMMANDS);
	bValue = (bState == BST_CHECKED) ? true : false;
	istate.SetResolveCommands(bValue);

	bState = ::IsDlgButtonChecked(hDlg, IDC_IMPORT_HARDPENS);
	bValue = (bState == BST_CHECKED) ? true : false;
	istate.SetHardPens(bValue);

	bState = ::IsDlgButtonChecked(hDlg, IDC_IMPORT_HARDSTYLE);
	bValue = (bState == BST_CHECKED) ? true : false;
	istate.SetHardStyle(bValue);

	bState = ::IsDlgButtonChecked(hDlg, IDC_IMPORT_POLYLINE);
	bValue = (bState == BST_CHECKED) ? true : false;
	istate.SetPolyLines(bValue);

	bState = ::IsDlgButtonChecked(hDlg, IDC_IMPORT_EXPLODE);
	bValue = (bState == BST_CHECKED) ? true : false;
	istate.SetExplode(bValue);

	bState = ::IsDlgButtonChecked(hDlg, IDC_IMPORT_VIEWBOX);
	bValue = (bState == BST_CHECKED) ? true : false;
	istate.SetViewBoxes(bValue);

	bState = ::IsDlgButtonChecked(hDlg, IDC_IMPORT_VIEWPORT);
	bValue = (bState == BST_CHECKED) ? true : false;
	istate.SetViewports(bValue);

	// get scale text
	char szBuf[7];
	::GetDlgItemText(hDlg, IDC_IMPORT_SCALE_VALUE, szBuf, 6);
	istate.SetScaleValue(atof(szBuf));

	//// get translate filename
	//LRESULT lResult = ::SendDlgItemMessage(hDlg, IDC_IMPORT_TRANSLATELIST, CB_GETCURSEL, 0, 0);
	//if(lResult != CB_ERR)
	//{
	//	char szBuf2[MAX_PATH];

	//	::SendDlgItemMessage(hDlg, IDC_IMPORT_TRANSLATELIST, CB_GETLBTEXT, lResult, (LPARAM)szBuf2);
	//	istate.SetTranslateFile(szBuf2);
	//}
}

void DlgProcSetupImportInit(HWND hDlg)
{
	UINT bCheck; /*= (istate.GetTranslatePens() == true) ? BST_CHECKED : BST_UNCHECKED;
	::CheckDlgButton(hDlg, IDC_IMPORT_TRANSLATE, bCheck);*/

	bCheck = (istate.GetModifyFont() == true) ? BST_CHECKED : BST_UNCHECKED;
	::CheckDlgButton(hDlg, IDC_IMPORT_MODIFYFONT, bCheck);

	bCheck = (istate.GetUnblockInserts() == true) ? BST_CHECKED : BST_UNCHECKED;
	::CheckDlgButton(hDlg, IDC_IMPORT_UNBLOCK, bCheck);

	bCheck = (istate.GetPurgeBlocks() == true) ? BST_CHECKED : BST_UNCHECKED;
	::CheckDlgButton(hDlg, IDC_IMPORT_PURGEBLOCKS, bCheck);

	bCheck = (istate.GetScale() == true) ? BST_CHECKED : BST_UNCHECKED;
	::CheckDlgButton(hDlg, IDC_IMPORT_SCALE, bCheck);

	bCheck = (istate.GetKillLayers() == true) ? BST_CHECKED : BST_UNCHECKED;
	::CheckDlgButton(hDlg, IDC_IMPORT_KILLLAYERS, bCheck);

	bCheck = (istate.GetOmitLayers() == true) ? BST_CHECKED : BST_UNCHECKED;
	::CheckDlgButton(hDlg, IDC_IMPORT_OMITLAYERS, bCheck);

	bCheck = (istate.GetResolveCommands() == true) ? BST_CHECKED : BST_UNCHECKED;
	::CheckDlgButton(hDlg, IDC_IMPORT_COMMANDS, bCheck);

	bCheck = (istate.GetHardPens() == true) ? BST_CHECKED : BST_UNCHECKED;
	::CheckDlgButton(hDlg, IDC_IMPORT_HARDPENS, bCheck);

	bCheck = (istate.GetHardStyle() == true) ? BST_CHECKED : BST_UNCHECKED;
	::CheckDlgButton(hDlg, IDC_IMPORT_HARDSTYLE, bCheck);

	bCheck = (istate.GetPolylines() == true) ? BST_CHECKED : BST_UNCHECKED;
	::CheckDlgButton(hDlg, IDC_IMPORT_POLYLINE, bCheck);

	bCheck = (istate.GetExplode() == true) ? BST_CHECKED : BST_UNCHECKED;
	::CheckDlgButton(hDlg, IDC_IMPORT_EXPLODE, bCheck);

	bCheck = (istate.GetViewBoxes() == true) ? BST_CHECKED : BST_UNCHECKED;
	::CheckDlgButton(hDlg, IDC_IMPORT_VIEWBOX, bCheck);

	bCheck = (istate.GetViewports() == true) ? BST_CHECKED : BST_UNCHECKED;
	::CheckDlgButton(hDlg, IDC_IMPORT_VIEWPORT, bCheck);

	//// enable translate file list if necessary
	//if(::IsDlgButtonChecked(hDlg, IDC_IMPORT_TRANSLATE))
	//	::EnableWindow(::GetDlgItem(hDlg, IDC_IMPORT_TRANSLATELIST), true);
	//else
	//	::EnableWindow(::GetDlgItem(hDlg, IDC_IMPORT_TRANSLATELIST), false);

	//// fill translate file list
	//TCHAR szPath[MAX_PATH];
	//char *pStr;
	//GetModuleFileName(app.GetInstance(), szPath, MAX_PATH);
	//pStr = strrchr(szPath, '\\');
	//if (pStr != NULL)
	//*(++pStr)='\0';

	//CString s3 = szPath;
	//s3 += "Pens\\Colors\\*.txt";

	//char strSearch[12] = "_readme.txt";

	//int iResult = DlgDirListComboBox(hDlg, s3.GetBuffer(), IDC_IMPORT_TRANSLATELIST, 0, DDL_READWRITE);
	//if(iResult == 0)
	//	MessageBox(hDlg, "Pen Color files must be located in \"(ApplicationDirectory)\\Pens\\Colors\\\"", 0, 0);

	//int lResult = ::SendDlgItemMessage(hDlg, IDC_IMPORT_TRANSLATELIST, CB_FINDSTRING, (WPARAM) -1, (LPARAM)&strSearch); 
	//if(lResult != CB_ERR)
	//	::SendDlgItemMessage(hDlg, IDC_IMPORT_TRANSLATELIST, CB_DELETESTRING, (WPARAM)lResult, 0);

	//// select pen color file
	//CString strSearch2 = istate.GetTranslateFile();
	//lResult = ::SendDlgItemMessage(hDlg, IDC_IMPORT_TRANSLATELIST, CB_SELECTSTRING, (WPARAM) -1, (LPARAM)(LPSTR)strSearch2.GetBuffer()); 
	//if(lResult == CB_ERR)
	//	::SendDlgItemMessage(hDlg, IDC_IMPORT_TRANSLATELIST, CB_SETCURSEL, 0, 0);
	
	// fill scale value
	char szBuf[7];
	_gcvt(istate.GetScaleValue(), 4, szBuf);
	SetDlgItemText(hDlg, IDC_IMPORT_SCALE_VALUE, szBuf);
}