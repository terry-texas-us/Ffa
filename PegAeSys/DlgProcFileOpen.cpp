#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "dlgs.h"

#define edt1	0x0480
#define stc3	0x0442
#define stc2	0x0441
#define cmb1	0x0470
#define lst1	0x0460
#define chx1	0x0410

void ResizeCommonDlg(HWND hDlg, UINT AddSizeX, UINT AddSizeY);
//void RecentDirectoriesLoad(HWND hDlg);

UINT_PTR CALLBACK OFNHookProcFileOpen(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();
//	char szBuf2[256];

	switch (uiMsg) 
	{
		case WM_INITDIALOG:
		{
			if(pDoc->SaveShadow())
				::CheckDlgButton(hDlg, IDC_ENABLE_SHADOW, BST_CHECKED);

//			if(pDoc->IsFileReadOnly())
//				::CheckDlgButton(hDlg, IDC_READONLY, BST_CHECKED);

//			ResizeCommonDlg(hDlg, 300, 300);

//			::BringWindowToTop(::GetDlgItem(hDlg, IDC_ENABLE_SHADOW));
//			::ShowWindow(::GetDlgItem(hDlg, IDC_ENABLE_SHADOW), SW_HIDE);
//			::ShowWindow(::GetDlgItem(hDlg, IDC_ENABLE_SHADOW), SW_SHOW);

			pDoc->RecentDirectoriesLoad(hDlg);

			return TRUE;
		}
		//case WM_NOTIFY:
		//{
		//	LPOFNOTIFY lpOfNotify = (LPOFNOTIFY) lParam;

		//	//if(lpOfNotify->hdr == CDN_FOLDERCHANGE)
		//	switch((int)lpOfNotify->hdr.code)
		//	{
		//	case CDN_FOLDERCHANGE:
		//		{
		//			MessageBox(0,0,0,0);
		//			break;
		//		}
		//	}

		//	return TRUE;
		//}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{
				case IDC_ENABLE_SHADOW:
				{
					if(HIWORD(wParam) == BN_CLICKED)
						pDoc->SetSaveShadow(!pDoc->SaveShadow());
					return TRUE;
				}
				case IDC_RECENT_DIR:
				{
					if(HIWORD(wParam) == CBN_SELCHANGE)
					{
						//MessageBox(0, 0, 0, 0);
						char szBuf[512];
						::GetDlgItemText(hDlg, IDC_RECENT_DIR, szBuf, 512);


						//HWND chWnd = GetDlgItem(phWnd, lst1);
						//::DlgDirList(::GetParent(hDlg), szBuf, lst1, 0, 0);//DDL_DIRECTORY);
						//::DlgDirSelectComboBoxEx(hDlg, 
						::SetCurrentDirectory(szBuf);

					//	OPENFILENAME of;
					//	::ZeroMemory(&of, sizeof(OPENFILENAME));
					//	of.lpstrInitialDir = szBuf;
					//	of.lStructSize = sizeof(OPENFILENAME);
					//	of.hwndOwner = 0;
					////	of.hwndOwner = hDlg;
					//	of.hInstance = app.GetInstance();
					//	//of.lpstrFilter = szFilter;
					//	//of.lpstrFilter = "Job Files (.jb1)\0*.jb1\0Tracing Files (.tra)\0*.tra\0";
					//	//of.nFilterIndex = nFilterIndex;
					//	of.lpstrFile = new char[MAX_PATH]; 
					//	of.lpstrFile[0] = 0;
					//	of.nMaxFile = MAX_PATH;
					//	of.lpstrTitle = "Include Tracing";
					//	of.Flags = OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
					//	of.lpstrDefExt = "tra";
					//	//of.lpfnHook = OFNHookProcTracingInclude;
					//	//of.lpTemplateName = MAKEINTRESOURCE(IDD_TRACING_INCLUDE);
					////	of.lpTemplateName = MAKEINTRESOURCE(IDD_TRACING_EX);

						OFNOTIFY ofNotify;
						ofNotify.hdr.code = CDN_FOLDERCHANGE;
						//ofNotify.lpOFN = &of;

						PostMessage(::GetParent(hDlg), WM_NOTIFY, 0, (LPARAM)&ofNotify);
						
					}
					return TRUE;
				}
//				case IDC_READONLY:
//				{
//					if(HIWORD(wParam) == BN_CLICKED)
//						pDoc->SetFileReadOnly(!pDoc->IsFileReadOnly());
//					return TRUE;
//				}
//				case IDCANCEL:
//					MessageBox(0, 0, 0, 0);
//					return TRUE;
			}
		}		
		case WM_DESTROY:
		{
//			if(::IsDlgButtonChecked(hDlg, IDC_READONLY))
//			if(HIWORD(wParam) == BN_CLICKED)
//				pDoc->SetFileReadOnly(!pDoc->IsFileReadOnly());
//			pDoc->SetFileReadOnly(::IsDlgButtonChecked(hDlg, IDC_READONLY));
			return TRUE;
		}
	}
	return FALSE;		// Message for default dialog handlers
}

void CPegDoc::RecentDirectoriesLoad(HWND hDlg)
{
	HWND hWndRD = ::GetDlgItem(hDlg, IDC_RECENT_DIR);

	for(int i = 0; i < 5; i++)
	{
		CString strPath = registry.GetItemPath(i);

		LRESULT lResult = SendMessage(hWndRD, (UINT)CB_FINDSTRING, (WPARAM)-1, (LPARAM)(LPCTSTR)strPath.GetBuffer());
		if(lResult == CB_ERR)
			SendMessage(hWndRD, (UINT)CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)strPath.GetBuffer());
	}

	::SendMessage(hWndRD, CB_SETCURSEL, 0, 0);
}

void ResizeCommonDlg(HWND hDlg, UINT AddSizeX, UINT AddSizeY)
{
	const UINT cNum = 7; //Number of controls to move/resize

	HWND phWnd = GetParent(hDlg);
	
	RECT Rect;
	GetWindowRect(phWnd, &Rect);
		
	// Change the size of parent window according to 
	SetWindowPos(phWnd, NULL, 0, 0, Rect.right - Rect.left + AddSizeX, 
				 Rect.bottom - Rect.top + AddSizeY, SWP_NOMOVE);

	//Create an array that contains the control names 
	int Control[cNum] = {stc3, stc2,     //The two label ctrls
						 edt1, cmb1,     //Edit and combo ctrls
						 IDOK, IDCANCEL, //The dialog buttons 
						 lst1};   //The Explorer window

	HWND chWnd;
	RECT cRect; 

	//Move all controls to a compensate for the new parent size
	for (int i = 0 ; i < cNum ; i++)
	{
		chWnd = GetDlgItem(phWnd, Control[i]);

		GetWindowRect(chWnd, &cRect);

		POINT pointUL, pointLR;
		pointUL.x = cRect.left;
		pointUL.y = cRect.top;
		pointLR.x = cRect.right;
		pointLR.y = cRect.bottom;

		ScreenToClient(phWnd, &pointUL);
		ScreenToClient(phWnd, &pointLR);

		cRect.left   = pointUL.x;
		cRect.top    = pointUL.y;
		cRect.right	 = pointLR.x;
		cRect.bottom = pointLR.y;

		if (Control[i] != lst1)  
		{
			SetWindowPos(chWnd, NULL, cRect.left,
				cRect.top + AddSizeY, 0, 0, SWP_NOSIZE);

			if((Control[i] == edt1) || (Control[i] == cmb1))
			{
				SetWindowPos(chWnd, NULL, 0, 0, cRect.right  - cRect.left + AddSizeX, 
							cRect.bottom - cRect.top + 0, SWP_NOMOVE);
			}
			else if((Control[i] == IDOK) || (Control[i] == IDCANCEL))
			{
				SetWindowPos(chWnd, NULL, cRect.left + AddSizeX,
				cRect.top + AddSizeY, 0, 0, SWP_NOSIZE);
			}
		}
		else //size the folder view control appropriately
		{
			SetWindowPos(chWnd, NULL, 0, 0, cRect.right  - cRect.left + AddSizeX, 
							cRect.bottom - cRect.top + AddSizeY, SWP_NOMOVE);
		}
	}
}