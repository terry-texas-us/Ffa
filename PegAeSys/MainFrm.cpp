#include "stdafx.h"

#include "MainFrm.h"
#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "dde.h"
//#include "DlgProcEditOps.h"
//#include "FileJob.h"
#include "Grid.h"
//#include "Lex.h"

#include "solo.h"

#define SOLO_CLOSE_IMMEDIATE 27

CMainFrame frame;
CPegDoc doc;
CPegView viewThe;

// mak - status windows
#include "StateInfoFrm.h"
extern CInfoStatusFrm statusframe;
extern CInfoModeFrm modeframe;
extern CInfoOdomFrm odomframe;
extern CBackgroundLayer BackgroundLayer;

//#include "WndPrintProc.h"
#include "DlgProcPrintPreview.h"
//extern CPrintFrm printframe;
extern CStateFrm stateframe;

#include "DlgProcPlotPreview.h"
extern CPlotFrm plotframe;

extern CSolo solo;
//extern bool isSoloOpen;

long lScanCode = 0x00;

LRESULT HANDLE_NCACTIVATE(HWND hwndMain, HWND hwnd, WPARAM wParam, LPARAM lParam);
LRESULT HANDLE_ENABLE(HWND hwndMain, HWND hwnd, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProcMain(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CPegView* pView = CPegView::GetActiveView();
//	CPegDoc *pDoc = CPegDoc::GetDoc();

	switch(nMsg)
	{
		case WM_SOLO_LAYER:
		case WM_SOLO_TRACING:
		case WM_SOLO_BLOCK:
			app.SetThisIsSoloWindow(true);
			break;
		//case WM_PROGRESS_DONE:
		//	pDoc->StopProgress();
		//	break;
	}

	// msg handler routines for solo editor
	if(nMsg == WM_SOLO_LAYER)
		frame.OnSoloLayer(hwnd);
	else if(nMsg == WM_SOLO_TRACING)
		frame.OnSoloTracing(hwnd);
	else if(nMsg == WM_SOLO_BLOCK)
		frame.OnSoloBlock(hwnd);

	else if(nMsg == WM_SOLO_RETURN_BLOCK)
		frame.OnSoloReturnBlock(hwnd);
	else if(nMsg == WM_SOLO_RETURN_TRACING)
		frame.OnSoloReturnTracing(hwnd);
	else if(nMsg == WM_SOLO_RETURN_LAYER)
		frame.OnSoloReturnLayer(hwnd);
//	else if(nMsg == WM_SOLO_RETURN_NONE)
//		isSoloOpen = false;
		
	if(nMsg == WM_KEYDOWN)
	{
		// number pad plus and minus
		lScanCode = lParam;
		 
		lScanCode = lScanCode >> 16;
		lScanCode = lScanCode & 0xFF;

		if(wParam == VK_SUBTRACT || wParam == VK_OEM_MINUS)
		{
			// numeric keypad "-"
			if(lScanCode == 0x4A)
				pView->OnWindowZoomOut(hwnd);
			else
				app.OnModeTrap();

			return 0;
		}

		if(wParam == VK_ADD || wParam == VK_OEM_PLUS)
		{
			// numeric keypad "+"
			if(lScanCode == 0x4E)
				pView->OnWindowZoomIn(hwnd);

			return 0;
		}
	}

	//// destroy solo window immediately if main window is closed
	//if((nMsg == WM_CLOSE) && (wParam == SOLO_CLOSE_IMMEDIATE))
	//{
	//	frame.OnSoloDeleteKey();
	//	DestroyWindow(hwnd);
	//}

	switch (nMsg) 
	{
		HANDLE_MSG(hwnd, WM_CLOSE, frame.OnClose);
		HANDLE_MSG(hwnd, WM_COMMAND, frame.OnCommand);
		HANDLE_MSG(hwnd, WM_CREATE, frame.OnCreate);
		HANDLE_MSG(hwnd, WM_DESTROY, frame.OnDestroy);
		HANDLE_MSG(hwnd, WM_KEYDOWN, frame.OnKey);
		HANDLE_MSG(hwnd, WM_KEYUP, frame.OnKey);
		HANDLE_MSG(hwnd, WM_LBUTTONDOWN, pView->OnLButtonDown);
		HANDLE_MSG(hwnd, WM_LBUTTONUP, pView->OnLButtonUp);
		HANDLE_MSG(hwnd, WM_MOUSEMOVE, pView->OnMouseMove);
		HANDLE_MSG(hwnd, WM_PAINT, frame.OnPaint);
		HANDLE_MSG(hwnd, WM_RBUTTONDOWN, pView->OnRButtonDown);
		HANDLE_MSG(hwnd, WM_RBUTTONUP, pView->OnRButtonUp);
		HANDLE_MSG(hwnd, WM_SETFOCUS, pView->OnSetFocus);
		HANDLE_MSG(hwnd, WM_SIZE, pView->OnSize);

		// mak
		HANDLE_MSG(hwnd, WM_VSCROLL, pView->OnVScroll);
		HANDLE_MSG(hwnd, WM_HSCROLL, pView->OnHScroll);
		HANDLE_MSG(hwnd, WM_MOUSEWHEEL, pView->OnMouseWheel);

		HANDLE_MSG(hwnd, WM_MOVE, frame.OnMove);
		HANDLE_MSG(hwnd, WM_ERASEBKGND, pView->OnEraseBkgnd);
		
//#ifdef ENABLE_DRAGNDROP
//		HANDLE_MSG(hwnd, WM_DROPFILES, frame.OnDropfiles);
//#endif
	}

	return (::DefWindowProc(hwnd, nMsg, wParam, lParam));
}

// CMainFrame construction/destruction
CMainFrame::CMainFrame()
{
	m_wSoloType = SOLO_TYPE_NONE;
}

//CMainFrame::~CMainFrame()
//{
//}

//// CMainFrame diagnostics
//#ifdef _DEBUG
//void CMainFrame::AssertValid() const
//{
//}

//void CMainFrame::Dump(CDumpContext&) const
//{
//}
//#endif // _DEBUG

///<summary>
/// 
///</summary>
void CMainFrame::OnClose(HWND hwnd)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	if(m_wSoloType == SOLO_TYPE_NONE)
	{
		int iRetVal = pDoc->DisplaySaveMessageConfirmation(IDS_MSG_CONFIRM_CLOSE);

		if(iRetVal != IDCANCEL)
		{
			if(iRetVal == IDYES)
				if(!pDoc->OpenFileSave())
					return;

			OnSoloSendClose();
			app.KillAutoSave();
			DestroyWindow(hwnd);
		}

//		if(!pDoc->IsFileReadOnly())
//		{
//			int iRetVal = IDNO;
//			if((pDoc->m_wOpenFileType != CPegDoc::FILE_TYPE_NONE) || (pDoc->GetHotCount() > 0))
//				iRetVal = IDYES;
//
//			if(iRetVal == IDYES)
//				iRetVal = msgConfirm(IDS_MSG_CONFIRM_CLOSE, pDoc->GetPathName());
//		
//			if(iRetVal != IDCANCEL)
//			{
//				if(iRetVal == IDYES)
//					pDoc->OpenFileSave();
////
////				OnSoloSendClose();
////				app.KillAutoSave();
//				DestroyWindow(hwnd);
//			}
//		}
//		else
//		{
////			OnSoloSendClose();
////			app.KillAutoSave();
//			DestroyWindow(hwnd);
//		}
	}
	else if(m_wSoloType == SOLO_TYPE_BLOCK)
	{
		int iRetVal = msgConfirm(IDS_MSG_CONFIRM_CLOSE, "the block");
			
		if(iRetVal != IDCANCEL)
		{
			if(iRetVal == IDYES)
			{
				solo.SaveSoloFileBlock();
				SendNotifyMessage(solo.SoloFileGetPrevHwnd(), WM_SOLO_RETURN_BLOCK, 0, 0);
			}
			else
			{
				pDoc->OnEditTrapWorkAndActive();
				solo.OnSoloTrapBlock();
				pDoc->LayersRemove("__blockedit__");
				OnSoloDeleteKey();
			}
			
			SendNotifyMessage(solo.SoloFileGetPrevHwnd(), WM_SOLO_RETURN_NONE, 0, 0);
			DestroyWindow(hwnd);
		}
	}
	else if(m_wSoloType == SOLO_TYPE_TRACING)
	{
		int iRetVal = msgConfirm(IDS_MSG_CONFIRM_CLOSE, pDoc->GetPathName());
			
		if(iRetVal != IDCANCEL)
		{
			if(iRetVal == IDYES)
			{
				solo.SaveSoloFileTracing();
				SendNotifyMessage(solo.SoloFileGetPrevHwnd(), WM_SOLO_RETURN_TRACING, 0, 0);
			}
			else
			{
				OnSoloDeleteKey();
			}
			
			SendNotifyMessage(solo.SoloFileGetPrevHwnd(), WM_SOLO_RETURN_NONE, 0, 0);
			DestroyWindow(hwnd);
		}
	}
	else if(m_wSoloType == SOLO_TYPE_LAYER)
	{
		int iRetVal = msgConfirm(IDS_MSG_CONFIRM_CLOSE, "the layer");
			
		if(iRetVal != IDCANCEL)
		{
			if(iRetVal == IDYES)
			{
				solo.SaveSoloFileLayer();
				SendNotifyMessage(solo.SoloFileGetPrevHwnd(), WM_SOLO_RETURN_LAYER, 0, 0);
			}
			else
			{
				OnSoloDeleteKey();
			}
			
			SendNotifyMessage(solo.SoloFileGetPrevHwnd(), WM_SOLO_RETURN_NONE, 0, 0);
			DestroyWindow(hwnd);
		}
	}
	else
	{
		DestroyWindow(hwnd);
	}
}

void CMainFrame::OnSoloSendClose()
{
	// get solo hwnd
	HKEY hKey;
	TCHAR szData4[1024];
	DWORD buffSize4 = 1024;

	RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\Solo", 0, KEY_ALL_ACCESS, &hKey);
	RegQueryValueEx(hKey, "solohwnd", NULL, NULL, (LPBYTE)szData4, &buffSize4);
	RegCloseKey(hKey);
	CString prevHwnd = szData4;

	DWORD prevhandle = atoi(prevHwnd);
	HWND soloHandle = (HWND)prevhandle;

	SendNotifyMessage(soloHandle, WM_CLOSE, (WPARAM)SOLO_CLOSE_IMMEDIATE, 0);
}

///<summary>
/// 
///</summary>
void CMainFrame::OnSoloDeleteKey()
{
	HKEY hKey;

	RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\Solo", 0, KEY_ALL_ACCESS, &hKey);
	RegDeleteValue(hKey, "tempname");
	RegDeleteValue(hKey, "hwnd");
	RegDeleteValue(hKey, "pathnameblock");
	RegDeleteValue(hKey, "temppath");
	RegDeleteValue(hKey, "tracingname");
	RegDeleteValue(hKey, "solohwnd");
	RegCloseKey(hKey);
}

///<summary>
/// 
///</summary>
void CMainFrame::OnSoloReturnTracing(HWND)
{	
//	isSoloOpen = false;

	CPegView* pView = CPegView::GetActiveView();

	// load tracing name from registry
	HKEY hKey;
	TCHAR szData[1024];
	DWORD buffSize = 1024;

	RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\Solo", 0, KEY_ALL_ACCESS, &hKey);
	RegQueryValueEx(hKey, "tracingname", NULL, NULL, (LPBYTE)szData, &buffSize);
	RegCloseKey(hKey);
	CString strTraceName = szData;

	// open layer
	CPegDoc *pDoc = CPegDoc::GetDoc();
	CLayer *pLayer = pDoc->LayersGet(strTraceName);

	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_LAYER_ERASE, pLayer);
	pDoc->LayersRemove(strTraceName);
	pDoc->TracingMap(strTraceName);

	// delete registry keys
	OnSoloDeleteKey();
	pView->InvalidateRect(NULL, TRUE);
}

///<summary>
/// 
///</summary>
void CMainFrame::OnSoloReturnBlock(HWND)
{
//	isSoloOpen = false;

	CPegView* pView = CPegView::GetActiveView();

	// get pathnameblock value from registry
	HKEY hKey;
	TCHAR szData[1024];
	DWORD buffSize = 1024;

	RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\Solo", 0, KEY_ALL_ACCESS, &hKey);
	RegQueryValueEx(hKey, "pathnameblock", NULL, NULL, (LPBYTE)szData, &buffSize);
	RegCloseKey(hKey);
	CString strFileName = szData;

	// open file and merge
//	CFileException e;
	CFilePeg* pFile = new CFilePeg;
//	pFile->Open(strFileName, CFile::modeReadWrite | CFile::shareDenyWrite, &e);
	pFile->Open(strFileName, CFile::modeReadWrite | CFile::shareDenyWrite, false);
	
//	if (e.m_cause == CFileException::sharingViolation)
//	{	
//		msgWarning(IDS_MSG_FILE_LOCKED, strFileName);
//		return;
//	}
	
	pFile->Load();
	pFile->Close();
	
	OnSoloDeleteKey();
	pView->InvalidateRect(NULL, TRUE);
}

///<summary>
/// 
///</summary>
void CMainFrame::OnSoloReturnLayer(HWND)
{
//	isSoloOpen = false;

	CPegDoc *pDoc = CPegDoc::GetDoc();
	CPegView* pView = CPegView::GetActiveView();

	// get pathnameblock value from registry
	HKEY hKey;
	TCHAR szData[1024];
	DWORD buffSize = 1024;

	RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\Solo", 0, KEY_ALL_ACCESS, &hKey);
	RegQueryValueEx(hKey, "temppath", NULL, NULL, (LPBYTE)szData, &buffSize);
	RegCloseKey(hKey);
	CString strFileName = szData;

	// get old layer info
//	CLayer *oldLayer = pDoc->LayersGet(pDoc->SoloFileGetLayerName());
	CLayer *oldLayer = pDoc->LayersGet(solo.SoloFileGetLayerName());
	WORD tFlags = oldLayer->GetTracingFlgs();
	WORD sFlags = oldLayer->GetStateFlgs();
//	pDoc->LayersRemove(pDoc->SoloFileGetLayerName());
	pDoc->LayersRemove(solo.SoloFileGetLayerName());

	// open file and merge
	CLayer *pLayer = new CLayer;
//	pLayer->SetName(pDoc->SoloFileGetLayerName());
	pLayer->SetName(solo.SoloFileGetLayerName());
	pLayer->ClrStateFlg();
	pLayer->ClrTracingFlg();
	pLayer->SetStateFlg(sFlags);
	pLayer->SetTracingFlg(tFlags);

	pDoc->LayersAdd(pLayer);
	pDoc->TracingLoadLayer(strFileName, pLayer);
	pDoc->TracingFuse(strFileName);

	// cleanup
	OnSoloDeleteKey();
	pView->InvalidateRect(NULL, TRUE);
}

void CMainFrame::OnSoloAddHwnd()
{
	SECURITY_ATTRIBUTES shouldInherit;
	shouldInherit.nLength = sizeof(SECURITY_ATTRIBUTES);
	shouldInherit.lpSecurityDescriptor = NULL;
	shouldInherit.bInheritHandle = TRUE;

	CString s1 = "SOFTWARE\\Fanning\\Solo";
	HKEY fKey;
	DWORD dwDisp = 0;
	LPDWORD lpdwDisp = &dwDisp;

	// get HWND to send to child process
	DWORD handle = (DWORD)app.GetSafeHwnd();
	char HwndTemp[25];
	itoa(handle, HwndTemp, 10);
	CString appHwnd = HwndTemp;

	if(RegCreateKeyEx(HKEY_CURRENT_USER, s1, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, &shouldInherit, &fKey, lpdwDisp) == ERROR_SUCCESS)
	{
		RegSetValueEx(fKey, "solohwnd", 0, REG_SZ, (LPBYTE)appHwnd.GetBuffer(appHwnd.GetLength()),appHwnd.GetLength() + 1);
		RegCloseKey(fKey);
	}
}

///<summary>
/// 
///</summary>
void CMainFrame::OnSoloTracing(HWND)
{
	m_wSoloType = SOLO_TYPE_TRACING;

	//add hwnd to registry
	OnSoloAddHwnd();

	HKEY hKey;
	TCHAR szData[1024];
	DWORD buffSize = 1024;

	RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\Solo", 0, KEY_ALL_ACCESS, &hKey);
	RegQueryValueEx(hKey, "tracingname", NULL, NULL, (LPBYTE)szData, &buffSize);
	RegCloseKey(hKey);

	CString layer = szData;

	OnSoloLoadPrevHwnd();

	// open tracing document
	CPegDoc* pDoc = CPegDoc::GetDoc();
	pDoc->TracingOpen(layer);

	// disable features for solo window
	EnableMenuItem(app.GetMenu(), IDM_FILE_NEW, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_OPEN, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_SAVE, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_SAVEAS, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_CLOSE, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_MANAGE, MF_GRAYED);

	app.SetThisIsSoloWindow(true);
}

///<summary>
/// 
///</summary>
void CMainFrame::OnSoloLayer(HWND)
{
	m_wSoloType = SOLO_TYPE_LAYER;

	//add hwnd to registry
	OnSoloAddHwnd();

	CPegDoc* pDoc = CPegDoc::GetDoc();
	CPegView *pView = CPegView::GetActiveView();

	HKEY hKey;
	TCHAR szData3[1024];
	TCHAR szData5[1024];
	DWORD buffSize = 1024;
	DWORD buffSize5 = 1024;

	RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\Solo", 0, KEY_ALL_ACCESS, &hKey);
	RegQueryValueEx(hKey, "tempname", NULL, NULL, (LPBYTE)szData3, &buffSize);
	RegCloseKey(hKey);
	CString layername = szData3;

	RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\Solo", 0, KEY_ALL_ACCESS, &hKey);
	RegQueryValueEx(hKey, "temppath", NULL, NULL, (LPBYTE)szData5, &buffSize5);
	RegCloseKey(hKey);
	CString strFileName = szData5;

	// load hwnd and block name
	OnSoloLoadPrevHwnd();
//	pDoc->SoloFileSetLayerName(layername);
	solo.SoloFileSetLayerName(layername);

	// open file for modifications
	pDoc->TracingOpen(strFileName);

	// make view friendly
	pView->OnWindowBest();
	pView->OnWindowZoomOut(pView->GetSafeHwnd());

	// disable features for solo window
	EnableMenuItem(app.GetMenu(), IDM_FILE_NEW, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_OPEN, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_SAVE, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_SAVEAS, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_CLOSE, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_MANAGE, MF_GRAYED);

	app.SetThisIsSoloWindow(true);
}

///<summary>
/// 
///</summary>
void CMainFrame::OnSoloBlock(HWND)
{
	m_wSoloType = SOLO_TYPE_BLOCK;

	//add hwnd to registry
	OnSoloAddHwnd();

	CPegDoc* pDoc = CPegDoc::GetDoc();
	CPegView *pView = CPegView::GetActiveView();

	HKEY hKey;
	TCHAR szData3[1024];
	TCHAR szData5[1024];
	DWORD buffSize = 1024;
	DWORD buffSize5 = 1024;

	RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\Solo", 0, KEY_ALL_ACCESS, &hKey);
	RegQueryValueEx(hKey, "tempname", NULL, NULL, (LPBYTE)szData3, &buffSize);
	RegCloseKey(hKey);
	CString blockname = szData3;

	RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\Solo", 0, KEY_ALL_ACCESS, &hKey);
	RegQueryValueEx(hKey, "temppath", NULL, NULL, (LPBYTE)szData5, &buffSize5);
	RegCloseKey(hKey);
	CString strFileName = szData5;

	// load hwnd and block name
	OnSoloLoadPrevHwnd();
//	pDoc->SoloFileSetBlockName(blockname);
	solo.SoloFileSetBlockName(blockname);

	pDoc->OpenFile(strFileName);

	// add blocks to file;
	CBlock *pBlock;
	pDoc->BlksLookup(blockname, pBlock);

	pDoc->LayersRemoveAll();

	CLayer *player = new CLayer("__blockedit__");

	POSITION pos2 = player->GetHeadPosition();
	POSITION pos = pBlock->GetHeadPosition();
	while (pos != 0)
	{
		CPrim *pprim = pBlock->GetNext(pos);
		CSeg* pseg = new CSeg(pprim);
		player->InsertAfter(pos2, pseg);
	}

	pDoc->LayersAdd(player);
	pDoc->WorkLayerSet(player);

	pView->OnWindowBest();
	pView->OnWindowZoomOut(pView->GetSafeHwnd());

	// disable features for solo window
	EnableMenuItem(app.GetMenu(), IDM_FILE_NEW, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_OPEN, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_SAVE, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_SAVEAS, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_CLOSE, MF_GRAYED);
	EnableMenuItem(app.GetMenu(), IDM_FILE_MANAGE, MF_GRAYED);

	app.SetThisIsSoloWindow(true);
}

///<summary>
/// 
///</summary>
void CMainFrame::OnSoloLoadPrevHwnd()
{
	HKEY hKey;
	TCHAR szData4[1024];
	DWORD buffSize4 = 1024;

	RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\Solo", 0, KEY_ALL_ACCESS, &hKey);
	RegQueryValueEx(hKey, "hwnd", NULL, NULL, (LPBYTE)szData4, &buffSize4);
	RegCloseKey(hKey);
	CString prevHwnd = szData4;

	DWORD prevhandle = atoi(prevHwnd);
	HWND prvHandle = (HWND)prevhandle;

//	CPegDoc *pDoc = CPegDoc::GetDoc();
//	pDoc->SoloFileSetHwnd(prvHandle);
	solo.SoloFileSetHwnd(prvHandle);
}

///<summary>
/// 
///</summary>
void CMainFrame::OnCommand(HWND hwnd, int id, HWND hWndCtrl, UINT uCodeNotify)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	CPegView* pView = CPegView::GetActiveView();

	//if(uCodeNotify == BN_CLICKED)
	//{
	//	switch(app.GetPegState())
	//	{
	//		case STATE_NORMAL:
	//		case STATE_SOLO:
	//			break;
	//		case STATE_PRINT:
	//			stateframe.ToggleState();
	//			break;
	//		case STATE_PLOT: 
	//			plotframe.ToggleState();
	//			break;
	//	}
	//}

	switch(id)
	{
		case ID_HELP_KEY:					WinHelp(hwnd, "peg.hlp", HELP_KEY, (DWORD) (LPSTR) "READY"); break;

		case ID_VIEW_CLIP:					pView->OnClippingRegion(); break;

		case ID_BACKGROUNDIMAGE_LOAD:		pView->OnBackgroundImageLoad(); break;
		case ID_BACKGROUNDIMAGE_REMOVE:		pView->OnBackgroundImageRemove(); break;
		case ID_CAMERA_ROTATEDOWN:			pView->OnCameraRotateDown(); break;
		case ID_CAMERA_ROTATELEFT:			pView->OnCameraRotateLeft(); break;
		case ID_CAMERA_ROTATERIGHT:			pView->OnCameraRotateRight(); break;
		case ID_CAMERA_ROTATEUP:    		pView->OnCameraRotateUp(); break;
		case ID_CAMERA_ROTATECLOCKWISE:		pView->OnCameraZRotateRight(); break;
		case ID_CAMERA_ROTATECOUNTER:		pView->OnCameraZRotateLeft(); break;
		case ID_CAMERA_ZROT_LEFT:			pView->OnCameraZRotateLeft(); break;
		case ID_CAMERA_ZROT_RIGHT:			pView->OnCameraZRotateRight(); break;

		case ID_PRIM_PERPJUMP:				pView->OnPrimPerpJump(); break;
		case ID_PRIM_SNAPTO:				pView->OnPrimSnapTo(); break;
		case ID_RELATIVEMOVES_DOWN:			pView->OnRelativeMovesDown(); break;
		case ID_RELATIVEMOVES_DOWNROTATE:	pView->OnRelativeMovesDownRotate();	break;
		case ID_RELATIVEMOVES_ENG_DOWN:		pView->OnRelativeMovesEngDown(); break;
		case ID_RELATIVEMOVES_ENG_DOWNROTATE:	pView->OnRelativeMovesEngDownRotate(); break;
		case ID_RELATIVEMOVES_ENG_IN:		pView->OnRelativeMovesEngIn(); break;
		case ID_RELATIVEMOVES_ENG_LEFT:		pView->OnRelativeMovesEngLeft(); break;
		case ID_RELATIVEMOVES_ENG_LEFTROTATE:	pView->OnRelativeMovesEngLeftRotate(); break;
		case ID_RELATIVEMOVES_ENG_OUT:		pView->OnRelativeMovesEngOut(); break;
		case ID_RELATIVEMOVES_ENG_RIGHT:	pView->OnRelativeMovesEngRight(); break;
		case ID_RELATIVEMOVES_ENG_RIGHTROTATE:	pView->OnRelativeMovesEngRightRotate(); break;
		case ID_RELATIVEMOVES_ENG_UP:		pView->OnRelativeMovesEngUp(); break;
		case ID_RELATIVEMOVES_ENG_UPROTATE:	pView->OnRelativeMovesEngUpRotate(); break;
		case ID_RELATIVEMOVES_IN:			pView->OnRelativeMovesIn();	break;
		case ID_RELATIVEMOVES_LEFT:			pView->OnRelativeMovesLeft(); break;
		case ID_RELATIVEMOVES_LEFTROTATE:	pView->OnRelativeMovesLeftRotate(); break;
		case ID_RELATIVEMOVES_OUT:			pView->OnRelativeMovesOut(); break;
		case ID_RELATIVEMOVES_RIGHT:		pView->OnRelativeMovesRight(); break;
		case ID_RELATIVEMOVES_RIGHTROTATE:	pView->OnRelativeMovesRightRotate(); break;
		case ID_RELATIVEMOVES_UP:			pView->OnRelativeMovesUp(); break;
		case ID_RELATIVEMOVES_UPROTATE:		pView->OnRelativeMovesUpRotate(); break;
		case ID_SETUP_DIMANGLE:				pView->OnSetupDimAngle(); break;
		case ID_SETUP_DIMLENGTH:			pView->OnSetupDimLength(); break;
		case ID_VIEW_ODOMETER:				pView->OnViewOdometer(); break;
		case ID_VIEW_LIGHTING:				pView->OnViewLighting(); break;
		case ID_VIEW_PENWIDTHS:				pView->OnViewPenWidths(); break;
		case ID_VIEW_PRINTPREVIEW:			pView->OnViewPrintPreview(); break;
		case ID_VIEW_TRUETYPEFONTS:			pView->OnViewTrueTypeFonts(); break;
		case ID_VIEW_BACKGROUNDIMAGE:		pView->OnViewBackgroundImage(); break;
		case ID_VIEW_LAYERSBYCOLOR:			pView->OnViewColorByLayer(); break;

//#ifdef ENABLE_UNDO
		case ID_UNDO:						app.OnUndo(); break;
//#endif
			
//		case ID_CLIP_SET:					pView->OnClipView(); break;

		case ID_VIEW_REFRESH:				pView->OnViewRefresh(); break;
		case ID_VIEW_SCROLLBARS:			pView->OnScrollBars(); break;
		case ID_VIEW_RENDERED:				pView->OnViewRendered(); break;
		case ID_VIEW_WINDOW:				pView->OnViewWindow(hwnd); break;
		case ID_VIEW_WIREFRAME:				pView->OnViewWireframe(); break;
		case ID_VIEW_ZOOM:					pView->OnViewZoom(); break;
		case ID_WINDOW_PAN:     			pView->OnWindowPan(hwnd); break;
		case ID_WINDOW_PAN_LEFT:			pView->OnWindowPanLeft(); break;
		case ID_WINDOW_PAN_RIGHT:			pView->OnWindowPanRight(); break;
		case ID_WINDOW_PAN_UP:				pView->OnWindowPanUp(); break;
		case ID_WINDOW_PAN_DOWN:			pView->OnWindowPanDown(); break;
		case ID_WINDOW_BEST:    			pView->OnWindowBest(); break;
		case ID_WINDOW_NORMAL:				pView->OnWindowNormal(hwnd); break;
		case ID_WINDOW_ZOOMIN:   			pView->OnWindowZoomIn(hwnd); break;
		case ID_WINDOW_ZOOMOUT:     		pView->OnWindowZoomOut(hwnd); break;
		case ID_3DVIEWS_AXONOMETRIC:		pView->On3dViewsAxonometric(); break;
		case ID_3DVIEWS_BACK:				pView->On3dViewsBack(); break;
		case ID_3DVIEWS_BOTTOM:				pView->On3dViewsBottom(); break;
		case ID_3DVIEWS_FRONT:				pView->On3dViewsFront(); break;
		case ID_3DVIEWS_ISOMETRIC:			pView->On3dViewsIsometric(); break;
		case ID_3DVIEWS_LEFT:				pView->On3dViewsLeft(); break;
		case ID_3DVIEWS_OBLIQUE:			pView->On3dViewsOblique(); break;
		case ID_3DVIEWS_PERSPECTIVE:		pView->On3dViewsPerspective(); break;
		case ID_3DVIEWS_RIGHT:				pView->On3dViewsRight(); break;
		case ID_3DVIEWS_TOP:				pView->On3dViewsTop(); break;
//		case ID_BLOCKS_LOAD:				pDoc->OnBlocksLoad(); break;
//		case ID_BLOCKS_UNLOAD:				pDoc->OnBlocksUnload(); break;
		case ID_CLEAR_ACTIVELAYERS:			pDoc->OnClearActiveLayers(); break;
		case ID_CLEAR_ALL:					app.OnFileNew(false); break;
		case ID_CLEAR_ALLLAYERS:			pDoc->OnClearAllLayers(); break;
		case ID_CLEAR_ALLTRACINGS:			pDoc->OnClearAllTracings(); break;
		case ID_CLEAR_MAPPEDTRACINGS:		pDoc->OnClearMappedTracings(); break;
		case ID_CLEAR_VIEWEDTRACINGS:		pDoc->OnClearViewedTracings(); break;
		case ID_CLEAR_WORKINGLAYER:			pDoc->OnClearWorkingLayer(); break;
		case ID_EDIT_IMAGETOCLIPBOARD:		pDoc->OnEditImageToClipboard(); break;
		case ID_EDIT_SEGTOWORK:				pDoc->OnEditSegToWork(); break;
		case ID_EDIT_TRACE:					pDoc->OnEditTrace(); break;
		case ID_EDIT_TRAPDELETE:			pDoc->OnEditTrapDelete(); break;
		case ID_EDIT_TRAPQUIT:				pDoc->OnEditTrapQuit(); break;
		case ID_EDIT_TRAPCOPY:				pDoc->OnEditTrapCopy(); break;
		case ID_EDIT_TRAPCUT:				pDoc->OnEditTrapCut(); break;
		case ID_EDIT_TRAPPASTE:				pDoc->OnEditTrapPaste(); break;
		case ID_EDIT_TRAPWORK:				pDoc->OnEditTrapWork(); break;
		case ID_EDIT_TRAPWORKANDACTIVE:		pDoc->OnEditTrapWorkAndActive(); break;
		case ID_FILE:						pDoc->OnFile(hwnd); break;
		case IDM_FILE_CLOSE:				pDoc->OnFileClose(); break;
		case IDM_FILE_MANAGE:				pDoc->OnFileManage(); break;

		case ID_ACTIVE_ALL:					pDoc->OnActiveAll(); break;
		case ID_STATIC_ALL:					pDoc->OnStaticAll(); break;

		case ID_BITMAP_EDIT:				pDoc->OnBitmapEdit(); break;

		case ID_FILE_PLOT:					pDoc->OnFilePlot(); break;
	//	case ID_FILE_PRINTCURRENTVIEW:		pDoc->OnFilePrintCurrentView(); break;
		case ID_FILE_RECOVER:				pDoc->OnFileRecover(); break;
		case ID_FILE_QUERY:					pDoc->OnFileQuery(); break;
		case IDM_FILE_SAVE:					pDoc->OnFileSave(); break;
		case IDM_FILE_SAVEAS:				pDoc->OnFileSaveAs(); break;
		case ID_FILE_TRACING:				pDoc->OnFileTracing(); break;
		case ID_FILE_CLEAN:					pDoc->OnFileClean(); break;
		case ID_FILE_MRU1:					pDoc->OnMruOpen(1); break;
		case ID_FILE_MRU2:					pDoc->OnMruOpen(2); break;
		case ID_FILE_MRU3:					pDoc->OnMruOpen(3); break;
		case ID_FILE_MRU4:					pDoc->OnMruOpen(4); break;
		case ID_FILE_MRU5:					pDoc->OnMruOpen(5); break;
		case ID_FILE_ARCHIVE:				pDoc->OnFileArchive(); break;
		case ID_IMPORT_DWG:					pDoc->OnImportAutoCAD(); break;						
		case ID_IMPORT_BMP:					pDoc->OnImportBmp(); break;
		case ID_IMPORT_PDF:					pDoc->OnImportPdf(); break;
		case ID_IMPORT_PEG:					pDoc->OnImportPeg(); break;
		case ID_IMPORT_SPREADSHEET:			pDoc->OnImportSpreadsheet(); break;
		case ID_LAYER_ACTIVE:				pDoc->OnLayerActive(); break;
		case ID_LAYER_HIDDEN:				pDoc->OnLayerHidden(true); break;
		case ID_LAYER_MELT:					pDoc->OnLayerMelt(); break;
		case ID_LAYER_STATIC:				pDoc->OnLayerStatic(); break;
		case ID_LAYER_WORK:					pDoc->OnLayerWork(); break;
		case ID_LAYERS_ACTIVEALL:			pDoc->OnLayersActiveAll(); break;
		case ID_LAYERS_STATICALL:			pDoc->OnLayersStaticAll(); break;
		case ID_LAYERS_HIDEALL:				pDoc->OnLayersHideAll(); break;
//		case ID_LAYERS_LOAD:				pDoc->OnFileTracing(); break;
//		case ID_LAYERS_UNLOAD:				pDoc->OnFileTracing(); break;
		case ID_EXPORT_PEG2000:				pDoc->OnExportPeg2000(); break;
		case ID_EXPORT_DWG:					pDoc->OnExportDWG(); break;
		
		case ID_PROFILES_CLASSIC:			pDoc->OnProfileSet(PROFILE_CLASSIC); break;
		case ID_PROFILES_NEW:				pDoc->OnProfileSet(PROFILE_NEW); break;
		case ID_PROFILES_CUSTOM1:			pDoc->OnProfileSet(PROFILE_CUSTOM1); break;
		case ID_PROFILES_CUSTOM2:			pDoc->OnProfileSet(PROFILE_CUSTOM2); break;
		case ID_PROFILES_MANAGER:			pDoc->OnProfileManager(); break;
		case ID_TOOLS_OPTIONS:				pDoc->OnOptions(); break;
		case ID_SETUP:						pDoc->OnSetup(); break;

		case ID_PENS_LOADCOLORS:			pDoc->OnPensLoadColors(); break;
		case ID_PENS_UNLOADCOLORS:			app.PenUseDefaults(); break;
		case ID_PENS_TRANSLATE:				pDoc->OnPensTranslate(); break;
		case ID_PRIM_BREAK:					pDoc->OnPrimBreak(); break;
		case ID_PRIM_DELETE:				pDoc->OnPrimDelete(); break;
		case ID_PRIM_EXTRACTNUM:     		pDoc->OnPrimExtractNum(); break;
		case ID_PRIM_EXTRACTSTR:			pDoc->OnPrimExtractStr(); break;
		case ID_PRIM_MODIFY_ATTRIBUTES:		pDoc->OnPrimModifyAttributes(); break;
		case ID_PRIM_GET_ATTRIBUTES:		pDoc->OnPrimGetAttributes(); break;
		case ID_PRIM_GOTOCENTERPOINT:		pDoc->OnPrimGotoCenterPoint(); break;
		case ID_PRIM_SNAPTOENDPOINT:		pDoc->OnPrimSnapToEndPoint(); break;
		case ID_SEG_BREAK:  				pDoc->OnSegBreak(); break;
		case ID_SEG_DELETE:					pDoc->OnSegDelete(); break;
		case ID_SEG_DELETELAST:				pDoc->OnSegDeleteLast(); break;
		case ID_SEG_EXCHANGE:				pDoc->OnSegExchange(); break;
		case ID_SEG_INSERT:  				pDoc->OnSegInsert(); break;
		case ID_SETUP_FILL_HATCH:   		pDoc->OnSetupFillHatch(); break;
		case ID_SETUP_GOTOPOINT:			pDoc->OnSetupGotoPoint(); break;
//		case ID_SETUP_IMPORT:				pDoc->OnSetupImport(); break;
		case ID_SETUP_MOUSEBUTTONS:			pDoc->OnSetupMouseButtons(); break;
		case ID_SETUP_NOTE:					pDoc->OnSetupNote(); break;		
		case ID_SETUP_PENCOLOR:				pDoc->OnSetupPenColor(); break;
		case ID_SETUP_PENSTYLE:				pDoc->OnSetupPenStyle(); break;
		case ID_SETUP_SAVEPOINT:			pDoc->OnSetupSavePoint(); break;
		case ID_TRACING_CLOAK:				pDoc->OnTracingCloak(); break;
		case ID_TRACING_FUSE:				pDoc->OnTracingFuse(); break;
		case ID_TRACING_MAP:				pDoc->OnTracingMap(); break;
		case ID_TRACING_VIEW:				pDoc->OnTracingView(); break;

		case ID_TRACINGS_MAPALL:			pDoc->OnTracingMapAll(); break;
		case ID_TRACINGS_VIEWALL:			pDoc->OnTracingViewAll(); break;
		case ID_TRACINGS_CLOAKALL:			pDoc->OnTracingCloakAll(); break;

		case ID_TRAPCOMMANDS_BLOCK:			pDoc->OnTrapCommandsBlock(); break;
		case ID_TRAPCOMMANDS_COMPRESS:		pDoc->OnTrapCommandsCompress(); break;
		case ID_TRAPCOMMANDS_EXPAND:		pDoc->OnTrapCommandsExpand(); break;
		case ID_TRAPCOMMANDS_FILTER:		pDoc->OnTrapCommandsFilter(); break;
//		case ID_TRAPCOMMANDS_HIGHLIGHT:		pDoc->OnTrapCommandsHighlight(); break;
		case ID_TRAPCOMMANDS_INVERT:		pDoc->OnTrapCommandsInvert(); break;
		case ID_TRAPCOMMANDS_QUERY:			pDoc->OnTrapCommandsQuery(); break;
		case ID_TRAPCOMMANDS_SQUARE:		pDoc->OnTrapCommandsSquare(); break;
		case ID_TRAPCOMMANDS_UNBLOCK:		pDoc->OnTrapCommandsUnblock(); break;
		case ID_TRAPCOMMANDS_VECTORIZE:		pDoc->OnTrapCommandsVectorize(); break;
		case ID_APP_ABOUT:					app.OnAppAbout(); break;
		case ID_EDIT_CF_IMAGE:				app.OnEditCfImage(); break;
		case ID_EDIT_CF_SEGMENTS:			app.OnEditCfSegments(); break;
		case ID_EDIT_CF_TEXT:				app.OnEditCfText(); break;
		case ID_FILE_EXIT:					app.OnAppExit(); break;
		case IDM_FILE_NEW:					app.OnFileNew(true); break;
		case IDM_FILE_OPEN:					app.OnFileOpen(); break;
		case ID_FILE_RUN:					app.OnFileRun(); break;
		case ID_HELP_CONTENTS:				app.OnHelpContents(); break;
		case ID_MODE_ANNOTATE:				app.OnModeAnnotate(); break;
		case ID_MODE_CUT:					app.OnModeCut(); break;
		case ID_MODE_DIMENSION:				app.OnModeDimension(); break;
		case ID_MODE_DRAW:					app.OnModeDraw(); break;
		case ID_MODE_DRAW2:					app.OnModeDraw2(); break;
		case ID_MODE_BACKGROUNDEDIT:		app.OnModeBackgroundEdit(); break;
		case ID_MODE_EDIT:					app.OnModeEdit(); break;
		case ID_MODE_FIXUP:					app.OnModeFixup(); break;
		case ID_MODE_LETTER:				app.OnModeLetter(); break;
		case ID_MODE_LPD:					app.OnModeLPD(); break;
		case ID_MODE_NODAL:					app.OnModeNodal(); break;
		case ID_MODE_PIPE:					app.OnModePipe(); break;
		case ID_MODE_POWER:					app.OnModePower(); break;
		case ID_MODE_PRIM_EDIT:				app.OnModePrimEdit(); break;
		case ID_MODE_REVISE:				app.OnModeRevise(); break;
		case ID_MODE_SEGEDIT:				app.OnModeSegEdit(); break;
		case ID_MODE_PRIM_MEND:				app.OnModePrimMend(); break;
		case ID_MODE_RLPD:					app.OnModeRLPD(); break;
		case ID_MODE_TRAP:					app.OnModeTrap(); break;
		case ID_TRAPCOMMANDS_ADDSEGMENTS:	app.OnTrapCommandsAddSegments(); break;
		case ID_PENS_EDITCOLORS:			app.PenColorsChoose(); break;
		case ID_PENS_USEDEFAULTS:			app.PenUseDefaults(); break;
		case ID_VIEW_STATEINFORMATION:		app.OnViewStatusInformation(); break;
		case ID_VIEW_MODEINFORMATION:		app.OnViewModeInformation(); break;
		case ID_VIEW_INFORMATIONTOGGLE:		app.OnViewInformationToggle(); break;
		default:
			FORWARD_WM_COMMAND(hwnd, id, hWndCtrl, uCodeNotify, ::DefWindowProc);
	}
}

///<summary>
/// oncreate message handler
///</summary>
BOOL CMainFrame::OnCreate(HWND hwnd, LPCREATESTRUCT)
{
	CDC* pDC = CDC::FromHandle(::GetDC(hwnd));
	
	vpActive.Set(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));

	viewport::SetDeviceWidthInPixels(pDC->GetDeviceCaps(HORZRES));
	viewport::SetDeviceHeightInPixels(pDC->GetDeviceCaps(VERTRES));
	viewport::SetDeviceWidthInInches(double(pDC->GetDeviceCaps(HORZSIZE)) / 25.4);
	viewport::SetDeviceHeightInInches(double(pDC->GetDeviceCaps(VERTSIZE)) / 25.4);
	
//	opengl::Initialize(pDC);

//	if(!app.IsThisSoloWindow())
//		app.CheckDirtyAutoSaveFile();

	return (TRUE);
}

///<summary>
/// ondestroy message handler
///</summary>
void CMainFrame::OnDestroy(HWND hwnd)
{	
	CPegView* pView = CPegView::GetActiveView();
	CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();

	if (pDC != NULL)
	{
		// Stock objects are never left "current" so it is safe to delete whatever the old object is
		pDC->SelectStockObject(BLACK_PEN)->DeleteObject();
		pDC->SelectStockObject(WHITE_BRUSH)->DeleteObject();
	}

	//	opengl::Release();
	
	::ReleaseDC(hwnd, pDC->GetSafeHdc());
	
	app.LineFontRelease();
	app.StrokeFontRelease();
	
	dde::Uninitialize();

	PostQuitMessage(0); 		// Force WM_QUIT message to terminate message loop
}

///<summary>
/// 
///</summary>
#ifdef ENABLE_DRAGNDROP
void CMainFrame::OnDropfiles(HWND, HDROP hDrop)
{
	// TODO: OnDropFiles only for jb1 with no test for wrong file type
	CPegDoc* pDoc = CPegDoc::GetDoc();

	int iFiles = ::DragQueryFile(hDrop, (UINT) -1, 0, 0);

	for (int i = 0; i < iFiles; i++)
	{
		CString strName;
		
		DragQueryFile(hDrop, (UINT) i, strName.GetBuffer(MAX_PATH), MAX_PATH);
		strName.ReleaseBuffer();

		CFileJob fj;
	
		if (fj.OpenForRead(strName))
		{
			CLayer* pLayer = pDoc->WorkLayerGet();
			
			try
			{
				fj.ReadHeader();
				fj.ReadSegs(pLayer);
				fj.Close();
			}
			catch(char* szMessage)
			{
				msgInformation(szMessage);
			}

			detsegs.AddTail(pLayer);
			pDoc->UpdateAllViews(NULL, CPegDoc::HINT_LAYER_SAFE, pLayer);
		}
	}
	DragFinish(hDrop);
}
#endif

///<summary>
/// onkey message handler
///</summary>
void CMainFrame::OnKey(HWND hwnd, UINT nVirtualKey, BOOL bDown, int iRepeat, UINT nFlags)
{
	static int iMove = 0;

	if (bDown)
	{
		if (nVirtualKey == VK_LEFT || nVirtualKey == VK_RIGHT || nVirtualKey == VK_UP || nVirtualKey == VK_DOWN)
		{
			iMove = (nFlags & 0x4000) ? Min(16, ++iMove) : 1;

			CPoint pntPos;
				
			::GetCursorPos(&pntPos);
			::ScreenToClient(hwnd, &pntPos);
			
			switch (nVirtualKey)
			{
				case VK_LEFT:
					pntPos.x -= iMove;
					break;
				
				case VK_UP:
					pntPos.y -= iMove;
					break;

				case VK_RIGHT:
					pntPos.x += iMove;
					break;

				case VK_DOWN:
					pntPos.y += iMove;
					break;
			}
			::ClientToScreen(hwnd, &pntPos);
			::SetCursorPos(pntPos.x, pntPos.y);
		}
		else
		{
			FORWARD_WM_KEYDOWN(hwnd, nVirtualKey, iRepeat, nFlags, ::DefWindowProc) ;
		}
	}
	else
	{
		FORWARD_WM_KEYUP(hwnd, nVirtualKey, iRepeat, nFlags, ::DefWindowProc) ;
	}
}

void CMainFrame::OnMove(HWND, int x, int y)
{
	SendMessage(statusframe.GetHWND(), WM_MOVE, 0, MAKELPARAM(x,y));
	SendMessage(modeframe.GetHWND(), WM_MOVE, 0, MAKELPARAM(x,y));
	SendMessage(odomframe.GetHWND(), WM_MOVE, 0, MAKELPARAM(x,y));
//	SendMessage(BackgroundLayer.GetHWND(), WM_MOVE, 0, MAKELPARAM(x,y));
}

void PaintNormal(HWND hwnd);
void PaintPlot(HWND hwnd);
void PaintSolo(HWND hwnd);

///<summary>
/// paint message handler
///</summary>
void CMainFrame::OnPaint(HWND hwnd)
{
	switch(app.GetPegState())
	{
		case STATE_BITMAP:
		case STATE_BACKGROUND:
		case STATE_NORMAL:
			PaintNormal(hwnd);
			break;
		case STATE_SOLO:
			PaintSolo(hwnd);
			break;
		case STATE_PLOT:
			PaintPlot(hwnd);
			break;
	};
}

void PaintSolo(HWND hwnd)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	CPegView* pView = CPegView::GetActiveView();

	CRect rcUpdRect;
	
	if(::GetUpdateRect(hwnd, &rcUpdRect, TRUE) != 0) 
	{
		CDC* pDC = pView->GetDC();

		pView->ClipDisplay(pDC);

//		pView->OriginDisplay(pDC);
//		pView->BackgroundImageDisplay(pDC);
		grid::Display(pDC);
//		pDoc->LayersDisplayAll(pDC);

		CLayer *pLayer = new CLayer;

		switch(app.GetSoloType())
		{
			case SOLO_TRACING:
			case SOLO_LAYER:
			case SOLO_BLOCK:
				pLayer = pDoc->WorkLayerGet();
				break;
//			case SOLO_BLOCK:
//				pLayer = pDoc->LayersGet("solo:Block");
//				break;
		}

		
		if(pLayer != 0)
			pLayer->Display(pDC, trapsegs.Identify() && !trapsegs.IsEmpty());

		// select all region
		if(pView->IsClipped())
			pView->ClipStartAll();
	}

	::ValidateRect(hwnd, 0);
}

void PaintNormal(HWND hwnd)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	CPegView* pView = CPegView::GetActiveView();

	CRect rcUpdRect;
	
	if(::GetUpdateRect(hwnd, &rcUpdRect, TRUE) != 0) 
	{
		CDC* pDC = pView->GetDC();

		pView->ClipDisplay(pDC);

		pView->OriginDisplay(pDC);
		pView->PivotDisplay(pDC);

//		::InvalidateRect(BackgroundLayer.GetHWND(), NULL, TRUE);
		pView->BackgroundImageDisplay(pDC);

		pDoc->BitmapLayerDisplay(pDC);
		grid::Display(pDC);
		
		pDoc->LayersDisplayAll(pDC);

		// select all region
		if(pView->IsClipped())
			pView->ClipStartAll();
	}

	::ValidateRect(hwnd, 0);
}

void PaintPlot(HWND hwnd)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	CPegView* pView = CPegView::GetActiveView();

	CRect rcUpdRect;
	
	if(::GetUpdateRect(hwnd, &rcUpdRect, TRUE) != 0) 
	{
		CDC* pDC = pView->GetDC();

//		pView->ClipDisplay(pDC);

		if(printStates.GetDispBkgImage())
			pView->BackgroundImageDisplay(pDC);

		pDoc->BitmapLayerDisplay(pDC);
//		pView->OriginDisplay(pDC);
		pDoc->LayersDisplayAll(pDC);

		// display tracing layer names
		if(printStates.GetDispLayer())
			pDoc->DisplayTracingLayerText(pDC);

		pDoc->PrintBox(pDC);

		// select all region
//		if(pView->IsClipped())
//			pView->ClipStartAll();

//		pDoc->BorderLayersDisplayAll(pDC);
	}

	::ValidateRect(hwnd, 0);
}