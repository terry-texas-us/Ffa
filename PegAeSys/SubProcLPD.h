#pragma once

LRESULT CALLBACK SubProcLPD(HWND, UINT, WPARAM, LPARAM);

enum	ETransition {None, Begin, End, Both};
//enum	EElbow {Mittered, Radial};

namespace lpd
{
	enum    EJust {Left = - 1, Center, Right};
	
	extern EJust	eJust;
	extern EElbow	eElbow;			// elbow type (0 mittered 1 radius)
	extern bool		bContSec;
	extern bool		bGenTurnVanes;	// turning vanes generation flag
	extern double	dSecWid;
	extern double	dSecDep;
	extern double	dInsRadElFac;	// inside radius elbow factor
	
	extern double dDep[3];
	extern double dWid[3];

	bool	Fnd2LnsGivLn(CPrimLine*, double, CPrim*&, CLine*, CPrim*&, CLine*);
	int 	GenElbow(double, double, CLine*);
	void	GenEndCap(const CPnt&, const CPnt&, double, double, CSeg*);
	int 	GenRiseDrop(char, double, double, CLine*);
	bool	GenSection(double, double, CLine*); 
	void	GenSizeNote(CPnt, double, double, double);
	bool	GenTap(CPnt, CPnt, double, double, CLine*);
	CPnt	GenTee(CPnt, CPnt, double*, double*, CLine*);
	CPnt	GenTakeoff(CPnt, CPnt, double*, double*, CLine*);
	int		GenTransition(ETransition, int*, double*, double*, double*, CLine*);	
	bool	SelEndCapUsingPoint(const CPnt&, CSeg*&, CPrimMark*&);
	void	SetOptions(double*, double*);
	double	TransitionLen(int aiJus, double adSlo, double* adWid, double* adDep, double* adWidChg, double* adDepChg);
}