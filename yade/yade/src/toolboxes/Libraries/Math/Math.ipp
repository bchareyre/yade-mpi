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
RealType Math<RealType>::aCos (RealType fValue)
{
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
RealType Math<RealType>::aSin (RealType fValue)
{
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
RealType Math<RealType>::aTan (RealType fValue)
{
    return (RealType)atan((double)fValue);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::aTan2 (RealType fY, RealType fX)
{
    return (RealType)atan2((double)fY,(double)fX);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::roundUp (RealType fValue)
{
    return (RealType)ceil((double)fValue);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::cosinus (RealType fValue)
{
    return (RealType)cos((double)fValue);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::eExp (RealType fValue)
{
    return (RealType)exp((double)fValue);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::fAbs (RealType fValue)
{
    return (RealType)fabs((double)fValue);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::roundDown (RealType fValue)
{
    return (RealType)floor((double)fValue);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::fMod (RealType fX, RealType fY)
{
    return (RealType)fmod((double)fX,(double)fY);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::invSqRoot (RealType fValue)
{
    return (RealType)(1.0/sqRoot((double)fValue));
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::logarithm (RealType fValue)
{
    return (RealType)log((double)fValue);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::power (RealType fBase, RealType fExponent)
{
    return (RealType)pow((double)fBase,(double)fExponent);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::sinus (RealType fValue)
{
    return (RealType)sin((double)fValue);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::sqr (RealType fValue)
{
    return fValue*fValue;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::sqRoot (RealType fValue)
{
    return (RealType)sqrt((double)fValue);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::tangent (RealType fValue)
{
    return (RealType)tan((double)fValue);
}
//----------------------------------------------------------------------------
template <class RealType>
int Math<RealType>::sign (int iValue)
{
    if ( iValue > 0 )
        return 1;

    if ( iValue < 0 )
        return -1;

    return 0;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::sign (RealType fValue)
{
    if ( fValue > (RealType)0.0 )
        return (RealType)1.0;

    if ( fValue < (RealType)0.0 )
        return -(RealType)1.0;

    return (RealType)0.0;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::unitRandom (unsigned int uiSeed )
{
    if ( uiSeed > 0 )
        srand(uiSeed);

    double dRatio = ((double)rand())/((double)(RAND_MAX));
    return (RealType)dRatio;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::symmetricRandom (unsigned int uiSeed)
{
    if ( uiSeed > 0.0 )
        srand(uiSeed);

    double dRatio = ((double)rand())/((double)(RAND_MAX));
    return (RealType)(2.0*dRatio - 1.0);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::intervalRandom (RealType fMin, RealType fMax, unsigned int uiSeed)
{
    if ( uiSeed > 0 )
        srand(uiSeed);

    double dRatio = ((double)rand())/((double)(RAND_MAX));
    return fMin+(fMax-fMin)*((RealType)dRatio);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Math<RealType>::fastSin0 (RealType fAngle)
{
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
RealType Math<RealType>::fastSin1 (RealType fAngle)
{
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
RealType Math<RealType>::fastCos0 (RealType fAngle)
{
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
RealType Math<RealType>::fastCos1 (RealType fAngle)
{
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
RealType Math<RealType>::fastTan0 (RealType fAngle)
{
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
RealType Math<RealType>::fastTan1 (RealType fAngle)
{
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
RealType Math<RealType>::fastInvSin0 (RealType fValue)
{
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
RealType Math<RealType>::fastInvSin1 (RealType fValue)
{
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
RealType Math<RealType>::fastInvCos0 (RealType fValue)
{
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
RealType Math<RealType>::fastInvCos1 (RealType fValue)
{
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
RealType Math<RealType>::fastInvTan0 (RealType fValue)
{
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
RealType Math<RealType>::fastInvTan1 (RealType fValue)
{
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

