#pragma once

/*
The view target and the direction create a display coordinate system (DCS).
The z-direction equals the direction from target to camera (points toward viewer)
The DCS x-direction is calculated as cross product of (0,0,1) and the view direction.
If the view direction is parallel to (0,0,1), the x-direction is (1,0,0).
The DCS origin is the target point.

Parallel projections:
	The view may be offset from the line of sight (target point may be off the screen)
	A view need additional data to define DCS rectangle to be displayed. The rectangle's
	center is a 2d point (located on the DCS xy plane). Its size is given by its height and width;
	its rotation around its center is given be the twist angle.

Perspective projections:
	The DCS rectangle is always centered around the target point (DCS origin).
	Its size is taken from the views length. Start with a rectangle of 42 units diagonal length
	located at lens length distance from camera point. 
	By dividing 42 times the view distance by the lens length, getting the diagonal length of the DCS rectangle.
	Next, select a rectangle with the smae proportions as the views width and height, and rotate this by view twist angle.
*/

const short AV_PERSPECTIVE = 0x01;
const short AV_NEARCLIPPING = 0x02;
const short AV_FARCLIPPING = 0x04;

class CAbstractView : public CObject
{
protected:
	short	m_nViewMode;			//	bit 1	Perspective mode flag for this view
									//	bit 2	Front clipping plane status for this view
									//	bit 3	Back clipping plane status for this view
									//	bit 16	Front clipping plane is located at the camera
	short	m_nRenderMode;			// render mode for this view (not used)
	short	m_sUcsOrthoViewType;	// Orthographic type of UCS (not used)
									//	top	1, bottom 2, front 3, back 4, left 5, right 6
	CPnt	m_ptUCSOrigin;			// UCS origin
	CVec	m_vUCSXAxis;			// UCS X axis
	CVec	m_vUCSYAxis;			// UCS Y axis
	double	m_dElevation;			// elevation of the UCS plane for this view  

	CPnt	m_ptEye;
	CPnt	m_ptTarget;				// view target in WCS
	CVec	m_vDirection;			// view direction in WCS.
	CVec	m_vVwUp;
	double	m_dViewTwist;			// twist angle of this view in radians
	double	m_dHeight;				// view height in DCS
	double	m_dWidth;				// view width in DCS
	double	m_dCenter[2];			// center point of the view in DCS 
	double	m_dLensLength;			// lens length used for perspective mode in this view (DXF 42).
	double	m_dNearClipDistance;	// distance from the target to the near (front) clipping plane along the target-camera line.
	double	m_dFarClipDistance;		// distance from the target to the far (back) clipping plane along the target-camera line

public: // Constructors and destructor
	CAbstractView();
	CAbstractView(const CAbstractView& av);
	CAbstractView(OdDbViewportPtr pView);
	~CAbstractView() {}

public: // Operators
	CAbstractView& operator=(const CAbstractView& av);

public: // Methods
	void	GetCenter(double& dX, double& dY) const {dX = m_dCenter[0]; dY = m_dCenter[1];}
	CVec	GetDirection() const {return m_vDirection;}
	double* GetDirection(double* d) const {m_vDirection.Get(d); return (d);}
	CPnt	GetEye() const {return m_ptEye;}
	double	GetFarClipDistance() const {return m_dFarClipDistance;}
	double	GetHeight() const {return m_dHeight;}
	double	GetLensLength() const {return m_dLensLength;}
	double	GetNearClipDistance() const {return m_dNearClipDistance;}
	CPnt	GetTarget() const {return m_ptTarget;}
	double* GetTarget(double* d) const {m_ptTarget.Get(d); return (d);}
	double	GetUMax() const {return m_dCenter[0] + m_dWidth / 2.;}
	double	GetUMin() const {return m_dCenter[0] - m_dWidth / 2.;}
	double	GetVMax() const {return m_dCenter[1] + m_dHeight / 2.;}
	double	GetVMin() const {return m_dCenter[1] - m_dHeight / 2.;}
	CVec	GetVwUp() const {return m_vVwUp;}
	double* GetVwUp(double* d) const {m_vVwUp.Get(d); return (d);}
	
	bool	IsNearClippingEnabled() const {return ((m_nViewMode & AV_NEARCLIPPING) == AV_NEARCLIPPING);}
	bool	IsFarClippingEnabled() const {return ((m_nViewMode & AV_FARCLIPPING) == AV_FARCLIPPING);}
	bool	IsPerspectiveEnabled() const {return ((m_nViewMode & AV_PERSPECTIVE) == AV_PERSPECTIVE);}

	void	LookAt(const CPnt& ptEye, const CPnt& ptTarget, const CVec& vVwUp);
	void	SetDirection(const CVec& v);
	void	SetEye(const CPnt& pt) {m_ptEye = pt;}
	void	SetEye(const CVec& v);
	void	SetFarClipDistance(const double dFar) {m_dFarClipDistance = dFar;}
	void	SetFarClippingEnabled(bool);
	void	SetLensLength(double dLen) {m_dLensLength = dLen;}
	void	SetNearClipDistance(const double dNear) {m_dNearClipDistance = dNear;}
	void	SetNearClippingEnabled(bool);
	void	SetPerspectiveEnabled(bool);
	void	SetTarget(const CPnt& pt) {m_ptTarget = pt;}
	void	SetVwUp(const CVec&);
};