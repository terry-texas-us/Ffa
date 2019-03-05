#pragma once

class CModelView : public CAbstractView
{
private:
	double			m_dUMin;
	double			m_dVMin;
	double			m_dUMax;
	double			m_dVMax; 
	CTMat			m_tmView;
	CTMat			m_tmViewInverse;

public: // Constructors and destructor
	CModelView();
	CModelView(const CModelView& src);

	~CModelView() {}

public: // Operators
	CModelView& operator=(const CModelView& src);

public: // Methods
	void	AdjustWnd(double dAspectRatio);
	void	BuildTransformMatrix();
	void	DoTransform(CPnt4s& pta) const;
	void	DoTransform(CPnt4& pt) const;
	void	DoTransform(int iPts, CPnt4* pt) const;
	void	DoTransform(CVec4& v) const;
	
	CTMat	GetMatrix() const {return m_tmView;}
	CTMat	GetMatrixInverse() const {return m_tmViewInverse;}
	double	GetUExt() const {return m_dUMax - m_dUMin;}
	double	GetUMax() const {return m_dUMax;}
	double	GetUMin() const {return m_dUMin;}
	double	GetVExt() const {return m_dVMax - m_dVMin;}
	double	GetVMax() const {return m_dVMax;}
	double	GetVMin() const {return m_dVMin;}
	
	void	Initialize();
	void	SizeUpdate();
	void	LoadIdentity() {m_tmView.Identity();}
	void	SetCenteredWnd(double dUExt, double dVExt);
	void	SetMatrix(const CTMat& tm) {m_tmView = tm;}
	void	SetWnd(double dUMin, double dVMin, double dUMax, double dVMax);

	void	RotateNorm(const CVec& vNorm);
	void	RotateZ(double dSinAng, double dCosAng);
	void	Scale(const CVec& v);
	void	Translate(const CVec& v);
};

typedef CList<CModelView> CModelViews;

extern CModelViews mvs;
extern CModelView vActive;

namespace view
{
	inline void		AdjustWnd(double dAspectRatio) {vActive.AdjustWnd(dAspectRatio);}
	void			AdjustWnd(double&, double&, double&, double&, double);
	inline void		GetActive(CModelView& v) {v = vActive;}
	inline CVec		GetDirection() {return vActive.GetDirection();}
	inline CPnt		GetEye() {return vActive.GetEye();}
	inline double	GetFarClipDistance() {return vActive.GetFarClipDistance();}
	inline double	GetLensLength() {return vActive.GetLensLength();}
	inline CTMat	GetMatrix() {return vActive.GetMatrix();}
	inline CTMat	GetMatrixInverse() {return vActive.GetMatrixInverse();}
	inline double	GetNearClipDistance() {return vActive.GetNearClipDistance();}
	inline CPnt		GetTarget() {return vActive.GetTarget();}
	inline double	GetUExt() {return vActive.GetUExt();}
	inline double	GetUMax() {return vActive.GetUMax();}
	inline double	GetUMin() {return vActive.GetUMin();}
	inline double	GetVExt() {return vActive.GetVExt();}
	inline double	GetVMax() {return vActive.GetVMax();}
	inline double	GetVMin() {return vActive.GetVMin();}
	inline CVec		GetVwUp() {return vActive.GetVwUp();}
	inline void		Initialize() {vActive.Initialize();}
	inline void		SizeUpdate() {vActive.SizeUpdate(); }
	inline void		LoadIdentity() {vActive.LoadIdentity();}
	inline void		LookAt(const CPnt& ptEye, const CPnt& ptTarget, const CVec& vVwUp) {vActive.LookAt(ptEye, ptTarget, vVwUp); vActive.BuildTransformMatrix();}

	void			PopActive();
	void			PushActive();

	inline void		RotateNorm(const CVec& vNorm) {vActive.RotateNorm(vNorm);}
	inline void		RotateZ(double dSinAng, double dCosAng) {vActive.RotateZ(dSinAng, dCosAng);}
	inline void		Scale(const CVec& v) {vActive.Scale(v);}
	inline void		SetActive(const CModelView& v) {vActive = v; vActive.BuildTransformMatrix();}
	inline void		SetCenteredWnd(double dUExt = 0., double dVExt = 0.) {vActive.SetCenteredWnd(dUExt, dVExt);}
	inline void		SetDirection(const CVec& v) {vActive.SetDirection(v); vActive.BuildTransformMatrix();}
	inline void		SetEye(const CPnt& pt) {vActive.SetEye(pt); vActive.BuildTransformMatrix();}
	inline void		SetEye(const CVec& v) {vActive.SetEye(v); vActive.BuildTransformMatrix();}
	inline void		SetFarClipDistance(double d) {vActive.SetFarClipDistance(d);  vActive.BuildTransformMatrix();}
	inline void		SetLensLength(double d) {vActive.SetLensLength(d);  vActive.BuildTransformMatrix();}
	inline void		SetMatrix(const CTMat& tm) {vActive.SetMatrix(tm);}
	inline void		SetNearClipDistance(double d) {vActive.SetNearClipDistance(d); vActive.BuildTransformMatrix();}
	inline void		SetPerspectiveEnabled(bool b) {vActive.SetPerspectiveEnabled(b);  vActive.BuildTransformMatrix();}
	inline void		SetTarget(const CPnt& pt) {vActive.SetTarget(pt); vActive.BuildTransformMatrix();}
	inline void		SetVwUp(const CVec& v) {vActive.SetVwUp(v);  vActive.BuildTransformMatrix();}
	inline void		SetWnd(double dUMin, double dVMin, double dUMax, double dVMax) {vActive.SetWnd(dUMin, dVMin, dUMax, dVMax);}
	inline void		Translate(const CVec& v) {vActive.Translate(v);}
}

extern CModelView vOver;

inline void	ModelView_DoTransform(CPnt4& pt) {vActive.DoTransform(pt);}
inline void	ModelView_DoTransform(int iPts, CPnt4* pt) {vActive.DoTransform(iPts, pt);}
inline void	ModelView_DoTransform(CPnt4s& pta) {vActive.DoTransform(pta);}
inline void	ModelView_DoTransform(CVec4& v) {vActive.DoTransform(v);}