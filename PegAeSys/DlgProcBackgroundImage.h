#pragma once

#include "StateInfoFrm.h"

class CBackgroundImage : public CInfoFrm
{
	public:
		CBackgroundImage(int x, int y, int width, int height);
		void OnPaint(HWND hwnd);

		CPnt GetPoint() { return m_pntUL; }
		void SetPoint(CPnt pnt) { m_pntUL = pnt; }

	private:
		CPnt m_pntUL;
};