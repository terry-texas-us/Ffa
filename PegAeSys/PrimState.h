#pragma once

class CPrimState
{
	public:
		CPrimState();

	private:
		short			m_nMarkStyle;	
		PENCOLOR		m_nPenColor;
		PENSTYLE		m_nPenStyle;	
		CFontDef		m_fd;
		CCharCellDef	m_ccd;
		short			m_nPolygonIntStyle;
		short			m_nPolygonIntStyleId;

	public: // Constructors and destructor
	public: // Operators
		const CPrimState& operator=(const CPrimState&);

	public: // Methods
		void			GetCharCellDef(CCharCellDef& ccd) const {ccd = m_ccd;}
		void			GetFontDef(CFontDef& fd) const {fd = m_fd;}
		const short&	MarkStyle() const {return m_nMarkStyle;}
		const PENCOLOR&	PenColor() const {return (m_nPenColor);}
		const PENSTYLE&	PenStyle() const {return (m_nPenStyle);}
		const short&	PolygonIntStyle() const {return (m_nPolygonIntStyle);}
		const short&	PolygonIntStyleId() const {return (m_nPolygonIntStyleId);}
		void			Restore(CDC* pDC, int iSaveId);
		int				Save();
		void			SetCharCellDef(const CCharCellDef& ccd) {m_ccd = ccd;}
		void			SetFontDef(CDC* pDC, const CFontDef& fd);
		void			SetMarkStyle(short n) {m_nMarkStyle = n;}
		void			SetPolygonIntStyle(short n) {m_nPolygonIntStyle = n;}
		void			SetPolygonIntStyleId(short n) {m_nPolygonIntStyleId = n;}
		void			SetPen(CDC* pDC, PENCOLOR nPenColor, int nPenWidth, PENSTYLE nPenStyle);
		void			SetPen(CDC* pDC, PENCOLOR nPenColor, PENSTYLE nPenStyle);
		void			SetPenColor(PENCOLOR nPenColor);
		void			SetPenColor(CDC* pDC, PENCOLOR nPenColor);
		void			SetPenStyle(PENSTYLE nPenStyle);
		void			SetPenStyle(CDC* pDC, PENSTYLE nPenStyle);
		int				SetROP2(CDC* pDC, int iDrawMode);
		void			SetTxtAlign(CDC* pDC, WORD wHorAlign, WORD wVerAlign);

		short			GetPolygonIntStyle() { return m_nPolygonIntStyle; }
		short			GetPolygonIntStyleId() { return m_nPolygonIntStyleId; }

		void			SetBrush(CDC *pDC, PENCOLOR nPenColor);
};

extern CPrimState pstate;