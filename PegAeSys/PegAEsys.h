#pragma once

// PegAEsys.h : main header file for the PegAEsys application

class CSeg;
class CPrim;

enum EPegState {STATE_NORMAL, STATE_PLOT, STATE_SOLO, STATE_BITMAP, STATE_BACKGROUND};
enum ESoloType {SOLO_LAYER, SOLO_TRACING, SOLO_BLOCK};
enum EUnits {Architectural, Engineering, Feet, Inches, Meters, Millimeters, Centimeters, Decimeters, Kilometers};
enum ERubs {Lines = 1, Rectangles, Circles, Quads};
enum EStatusLineItem {WorkCnt, TrapCnt, Pen, Line, TextHeight, Scale, WndRatio, DimLen, DimAng, WorkColor, All};

enum EElbow {Mittered, Radial};
enum EJust {Left = - 1, Center, Right};

enum EProfState {PROF_CLASSIC, PROF_NEW, PROF_CUSTOM1, PROF_CUSTOM2};

enum ProfileType {PROFILE_CLASSIC, PROFILE_NEW, PROFILE_CUSTOM1, PROFILE_CUSTOM2};

extern COLORREF crHotCols[256];
extern COLORREF crWarmCols[16];
extern COLORREF* pColTbl;

extern COLORREF crHotColsTemp[256];
extern COLORREF crWarmColsTemp[16];

extern double dPWids[];

class CPegApp : public CObject
{
public:
	WNDPROC		m_wpMainWnd;
	CString		m_strAppPath;
	CFont*		m_pFontApp;

private:
	HWND		m_hWndApp;
	HINSTANCE	m_hInstance;
	HACCEL		m_hAccTbl;
	HMENU		m_hMenu;

	ESoloType	m_eSoloType;
	EPegState	m_ePegState;
	EProfState	m_eProfState;

//	HWND		m_hWndStatButton;

	// mak - used for autosave
//	DWORD		m_dwProcessId;

//	HWND		m_hWndToolbar;
//	HWND		m_hWndToolbarWindow;

	UINT		m_nClipboardFormatPegSegs;	// 0
	int 		m_iGinRubTyp;
	int			m_iUnitsPrec;				// 8
	EUnits		m_eUnits;					// Inches
	bool		m_bEditCFImage;
	bool		m_bEditCFSegments;
	bool		m_bEditCFText;
	double		m_dEngAngZ;					// 0.
	double		m_dEngLen;					// 0.
	double		m_dDimAngZ;					// 0.
	double		m_dDimLen;					// 0.
	double		m_dScale;					// 1.
	CPnt		m_ptCursorPosDev;
	CPnt		m_ptRubStart;
	CPnt		m_ptRubEnd;
	CPnt		m_ptEditSegBeg;
	CPnt		m_ptEditSegEnd;
	CPnt		m_ptHomePoint[9];
	WORD		m_wOpHighlighted;
	char*		m_pStrokeFontDef;			// 0
	CString		m_strShadowDir;				// "PegAEsys Shadow Files"

	int			m_nTimerEvent;
	bool		m_TimerRunning;
	CString		m_strAutoSaveFile;

	bool		m_bIsSoloWindow;

	int			m_nTransFactor;
	
public:
//	bool		m_bViewToolbar;
//	bool		m_bToolbarDocked;

	bool		m_bViewStatusInfo;
	bool		m_bViewModeInfo;

	bool		m_bTrapModeAddSegments;

	int			m_iModeId;
	int			m_iPrimModeId;
	CPoint		m_pntGinStart;
	CPoint		m_pntGinEnd;
	CPoint		m_pntGinCur;
	CPnt		m_ptCursorPosWorld;

	CString		m_strColorFile;
	int			m_nBkgdColor;

public:
	bool		IsStateInfoOn()	{ return m_bViewStatusInfo; }
	bool		IsModeInfoOn()	{ return m_bViewModeInfo; }

public:
	CPegApp();

	void		DisplayPoint(CDC* pDC, CPnts *pts, DWORD dwMask);
	
	UINT		CheckMenuItem(UINT uId, UINT uCheck) {return (::CheckMenuItem(m_hMenu, uId, uCheck));}
	CPnt		CursorPosGet();
	void		CursorPosSet(const CPnt& pt);
	HACCEL		GetAccTbl() const {return (m_hAccTbl);}
	UINT		GetClipboardFormatPegSegs() const {return (m_nClipboardFormatPegSegs);}
	double		GetDimAngZ() const {return (m_dDimAngZ);}
	double		GetDimLen() const {return (m_dDimLen);}
	bool		GetEditCFImage() const {return m_bEditCFImage;}
	bool		GetEditCFSegments() const {return m_bEditCFSegments;}
	bool		GetEditCFText() const {return m_bEditCFText;}
	CPnt		GetEditSegBeg() const {return (m_ptEditSegBeg);}
	CPnt		GetEditSegEnd() const {return (m_ptEditSegEnd);}
	double		GetEngAngZ() const {return (m_dEngAngZ);}
	double		GetEngLen() const {return (m_dEngLen);}
	static WORD	GetFileTypeFromPath(const CString& strFilePath);
	HINSTANCE	GetInstance() const {return (m_hInstance);}
	WNDPROC		GetMainWndProc() const {return (m_wpMainWnd);}	
	HMENU		GetMenu() const {return (m_hMenu);}
	
	//	DWORD		GetPID() { return m_dwProcessId; }

	CString		GetAppPath();

	EPegState	GetPegState() { return m_ePegState; }
	void		SetPegState(EPegState state) { m_ePegState = state; }

	ESoloType	GetSoloType() { return m_eSoloType; }
	void		SetSoloType(ESoloType eType) { m_eSoloType = eType; }

	HWND		GetSafeHwnd() const {return (m_hWndApp);}
//	HWND		GetToolbarWindowHwnd() const {return (m_hWndToolbarWindow); }

	double		GetScale() const {return m_dScale;}
	CString		GetShadowDir() {return m_strShadowDir;}
	HMENU		GetSubMenu(int nPos) const {return (::GetSubMenu(m_hMenu, nPos));}
	EUnits		GetUnits() const {return (m_eUnits);}
	int			GetUnitsPrec() const {return (m_iUnitsPrec);}

	COLORREF	GetTextCol();

	void		StatusInfoEnable(bool bValue);
	void		ModeInfoEnable(bool bValue);

	int			GetTransFactor();
	void		SetTransFactor(int nValue);

//	HWND		GetStatButtonHwnd() { return m_hWndStatButton; }
	
	void		HatchesLoad(const CString& strFileName);
	void		HatchesLoadFile(const CString& strFileName);
	void		HatchesLoadInternal();

	CPnt		HomePointGet(int i) const;
	void		HomePointSave(int i, const CPnt& pt);
	void		InitGbls(CDC* pDC);
	void		LineFontCreate();
	void		LineFontRelease();
	void		ModeLineDisplay();
	WORD		ModeLineHighlightOp(WORD wOp);
	void		ModeLineUnhighlightOp(WORD& wOp);
	void		ModifyMenu(UINT, LPCTSTR);
	void		ModifyMenu(UINT uPos, UINT_PTR uIdNewItem, LPCTSTR lpNewItem)
				{::ModifyMenu(m_hMenu, uPos, MF_BYPOSITION | MF_STRING | MF_POPUP, uIdNewItem, lpNewItem);}
	void		PenColorsChoose();
	COLORREF	PenColorsGetHot(PENCOLOR nPenColor) const {return (crHotCols[nPenColor]);} 
	void		PenColorsLoad(const CString &strFileName);
	void		PenColorsLoadTemp(const CString &strFileName);

	void		PenColorsSetFile(CString strFilename);
	CString		PenColorsGetFile();

	void		BackgroundColorSet(int nIndex);
	int			BackgroundColorGet();

	void		ProfileWrite(EProfState eState);
	void		ProfileLoad();
	void		ProfileLoadFromFile(CString strFilename);
	void		ProfileSetState(EProfState eProf) { m_eProfState = eProf; }
	EProfState	ProfileGetState() { return m_eProfState; }
	
    void		PenStylesLoad(const CString &strFileName);
	void		PenStylesLoadInternal();
	void		PenStylesLoadFile(const CString &strFileName);

	// mak - include default values
	void		PenUseDefaults();
	void		PenLoadDefaultWidths();

	// mak - more solo ish
	void		SetThisIsSoloWindow(bool bValue) { m_bIsSoloWindow = bValue; }
	bool		IsThisSoloWindow() { return m_bIsSoloWindow; }

	//MAK
	void		InitScrollbar();
	void		InitWorkspace();
	bool		InitKeyPlan();
	bool		InitPreview();

	HWND		MakeStatusWindow();
	HWND		MakeModeWindow();
	HWND		MakeOdometerWindow();
	HWND		MakeBackgroundLayer();

	double		PenWidthsGet(PENCOLOR nPenColor) const {return (dPWids[nPenColor]);}
	void		PenWidthsLoad(const CString& strFileName);
	void		RubberBandingDisable();
	void		RubberBandingEnable(ERubs eRub);
	CPnt		RubberBandingGetStart() const {return m_ptRubStart;}
	int			RubberBandingGetType() const {return (m_iGinRubTyp);}
	void		RubberBandingStartAtEnable(const CPnt& pt, ERubs eRub);
	void		SetBackGround(COLORREF cr);
	void		SetDimAngZ(double d) {m_dDimAngZ = d;}
	void		SetDimLen(double d) {m_dDimLen = d;}
	void		SetEditSegBeg(const CPnt& pt) {m_ptEditSegBeg = pt;}
	void		SetEditSegEnd(const CPnt& pt) {m_ptEditSegEnd = pt;}
	void		SetEngAngZ(double d) {m_dEngAngZ = d;}
	void		SetEngLen(double d) {m_dEngLen = d;}
	void		SetModeCursor(int iModeId);
	void		SetScale(double d) {if (d > DBL_EPSILON) m_dScale = d;}
	void		SetShadowDir(const CString& strDir);

	// mak
	void		SetPegMenu(HMENU menu) { m_hMenu = menu; }
	void		DestroyPegMenu() { ::DestroyMenu(m_hMenu); }

	void		SetUnits(EUnits eUnits) {m_eUnits = eUnits;}

	void		SetUnitsPrec(int iPrec) {if (iPrec > 0) m_iUnitsPrec = iPrec;}
	void		SetWindowMode(int iModeId);
	void		StatusLineDisplay(EStatusLineItem = All);
	char*		StrokeFontGet() const {return m_pStrokeFontDef;}
	void		StrokeFontLoad(const CString& strPathName);
	void		StrokeFontRelease();

	// MAK - AUTOSAVE -
	void		StartAutoSave();
	void		KillAutoSave();
	//void		RestoreAutoSave();
	//void		SetAutoSaveFileName(CString strFileName) {m_strAutoSaveFile = strFileName;}
	//void		CheckDirtyAutoSaveFile();

	// mak - dwg import
	void		ImportFilter();

public:
	BOOL		InitInstance(HINSTANCE, int);

public:
	void		OnAppAbout();
	void		OnAppExit();
	void		OnEditCfImage();
	void		OnEditCfSegments();
	void		OnEditCfText();
	void		OnFileNew(bool bConfirmation);
	void		OnFileOpen();
	void		OnFileRun();
	void		OnHelpContents();
	void		OnModeAnnotate();
	void		OnModeCut();
	void		OnModeDimension();
	void		OnModeDraw();
	void		OnModeDraw2();
	void		OnModeBackgroundEdit();
	void		OnModeEdit();
	void		OnModeFixup();
	void		OnModeLPD();
	void		OnModeLetter();
	void		OnModeNodal();
	void		OnModePipe();
	void		OnModePower();
	void		OnModePrimEdit();
	void		OnModePrimMend();
	void		OnModeRevise();
	void		OnModeRLPD();
	void		OnModeSegEdit();
	void		OnModeTrap();
	void		OnTrapCommandsAddSegments();
	void		OnViewModeInformation();
	void		OnViewStatusInformation();
	void		OnUndo();
	void		OnViewInformationToggle();
};

extern CPegApp	app;

bool		AppGetGinCur(CPrim*&);
bool		AppGetGinCur(CSeg*&);
bool		AppGetGinCur(CSeg*&, CPrim*&);
COLORREF	AppGetTextCol();
void		AppSetGinCur(CSeg* = 0, CPrim* = 0);