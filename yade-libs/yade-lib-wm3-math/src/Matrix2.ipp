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
Matrix2<RealType>::Matrix2 (bool bZero)
{
    if ( bZero )
        makeZero();
    else
        makeIdentity();
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType>::Matrix2 (const Matrix2& rkM)
{
    memcpy(m_afEntry,rkM.m_afEntry,4*sizeof(RealType));
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType>::Matrix2 (RealType fM00, RealType fM01, RealType fM10, RealType fM11)
{
    m_afEntry[0] = fM00;
    m_afEntry[1] = fM01;
    m_afEntry[2] = fM10;
    m_afEntry[3] = fM11;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType>::Matrix2 (const RealType afEntry[4], bool bRowMajor)
{
    if ( bRowMajor )
    {
        memcpy(m_afEntry,afEntry,4*sizeof(RealType));
    }
    else
    {
        m_afEntry[0] = afEntry[0];
        m_afEntry[1] = afEntry[2];
        m_afEntry[2] = afEntry[1];
        m_afEntry[3] = afEntry[3];
    }
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType>::Matrix2 (const Vector2<RealType>& rkU, const Vector2<RealType>& rkV,
    bool bColumns)
{
    if ( bColumns )
    {
        m_afEntry[0] = rkU[0];
        m_afEntry[1] = rkV[0];
        m_afEntry[2] = rkU[1];
        m_afEntry[3] = rkV[1];
    }
    else
    {
        m_afEntry[0] = rkU[0];
        m_afEntry[1] = rkU[1];
        m_afEntry[2] = rkV[0];
        m_afEntry[3] = rkV[1];
    }
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType>::Matrix2 (const Vector2<RealType>* akV, bool bColumns)
{
    if ( bColumns )
    {
        m_afEntry[0] = akV[0][0];
        m_afEntry[1] = akV[1][0];
        m_afEntry[2] = akV[0][1];
        m_afEntry[3] = akV[1][1];
    }
    else
    {
        m_afEntry[0] = akV[0][0];
        m_afEntry[1] = akV[0][1];
        m_afEntry[2] = akV[1][0];
        m_afEntry[3] = akV[1][1];
    }
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType>::Matrix2 (RealType fM00, RealType fM11)
{
    makeDiagonal(fM00,fM11);
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType>::Matrix2 (RealType fAngle)
{
    fromAngle(fAngle);
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType>::Matrix2 (const Vector2<RealType>& rkU, const Vector2<RealType>& rkV)
{
    makeTensorProduct(rkU,rkV);
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType>::operator const RealType* () const
{
    return m_afEntry;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType>::operator RealType* ()
{
    return m_afEntry;
}
//----------------------------------------------------------------------------
template <class RealType>
const RealType* Matrix2<RealType>::operator[] (int iRow) const
{
    return &m_afEntry[2*iRow];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType* Matrix2<RealType>::operator[] (int iRow)
{
    return &m_afEntry[2*iRow];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Matrix2<RealType>::operator() (int iRow, int iCol) const
{
    return m_afEntry[I(iRow,iCol)];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType& Matrix2<RealType>::operator() (int iRow, int iCol)
{
    return m_afEntry[I(iRow,iCol)];
}
//----------------------------------------------------------------------------
template <class RealType>
int Matrix2<RealType>::I (int iRow, int iCol)
{
    assert( 0 <= iRow && iRow < 2 && 0 <= iCol && iCol < 2 );
    return iCol + 2*iRow;
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix2<RealType>::makeZero ()
{
    memset(m_afEntry,0,4*sizeof(RealType));
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix2<RealType>::makeIdentity ()
{
    m_afEntry[0] = (RealType)1.0;
    m_afEntry[1] = (RealType)0.0;
    m_afEntry[2] = (RealType)0.0;
    m_afEntry[3] = (RealType)1.0;
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix2<RealType>::makeDiagonal (RealType fM00, RealType fM11)
{
    m_afEntry[0] = fM00;
    m_afEntry[1] = (RealType)0.0;
    m_afEntry[2] = (RealType)0.0;
    m_afEntry[3] = fM11;
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix2<RealType>::fromAngle (RealType fAngle)
{
    m_afEntry[0] = Math<RealType>::Cos(fAngle);
    m_afEntry[2] = Math<RealType>::Sin(fAngle);
    m_afEntry[1] = -m_afEntry[2];
    m_afEntry[3] =  m_afEntry[0];
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix2<RealType>::makeTensorProduct (const Vector2<RealType>& rkU,
    const Vector2<RealType>& rkV)
{
    m_afEntry[0] = rkU[0]*rkV[0];
    m_afEntry[1] = rkU[0]*rkV[1];
    m_afEntry[2] = rkU[1]*rkV[0];
    m_afEntry[3] = rkU[1]*rkV[1];
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix2<RealType>::setRow (int iRow, const Vector2<RealType>& rkV)
{
    assert( 0 <= iRow && iRow < 2 );
    for (int iCol = 0, i = 2*iRow; iCol < 2; iCol++, i++)
        m_afEntry[i] = rkV[iCol];
}
//----------------------------------------------------------------------------
template <class RealType>
Vector2<RealType> Matrix2<RealType>::getRow (int iRow) const
{
    assert( 0 <= iRow && iRow < 2 );
    Vector2<RealType> kV;
    for (int iCol = 0, i = 2*iRow; iCol < 2; iCol++, i++)
        kV[iCol] = m_afEntry[i];
    return kV;
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix2<RealType>::setColumn (int iCol, const Vector2<RealType>& rkV)
{
    assert( 0 <= iCol && iCol < 2 );
    for (int iRow = 0, i = iCol; iRow < 2; iRow++, i += 2)
        m_afEntry[i] = rkV[iRow];
}
//----------------------------------------------------------------------------
template <class RealType>
Vector2<RealType> Matrix2<RealType>::getColumn (int iCol) const
{
    assert( 0 <= iCol && iCol < 2 );
    Vector2<RealType> kV;
    for (int iRow = 0, i = iCol; iRow < 2; iRow++, i += 2)
        kV[iRow] = m_afEntry[i];
    return kV;
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix2<RealType>::getColumnMajor (RealType* afCMajor) const
{
    for (int iRow = 0, i = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
            afCMajor[i++] = m_afEntry[I(iCol,iRow)];
    }
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType>& Matrix2<RealType>::operator= (const Matrix2& rkM)
{
    memcpy(m_afEntry,rkM.m_afEntry,4*sizeof(RealType));
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
int Matrix2<RealType>::compareArrays (const Matrix2& rkM) const
{
    return memcmp(m_afEntry,rkM.m_afEntry,4*sizeof(RealType));
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix2<RealType>::operator== (const Matrix2& rkM) const
{
    return CompareArrays(rkM) == 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix2<RealType>::operator!= (const Matrix2& rkM) const
{
    return CompareArrays(rkM) != 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix2<RealType>::operator<  (const Matrix2& rkM) const
{
    return CompareArrays(rkM) < 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix2<RealType>::operator<= (const Matrix2& rkM) const
{
    return CompareArrays(rkM) <= 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix2<RealType>::operator>  (const Matrix2& rkM) const
{
    return CompareArrays(rkM) > 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix2<RealType>::operator>= (const Matrix2& rkM) const
{
    return CompareArrays(rkM) >= 0;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType> Matrix2<RealType>::operator+ (const Matrix2& rkM) const
{
    Matrix2 kSum;
    for (int i = 0; i < 4; i++)
        kSum.m_afEntry[i] = m_afEntry[i] + rkM.m_afEntry[i];
    return kSum;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType> Matrix2<RealType>::operator- (const Matrix2& rkM) const
{
    Matrix2 kDiff;
    for (int i = 0; i < 4; i++)
        kDiff.m_afEntry[i] = m_afEntry[i] - rkM.m_afEntry[i];
    return kDiff;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType> Matrix2<RealType>::operator* (const Matrix2& rkM) const
{
    Matrix2 kProd;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
        {
            int i = I(iRow,iCol);
            kProd.m_afEntry[i] = (RealType)0.0;
            for (int iMid = 0; iMid < 2; iMid++)
            {
                kProd.m_afEntry[i] +=
                    m_afEntry[I(iRow,iMid)]*rkM.m_afEntry[I(iMid,iCol)];
            }
        }
    }
    return kProd;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType> Matrix2<RealType>::operator* (RealType fScalar) const
{
    Matrix2 kProd;
    for (int i = 0; i < 4; i++)
        kProd.m_afEntry[i] = fScalar*m_afEntry[i];
    return kProd;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType> Matrix2<RealType>::operator/ (RealType fScalar) const
{
    Matrix2 kQuot;
    int i;

    if ( fScalar != (RealType)0.0 )
    {
        RealType fInvScalar = ((RealType)1.0)/fScalar;
        for (i = 0; i < 4; i++)
            kQuot.m_afEntry[i] = fInvScalar*m_afEntry[i];
    }
    else
    {
        for (i = 0; i < 4; i++)
            kQuot.m_afEntry[i] = Math<RealType>::MAX_REAL;
    }

    return kQuot;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType> Matrix2<RealType>::operator- () const
{
    Matrix2 kNeg;
    for (int i = 0; i < 4; i++)
        kNeg.m_afEntry[i] = -m_afEntry[i];
    return kNeg;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType>& Matrix2<RealType>::operator+= (const Matrix2& rkM)
{
    for (int i = 0; i < 4; i++)
        m_afEntry[i] += rkM.m_afEntry[i];
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType>& Matrix2<RealType>::operator-= (const Matrix2& rkM)
{
    for (int i = 0; i < 4; i++)
        m_afEntry[i] -= rkM.m_afEntry[i];
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType>& Matrix2<RealType>::operator*= (RealType fScalar)
{
    for (int i = 0; i < 4; i++)
        m_afEntry[i] *= fScalar;
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType>& Matrix2<RealType>::operator/= (RealType fScalar)
{
    int i;

    if ( fScalar != (RealType)0.0 )
    {
        RealType fInvScalar = ((RealType)1.0)/fScalar;
        for (i = 0; i < 4; i++)
            m_afEntry[i] *= fInvScalar;
    }
    else
    {
        for (i = 0; i < 4; i++)
            m_afEntry[i] = Math<RealType>::MAX_REAL;
    }

    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector2<RealType> Matrix2<RealType>::operator* (const Vector2<RealType>& rkV) const
{
    Vector2<RealType> kProd;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        kProd[iRow] = (RealType)0.0;
        for (int iCol = 0; iCol < 2; iCol++)
            kProd[iRow] += m_afEntry[I(iRow,iCol)]*rkV[iCol];
    }
    return kProd;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType> Matrix2<RealType>::transpose () const
{
    Matrix2 kTranspose;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
            kTranspose.m_afEntry[I(iRow,iCol)] = m_afEntry[I(iCol,iRow)];
    }
    return kTranspose;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType> Matrix2<RealType>::transposeTimes (const Matrix2& rkM) const
{
    // P = A^T*B, P[r][c] = sum_m A[m][r]*B[m][c]
    Matrix2 kProd;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
        {
            int i = I(iRow,iCol);
            kProd.m_afEntry[i] = (RealType)0.0;
            for (int iMid = 0; iMid < 2; iMid++)
            {
                kProd.m_afEntry[i] +=
                    m_afEntry[I(iMid,iRow)]*rkM.m_afEntry[I(iMid,iCol)];
            }
        }
    }
    return kProd;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType> Matrix2<RealType>::timesTranspose (const Matrix2& rkM) const
{
    // P = A*B^T, P[r][c] = sum_m A[r][m]*B[c][m]
    Matrix2 kProd;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
        {
            int i = I(iRow,iCol);
            kProd.m_afEntry[i] = (RealType)0.0;
            for (int iMid = 0; iMid < 2; iMid++)
            {
                kProd.m_afEntry[i] +=
                    m_afEntry[I(iRow,iMid)]*rkM.m_afEntry[I(iCol,iRow)];
            }
        }
    }
    return kProd;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType> Matrix2<RealType>::inverse () const
{
    Matrix2 kInverse;

    RealType fDet = m_afEntry[0]*m_afEntry[3] - m_afEntry[1]*m_afEntry[2];
    if ( Math<RealType>::fAbs(fDet) > Math<RealType>::ZERO_TOLERANCE )
    {
        RealType fInvDet = ((RealType)1.0)/fDet;
        kInverse[0][0] =  m_afEntry[3]*fInvDet;
        kInverse[0][1] = -m_afEntry[1]*fInvDet;
        kInverse[1][0] = -m_afEntry[2]*fInvDet;
        kInverse[1][1] =  m_afEntry[0]*fInvDet;
    }
    else
    {
        kInverse.MakeZero();
    }

    return kInverse;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType> Matrix2<RealType>::adjoint () const
{
    return Matrix2(
         m_afEntry[3],-m_afEntry[1],
        -m_afEntry[2], m_afEntry[0]);
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Matrix2<RealType>::determinant () const
{
    return m_afEntry[0]*m_afEntry[3] - m_afEntry[1]*m_afEntry[2];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Matrix2<RealType>::qForm (const Vector2<RealType>& rkU,
    const Vector2<RealType>& rkV) const
{
    return rkU.Dot((*this)*rkV);
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix2<RealType>::toAngle (RealType& rfAngle) const
{
    // assert:  matrix is a orientation
    rfAngle = Math<RealType>::ATan2(m_afEntry[2],m_afEntry[0]);
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix2<RealType>::orthonormalize ()
{
    // Algorithm uses Gram-Schmidt orthogonalization.  If 'this' matrix is
    // M = [m0|m1], then orthonormal output matrix is Q = [q0|q1],
    //
    //   q0 = m0/|m0|
    //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
    //
    // where |V| indicates length of vector V and A*B indicates dot
    // product of vectors A and B.

    // compute q0
    RealType fInvLength = Math<RealType>::InvSqrt(m_afEntry[0]*m_afEntry[0] +
        m_afEntry[2]*m_afEntry[2]);

    m_afEntry[0] *= fInvLength;
    m_afEntry[2] *= fInvLength;

    // compute q1
    RealType fDot0 = m_afEntry[0]*m_afEntry[1] + m_afEntry[2]*m_afEntry[3];
    m_afEntry[1] -= fDot0*m_afEntry[0];
    m_afEntry[3] -= fDot0*m_afEntry[2];

    fInvLength = Math<RealType>::InvSqrt(m_afEntry[1]*m_afEntry[1] +
        m_afEntry[3]*m_afEntry[3]);

    m_afEntry[1] *= fInvLength;
    m_afEntry[3] *= fInvLength;
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix2<RealType>::eigenDecomposition (Matrix2& rkRot, Matrix2& rkDiag) const
{
    RealType fTrace = m_afEntry[0] + m_afEntry[3];
    RealType fDiff = m_afEntry[0] - m_afEntry[3];
    RealType fDiscr = Math<RealType>::Sqrt(fDiff*fDiff +
        ((RealType)4.0)*m_afEntry[1]*m_afEntry[1]);
    RealType fEVal0 = ((RealType)0.5)*(fTrace-fDiscr);
    RealType fEVal1 = ((RealType)0.5)*(fTrace+fDiscr);
    rkDiag.MakeDiagonal(fEVal0,fEVal1);

    RealType fCos, fSin;
    if ( fDiff >= (RealType)0.0 )
    {
        fCos = m_afEntry[1];
        fSin = fEVal0 - m_afEntry[0];
    }
    else
    {
        fCos = fEVal0 - m_afEntry[3];
        fSin = m_afEntry[1];
    }
    RealType fTmp = Math<RealType>::InvSqrt(fCos*fCos + fSin*fSin);
    fCos *= fTmp;
    fSin *= fTmp;

    rkRot.m_afEntry[0] = fCos;
    rkRot.m_afEntry[1] = -fSin;
    rkRot.m_afEntry[2] = fSin;
    rkRot.m_afEntry[3] = fCos;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix2<RealType> operator* (RealType fScalar, const Matrix2<RealType>& rkM)
{
    return rkM*fScalar;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector2<RealType> operator* (const Vector2<RealType>& rkV, const Matrix2<RealType>& rkM)
{
    return Vector2<RealType>(
        rkV[0]*rkM[0][0] + rkV[1]*rkM[1][0],
        rkV[0]*rkM[0][1] + rkV[1]*rkM[1][1]);
}
//----------------------------------------------------------------------------

