#pragma once

class CLayer : public CSegs
{
public:
	static const WORD STATE_FLG_RESIDENT	= 0x0001;	// entry in table list is saved
	static const WORD STATE_FLG_INTERNAL	= 0x0002;	// segment list saved within drawing
	static const WORD STATE_FLG_HOT			= 0x0004;	// may have segments added (0 or 1)
	static const WORD STATE_FLG_WARM		= 0x0008;	// may have segments modified (0 or more)
	static const WORD STATE_FLG_COLD		= 0x0010;	// tracing which is viewed or layer which is static (no additions or modifications)
	static const WORD STATE_FLG_OFF			= 0x0020;

//	static const WORD TRACING_FLG_OPENED	= 0x0004;
	static const WORD TRACING_FLG_MAPPED	= 0x0008;
	static const WORD TRACING_FLG_VIEWED	= 0x0010;
	static const WORD TRACING_FLG_CLOAKED	= 0x0020;

private:
	CString		m_strName;	 	// layer name.	If layer is externally referenced 
								// this is the full specification of the file name.
	WORD		m_wTracingFlgs; // Tracing flag values
								//		b2 set - tracing is mapped
	WORD		m_wStateFlgs;	// layer state flag values
								//		b0 set - layer is displayed using bitmap when possible
								//		b1 set - layer is externally referenced.  This means it is not necessary to write disk.
								//		b2 set - layer is off
	PENCOLOR	m_nPenColor;	// color number, negative if layer is off)
	CString		m_strPenStyleName;

public: // Constructors and destructor
	CLayer(const CString& = "", WORD = STATE_FLG_RESIDENT | STATE_FLG_INTERNAL | STATE_FLG_WARM);
	
	~CLayer() {}

	const CLayer& operator=(const CLayer&);

	void		ClrStateFlg(WORD w = 0xffff) {m_wStateFlgs &= ~w;}
	void		ClrTracingFlg(WORD w = 0xffff) {m_wTracingFlgs &= ~w;}
	void		Display(CDC* pDC);
	void		Display(CDC* pDC, bool bIdentifyTrap);
	WORD		GetStateFlgs() const {return m_wStateFlgs;}
	CString		GetPenStyleName() const {return m_strPenStyleName;}	
	CString		GetName() const {return m_strName;}
	WORD		GetTracingFlgs() const {return m_wTracingFlgs;}
	bool		IsCold() const {return ((m_wStateFlgs & STATE_FLG_COLD) == STATE_FLG_COLD);}
	bool		IsHot() const {return ((m_wStateFlgs & STATE_FLG_HOT) == STATE_FLG_HOT);}
	bool		IsInternal() const {return ((m_wStateFlgs & STATE_FLG_INTERNAL) == STATE_FLG_INTERNAL);}
	bool		IsMapped() const {return ((m_wTracingFlgs & TRACING_FLG_MAPPED) == TRACING_FLG_MAPPED);}
	bool		IsOff() const {return ((m_wStateFlgs & STATE_FLG_OFF) == STATE_FLG_OFF);}
	bool		IsOn() const {return !IsOff();}
//	bool		IsOpened() const {return ((m_wTracingFlgs & TRACING_FLG_OPENED) == TRACING_FLG_OPENED);} 
	bool		IsCloaked() const {return ((m_wTracingFlgs & TRACING_FLG_CLOAKED) == TRACING_FLG_CLOAKED);}
	bool		IsResident() const {return ((m_wStateFlgs & STATE_FLG_RESIDENT) == STATE_FLG_RESIDENT);}
	bool		IsViewed() const {return ((m_wTracingFlgs & TRACING_FLG_VIEWED) == TRACING_FLG_VIEWED);}
	bool		IsWarm() const {return ((m_wStateFlgs & STATE_FLG_WARM) == STATE_FLG_WARM);}
	PENCOLOR	PenColor() const {return m_nPenColor;}
	PENSTYLE	PenStyle() const;
	void		PenTranslation(WORD, PENCOLOR*, PENCOLOR*);
	void		PenStyleTranslation(const PENSTYLE& nStyleNew, const PENSTYLE& nStyleOld);
	void		RemoveFromSegLists();
	void		SetPenColor(PENCOLOR n) {m_nPenColor = n;}
	void		SetStateFlg(WORD w) {m_wStateFlgs |= w;}
	void		SetStateCold();
	void		SetStateHot();
	void		SetStateOff();
	void		SetStateWarm();
	void		SetPenStyleName(const CString& strName) {m_strPenStyleName = strName;}
	void		SetName(const CString& strName) {m_strName = strName;}
	void		SetTracingFlg(WORD w) {m_wTracingFlgs = w;}

	//mak
//	void		PrintLayer(CDC* pDC, bool bIdentifyTrap);
};

typedef CTypedPtrArray<CObArray, CLayer*> CLayers;
