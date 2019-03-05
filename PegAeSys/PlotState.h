#pragma once

//enum PlotSize { PLOT_FULL, PLOT_HALF, PLOT_QUARTER };
//
//class CPlotState : public CPrintState
//{
//	private:
//		CRect		m_rectClip;
//		CPoint		m_pntOrigin;
//		CLayers		m_layers;
//		PlotSize	m_plotSize;
//
//	public: 
//		CPlotState();
//
//		void	SetClipRect(int x1, int y1, int x2, int y2) { m_rectClip.SetRect(x1, y1, x2, y2); }
//		void	SetOrigin(int x, int y) { m_pntOrigin.SetPoint(x, y); }
//		void	SetPlotSize(PlotSize pSize) { m_plotSize = pSize; }
//
//		CRect		GetClipRect() { return m_rectClip; }
//		CPoint		GetOrigin() { return m_pntOrigin; }
//		PlotSize	GetPlotSize() { return m_plotSize; }
//};
//
//extern CPlotState plotState;