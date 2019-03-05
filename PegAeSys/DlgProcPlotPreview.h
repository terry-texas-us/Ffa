#pragma once

void DlgProcPlotPreviewEnable();
void DlgProcPlotPreviewDisable();
void DlgProcPlotPreviewUsePenWidths(CString strFile);
void DlgProcPlotPreviewTranslatePens(CString strFile);

class CPlotFrm : public CInfoFrm
{
	public:
		CPlotFrm(int x, int y, int width, int height);
		void OnPaint(HWND hwnd);
};