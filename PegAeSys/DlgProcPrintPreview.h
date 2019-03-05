#pragma once

#include "StateInfoFrm.h"

class CStateFrm : public CInfoFrm
{
	public:
		CStateFrm(int x, int y, int width, int height);
		void OnPaint(HWND hwnd);
};

class CSoloStateFrm : public CInfoFrm
{
	public:
		CSoloStateFrm(int x, int y, int width, int height);
		void OnPaint(HWND hwnd);
};