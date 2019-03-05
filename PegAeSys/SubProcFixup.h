#pragma once

LRESULT CALLBACK SubProcFixup(HWND, UINT, WPARAM, LPARAM);

namespace fixup
{
	extern double dInfluenceAngle;
	extern double dFilletChamfer;
}