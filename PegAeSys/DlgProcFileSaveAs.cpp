#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#define edt1	0x0480
#define stc3	0x0442
#define stc2	0x0441
#define cmb1	0x0470
#define lst1	0x0460

void ResizeCommonDlg(HWND hDlg, UINT AddSizeX, UINT AddSizeY);

UINT_PTR CALLBACK OFNHookProcFileSaveAs(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();
	char szBuf2[256];

	switch (uiMsg)
	{
		case WM_INITDIALOG:
		{
			if(pDoc->SaveShadow())
				::CheckDlgButton(hDlg, IDC_ENABLE_SHADOW, BST_CHECKED);

//			if(pDoc->IsFileReadOnly())
//				::CheckDlgButton(hDlg, IDC_READONLY, BST_CHECKED);

//			ResizeCommonDlg(hDlg, 300, 300);

			return TRUE;
		}
		case WM_NOTIFY:
		{
			LPOFNOTIFY lpofn;
			lpofn = (LPOFNOTIFY) lParam;
			if (lpofn->hdr.code == CDN_TYPECHANGE)
			{
				SendMessage(GetParent(hDlg), CDM_GETSPEC, (WPARAM) 256, (LPARAM)szBuf2);
				CString strText = szBuf2;

				if(strText.ReverseFind('.') != -1)	
				{
					CString strType = strText.Left(strText.ReverseFind('.'));

					switch(lpofn->lpOFN->nFilterIndex)
					{
						case 1:	strType.Append(".peg"); break;
						case 2: strType.Append(".jb1"); break;
						case 3: strType.Append(".tra"); break;
						case 4: strType.Append(".ffa"); break;
						case 5: strType.Append(".dwg"); break;
						case 6: strType.Append(".dxf"); break;
					}
				
					SendMessage(GetParent(hDlg), CDM_SETCONTROLTEXT, edt1, (LPARAM)strType.GetBuffer()); 
				}
			}

			return TRUE;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{
				case IDC_ENABLE_SHADOW:
					if(HIWORD(wParam) == BN_CLICKED)
						pDoc->SetSaveShadow(!pDoc->SaveShadow());
					
					return TRUE;
//				case IDC_READONLY:
//					if(HIWORD(wParam) == BN_CLICKED)
//						pDoc->SetFileReadOnly(!pDoc->IsFileReadOnly());
//
//					return TRUE;
			}
		}
	}
	return FALSE;		// Message for default dialog handlers
}