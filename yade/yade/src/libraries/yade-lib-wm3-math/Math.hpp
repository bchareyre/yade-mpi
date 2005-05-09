// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3MATH_H
#define WM3MATH_H

//#include "Wm3System.h"

//namespace Wm3
//{

#include <cmath>
#include <cstdlib>
#include <cassert>
#include <cfloat>
#include <algorithm>



template <class RealType>
class Math
{
public:
    // Wrappers to hide implementations of functions.  The ACos and ASin
    // functions clamp the input argument to [-1,1] to avoid NaN issues
    // when the input is slightly larger than 1 or slightly smaller than -1.
    // Other functions have the potential for using a fast and approximate
    // algorithm rather than calling the standard math library functions.
    static RealType aCos (RealType fValue);
    static RealType aSin (RealType fValue);
    static RealType aTan (RealType fValue);
    static RealType aTan2 (RealType fY, RealType fX);
    static RealType roundUp (RealType fValue);
    static RealType cosinus (RealType fValue);
    static RealType eExp (RealType fValue);
    static RealType fAbs (RealType fValue);
    static RealType roundDown (RealType fValue);
    static RealType fMod (RealType fX, RealType fY);
    static RealType invSqRoot (RealType fValue);
    static RealType logarithm (RealType fValue);
    static RealType power (RealType fBase, RealType fExponent);
    static RealType sinus (RealType fValue);
    static RealType sqr (RealType fValue);
    static RealType sqRoot (RealType fValue);
    static RealType tangent (RealType fValue);

    // Return -1 if the input is negative, 0 if the input is zero, and +1
    // if the input is positive.
    static int sign (int iValue);
    static RealType sign (RealType fValue);

    // Generate a random number in [0,1).  The random number generator may
    // be seeded by a first call to UnitRandom with a positive seed.
    static RealType unitRandom (unsigned int uiSeed = 0);

    // Generate a random number in [-1,1).  The random number generator may
    // be seeded by a first call to SymmetricRandom with a positive seed.
    static RealType symmetricRandom (unsigned int uiSeed = 0);

    // Generate a random number in [min,max).  The random number generator may
    // be seeded by a first call to IntervalRandom with a positive seed.
    static RealType intervalRandom (RealType fMin, RealType fMax, unsigned int uiSeed = 0);

    // Fast evaluation of trigonometric and inverse trigonometric functions
    // using polynomial approximations.  The speed ups were measured on an
    // AMD 2800 (2.08 GHz) processor using Visual Studion .NET 2003 with a
    // release build.

    // The input must be in [0,pi/2].
    // max error sin0 = 1.7e-04, speed up = 4.0
    // max error sin1 = 1.9e-08, speed up = 2.8
    static RealType fastSin0 (RealType fAngle);
    static RealType fastSin1 (RealType fAngle);

    // The input must be in [0,pi/2]
    // max error cos0 = 1.2e-03, speed up = 4.5
    // max error cos1 = 6.5e-09, speed up = 2.8
    static RealType fastCos0 (RealType fAngle);
    static RealType fastCos1 (RealType fAngle);

    // The input must be in [0,pi/4].
    // max error tan0 = 8.1e-04, speed up = 5.6
    // max error tan1 = 1.9e-08, speed up = 3.4
    static RealType fastTan0 (RealType fAngle);
    static RealType fastTan1 (RealType fAngle);

    // The input must be in [0,1].
    // max error invsin0 = 6.8e-05, speed up = 7.5
    // max error invsin1 = 1.4e-07, speed up = 5.5
    static RealType fastInvSin0 (RealType fValue);
    static RealType fastInvSin1 (RealType fValue);

    // The input must be in [0,1].
    // max error invcos0 = 6.8e-05, speed up = 7.5
    // max error invcos1 = 1.4e-07, speed up = 5.7
    static RealType fastInvCos0 (RealType fValue);
    static RealType fastInvCos1 (RealType fValue);

    // The input must be in [-1,1].
    // max error invtan0 = 1.2e-05, speed up = 2.8
    // max error invtan1 = 2.3e-08, speed up = 1.8
    static RealType fastInvTan0 (RealType fValue);
    static RealType fastInvTan1 (RealType fValue);

    // A fast approximation to 1/sqrt.
    static RealType fastInvSqrt (RealType fValue);

    // common constants
    static const RealType EPSILON;
    static const RealType ZERO_TOLERANCE;
    static const RealType MAX_REAL;
    static const RealType PI;
    static const RealType TWO_PI;
    static const RealType HALF_PI;
    static const RealType INV_PI;
    static const RealType INV_TWO_PI;
    static const RealType DEG_TO_RAD;
    static const RealType RAD_TO_DEG;
};

#include "Math.ipp"

// Fast conversion from a IEEE 32-bit floating point number F in [0,1] to a
// a 32-bit integer I in [0,2^L-1].
//
//   fFloat = F
//   iLog = L
//   iInt = I

#define WM3_SCALED_FLOAT_TO_INT(fFloat,iLog,iInt)\
{ \
    int iShift = 150 - iLog - ((*(int*)(&fFloat) >> 23) & 0xFF); \
    if ( iShift < 24 ) \
    { \
        iInt = ((*(int*)(&fFloat) & 0x007FFFFF) | \
            0x00800000) >> iShift; \
        if ( iInt == (1 << iLog) ) \
        { \
            iInt--; \
        } \
    } \
    else \
    { \
        iInt = 0; \
    } \
}

#ifdef DOUBLE_PRECISION
	typedef double Real;
#else
	typedef float Real;
#endif

typedef Math<float> Mathf;
typedef Math<double> Mathd;

typedef Math<Real> Mathr;


//}

#endif

