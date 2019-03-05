#pragma once

LRESULT CALLBACK SubProcRLPD(HWND, UINT, WPARAM, LPARAM);

namespace rlpd
{
	enum EJust0 {Left0 = - 1, Center0, Right0};
	enum ETransition {None, Begin, End, Both};

	extern EJust0	eJust0;
	extern bool		bGenCenterline;
	extern bool		bContSec;
	extern double	dSecDiam;
	extern double	dInsideRadiusFactor;

	extern 	double	dDiam[3];

	bool	ClnCornsOnParLns(CLine*, CLine*);
	bool	Fnd2LnsGivLn(CPrimLine*, double, CPrim*&, CLine*, CPrim*&, CLine*);
	int 	GenElbow(double, double, CLine*);
	void	GenEndCap(const CPnt&, const CPnt&, double, double, CSeg*);
	bool	GenSection(double, CLine*);
	void	GenSizeNote(CPnt, double, double);
	bool	GenTap(CPnt, CPnt, double, CLine*);
	int		GenTransition(ETransition, int*, double*, double*, double*, CLine*);	
	bool	SelEndCapUsingPoint(const CPnt&, CSeg*&, CPrimMark*&);
	void	SetOptions(double*);
	double	TransitionLen(int aiJus, double adSlo, double* adWid, double* adDep, double* adWidChg, double* adDepChg);
}