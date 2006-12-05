// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::ACos(RealType fValue) {
    if ( -(RealType)1.0 < fValue )
    {
        if ( fValue < (RealType)1.0 )
            return (RealType)acos((double)fValue);
        else
            return (RealType)0.0;
    }
    else
    {
        return PI;
    }
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::ASin(RealType fValue) {
    if ( -(RealType)1.0 < fValue )
    {
        if ( fValue < (RealType)1.0 )
            return (RealType)asin((double)fValue);
        else
            return HALF_PI;
    }
    else
    {
        return -HALF_PI;
    }
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::ATan(RealType fValue) {
    return (RealType)atan((double)fValue);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::ATan2(RealType fY, RealType fX) {
    return (RealType)atan2((double)fY,(double)fX);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::Ceil(RealType fValue) {
    return (RealType)ceil((double)fValue);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::Cos(RealType fValue) {
    return (RealType)cos((double)fValue);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::Exp(RealType fValue) {
    return (RealType)exp((double)fValue);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::FAbs(RealType fValue) {
    return (RealType)fabs((double)fValue);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::Floor(RealType fValue) {
    return (RealType)floor((double)fValue);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::FMod(RealType fX, RealType fY) {
    return (RealType)fmod((double)fX,(double)fY);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::InvSqrt(RealType fValue) {
    return (RealType)(1.0/sqRoot((double)fValue));
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::Log(RealType fValue) {
    return (RealType)log((double)fValue);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::Pow(RealType fBase, RealType fExponent) {
    return (RealType)pow((double)fBase,(double)fExponent);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::Sin(RealType fValue) {
    return (RealType)sin((double)fValue);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::Sqr(RealType fValue) {
    return fValue*fValue;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::Sqrt(RealType fValue) {
    return (RealType)sqrt((double)fValue);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::Tan(RealType fValue) {
    return (RealType)tan((double)fValue);
}
//----------------------------------------------------------------------------
template <class RealType>
int Math<RealType>::Sign(int iValue) {
    if ( iValue > 0 )
        return 1;

    if ( iValue < 0 )
        return -1;

    return 0;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::Sign(RealType fValue) {
    if ( fValue > (RealType)0.0 )
        return (RealType)1.0;

    if ( fValue < (RealType)0.0 )
        return -(RealType)1.0;

    return (RealType)0.0;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::UnitRandom(unsigned int uiSeed ) {
    if ( uiSeed > 0 )
        srand(uiSeed);

    double dRatio = ((double)rand())/((double)(RAND_MAX));
    return (RealType)dRatio;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::SymmetricRandom(unsigned int uiSeed) {
    if ( uiSeed > 0.0 )
        srand(uiSeed);

    double dRatio = ((double)rand())/((double)(RAND_MAX));
    return (RealType)(2.0*dRatio - 1.0);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::IntervalRandom(RealType fMin, RealType fMax, unsigned int uiSeed) {
    if ( uiSeed > 0 )
        srand(uiSeed);

    double dRatio = ((double)rand())/((double)(RAND_MAX));
    return fMin+(fMax-fMin)*((RealType)dRatio);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::FastSin0(RealType fAngle) {
    RealType fASqr = fAngle*fAngle;
    RealType fResult = (RealType)7.61e-03;
    fResult *= fASqr;
    fResult -= (RealType)1.6605e-01;
    fResult *= fASqr;
    fResult += (RealType)1.0;
    fResult *= fAngle;
    return fResult;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::FastSin1(RealType fAngle) {
    RealType fASqr = fAngle*fAngle;
    RealType fResult = -(RealType)2.39e-08;
    fResult *= fASqr;
    fResult += (RealType)2.7526e-06;
    fResult *= fASqr;
    fResult -= (RealType)1.98409e-04;
    fResult *= fASqr;
    fResult += (RealType)8.3333315e-03;
    fResult *= fASqr;
    fResult -= (RealType)1.666666664e-01;
    fResult *= fASqr;
    fResult += (RealType)1.0;
    fResult *= fAngle;
    return fResult;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::FastCos0(RealType fAngle) {
    RealType fASqr = fAngle*fAngle;
    RealType fResult = (RealType)3.705e-02;
    fResult *= fASqr;
    fResult -= (RealType)4.967e-01;
    fResult *= fASqr;
    fResult += (RealType)1.0;
    return fResult;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::FastCos1(RealType fAngle) {
    RealType fASqr = fAngle*fAngle;
    RealType fResult = -(RealType)2.605e-07;
    fResult *= fASqr;
    fResult += (RealType)2.47609e-05;
    fResult *= fASqr;
    fResult -= (RealType)1.3888397e-03;
    fResult *= fASqr;
    fResult += (RealType)4.16666418e-02;
    fResult *= fASqr;
    fResult -= (RealType)4.999999963e-01;
    fResult *= fASqr;
    fResult += (RealType)1.0;
    return fResult;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::FastTan0(RealType fAngle) {
    RealType fASqr = fAngle*fAngle;
    RealType fResult = (RealType)2.033e-01;
    fResult *= fASqr;
    fResult += (RealType)3.1755e-01;
    fResult *= fASqr;
    fResult += (RealType)1.0;
    fResult *= fAngle;
    return fResult;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::FastTan1(RealType fAngle) {
    RealType fASqr = fAngle*fAngle;
    RealType fResult = (RealType)9.5168091e-03;
    fResult *= fASqr;
    fResult += (RealType)2.900525e-03;
    fResult *= fASqr;
    fResult += (RealType)2.45650893e-02;
    fResult *= fASqr;
    fResult += (RealType)5.33740603e-02;
    fResult *= fASqr;
    fResult += (RealType)1.333923995e-01;
    fResult *= fASqr;
    fResult += (RealType)3.333314036e-01;
    fResult *= fASqr;
    fResult += (RealType)1.0;
    fResult *= fAngle;
    return fResult;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::FastInvSin0(RealType fValue) {
    RealType fRoot = Math<RealType>::Sqrt(((RealType)1.0)-fValue);
    RealType fResult = -(RealType)0.0187293;
    fResult *= fValue;
    fResult += (RealType)0.0742610;
    fResult *= fValue;
    fResult -= (RealType)0.2121144;
    fResult *= fValue;
    fResult += (RealType)1.5707288;
    fResult = HALF_PI - fRoot*fResult;
    return fResult;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::FastInvSin1(RealType fValue) {
    RealType fRoot = Math<RealType>::Sqrt(FAbs(((RealType)1.0)-fValue));
    RealType fResult = -(RealType)0.0012624911;
    fResult *= fValue;
    fResult += (RealType)0.0066700901;
    fResult *= fValue;
    fResult -= (RealType)0.0170881256;
    fResult *= fValue;
    fResult += (RealType)0.0308918810;
    fResult *= fValue;
    fResult -= (RealType)0.0501743046;
    fResult *= fValue;
    fResult += (RealType)0.0889789874;
    fResult *= fValue;
    fResult -= (RealType)0.2145988016;
    fResult *= fValue;
    fResult += (RealType)1.5707963050;
    fResult = HALF_PI - fRoot*fResult;
    return fResult;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::FastInvCos0(RealType fValue) {
    RealType fRoot = Math<RealType>::Sqrt(((RealType)1.0)-fValue);
    RealType fResult = -(RealType)0.0187293;
    fResult *= fValue;
    fResult += (RealType)0.0742610;
    fResult *= fValue;
    fResult -= (RealType)0.2121144;
    fResult *= fValue;
    fResult += (RealType)1.5707288;
    fResult *= fRoot;
    return fResult;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::FastInvCos1(RealType fValue) {
    RealType fRoot = Math<RealType>::Sqrt(FAbs(((RealType)1.0)-fValue));
    RealType fResult = -(RealType)0.0012624911;
    fResult *= fValue;
    fResult += (RealType)0.0066700901;
    fResult *= fValue;
    fResult -= (RealType)0.0170881256;
    fResult *= fValue;
    fResult += (RealType)0.0308918810;
    fResult *= fValue;
    fResult -= (RealType)0.0501743046;
    fResult *= fValue;
    fResult += (RealType)0.0889789874;
    fResult *= fValue;
    fResult -= (RealType)0.2145988016;
    fResult *= fValue;
    fResult += (RealType)1.5707963050;
    fResult *= fRoot;
    return fResult;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::FastInvTan0(RealType fValue) {
    RealType fVSqr = fValue*fValue;
    RealType fResult = (RealType)0.0208351;
    fResult *= fVSqr;
    fResult -= (RealType)0.085133;
    fResult *= fVSqr;
    fResult += (RealType)0.180141;
    fResult *= fVSqr;
    fResult -= (RealType)0.3302995;
    fResult *= fVSqr;
    fResult += (RealType)0.999866;
    fResult *= fValue;
    return fResult;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::FastInvTan1(RealType fValue) {
    RealType fVSqr = fValue*fValue;
    RealType fResult = (RealType)0.0028662257;
    fResult *= fVSqr;
    fResult -= (RealType)0.0161657367;
    fResult *= fVSqr;
    fResult += (RealType)0.0429096138;
    fResult *= fVSqr;
    fResult -= (RealType)0.0752896400;
    fResult *= fVSqr;
    fResult += (RealType)0.1065626393;
    fResult *= fVSqr;
    fResult -= (RealType)0.1420889944;
    fResult *= fVSqr;
    fResult += (RealType)0.1999355085;
    fResult *= fVSqr;
    fResult -= (RealType)0.3333314528;
    fResult *= fVSqr;
    fResult += (RealType)1.0;
    fResult *= fValue;
    return fResult;
}
//----------------------------------------------------------------------------

