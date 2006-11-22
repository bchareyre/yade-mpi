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
Vector3<RealType>::Vector3 ()
{
    // uninitialized for performance in array construction
}
//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType>::Vector3 (RealType fX, RealType fY, RealType fZ)
{
    m_afTuple[0] = fX;
    m_afTuple[1] = fY;
    m_afTuple[2] = fZ;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType>::Vector3 (const Vector3& rkV)
{
    m_afTuple[0] = rkV.m_afTuple[0];
    m_afTuple[1] = rkV.m_afTuple[1];
    m_afTuple[2] = rkV.m_afTuple[2];
}
//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType>::operator const RealType* () const
{
    return m_afTuple;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType>::operator RealType* ()
{
    return m_afTuple;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector3<RealType>::operator[] (int i) const
{
    assert( 0 <= i && i <= 2 );
//    if ( i < 0 )
//        i = 0;
//    else if ( i > 2 )
//        i = 2;

    return m_afTuple[i];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType& Vector3<RealType>::operator[] (int i)
{
    assert( 0 <= i && i <= 2 );
//    if ( i < 0 )
//        i = 0;
//    else if ( i > 2 )
//        i = 2;

    return m_afTuple[i];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector3<RealType>::x () const
{
    return m_afTuple[0];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType& Vector3<RealType>::x ()
{
    return m_afTuple[0];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector3<RealType>::y () const
{
    return m_afTuple[1];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType& Vector3<RealType>::y ()
{
    return m_afTuple[1];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector3<RealType>::z () const
{
    return m_afTuple[2];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType& Vector3<RealType>::z ()
{
    return m_afTuple[2];
}
//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType>& Vector3<RealType>::operator= (const Vector3& rkV)
{
    m_afTuple[0] = rkV.m_afTuple[0];
    m_afTuple[1] = rkV.m_afTuple[1];
    m_afTuple[2] = rkV.m_afTuple[2];
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
int Vector3<RealType>::compareArrays (const Vector3& rkV) const
{
    return memcmp(m_afTuple,rkV.m_afTuple,3*sizeof(RealType));
}
//----------------------------------------------------------------------------
template <class RealType>
bool Vector3<RealType>::operator== (const Vector3& rkV) const
{
    return CompareArrays(rkV) == 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Vector3<RealType>::operator!= (const Vector3& rkV) const
{
    return CompareArrays(rkV) != 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Vector3<RealType>::operator< (const Vector3& rkV) const
{
    return CompareArrays(rkV) < 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Vector3<RealType>::operator<= (const Vector3& rkV) const
{
    return CompareArrays(rkV) <= 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Vector3<RealType>::operator> (const Vector3& rkV) const
{
    return CompareArrays(rkV) > 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Vector3<RealType>::operator>= (const Vector3& rkV) const
{
    return CompareArrays(rkV) >= 0;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType> Vector3<RealType>::operator+ (const Vector3& rkV) const
{
    return Vector3(
        m_afTuple[0]+rkV.m_afTuple[0],
        m_afTuple[1]+rkV.m_afTuple[1],
        m_afTuple[2]+rkV.m_afTuple[2]);
}
//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType> Vector3<RealType>::operator- (const Vector3& rkV) const
{
    return Vector3(
        m_afTuple[0]-rkV.m_afTuple[0],
        m_afTuple[1]-rkV.m_afTuple[1],
        m_afTuple[2]-rkV.m_afTuple[2]);
}
//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType> Vector3<RealType>::operator* (RealType fScalar) const
{
    return Vector3(
        fScalar*m_afTuple[0],
        fScalar*m_afTuple[1],
        fScalar*m_afTuple[2]);
}
//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType> Vector3<RealType>::operator/ (RealType fScalar) const
{
    Vector3 kQuot;

    if ( fScalar != (RealType)0.0 )
    {
        RealType fInvScalar = ((RealType)1.0)/fScalar;
        kQuot.m_afTuple[0] = fInvScalar*m_afTuple[0];
        kQuot.m_afTuple[1] = fInvScalar*m_afTuple[1];
        kQuot.m_afTuple[2] = fInvScalar*m_afTuple[2];
    }
    else
    {
        kQuot.m_afTuple[0] = Math<RealType>::MAX_REAL;
        kQuot.m_afTuple[1] = Math<RealType>::MAX_REAL;
        kQuot.m_afTuple[2] = Math<RealType>::MAX_REAL;
    }

    return kQuot;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType> Vector3<RealType>::operator- () const
{
    return Vector3(
        -m_afTuple[0],
        -m_afTuple[1],
        -m_afTuple[2]);
}
//----------------------------------------------------------------------------
template <class RealType , typename RealType2>
Vector3<RealType> operator* (RealType2 fScalar, const Vector3<RealType>& rkV)
{
    return Vector3<RealType>(
        fScalar*rkV[0],
        fScalar*rkV[1],
        fScalar*rkV[2]);
}

//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType>& Vector3<RealType>::operator+= (const Vector3& rkV)
{
    m_afTuple[0] += rkV.m_afTuple[0];
    m_afTuple[1] += rkV.m_afTuple[1];
    m_afTuple[2] += rkV.m_afTuple[2];
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType>& Vector3<RealType>::operator-= (const Vector3& rkV)
{
    m_afTuple[0] -= rkV.m_afTuple[0];
    m_afTuple[1] -= rkV.m_afTuple[1];
    m_afTuple[2] -= rkV.m_afTuple[2];
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType>& Vector3<RealType>::operator*= (RealType fScalar)
{
    m_afTuple[0] *= fScalar;
    m_afTuple[1] *= fScalar;
    m_afTuple[2] *= fScalar;
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType>& Vector3<RealType>::operator/= (RealType fScalar)
{
    if ( fScalar != (RealType)0.0 )
    {
        RealType fInvScalar = ((RealType)1.0)/fScalar;
        m_afTuple[0] *= fInvScalar;
        m_afTuple[1] *= fInvScalar;
        m_afTuple[2] *= fInvScalar;
    }
    else
    {
        m_afTuple[0] = Math<RealType>::MAX_REAL;
        m_afTuple[1] = Math<RealType>::MAX_REAL;
        m_afTuple[2] = Math<RealType>::MAX_REAL;
    }

    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector3<RealType>::length () const
{
    return Math<RealType>::sqRoot(
        m_afTuple[0]*m_afTuple[0] +
        m_afTuple[1]*m_afTuple[1] +
        m_afTuple[2]*m_afTuple[2]);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector3<RealType>::squaredLength () const
{
    return
        m_afTuple[0]*m_afTuple[0] +
        m_afTuple[1]*m_afTuple[1] +
        m_afTuple[2]*m_afTuple[2];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector3<RealType>::dot (const Vector3& rkV) const
{
    return
        m_afTuple[0]*rkV.m_afTuple[0] +
        m_afTuple[1]*rkV.m_afTuple[1] +
        m_afTuple[2]*rkV.m_afTuple[2];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector3<RealType>::angleBetweenUnitVectors(const Vector3& rkV) const
{
    return Mathr::aCos( this->dot(rkV) );
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector3<RealType>::normalize ()
{
    RealType fLength = length();

    if ( fLength > Math<RealType>::ZERO_TOLERANCE )
    {
        RealType fInvLength = ((RealType)1.0)/fLength;
        m_afTuple[0] *= fInvLength;
        m_afTuple[1] *= fInvLength;
        m_afTuple[2] *= fInvLength;
    }
    else
    {
        fLength = (RealType)0.0;
        m_afTuple[0] = (RealType)0.0;
        m_afTuple[1] = (RealType)0.0;
        m_afTuple[2] = (RealType)0.0;
    }

    return fLength;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType> Vector3<RealType>::maxVector (const Vector3& rkV) const
{
	return Vector3(max(x(),rkV.x()),max(y(),rkV.y()),max(z(),rkV.z()));
}

//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType> Vector3<RealType>::minVector (const Vector3& rkV) const
{
	return Vector3(min(x(),rkV.x()),min(y(),rkV.y()),min(z(),rkV.z()));
}

//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType> Vector3<RealType>::cross (const Vector3& rkV) const
{
    return Vector3(
        m_afTuple[1]*rkV.m_afTuple[2] - m_afTuple[2]*rkV.m_afTuple[1],
        m_afTuple[2]*rkV.m_afTuple[0] - m_afTuple[0]*rkV.m_afTuple[2],
        m_afTuple[0]*rkV.m_afTuple[1] - m_afTuple[1]*rkV.m_afTuple[0]);
}
//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType> Vector3<RealType>::unitCross (const Vector3& rkV) const
{
    Vector3 kCross(
        m_afTuple[1]*rkV.m_afTuple[2] - m_afTuple[2]*rkV.m_afTuple[1],
        m_afTuple[2]*rkV.m_afTuple[0] - m_afTuple[0]*rkV.m_afTuple[2],
        m_afTuple[0]*rkV.m_afTuple[1] - m_afTuple[1]*rkV.m_afTuple[0]);
    kCross.normalize();
    return kCross;
}
//----------------------------------------------------------------------------
template <class RealType>
void Vector3<RealType>::getBarycentrics (const Vector3<RealType>& rkV0,
    const Vector3<RealType>& rkV1, const Vector3<RealType>& rkV2,
    const Vector3<RealType>& rkV3, RealType afBary[4]) const
{
    // compute the vectors relative to V3 of the tetrahedron
    Vector3<RealType> akDiff[4] =
    {
        rkV0 - rkV3,
        rkV1 - rkV3,
        rkV2 - rkV3,
        *this - rkV3
    };

    // If the vertices have large magnitude, the linear system of
    // equations for computing barycentric coordinates can be
    // ill-conditioned.  To avoid this, uniformly scale the tetrahedron
    // edges to be of order 1.  The scaling of all differences does not
    // change the barycentric coordinates.
    RealType fMax = (RealType)0.0;
    int i;
    for (i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
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
        for (i = 0; i < 4; i++)
            akDiff[i] *= fInvMax;
    }

    RealType fDet = akDiff[0].Dot(akDiff[1].Cross(akDiff[2]));
    Vector3<RealType> kE1cE2 = akDiff[1].Cross(akDiff[2]);
    Vector3<RealType> kE2cE0 = akDiff[2].Cross(akDiff[0]);
    Vector3<RealType> kE0cE1 = akDiff[0].Cross(akDiff[1]);
    if ( Math<RealType>::fAbs(fDet) > Math<RealType>::ZERO_TOLERANCE )
    {
        RealType fInvDet = ((RealType)1.0)/fDet;
        afBary[0] = akDiff[3].Dot(kE1cE2)*fInvDet;
        afBary[1] = akDiff[3].Dot(kE2cE0)*fInvDet;
        afBary[2] = akDiff[3].Dot(kE0cE1)*fInvDet;
        afBary[3] = (RealType)1.0 - afBary[0] - afBary[1] - afBary[2];
    }
    else
    {
        // The tetrahedron is potentially flat.  Determine the face of
        // maximum area and compute barycentric coordinates with respect
        // to that face.
        Vector3<RealType> kE02 = rkV0 - rkV2;
        Vector3<RealType> kE12 = rkV1 - rkV2;
        Vector3<RealType> kE02cE12 = kE02.Cross(kE12);
        RealType fMaxSqrArea = kE02cE12.SquaredLength();
        int iMaxIndex = 3;
        RealType fSqrArea = kE0cE1.SquaredLength();
        if ( fSqrArea > fMaxSqrArea )
        {
            iMaxIndex = 0;
            fMaxSqrArea = fSqrArea;
        }
        fSqrArea = kE1cE2.SquaredLength();
        if ( fSqrArea > fMaxSqrArea )
        {
            iMaxIndex = 1;
            fMaxSqrArea = fSqrArea;
        }
        fSqrArea = kE2cE0.SquaredLength();
        if ( fSqrArea > fMaxSqrArea )
        {
            iMaxIndex = 2;
            fMaxSqrArea = fSqrArea;
        }

        if ( fMaxSqrArea > Math<RealType>::ZERO_TOLERANCE )
        {
            RealType fInvSqrArea = ((RealType)1.0)/fMaxSqrArea;
            Vector3<RealType> kTmp;
            if ( iMaxIndex == 0 )
            {
                kTmp = akDiff[3].Cross(akDiff[1]);
                afBary[0] = kE0cE1.Dot(kTmp)*fInvSqrArea;
                kTmp = akDiff[0].Cross(akDiff[3]);
                afBary[1] = kE0cE1.Dot(kTmp)*fInvSqrArea;
                afBary[2] = (RealType)0.0;
                afBary[3] = (RealType)1.0 - afBary[0] - afBary[1];
            }
            else if ( iMaxIndex == 1 )
            {
                afBary[0] = (RealType)0.0;
                kTmp = akDiff[3].Cross(akDiff[2]);
                afBary[1] = kE1cE2.Dot(kTmp)*fInvSqrArea;
                kTmp = akDiff[1].Cross(akDiff[3]);
                afBary[2] = kE1cE2.Dot(kTmp)*fInvSqrArea;
                afBary[3] = (RealType)1.0 - afBary[1] - afBary[2];
            }
            else if ( iMaxIndex == 2 )
            {
                kTmp = akDiff[2].Cross(akDiff[3]);
                afBary[0] = kE2cE0.Dot(kTmp)*fInvSqrArea;
                afBary[1] = (RealType)0.0;
                kTmp = akDiff[3].Cross(akDiff[0]);
                afBary[2] = kE2cE0.Dot(kTmp)*fInvSqrArea;
                afBary[3] = (RealType)1.0 - afBary[0] - afBary[2];
            }
            else
            {
                akDiff[3] = *this - rkV2;
                kTmp = akDiff[3].Cross(kE12);
                afBary[0] = kE02cE12.Dot(kTmp)*fInvSqrArea;
                kTmp = kE02.Cross(akDiff[3]);
                afBary[1] = kE02cE12.Dot(kTmp)*fInvSqrArea;
                afBary[2] = (RealType)1.0 - afBary[0] - afBary[1];
                afBary[3] = (RealType)0.0;
            }
        }
        else
        {
            // The tetrahedron is potentially a sliver.  Determine the edge of
            // maximum length and compute barycentric coordinates with respect
            // to that edge.
            RealType fMaxSqrLength = akDiff[0].SquaredLength();
            iMaxIndex = 0;  // <V0,V3>
            RealType fSqrLength = akDiff[1].SquaredLength();
            if ( fSqrLength > fMaxSqrLength )
            {
                iMaxIndex = 1;  // <V1,V3>
                fMaxSqrLength = fSqrLength;
            }
            fSqrLength = akDiff[2].SquaredLength();
            if ( fSqrLength > fMaxSqrLength )
            {
                iMaxIndex = 2;  // <V2,V3>
                fMaxSqrLength = fSqrLength;
            }
            fSqrLength = kE02.SquaredLength();
            if ( fSqrLength > fMaxSqrLength )
            {
                iMaxIndex = 3;  // <V0,V2>
                fMaxSqrLength = fSqrLength;
            }
            fSqrLength = kE12.SquaredLength();
            if ( fSqrLength > fMaxSqrLength )
            {
                iMaxIndex = 4;  // <V1,V2>
                fMaxSqrLength = fSqrLength;
            }
            Vector3<RealType> kE01 = rkV0 - rkV1;
            fSqrLength = kE01.SquaredLength();
            if ( fSqrLength > fMaxSqrLength )
            {
                iMaxIndex = 5;  // <V0,V1>
                fMaxSqrLength = fSqrLength;
            }

            if ( fMaxSqrLength > Math<RealType>::ZERO_TOLERANCE )
            {
                RealType fInvSqrLength = ((RealType)1.0)/fMaxSqrLength;
                if ( iMaxIndex == 0 )
                {
                    // P-V3 = t*(V0-V3)
                    afBary[0] = akDiff[3].Dot(akDiff[0])*fInvSqrLength;
                    afBary[1] = (RealType)0.0;
                    afBary[2] = (RealType)0.0;
                    afBary[3] = (RealType)1.0 - afBary[0];
                }
                else if ( iMaxIndex == 1 )
                {
                    // P-V3 = t*(V1-V3)
                    afBary[0] = (RealType)0.0;
                    afBary[1] = akDiff[3].Dot(akDiff[1])*fInvSqrLength;
                    afBary[2] = (RealType)0.0;
                    afBary[3] = (RealType)1.0 - afBary[1];
                }
                else if ( iMaxIndex == 2 )
                {
                    // P-V3 = t*(V2-V3)
                    afBary[0] = (RealType)0.0;
                    afBary[1] = (RealType)0.0;
                    afBary[2] = akDiff[3].Dot(akDiff[2])*fInvSqrLength;
                    afBary[3] = (RealType)1.0 - afBary[2];
                }
                else if ( iMaxIndex == 3 )
                {
                    // P-V2 = t*(V0-V2)
                    akDiff[3] = *this - rkV2;
                    afBary[0] = akDiff[3].Dot(kE02)*fInvSqrLength;
                    afBary[1] = (RealType)0.0;
                    afBary[2] = (RealType)1.0 - afBary[0];
                    afBary[3] = (RealType)0.0;
                }
                else if ( iMaxIndex == 4 )
                {
                    // P-V2 = t*(V1-V2)
                    akDiff[3] = *this - rkV2;
                    afBary[0] = (RealType)0.0;
                    afBary[1] = akDiff[3].Dot(kE12)*fInvSqrLength;
                    afBary[2] = (RealType)1.0 - afBary[1];
                    afBary[3] = (RealType)0.0;
                }
                else
                {
                    // P-V1 = t*(V0-V1)
                    akDiff[3] = *this - rkV1;
                    afBary[0] = akDiff[3].Dot(kE01)*fInvSqrLength;
                    afBary[1] = (RealType)1.0 - afBary[0];
                    afBary[2] = (RealType)0.0;
                    afBary[3] = (RealType)0.0;
                }
            }
            else
            {
                // tetrahedron is a nearly a point, just return equal weights
                afBary[0] = (RealType)0.25;
                afBary[1] = afBary[0];
                afBary[2] = afBary[0];
                afBary[3] = afBary[0];
            }
        }
    }
}
//----------------------------------------------------------------------------
template <class RealType>
void Vector3<RealType>::orthonormalize (Vector3& rkU, Vector3& rkV, Vector3& rkW)
{
    // If the input vectors are v0, v1, and v2, then the Gram-Schmidt
    // orthonormalization produces vectors u0, u1, and u2 as follows,
    //
    //   u0 = v0/|v0|
    //   u1 = (v1-(u0*v1)u0)/|v1-(u0*v1)u0|
    //   u2 = (v2-(u0*v2)u0-(u1*v2)u1)/|v2-(u0*v2)u0-(u1*v2)u1|
    //
    // where |A| indicates length of vector A and A*B indicates dot
    // product of vectors A and B.

    // compute u0
    rkU.normalize();

    // compute u1
    RealType fDot0 = rkU.Dot(rkV); 
    rkV -= fDot0*rkU;
    rkV.normalize();

    // compute u2
    RealType fDot1 = rkV.Dot(rkW);
    fDot0 = rkU.Dot(rkW);
    rkW -= fDot0*rkU + fDot1*rkV;
    rkW.normalize();
}
//----------------------------------------------------------------------------
template <class RealType>
void Vector3<RealType>::orthonormalize (Vector3* akV)
{
    Orthonormalize(akV[0],akV[1],akV[2]);
}
//----------------------------------------------------------------------------
template <class RealType>
void Vector3<RealType>::generateOrthonormalBasis (Vector3& rkU, Vector3& rkV,
    Vector3& rkW, bool bUnitLengthW)
{
    if ( !bUnitLengthW )
        rkW.normalize();

    RealType fInvLength;

    if ( Math<RealType>::fAbs(rkW.m_afTuple[0]) >=
         Math<RealType>::fAbs(rkW.m_afTuple[1]) )
    {
        // W.x or W.z is the largest magnitude component, swap them
        fInvLength = Math<RealType>::InvSqrt(rkW.m_afTuple[0]*rkW.m_afTuple[0] +
            rkW.m_afTuple[2]*rkW.m_afTuple[2]);
        rkU.m_afTuple[0] = -rkW.m_afTuple[2]*fInvLength;
        rkU.m_afTuple[1] = (RealType)0.0;
        rkU.m_afTuple[2] = +rkW.m_afTuple[0]*fInvLength;
    }
    else
    {
        // W.y or W.z is the largest magnitude component, swap them
        fInvLength = Math<RealType>::InvSqrt(rkW.m_afTuple[1]*rkW.m_afTuple[1] +
            rkW.m_afTuple[2]*rkW.m_afTuple[2]);
        rkU.m_afTuple[0] = (RealType)0.0;
        rkU.m_afTuple[1] = +rkW.m_afTuple[2]*fInvLength;
        rkU.m_afTuple[2] = -rkW.m_afTuple[1]*fInvLength;
    }

    rkV = rkW.Cross(rkU);
}
//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType> Vector3<RealType>::multDiag (const Vector3& rkV) const
{
	return Vector3(x()*rkV.x(),y()*rkV.y(),z()*rkV.z());
}
//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType> Vector3<RealType>::divDiag (const Vector3& rkV) const
{
	return Vector3(x()/rkV.x(),y()/rkV.y(),z()/rkV.z());
}


