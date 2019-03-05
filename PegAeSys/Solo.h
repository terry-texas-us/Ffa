#pragma once

class CSolo
{
	public:
		CSolo();

//		bool		SaveCurrentOpenFile();
		bool		SaveSoloFileBlock();
		bool		SaveSoloFileTracing();
		bool		SaveSoloFileLayer();
		void		SoloFileSetHwnd(HWND hwnd_) { m_hPrevHwnd = hwnd_; }
		void		SoloFileSetBlockName(CString strBlockName) {m_strBlockName = strBlockName;}
		CString		SoloFileGetBlockName() { return m_strBlockName; }
		void		SoloFileSetLayerName(CString strLayerName) {m_strLayerName = strLayerName;}
		CString		SoloFileGetLayerName() { return m_strLayerName; }
		HWND		SoloFileGetPrevHwnd() { return m_hPrevHwnd; }
		void		OnSoloTrapBlock();

	private:
		HWND			m_hPrevHwnd;
		CString			m_strBlockName;
		CString			m_strLayerName;
};