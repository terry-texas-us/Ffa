#pragma once

class CVaxFloat
{
	public:
		CVaxFloat() {m_f = 0.f;}

		void Convert(const double&);
		double Convert() const;
		
	private:	
		float m_f;
};

class CVaxPnt
{
	public:
		CVaxPnt() {}

		void Convert(const CPnt&);
		CPnt Convert() const;

	private:	
		CVaxFloat x;
		CVaxFloat y;
		CVaxFloat z;
};

class CVaxVec
{
	public:
		CVaxVec() {}

		void Convert(const CVec&);
		CVec Convert() const;
		
	private:	
		CVaxFloat x;
		CVaxFloat y;
		CVaxFloat z;
};