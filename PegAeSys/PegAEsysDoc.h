// PegAEsysDoc.h : interface of the CPegDoc class

#pragma once

//#include "Prefs.h"

const PENSTYLE PENSTYLE_LookupErr = SHRT_MAX;

class CFilePeg;
class CPegView;

class CPegDoc : public CObject
{
public:
	static const WORD FILE_TYPE_NONE		= 0;
	static const WORD FILE_TYPE_PEG			= 1;	// current peg format
	static const WORD FILE_TYPE_PEG_2000	= 9;	// peg format for old version
	static const WORD FILE_TYPE_JOB			= 2;
	static const WORD FILE_TYPE_JOB2		= 10;
	static const WORD FILE_TYPE_TRACING		= 3;
	static const WORD FILE_TYPE_DXF			= 6;
	static const WORD FILE_TYPE_DWG			= 5;
	static const WORD FILE_TYPE_BLOCK		= 7;
	static const WORD FILE_TYPE_FFA			= 4;
	static const WORD FILE_TYPE_BMP			= 8;

	// tabs
	static const WORD TAB_OPEN_APPEARANCE	= 8;
	static const WORD TAB_OPEN_PROFILES		= 9;
	static const WORD TAB_OPEN_DIMENSION	= 10;
	static const WORD TAB_OPEN_HATCH		= 11;
	static const WORD TAB_OPEN_CONSTRAINTS	= 12;
	static const WORD TAB_OPEN_USERAXIS		= 13;
	static const WORD TAB_OPEN_SCALE		= 14;
	static const WORD TAB_OPEN_UNITS		= 15;
	static const WORD TAB_OPEN_VIEW			= 16;
	static const WORD TAB_OPEN_MOUSE		= 17;
	static const WORD TAB_OPEN_SCROLL		= 18;
	static const WORD TAB_OPEN_REGISTRY		= 19;
	static const WORD TAB_OPEN_NOTE			= 20;
	static const WORD TAB_OPEN_ANNOTATE		= 21;
	static const WORD TAB_OPEN_DRAW			= 22;
	static const WORD TAB_OPEN_DRAW2		= 23;
	static const WORD TAB_OPEN_EDIT			= 24;
	static const WORD TAB_OPEN_LPD			= 25;
	static const WORD TAB_OPEN_PIPE			= 26;
	static const WORD TAB_OPEN_RLPD			= 27;
	static const WORD TAB_OPEN_FIXUP		= 28;
	static const WORD TAB_OPEN_PEN			= 29;
	static const WORD TAB_OPEN_ISOMETRIC	= 30;
	static const WORD TAB_OPEN_AXONOMETRIC	= 31;
	static const WORD TAB_OPEN_OBLIQUE		= 32;
	static const WORD TAB_OPEN_PERSPECTIVE	= 33;
	static const WORD TAB_OPEN_SETUPDIMENSION	= 34;

	static const LPARAM HINT_NONE	= 0;

	static const LPARAM HINT_PRIM	= 0x0001;
	static const LPARAM HINT_SEG	= 0x0002;
	static const LPARAM HINT_SEGS	= 0x0004;
	static const LPARAM HINT_LAYER	= 0x0008;

	static const LPARAM HINT_SAFE	= 0x0100;
	static const LPARAM HINT_ERASE	= 0x0200;	
	static const LPARAM HINT_TRAP	= 0x0400;

	static const LPARAM HINT_PRIM_SAFE				= HINT_PRIM | HINT_SAFE;
	static const LPARAM HINT_PRIM_ERASE_SAFE		= HINT_PRIM | HINT_ERASE | HINT_SAFE;
	static const LPARAM HINT_SEG_SAFE				= HINT_SEG | HINT_SAFE;
	static const LPARAM HINT_SEG_ERASE_SAFE			= HINT_SEG | HINT_ERASE | HINT_SAFE;
	static const LPARAM HINT_SEG_SAFE_TRAP			= HINT_SEG | HINT_SAFE | HINT_TRAP;
	static const LPARAM HINT_SEG_ERASE_SAFE_TRAP	= HINT_SEG | HINT_ERASE | HINT_SAFE | HINT_TRAP;

	static const LPARAM HINT_SEGS_SAFE				= HINT_SEGS | HINT_SAFE;
	static const LPARAM HINT_SEGS_TRAP				= HINT_SEGS | HINT_TRAP;
	static const LPARAM HINT_SEGS_SAFE_TRAP			= HINT_SEGS | HINT_SAFE | HINT_TRAP;
	static const LPARAM HINT_SEGS_ERASE_SAFE_TRAP	= HINT_SEGS | HINT_ERASE | HINT_SAFE | HINT_TRAP;

	static const LPARAM HINT_LAYER_SAFE				= HINT_LAYER | HINT_SAFE;
	static const LPARAM HINT_LAYER_ERASE			= HINT_LAYER | HINT_ERASE;

// Attributes
public:
	CString			m_strIdentifiedLayerName;
	WORD			m_wOpenFileType;

private:
	CPenStyles		m_PenStyles;
	CBlocks			m_blks;
	CLayers			m_layers;
	CLayer* 		m_pLayerWork;
	CString			m_strPathName;
	CString			m_strTitle;
	
	CString			m_strLastDirectory;

	CLayers			m_BorderLayers;
	CString			m_strLayerState;

	WORD			m_wOpenTab;
	DWORD			m_wOpenFocus;

	// registry object
	CPegReg			registry;

	bool			m_bModified;
	CFilePeg*		m_pFilePeg;
	CSegs			m_segsDeleted;
	bool			m_bTrapHighlight;

	// mak - shadow ish
	bool			m_bSaveShadow;
	bool			m_bIsFileReadOnly;

//	CLayer			*pBitmapLayer;

public: // Constructors and destructor
	CPegDoc();
	CPegDoc(const CPegDoc &b);
	const CPegDoc& operator= (const CPegDoc& param);
	
// Implementation
public:
	virtual ~CPegDoc();
	
// Operations
public:
	void		TextBlockWordWrap(CFontDef &fd, CRefSys &rs, CString strInput, int nLength);
	void		AddTextBlock(char* pszText);
	CLayer* 	AnyLayerRemove(CSeg* pSeg);

	int			BlockGetRefCount(const CString& strName) const;
	void		BlksGetNextAssoc(POSITION& pos, CString& strKey, CBlock*& pBlock) {m_blks.GetNextAssoc(pos, strKey, pBlock);}
	POSITION	BlksGetStartPosition() {return m_blks.GetStartPosition();}
	bool		BlksIsEmpty() const {return m_blks.IsEmpty() == TRUE;}
	bool		BlksLookup(CString strKey, CBlock*& pBlock) const;
	void		BlksRemoveAll();
	int			BlksRemoveUnused();
	void		BlksSetAt(CString strKey, CBlock* pBlock) {m_blks.SetAt(strKey, pBlock);}
	WORD		BlksSize() const {return (WORD(m_blks.GetSize()));}
	
	POSITION	DeletedSegsAddHead(CSeg* pSeg) {return (m_segsDeleted.AddHead(pSeg));}
	POSITION	DeletedSegsAddTail(CSeg* pSeg) {return (m_segsDeleted.AddTail(pSeg));}
	bool		DeletedSegsIsEmpty() {return (m_segsDeleted.IsEmpty() == TRUE);}
// CrashSite: CobList asserts on calls to RemoveHead & RemoveTail if list is empty!)
	CSeg*		DeletedSegsRemoveHead() {return (m_segsDeleted.RemoveHead());}
	void		DeletedSegsRemoveSegs() {m_segsDeleted.RemoveSegs();}
	CSeg*		DeletedSegsRemoveTail() {return (m_segsDeleted.RemoveTail());}
	void		DeletedSegsRestore();

	// mak - 
	void		DisplayTracingLayerText(CDC *pDC);
	void		PrintBox(CDC *pDC);
	
	void		DoPrimDelete(const CPnt& pt);

	bool		FileAutoCADOpenExclusive(CString strFilePath);
	bool		FileFFAOpenExclusive(CString strFilePath);
	bool		FilePegOpenExclusive(const CString& strFilePath);
	bool		FilePegRecover(const CString& strFilePath);
	bool		FileTypeIsTracing(WORD wType) const;
	static		CPegDoc* GetDoc();

//	void		StopProgress();

	// mak
	CString		FileTracingInclude();

//	void		VerifyPens();

	// mak
	void		DoEditPrimitiveEscape(bool bSetMode = true);

	void		GetExtents(CPnt&, CPnt&, const CTMat&) const;
	CFilePeg*	GetFilePeg() const {return m_pFilePeg;}
	int			GetHotCount() const;
	CString		GetPathName() const {return m_strPathName;}

	void		SetOptionsPage(WORD type) { m_wOpenTab = type; }
	WORD		GetOptionsPage() { return m_wOpenTab; }

	void		SetOptionsFocus(DWORD type) { m_wOpenFocus = type; }
	DWORD		GetOptionsFocus() { return m_wOpenFocus; }

	const CString& GetTitle() const {return m_strTitle;}
	int 		GetWarmCount() const;
	void		InitAll();

	CString		LayerFind(CSeg *pSeg);
	CString		LayersGetState();
	void		LayersSetState(CString strLayers);
	CLayers*	LayersGetAll();
	void		LayersSetAll(CLayers cLayers);
	void		LayersSoloHideAll(CString& strLayers);
	void		LayersSoloUnHideAll(CString strName);
	void		LayerBlank(const CString& strName);
	bool		LayerMelt(CString& strName);
	void		LayerMeltAll(CString strName);
	void		LayerMeltAllWithoutSaving(CString strName);
	void		LayersAdd(CLayer* pLayer) {m_layers.Add(pLayer);}
	void		LayersDisplayAll(CDC* pDC);
	CLayer*		LayersGet(const CString& strName) const;
	CLayer* 	LayersGetAt(int) const;
	int 		LayersGetSize() const {return (int) m_layers.GetSize();}
	int			LayersLookup(const CString& strName) const;
	void		LayersRemove(const CString& strName);
	void		LayersRemoveAll();
	void		LayersRemoveAt(int);
	int			LayersRemoveEmpty();
	CLayer* 	LayersSelUsingPoint(const CPnt&) const;
	void		LayersPrintAll(CDC *pDC);
	void		LayersOmitHidden();
	void		ConvertFontToStroke();
	void		ConvertByLayerPens();
	void		ConvertByLayerStyle();
	void		ResolveCommands();
	void		UnblockToCompressed();
	void		ConvertPolylines();

	void		RecentDirectoriesLoad(HWND hDlg);
//	
	void		OpenFileSaveCleanWarning();

	bool		OpenFile();
	bool		OpenFile(const CString& strFilePathName);
	bool		OpenFileClose(bool bSaveFirst);
	bool		OpenFileSave();
	bool		OpenFileSaveAs();
	bool		OpenFileRecover();
	int			DisplaySaveMessageConfirmation(UINT uiMsg);

	bool		OpenAutoCADSave(CString strPathName, bool bDispMsg);
	bool		OpenPegSave(CString strPathName, bool bDispMsg);
	bool		OpenTracingSave(CString strPathName, bool bDispMsg);
	bool		OpenAutoCADSaveAs(WORD wFileTypeTo, CString strPathNameTo, bool bDispMsg);
	bool		OpenPegSaveAs(CString strPathNameTo, bool bDispMsg);
//	bool		OpenPeg2000SaveAs(CString strPathNameTo);
	bool		OpenTracingSaveAs(WORD wFileTypeTo, CString strPathNameTo, bool bDispMsg);

//	bool		OpenPegAutoSave(CString strPathName);

	void		SetSaveShadow(bool bSaveShadow) { m_bSaveShadow = bSaveShadow; }
	bool		SaveShadow() { return m_bSaveShadow; }

	void		SetFileReadOnly(bool bValue) { m_bIsFileReadOnly = bValue; }
	bool		IsFileReadOnly() { return m_bIsFileReadOnly; }

	int			PenStyleGetRefCount(PENSTYLE nPenStyle) const;
	void		PenStylesAdd(CPenStyle* pPenStyle) {m_PenStyles.Add(pPenStyle);}
	int			PenStylesFillCB(HWND hwnd) const;
	CPenStyle*	PenStylesGetAt(int i) const {return m_PenStyles.GetAt(i);}
	CPenStyle*	PenStylesGetAtSafe(int);
	int			PenStylesGetSize() const {return (int) m_PenStyles.GetSize();}
	PENSTYLE	PenStylesLookup(const CString& strName) const;
	void		PenStylesInsert(int i, CPenStyle* pPenStyle) {m_PenStyles.InsertAt(i, pPenStyle);}
	void		PenColorsLoadDefault();
	int			PenStylesRemoveUnused();
	void		PenStylesClear();
	void		PenTranslation(WORD, PENCOLOR*, PENCOLOR*);
	void		PenStyleTranslation(const PENSTYLE&, const PENSTYLE&);

	void		SetRegistry(const CString filename) { registry.SetKey(filename); }
	void		DisplayMRU() { registry.DisplayMRU(); }
	void		SetMenu() { registry.SetMenu(); }
	void		SetPeg() { registry.SetPeg(); }
	void		SetJB1() { registry.SetJB1(); }
	void		SetTra() { registry.SetTra(); }
//	//void		SetDWG() { registry.SetDWG(); }
//	//void		SetDXF() { registry.SetDXF(); }

	int			RemoveDuplicateSegments();
	int			RemoveEmptyNotes();
	int			RemoveEmptySegs();
//	bool		SaveCurrentOpenFile();

//	// solo ish moved to solo file
//	//bool		SaveSoloFileBlock();
//	//bool		SaveSoloFileTracing();
//	//bool		SaveSoloFileLayer();
//	//void		SoloFileSetHwnd(HWND hwnd_) { m_hPrevHwnd = hwnd_; }
//	//void		SoloFileSetBlockName(CString strBlockName) {m_strBlockName = strBlockName;}
//	//CString		SoloFileGetBlockName() { return m_strBlockName; }
//	//void		SoloFileSetLayerName(CString strLayerName) {m_strLayerName = strLayerName;}
//	//CString		SoloFileGetLayerName() { return m_strLayerName; }
//	//HWND		SoloFileGetPrevHwnd() { return m_hPrevHwnd; }
////	void		OnSoloTrapBlock();

	void		OnImportAutoCAD();
	void		OnImportPeg();
	bool		OnImportBmp();
	bool		OnImportPdf();
	void		OnImportSpreadsheet();
	void		OnSetupImport();
	bool		OpenAutoCADImport();
	bool		OpenPegImport();
	bool		ImportAutoCADSettingsConfirm();
	bool		ImportPegSettingConfirm();
	void		ImportScaleSet();

    void		OnMruOpen(int iFileNumber);

	void		FileManageUnblockReferences(CString strName);

	void		SetFilePeg(CFilePeg* pFile);
	void		SetModifiedFlag() {}
	void		SetOpenFile(WORD wFileType, const CString& strFileName);

	void		TracingCloak(const CString& strPathName);
	void		TracingFuse(CString& strPathName);
	void		TracingFuseAll(CString& strLayers);
	bool		TracingLoadLayer(const CString& strPathName, CLayer* pLayer);
	bool		TracingMap(const CString& strPathName);
	bool		TracingOpen(const CString& strPathName);
	bool		TracingView(const CString& strPathName);

	bool		BorderMap(const CString& strPathName);
	void		BorderLayersAdd(CLayer* pLayer) {m_BorderLayers.Add(pLayer);}
	void		BorderLayersDisplayAll(CDC* pDC);
	//CLayer*		LayersGet(const CString& strName) const;
	CLayer* 	BorderLayersGetAt(int) const;
	int 		BorderLayersGetSize() const {return (int) m_BorderLayers.GetSize();}
	void		BorderLayersRemove(const CString& strName);
	void		BorderLayersRemoveAt(int);
	int			BorderLayersLookup(const CString& strName) const;
	void		BorderLayersRemoveAll();

	void		BitmapEditEnable();
	void		BitmapEditDisable();
	void		OnBitmapEdit();
	void		BitmapLayerAddTail(CSeg* pSeg);
	void		BitmapLayerSave();
	void		BitmapLayerLoad();
	void		BitmapLayerLoadFile(CPnt pntCursor, CString strPathName);
	void		BitmapLayerDisplay(CDC* pDC);
	//CLayer*		BitmapLayerGet() { return pBitmapLayer; }

	void		UpdateAllViews(CPegView* pView, LPARAM lHint = 0L, CObject* pOb = NULL);
	void		WorkLayerAddTail(CSeg* pSeg);
	void		WorkLayerAddTail(CSegs* pSegs);
	void		WorkLayerClear();
	POSITION	WorkLayerFind(CSeg* pSeg) const {return (m_pLayerWork->Find(pSeg));}
	CLayer*		WorkLayerGet() const {return m_pLayerWork;}
	POSITION	WorkLayerGetHeadPosition() const {return m_pLayerWork->GetHeadPosition();}
	CSeg*		WorkLayerGetNext(POSITION& pos) const {return m_pLayerWork->GetNext(pos);}
	CSeg*		WorkLayerGetPrev(POSITION& pos) const {return m_pLayerWork->GetPrev(pos);}
	CSeg*		WorkLayerGetTail() const;
	POSITION	WorkLayerGetTailPosition() const {return m_pLayerWork->GetTailPosition();}
	void		WorkLayerInit();
	void		WorkLayerSet(CLayer* pLayer);
	void		WriteShadowFile(bool bDispMsg);
	void		WriteAutosaveFile();

// Command message functions
public:
	void		OnBlocksLoad();
	void		OnBlocksUnload();
	void		OnClearActiveLayers();
	void		OnClearAllLayers();
	void		OnClearAllTracings();
	void		OnClearMappedTracings();
	void		OnClearViewedTracings();
	void		OnClearWorkingLayer();
	void		OnEditImageToClipboard();
	void		OnEditSegToWork();
	void		OnEditTrace();
	void		OnEditTrapCopy();
	void		OnEditTrapCut();
	void		OnEditTrapDelete();
	void		OnEditTrapPaste();
	void		OnEditTrapQuit();
	void		OnEditTrapWork();
	void		OnEditTrapWorkAndActive();
	void		OnFile(HWND hwnd);
	int			OnFileClose();
	void		OnFileRecover();
	void		OnFileManage();
	void		OnFileClean();
	void		OnFileSaveAs();
	void		OnFilePlot();
//	void		OnFilePrintCurrentView();
	void		OnFilePrintPreview();
	void		OnFileQuery();
	void		OnFileSave();
	void		OnFileTracing();
	void		OnFileArchive();
	void		OnExportPeg2000();
	bool		ExportDWG();
	void		OnExportDWG();
	void		OnIronDrawing();

	void		OnActiveAll();
	void		OnStaticAll();

	bool		OnFileExportArchive();

	void		OnProfileSet(ProfileType eProfile);
	void		OnProfileManager();
	void		OnOptions();
	void		OnSetup();

	void		OnLayerActive();
	bool		OnLayerHidden(bool bUseInternal, CString strLayerName = "0");
	void		OnLayerMelt();
	void		OnLayerStatic();
	void		OnLayerWork();
	void		OnLayersActiveAll();
	void		OnLayersStaticAll();
	void		OnLayersHideAll();
	void		OnPensLoadColors();
	void		OnPensRemoveUnusedStyles();
	void		OnPensTranslate();
	void		OnPrimBreak();
	void		OnPrimDelete();
	void 		OnPrimExtractNum();
	void		OnPrimExtractStr();
	void		OnPrimGotoCenterPoint();
	void		OnPrimModifyAttributes();
	void		OnPrimGetAttributes();
	void		OnPrimSnapToEndPoint();
	void		OnSegBreak();
	void		OnSegDelete();
	void		OnSegDeleteLast();
	void		OnSegExchange();
	void		OnSegInsert();
	void		OnSetupFillHollow();
	void		OnSetupFillPattern();
	void		OnSetupFillSolid();
	void		OnSetupFillHatch();
	void		OnSetupGotoPoint();
	void		OnSetupMouseButtons();
	void		OnSetupNote();
	void		OnSetupPenColor();
	void		OnSetupPenStyle();
	void		OnSetupSavePoint();
	void		OnTracingCloak();
	void		OnTracingFuse();
	void		OnTracingMap();
//	void		OnTracingOpen();
	void		OnTracingView();

	void		OnTracingMapAll();
	void		OnTracingViewAll();
	void		OnTracingCloakAll();

	void		OnTrapCommandsBlock();
	void		OnTrapCommandsCompress();
	void		OnTrapCommandsExpand();
	void		OnTrapCommandsFilter();
//	void		OnTrapCommandsHighlight();
	void 		OnTrapCommandsInvert();
	void		OnTrapCommandsQuery();
	void		OnTrapCommandsSquare();
	void		OnTrapCommandsUnblock();
	void		OnTrapCommandsVectorize();
};

extern CPegDoc	doc;

void	DoEditTrapCopy();
void	DoEditTrapPopupCommands(HWND hwnd);

void	DoFileDrawingPensTranslate(HWND hwnd);