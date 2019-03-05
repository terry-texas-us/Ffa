#pragma once

namespace dlgproceditops
{
	extern double dXRotAng;
	extern double dYRotAng;
	extern double dZRotAng;

	extern double dXScale;
	extern double dYScale;
	extern double dZScale;

	extern double dXMirrorScale;
	extern double dYMirrorScale;
	extern double dZMirrorScale;

	extern double dVernierSize;
	extern double dVernierRot;

	extern int iRotOrd[3];

	// functions
	CVec	GetMirrorScale();
	CVec	GetRotAng();
	CTMat	GetInvertedRotTrnMat();
	CTMat	GetRotTrnMat();
	CVec	GetScale();
	CVec	GetInvertedScale();
	
	CVec	GetVernierInvertedScale();
	CVec	GetVernierScale();
	CTMat	GetVernierInvertedRotTrnMat();
	CTMat	GetVernierRotTrnMat();

	void	SetMirrorScale(double dX, double dY, double dZ);
	void	SetRotAng(double dX, double dY, double dZ);
	void	SetRotOrd(int i0, int i1, int i2);
	void	SetScale(double dX, double dY, double dZ);
}