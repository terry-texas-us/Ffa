// PegAEsysView.h : interface of the CPegView class

#pragma once

class CPegView : public CObject
{
public:
	CPegView();

public:
	enum	EClipMode		{MODE_NONE, MODE_PLOT_CLIPSET, MODE_PLOT_ORIGINSET};
	enum	EDefaultView	{VIEW_UNCHANGED, VIEW_BEST, VIEW_OVERVIEW};

private:	// Attributes
	HWND		m_hWnd;
	HDC			m_hDC;	// private hDC to be used for all drawing to this window

	bool		m_bViewClipping;
	bool		m_bViewBackgroundImage;
	bool		m_bViewOdometer;
	bool		m_bViewPenWidths;
	bool		m_bViewPrintPreview;
	bool		m_bViewRendered;
	bool		m_bViewTrueTypeFonts;
	bool		m_bViewWireframe;
	bool		m_bViewScrollBars;
	bool		m_bViewOrigin;
	bool		m_bViewColorByLayer;

	int			m_nOriginColor1;
	int			m_nOriginColor2;

	int			m_nBackgroundTrans;
	double		m_dBackgroundScaleX;
	double		m_dBackgroundScaleY;
	double		m_dBackgroundRotation;
	CPrimBitmap *m_BackgroundBitmap;

	CBitmap 	m_bmBackgroundImage;
	CPalette	m_palBackgroundImage;
	CVec		m_vRelPos;

	EClipMode	m_eClipMode;
	EDefaultView m_eDefaultView;

	CString		m_strPenWidthsFile;

public:
	void	ClipDisplay(CDC* pDC);
	void	OriginDisplay(CDC *pDC);
	void	PivotDisplay(CDC *pDC);

	int		GetBackgroundTrans() { return m_nBackgroundTrans; }
	void	SetBackgroundTrans(int nValue) { m_nBackgroundTrans = nValue; }

	void	BackgroundImageDisplay(CDC* pDC);
	void	DisplayOdometer();
	void	DoCameraRotate(int iDir);
	void	DoCustomMouseClick(LPTSTR lpCmds);
	void	DoWindowPan0(int iDir);
	static	CPegView* GetActiveView();
	void	GetClientRect(LPRECT lpRect) const {::GetClientRect(m_hWnd, lpRect);}
	CDC*	GetDC() const {return CDC::FromHandle(m_hDC);}
	CVec	GetRelPos() const {return m_vRelPos;}
	HWND	GetSafeHwnd() const {return m_hWnd;}
	bool	GetViewPenWidths() const {return m_bViewPenWidths;}
	bool	GetViewRendered() const {return m_bViewRendered;}
	bool	GetViewTrueTypeFonts() const {return m_bViewTrueTypeFonts;}
	bool	GetViewWireframe() const {return m_bViewWireframe;}
	BOOL	InvalidateRect(CONST RECT* lpRect, BOOL bErase) {return ::InvalidateRect(m_hWnd, lpRect, bErase);}
	void	SetHandle(HWND hWnd) {m_hWnd = hWnd; m_hDC = ::GetDC(hWnd);}

	void	OnUpdate(CPegView* pSender, LPARAM lHint, CObject* pHint);

	void	LoadStateNormal();
//	void	LoadStatePrint();
	void	LoadStatePlot();
	void	UnloadStateNormal();
//	void	UnloadStatePrint();
	void	UnloadStatePlot();

	void	OdometerEnable(bool bValue);
	void	ScrollbarEnable(bool bValue);
	void	TTFEnable(bool bValue);
	void	OriginEnable(bool bValue);
	void	PenWidthsEnable(bool bValue);
	void	ColorByLayerEnable(bool bValue);

	void	PenWidthsSetFile(CString strFilename);
	CString	PenWdithsGetFile();

	void	OriginSetColor1(int nIndex);
	void	OriginSetColor2(int nIndex);
	int		OriginGetColor1();
	int		OriginGetColor2();

	void	SetDefaultView(EDefaultView eView) { m_eDefaultView = eView; }
	EDefaultView GetDefaultView() { return m_eDefaultView; }

	void	ClipInit();
	void	ClipStartAll();
	void	ClipEndAll();

	void	SetClipMode(EClipMode eMode) { m_eClipMode = eMode; }

public:
	bool IsOdometerOn() { return m_bViewOdometer; }
	bool IsClipped() { return m_bViewClipping; }
	bool IsBackgroundImageOn() { return m_bViewBackgroundImage; }
	bool IsPenWidthsOn() { return m_bViewPenWidths; }
	bool IsPrintPreviewOn() { return m_bViewPrintPreview; }
	bool IsTTFontsOn() {return m_bViewTrueTypeFonts; }
	bool IsScrollbarsOn() { return m_bViewScrollBars; }
	bool IsOriginMarkerOn() { return m_bViewOrigin; }
	bool IsColorByLayerOn() { return m_bViewColorByLayer; }

public:
	void OnBackgroundImageLoad();
	void OnBackgroundImageRemove();
	void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
	void OnLButtonDown(HWND, BOOL, int, int, UINT);
	void OnLButtonUp(HWND, int, int, UINT);
	void OnRButtonDown(HWND, BOOL, int, int, UINT);
	void OnRButtonUp(HWND, int, int, UINT);
	void OnSetFocus(HWND hwnd, HWND hWndOld);
	void OnSize(HWND hwnd, UINT state, int cx, int cy);
	void OnSetupScale(HWND hwnd);
	void On3dViewsAxonometric();
	void On3dViewsBack();
	void On3dViewsBottom();
	void On3dViewsFront();
	void On3dViewsIsometric();
	void On3dViewsLeft();
	void On3dViewsOblique();
	void On3dViewsPerspective();
	void On3dViewsRight();
	void On3dViewsTop();

	void SetProjectionMenuText(WORD wProjType);

	void OnViewBackgroundImage();
	void ShowBackgroundImage(bool bValue);

	void OnViewColorByLayer();
	void OnViewTrueTypeFonts();
	void OnViewPenWidths();
	void OnViewPrintPreview();
	void OnViewOdometer();
	void OnViewRefresh();
	void OnViewZoom();
	void OnViewWireframe();
	void OnViewRendered();
	void OnViewLighting();
	void OnViewWindow(HWND hwnd);
	void OnWindowBest();
	void OnWindowNormal(HWND hwnd);
	void OnWindowPan(HWND hwnd);
	void OnWindowZoomIn(HWND hwnd);
	void OnWindowZoomOut(HWND hwnd);
	void OnWindowPanDown();
	void OnWindowPanLeft();
	void OnWindowPanRight();
	void OnWindowPanUp();
	void OnCameraRotateDown();
	void OnCameraRotateLeft();
	void OnCameraRotateRight();
	void OnCameraRotateUp();

	void OnCameraZRotateLeft();
	void OnCameraZRotateRight();

	void OnPrimPerpJump();
	void OnSetupDimAngle();
	void OnSetupDimLength();
	void OnSetupUnits();
	void OnSetupConstraintsAxis();
	void OnSetupConstraintsGrid();
	void OnRelativeMovesDown();
	void OnRelativeMovesDownRotate();
	void OnRelativeMovesEngDown();
	void OnRelativeMovesEngDownRotate();
	void OnRelativeMovesEngIn();
	void OnRelativeMovesEngOut();
	void OnRelativeMovesEngLeft();
	void OnRelativeMovesEngLeftRotate();
	void OnRelativeMovesEngRight();
	void OnRelativeMovesEngRightRotate();
	void OnRelativeMovesEngUp();
	void OnRelativeMovesEngUpRotate();
	void OnRelativeMovesIn();
	void OnRelativeMovesLeft();
	void OnRelativeMovesLeftRotate();
	void OnRelativeMovesOut();
	void OnRelativeMovesRight();
	void OnRelativeMovesRightRotate();
	void OnRelativeMovesUp();
	void OnRelativeMovesUpRotate();
	void OnPrimSnapTo();

	// scrollbar message handlers
	void DoScroll(int dir);
	void OnVScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
	void OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
	void OnMouseWheel(HWND hwnd, int xPos, int yPos, int zDelta, UINT fFlags);
	void OnScrollBars();
	void UpdateScroll();

	void OnClippingRegion();

#ifdef SHOW_CLIPPING
	bool OnEraseBkgnd(HWND hwnd, HDC hdc);
#endif

//	bool Pnt4_IsInView(const CPnt4& pt);
//	bool Pnt4_ClipLine(CPnt4& ptA, CPnt4& ptB);

//	void OnClipView();
};

extern CPegView viewThe;

void DoWindowPan(HWND hwnd, double dRatio);