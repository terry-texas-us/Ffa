#pragma once

LRESULT CALLBACK SubProcNodal(HWND, UINT, WPARAM, LPARAM);

#define btest(m, p) 		((bool) (((((DWORD) m) >> ((int) p)) & 1UL) == 1 ? true : false))
#define ibset(m, p) 		((DWORD) (((DWORD) m) | (1UL << ((int) p))))
#define ibclr(m, p) 		((DWORD) (((DWORD) m) & ~(1UL << ((int) p))))

class CNodalPrim : public CObject
{
public:
	const CPrim*	pPrim;
	DWORD			dwMask;
};

class CUniqPt : public CObject
{
public:
	int 	iCnt;
	CPnt	pt;
};

namespace nodal
{
	extern bool		bAdd;
	extern CObList*	UPL;
	extern CSegs*	SegList;
	extern CObList*	PrimLis;

	void	AddByArea(CPnt, CPnt);
	void	AddByPt(const CPnt&);
	void	AddEngPrim();
	void	AddPrimBit(bool, const CPrim*, int);
	void	AddPrimByPt(const CPnt);
	void	CopyPrims(const CVec&);
	void	GenFilAreas(const CVec&);
	void	GenVecs(const CVec&);
	DWORD	GetPrimMask(CPrim*);
	void	Translate(const CVec&);
	void	UpdLis(bool, CSeg*, CPrim*, DWORD, int, CPnt);
}
