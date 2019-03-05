#pragma once

class CImportState
{
private:
	bool		m_bTranslatePens;
	bool		m_bModifyFont;
	bool		m_bUnblockInserts;
	bool		m_bPurgeBlocks;
	bool		m_bScale;
	double		m_iScaleValue;
	bool		m_bKillLayers;
	bool		m_bOmitLayers;
	bool		m_bResolveCommands;
	bool		m_bHardPens;

	bool		m_bPolylines;
	bool		m_bExplode;
	bool		m_bViewBoxes;
	bool		m_bViewports;

	CString		m_strTranslateFile;
	bool		m_bHardStyle;

public:
	bool		m_bIsImport;

public: // Constructors and destructor
	CImportState();

public: // Operators
	const CImportState& operator=(const CImportState&);

public: // Methods
	void			SetTranslatePens(bool bValue) { m_bTranslatePens = bValue; }
	void			SetModifyFont(bool bValue) { m_bModifyFont = bValue; }
	void			SetUnblockInserts(bool bValue) { m_bUnblockInserts = bValue; }
	void			SetPurgeBlocks(bool bValue) { m_bPurgeBlocks = bValue; }
	void			SetScale(bool bValue) { m_bScale = bValue; }
	void			SetScaleValue(double iValue) { m_iScaleValue = iValue; }
	void			SetKillLayers(bool bValue) { m_bKillLayers = bValue; }
	void			SetOmitLayers(bool bValue) { m_bOmitLayers = bValue; }
	void			SetResolveCommands(bool bValue) { m_bResolveCommands = bValue; }
	void			SetHardPens(bool bValue) { m_bHardPens = bValue; }
	void			SetHardStyle(bool bValue) { m_bHardStyle = bValue; }
	void			SetTranslateFile(const CString &strName) { m_strTranslateFile = strName; }
	void			SetPolyLines(bool bValue) { m_bPolylines = bValue; }
	void			SetExplode(bool bValue) { m_bExplode = bValue; }
	void			SetViewBoxes(bool bValue) { m_bViewBoxes = bValue; }
	void			SetViewports(bool bValue) { m_bViewports = bValue; }

	bool			GetTranslatePens() { return m_bTranslatePens; }
	bool			GetModifyFont() { return m_bModifyFont; }
	bool			GetUnblockInserts() { return m_bUnblockInserts; }
	bool			GetPurgeBlocks() { return m_bPurgeBlocks; }
	bool			GetScale() { return m_bScale; }
	double 			GetScaleValue() { return m_iScaleValue; }
	bool			GetKillLayers() { return m_bKillLayers; }
	bool			GetOmitLayers() { return m_bOmitLayers; }
	bool			GetResolveCommands() { return m_bResolveCommands; }
	bool			GetHardPens() { return m_bHardPens; }
	bool			GetHardStyle() { return m_bHardStyle; }
	CString			GetTranslateFile() { return m_strTranslateFile; }
	bool			GetPolylines() { return m_bPolylines; }
	bool			GetExplode() { return m_bExplode; }
	bool			GetViewBoxes() { return m_bViewBoxes; }
	bool			GetViewports() { return m_bViewports; }
};

extern CImportState istate;