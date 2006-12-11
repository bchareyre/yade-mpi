// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef YADE_WM3MATH_H
#define YADE_WM3MATH_H

#warning Deprecated header, include <Wm3Math.h> and <yade/yade-core/yadeWm3.hpp> instead

#ifndef USE_BASTARDIZED_WM3
#include<Wm3Math.h>
#include<yade/yade-core/yadeWm3.hpp>
using namespace Wm3;
#else

#ifdef DOUBLE_PRECISION
	typedef double Real;
#else
	typedef float Real;
#endif

// for i in *.hpp; do perl -pi -e"s@#warning.*@#warning Deprecated header, include <Wm3${i%%.hpp}.h> and <yade/yade-core/yadeWm3.hpp> instead@" $i; done


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
	__attribute__((deprecated)) inline     static RealType aCos(RealType fValue){return ACos(fValue);}
    static RealType ACos(RealType fValue) ;
	__attribute__((deprecated)) inline     static RealType aSin(RealType fValue){return ASin(fValue);}
    static RealType ASin(RealType fValue) ;
	__attribute__((deprecated)) inline     static RealType aTan(RealType fValue){return ATan(fValue);}
    static RealType ATan(RealType fValue) ;
	__attribute__((deprecated)) inline     static RealType aTan2(RealType fY, RealType fX){return ATan2(fY,fX);}
    static RealType ATan2(RealType fY, RealType fX) ;
	__attribute__((deprecated)) inline     static RealType roundUp(RealType fValue){return Ceil(fValue);}
    static RealType Ceil(RealType fValue) ;
	__attribute__((deprecated)) inline     static RealType cosinus(RealType fValue){return Cos(fValue);}
    static RealType Cos(RealType fValue) ;
	__attribute__((deprecated)) inline     static RealType eExp(RealType fValue){return Exp(fValue);}
    static RealType Exp(RealType fValue) ;
	__attribute__((deprecated)) inline     static RealType fAbs(RealType fValue){return FAbs(fValue);}
    static RealType FAbs(RealType fValue) ;
	__attribute__((deprecated)) inline     static RealType roundDown(RealType fValue){return Floor(fValue);}
    static RealType Floor(RealType fValue) ;
	__attribute__((deprecated)) inline     static RealType fMod(RealType fX, RealType fY){return FMod(fX,fY);}
    static RealType FMod(RealType fX, RealType fY) ;
	__attribute__((deprecated)) inline     static RealType invSqRoot(RealType fValue){return InvSqrt(fValue);}
    static RealType InvSqrt(RealType fValue) ;
	__attribute__((deprecated)) inline     static RealType logarithm(RealType fValue){return Log(fValue);}
    static RealType Log(RealType fValue) ;
	__attribute__((deprecated)) inline     static RealType power(RealType fBase, RealType fExponent){return Pow(fBase,fExponent);}
    static RealType Pow(RealType fBase, RealType fExponent) ;
	__attribute__((deprecated)) inline     static RealType sinus(RealType fValue){return Sin(fValue);}
    static RealType Sin(RealType fValue) ;
	__attribute__((deprecated)) inline     static RealType sqr(RealType fValue){return Sqr(fValue);}
    static RealType Sqr(RealType fValue) ;
	__attribute__((deprecated)) inline     static RealType sqRoot(RealType fValue){return Sqrt(fValue);}
    static RealType Sqrt(RealType fValue) ;
	__attribute__((deprecated)) inline     static RealType tangent(RealType fValue){return Tan(fValue);}
    static RealType Tan(RealType fValue) ;

    // Return -1 if the input is negative, 0 if the input is zero, and +1
    // if the input is positive.
	__attribute__((deprecated)) inline     static int sign(int iValue){return Sign(iValue);}
    static int Sign(int iValue) ;
	__attribute__((deprecated)) inline     static RealType sign(RealType fValue){return Sign(fValue);}
    static RealType Sign(RealType fValue) ;

    // Generate a random number in [0,1).  The random number generator may
    // be seeded by a first call to UnitRandom with a positive seed.
	__attribute__((deprecated)) inline     static RealType unitRandom(unsigned int uiSeed = 0){return UnitRandom(0);}
    static RealType UnitRandom(unsigned int uiSeed = 0) ;

    // Generate a random number in [-1,1).  The random number generator may
    // be seeded by a first call to SymmetricRandom with a positive seed.
	__attribute__((deprecated)) inline     static RealType symmetricRandom(unsigned int uiSeed = 0){return SymmetricRandom(0);}
    static RealType SymmetricRandom(unsigned int uiSeed = 0) ;

    // Generate a random number in [min,max).  The random number generator may
    // be seeded by a first call to IntervalRandom with a positive seed.
	__attribute__((deprecated)) inline     static RealType intervalRandom(RealType fMin, RealType fMax, unsigned int uiSeed = 0){return IntervalRandom(fMin,fMax,0);}
    static RealType IntervalRandom(RealType fMin, RealType fMax, unsigned int uiSeed = 0) ;

    // Fast evaluation of trigonometric and inverse trigonometric functions
    // using polynomial approximations.  The speed ups were measured on an
    // AMD 2800 (2.08 GHz) processor using Visual Studion .NET 2003 with a
    // release build.

    // The input must be in [0,pi/2].
    // max error sin0 = 1.7e-04, speed up = 4.0
    // max error sin1 = 1.9e-08, speed up = 2.8
	__attribute__((deprecated)) inline     static RealType fastSin0(RealType fAngle){return FastSin0(fAngle);}
    static RealType FastSin0(RealType fAngle) ;
	__attribute__((deprecated)) inline     static RealType fastSin1(RealType fAngle){return FastSin1(fAngle);}
    static RealType FastSin1(RealType fAngle) ;

    // The input must be in [0,pi/2]
    // max error cos0 = 1.2e-03, speed up = 4.5
    // max error cos1 = 6.5e-09, speed up = 2.8
	__attribute__((deprecated)) inline     static RealType fastCos0(RealType fAngle){return FastCos0(fAngle);}
    static RealType FastCos0(RealType fAngle) ;
	__attribute__((deprecated)) inline     static RealType fastCos1(RealType fAngle){return FastCos1(fAngle);}
    static RealType FastCos1(RealType fAngle) ;

    // The input must be in [0,pi/4].
    // max error tan0 = 8.1e-04, speed up = 5.6
    // max error tan1 = 1.9e-08, speed up = 3.4
	__attribute__((deprecated)) inline     static RealType fastTan0(RealType fAngle){return FastTan0(fAngle);}
    static RealType FastTan0(RealType fAngle) ;
	__attribute__((deprecated)) inline     static RealType fastTan1(RealType fAngle){return FastTan1(fAngle);}
    static RealType FastTan1(RealType fAngle) ;

    // The input must be in [0,1].
    // max error invsin0 = 6.8e-05, speed up = 7.5
    // max error invsin1 = 1.4e-07, speed up = 5.5
	__attribute__((deprecated)) inline     static RealType fastInvSin0(RealType fValue){return FastInvSin0(fValue);}
    static RealType FastInvSin0(RealType fValue) ;
	__attribute__((deprecated)) inline     static RealType fastInvSin1(RealType fValue){return FastInvSin1(fValue);}
    static RealType FastInvSin1(RealType fValue) ;

    // The input must be in [0,1].
    // max error invcos0 = 6.8e-05, speed up = 7.5
    // max error invcos1 = 1.4e-07, speed up = 5.7
	__attribute__((deprecated)) inline     static RealType fastInvCos0(RealType fValue){return FastInvCos0(fValue);}
    static RealType FastInvCos0(RealType fValue) ;
	__attribute__((deprecated)) inline     static RealType fastInvCos1(RealType fValue){return FastInvCos1(fValue);}
    static RealType FastInvCos1(RealType fValue) ;

    // The input must be in [-1,1].
    // max error invtan0 = 1.2e-05, speed up = 2.8
    // max error invtan1 = 2.3e-08, speed up = 1.8
	__attribute__((deprecated)) inline     static RealType fastInvTan0(RealType fValue){return FastInvTan0(fValue);}
    static RealType FastInvTan0(RealType fValue) ;
	__attribute__((deprecated)) inline     static RealType fastInvTan1(RealType fValue){return FastInvTan1(fValue);}
    static RealType FastInvTan1(RealType fValue) ;

    // A fast approximation to 1/sqrt.
	__attribute__((deprecated)) inline     static RealType fastInvSqrt(RealType fValue){return FastInvSqrt(fValue);}
    static RealType FastInvSqrt(RealType fValue) ;

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


typedef Math<float> Mathf;
typedef Math<double> Mathd;
typedef Math<Real> Mathr;


//}

#endif
#endif
