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
Vector2<RealType>::Vector2 ()
{
    // uninitialized for performance in array construction
}
//----------------------------------------------------------------------------
template <class RealType>
Vector2<RealType>::Vector2 (RealType fX, RealType fY)
{
    m_afTuple[0] = fX;
    m_afTuple[1] = fY;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector2<RealType>::Vector2 (const Vector2& rkV)
{
    m_afTuple[0] = rkV.m_afTuple[0];
    m_afTuple[1] = rkV.m_afTuple[1];
}
//----------------------------------------------------------------------------
template <class RealType>
Vector2<RealType>::operator const RealType* () const
{
    return m_afTuple;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector2<RealType>::operator RealType* ()
{
    return m_afTuple;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector2<RealType>::operator[] (int i) const
{
    assert( 0 <= i && i <= 1 );
    if ( i < 0 )
        i = 0;
    else if ( i > 1 )
        i = 1;

    return m_afTuple[i];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType& Vector2<RealType>::operator[] (int i)
{
    assert( 0 <= i && i <= 1 );
    if ( i < 0 )
        i = 0;
    else if ( i > 1 )
        i = 1;

    return m_afTuple[i];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector2<RealType>::x () const
{
    return m_afTuple[0];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType& Vector2<RealType>::x ()
{
    return m_afTuple[0];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector2<RealType>::y () const
{
    return m_afTuple[1];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType& Vector2<RealType>::y ()
{
    return m_afTuple[1];
}
//----------------------------------------------------------------------------
template <class RealType>
Vector2<RealType>& Vector2<RealType>::operator= (const Vector2& rkV)
{
    m_afTuple[0] = rkV.m_afTuple[0];
    m_afTuple[1] = rkV.m_afTuple[1];
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
int Vector2<RealType>::compareArrays (const Vector2& rkV) const
{
    return memcmp(m_afTuple,rkV.m_afTuple,2*sizeof(RealType));
}
//----------------------------------------------------------------------------
template <class RealType>
bool Vector2<RealType>::operator== (const Vector2& rkV) const
{
    return compareArrays(rkV) == 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Vector2<RealType>::operator!= (const Vector2& rkV) const
{
    return compareArrays(rkV) != 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Vector2<RealType>::operator< (const Vector2& rkV) const
{
    return compareArrays(rkV) < 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Vector2<RealType>::operator<= (const Vector2& rkV) const
{
    return compareArrays(rkV) <= 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Vector2<RealType>::operator> (const Vector2& rkV) const
{
    return compareArrays(rkV) > 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Vector2<RealType>::operator>= (const Vector2& rkV) const
{
    return compareArrays(rkV) >= 0;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector2<RealType> Vector2<RealType>::operator+ (const Vector2& rkV) const
{
    return Vector2(
        m_afTuple[0]+rkV.m_afTuple[0],
        m_afTuple[1]+rkV.m_afTuple[1]);
}
//----------------------------------------------------------------------------
template <class RealType>
Vector2<RealType> Vector2<RealType>::operator- (const Vector2& rkV) const
{
    return Vector2(
        m_afTuple[0]-rkV.m_afTuple[0],
        m_afTuple[1]-rkV.m_afTuple[1]);
}
//----------------------------------------------------------------------------
template <class RealType>
Vector2<RealType> Vector2<RealType>::operator* (RealType fScalar) const
{
    return Vector2(
        fScalar*m_afTuple[0],
        fScalar*m_afTuple[1]);
}
//----------------------------------------------------------------------------
template <class RealType>
Vector2<RealType> Vector2<RealType>::operator/ (RealType fScalar) const
{
    Vector2 kQuot;

    if ( fScalar != (RealType)0.0 )
    {
        RealType fInvScalar = ((RealType)1.0)/fScalar;
        kQuot.m_afTuple[0] = fInvScalar*m_afTuple[0];
        kQuot.m_afTuple[1] = fInvScalar*m_afTuple[1];
    }
    else
    {
        kQuot.m_afTuple[0] = Math<RealType>::MAX_REAL;
        kQuot.m_afTuple[1] = Math<RealType>::MAX_REAL;
    }

    return kQuot;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector2<RealType> Vector2<RealType>::operator- () const
{
    return Vector2(
        -m_afTuple[0],
        -m_afTuple[1]);
}
//----------------------------------------------------------------------------
template <class RealType>
Vector2<RealType> operator* (RealType fScalar, const Vector2<RealType>& rkV)
{
    return Vector2<RealType>(
        fScalar*rkV[0],
        fScalar*rkV[1]);
}
//----------------------------------------------------------------------------
template <class RealType>
Vector2<RealType>& Vector2<RealType>::operator+= (const Vector2& rkV)
{
    m_afTuple[0] += rkV.m_afTuple[0];
    m_afTuple[1] += rkV.m_afTuple[1];
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector2<RealType>& Vector2<RealType>::operator-= (const Vector2& rkV)
{
    m_afTuple[0] -= rkV.m_afTuple[0];
    m_afTuple[1] -= rkV.m_afTuple[1];
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector2<RealType>& Vector2<RealType>::operator*= (RealType fScalar)
{
    m_afTuple[0] *= fScalar;
    m_afTuple[1] *= fScalar;
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector2<RealType>& Vector2<RealType>::operator/= (RealType fScalar)
{
    if ( fScalar != (RealType)0.0 )
    {
        RealType fInvScalar = ((RealType)1.0)/fScalar;
        m_afTuple[0] *= fInvScalar;
        m_afTuple[1] *= fInvScalar;
    }
    else
    {
        m_afTuple[0] = Math<RealType>::MAX_REAL;
        m_afTuple[1] = Math<RealType>::MAX_REAL;
    }

    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector2<RealType>::length () const
{
    return Math<RealType>::Sqrt(
        m_afTuple[0]*m_afTuple[0] +
        m_afTuple[1]*m_afTuple[1]);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector2<RealType>::squaredLength () const
{
    return
        m_afTuple[0]*m_afTuple[0] +
        m_afTuple[1]*m_afTuple[1];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector2<RealType>::dot (const Vector2& rkV) const
{
    return
        m_afTuple[0]*rkV.m_afTuple[0] +
        m_afTuple[1]*rkV.m_afTuple[1];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector2<RealType>::normalize ()
{
    RealType fLength = length();

    if ( fLength > Math<RealType>::ZERO_TOLERANCE )
    {
        RealType fInvLength = ((RealType)1.0)/fLength;
        m_afTuple[0] *= fInvLength;
        m_afTuple[1] *= fInvLength;
    }
    else
    {
        fLength = (RealType)0.0;
        m_afTuple[0] = (RealType)0.0;
        m_afTuple[1] = (RealType)0.0;
    }

    return fLength;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector2<RealType> Vector2<RealType>::perp () const
{
    return Vector2(m_afTuple[1],-m_afTuple[0]);
}
//----------------------------------------------------------------------------
template <class RealType>
Vector2<RealType> Vector2<RealType>::unitPerp () const
{
    Vector2 kPerp(m_afTuple[1],-m_afTuple[0]);
    kPerp.Normalize();
    return kPerp;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector2<RealType>::dotPerp (const Vector2& rkV) const
{
    return m_afTuple[0]*rkV.m_afTuple[1] - m_afTuple[1]*rkV.m_afTuple[0];
}
//----------------------------------------------------------------------------
template <class RealType>
void Vector2<RealType>::getBarycentrics (const Vector2<RealType>& rkV0,
    const Vector2<RealType>& rkV1, const Vector2<RealType>& rkV2, RealType afBary[3])
    const
{
    // compute the vectors relative to V2 of the triangle
    Vector2<RealType> akDiff[3] =
    {
        rkV0 - rkV2,
        rkV1 - rkV2,
        *this - rkV2
    };

    // If the vertices have large magnitude, the linear system of equations
    // for computing barycentric coordinates can be ill-conditioned.  To avoid
    // this, uniformly scale the triangle edges to be of order 1.  The scaling
    // of all differences does not change the barycentric coordinates.
    RealType fMax = (RealType)0.0;
    int i;
    for (i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            RealType fValue = Math<RealType>::fAbs(akDiff[i][j]);
            if ( fValue > fMax )
                fMax = fValue;
        }
    }

    // scale down only large data
    if ( fMax > (RealType)1.0 )
    {
        RealType fInvMax = ((RealType)1.0)/fMax;
        for (i = 0; i < 3; i++)
            akDiff[i] *= fInvMax;
    }

    RealType fDet = akDiff[0].DotPerp(akDiff[1]);
    if ( Math<RealType>::fAbs(fDet) > Math<RealType>::ZERO_TOLERANCE )
    {
        RealType fInvDet = ((RealType)1.0)/fDet;
        afBary[0] = akDiff[2].DotPerp(akDiff[1])*fInvDet;
        afBary[1] = akDiff[0].DotPerp(akDiff[2])*fInvDet;
        afBary[2] = (RealType)1.0 - afBary[0] - afBary[1];
    }
    else
    {
        // The triangle is a sliver.  Determine the longest edge and
        // compute barycentric coordinates with respect to that edge.
        Vector2<RealType> kE2 = rkV0 - rkV1;
        RealType fMaxSqrLength = kE2.SquaredLength();
        int iMaxIndex = 2;
        RealType fSqrLength = akDiff[1].SquaredLength();
        if ( fSqrLength > fMaxSqrLength )
        {
            iMaxIndex = 1;
            fMaxSqrLength = fSqrLength;
        }
        fSqrLength = akDiff[0].SquaredLength();
        if ( fSqrLength > fMaxSqrLength )
        {
            iMaxIndex = 2;
            fMaxSqrLength = fSqrLength;
        }

        if ( fMaxSqrLength > Math<RealType>::ZERO_TOLERANCE )
        {
            RealType fInvSqrLength = ((RealType)1.0)/fMaxSqrLength;
            if ( iMaxIndex == 0 )
            {
                // P-V2 = t(V0-V2)
                afBary[0] = akDiff[2].Dot(akDiff[0])*fInvSqrLength;
                afBary[1] = (RealType)0.0;
                afBary[2] = (RealType)1.0 - afBary[0];
            }
            else if ( iMaxIndex == 1 )
            {
                // P-V2 = t(V1-V2)
                afBary[0] = (RealType)0.0;
                afBary[1] = akDiff[2].Dot(akDiff[1])*fInvSqrLength;
                afBary[2] = (RealType)1.0 - afBary[1];
            }
            else
            {
                // P-V1 = t(V0-V1)
                akDiff[2] = *this - rkV1;
                afBary[0] = akDiff[2].Dot(kE2)*fInvSqrLength;
                afBary[1] = (RealType)1.0 - afBary[0];
                afBary[2] = (RealType)0.0;
            }
        }
        else
        {
            // triangle is a nearly a point, just return equal weights
            afBary[0] = ((RealType)1.0)/(RealType)3.0;
            afBary[1] = afBary[0];
            afBary[2] = afBary[0];
        }
    }
}
//----------------------------------------------------------------------------
template <class RealType>
void Vector2<RealType>::orthonormalize (Vector2& rkU, Vector2& rkV)
{
    // If the input vectors are v0 and v1, then the Gram-Schmidt
    // orthonormalization produces vectors u0 and u1 as follows,
    //
    //   u0 = v0/|v0|
    //   u1 = (v1-(u0*v1)u0)/|v1-(u0*v1)u0|
    //
    // where |A| indicates length of vector A and A*B indicates dot
    // product of vectors A and B.

    // compute u0
    rkU.Normalize();

    // compute u1
    RealType fDot0 = rkU.Dot(rkV); 
    rkV -= rkU*fDot0;
    rkV.Normalize();
}
//----------------------------------------------------------------------------
template <class RealType>
void Vector2<RealType>::generateOrthonormalBasis (Vector2& rkU, Vector2& rkV,
    bool bUnitLengthV)
{
    if ( !bUnitLengthV )
        rkV.Normalize();

    rkU = rkV.Perp();
}
//----------------------------------------------------------------------------

template <class RealType>
Vector2<RealType> Vector2<RealType>::maxVector (const Vector2& rkV) const
{
	return Vector2(max(x(),rkV.x()),max(y(),rkV.y()));
}

//----------------------------------------------------------------------------
template <class RealType>
Vector2<RealType> Vector2<RealType>::minVector (const Vector2& rkV) const
{
	return Vector2(min(x(),rkV.x()),min(y(),rkV.y()));
}
//----------------------------------------------------------------------------
template <class RealType>
Vector2<RealType> Vector2<RealType>::multDiag (const Vector2& rkV) const
{
	return Vector2(x()*rkV.x(),y()*rkV.y());
}
