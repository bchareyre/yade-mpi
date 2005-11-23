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
Quaternion<RealType>::Quaternion ()
{
    // uninitialized for performance in array construction
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType>::Quaternion (RealType fW, RealType fX, RealType fY, RealType fZ)
{
    m_afTuple[0] = fW;
    m_afTuple[1] = fX;
    m_afTuple[2] = fY;
    m_afTuple[3] = fZ;
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType>::Quaternion (const Quaternion& rkQ)
{
    memcpy(m_afTuple,rkQ.m_afTuple,4*sizeof(RealType));
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType>::Quaternion (const Matrix3<RealType>& rkRot)
{
    fromRotationMatrix(rkRot);
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType>::Quaternion (const Vector3<RealType>& rkAxis, RealType fAngle)
{
    fromAxisAngle(rkAxis,fAngle);
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType>::Quaternion (const Vector3<RealType> akRotColumn[3])
{
    fromRotationMatrix(akRotColumn);
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType>::operator const RealType* () const
{
    return m_afTuple;
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType>::operator RealType* ()
{
    return m_afTuple;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Quaternion<RealType>::operator[] (int i) const
{
    assert( 0 <= i && i <= 3 );
//    if ( i < 0 )
//        i = 0;
//    else if ( i > 3 )
//        i = 3;

    return m_afTuple[i];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType& Quaternion<RealType>::operator[] (int i)
{
    assert( 0 <= i && i <= 3 );
//   if ( i < 0 )
//        i = 0;
//    else if ( i > 3 )
//        i = 3;

    return m_afTuple[i];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Quaternion<RealType>::w () const
{
    return m_afTuple[0];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType& Quaternion<RealType>::w ()
{
    return m_afTuple[0];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Quaternion<RealType>::x () const
{
    return m_afTuple[1];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType& Quaternion<RealType>::x ()
{
    return m_afTuple[1];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Quaternion<RealType>::y () const
{
    return m_afTuple[2];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType& Quaternion<RealType>::y ()
{
    return m_afTuple[2];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Quaternion<RealType>::z () const
{
    return m_afTuple[3];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType& Quaternion<RealType>::z ()
{
    return m_afTuple[3];
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType>& Quaternion<RealType>::operator= (const Quaternion& rkQ)
{
    memcpy(m_afTuple,rkQ.m_afTuple,4*sizeof(RealType));
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
int Quaternion<RealType>::compareArrays (const Quaternion& rkQ) const
{
    return memcmp(m_afTuple,rkQ.m_afTuple,4*sizeof(RealType));
}
//----------------------------------------------------------------------------
template <class RealType>
bool Quaternion<RealType>::operator== (const Quaternion& rkQ) const
{
    return compareArrays(rkQ) == 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Quaternion<RealType>::operator!= (const Quaternion& rkQ) const
{
    return compareArrays(rkQ) != 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Quaternion<RealType>::operator< (const Quaternion& rkQ) const
{
    return compareArrays(rkQ) < 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Quaternion<RealType>::operator<= (const Quaternion& rkQ) const
{
    return compareArrays(rkQ) <= 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Quaternion<RealType>::operator> (const Quaternion& rkQ) const
{
    return compareArrays(rkQ) > 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Quaternion<RealType>::operator>= (const Quaternion& rkQ) const
{
    return compareArrays(rkQ) >= 0;
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType> Quaternion<RealType>::operator+ (const Quaternion& rkQ) const
{
    Quaternion kSum;
    for (int i = 0; i < 4; i++)
        kSum.m_afTuple[i] = m_afTuple[i] + rkQ.m_afTuple[i];
    return kSum;
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType> Quaternion<RealType>::operator- (const Quaternion& rkQ) const
{
    Quaternion kDiff;
    for (int i = 0; i < 4; i++)
        kDiff.m_afTuple[i] = m_afTuple[i] - rkQ.m_afTuple[i];
    return kDiff;
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType> Quaternion<RealType>::operator* (const Quaternion& rkQ) const
{
    // NOTE:  Multiplication is not generally commutative, so in most
    // cases p*q != q*p.

    Quaternion kProd;

    kProd.m_afTuple[0] =
        m_afTuple[0]*rkQ.m_afTuple[0] -
        m_afTuple[1]*rkQ.m_afTuple[1] -
        m_afTuple[2]*rkQ.m_afTuple[2] -
        m_afTuple[3]*rkQ.m_afTuple[3];

    kProd.m_afTuple[1] =
        m_afTuple[0]*rkQ.m_afTuple[1] +
        m_afTuple[1]*rkQ.m_afTuple[0] +
        m_afTuple[2]*rkQ.m_afTuple[3] -
        m_afTuple[3]*rkQ.m_afTuple[2];

    kProd.m_afTuple[2] =
        m_afTuple[0]*rkQ.m_afTuple[2] +
        m_afTuple[2]*rkQ.m_afTuple[0] +
        m_afTuple[3]*rkQ.m_afTuple[1] -
        m_afTuple[1]*rkQ.m_afTuple[3];

    kProd.m_afTuple[3] =
        m_afTuple[0]*rkQ.m_afTuple[3] +
        m_afTuple[3]*rkQ.m_afTuple[0] +
        m_afTuple[1]*rkQ.m_afTuple[2] -
        m_afTuple[2]*rkQ.m_afTuple[1];

    return kProd;
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType> Quaternion<RealType>::operator* (RealType fScalar) const
{
    Quaternion kProd;
    for (int i = 0; i < 4; i++)
        kProd.m_afTuple[i] = fScalar*m_afTuple[i];
    return kProd;
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType> Quaternion<RealType>::operator/ (RealType fScalar) const
{
    Quaternion kQuot;
    int i;

    if ( fScalar != (RealType)0.0 )
    {
        RealType fInvScalar = ((RealType)1.0)/fScalar;
        for (i = 0; i < 4; i++)
            kQuot.m_afTuple[i] = fInvScalar*m_afTuple[i];
    }
    else
    {
        for (i = 0; i < 4; i++)
            kQuot.m_afTuple[i] = Math<RealType>::MAX_REAL;
    }

    return kQuot;
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType> Quaternion<RealType>::operator- () const
{
    Quaternion kNeg;
    for (int i = 0; i < 4; i++)
        kNeg.m_afTuple[i] = -m_afTuple[i];
    return kNeg;
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType> operator* (RealType fScalar, const Quaternion<RealType>& rkQ)
{
    Quaternion<RealType> kProd;
    for (int i = 0; i < 4; i++)
        kProd[i] = fScalar*rkQ[i];
    return kProd;
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType>& Quaternion<RealType>::operator+= (const Quaternion& rkQ)
{
    for (int i = 0; i < 4; i++)
        m_afTuple[i] += rkQ.m_afTuple[i];
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType>& Quaternion<RealType>::operator-= (const Quaternion& rkQ)
{
    for (int i = 0; i < 4; i++)
        m_afTuple[i] -= rkQ.m_afTuple[i];
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType>& Quaternion<RealType>::operator*= (RealType fScalar)
{
    for (int i = 0; i < 4; i++)
        m_afTuple[i] *= fScalar;
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType>& Quaternion<RealType>::operator/= (RealType fScalar)
{
    int i;

    if ( fScalar != (RealType)0.0 )
    {
        RealType fInvScalar = ((RealType)1.0)/fScalar;
        for (i = 0; i < 4; i++)
            m_afTuple[i] *= fInvScalar;
    }
    else
    {
        for (i = 0; i < 4; i++)
            m_afTuple[i] = Math<RealType>::MAX_REAL;
    }

    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType>& Quaternion<RealType>::fromRotationMatrix (
    const Matrix3<RealType>& rkRot)
{
    // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    // article "Quaternion Calculus and Fast Animation".

    RealType fTrace = rkRot(0,0) + rkRot(1,1) + rkRot(2,2);
    RealType fRoot;

    if ( fTrace > (RealType)0.0 )
    {
        // |w| > 1/2, may as well choose w > 1/2
        fRoot = Math<RealType>::sqRoot(fTrace + (RealType)1.0);  // 2w
        m_afTuple[0] = ((RealType)0.5)*fRoot;
        fRoot = ((RealType)0.5)/fRoot;  // 1/(4w)
        m_afTuple[1] = (rkRot(2,1)-rkRot(1,2))*fRoot;
        m_afTuple[2] = (rkRot(0,2)-rkRot(2,0))*fRoot;
        m_afTuple[3] = (rkRot(1,0)-rkRot(0,1))*fRoot;
    }
    else
    {
        // |w| <= 1/2
        int i = 0;
        if ( rkRot(1,1) > rkRot(0,0) )
            i = 1;
        if ( rkRot(2,2) > rkRot(i,i) )
            i = 2;
        int j = ms_iNext[i];
        int k = ms_iNext[j];

        fRoot = Math<RealType>::sqRoot(rkRot(i,i)-rkRot(j,j)-rkRot(k,k)+(RealType)1.0);
        RealType* apfQuat[3] = { &m_afTuple[1], &m_afTuple[2], &m_afTuple[3] };
        *apfQuat[i] = ((RealType)0.5)*fRoot;
        fRoot = ((RealType)0.5)/fRoot;
        m_afTuple[0] = (rkRot(k,j)-rkRot(j,k))*fRoot;
        *apfQuat[j] = (rkRot(j,i)+rkRot(i,j))*fRoot;
        *apfQuat[k] = (rkRot(k,i)+rkRot(i,k))*fRoot;
    }

    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
void Quaternion<RealType>::toRotationMatrix (Matrix3<RealType>& rkRot) const
{
    RealType fTx  = ((RealType)2.0)*m_afTuple[1];
    RealType fTy  = ((RealType)2.0)*m_afTuple[2];
    RealType fTz  = ((RealType)2.0)*m_afTuple[3];
    RealType fTwx = fTx*m_afTuple[0];
    RealType fTwy = fTy*m_afTuple[0];
    RealType fTwz = fTz*m_afTuple[0];
    RealType fTxx = fTx*m_afTuple[1];
    RealType fTxy = fTy*m_afTuple[1];
    RealType fTxz = fTz*m_afTuple[1];
    RealType fTyy = fTy*m_afTuple[2];
    RealType fTyz = fTz*m_afTuple[2];
    RealType fTzz = fTz*m_afTuple[3];

    rkRot(0,0) = (RealType)1.0-(fTyy+fTzz);
    rkRot(0,1) = fTxy-fTwz;
    rkRot(0,2) = fTxz+fTwy;
    rkRot(1,0) = fTxy+fTwz;
    rkRot(1,1) = (RealType)1.0-(fTxx+fTzz);
    rkRot(1,2) = fTyz-fTwx;
    rkRot(2,0) = fTxz-fTwy;
    rkRot(2,1) = fTyz+fTwx;
    rkRot(2,2) = (RealType)1.0-(fTxx+fTyy);
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType>& Quaternion<RealType>::fromRotationMatrix (
    const Vector3<RealType> akRotColumn[3])
{
    Matrix3<RealType> kRot;
    for (int iCol = 0; iCol < 3; iCol++)
    {
        kRot(0,iCol) = akRotColumn[iCol][0];
        kRot(1,iCol) = akRotColumn[iCol][1];
        kRot(2,iCol) = akRotColumn[iCol][2];
    }
    return fromRotationMatrix(kRot);
}
//----------------------------------------------------------------------------
template <class RealType>
void Quaternion<RealType>::toRotationMatrix (Vector3<RealType> akRotColumn[3]) const
{
    Matrix3<RealType> kRot;
    toRotationMatrix(kRot);
    for (int iCol = 0; iCol < 3; iCol++)
    {
        akRotColumn[iCol][0] = kRot(0,iCol);
        akRotColumn[iCol][1] = kRot(1,iCol);
        akRotColumn[iCol][2] = kRot(2,iCol);
    }
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType>& Quaternion<RealType>::fromAxisAngle (
    const Vector3<RealType>& rkAxis, RealType fAngle)
{
    // assert:  axis[] is unit length
    //
    // The quaternion representing the orientation is
    //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

    RealType fHalfAngle = ((RealType)0.5)*fAngle;
    RealType fSin = Math<RealType>::sinus(fHalfAngle);
    m_afTuple[0] = Math<RealType>::cosinus(fHalfAngle);
    m_afTuple[1] = fSin*rkAxis[0];
    m_afTuple[2] = fSin*rkAxis[1];
    m_afTuple[3] = fSin*rkAxis[2];

    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
void Quaternion<RealType>::toAxisAngle (Vector3<RealType>& rkAxis, RealType& rfAngle)
    const
{
    // The quaternion representing the orientation is
    //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

    RealType fSqrLength = m_afTuple[1]*m_afTuple[1] + m_afTuple[2]*m_afTuple[2]
        + m_afTuple[3]*m_afTuple[3];
    if ( fSqrLength > Math<RealType>::ZERO_TOLERANCE )
    {
        rfAngle = ((RealType)2.0)*Math<RealType>::aCos(m_afTuple[0]);
        RealType fInvLength = Math<RealType>::invSqRoot(fSqrLength);
        rkAxis[0] = m_afTuple[1]*fInvLength;
        rkAxis[1] = m_afTuple[2]*fInvLength;
        rkAxis[2] = m_afTuple[3]*fInvLength;
    }
    else
    {
        // angle is 0 (mod 2*pi), so any axis will do
        rfAngle = (RealType)0.0;
        rkAxis[0] = (RealType)1.0;
        rkAxis[1] = (RealType)0.0;
        rkAxis[2] = (RealType)0.0;
    }
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Quaternion<RealType>::length () const
{
    return Math<RealType>::sqRoot(
        m_afTuple[0]*m_afTuple[0] +
        m_afTuple[1]*m_afTuple[1] +
        m_afTuple[2]*m_afTuple[2] +
        m_afTuple[3]*m_afTuple[3]);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Quaternion<RealType>::squaredLength () const
{
    return
        m_afTuple[0]*m_afTuple[0] +
        m_afTuple[1]*m_afTuple[1] +
        m_afTuple[2]*m_afTuple[2] +
        m_afTuple[3]*m_afTuple[3];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Quaternion<RealType>::dot (const Quaternion& rkQ) const
{
    RealType fDot = (RealType)0.0;
    for (int i = 0; i < 4; i++)
        fDot += m_afTuple[i]*rkQ.m_afTuple[i];
    return fDot;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Quaternion<RealType>::normalize ()
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
Quaternion<RealType> Quaternion<RealType>::inverse () const
{
    Quaternion kInverse;

    RealType fNorm = (RealType)0.0;
    int i;
    for (i = 0; i < 4; i++)
        fNorm += m_afTuple[i]*m_afTuple[i];

    if ( fNorm > (RealType)0.0 )
    {
        RealType fInvNorm = ((RealType)1.0)/fNorm;
        kInverse.m_afTuple[0] = m_afTuple[0]*fInvNorm;
        kInverse.m_afTuple[1] = -m_afTuple[1]*fInvNorm;
        kInverse.m_afTuple[2] = -m_afTuple[2]*fInvNorm;
        kInverse.m_afTuple[3] = -m_afTuple[3]*fInvNorm;
    }
    else
    {
        // return an invalid result to flag the error
        for (i = 0; i < 4; i++)
            kInverse.m_afTuple[i] = (RealType)0.0;
    }

    return kInverse;
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType> Quaternion<RealType>::conjugate () const
{
    return Quaternion(m_afTuple[0],-m_afTuple[1],-m_afTuple[2],
        -m_afTuple[3]);
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType> Quaternion<RealType>::exp () const
{
    // If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
    // exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
    // use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.
    
    Quaternion kResult;
    
    RealType fAngle = Math<RealType>::sqRoot(m_afTuple[1]*m_afTuple[1] +
        m_afTuple[2]*m_afTuple[2] + m_afTuple[3]*m_afTuple[3]);
        
    RealType fSin = Math<RealType>::sinus(fAngle);
    kResult.m_afTuple[0] = Math<RealType>::cosinus(fAngle);
    
    int i;
    
    if ( Math<RealType>::fAbs(fSin) >= Math<RealType>::ZERO_TOLERANCE )
    {
        RealType fCoeff = fSin/fAngle;
        for (i = 1; i <= 3; i++)
            kResult.m_afTuple[i] = fCoeff*m_afTuple[i];
    }
    else
    {
        for (i = 1; i <= 3; i++)
            kResult.m_afTuple[i] = m_afTuple[i];
    }
    
    return kResult * std::exp(m_afTuple[0]);
} 
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType> Quaternion<RealType>::log () const
{
    // If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
    // log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
    // sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.
    
    Quaternion kResult;
    kResult.m_afTuple[0] = (RealType)0.0;
    
    int i;
    
    if ( Math<RealType>::fAbs(m_afTuple[0]) < (RealType)1.0 )
    {
        RealType fAngle = Math<RealType>::aCos(m_afTuple[0]);
        RealType fSin = Math<RealType>::sinus(fAngle);
        if ( Math<RealType>::fAbs(fSin) >= Math<RealType>::ZERO_TOLERANCE )
        {
            RealType fCoeff = fAngle/fSin;
            for (i = 1; i <= 3; i++)
                kResult.m_afTuple[i] = fCoeff*m_afTuple[i];
            return kResult;
        }
    }
    
    for (i = 1; i <= 3; i++)
        kResult.m_afTuple[i] = m_afTuple[i];
        
    return kResult;
}
//-----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType>& Quaternion<RealType>::power (const RealType q)
{
	Quaternion<RealType> qt = (this->log() * q ).exp();
	m_afTuple[0] = qt[0];
	m_afTuple[1] = qt[1];
	m_afTuple[2] = qt[2];
	m_afTuple[3] = qt[3];
	return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType> Quaternion<RealType>::rotate (const Vector3<RealType>& rkVector)
    const
{
    // Given a vector u = (x0,y0,z0) and a unit length quaternion
    // q = <w,x,y,z>, the vector v = (x1,y1,z1) which represents the
    // orientation of u by q is v = q*u*q^{-1} where * indicates quaternion
    // multiplication and where u is treated as the quaternion <0,x0,y0,z0>.
    // Note that q^{-1} = <w,-x,-y,-z>, so no real work is required to
    // invert q.  Now
    //
    //   q*u*q^{-1} = q*<0,x0,y0,z0>*q^{-1}
    //     = q*(x0*i+y0*j+z0*k)*q^{-1}
    //     = x0*(q*i*q^{-1})+y0*(q*j*q^{-1})+z0*(q*k*q^{-1})
    //
    // As 3-vectors, q*i*q^{-1}, q*j*q^{-1}, and 2*k*q^{-1} are the columns
    // of the orientation matrix computed in Quaternion<RealType>::ToRotationMatrix.
    // The vector v is obtained as the product of that orientation matrix with
    // vector u.  As such, the quaternion representation of a orientation
    // matrix requires less space than the matrix and more time to compute
    // the rotated vector.  Typical space-time tradeoff...

    Matrix3<RealType> kRot;
    ToRotationMatrix(kRot);
    return kRot*rkVector;
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType>& Quaternion<RealType>::slerp (RealType fT, const Quaternion& rkP,
    const Quaternion& rkQ)
{
    RealType fCos = rkP.Dot(rkQ);
    RealType fAngle = Math<RealType>::ACos(fCos);

    if ( Math<RealType>::fAbs(fAngle) >= Math<RealType>::ZERO_TOLERANCE )
    {
        RealType fSin = Math<RealType>::Sin(fAngle);
        RealType fInvSin = ((RealType)1.0)/fSin;
        RealType fCoeff0 = Math<RealType>::Sin(((RealType)1.0-fT)*fAngle)*fInvSin;
        RealType fCoeff1 = Math<RealType>::Sin(fT*fAngle)*fInvSin;
        *this = fCoeff0*rkP + fCoeff1*rkQ;
    }
    else
    {
        *this = rkP;
    }

    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType>& Quaternion<RealType>::slerpExtraSpins (RealType fT,
    const Quaternion& rkP, const Quaternion& rkQ, int iExtraSpins)
{
    RealType fCos = rkP.Dot(rkQ);
    RealType fAngle = Math<RealType>::ACos(fCos);

    if ( Math<RealType>::fAbs(fAngle) >= Math<RealType>::ZERO_TOLERANCE )
    {
        RealType fSin = Math<RealType>::Sin(fAngle);
        RealType fPhase = Math<RealType>::PI*iExtraSpins*fT;
        RealType fInvSin = ((RealType)1.0)/fSin;
        RealType fCoeff0 = Math<RealType>::Sin(((RealType)1.0-fT)*fAngle-fPhase)*fInvSin;
        RealType fCoeff1 = Math<RealType>::Sin(fT*fAngle + fPhase)*fInvSin;
        *this = fCoeff0*rkP + fCoeff1*rkQ;
    }
    else
    {
        *this = rkP;
    }

    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType>& Quaternion<RealType>::intermediate (const Quaternion& rkQ0,
    const Quaternion& rkQ1, const Quaternion& rkQ2)
{
    // assert:  Q0, Q1, Q2 all unit-length
    Quaternion kQ1Inv = rkQ1.Conjugate();
    Quaternion kP0 = kQ1Inv*rkQ0;
    Quaternion kP2 = kQ1Inv*rkQ2;
    Quaternion kArg = -((RealType)0.25)*(kP0.Log()+kP2.Log());
    Quaternion kA = rkQ1*kArg.Exp();
    *this = kA;

    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType>& Quaternion<RealType>::squad (RealType fT, const Quaternion& rkQ0,
    const Quaternion& rkA0, const Quaternion& rkA1, const Quaternion& rkQ1)
{
    RealType fSlerpT = ((RealType)2.0)*fT*((RealType)1.0-fT);
    Quaternion kSlerpP = Slerp(fT,rkQ0,rkQ1);
    Quaternion kSlerpQ = Slerp(fT,rkA0,rkA1);
    return Slerp(fSlerpT,kSlerpP,kSlerpQ);
}
//----------------------------------------------------------------------------
template <class RealType>
Quaternion<RealType>& Quaternion<RealType>::align (const Vector3<RealType>& rkV1,
    const Vector3<RealType>& rkV2)
{
    // If V1 and V2 are not parallel, the axis of orientation is the unit-length
    // vector U = Cross(V1,V2)/Length(Cross(V1,V2)).  The angle of orientation,
    // A, is the angle between V1 and V2.  The quaternion for the orientation is
    // q = cos(A/2) + sin(A/2)*(ux*i+uy*j+uz*k) where U = (ux,uy,uz).
    //
    // (1) Rather than extract A = acos(Dot(V1,V2)), multiply by 1/2, then
    //     compute sin(A/2) and cos(A/2), we reduce the computational costs by
    //     computing the bisector B = (V1+V2)/Length(V1+V2), so cos(A/2) =
    //     Dot(V1,B).
    //
    // (2) The orientation axis is U = Cross(V1,B)/Length(Cross(V1,B)), but
    //     Length(Cross(V1,B)) = Length(V1)*Length(B)*sin(A/2) = sin(A/2), in
    //     which case sin(A/2)*(ux*i+uy*j+uz*k) = (cx*i+cy*j+cz*k) where
    //     C = Cross(V1,B).
    //
    // If V1 and V2 are parallel, or nearly parallel as far as the floating
    // point calculations are concerned, the calculation of B will produce
    // the zero vector: Vector3::Normalize checks for closeness to zero and
    // returns the zero vector accordingly.  Thus, we test for parallelism
    // by checking if cos(A/2) is zero.  The test for exactly zero is usually
    // not recommend for floating point arithmetic, but the implementation of
    // Vector3::Normalize guarantees the comparison is robust.

    Vector3<RealType> kBisector = rkV1 + rkV2;
    kBisector.normalize();

    RealType fCosHalfAngle = rkV1.dot(kBisector);
    Vector3<RealType> kCross;

    if ( fCosHalfAngle != (RealType)0.0 )
        kCross = rkV1.cross(kBisector);
    else
        kCross = rkV1.unitCross(Vector3<RealType>(rkV2.z(),rkV2.x(),rkV2.y()));

    m_afTuple[0] = fCosHalfAngle;
    m_afTuple[1] = kCross.x();
    m_afTuple[2] = kCross.y();
    m_afTuple[3] = kCross.z();
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
void Quaternion<RealType>::decomposeTwistTimesSwing (
    const Vector3<RealType>& rkV1, Quaternion& rkTwist, Quaternion& rkSwing)
{
    Vector3<RealType> kV2 = rotate(rkV1);
    rkSwing = align(rkV1,kV2);
    rkTwist = (*this)*rkSwing.conjugate();
}
//----------------------------------------------------------------------------
template <class RealType>
void Quaternion<RealType>::decomposeSwingTimesTwist (
    const Vector3<RealType>& rkV1, Quaternion& rkSwing, Quaternion& rkTwist)
{
    Vector3<RealType> kV2 = rotate(rkV1);
    rkSwing = align(rkV1,kV2);
    rkTwist = rkSwing.conjugate()*(*this);
}
//----------------------------------------------------------------------------


template <class RealType>
void Quaternion<RealType>::fromAxes (const Vector3<RealType>& axis1,const Vector3<RealType>& axis2,const Vector3<RealType>& axis3)
{
	Matrix3<RealType> m;

        m[0][0] = axis1.x();
        m[1][0] = axis1.y();
        m[2][0] = axis1.z();
	
        m[0][1] = axis2.x();
        m[1][1] = axis2.y();
        m[2][1] = axis2.z();

        m[0][2] = axis3.x();
        m[1][2] = axis3.y();
        m[2][2] = axis3.z();

	fromRotationMatrix(m);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class RealType>
void Quaternion<RealType>::toAxes ( Vector3<RealType>& axis1, Vector3<RealType>& axis2 ,Vector3<RealType>& axis3) const
{
	Matrix3<RealType> m;

	toRotationMatrix(m);

        axis1.x() = m[0][0];
        axis1.y() = m[1][0];
        axis1.z() = m[2][0];

	axis2.x() = m[0][1];
        axis2.y() = m[1][1];
        axis2.z() = m[2][1];


	axis3.x() = m[0][2];
        axis3.y() = m[1][2];
        axis3.z() = m[2][2];
  
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class RealType>
void Quaternion<RealType>::toEulerAngles (Vector3<RealType>& eulerAngles,RealType threshold) const
{
	RealType heading,attitude,bank;
	RealType test = x()*y() + z()*w();
	if (test > 0.5-threshold /*0.499*/) // singularity at north pole
	{ 
		heading = 2 * Math<RealType>::aTan2(x(),w());
		attitude = Math<RealType>::HALF_PI;
		bank = 0;
	}
	if (test < threshold-0.5 /*-0.499*/) // singularity at south pole
	{
		heading = -2 * Math<RealType>::aTan2(x(),w());
		attitude = - Math<RealType>::HALF_PI;
		bank = 0;
	}
	else
	{
		RealType sqx = x()*x();
		RealType sqy = y()*y();
		RealType sqz = z()*z();
		heading = Math<RealType>::aTan2(2*y()*w()-2*x()*z() , 1 - 2*sqy - 2*sqz);
		attitude = Math<RealType>::aSin(2*test);
		bank = Math<RealType>::aTan2(2*x()*w()-2*y()*z() , 1 - 2*sqx - 2*sqz);
	}
	eulerAngles[0] = bank;
 	eulerAngles[1] = heading;
 	eulerAngles[2] = attitude;
	

//  	Matrix3 m;
//   	this->normalize();
//  	this->toRotationMatrix(m);
//   	m.ToEulerAnglesXYZ(eulerAngles[0],eulerAngles[1],eulerAngles[2]);

}

template <class RealType>
void Quaternion<RealType>::toGLMatrix(RealType m[16]) const
{

    float x2  = 2.0f*x();
    float y2  = 2.0f*y();
    float z2  = 2.0f*z();
    float x2w = x2*w();
    float y2w = y2*w();
    float z2w = z2*w();
    float x2x = x2*x();
    float y2x = y2*x();
    float z2x = z2*x();
    float y2y = y2*y();
    float z2y = z2*y();
    float z2z = z2*z();

    m[0]  = 1.0f-(y2y+z2z);
    m[4]  = y2x-z2w;
    m[8]  = z2x+y2w;
    m[1]  = y2x+z2w;
    m[5]  = 1.0f-(x2x+z2z);
    m[9]  = z2y-x2w;
    m[2]  = z2x-y2w;
    m[6]  = z2y+x2w;
    m[10] = 1.0f-(x2x+y2y);

	m[12] = 0.0l;
	m[13] = 0.0l;
	m[14] = 0.0l;

	m[3] = 0.0l;
	m[7] = 0.0l;
	m[11] = 0.0l;
	m[15] = 1.0l;

}


template <class RealType>
Vector3<RealType> Quaternion<RealType>::operator* (const Vector3<RealType>& v) const
{
    // Given a vector u = (x0,y0,z0) and a unit length quaternion
    // q = <w,x,y,z>, the vector v = (x1,y1,z1) which represents the
    // orientation of u by q is v = q*u*q^{-1} where * indicates quaternion
    // multiplication and where u is treated as the quaternion <0,x0,y0,z0>.
    // Note that q^{-1} = <w,-x,-y,-z>, so no float work is required to
    // invert q.  Now
    //
    //   q*u*q^{-1} = q*<0,x0,y0,z0>*q^{-1}
    //     = q*(x0*i+y0*j+z0*k)*q^{-1}
    //     = x0*(q*i*q^{-1})+y0*(q*j*q^{-1})+z0*(q*k*q^{-1})
    //
    // As 3-vectors, q*i*q^{-1}, q*j*q^{-1}, and 2*k*q^{-1} are the columns
    // of the orientation matrix computed in Quaternion::ToRotationMatrix.
    // The vector v is obtained as the product of that orientation matrix with
    // vector u.  As such, the quaternion representation of a orientation
    // matrix requires less space than the matrix and more time to compute
    // the rotated vector.  Typical space-time tradeoff...

    Matrix3<RealType> m;
    toRotationMatrix(m);
    return m*v;
}
