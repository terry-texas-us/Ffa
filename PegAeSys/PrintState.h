#pragma once

enum ESheetSize { FULL_SHEET, HALF_SHEET, QUARTER_SHEET };

struct SPrintState
{
	SPrintState();

	// translate file colors
	CString		strTranslateFile;
	CString		strOldTranslate;
	bool		bTranslatePens;

	// pen widths
	bool		bUsePenWidths;
	CString		strPenWidthsFile;
	bool		bOldPenWidths;

	double		dMarginX;
	double		dMarginY;

	bool		bDispLayer;
	bool		bDispClipRect;
	bool		bDispBkgImage;
	bool		bPrintMarkers;
	bool		bUseBorders;

	ESheetSize	eSheetSize;
};

class CPrintStates
{
	public:
		CPrintStates() : nCurState(0) {}

		void SetUsePenWidths(bool bValue) { sState[nCurState].bUsePenWidths = bValue; }
		void SetPenWidthsFile(CString strValue) { sState[nCurState].strPenWidthsFile = strValue; }
		void SetDispLayer(bool bValue) { sState[nCurState].bDispLayer = bValue; }
		void SetTranslatePens(bool bValue) { sState[nCurState].bTranslatePens = bValue; }
		void SetTranslateFile(CString strValue) { sState[nCurState].strTranslateFile = strValue; }
		void SetMarginX(double dValue) { sState[nCurState].dMarginX = dValue; }
		void SetMarginY(double dValue) { sState[nCurState].dMarginY = dValue; }
		void SetOldTranslate(CString strValue) { sState[nCurState].strOldTranslate = strValue; }
	//	void SetOldPenWidths(CString strValue) { sState[nCurState].strOldTranslate = strValue; }
		void SetOldPenWidths(bool bValue) { sState[nCurState].bOldPenWidths = bValue; }
		void SetDispClipRect(bool bValue) { sState[nCurState].bDispClipRect = bValue; }
		void SetDispBkgImage(bool bValue) { sState[nCurState].bDispBkgImage = bValue; }
		void SetPrintMarkers(bool bValue) { sState[nCurState].bPrintMarkers = bValue; }
		void SetUseBorders(bool bValue) { sState[nCurState].bUseBorders = bValue; }
		void SetSheetSize(ESheetSize eValue) { sState[nCurState].eSheetSize = eValue; }

		bool GetUsePenWidths() { return sState[nCurState].bUsePenWidths; }
		CString GetPenWidthsFile() { return sState[nCurState].strPenWidthsFile; }
		bool GetDispLayer() { return sState[nCurState].bDispLayer; }
		bool GetTranslatePens() { return sState[nCurState].bTranslatePens; }
		CString GetTranslateFile() { return sState[nCurState].strTranslateFile; }
		double GetMarginX() { return sState[nCurState].dMarginX; }
		double GetMarginY() { return sState[nCurState].dMarginY; }
		CString GetOldTranslate() { return sState[nCurState].strOldTranslate; }
	//	CString GetOldPenWidths() { return sState[nCurState].strOldTranslate; }
		bool GetOldPenWidths() { return sState[nCurState].bOldPenWidths; }
		bool GetDispClipRect() { return sState[nCurState].bDispClipRect; }
		bool GetDispBkgImage() { return sState[nCurState].bDispBkgImage; }
		bool GetPrintMarkers() { return sState[nCurState].bPrintMarkers; }
		bool GetUseBorders() { return sState[nCurState].bUseBorders; }
		ESheetSize GetSheetSize() { return sState[nCurState].eSheetSize; }

		void SetCurrentState(int nState) { nCurState = nState; }
		int GetCurrentState() { return nCurState; }

	private:
		SPrintState sState[4];
		int nCurState;
};

extern CPrintStates printStates;