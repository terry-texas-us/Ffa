#pragma once

#include "StateInfoFrm.h"

class CBitmapEdit : public CInfoFrm
{
	public:
		CBitmapEdit(int x, int y, int width, int height);
		void OnPaint(HWND hwnd);
};