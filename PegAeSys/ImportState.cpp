#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysView.h"

CImportState::CImportState()
{
	m_bTranslatePens = true;
	m_bModifyFont = true;
	m_bUnblockInserts = true;
	m_bPurgeBlocks = true;
	m_bScale = true;
	m_iScaleValue = 96.00;
	m_bKillLayers = true;
	m_bOmitLayers = true;
	m_bResolveCommands = true;
	m_bHardPens = true;
	m_bPolylines = true;
	m_bExplode = true;
	m_bViewBoxes = true;
	m_bViewports = true;

	m_bIsImport = false;

	m_bHardStyle = true;
	m_strTranslateFile = "Default.txt";
}

const CImportState& CImportState::operator=(const CImportState& src)
{
	m_bTranslatePens = src.m_bTranslatePens;
	m_bModifyFont = src.m_bModifyFont;
	m_bUnblockInserts = src.m_bUnblockInserts;
	m_bPurgeBlocks = src.m_bPurgeBlocks;
	m_bScale = src.m_bScale;
	m_iScaleValue = src.m_iScaleValue;
	m_bKillLayers = src.m_bKillLayers;
	m_bOmitLayers = src.m_bOmitLayers;
	m_bResolveCommands = src.m_bResolveCommands;
	m_bHardPens = src.m_bHardPens;
	m_bPolylines = src.m_bPolylines;
	m_bExplode = src.m_bExplode;
	m_bViewBoxes = src.m_bViewBoxes;
	m_bViewports = src.m_bViewports;

	m_bIsImport = src.m_bIsImport;

	m_bHardStyle = src.m_bHardStyle;
	m_strTranslateFile = src.m_strTranslateFile;

	return (*this);
}