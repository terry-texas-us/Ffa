#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

void DlgProcBitmapLoadPaint(HWND hDlg, RECT dimen, char *pathname, bool bPreview);

UINT CALLBACK OFNHookProcBitmapLoad(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	static char psz[MAX_PATH];
	static bool preview = true;

	// w - 265
	// h - 220

	RECT dimen;
	dimen.top = 47;
	dimen.bottom = 267;
	dimen.left = 577;
	dimen.right = 842;

	switch (uiMsg) 
	{
		case WM_INITDIALOG:
		{
			if(preview)
				::CheckDlgButton(hDlg, IDC_BITMAP_PREVIEW, BST_CHECKED);

			return (TRUE);
		}
		case WM_PAINT:
		{
			DlgProcBitmapLoadPaint(hDlg, dimen, psz, preview);
			return (TRUE);
		}
		case WM_NOTIFY:
		{
			LPOFNOTIFY lpofn = (LPOFNOTIFY) lParam;
			if (lpofn->hdr.code == CDN_SELCHANGE)
			{
				CommDlg_OpenSave_GetFilePath(GetParent(hDlg), psz, MAX_PATH);
				InvalidateRect(hDlg, &dimen, true);
			}
			return (TRUE);
		}
		case WM_COMMAND:
		{
			char szFilePath[MAX_PATH];
			CommDlg_OpenSave_GetFilePath(GetParent(hDlg), szFilePath, MAX_PATH);
			
			CString strFileName(szFilePath);
			
			CFileStatus	fs;
			if (!CFile::GetStatus(strFileName, fs))
			{
				msgWarning(IDS_MSG_FILE_NOT_FOUND, strFileName, hDlg);
				return (TRUE);
			}
			switch (LOWORD(wParam)) 
			{
				case IDCANCEL:
					PostMessage(GetParent(hDlg), WM_COMMAND, MAKEWPARAM(IDCANCEL, 0), 0);
					return TRUE;
				case IDC_BITMAP_PREVIEW:
					if(HIWORD(wParam) == BN_CLICKED)
					{
						preview = !preview;
						InvalidateRect(hDlg, NULL, TRUE);
					}
					return TRUE;
			}
		}		
	}
	return (FALSE); 		// Message for default dialog handlers                                                                                                             
}

void DlgProcBitmapLoadPaint(HWND hDlg, RECT dimen, char *pathname, bool bPreview)
{
	COLORREF old;
	PAINTSTRUCT ps;
	CDC dc;
	dc.Attach(::BeginPaint(hDlg, &ps));

	// mak - 
	old = dc.SetBkColor(RGB(255, 255, 255));

	dc.Rectangle(&dimen);

	if(pathname != 0 && bPreview)
	{
		HBITMAP hBitmap = (HBITMAP)::LoadImage(app.GetInstance(), pathname, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

		if(hBitmap == 0)
			return;

		CBitmap m_bmpBitmap;
		m_bmpBitmap.Attach(hBitmap);

		CDC dc2;
		dc2.CreateCompatibleDC(&dc);
		dc2.SelectObject(&m_bmpBitmap);

		// size of bitmap
		BITMAP bm;
		::GetObject(hBitmap, sizeof(bm), &bm);
		int width = bm.bmWidth;
		int height = bm.bmHeight;

		RECT dimen2;
		dimen2.top = dimen.top + 1;
		dimen2.bottom = dimen.bottom - 1;
		dimen2.left = dimen.left + 1;
		dimen2.right = dimen.right - 1;

		dc.StretchBlt(dimen2.left, dimen2.top, dimen2.right - dimen2.left, dimen2.bottom - dimen2.top, &dc2, 0, 0, width, height, SRCCOPY);

		DeleteObject(m_bmpBitmap);
	}
	
	dc.SetBkColor(old);
	dc.Detach();
	
	::EndPaint(hDlg, &ps);
}