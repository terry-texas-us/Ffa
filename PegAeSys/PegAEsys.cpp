// PegAEsys.cpp : Defines the class behaviors for the application.

////////////////////////////////////////////////////////////////////////////////////////////////
// INCLUDES
////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "Directory.h"
#include "FileBitmap.h"
#include "Grid.h"
#include "ddeGItms.h"
#include "DlgProcEditOps.h"
#include "FileDWGDirect.h"
#include "Hatch.h"
#include "SubProcAnnotate.h"
#include "SubProcCut.h"
#include "SubProcDimension.h"
#include "SubProcDraw.h"
#include "SubProcDraw2.h"
#include "SubProcBackgroundEdit.h"
#include "SubProcEdit.h"
#include "SubProcEditPrimitive.h"
#include "SubProcEditSegment.h"
#include "SubProcFixup.h"
#include "SubProcLPD.h"
#include "SubProcMendPrim.h"
#include "SubProcNodal.h"
#include "SubProcPower.h"
#include "SubProcPipe.h"
#include "SubProcRLPD.h"
#include "SubProcTrapAdd.h"
#include "SubProcTrapRemove.h"
#include "UnitsString.h"
#include "UserAxis.h"
#include "Dde.h"
#include "Scroll.h"
#include "Solo.h"
#include "PenStyleDefault.h"
#include "HatchDefault.h"
#include "PegWidths.h"
#include "PegColors.h"
#include "Lex.h"

#include "StateInfoFrm.h"
#include "DlgProcPrintPreview.h"
#include "DlgProcPlotPreview.h"

#include "..\\src\\irrXML.h"
#include "ProfileXML.h"

#include "Undo.h"

//using namespace cimg_library;

////////////////////////////////////////////////////////////////////////////////////////////////
// DECLARATIONS
////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProcMain(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcKeyPlan(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcPreview(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK EnumWindowsProc2(HWND hwnd, LPARAM lParam);

// mak - timer
VOID CALLBACK TimerProc(HWND, UINT, UINT_PTR, DWORD);

// mak - status windows
LRESULT CALLBACK WndProcStateInfo(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcModeInfo(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcOdometer(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcBackgroundLayer(HWND, UINT, WPARAM, LPARAM);
extern CInfoStatusFrm statusframe;
extern CInfoModeFrm modeframe;
extern CInfoOdomFrm odomframe;
extern CBackgroundLayer BackgroundLayer;

// print/plot windows
extern CSoloStateFrm soloStateFrame;
extern CStateFrm stateframe;
extern CPlotFrm plotframe;

#include "DlgProcBitmapEdit.h"
extern CBitmapEdit bitmapEdit;

#include "DlgProcBackgroundImage.h"
extern CBackgroundImage backgroundImage;

COLORREF* pColTbl = crHotCols;
double	gbl_dExtNum = 0.;
char	gbl_szExtStr[128]; 
CSeg*	pGinCurSeg;
CPrim*	pGinCurPrim;

//#ifdef ENABLE_UNDO
CUndo undo;
//#endif

CPrimState pstate;
CImportState istate;
CExportState estate;
CViewports vps;
CViewport vpActive;
CModelViews mvs;
CModelView vActive;
CModelView vOver;
CModelTransform mspace;
CSegsTrap trapsegs;
CSegsDet detsegs;
CTMat tmEditSeg;
CSolo solo;
CPrintStates printStates;

extern DWORD	dwMendPrimPtId;
extern CPrim*	pPrimToMend;
extern CPrim*	pPrimToMendCopy;
extern CPnt		ptMendPrimBeg;

extern CRefSys *rsNBuff;
extern CFontDef *fdNBuff;

////////////////////////////////////////////////////////////////////////////////////////////////
// WINMAIN
////////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE,  LPSTR lpCmdLine, int nCmdShow)
{
	TCHAR szClassName[32];
	SecureZeroMemory(szClassName, 32);
	::LoadString(hInstance, IDR_MAINFRAME, szClassName, sizeof(szClassName));
	
	WNDCLASS  wc;
	::ZeroMemory(&wc, sizeof(WNDCLASS));

	// Initialize and register window classes.
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= WndProcMain;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PEGAESYS));
	wc.hCursor			= LoadCursor(0, 0);
	wc.hbrBackground	= GetStockBrush(BLACK_BRUSH);
	wc.lpszMenuName 	= 0; 
	wc.lpszClassName	= szClassName;
	
	if(!RegisterClass(&wc))
		return FALSE;

	// Initialize any globals required prior to handling WM_CREATE message here!
	app.InitInstance(hInstance, nCmdShow);

	// mak - handle command line parameters here
	if(strlen(lpCmdLine) != 0)
	{
		CString strCmdLine = GetCommandLine();

		// mak - convert short pathname to long pathname 
		// Windows 98 or Windows NT 2000 or later
		char LongPath[4096];
		::GetLongPathName(lpCmdLine, LongPath, 4096);

		if(strCmdLine.Find(".exe") != -1)
			CPegDoc::GetDoc()->OpenFile(LongPath);
		else
			CPegDoc::GetDoc()->OpenFile(GetCommandLine());
	}

	// Create the message loop
	MSG msg;
	while(GetMessage(&msg, 0, 0, 0))
	{	// Process accelerator keys for menu commands
		if(!TranslateAccelerator(app.GetSafeHwnd(), app.GetAccTbl(), &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	// Returns the value from PostQuitMessage
	return(msg.wParam);
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//// CPegApp Definitions
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////////////////////////////////
CPegApp::CPegApp()
{
	SetShadowDir("PegAEsys Shadow Files");

	m_bEditCFImage = false;
	m_bEditCFSegments = true;
	m_bEditCFText = true;
	m_bViewStatusInfo = true;		// View state info within the view
	m_bViewModeInfo = true;			// View mode info within the view
	m_bTrapModeAddSegments = true;
    m_nClipboardFormatPegSegs = 0;
	m_dEngLen = 0.;
	m_dEngAngZ = 0.;
	m_dScale = 1.;
	m_pStrokeFontDef = 0;

	m_dScale = 96;
	m_eUnits = Engineering;
	m_iUnitsPrec = 8;

	m_dDimLen = .125;
	m_dDimAngZ = 45.0;

	m_nTimerEvent = 1;
	m_TimerRunning = false;
	m_bIsSoloWindow = false;

//	m_dwProcessId = 0;

	m_ePegState = STATE_NORMAL;
	m_eProfState = PROF_NEW;

	m_nTransFactor = 200;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// The one and only CPegApp object
////////////////////////////////////////////////////////////////////////////////////////////////
CPegApp app;

////////////////////////////////////////////////////////////////////////////////////////////////
// Initialization
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CPegApp::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	m_hInstance = hInstance;
	m_hMenu		= ::LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));
	m_hAccTbl	= LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));

	TCHAR szClassName[32];
	SecureZeroMemory(szClassName, 32);
	::LoadString(m_hInstance, IDR_MAINFRAME, szClassName, sizeof(szClassName));

	HWND hWnd = ::CreateWindow(
			szClassName,						// window class name
			"PegAEsys",							// window caption
			WS_OVERLAPPEDWINDOW |				// window style
			WS_CLIPCHILDREN | WS_CLIPSIBLINGS   //
			| WS_VSCROLL | WS_HSCROLL,	        // 								
			CW_USEDEFAULT,						// initial x position 
			CW_USEDEFAULT,						// initial y position
			CW_USEDEFAULT,						// initial x size
			CW_USEDEFAULT,						// initial y size
			HWND_DESKTOP,						// window has no parent
			m_hMenu,							// window menu handle
			m_hInstance,						// program instance handle
			0);									// no additional information 
	
	m_hWndApp = hWnd;
	CPegView::GetActiveView()->SetHandle(hWnd);

	m_wpMainWnd = (WNDPROC)::GetWindowLong(hWnd, GWL_WNDPROC);

	// Get app path by stripping quotes and program name from command line
	m_strAppPath = ::GetCommandLine();
	Path_UnquoteSpaces(m_strAppPath);

/////////////////////////////////////////////////////////////////////////////////////
// MAK - REMOVE MENU ITEMS THAT ARE IN PROGRESS AND GRAYED
/////////////////////////////////////////////////////////////////////////////////////
#ifndef GL_FUNCTIONALITY
//	::EnableMenuItem(m_hMenu, ID_VIEW_LIGHTING, MF_BYCOMMAND | MF_GRAYED); 
//	::EnableMenuItem(m_hMenu, ID_VIEW_RENDERED, MF_BYCOMMAND | MF_GRAYED); 
//	::EnableMenuItem(m_hMenu, ID_VIEW_WIREFRAME, MF_BYCOMMAND | MF_GRAYED); 
	::RemoveMenu(m_hMenu, ID_VIEW_LIGHTING, MF_BYCOMMAND);
	::RemoveMenu(m_hMenu, ID_VIEW_RENDERED, MF_BYCOMMAND);
//	::RemoveMenu(m_hMenu, ID_VIEW_WIREFRAME, MF_BYCOMMAND);
	::RemoveMenu(m_hMenu, ID_VIEW_SOLID, MF_BYCOMMAND);
#endif // GL_FUNCTIONALITY

	::RemoveMenu(m_hMenu, ID_VIEW_CLIP, MF_BYCOMMAND);
//	::RemoveMenu(m_hMenu, ID_FILE_RUN, MF_BYCOMMAND);

//#ifndef ENABLE_CURSOR
//	HMENU subSetupMenu = ::GetSubMenu(m_hMenu, 5);
//	::RemoveMenu(subSetupMenu, 5, MF_BYPOSITION);
//#endif

//#ifndef ENABLE_BITMAP_PRIM
//	::RemoveMenu(m_hMenu, ID_IMPORT_BMP, MF_BYCOMMAND);
//#endif 
	
//#ifndef ENABLE_DWG_EXPORT
//	::RemoveMenu(m_hMenu, ID_EXPORT_DWG, MF_BYCOMMAND);
//#endif

/////////////////////////////////////////////////////////////////////////////////////

	// mak - set "Manage" as default menu item
	HMENU subMenu = ::GetSubMenu(m_hMenu, 0);
	::SetMenuDefaultItem(subMenu, IDM_FILE_MANAGE, MF_BYCOMMAND);

	// mak - register KeyPlan
	if(!InitKeyPlan())
		return FALSE;

	// mak - register Preview
	if(!InitPreview())
		return FALSE;

	// mak - load Default values
	PenStylesLoad(m_strAppPath + "\\Pens\\LineTypes.txt");
	HatchesLoad(m_strAppPath + "\\Hatches\\DefaultSet.txt");
	PenWidthsLoad(m_strAppPath + "\\Pens\\Widths\\Plotter.txt");
	PenColorsLoad(m_strAppPath + "\\Pens\\Colors\\Default.txt");
	StrokeFontLoad("");

	// mak - initialize globals
	InitGbls(CWnd::FromHandle(hWnd)->GetDC());
	
	// Mru initialize
	doc.DisplayMRU();

	// MAK - set windows file associations
	doc.SetPeg();
	doc.SetJB1();
	doc.SetTra();
//	doc.SetDWG();
//	doc.SetDXF();

	// set background and mode
	SetBackGround(crHotCols[0]);
	SetWindowMode(ID_MODE_DRAW);

	MakeStatusWindow();
	MakeModeWindow();
	MakeOdometerWindow();

//	MakeBackgroundLayer();

	//scrollbar initialization
	InitScrollbar();

	// Load Profiles
	ProfileLoad();

	stateframe.SetHandle(::CreateDialog(m_hInstance, MAKEINTRESOURCE(IDD_STATE_BUTTON), hWnd, DlgProcStatePreview));
	
//	ShowWindow(stateframe.GetHWND(), SW_HIDE);

	soloStateFrame.SetHandle(::CreateDialog(m_hInstance, MAKEINTRESOURCE(IDD_SOLOSTATE_BUTTON), hWnd, DlgProcSoloStatePreview));
	ShowWindow(soloStateFrame.GetHWND(), SW_HIDE);

	// Bitmap Edit Sidebar
	bitmapEdit.SetHandle(::CreateDialog(m_hInstance, MAKEINTRESOURCE(IDD_BITMAP_EDIT), hWnd, DlgProcBitmapEdit));
	ShowWindow(bitmapEdit.GetHWND(), SW_HIDE);

	// Background Image Sidebar
	backgroundImage.SetHandle(::CreateDialog(m_hInstance, MAKEINTRESOURCE(IDD_BACKGROUND_MODE), hWnd, DlgProcBackgroundImage));
	ShowWindow(backgroundImage.GetHWND(), SW_HIDE);

	// i am currently using the slimmed down version without jb2
	//plotframe.SetHandle(::CreateDialog(m_hInstance, MAKEINTRESOURCE(IDD_PLOTPREVIEW), hWnd, DlgProcPlotPreview));
	plotframe.SetHandle(::CreateDialog(m_hInstance, MAKEINTRESOURCE(IDD_PRINTPREVIEW), hWnd, DlgProcPlotPreview));
	ShowWindow(plotframe.GetHWND(), SW_HIDE);

	// The main window has been initialized, so show and update it
	::ShowWindow(hWnd, nCmdShow);
	::UpdateWindow(hWnd);

	// MAK - drag and drop functionality
//#ifdef ENABLE_DRAGNDROP
//	::DragAcceptFiles(hWnd, TRUE);
//#else
	::DragAcceptFiles(hWnd, FALSE);
//#endif

	// Initialize for using DDEML
	dde::Setup(m_hInstance);

	// This is the private data format used to pass segs from one instance of peg  to another
	m_nClipboardFormatPegSegs = RegisterClipboardFormat("PegSegs");

	// initialize "proj:" top view
	CPegView::GetActiveView()->On3dViewsTop();

	//
	InitWorkspace();

	// clip stuff
	CPegView::GetActiveView()->ClipInit();

//	if(!app.IsThisSoloWindow())
//		app.CheckDirtyAutoSaveFile();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//// mak - put pid in registry
	//HKEY hKey;
	//DWORD dwDisp = 0;
	//LPDWORD lpdwDisp = &dwDisp;
	//CString l_strExampleKey = "SOFTWARE\\Fanning\\AutoSave";

	//// create and/or open key
	//if(RegCreateKeyEx(HKEY_CURRENT_USER, l_strExampleKey, 0L, NULL, REG_OPTION_NON_VOLATILE, 
	//							   KEY_ALL_ACCESS, NULL, &hKey, lpdwDisp) == ERROR_SUCCESS)
	//{
	//	char strPID[8];
	//	GetWindowThreadProcessId(app.GetSafeHwnd(), &m_dwProcessId); 
	//	itoa(m_dwProcessId, strPID, 10);
	//	RegSetValueEx(hKey, strPID, 0, REG_SZ, 0, 0);
	//	::RegCloseKey(hKey);
	//}
	////////////////////////////////////////////////////////////////////////////////////////////////////

	// mak - intialize lex
	lex::Init();
	
//	opengl::Initialize(pDC);

	// mak - create line font for on-screen text
	app.LineFontCreate();

	// HACK: to make 0 layer work instead of active
	OnFileNew(false);

//	if(!app.IsThisSoloWindow())
//		app.CheckDirtyAutoSaveFile();

	::InvalidateRect(odomframe.GetHWND(), NULL, TRUE);
	::InvalidateRect(statusframe.GetHWND(), NULL, TRUE);
	::InvalidateRect(modeframe.GetHWND(), NULL, TRUE);
//	::InvalidateRect(BackgroundLayer.GetHWND(), NULL, TRUE);

	CString strPenFile = app.PenColorsGetFile();
	app.PenColorsLoad(app.GetAppPath() + "\\Pens\\Colors\\" + strPenFile);

	BringWindowToTop(m_hWndApp);

		//CImg<unsigned char> img(640,400,1,3); // Define a 640x400 color image with 8 bits per color component.
		//img.fill(0); // Set pixel values to 0 (color : black)
		//unsigned char purple[] = { 255,0,255 }; // Define a purple color
		//img.draw_text("Hello World",100,100,purple); // Draw a purple "Hello world" at coordinates (100,100).
		//img.display("My first CImg code");
		//CBitmap bitmap;
		//bitmap.Attach(img);

	//PDFToImageDrawToHDC("C:\\in.pdf", "C:\\out.tif", NULL, NULL, 1, 0, 0, 24, COMPRESSION_PACKBITS, 70, TRUE, TRUE, -1, -1);

	return TRUE;
}

bool CPegApp::InitKeyPlan()
{
	WNDCLASS wc;
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= WndProcKeyPlan;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= m_hInstance;
	wc.hIcon			= 0;
	wc.hCursor			= ::LoadCursor(0, IDC_CROSS);
	wc.hbrBackground	= (HBRUSH) ::GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName 	= 0; 
	wc.lpszClassName	= "View";

	if(!::RegisterClass(&wc))
		return FALSE;

	return TRUE;
}

bool CPegApp::InitPreview()
{
	WNDCLASS wc;
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= WndProcPreview;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= m_hInstance;
	wc.hIcon			= 0;
	wc.hCursor			= ::LoadCursor(0, IDC_CROSS);
	wc.hbrBackground	= (HBRUSH) ::GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName 	= 0; 
	wc.lpszClassName	= "Preview";

	if (!::RegisterClass(&wc))
		return (FALSE);
	
	return TRUE;
}

void CPegApp::InitWorkspace()
{
	// code added so that the z plane = 0 and lower left is (0, 0)
	view::Initialize();
	CPnt pt;
	pt[0] = view::GetUExt() / 2;
	pt[1] = view::GetVExt() / 2;
	pt[2] = 0.0;

	CPnt4 ptView(pt, 1);
	ModelView_DoTransform(ptView);
	
	if (!Pnt4_IsInView(ptView))
	{	// Redefine the view so targeted position becomes center
		view::SetTarget(pt);
		view::SetEye(view::GetDirection());
		::InvalidateRect(m_hWndApp, 0, TRUE);
		ptView = pt;
		ModelView_DoTransform(ptView);
	}

	// Move the cursor to specified position.
	CPoint pntCurPos = viewport::DoProjection(ptView);
	m_ptCursorPosDev(pntCurPos.x, pntCurPos.y, ptView[2] / ptView[3]);
	m_ptCursorPosWorld = pt;
}

void CPegApp::InitScrollbar()
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
	si.nMin = 0;

	// vertical scrollbar
	si.nPage = scroll::nVScrollThumbSize;
	si.nMax = scroll::nVScrollMax - 1;
	si.nPos = ((scroll::nVScrollMax - scroll::nVScrollThumbSize) / 2);
	SetScrollInfo(app.GetSafeHwnd(), SB_VERT, &si, TRUE);

	// horizontal scrollbar
	si.nPage = scroll::nHScrollThumbSize;
	si.nMax = scroll::nHScrollMax - 1;
	si.nPos = ((scroll::nHScrollMax - scroll::nHScrollThumbSize) / 2);
	SetScrollInfo(app.GetSafeHwnd(), SB_HORZ, &si, TRUE);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// CPegApp message handlers
////////////////////////////////////////////////////////////////////////////////////////////////
void CPegApp::OnAppAbout()
{
	 HINSTANCE hinst = GetWindowInstance(m_hWndApp);
	::DialogBox(hinst, MAKEINTRESOURCE(IDD_ABOUTBOX), m_hWndApp, DlgProcAbout);
}

HWND CPegApp::MakeStatusWindow()
{
	WNDCLASS  wc;
	::ZeroMemory(&wc, sizeof(WNDCLASS));

	// Initialize and register window classes.
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= WndProcStateInfo;
	wc.hInstance		= app.GetInstance();
	wc.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor			= LoadCursor(NULL, IDC_ARROW);   
	wc.hbrBackground	= CreateSolidBrush(RGB(128, 128, 128));
	wc.lpszMenuName 	= 0; 
	wc.lpszClassName	= "state info";
	
	if(!RegisterClass(&wc))
		return FALSE;

	HWND hwndChild = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TRANSPARENT | /*WS_EX_TOPMOST |*/ WS_EX_TOOLWINDOW, 
		"state info", "state info", WS_VISIBLE | WS_POPUP,
		0, 0, 0, 0, app.GetSafeHwnd(), 0, app.GetInstance(), 0);

	MakeWindowTransparent(hwndChild, (unsigned char)m_nTransFactor);
	statusframe.SetHandle(hwndChild);

	return hwndChild;
}

HWND CPegApp::MakeModeWindow()
{
	WNDCLASS  wc;
	::ZeroMemory(&wc, sizeof(WNDCLASS));

	// Initialize and register window classes.
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= WndProcModeInfo;
	wc.hInstance		= app.GetInstance();
	wc.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor			= LoadCursor(NULL, IDC_ARROW);   
	wc.hbrBackground	= CreateSolidBrush(RGB(128, 128, 128));
	wc.lpszMenuName 	= 0; 
	wc.lpszClassName	= "mode info";
	
	if (!RegisterClass(&wc))
		return FALSE;

	HWND hwndChild = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TRANSPARENT | /*WS_EX_TOPMOST |*/ WS_EX_TOOLWINDOW, 
		"mode info", "mode info", WS_VISIBLE | WS_POPUP,
		0, 0, 0, 0, app.GetSafeHwnd(), 0, app.GetInstance(), 0);

	MakeWindowTransparent(hwndChild, (unsigned char)m_nTransFactor);
	modeframe.SetHandle(hwndChild);

	return hwndChild;
}

HWND CPegApp::MakeOdometerWindow()
{
	WNDCLASS  wc;
	::ZeroMemory(&wc, sizeof(WNDCLASS));

	// Initialize and register window classes.
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= WndProcOdometer;
	wc.hInstance		= app.GetInstance();
	wc.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor			= LoadCursor(NULL, IDC_ARROW);   
	wc.hbrBackground	= CreateSolidBrush(RGB(128, 128, 128));
	wc.lpszMenuName 	= 0; 
	wc.lpszClassName	= "odometer info";
	
	if (!RegisterClass(&wc))
		return FALSE;

	HWND hwndChild = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TRANSPARENT | /*WS_EX_TOPMOST |*/ WS_EX_TOOLWINDOW, 
		"odometer info", "odometer info", WS_VISIBLE | WS_POPUP,
		0, 0, 0, 0, app.GetSafeHwnd(), 0, app.GetInstance(), 0);

	MakeWindowTransparent(hwndChild, (unsigned char)m_nTransFactor);
	odomframe.SetHandle(hwndChild);

	ShowWindow(hwndChild, SW_HIDE);

	return hwndChild;
}

HWND CPegApp::MakeBackgroundLayer()
{
	WNDCLASS  wc;
	::ZeroMemory(&wc, sizeof(WNDCLASS));

	// Initialize and register window classes.
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= WndProcBackgroundLayer;
	wc.hInstance		= app.GetInstance();
	wc.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor			= LoadCursor(NULL, IDC_ARROW);   
	wc.hbrBackground	= CreateSolidBrush(RGB(0, 0, 0));
//	wc.hbrBackground	= CreateSolidBrush(RGB(255, 255, 255));
	wc.lpszMenuName 	= 0; 
	wc.lpszClassName	= "background layer";
	
	if (!RegisterClass(&wc))
		return FALSE;

	HWND hwndChild = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TRANSPARENT | /*WS_EX_TOPMOST |*/ WS_EX_TOOLWINDOW, 
		"background layer", "background layer", WS_VISIBLE | WS_POPUP,
		0, 0, 0, 0, app.GetSafeHwnd(), 0, app.GetInstance(), 0);

	MakeWindowTransparent(hwndChild, (unsigned char)m_nTransFactor);
//	MakeWindowTransparent2(hwndChild, RGB(255,255,255));
	BackgroundLayer.SetHandle(hwndChild);

	ShowWindow(hwndChild, SW_HIDE);

	return hwndChild;
}

CPnt CPegApp::CursorPosGet()
{
	CPoint pntCurPos;
	::GetCursorPos(&pntCurPos);
	::ScreenToClient(m_hWndApp, &pntCurPos);
	
	CPnt pt(double(pntCurPos.x), double(pntCurPos.y), m_ptCursorPosDev[2]);
	if (pt != m_ptCursorPosDev) 
	{
		m_ptCursorPosDev = pt;
		m_ptCursorPosWorld = m_ptCursorPosDev;
		
		viewport::DoProjectionInverse(m_ptCursorPosWorld);
		
		m_ptCursorPosWorld = view::GetMatrixInverse() * m_ptCursorPosWorld;
		m_ptCursorPosWorld = grid::UserSnapPt(m_ptCursorPosWorld);
	}

	return (m_ptCursorPosWorld);
}

void CPegApp::StartAutoSave()
{
	if(!m_TimerRunning)
	{
		::SetTimer(m_hWndApp, m_nTimerEvent, 600000, TimerProc);
		//::SetTimer(m_hWndApp, m_nTimerEvent, 4000, TimerProc);
		m_TimerRunning = true;
	}
}

//static bool bPIDFound = false;
//
//void CPegApp::CheckDirtyAutoSaveFile()
//{
//	CPegDoc *pDoc = CPegDoc::GetDoc();
//
//	WIN32_FIND_DATA FindFileData;
//	HANDLE hFind;
//
//	// get temp path name for file
//	TCHAR szPath[MAX_PATH];
//	GetTempPath(MAX_PATH, szPath);
//	CString strPathName = szPath;
//	strPathName.Append("*(Recovered)*");
//
//	hFind = ::FindFirstFile(strPathName, &FindFileData);
//
//	if(hFind != INVALID_HANDLE_VALUE) 
//	{
//		char szFilter[250];
//		SecureZeroMemory(szFilter, 250);
//		::LoadString(app.GetInstance(), IDS_MSG_RECOVER, szFilter, sizeof(szFilter));
//
//		// do while loop will continue to look for all temp peg files
//		do
//		{
//			bPIDFound = false;
//			CString strOpenFile = szPath;
//			strOpenFile.Append(FindFileData.cFileName);
//
//			// mak - remove pid info from ish
//			CString strTheFileName = FindFileData.cFileName;
//			int index = strTheFileName.Find("-");
//			CString strFilePID = strTheFileName.Left(index);
//
//			char *szPID = strFilePID.GetBuffer();
//			EnumWindows(EnumWindowsProc2, (LPARAM)szPID);
//
//			if(!bPIDFound)
//			{
//				int nRet = MessageBox(m_hWndApp, szFilter, FindFileData.cFileName, MB_YESNO | MB_ICONASTERISK | MB_APPLMODAL);
//				
//				if(nRet != IDCANCEL)
//				{
//					if(nRet == IDYES)
//					{
//						pDoc->OpenFile(strOpenFile);
//						if(!pDoc->OpenFileSaveAs())
//							pDoc->OpenFileClose(false);
//					}
//					
//					::DeleteFile(strOpenFile);
//				}
//			}
//		}
//		while(FindNextFile(hFind, &FindFileData) != 0);
//
//		FindClose(hFind);
//	}
//}

//BOOL CALLBACK EnumWindowsProc2(HWND hwnd, LPARAM lParam)
//{
//	DWORD ProcessId;
//	char szPID[8];
//	strcpy(szPID, (char*)lParam);
//
//	GetWindowThreadProcessId(hwnd, &ProcessId);
//	char szCurPID[8];
//	itoa(ProcessId, szCurPID, 10);
//
//	if(strcmp(szCurPID, szPID) == 0)
//	{
//		bPIDFound = true;
//		return false;
//	}
//	else
//	{
//		return true;
//	}
//}

VOID CALLBACK TimerProc(HWND, UINT, UINT_PTR, DWORD)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	CString strFilePath = pDoc->GetPathName();
	WORD wFileType = app.GetFileTypeFromPath(strFilePath);

	//::MessageBox(app.GetSafeHwnd(), "Timer was called.", "Time Callback", 0);

	if(wFileType == CPegDoc::FILE_TYPE_PEG)
	{
//		// get temp path name for file
//		TCHAR szPath[MAX_PATH];
//		//GetTempPath(MAX_PATH, szPath);
//		CString strPathName = szPath;
//		CString ShadowPath = app.GetShadowDir();
//
//		// append PID
////		DWORD ProcessId;
//		//char strPID[8];
////		GetWindowThreadProcessId(app.GetSafeHwnd(), &ProcessId); 
////		itoa(ProcessId, strPID, 10);
//	/*	itoa(app.GetPID(), strPID, 10);
//		strPathName.Append(strPID);
//		strPathName.Append("-");
//*/
//		// append "recovered"
//		//strPathName.Append("(Recovered)");
//
//		// append file name
//		CString strName = strFilePath;
//		strName = strName.Right(strName.GetLength() - strName.ReverseFind('\\'));
//		//strName.TrimLeft('\\');
//		//strPathName.Append(strName);
//		strShadowPath.Append(strName);

//		MessageBox(0, "debug 1", 0, 0);
		pDoc->WriteAutosaveFile();
//		MessageBox(0, "debug 2", 0, 0);

//		if(bSuccess)
//		{
////			pDoc->OpenPegAutoSave(strPathName);
////			app.SetAutoSaveFileName(strPathName);
//		}
	}
}

void CPegApp::KillAutoSave()
{
	::KillTimer(m_hWndApp, m_nTimerEvent);
	m_TimerRunning = false;
	::DeleteFile(m_strAutoSaveFile);
}

//void CPegApp::RestoreAutoSave()
//{
//
//}

///<summary>
///Positions cursor at targeted position.
///</summary>
void CPegApp::CursorPosSet(const CPnt& pt)
{
	CPnt4 ptView(pt, 1);

	ModelView_DoTransform(ptView);
	
	if (!Pnt4_IsInView(ptView))
	{	// Redefine the view so targeted position becomes center
		view::SetTarget(pt);
		view::SetEye(view::GetDirection());
		
		::InvalidateRect(m_hWndApp, 0, TRUE);
		
		ptView = pt;
		ModelView_DoTransform(ptView);
	}

	// Move the cursor to specified position.
	CPoint pntCurPos = viewport::DoProjection(ptView);
	m_ptCursorPosDev(pntCurPos.x, pntCurPos.y, ptView[2] / ptView[3]);
	m_ptCursorPosWorld = pt;
	
	::ClientToScreen(m_hWndApp, &pntCurPos);
	::SetCursorPos(pntCurPos.x, pntCurPos.y);
}

void CPegApp::DisplayPoint(CDC* pDC, CPnts *pts, DWORD dwMask)
{
	if((app.m_iModeId == ID_MODE_NODAL) || (app.m_iModeId == ID_MODE_NODALR))
	{
		// save pen state
		int pStateSave = pstate.Save();
		PENCOLOR nColor = pstate.PenColor();

		// Set brush to hollow
		LOGBRUSH logbrush;
		logbrush.lbStyle = BS_HOLLOW;
		HBRUSH hbrush = CreateBrushIndirect(&logbrush);
		HBRUSH holdbrush = (HBRUSH)pDC->SelectObject((HBRUSH)hbrush);

		for(int i = 0; i < pts->GetSize(); i++)
		{
			CPnt4 ptView(pts->GetAt(i), 1);

			ModelView_DoTransform(ptView);

			int pntSize = 4;
			
			CPoint pntCurPos = viewport::DoProjection(ptView);

			if(btest(dwMask, i))
			{
				pstate.SetPen(pDC, trapsegs.PenColor(), 1);
//				pstate.SetBrush(pDC, trapsegs.PenColor());
			}
			else
			{
				pstate.SetPen(pDC, nColor, 1);
//				pstate.SetBrush(pDC, nColor);
			}
				
			// draw circle
			pDC->Ellipse(pntCurPos.x - (pntSize / 2), pntCurPos.y - (pntSize / 2), 
						 pntCurPos.x + (pntSize / 2), pntCurPos.y + (pntSize / 2));
		}

		pDC->SelectObject((HBRUSH)holdbrush);

		// restore pen state
		pstate.Restore(pDC, pStateSave);
	}
}

WORD CPegApp::GetFileTypeFromPath(const CString& strPathName)
{
	int nExt = strPathName.ReverseFind('.');
	
	if (nExt != - 1)
	{
		CString strExt = strPathName.Mid(nExt);
		
		if (strExt.CompareNoCase(".blk") == 0)
			return (CPegDoc::FILE_TYPE_BLOCK);
		else if (strExt.CompareNoCase(".dwg") == 0)
			return (CPegDoc::FILE_TYPE_DWG);
		else if (strExt.CompareNoCase(".dxf") == 0)
			return (CPegDoc::FILE_TYPE_DXF);
		else if (strExt.CompareNoCase(".ffa") == 0)
			return (CPegDoc::FILE_TYPE_FFA);
		else if (strExt.CompareNoCase(".peg") == 0)
			return (CPegDoc::FILE_TYPE_PEG);
		else if (strExt.CompareNoCase(".tra") == 0)
			return (CPegDoc::FILE_TYPE_TRACING);
		else if (strExt.CompareNoCase(".jb1") == 0)
			return (CPegDoc::FILE_TYPE_JOB);
		else if (strExt.CompareNoCase(".bmp") == 0)
			return (CPegDoc::FILE_TYPE_BMP);
	}
	return (CPegDoc::FILE_TYPE_NONE);
}

void CPegApp::HatchesLoad(const CString& strFileName)
{
	CStdioFile fl;
		
	if(fl.Open(strFileName, CFile::modeRead | CFile::typeText))
	{
		fl.Close();
		HatchesLoadFile(strFileName);
	}
	else
	{
		HatchesLoadInternal();
	}
}

// Loads the hatch table.
void CPegApp::HatchesLoadFile(const CString& strFileName)
{	
	CFileException e;
	CStdioFile fl;
	
	if (!fl.Open(strFileName, CFile::modeRead | CFile::typeText, &e))
		return;

	char	szLn[128];
	double	dTotStrsLen;
	int 	iNmbEnts, iNmbStrsId;
	
	char szValDel[] = ",\0";
	char szValDel2[] = "\n";
	int iHatId = 0;
	int iNmbHatLns = 0;
	int iTblId = 0;
	
	while (fl.ReadString(szLn, sizeof(szLn) - 1) != 0) 
	{
		if (*szLn == '!')// New Hatch index
		{
			if (iHatId != 0)
				hatch::fTableValue[hatch::iTableOffset[iHatId]] = float(iNmbHatLns);
			hatch::iTableOffset[++iHatId] = iTblId++;
			iNmbHatLns = 0;
			hatch::HatchText[iHatId] = strtok(szLn, szValDel2);

			//format hatch name text
			hatch::HatchText[iHatId] = hatch::HatchText[iHatId].TrimLeft('!');
			hatch::HatchText[iHatId] = hatch::HatchText[iHatId].TrimLeft();
			hatch::HatchText[iHatId] = hatch::HatchText[iHatId].TrimRight();
		}
		else 
		{
			iNmbStrsId = iTblId;
			iTblId += 2;
			iNmbEnts = 0;
			dTotStrsLen = 0.;
			char* pTok = strtok(szLn, szValDel);
			while (pTok != 0) 
			{
				hatch::fTableValue[iTblId] = float(atof(pTok));
				iNmbEnts++;
				if (iNmbEnts >= 6)
					dTotStrsLen = dTotStrsLen + hatch::fTableValue[iTblId];
				iTblId++;
				pTok = strtok(0, szValDel);
			}
			hatch::fTableValue[iNmbStrsId++] = float(iNmbEnts - 5);
			hatch::fTableValue[iNmbStrsId] = float(dTotStrsLen);
			iNmbHatLns++;
		}
	}

	hatch::iTableSize = iHatId;
}

void CPegApp::HatchesLoadInternal()
{
	char	szLn[128];
	double	dTotStrsLen;
	int 	iNmbEnts, iNmbStrsId;
	
	char szValDel[] = ",\0";
	char szValDel2[] = "\n";
	int iHatId = 0;
	int iNmbHatLns = 0;
	int iTblId = 0;
	
	for(int i = 0; i < 160; i++)
	{
		strcpy(szLn, strHatch[i].GetBuffer());

		if(*szLn == '!')// New Hatch index
		{
			if (iHatId != 0)
				hatch::fTableValue[hatch::iTableOffset[iHatId]] = float(iNmbHatLns);

			hatch::iTableOffset[++iHatId] = iTblId++;
			iNmbHatLns = 0;
			hatch::HatchText[iHatId] = strtok(szLn, szValDel2);

			//format hatch name text
			hatch::HatchText[iHatId] = hatch::HatchText[iHatId].TrimLeft('!');
			hatch::HatchText[iHatId] = hatch::HatchText[iHatId].TrimLeft();
			hatch::HatchText[iHatId] = hatch::HatchText[iHatId].TrimRight();
		}
		else 
		{
			iNmbStrsId = iTblId;
			iTblId += 2;
			iNmbEnts = 0;
			dTotStrsLen = 0.;
			char* pTok = strtok(szLn, szValDel);
			while (pTok != 0) 
			{
				hatch::fTableValue[iTblId] = float(atof(pTok));
				iNmbEnts++;
				if (iNmbEnts >= 6)
					dTotStrsLen = dTotStrsLen + hatch::fTableValue[iTblId];
				iTblId++;
				pTok = strtok(0, szValDel);
			}
			hatch::fTableValue[iNmbStrsId++] = float(iNmbEnts - 5);
			hatch::fTableValue[iNmbStrsId] = float(dTotStrsLen);
			iNmbHatLns++;
		}
	}

	hatch::iTableSize = iHatId;
}

CPnt CPegApp::HomePointGet(int i) const
{
	if (i >= 0 && i < 9)
		return (m_ptHomePoint[i]);
	
	return (ORIGIN);
}

void CPegApp::HomePointSave(int i, const CPnt& pt)
{
	if (i >= 0 && i < 9)
		m_ptHomePoint[i] = pt;
}

void CPegApp::ImportFilter()
{
	CPegDoc *pDoc = CPegDoc::GetDoc();
	CPegView *pView = CPegView::GetActiveView();

//	if(istate.GetTranslatePens())
//	{
//		TCHAR szPath[MAX_PATH];
//		char *pStr;
//		GetModuleFileName(app.GetInstance(), szPath, MAX_PATH);
//		pStr = strrchr(szPath, '\\');
//		if (pStr != NULL)
//		*(++pStr)='\0';
//
//		CString s3 = szPath;
//		s3 += "Pens\\Colors\\";
//		s3 += istate.GetTranslateFile();
//
//		app.PenColorsLoad(s3);
////		app.SetBackGround(crHotCols[0]);
//		app.SetBackGround(crHotCols[app.BackgroundColorGet()]);
//		pDoc->UpdateAllViews(NULL, 0L, NULL);
//	}

	if(istate.GetUnblockInserts())
		pDoc->UnblockToCompressed();

	if(istate.GetPurgeBlocks())
		pDoc->BlksRemoveUnused();

	if(istate.GetModifyFont())
		pDoc->ConvertFontToStroke();

	// fix so that it only scales working on paper space
	if(istate.GetScale())
		pDoc->ImportScaleSet();

	if(istate.GetKillLayers())
		pDoc->LayersRemoveEmpty();

	if(istate.GetOmitLayers())
		pDoc->LayersOmitHidden();

	if(istate.GetResolveCommands())
		pDoc->ResolveCommands();

	if(istate.GetHardPens())
		pDoc->ConvertByLayerPens();

	if(istate.GetHardStyle())
		pDoc->ConvertByLayerStyle();

	if(istate.GetPolylines())
		pDoc->ConvertPolylines();

	pView->OnWindowBest();
}

//Initializes all peg global sections to their default (startup) values.
void CPegApp::InitGbls(CDC* pDC)
{
	CCharCellDef ccd;
	pstate.SetCharCellDef(ccd);

	CFontDef fd;
	pstate.SetFontDef(pDC, fd);

	AppSetGinCur();
	pstate.SetPen(pDC, 1, 1);
}

// Creates the font used for all overlaid text display. 
// This is primarily the mode line and status line. 
// Should be called at start-up and every time window is resized.
void CPegApp::LineFontCreate()
{	
	LineFontRelease();
	
	m_pFontApp = new CFont;
	m_pFontApp->CreateStockObject(ANSI_VAR_FONT);
}

void CPegApp::LineFontRelease()
{
	if (m_pFontApp != 0)
	{
		m_pFontApp->DeleteObject();
		delete m_pFontApp;
		m_pFontApp = 0;
	}
}

void CPegApp::ModifyMenu(UINT uPos, LPCTSTR lpNewItem)
{
	if (m_hMenu != 0)
	{
		HMENU hMenu = ::GetSubMenu(m_hMenu, uPos);
		::ModifyMenu(m_hMenu, uPos, MF_BYPOSITION | MF_STRING | MF_POPUP, UINT_PTR(hMenu), lpNewItem);
		::DrawMenuBar(m_hWndApp);
	}
}

void CPegApp::PenColorsChoose()
{
	CHOOSECOLOR cc;

	::ZeroMemory(&cc, sizeof(CHOOSECOLOR));
	cc.lStructSize = sizeof(CHOOSECOLOR);
	
	cc.rgbResult = crHotCols[pstate.PenColor()];
	cc.lpCustColors = crHotCols;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT | CC_SOLIDCOLOR;
	::ChooseColor(&cc);

	cc.rgbResult = crWarmCols[pstate.PenColor()];
	cc.lpCustColors = crWarmCols;
	::ChooseColor(&cc);
//	SetBackGround(crHotCols[0]);
	SetBackGround(crHotCols[app.BackgroundColorGet()]);
	
	CPegDoc::GetDoc()->UpdateAllViews(NULL, 0L, NULL);
}

// Loads the color table.
void CPegApp::PenColorsLoad(const CString& strFileName)
{	
	CStdioFile fl;
	if (fl.Open(strFileName, CFile::modeRead | CFile::typeText)) 
	{
		char	pBuf[128];
		LPSTR	pId, pRed, pGreen, pBlue;
		
		while (fl.ReadString(pBuf, sizeof(pBuf) - 1) != 0 && strnicmp(pBuf, "<Colors>", 8) != 0);
		
		while (fl.ReadString(pBuf, sizeof(pBuf) - 1) != 0 && *pBuf != '<')
		{
			pId = strtok(pBuf, "=");
			pRed = strtok(0, ",");
			pGreen = strtok(0, ",");
			pBlue = strtok(0, ",");
			crHotCols[atoi(pId)] = RGB(atoi(pRed), atoi(pGreen), atoi(pBlue));
			pRed = strtok(0, ",");
			pGreen = strtok(0, ",");
			pBlue = strtok(0, "\n");
			crWarmCols[atoi(pId)] = RGB(atoi(pRed), atoi(pGreen), atoi(pBlue));
		}
	}
}

void CPegApp::PenColorsLoadTemp(const CString& strFileName)
{	
	CStdioFile fl;
	if (fl.Open(strFileName, CFile::modeRead | CFile::typeText)) 
	{
		char	pBuf[128];
		LPSTR	pId, pRed, pGreen, pBlue;
		
		while (fl.ReadString(pBuf, sizeof(pBuf) - 1) != 0 && strnicmp(pBuf, "<Colors>", 8) != 0);
		
		while (fl.ReadString(pBuf, sizeof(pBuf) - 1) != 0 && *pBuf != '<')
		{
			pId = strtok(pBuf, "=");
			pRed = strtok(0, ",");
			pGreen = strtok(0, ",");
			pBlue = strtok(0, ",");
			crHotColsTemp[atoi(pId)] = RGB(atoi(pRed), atoi(pGreen), atoi(pBlue));
			pRed = strtok(0, ",");
			pGreen = strtok(0, ",");
			pBlue = strtok(0, "\n");
			crWarmColsTemp[atoi(pId)] = RGB(atoi(pRed), atoi(pGreen), atoi(pBlue));
		}
	}
}

void CPegApp::PenColorsSetFile(CString strFilename)
{
	m_strColorFile = strFilename;
}

CString	CPegApp::PenColorsGetFile()
{
	return m_strColorFile;
}

void CPegApp::BackgroundColorSet(int nIndex)
{
	m_nBkgdColor = nIndex;
	SetBackGround(crHotCols[nIndex]);
}

int CPegApp::BackgroundColorGet()
{
	return m_nBkgdColor;
}

void CPegApp::PenLoadDefaultWidths()
{
	dPWids[0] = 0.; 
	dPWids[1] = .0075;
	dPWids[2] = .015; 
	dPWids[3] = .0225; 
	dPWids[4] = .03;
	dPWids[5] = .0075;
	dPWids[6] = .015;
	dPWids[7] = .0225; 
	dPWids[8] = .03;
	dPWids[9] = .0075; 
	dPWids[10] = .015; 
	dPWids[11] = .0225; 
	dPWids[12] = .03; 
	dPWids[13] = .06; 
	dPWids[14] = .10; 
	dPWids[15] = .0075; 
	dPWids[16] = .0075;
}

void CPegApp::PenUseDefaults()
{
	//CPegDoc *pDoc = CPegDoc::GetDoc();

	//// default colors
	//pDoc->PenColorsLoadDefault();

	//// default widths
	//PenLoadDefaultWidths();

	//// default style
	//PenStylesLoadInternal();
}

void CPegApp::PenStylesLoad(const CString &strFileName)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	if(pDoc == NULL)
		return;

	CStdioFile fl;
	
	if(fl.Open(strFileName, CFile::modeRead | CFile::typeText))
	{
		fl.Close();
		PenStylesLoadFile(strFileName);
	}
	else
	{
		PenStylesLoadInternal();
	}
}

///<summary>Loads the PenStyle table.</summary>
void CPegApp::PenStylesLoadFile(const CString& strFileName)
{	
	CPegDoc* pDoc = CPegDoc::GetDoc();
	if (pDoc == NULL)
		return;

	CStdioFile fl;
	
	// TODO check for existance of table  - crashes on 99 otherwise ands no pen dlg box.
		
	if (fl.Open(strFileName, CFile::modeRead | CFile::typeText)) 
	{
		// mak - remove existing styles
		pDoc->PenStylesClear();

		CString strDescription;
		CString strName;
		char pBuf[128];
		
		WORD wLensMax = 8;
		double* pLen = new double[wLensMax];
		
		while (fl.ReadString(pBuf, sizeof(pBuf) - 1) != 0)
		{
			int iId = atoi(strtok(pBuf, "="));
			strName = strtok(0, ",");
			strDescription = strtok(0, "\n");
			fl.ReadString(pBuf, sizeof(pBuf) - 1);
			
			WORD wLens = WORD(atoi(strtok(pBuf,",\n")));
			
			if (wLens > wLensMax)
			{
				delete [] pLen;
				pLen = new double[wLens];
				wLensMax = wLens;
			}
			for (WORD w = 0; w < wLens; w++)
				pLen[w] = atof(strtok(0, ",\n"));
			
			pDoc->PenStylesInsert(iId, new CPenStyle(strName, strDescription, wLens, pLen));
		}
		delete [] pLen;
	}
}

void CPegApp::PenStylesLoadInternal()
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	if (pDoc == NULL)
		return;
	
	// TODO check for existance of table  - crashes on 99 otherwise ands no pen dlg box.
		
	// mak - remove existing styles
	pDoc->PenStylesClear();

	CString strDescription;
	CString strName;
	char pBuf[128];
	
	WORD wLensMax = 8;
	double* pLen = new double[wLensMax];
	
	for(int i = 0; i < 83; i++)
	{
		strcpy(pBuf, strStyle[i].GetBuffer());

		int iId = atoi(strtok(pBuf, "="));
		strName = strtok(0, ",");
		strDescription = strtok(0, "\n");
		
		strcpy(pBuf, strStyle[++i].GetBuffer());
		
		WORD wLens = WORD(atoi(strtok(pBuf,",\n")));
		
		if (wLens > wLensMax)
		{
			delete [] pLen;
			pLen = new double[wLens];
			wLensMax = wLens;
		}
		for (WORD w = 0; w < wLens; w++)
			pLen[w] = atof(strtok(0, ",\n"));
		
		pDoc->PenStylesInsert(iId, new CPenStyle(strName, strDescription, wLens, pLen));
	}

	delete [] pLen;
}

void CPegApp::PenWidthsLoad(const CString& strFileName)
{	
	CStdioFile fl;
	
	if (fl.Open(strFileName, CFile::modeRead | CFile::typeText)) 
	{
		char pBuf[32];
		
		while (fl.ReadString(pBuf, sizeof(pBuf) - 1) != 0)
		{
			int iId = atoi(strtok(pBuf, "="));
			double dVal = atof(strtok(0, ",\n"));
			if (iId >= 0 && iId < sizeof(dPWids) / sizeof(dPWids[0]))
				dPWids[iId] = dVal;
		}
	}
}

void CPegApp::ProfileWrite(EProfState eState)
{
	// open correct file
	CString s3 = GetAppPath() + "\\Profiles";
	switch(eState)
	{
		default:
		case PROF_CUSTOM1:	s3.Append("\\Custom1.txt"); break;
		case PROF_CUSTOM2:	s3.Append("\\Custom2.txt"); break;
	}

	ofstream outFile(s3);

	// header
	outFile << "<?xml version=\"1.0\"?>" << std::endl;
	outFile << "<config>" << std::endl;

	// data
	ProfileXMLAnnotateWriter(outFile);
	ProfileXMLAppearanceWriter(outFile);
	ProfileXMLAxonometricWriter(outFile);
	ProfileXMLDimensionWriter(outFile);
	ProfileXMLDrawWriter(outFile);
	ProfileXMLDraw2Writer(outFile);
	ProfileXMLEditWriter(outFile);
	ProfileXMLFixupWriter(outFile);
	ProfileXMLGridWriter(outFile);
	ProfileXMLHatchWriter(outFile);
	ProfileXMLImportWriter(outFile);
	ProfileXMLIsometricWriter(outFile);
	ProfileXMLLPDWriter(outFile);
	ProfileXMLMouseWriter(outFile);
	ProfileXMLNoteWriter(outFile);
	ProfileXMLObliqueWriter(outFile);
	ProfileXMLPensWriter(outFile);
	ProfileXMLPerspectiveWriter(outFile);
	ProfileXMLPipeWriter(outFile);
	ProfileXMLPrintWriter(1, outFile);
	ProfileXMLPrintWriter(2, outFile);
	ProfileXMLPrintWriter(3, outFile);
	ProfileXMLPrintWriter(4, outFile);
	ProfileXMLRLPDWriter(outFile);
	ProfileXMLScaleWriter(outFile);
	ProfileXMLScrollWriter(outFile);
	ProfileXMLUnitsWriter(outFile);
	ProfileXMLUserAxisWriter(outFile);
	ProfileXMLViewWriter(outFile);

	// footer
	outFile << "</config>";

	outFile.close();
}

void CPegApp::ProfileLoad()
{
	HKEY hKey;
	CString strKey = "SOFTWARE\\Fanning\\Profile";
	TCHAR szData[1024];
	DWORD buffSize = 1024;

	RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Fanning\\Profile", 0, KEY_ALL_ACCESS, &hKey);
	LONG lResult = RegQueryValueEx(hKey, "ProfState", NULL, NULL, (LPBYTE)szData, &buffSize);
	if(lResult == ERROR_SUCCESS)
	{
		CString strTest = szData;

		if(strTest == "new")
		{
			ProfileSetState(PROF_NEW);
			::CheckMenuRadioItem(GetSubMenu(5), ID_PROFILES_CLASSIC, ID_PROFILES_CUSTOM2, ID_PROFILES_NEW, MF_BYCOMMAND);
		}
		else if(strTest == "customone")
		{
			ProfileSetState(PROF_CUSTOM1);
			::CheckMenuRadioItem(GetSubMenu(5), ID_PROFILES_CLASSIC, ID_PROFILES_CUSTOM2, ID_PROFILES_CUSTOM1, MF_BYCOMMAND);
		}
		else if(strTest == "customtwo")
		{
			ProfileSetState(PROF_CUSTOM2);
			::CheckMenuRadioItem(GetSubMenu(5), ID_PROFILES_CLASSIC, ID_PROFILES_CUSTOM2, ID_PROFILES_CUSTOM2, MF_BYCOMMAND);
		}
		else //"classic"
		{
			ProfileSetState(PROF_CLASSIC);
			::CheckMenuRadioItem(GetSubMenu(5), ID_PROFILES_CLASSIC, ID_PROFILES_CUSTOM2, ID_PROFILES_CLASSIC, MF_BYCOMMAND);
		}

		RegCloseKey(hKey);

		// load the correct profile
		CString s3 = GetAppPath() + "\\Profiles";
		switch(ProfileGetState())
		{
			case PROF_NEW:		s3.Append("\\New.txt"); break;
			case PROF_CUSTOM1:	s3.Append("\\Custom1.txt"); break;
			case PROF_CUSTOM2:	s3.Append("\\Custom2.txt"); break;
			case PROF_CLASSIC:	s3.Append("\\Classic.txt"); break;
		}

		ProfileLoadFromFile(s3);
	}
	else
		CPegDoc::GetDoc()->OnProfileSet(PROFILE_NEW);
}

void CPegApp::ProfileLoadFromFile(CString strFilename)
{
	xml::IrrXMLReader* xml = xml::createIrrXMLReader(strFilename);

	// parse the file until end reached
	while(xml && xml->read())
	{
		switch(xml->getNodeType())
		{
			case xml::EXN_ELEMENT:
			{
				if(!strcmp("rlpd", xml->getNodeName()))
					ProfileXMLRLPD(xml);
				else if(!strcmp("draw", xml->getNodeName()))
					ProfileXMLDraw(xml);
				else if(!strcmp("draw2", xml->getNodeName()))
					ProfileXMLDraw2(xml);
				else if(!strcmp("annotate", xml->getNodeName()))
					ProfileXMLAnnotate(xml);
				else if(!strcmp("pens", xml->getNodeName()))
					ProfileXMLPens(xml);
				else if(!strcmp("edit", xml->getNodeName()))
					ProfileXMLEdit(xml);
				else if(!strcmp("lpd", xml->getNodeName()))
					ProfileXMLLPD(xml);
				else if(!strcmp("scale", xml->getNodeName()))
					ProfileXMLScale(xml);
				else if(!strcmp("units", xml->getNodeName()))
					ProfileXMLUnits(xml);
				else if(!strcmp("dimension", xml->getNodeName()))
					ProfileXMLDimension(xml);
				else if(!strcmp("fixup", xml->getNodeName()))
					ProfileXMLFixup(xml);
				else if(!strcmp("useraxis", xml->getNodeName()))
					ProfileXMLUserAxis(xml);
				else if(!strcmp("grid", xml->getNodeName()))
					ProfileXMLGrid(xml);
				else if(!strcmp("hatch", xml->getNodeName()))
					ProfileXMLHatch(xml);
				else if(!strcmp("pipe", xml->getNodeName()))
					ProfileXMLPipe(xml);
				else if(!strcmp("scroll", xml->getNodeName()))
					ProfileXMLScroll(xml);
				else if(!strcmp("mouse", xml->getNodeName()))
					ProfileXMLMouse(xml);
				else if(!strcmp("note", xml->getNodeName()))
					ProfileXMLNote(xml);
				else if(!strcmp("appearance", xml->getNodeName()))
					ProfileXMLAppearance(xml);
				else if(!strcmp("isometric", xml->getNodeName()))
					ProfileXMLIsometric(xml);
				else if(!strcmp("view", xml->getNodeName()))
					ProfileXMLView(xml);
				else if(!strcmp("print1", xml->getNodeName()))
					ProfileXMLPrint(1, xml);
				else if(!strcmp("print2", xml->getNodeName()))
					ProfileXMLPrint(2, xml);
				else if(!strcmp("plot3", xml->getNodeName()))
					ProfileXMLPrint(3, xml);
				else if(!strcmp("plot4", xml->getNodeName()))
					ProfileXMLPrint(4, xml);
				break;
			}
		}
	}

//	ShowWindow(stateframe.GetHWND(), SW_HIDE);

	delete xml;
}

// Disables rubberbanding.
void CPegApp::RubberBandingDisable()
{	
	CPegView* pView = CPegView::GetActiveView();
	CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();
	
	if (m_iGinRubTyp == 0) 
		return;

	int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
	
	if (m_iGinRubTyp == Lines) 
	{
		CPoint pnt[2];
		
		pnt[0] = m_pntGinStart;
		pnt[1] = m_pntGinEnd;
		pDC->Polyline(pnt, 2);
	}
	else if (m_iGinRubTyp == Quads)
	{
		CPoint pnt[5];
		int iPts = 1;
		
		pnt[0] = m_pntGinStart;
		pnt[1].x = m_pntGinStart.x + (m_pntGinCur.x - m_pntGinEnd.x);
		pnt[1].y = m_pntGinStart.y + (m_pntGinCur.y - m_pntGinEnd.y);
		
		if (pnt[1].x != m_pntGinStart.x || pnt[1].y != m_pntGinStart.y)
		{
			pnt[2] = m_pntGinCur;
			pnt[3] = m_pntGinEnd;
			iPts = 4;
		}				 
		pnt[iPts++] = m_pntGinStart;
		pDC->Polyline(pnt, iPts);
	}
	else if (m_iGinRubTyp == Circles) 
	{
		if(draw::eCircleType == draw::CIRCLE_CENTER_RADIUS)
		{
			CBrush* pBrushOld = (CBrush*) pDC->SelectStockObject(NULL_BRUSH);
			int iRadiusX = abs(m_pntGinEnd.x - m_pntGinStart.x);
			int iRadiusY = abs(m_pntGinEnd.y - m_pntGinStart.y);
			int iRadius = (int) sqrt((double) (iRadiusX * iRadiusX + iRadiusY * iRadiusY) + .5);
			pDC->Ellipse(m_pntGinStart.x - iRadius, m_pntGinStart.y + iRadius,
				m_pntGinStart.x + iRadius, m_pntGinStart.y - iRadius);
			pDC->SelectObject(pBrushOld);
		}
		else //if(draw::eCircleType == draw::CIRCLE_DIAMETER)
		{
			CBrush* pBrushOld = (CBrush*) pDC->SelectStockObject(NULL_BRUSH);

			CPnt pntStart(app.m_pntGinStart.x, app.m_pntGinStart.y, 0);
			CPnt pntEnd(app.m_pntGinEnd.x, app.m_pntGinEnd.y, 0);
			CPrimLine tempLine(pntStart, pntEnd);
			CPnt pntCenter = tempLine.GetCtrlPt();

			int iRadiusX = (int)(abs(app.m_pntGinEnd.x - app.m_pntGinStart.x));
			int iRadiusY = (int)(abs(app.m_pntGinEnd.y - app.m_pntGinStart.y));
			int iRadius = (int) sqrt((double) (iRadiusX * iRadiusX + iRadiusY * iRadiusY));
			iRadius = iRadius / 2;

			RECT rect;
			rect.left = (LONG)(pntCenter[0] - iRadius);
			rect.right = (LONG)(pntCenter[0] + iRadius);
			rect.top = (LONG)(pntCenter[1] - iRadius);
			rect.bottom = (LONG)(pntCenter[1] + iRadius);
			pDC->Ellipse(&rect);
		
			pDC->SelectObject(pBrushOld);
		}
	}
	else if (m_iGinRubTyp == Rectangles) 
	{
		CBrush* pBrushOld = (CBrush*) pDC->SelectStockObject(NULL_BRUSH);
		pDC->Rectangle(m_pntGinStart.x, m_pntGinStart.y, m_pntGinEnd.x, m_pntGinEnd.y);
		pDC->SelectObject(pBrushOld);
	}

	pstate.SetROP2(pDC, iDrawMode);
	m_iGinRubTyp = 0;
}

void CPegApp::RubberBandingEnable(ERubs aiRubOp)
{	
	// Enables rubberbanding.
	m_iGinRubTyp = aiRubOp;
	
	if (aiRubOp == Quads)
		m_pntGinCur = m_pntGinEnd;
}

void CPegApp::RubberBandingStartAtEnable(const CPnt& pt, ERubs eRub)
{
	CPnt4 ptView(pt, 1);
	
	ModelView_DoTransform(ptView);
	
	// mak - i took this out... i don't know why it is there
//	if (Pnt4_IsInView(ptView))
	{
		m_ptRubStart = pt;
		m_ptRubEnd = pt;

		m_pntGinStart = viewport::DoProjection(ptView);
		m_pntGinEnd = m_pntGinStart;
	}

	RubberBandingEnable(eRub);
}

void CPegApp::SetBackGround(COLORREF cr)
{
	CPegView* pView = CPegView::GetActiveView();
	if (pView != NULL)
	{
		HBRUSH hbr = ::CreateSolidBrush(cr);
		::SetClassLong(pView->GetSafeHwnd(), GCL_HBRBACKGROUND, (LONG) hbr);

		CDC* pDC = pView->GetDC();
		pDC->SetBkColor(cr);

		// mak - adding code to update status and mode screens when bkgd changes
		statusframe.InvalidateRect();
		modeframe.InvalidateRect();
		odomframe.InvalidateRect();
	}
}

void CPegApp::SetModeCursor(int iModeId)
{
	WORD wResourceId;

	switch (iModeId)
	{
		case ID_MODE_ANNOTATE:
			wResourceId = ID_MODE_ANNOTATE; 
			break;				
		case ID_MODE_CUT:
			wResourceId = ID_MODE_CUT; 
			break;			
		case ID_MODE_DIMENSION:
			wResourceId = ID_MODE_DIMENSION; 
			break;
		case ID_MODE_DRAW:
			wResourceId = ID_MODE_DRAW; 
			break;
		case IDC_BLANK:
			wResourceId = IDC_BLANK;
			break;
		case ID_MODE_LPD:
		case ID_MODE_PIPE:
		case ID_MODE_POWER:
		case ID_MODE_RLPD:
			wResourceId = IDC_APPS; 
			break;
		case ID_MODE_DRAW2:
			wResourceId = ID_MODE_DRAW2; 
			break;			
		case ID_MODE_EDIT:
		case ID_MODE_BACKGROUNDEDIT:
			wResourceId = ID_MODE_EDIT; 
			break;
		case ID_MODE_FIXUP:
			wResourceId = ID_MODE_FIXUP; 
			break;			
		case ID_MODE_NODAL:
			wResourceId = ID_MODE_NODAL; 
			break;
		case ID_MODE_NODALR:
			wResourceId = ID_MODE_NODALR; 
			break;			
		case ID_MODE_TRAP:
			wResourceId = ID_MODE_TRAP; 
			break;
		case ID_MODE_TRAPR:
			wResourceId = ID_MODE_TRAPR; 
			break;
		default:
			SetCursor(LoadCursor(0, IDC_CROSS));
			return;
	}
	HCURSOR hCursor = LoadCursor(m_hInstance, MAKEINTRESOURCE(wResourceId));

	if(hCursor != 0)
		SetCursor(hCursor);
}

CString CPegApp::GetAppPath()
{
	TCHAR szPath[MAX_PATH];
	char *pStr;
	GetModuleFileName(app.GetInstance(), szPath, MAX_PATH);
	pStr = strrchr(szPath, '\\');
	if (pStr != NULL)
    *(++pStr)='\0';

	return CString(szPath);
}

void CPegApp::SetShadowDir(const CString& strDir) 
{
	// mak - pegaesys shadow directory is in application root
	m_strShadowDir = GetAppPath();
	m_strShadowDir += (/*"\\" + */strDir + "\\");
	mkdir(m_strShadowDir);
}

void CPegApp::SetWindowMode(int aiModeId)
{
	static int nModeSave = ID_MODE_DRAW;

	// previous mode
	switch(m_iModeId)
	{
		case ID_MODE_EDIT:
		case ID_MODE_NODAL:
			CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);
			break;
		case ID_MODE_SEG_EDIT:
		case ID_MODE_PRIM_EDIT:
			//CSeg *pSeg;
			//CPrim *pPrim;
			//if((AppGetGinCur(pSeg) == 0) || (AppGetGinCur(pPrim) == 0))
			//	aiModeId = nModeSave;

		//	CPegDoc::GetDoc()->DoEditPrimitiveEscape(false);				
			break;
	}

	HWND hwnd = m_hWndApp;

	// new mode
	switch(aiModeId)
	{
		case ID_MODE_DRAW2:
			::SetWindowLong(hwnd, GWL_WNDPROC, (LPARAM) SubProcDraw2);
			break;
		case ID_MODE_ANNOTATE:
			::SetWindowLong(hwnd, GWL_WNDPROC, (LPARAM) SubProcAnnotate);
			m_iPrimModeId = ID_MODE_ANNOTATE;
			break;
		case ID_MODE_CUT:
			::SetWindowLong(hwnd, GWL_WNDPROC, (LPARAM) SubProcCut);
			break;
		case ID_MODE_DIMENSION:
			::SetWindowLong(hwnd, GWL_WNDPROC, (LPARAM) SubProcDimension);
			break;
		case ID_MODE_DRAW:
			::SetWindowLong(hwnd, GWL_WNDPROC, (LPARAM) SubProcDraw);
			m_iPrimModeId = ID_MODE_DRAW;
			break;
		case ID_MODE_BACKGROUNDEDIT:
			::SetWindowLong(hwnd, GWL_WNDPROC, (LPARAM) SubProcBackgroundEdit);
			break;
		case ID_MODE_EDIT:
			::SetWindowLong(hwnd, GWL_WNDPROC, (LPARAM) SubProcEdit);
			break;
		case ID_MODE_FIXUP:
			::SetWindowLong(hwnd, GWL_WNDPROC, (LPARAM) SubProcFixup);
			break;
		case ID_MODE_LPD:
			::SetWindowLong(hwnd, GWL_WNDPROC, (LPARAM) SubProcLPD);
			break;
		case ID_MODE_RLPD:
			::SetWindowLong(hwnd, GWL_WNDPROC, (LPARAM) SubProcRLPD);
			break;
		case ID_MODE_NODAL:
			::SetWindowLong(hwnd, GWL_WNDPROC, (LPARAM) SubProcNodal);
			break;
		case ID_MODE_PIPE:
			::SetWindowLong(hwnd, GWL_WNDPROC, (LPARAM) SubProcPipe);
			break;
		case ID_MODE_POWER:
			::SetWindowLong(hwnd, GWL_WNDPROC, (LPARAM) SubProcPower);
			break;
		case ID_MODE_PRIM_EDIT:
			::SetWindowLong(hwnd, GWL_WNDPROC, (LPARAM) SubProcEditPrimitive);
			nModeSave = m_iModeId;
			break;
		case ID_MODE_SEG_EDIT:
			::SetWindowLong(hwnd, GWL_WNDPROC, (LPARAM) SubProcEditSegment);
			nModeSave = m_iModeId;
			break;
		case ID_MODE_PRIM_MEND:
			::SetWindowLong(hwnd, GWL_WNDPROC, (LPARAM) SubProcMendPrim);
			break;
		case ID_MODE_TRAP:
			::SetWindowLong(hwnd, GWL_WNDPROC, (LPARAM) SubProcTrapAdd);
			break;
		case ID_MODE_TRAPR:
			::SetWindowLong(hwnd, GWL_WNDPROC, (LPARAM) SubProcTrapRemove);
	}
	
	RubberBandingDisable();
	
	m_iModeId = aiModeId;
	msgInformation(0);
	SetModeCursor(aiModeId);
	StatusLineDisplay();
	ModeLineDisplay();
}

void CPegApp::StatusLineDisplay(EStatusLineItem sli)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	CPegView* pView = CPegView::GetActiveView();

	sli = All;

	if(statusframe.GetDC() == 0)
		return;

	CDC* pDC = statusframe.GetDC();

	if(m_bViewStatusInfo)	
	{	
		CRect rc;
		char szBuf[128];

		CFont* pFont = pDC->SelectObject(m_pFontApp);
		UINT nTextAlign = pDC->SetTextAlign(TA_LEFT | TA_TOP);
		COLORREF crText = pDC->SetTextColor(GetTextCol()); 
		
		// mak - 
		COLORREF crBk = pDC->SetBkColor(RGB(128, 128, 128));

		// mak - for white backgrounds, etc.
		pDC->SetBkMode(TRANSPARENT);

		TEXTMETRIC tm;		 
		pDC->GetTextMetrics(&tm);
	
		CRect rcClient;
		pView->GetClientRect(&rcClient);

		////////////////////////////////////////////////////////////////////////
		// bitmap
		////////////////////////////////////////////////////////////////////////
		HBITMAP hBitmap = ::LoadBitmap(app.GetInstance(), MAKEINTRESOURCE(IDB_BITMAP2));
		if(hBitmap != NULL) 
		{
			CDC dcMemory;
			dcMemory.CreateCompatibleDC(0);

			HBITMAP hOldBitmap = (HBITMAP)SelectObject(dcMemory, hBitmap);
			pDC->BitBlt(0, 0, rcClient.right, 16, &dcMemory, 0, 0, SRCCOPY);

			SelectObject(dcMemory, hOldBitmap);

			DeleteObject(hBitmap);
			DeleteObject(hOldBitmap);
		}

		if (sli == All || sli == WorkCnt)
		{	// print num in work 
			rc.SetRect(0, rcClient.top, 8 * tm.tmAveCharWidth, rcClient.top + tm.tmHeight);
			sprintf(szBuf, "%i  ", pDoc->GetHotCount() + pDoc->GetWarmCount());
			pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED  | ETO_OPAQUE, &rc, szBuf, (UINT) strlen(szBuf), 0);
		}
		if (sli == All || sli == TrapCnt)
		{	// print num in trap 
			rc.SetRect(10 * tm.tmAveCharWidth, rcClient.top, 19 * tm.tmAveCharWidth, rcClient.top + tm.tmHeight);
			sprintf(szBuf, " %i  ", trapsegs.GetCount());
			pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE,  &rc, szBuf, (UINT) strlen(szBuf), 0);
		}
		if (sli == All || sli == Pen)
		{	// print pen  info
			rc.SetRect(21 * tm.tmAveCharWidth, rcClient.top, 27 * tm.tmAveCharWidth, rcClient.top + tm.tmHeight);
			sprintf(szBuf, "P %i ", pstate.PenColor());
			pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, szBuf, (UINT) strlen(szBuf), 0);
		}
		if (sli == All || sli == Line)
		{	// print line info
			rc.SetRect(29 * tm.tmAveCharWidth, rcClient.top, 35 * tm.tmAveCharWidth, rcClient.top + tm.tmHeight);
			sprintf(szBuf, "L %i", pstate.PenStyle());
			pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, szBuf, (UINT) strlen(szBuf), 0);
		}
		if (sli == All || sli == TextHeight)
		{	// print text height
			if(rsNBuff == 0 && fdNBuff == 0)
			{
				CCharCellDef ccd;
				pstate.GetCharCellDef(ccd);
				rc.SetRect(37 * tm.tmAveCharWidth, rcClient.top, 47 * tm.tmAveCharWidth, rcClient.top + tm.tmHeight);
				sprintf(szBuf, "T %6.2f", ccd.ChrHgtGet());
				pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, szBuf, (UINT) strlen(szBuf), 0);
			}
			else
			{
				rc.SetRect(37 * tm.tmAveCharWidth, rcClient.top, 47 * tm.tmAveCharWidth, rcClient.top + tm.tmHeight);
				sprintf(szBuf, "T %3s", "BUF");
				pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, szBuf, (UINT) strlen(szBuf), 0);
			}
		}
		if (sli == All || sli == Scale)
		{	//print scale
			rc.SetRect(49 * tm.tmAveCharWidth, rcClient.top, 59 * tm.tmAveCharWidth, rcClient.top + tm.tmHeight);
			sprintf(szBuf, "1: %6.2f", GetScale());
			pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, szBuf, (UINT) strlen(szBuf), 0);
		}
		if (sli == All || sli == WndRatio)
		{	//print zoom
			rc.SetRect(61 * tm.tmAveCharWidth, rcClient.top, 71 * tm.tmAveCharWidth, rcClient.top + tm.tmHeight); 
			double dRatio = viewport::GetWidthInInches() / view::GetUExt();
			sprintf(szBuf, "@ %6.3f", dRatio);
			pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, szBuf, (UINT) strlen(szBuf), 0);
		}
		if (sli == All || sli == DimLen)
		{	// print DimLen
			rc.SetRect(73 * tm.tmAveCharWidth, rcClient.top, 90 * tm.tmAveCharWidth, rcClient.top + tm.tmHeight);			  
			UnitsString_FormatLength(szBuf, GetUnits(), GetDimLen(), 16, 4);
			pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, szBuf, (UINT) strlen(szBuf), 0);
		}
		if (sli == All || sli == DimAng)
		{	// print DimAngle
			rc.SetRect(92 * tm.tmAveCharWidth, rcClient.top, 107 * tm.tmAveCharWidth, rcClient.top + tm.tmHeight); 
			sprintf(szBuf, ">> %8.4f", GetDimAngZ());
			pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, szBuf, (UINT) strlen(szBuf), 0);
		}
		if (sli == All || sli == WorkColor)
		{	// print work swatch if in layers by color
			if(pView->IsColorByLayerOn())
			{
				rc.SetRect(109 * tm.tmAveCharWidth, rcClient.top, 112 * tm.tmAveCharWidth, rcClient.top + tm.tmHeight); 
				PENCOLOR penColor = pDoc->WorkLayerGet()->PenColor();
				CBrush brush(crHotCols[penColor]);
				pDC->FillRect(&rc, &brush);
			}
		}

		// restore Device Context to its original state
		pDC->SetBkColor(crBk);
		pDC->SetTextColor(crText);
		pDC->SetTextAlign(nTextAlign);
		pDC->SelectObject(pFont);
	}
}

void CPegApp::StrokeFontLoad(const CString& strPathName)
{	
	// size (16384) of object for 96 character
	// font set with a maximum of 4096 stokes
	if (strPathName.GetLength() < 1)
	{
        int    fontlen;
        HRSRC  hrsrc;
		LPVOID fontptr;

        // Open binary resources
        hrsrc   = FindResource(NULL, MAKEINTRESOURCE(IDR_PEGSTROKEFONT), "STROKEFONT");
        fontlen = SizeofResource(NULL, hrsrc);
        m_pStrokeFontDef = new char[fontlen];

        fontptr = LockResource(LoadResource(NULL, hrsrc));
		memcpy(m_pStrokeFontDef, fontptr, fontlen);
    } 
    else 
    {
		HANDLE hFile = CreateFile(
			strPathName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0); 
		if (hFile != ((HANDLE) - 1)) 
		{ 
			if (SetFilePointer (hFile, 0, 0, FILE_BEGIN) != (DWORD) - 1)
			{
				if (m_pStrokeFontDef == 0)
					m_pStrokeFontDef = new char[16384];
			
				DWORD nBytesRead;
				ReadFile(hFile, m_pStrokeFontDef, 16384U, &nBytesRead, 0); 
			} 
			CloseHandle(hFile);
		}
	}
}

void CPegApp::StrokeFontRelease()
{
	// Releases a large character array loaded for stroke fonts.
	if (m_pStrokeFontDef != 0)
		delete [] m_pStrokeFontDef;
}

void CPegApp::OnAppExit()
{
	::SendMessage(m_hWndApp, WM_CLOSE, 0L, 0L);
}

void CPegApp::OnEditCfImage()
{
	m_bEditCFImage = !m_bEditCFImage;
	app.CheckMenuItem(ID_EDIT_CF_IMAGE, MF_BYCOMMAND | (m_bEditCFImage ? MF_CHECKED : MF_UNCHECKED));
}

void CPegApp::OnEditCfSegments()
{
	m_bEditCFSegments = !m_bEditCFSegments;
	app.CheckMenuItem(ID_EDIT_CF_SEGMENTS, MF_BYCOMMAND | (m_bEditCFSegments ? MF_CHECKED : MF_UNCHECKED));
}

void CPegApp::OnEditCfText()
{
	m_bEditCFText = !m_bEditCFText;
	app.CheckMenuItem(ID_EDIT_CF_TEXT, MF_BYCOMMAND | (m_bEditCFText ? MF_CHECKED : MF_UNCHECKED));
}

void DlgProcViewZoomDoBest();
void DlgProcViewZoomDoOverview();

void CPegApp::OnFileNew(bool bConfirmation)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	CPegView *pView = CPegView::GetActiveView();

	int	iSaveOpenFile = IDYES;

	// confirmation dialog
	if(bConfirmation)
		iSaveOpenFile = MessageBox(app.GetSafeHwnd(), "Create New Drawing?", "Confirm", MB_YESNO);
	
	// if the user does not cancel the open command - whether saving or not
	if(iSaveOpenFile == IDYES)
	{
		pDoc->OpenFileClose(false);
		pDoc->InitAll();

		// initialize default view
		switch(pView->GetDefaultView())
		{
//			case CPegView::VIEW_BEST:
//				DlgProcViewZoomDoBest();
//				break;
			case CPegView::VIEW_OVERVIEW:
				DlgProcViewZoomDoOverview();
		//		view::Initialize();
				break;
		}

		// switch to top view
		pView->On3dViewsTop();

		// reset cursor.z to zero
		CPnt cursorPos = app.CursorPosGet();
		if(cursorPos[2] != 0)
		{
			cursorPos[2] = 0;
			app.CursorPosSet(cursorPos);
		}

		// turn odometer off
		pView->OdometerEnable(false);

		// mak - 
	//	pDoc->SetFileReadOnly(false);

		// kill timer
		KillAutoSave();

		pView->InvalidateRect(NULL, TRUE);
	}
}

void CPegApp::OnFileOpen()
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
	pDoc->OpenFile();
	app.StartAutoSave();
}

void CPegApp::OnFileRun()
{
//	CString strPathName = CPegDoc::GetDoc()->GetPathName();
//	CString strWDirectory = strPathName.Left(strPathName.ReverseFind('\\'));

	//char szFilter[32];
	//SecureZeroMemory(szFilter, 32);
	//::LoadString(app.GetInstance(), IDS_OPENFILE_FILTER_APPS, szFilter, sizeof(szFilter));

	OPENFILENAME	of;
	::ZeroMemory(&of, sizeof(OPENFILENAME));
//	of.lpstrInitialDir = strWDirectory;
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = m_hWndApp;
	of.hInstance = app.GetInstance();
	//of.lpstrFilter = szFilter;
	of.lpstrFilter = "Application Files (.exe)\0*.exe\0";
	of.lpstrFile = new char[MAX_PATH]; 
	of.lpstrFile[0] = 0;
	of.nMaxFile = MAX_PATH;
	of.lpstrTitle = "Run File";
	of.lpstrDefExt = "exe";

	if (GetOpenFileName(&of))
	{
		WinExec(of.lpstrFile, SW_SHOW);
	}

	delete [] of.lpstrFile;
}

void CPegApp::OnHelpContents()
{
	::WinHelp(app.GetSafeHwnd(), "peg.hlp", HELP_CONTENTS, 0L);
}

void CPegApp::OnModeAnnotate()
{
	app.SetWindowMode(ID_MODE_ANNOTATE);
}

void CPegApp::OnModeCut()
{
	app.SetWindowMode(ID_MODE_CUT);
}

void CPegApp::OnModeDimension()
{
	app.SetWindowMode(ID_MODE_DIMENSION);
}

void CPegApp::OnModeDraw()
{
	app.SetWindowMode(ID_MODE_DRAW);
}

void CPegApp::OnModeDraw2()
{
	app.SetWindowMode(ID_MODE_DRAW2);
}

void CPegApp::OnModeBackgroundEdit()
{
	CPegView *pView = CPegView::GetActiveView();

	if(!pView->IsBackgroundImageOn())
		pView->OnBackgroundImageLoad();

	app.SetWindowMode(ID_MODE_BACKGROUNDEDIT);
	//CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);
}

void CPegApp::OnModeEdit()
{
	app.SetWindowMode(ID_MODE_EDIT);
	CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);
}

void CPegApp::OnModeFixup()
{
	app.SetWindowMode(ID_MODE_FIXUP);
}

void CPegApp::OnModeLetter()
{
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_ADD_NOTE), app.GetSafeHwnd(), DlgProcModeLetter);
}

void CPegApp::OnModeLPD()
{
	app.SetWindowMode(ID_MODE_LPD);
}

//#pragma tasMSG(TODO: OnModeNodal - has dwg refs)
void CPegApp::OnModeNodal()
{
//	CPegDoc* pDoc = CPegDoc::GetDoc();

	nodal::SegList = new CSegs;
	nodal::UPL = new CObList;
	nodal::PrimLis = new CObList;
	nodal::bAdd = true;

	//CPnts pts;
	//short nMarkStyle = pstate.MarkStyle();
	//pstate.SetMarkStyle(11);
	//
	//POSITION pos = pDoc->WorkLayerGetHeadPosition();
	//while (pos != 0)
	//{
	//	CSeg* pSeg = pDoc->WorkLayerGetNext(pos);
	//	
	//	POSITION posPrim = pSeg->GetHeadPosition();
	//	while (posPrim != 0)
	//	{
	//		CPrim* pPrim = pSeg->GetNext(posPrim);

	//		pPrim->GetAllPts(pts);
	//		// TODO display pts
	//	}
	//}
	//pstate.SetMarkStyle(nMarkStyle);

	app.SetWindowMode(ID_MODE_NODAL);
	CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);
}

void CPegApp::OnModePipe()
{
	app.SetWindowMode(ID_MODE_PIPE);
}

void CPegApp::OnModePower()
{
	app.SetWindowMode(ID_MODE_POWER);
}

void CPegApp::OnModePrimEdit()
{
	app.SetEditSegBeg(app.CursorPosGet());

	CSeg* pSeg = detsegs.SelSegAndPrimUsingPoint(app.GetEditSegBeg());
	
	if (pSeg != 0)
	{
		app.SetWindowMode(ID_MODE_PRIM_EDIT);
		AppSetGinCur(pSeg, detsegs.DetPrim());
	}
}

void CPegApp::OnModePrimMend()
{
	CPnt pt = app.CursorPosGet();
	CPnt4 ptView(pt, 1.);
	ModelView_DoTransform(ptView);
	
	pPrimToMend = 0;

	if (detsegs.IsSegEngaged())
	{	// Segment is currently engaged, see if cursor is on a control point
		CPnt ptDet;
		
		CPrim* pPrim = detsegs.DetPrim();
		
		CPrimPolygon::EdgeToEvaluate() = CPrimPolygon::Edge();
		CPrimPolyline::EdgeToEvaluate() = CPrimPolyline::Edge();

		if (pPrim->SelUsingPoint(ptView, detsegs.PicApertSiz(), ptDet))
		{	// Cursor is close enough to engaged primitive to use it first
			pPrimToMend = pPrim;
		}
	}
	if (pPrimToMend == 0)
	{	// No engaged segment, or engaged primitive to far from cursor 
		if (detsegs.SelSegAndPrimUsingPoint(ptMendPrimBeg) != 0)
		{	// Segment successfully engaged
			pPrimToMend = detsegs.DetPrim();
		}
	}
	ptMendPrimBeg = pt;

	if (pPrimToMend != 0)
	{
		pPrimToMend->Copy(pPrimToMendCopy);
		ptMendPrimBeg = pPrimToMend->SelAtCtrlPt(ptView);
		dwMendPrimPtId = 1 << CPrim::CtrlPt();
		
		app.SetWindowMode(ID_MODE_PRIM_MEND);
	}
}

void CPegApp::OnModeRevise()
{
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_ADD_NOTE), app.GetSafeHwnd(), DlgProcModeRevise);
}

void CPegApp::OnModeRLPD()
{
	app.SetWindowMode(ID_MODE_RLPD);
}

void CPegApp::OnModeSegEdit()
{
	//CPegDoc *pDoc = CPegDoc::GetDoc();

	//CLayer *pLayer = pDoc->BitmapLayerGet();

	//if(pLayer->IsWarm() && pLayer->IsOn())
	//{
	//	app.SetEditSegBeg(app.CursorPosGet());
	//	
	//	CSeg* pSeg = detsegs.SelSegAndPrimUsingPoint(app.GetEditSegBeg());
	//	CSeg *pSeg = pLayer->SelSegUsingPoint(

	//	if (pSeg != 0)
	//	{		
	//		app.SetWindowMode(ID_MODE_SEG_EDIT);
	//		AppSetGinCur(pSeg);
	//	}
	//}
	//else
	{
		app.SetEditSegBeg(app.CursorPosGet());
		
		CSeg* pSeg = detsegs.SelSegAndPrimUsingPoint(app.GetEditSegBeg());

		if (pSeg != 0)
		{		
			app.SetWindowMode(ID_MODE_SEG_EDIT);
			AppSetGinCur(pSeg);
		}
	}
}

void CPegApp::OnModeTrap()
{
	app.SetWindowMode(m_bTrapModeAddSegments ? ID_MODE_TRAP : ID_MODE_TRAPR);
}

void CPegApp::OnTrapCommandsAddSegments()
{
	m_bTrapModeAddSegments = !m_bTrapModeAddSegments;
	app.CheckMenuItem(ID_TRAPCOMMANDS_ADDSEGMENTS, (m_bTrapModeAddSegments ? MF_CHECKED : MF_UNCHECKED));

	if (m_iModeId == ID_MODE_TRAP)
		app.SetWindowMode(ID_MODE_TRAPR);
	else if (m_iModeId == ID_MODE_TRAPR)
		app.SetWindowMode(ID_MODE_TRAP);
}

void CPegApp::OnViewInformationToggle()
{
	OnViewModeInformation();

	// 0 - both on
	// 1 - state on; mode off
	// 2 - state off; mode on
	// 3 - both off

	//static int nInfoDisplay = 0;

	//switch(nInfoDisplay)
	//{
	//	case 0:
	//		OnViewModeInformation();
	//		nInfoDisplay = 1;
	//		break;
	//	case 1:
	//		OnViewModeInformation();
	//		OnViewStatusInformation();
	//		nInfoDisplay = 2;
	//		break;
	//	case 2:
	//		OnViewModeInformation();
	//		nInfoDisplay = 3;
	//		break;
	//	case 3:
	//		OnViewModeInformation();
	//		OnViewStatusInformation();
	//		nInfoDisplay = 0;
	//		break;
	//}
}

void CPegApp::OnViewModeInformation()
{
	ModeInfoEnable(!m_bViewModeInfo);
}

void CPegApp::ModeInfoEnable(bool bValue)
{
	m_bViewModeInfo = bValue;
	app.CheckMenuItem(ID_VIEW_MODEINFORMATION, (m_bViewModeInfo ? MF_CHECKED : MF_UNCHECKED));
	::ShowWindow(modeframe.GetHWND(), (m_bViewModeInfo ? SW_SHOW : SW_HIDE));
	BringWindowToTop(m_hWndApp);
	CPegDoc::GetDoc()->UpdateAllViews(NULL, 0L, NULL);
}

int	CPegApp::GetTransFactor() 
{ 
	return m_nTransFactor; 
}

void CPegApp::SetTransFactor(int nValue)
{ 
	m_nTransFactor = nValue; 
	MakeWindowTransparent(statusframe.GetHWND(), (unsigned char)app.GetTransFactor());
	MakeWindowTransparent(modeframe.GetHWND(), (unsigned char)app.GetTransFactor());
	MakeWindowTransparent(odomframe.GetHWND(), (unsigned char)app.GetTransFactor());
}

void CPegApp::OnViewStatusInformation()
{
	StatusInfoEnable(!m_bViewStatusInfo);
}

void CPegApp::StatusInfoEnable(bool bValue)
{
	m_bViewStatusInfo = bValue;
	app.CheckMenuItem(ID_VIEW_STATEINFORMATION, (m_bViewStatusInfo ? MF_CHECKED : MF_UNCHECKED));
	::ShowWindow(statusframe.GetHWND(), (m_bViewStatusInfo ? SW_SHOW : SW_HIDE));
	BringWindowToTop(m_hWndApp);
	CPegDoc::GetDoc()->UpdateAllViews(NULL, 0L, NULL);
}

void CPegApp::OnUndo()
{
//#ifdef ENABLE_UNDO
	//undo.RestoreLastState();
//#endif
}

bool AppGetGinCur(CPrim*& pPrim)
{
	pPrim = pGinCurPrim;
	return (pPrim != 0);
}

bool AppGetGinCur(CSeg*& pSeg)
{
	pSeg = pGinCurSeg;
	return (pSeg != 0);
}

bool AppGetGinCur(CSeg*& pSeg, CPrim*& pPrim)
{
	pSeg = pGinCurSeg;
	pPrim = pGinCurPrim;
	return (pSeg != 0);
}

COLORREF CPegApp::GetTextCol()
{
	return (~(crHotCols[0] | 0xff000000));
}

void AppSetGinCur(CSeg* pSeg, CPrim* pPrim)
{
	tmEditSeg.Identity();
	
	pGinCurSeg = pSeg;
	pGinCurPrim = pPrim;
}