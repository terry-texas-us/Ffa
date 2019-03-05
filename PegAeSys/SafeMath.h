#pragma once

const double PI = 3.141592653589793;
const double HALF_PI = (PI * .5);
const double RADIAN = (PI / 180.);
const double TWOPI = (PI * 2.);

inline double Sqr(double d)
{
    return d * d;
}

inline int Signum(double d) // sign +1, 0 or -1
{
    return (d < 0 ? - 1 : ( d > 0 ? 1 : 0));
}

inline double Sqrt(double d)
{
    if (d < 0.)
    {
		msgWarning(IDS_MSG_SQRT_NEG);
		return (0.);
	}
    return sqrt(d);
}

inline double Tan(double d) // tangent function with security check
{
    double s = sin(d), c = cos(d);
    return c == 0 ? 1e30 * Signum(s) : s / c; // avoid div. by zero
}

inline int Round(double d) // closest integer
{
    return  (int) (d + 0.5);
}

inline double Arc(double degrees)  // arc length of an angle
{
    return degrees * (PI / 180);
}

inline double Deg(double arc)
    // angle in degree, corresponding to an arc length
{
    return arc * (180 / PI);
}

// Arc functions
inline double ArcSin(double d)
{
    const double dmax = 1 - FLT_EPSILON;
	const double dmin = - (1 - FLT_EPSILON);
    return (d > dmax ? (PI / 2) : (d < dmin ? - (PI / 2): (atan(d / sqrt(1 - d * d)))));
}

// min and max functions, both for integer and double variables
inline double Min(const double dA, const double dB)
{
	return (dA < dB ? dA : dB);
}

inline int Min(const int iA, const int iB)
{
	return (iA < iB ? iA : iB);
}

inline double Max(double dA, double dB)
{
    return dA < dB ? dB : dA;
}

inline int Max(int iA, int iB)
{
    return iA < iB ? iB : iA;
}