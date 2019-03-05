#pragma once

class CTMat : public CMat4
{
	public:	// Constructors amd destructor
		CTMat() : CMat4() {CMat4::Identity();}
		CTMat(const CMat4& m) : CMat4(m) {}
		CTMat(const CVec4& v0, const CVec4& v1, const CVec4& v2, const CVec4& v3) : CMat4(v0, v1, v2, v3) {}
		
		CTMat(const CPnt& pt, const CVec& v) {DefUsingArbPtAndAx(pt, v);}
		CTMat(const CPnt& pt, const CVec& v, const double);
		CTMat(const CPnt& pt, const CVec& vXRef, const CVec& vYRef);		
		CTMat(int*, const CVec&);

		~CTMat() {}

	public: // Operators	
		
	public: // Methods
		void	DefUsingArbPtAndAx(const CPnt& ptP, const CVec& vN);
		CTMat	RotateX(const double dSinAng, const double dCosAng);
		CTMat	RotateY(const double dSinAng, const double dCosAng);
		CTMat	RotateZ(const double dSinAng, const double dCosAng);
		void	Scale(const CVec&);
		void	Translate(const CPnt&);
		void	Translate(const CVec&);

	public: // Friends
		friend CPnt operator*(const CTMat& tm, const CPnt& pt);
		friend CPnt4 operator*(const CTMat& tm, const CPnt4& pt);
		
		friend CVec operator*(const CTMat& tm, const CVec& v);
		friend CVec4 operator*(const CTMat& tm, const CVec4& v);
};