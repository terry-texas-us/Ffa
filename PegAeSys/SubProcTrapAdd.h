#pragma once

LRESULT CALLBACK SubProcTrapAdd(HWND, UINT, WPARAM, LPARAM);

namespace trap
{
	void AddLast();
	void AddByArea(CSegs*, CPnt, CPnt);
	void AddByLn(CSegs*, CPnt, CPnt);
	void PopupCommands(HWND);
}