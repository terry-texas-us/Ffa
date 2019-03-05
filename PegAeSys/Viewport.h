#pragma once

class CViewport : public CObject
{
private:
	CPnt	m_ptCenter;					// center of the viewport
	double	m_dHeight;					// height of the viewport
	int		m_iDeviceHeightInPixels;	//
	double	m_dDeviceHeightInInches;	//
	double	m_dWidth;					// width of the viewport
	int		m_iDeviceWidthInPixels;		//
	double  m_dDeviceWidthInInches;		//
	
public: // Constructors and destructors

	CViewport() {}
	CViewport(int iWidth, int iHeight) {Set(iWidth, iHeight);}
	CViewport(OdDbViewportPtr pView);
	CViewport(const CViewport& src);

	~CViewport() {};

	CViewport& operator=(const CViewport& src);

public: // Methods
	double	GetAspectRatio() const {return m_dHeight / m_dWidth;}
	CPnt	GetCenter() const {return m_ptCenter;}
	double	GetHeight() const {return m_dHeight;}
	double	GetWidthInInches() const {return m_dWidth / (m_iDeviceWidthInPixels / m_dDeviceWidthInInches);}
	double	GetHeightInInches() const {return m_dHeight / (m_iDeviceHeightInPixels / m_dDeviceHeightInInches);}
	double	GetWidth() const {return m_dWidth;}

	CPoint	DoProjection(const CPnt4& pt) const;
	void	DoProjection(CPoint* pnt, int iPts, CPnt4* pt) const;
	void	DoProjection(CPoint* pnt, CPnt4s& pta) const;
	void	DoProjectionInverse(CPnt& pt) const;

	void	Set(double dWidth, double dHeight);
	void	SetDeviceHeightInInches(double dHeight) {m_dDeviceHeightInInches = dHeight;}
	void	SetDeviceHeightInPixels(int iHeight) {m_iDeviceHeightInPixels = iHeight;}
	void	SetDeviceWidthInInches(double dWidth) {m_dDeviceWidthInInches = dWidth;}
	void	SetDeviceWidthInPixels(int iWidth) {m_iDeviceWidthInPixels = iWidth;}
	void	SetHeight(double dHeight) {m_dHeight = dHeight;}
	void	SetWidth(double dWidth) {m_dWidth = dWidth;}
	
};

typedef CList<CViewport> CViewports;

extern CViewport vpActive;
extern CViewports vps;

namespace viewport
{
	void			PopActive();
	void			PushActive();
	inline CPoint	DoProjection(const CPnt4& pt) {return vpActive.DoProjection(pt);}
	inline void		DoProjection(CPoint* pnt, int iPts, CPnt4* pt) {vpActive.DoProjection(pnt, iPts, pt);}
	inline void		DoProjection(CPoint* pnt, CPnt4s& pta) {vpActive.DoProjection(pnt, pta);}
	inline void		DoProjectionInverse(CPnt& pt) {vpActive.DoProjectionInverse(pt);}
	inline double	GetAspectRatio() {return vpActive.GetAspectRatio();}
	inline double	GetHeightInInches() {return vpActive.GetHeightInInches();}
	inline double	GetWidthInInches() {return vpActive.GetWidthInInches();}
	inline void		Set(int iWidth, int iHeight) {vpActive.Set(iWidth, iHeight);}
	inline void		SetDeviceHeightInInches(double dHeight) {vpActive.SetDeviceHeightInInches(dHeight);}
	inline void		SetDeviceHeightInPixels(int iHeight) {vpActive.SetDeviceHeightInPixels(iHeight);}
	inline void		SetDeviceWidthInInches(double dWidth) {vpActive.SetDeviceWidthInInches(dWidth);}
	inline void		SetDeviceWidthInPixels(int iWidth) {vpActive.SetDeviceWidthInPixels(iWidth);}

	inline CPnt		GetCenter() {return vpActive.GetCenter();}
	inline double	GetWidth() {return vpActive.GetWidth();}
	inline double	GetHeight() {return vpActive.GetHeight();}
}