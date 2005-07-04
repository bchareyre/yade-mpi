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
Vector4<RealType>::Vector4 ()
{
    // uninitialized for performance in array construction
}
//----------------------------------------------------------------------------
template <class RealType>
Vector4<RealType>::Vector4 (RealType fX, RealType fY, RealType fZ, RealType fW)
{
    m_afTuple[0] = fX;
    m_afTuple[1] = fY;
    m_afTuple[2] = fZ;
    m_afTuple[3] = fW;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector4<RealType>::Vector4 (const Vector4& rkV)
{
    m_afTuple[0] = rkV.m_afTuple[0];
    m_afTuple[1] = rkV.m_afTuple[1];
    m_afTuple[2] = rkV.m_afTuple[2];
    m_afTuple[3] = rkV.m_afTuple[3];
}
//----------------------------------------------------------------------------
template <class RealType>
Vector4<RealType>::operator const RealType* () const
{
    return m_afTuple;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector4<RealType>::operator RealType* ()
{
    return m_afTuple;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector4<RealType>::operator[] (int i) const
{
    assert( 0 <= i && i <= 3 );
    if ( i < 0 )
        i = 0;
    else if ( i > 3 )
        i = 3;

    return m_afTuple[i];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType& Vector4<RealType>::operator[] (int i)
{
    assert( 0 <= i && i <= 3 );
    if ( i < 0 )
        i = 0;
    else if ( i > 3 )
        i = 3;

    return m_afTuple[i];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector4<RealType>::x () const
{
    return m_afTuple[0];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType& Vector4<RealType>::x ()
{
    return m_afTuple[0];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector4<RealType>::y () const
{
    return m_afTuple[1];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType& Vector4<RealType>::y ()
{
    return m_afTuple[1];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector4<RealType>::z () const
{
    return m_afTuple[2];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType& Vector4<RealType>::z ()
{
    return m_afTuple[2];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector4<RealType>::w () const
{
    return m_afTuple[3];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType& Vector4<RealType>::w ()
{
    return m_afTuple[3];
}
//----------------------------------------------------------------------------
template <class RealType>
Vector4<RealType>& Vector4<RealType>::operator= (const Vector4& rkV)
{
    m_afTuple[0] = rkV.m_afTuple[0];
    m_afTuple[1] = rkV.m_afTuple[1];
    m_afTuple[2] = rkV.m_afTuple[2];
    m_afTuple[3] = rkV.m_afTuple[3];
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
int Vector4<RealType>::compareArrays (const Vector4& rkV) const
{
    return memcmp(m_afTuple,rkV.m_afTuple,4*sizeof(RealType));
}
//----------------------------------------------------------------------------
template <class RealType>
bool Vector4<RealType>::operator== (const Vector4& rkV) const
{
    return CompareArrays(rkV) == 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Vector4<RealType>::operator!= (const Vector4& rkV) const
{
    return CompareArrays(rkV) != 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Vector4<RealType>::operator< (const Vector4& rkV) const
{
    return CompareArrays(rkV) < 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Vector4<RealType>::operator<= (const Vector4& rkV) const
{
    return CompareArrays(rkV) <= 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Vector4<RealType>::operator> (const Vector4& rkV) const
{
    return CompareArrays(rkV) > 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Vector4<RealType>::operator>= (const Vector4& rkV) const
{
    return CompareArrays(rkV) >= 0;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector4<RealType> Vector4<RealType>::operator+ (const Vector4& rkV) const
{
    return Vector4(
        m_afTuple[0]+rkV.m_afTuple[0],
        m_afTuple[1]+rkV.m_afTuple[1],
        m_afTuple[2]+rkV.m_afTuple[2],
        m_afTuple[3]+rkV.m_afTuple[3]);
}
//----------------------------------------------------------------------------
template <class RealType>
Vector4<RealType> Vector4<RealType>::operator- (const Vector4& rkV) const
{
    return Vector4(
        m_afTuple[0]-rkV.m_afTuple[0],
        m_afTuple[1]-rkV.m_afTuple[1],
        m_afTuple[2]-rkV.m_afTuple[2],
        m_afTuple[3]-rkV.m_afTuple[3]);
}
//----------------------------------------------------------------------------
template <class RealType>
Vector4<RealType> Vector4<RealType>::operator* (RealType fScalar) const
{
    return Vector4(
        fScalar*m_afTuple[0],
        fScalar*m_afTuple[1],
        fScalar*m_afTuple[2],
        fScalar*m_afTuple[3]);
}
//----------------------------------------------------------------------------
template <class RealType>
Vector4<RealType> Vector4<RealType>::operator/ (RealType fScalar) const
{
    Vector4 kQuot;

    if ( fScalar != (RealType)0.0 )
    {
        RealType fInvScalar = ((RealType)1.0)/fScalar;
        kQuot.m_afTuple[0] = fInvScalar*m_afTuple[0];
        kQuot.m_afTuple[1] = fInvScalar*m_afTuple[1];
        kQuot.m_afTuple[2] = fInvScalar*m_afTuple[2];
        kQuot.m_afTuple[3] = fInvScalar*m_afTuple[3];
    }
    else
    {
        kQuot.m_afTuple[0] = Math<RealType>::MAX_REAL;
        kQuot.m_afTuple[1] = Math<RealType>::MAX_REAL;
        kQuot.m_afTuple[2] = Math<RealType>::MAX_REAL;
        kQuot.m_afTuple[3] = Math<RealType>::MAX_REAL;
    }

    return kQuot;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector4<RealType> Vector4<RealType>::operator- () const
{
    return Vector4(
        -m_afTuple[0],
        -m_afTuple[1],
        -m_afTuple[2],
        -m_afTuple[3]);
}
//----------------------------------------------------------------------------
template <class RealType>
Vector4<RealType> operator* (RealType fScalar, const Vector4<RealType>& rkV)
{
    return Vector4<RealType>(
        fScalar*rkV[0],
        fScalar*rkV[1],
        fScalar*rkV[2],
        fScalar*rkV[3]);
}
//----------------------------------------------------------------------------
template <class RealType>
Vector4<RealType>& Vector4<RealType>::operator+= (const Vector4& rkV)
{
    m_afTuple[0] += rkV.m_afTuple[0];
    m_afTuple[1] += rkV.m_afTuple[1];
    m_afTuple[2] += rkV.m_afTuple[2];
    m_afTuple[3] += rkV.m_afTuple[3];
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector4<RealType>& Vector4<RealType>::operator-= (const Vector4& rkV)
{
    m_afTuple[0] -= rkV.m_afTuple[0];
    m_afTuple[1] -= rkV.m_afTuple[1];
    m_afTuple[2] -= rkV.m_afTuple[2];
    m_afTuple[3] -= rkV.m_afTuple[3];
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector4<RealType>& Vector4<RealType>::operator*= (RealType fScalar)
{
    m_afTuple[0] *= fScalar;
    m_afTuple[1] *= fScalar;
    m_afTuple[2] *= fScalar;
    m_afTuple[3] *= fScalar;
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector4<RealType>& Vector4<RealType>::operator/= (RealType fScalar)
{
    if ( fScalar != (RealType)0.0 )
    {
        RealType fInvScalar = ((RealType)1.0)/fScalar;
        m_afTuple[0] *= fInvScalar;
        m_afTuple[1] *= fInvScalar;
        m_afTuple[2] *= fInvScalar;
        m_afTuple[3] *= fInvScalar;
    }
    else
    {
        m_afTuple[0] = Math<RealType>::MAX_REAL;
        m_afTuple[1] = Math<RealType>::MAX_REAL;
        m_afTuple[2] = Math<RealType>::MAX_REAL;
        m_afTuple[3] = Math<RealType>::MAX_REAL;
    }

    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector4<RealType>::length () const
{
    return Math<RealType>::Sqrt(
        m_afTuple[0]*m_afTuple[0] +
        m_afTuple[1]*m_afTuple[1] +
        m_afTuple[2]*m_afTuple[2] +
        m_afTuple[3]*m_afTuple[3]);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector4<RealType>::squaredLength () const
{
    return
        m_afTuple[0]*m_afTuple[0] +
        m_afTuple[1]*m_afTuple[1] +
        m_afTuple[2]*m_afTuple[2] +
        m_afTuple[3]*m_afTuple[3];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector4<RealType>::dot (const Vector4& rkV) const
{
    return
        m_afTuple[0]*rkV.m_afTuple[0] +
        m_afTuple[1]*rkV.m_afTuple[1] +
        m_afTuple[2]*rkV.m_afTuple[2] +
        m_afTuple[3]*rkV.m_afTuple[3];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Vector4<RealType>::normalize ()
{
    RealType fLength = length();

    if ( fLength > Math<RealType>::ZERO_TOLERANCE )
    {
        RealType fInvLength = ((RealType)1.0)/fLength;
        m_afTuple[0] *= fInvLength;
        m_afTuple[1] *= fInvLength;
        m_afTuple[2] *= fInvLength;
        m_afTuple[3] *= fInvLength;
    }
    else
    {
        fLength = (RealType)0.0;
        m_afTuple[0] = (RealType)0.0;
        m_afTuple[1] = (RealType)0.0;
        m_afTuple[2] = (RealType)0.0;
        m_afTuple[3] = (RealType)0.0;
    }

    return fLength;
}
//----------------------------------------------------------------------------

template <class RealType>
Vector4<RealType> Vector4<RealType>::maxVector (const Vector4& rkV) const
{
	return Vector4(max(x(),rkV.x()),max(y(),rkV.y()),max(z(),rkV.z()),max(w(),rkV.w()));
}

//----------------------------------------------------------------------------
template <class RealType>
Vector4<RealType> Vector4<RealType>::minVector (const Vector4& rkV) const
{
	return Vector4(min(x(),rkV.x()),min(y(),rkV.y()),min(z(),rkV.z()),min(w(),rkV.w()));
}
//----------------------------------------------------------------------------
template <class RealType>
Vector4<RealType> Vector4<RealType>::multDiag (const Vector4& rkV) const
{
	return Vector4(x()*rkV.x(),y()*rkV.y(),z()*rkV.z(),w()*rkV.w());
}
