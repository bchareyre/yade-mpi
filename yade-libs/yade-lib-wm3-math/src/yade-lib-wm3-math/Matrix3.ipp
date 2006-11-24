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
Matrix3<RealType>::Matrix3 (bool bZero)
{
    if ( bZero )
        makeZero();
    else
        makeIdentity();
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>::Matrix3 (const Matrix3& rkM)
{
    memcpy(m_afEntry,rkM.m_afEntry,9*sizeof(RealType));
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>::Matrix3 (RealType fM00, RealType fM01, RealType fM02, RealType fM10, RealType fM11,
    RealType fM12, RealType fM20, RealType fM21, RealType fM22)
{
    m_afEntry[0] = fM00;
    m_afEntry[1] = fM01;
    m_afEntry[2] = fM02;
    m_afEntry[3] = fM10;
    m_afEntry[4] = fM11;
    m_afEntry[5] = fM12;
    m_afEntry[6] = fM20;
    m_afEntry[7] = fM21;
    m_afEntry[8] = fM22;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>::Matrix3 (const RealType afEntry[9], bool bRowMajor)
{
    if ( bRowMajor )
    {
        memcpy(m_afEntry,afEntry,9*sizeof(RealType));
    }
    else
    {
        m_afEntry[0] = afEntry[0];
        m_afEntry[1] = afEntry[3];
        m_afEntry[2] = afEntry[6];
        m_afEntry[3] = afEntry[1];
        m_afEntry[4] = afEntry[4];
        m_afEntry[5] = afEntry[7];
        m_afEntry[6] = afEntry[2];
        m_afEntry[7] = afEntry[5];
        m_afEntry[8] = afEntry[8];
    }
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>::Matrix3 (const Vector3<RealType>& rkU, const Vector3<RealType>& rkV,
    const Vector3<RealType>& rkW, bool bColumns)
{
    if ( bColumns )
    {
        m_afEntry[0] = rkU[0];
        m_afEntry[1] = rkV[0];
        m_afEntry[2] = rkW[0];
        m_afEntry[3] = rkU[1];
        m_afEntry[4] = rkV[1];
        m_afEntry[5] = rkW[1];
        m_afEntry[6] = rkU[2];
        m_afEntry[7] = rkV[2];
        m_afEntry[8] = rkW[2];
    }
    else
    {
        m_afEntry[0] = rkU[0];
        m_afEntry[1] = rkU[1];
        m_afEntry[2] = rkU[2];
        m_afEntry[3] = rkV[0];
        m_afEntry[4] = rkV[1];
        m_afEntry[5] = rkV[2];
        m_afEntry[6] = rkW[0];
        m_afEntry[7] = rkW[1];
        m_afEntry[8] = rkW[2];
    }
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>::Matrix3 (const Vector3<RealType>* akV, bool bColumns)
{
    if ( bColumns )
    {
        m_afEntry[0] = akV[0][0];
        m_afEntry[1] = akV[1][0];
        m_afEntry[2] = akV[2][0];
        m_afEntry[3] = akV[0][1];
        m_afEntry[4] = akV[1][1];
        m_afEntry[5] = akV[2][1];
        m_afEntry[6] = akV[0][2];
        m_afEntry[7] = akV[1][2];
        m_afEntry[8] = akV[2][2];
    }
    else
    {
        m_afEntry[0] = akV[0][0];
        m_afEntry[1] = akV[0][1];
        m_afEntry[2] = akV[0][2];
        m_afEntry[3] = akV[1][0];
        m_afEntry[4] = akV[1][1];
        m_afEntry[5] = akV[1][2];
        m_afEntry[6] = akV[2][0];
        m_afEntry[7] = akV[2][1];
        m_afEntry[8] = akV[2][2];
    }
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>::Matrix3 (RealType fM00, RealType fM11, RealType fM22)
{
    makeDiagonal(fM00,fM11,fM22);
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>::Matrix3 (const Vector3<RealType>& rkAxis, RealType fAngle)
{
    fromAxisAngle(rkAxis,fAngle);
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>::Matrix3 (const Vector3<RealType>& rkU, const Vector3<RealType>& rkV)
{
    makeTensorProduct(rkU,rkV);
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>::operator const RealType* () const
{
    return m_afEntry;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>::operator RealType* ()
{
    return m_afEntry;
}
//----------------------------------------------------------------------------
template <class RealType>
const RealType* Matrix3<RealType>::operator[] (int iRow) const
{
    return &m_afEntry[3*iRow];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType* Matrix3<RealType>::operator[] (int iRow)
{
    return &m_afEntry[3*iRow];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Matrix3<RealType>::operator() (int iRow, int iCol) const
{
    return m_afEntry[I(iRow,iCol)];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType& Matrix3<RealType>::operator() (int iRow, int iCol)
{
    return m_afEntry[I(iRow,iCol)];
}
//----------------------------------------------------------------------------
template <class RealType>
int Matrix3<RealType>::I (int iRow, int iCol)
{
    assert( 0 <= iRow && iRow < 3 && 0 <= iCol && iCol < 3 );
    return iCol + 3*iRow;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>& Matrix3<RealType>::makeZero ()
{
    memset(m_afEntry,0,9*sizeof(RealType));
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>& Matrix3<RealType>::makeIdentity ()
{
    m_afEntry[0] = (RealType)1.0;
    m_afEntry[1] = (RealType)0.0;
    m_afEntry[2] = (RealType)0.0;
    m_afEntry[3] = (RealType)0.0;
    m_afEntry[4] = (RealType)1.0;
    m_afEntry[5] = (RealType)0.0;
    m_afEntry[6] = (RealType)0.0;
    m_afEntry[7] = (RealType)0.0;
    m_afEntry[8] = (RealType)1.0;
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>& Matrix3<RealType>::makeDiagonal (RealType fM00, RealType fM11, RealType fM22)
{
    m_afEntry[0] = fM00;
    m_afEntry[1] = (RealType)0.0;
    m_afEntry[2] = (RealType)0.0;
    m_afEntry[3] = (RealType)0.0;
    m_afEntry[4] = fM11;
    m_afEntry[5] = (RealType)0.0;
    m_afEntry[6] = (RealType)0.0;
    m_afEntry[7] = (RealType)0.0;
    m_afEntry[8] = fM22;
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>& Matrix3<RealType>::fromAxisAngle (const Vector3<RealType>& rkAxis,
    RealType fAngle)
{
    RealType fCos = Math<RealType>::Cos(fAngle);
    RealType fSin = Math<RealType>::Sin(fAngle);
    RealType fOneMinusCos = ((RealType)1.0)-fCos;
    RealType fX2 = rkAxis[0]*rkAxis[0];
    RealType fY2 = rkAxis[1]*rkAxis[1];
    RealType fZ2 = rkAxis[2]*rkAxis[2];
    RealType fXYM = rkAxis[0]*rkAxis[1]*fOneMinusCos;
    RealType fXZM = rkAxis[0]*rkAxis[2]*fOneMinusCos;
    RealType fYZM = rkAxis[1]*rkAxis[2]*fOneMinusCos;
    RealType fXSin = rkAxis[0]*fSin;
    RealType fYSin = rkAxis[1]*fSin;
    RealType fZSin = rkAxis[2]*fSin;
    
    m_afEntry[0] = fX2*fOneMinusCos+fCos;
    m_afEntry[1] = fXYM-fZSin;
    m_afEntry[2] = fXZM+fYSin;
    m_afEntry[3] = fXYM+fZSin;
    m_afEntry[4] = fY2*fOneMinusCos+fCos;
    m_afEntry[5] = fYZM-fXSin;
    m_afEntry[6] = fXZM-fYSin;
    m_afEntry[7] = fYZM+fXSin;
    m_afEntry[8] = fZ2*fOneMinusCos+fCos;

    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>& Matrix3<RealType>::makeTensorProduct (const Vector3<RealType>& rkU,
    const Vector3<RealType>& rkV)
{
    m_afEntry[0] = rkU[0]*rkV[0];
    m_afEntry[1] = rkU[0]*rkV[1];
    m_afEntry[2] = rkU[0]*rkV[2];
    m_afEntry[3] = rkU[1]*rkV[0];
    m_afEntry[4] = rkU[1]*rkV[1];
    m_afEntry[5] = rkU[1]*rkV[2];
    m_afEntry[6] = rkU[2]*rkV[0];
    m_afEntry[7] = rkU[2]*rkV[1];
    m_afEntry[8] = rkU[2]*rkV[2];
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix3<RealType>::setRow (int iRow, const Vector3<RealType>& rkV)
{
    assert( 0 <= iRow && iRow < 3 );
    for (int iCol = 0, i = 3*iRow; iCol < 3; iCol++, i++)
        m_afEntry[i] = rkV[iCol];
}
//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType> Matrix3<RealType>::getRow (int iRow) const
{
    assert( 0 <= iRow && iRow < 3 );
    Vector3<RealType> kV;
    for (int iCol = 0, i = 3*iRow; iCol < 3; iCol++, i++)
        kV[iCol] = m_afEntry[i];
    return kV;
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix3<RealType>::setColumn (int iCol, const Vector3<RealType>& rkV)
{
    assert( 0 <= iCol && iCol < 3 );
    for (int iRow = 0, i = iCol; iRow < 3; iRow++, i += 3)
        m_afEntry[i] = rkV[iRow];
}
//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType> Matrix3<RealType>::getColumn (int iCol) const
{
    assert( 0 <= iCol && iCol < 3 );
    Vector3<RealType> kV;
    for (int iRow = 0, i = iCol; iRow < 3; iRow++, i += 3)
        kV[iRow] = m_afEntry[i];
    return kV;
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix3<RealType>::getColumnMajor (RealType* afCMajor) const
{
    for (int iRow = 0, i = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
            afCMajor[i++] = m_afEntry[I(iCol,iRow)];
    }
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>& Matrix3<RealType>::operator= (const Matrix3& rkM)
{
    memcpy(m_afEntry,rkM.m_afEntry,9*sizeof(RealType));
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
int Matrix3<RealType>::compareArrays (const Matrix3& rkM) const
{
    return memcmp(m_afEntry,rkM.m_afEntry,9*sizeof(RealType));
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix3<RealType>::operator== (const Matrix3& rkM) const
{
    return CompareArrays(rkM) == 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix3<RealType>::operator!= (const Matrix3& rkM) const
{
    return CompareArrays(rkM) != 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix3<RealType>::operator<  (const Matrix3& rkM) const
{
    return CompareArrays(rkM) < 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix3<RealType>::operator<= (const Matrix3& rkM) const
{
    return CompareArrays(rkM) <= 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix3<RealType>::operator>  (const Matrix3& rkM) const
{
    return CompareArrays(rkM) > 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix3<RealType>::operator>= (const Matrix3& rkM) const
{
    return CompareArrays(rkM) >= 0;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType> Matrix3<RealType>::operator+ (const Matrix3& rkM) const
{
    Matrix3 kSum;
    for (int i = 0; i < 9; i++)
        kSum.m_afEntry[i] = m_afEntry[i] + rkM.m_afEntry[i];
    return kSum;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType> Matrix3<RealType>::operator- (const Matrix3& rkM) const
{
    Matrix3 kDiff;
    for (int i = 0; i < 9; i++)
        kDiff.m_afEntry[i] = m_afEntry[i] - rkM.m_afEntry[i];
    return kDiff;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType> Matrix3<RealType>::operator* (const Matrix3& rkM) const
{
    Matrix3 kProd;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
        {
            int i = I(iRow,iCol);
            kProd.m_afEntry[i] = (RealType)0.0;
            for (int iMid = 0; iMid < 3; iMid++)
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
Matrix3<RealType> Matrix3<RealType>::operator* (RealType fScalar) const
{
    Matrix3 kProd;
    for (int i = 0; i < 9; i++)
        kProd.m_afEntry[i] = fScalar*m_afEntry[i];
    return kProd;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType> Matrix3<RealType>::operator/ (RealType fScalar) const
{
    Matrix3 kQuot;
    int i;

    if ( fScalar != (RealType)0.0 )
    {
        RealType fInvScalar = ((RealType)1.0)/fScalar;
        for (i = 0; i < 9; i++)
            kQuot.m_afEntry[i] = fInvScalar*m_afEntry[i];
    }
    else
    {
        for (i = 0; i < 9; i++)
            kQuot.m_afEntry[i] = Math<RealType>::MAX_REAL;
    }

    return kQuot;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType> Matrix3<RealType>::operator- () const
{
    Matrix3 kNeg;
    for (int i = 0; i < 9; i++)
        kNeg.m_afEntry[i] = -m_afEntry[i];
    return kNeg;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>& Matrix3<RealType>::operator+= (const Matrix3& rkM)
{
    for (int i = 0; i < 9; i++)
        m_afEntry[i] += rkM.m_afEntry[i];
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>& Matrix3<RealType>::operator-= (const Matrix3& rkM)
{
    for (int i = 0; i < 9; i++)
        m_afEntry[i] -= rkM.m_afEntry[i];
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>& Matrix3<RealType>::operator*= (RealType fScalar)
{
    for (int i = 0; i < 9; i++)
        m_afEntry[i] *= fScalar;
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>& Matrix3<RealType>::operator/= (RealType fScalar)
{
    int i;

    if ( fScalar != (RealType)0.0 )
    {
        RealType fInvScalar = ((RealType)1.0)/fScalar;
        for (i = 0; i < 9; i++)
            m_afEntry[i] *= fInvScalar;
    }
    else
    {
        for (i = 0; i < 9; i++)
            m_afEntry[i] = Math<RealType>::MAX_REAL;
    }

    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType> Matrix3<RealType>::operator* (const Vector3<RealType>& rkV) const
{
    Vector3<RealType> kProd;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        kProd[iRow] = 0.0f;
        for (int iCol = 0; iCol < 3; iCol++)
            kProd[iRow] += m_afEntry[I(iRow,iCol)]*rkV[iCol];
            
    }
    return kProd;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType> Matrix3<RealType>::transpose () const
{
    Matrix3 kTranspose;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
            kTranspose.m_afEntry[I(iRow,iCol)] = m_afEntry[I(iCol,iRow)];
    }
    return kTranspose;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType> Matrix3<RealType>::transposeTimes (const Matrix3& rkM) const
{
    // P = A^T*B, P[r][c] = sum_m A[m][r]*B[m][c]
    Matrix3 kProd;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
        {
            int i = I(iRow,iCol);
            kProd.m_afEntry[i] = (RealType)0.0;
            for (int iMid = 0; iMid < 3; iMid++)
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
Matrix3<RealType> Matrix3<RealType>::timesTranspose (const Matrix3& rkM) const
{
    // P = A*B^T, P[r][c] = sum_m A[r][m]*B[c][m]
    Matrix3 kProd;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
        {
            int i = I(iRow,iCol);
            kProd.m_afEntry[i] = (RealType)0.0;
            for (int iMid = 0; iMid < 3; iMid++)
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
Matrix3<RealType> Matrix3<RealType>::inverse () const
{
    // Invert a 3x3 using cofactors.  This is faster than using a generic
    // Gaussian elimination because of the loop overhead of such a method.

    Matrix3 kInverse;

    kInverse[0][0] = m_afEntry[4]*m_afEntry[8] - m_afEntry[5]*m_afEntry[7];
    kInverse[0][1] = m_afEntry[2]*m_afEntry[7] - m_afEntry[1]*m_afEntry[8];
    kInverse[0][2] = m_afEntry[1]*m_afEntry[5] - m_afEntry[2]*m_afEntry[4];
    kInverse[1][0] = m_afEntry[5]*m_afEntry[6] - m_afEntry[3]*m_afEntry[8];
    kInverse[1][1] = m_afEntry[0]*m_afEntry[8] - m_afEntry[2]*m_afEntry[6];
    kInverse[1][2] = m_afEntry[2]*m_afEntry[3] - m_afEntry[0]*m_afEntry[5];
    kInverse[2][0] = m_afEntry[3]*m_afEntry[7] - m_afEntry[4]*m_afEntry[6];
    kInverse[2][1] = m_afEntry[1]*m_afEntry[6] - m_afEntry[0]*m_afEntry[7];
    kInverse[2][2] = m_afEntry[0]*m_afEntry[4] - m_afEntry[1]*m_afEntry[3];

    RealType fDet = m_afEntry[0]*kInverse[0][0] + m_afEntry[1]*kInverse[1][0]+
        m_afEntry[2]*kInverse[2][0];

    if ( Math<RealType>::fAbs(fDet) <= Math<RealType>::ZERO_TOLERANCE )
        return ZERO;

    kInverse /= fDet;
    return kInverse;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType> Matrix3<RealType>::adjoint () const
{
    Matrix3 kAdjoint;

    kAdjoint[0][0] = m_afEntry[4]*m_afEntry[8] - m_afEntry[5]*m_afEntry[7];
    kAdjoint[0][1] = m_afEntry[2]*m_afEntry[7] - m_afEntry[1]*m_afEntry[8];
    kAdjoint[0][2] = m_afEntry[1]*m_afEntry[5] - m_afEntry[2]*m_afEntry[4];
    kAdjoint[1][0] = m_afEntry[5]*m_afEntry[6] - m_afEntry[3]*m_afEntry[8];
    kAdjoint[1][1] = m_afEntry[0]*m_afEntry[8] - m_afEntry[2]*m_afEntry[6];
    kAdjoint[1][2] = m_afEntry[2]*m_afEntry[3] - m_afEntry[0]*m_afEntry[5];
    kAdjoint[2][0] = m_afEntry[3]*m_afEntry[7] - m_afEntry[4]*m_afEntry[6];
    kAdjoint[2][1] = m_afEntry[1]*m_afEntry[6] - m_afEntry[0]*m_afEntry[7];
    kAdjoint[2][2] = m_afEntry[0]*m_afEntry[4] - m_afEntry[1]*m_afEntry[3];

    return kAdjoint;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Matrix3<RealType>::determinant () const
{
    RealType fCo00 = m_afEntry[4]*m_afEntry[8] - m_afEntry[5]*m_afEntry[7];
    RealType fCo10 = m_afEntry[5]*m_afEntry[6] - m_afEntry[3]*m_afEntry[8];
    RealType fCo20 = m_afEntry[3]*m_afEntry[7] - m_afEntry[4]*m_afEntry[6];
    RealType fDet = m_afEntry[0]*fCo00 + m_afEntry[1]*fCo10 + m_afEntry[2]*fCo20;
    return fDet;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Matrix3<RealType>::qForm (const Vector3<RealType>& rkU, const Vector3<RealType>& rkV) const
{
    return rkU.Dot((*this)*rkV);
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType> Matrix3<RealType>::timesDiagonal (const Vector3<RealType>& rkDiag) const
{
    return Matrix3(
        m_afEntry[0]*rkDiag[0],m_afEntry[1]*rkDiag[1],m_afEntry[2]*rkDiag[2],
        m_afEntry[3]*rkDiag[0],m_afEntry[4]*rkDiag[1],m_afEntry[5]*rkDiag[2],
        m_afEntry[6]*rkDiag[0],m_afEntry[7]*rkDiag[1],m_afEntry[8]*rkDiag[2]);
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType> Matrix3<RealType>::diagonalTimes (const Vector3<RealType>& rkDiag) const
{
    return Matrix3(
        rkDiag[0]*m_afEntry[0],rkDiag[0]*m_afEntry[1],rkDiag[0]*m_afEntry[2],
        rkDiag[1]*m_afEntry[3],rkDiag[1]*m_afEntry[4],rkDiag[1]*m_afEntry[5],
        rkDiag[2]*m_afEntry[6],rkDiag[2]*m_afEntry[7],rkDiag[2]*m_afEntry[8]);
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix3<RealType>::toAxisAngle (Vector3<RealType>& rkAxis, RealType& rfAngle) const
{
    // Let (x,y,z) be the unit-length axis and let A be an angle of orientation.
    // The orientation matrix is R = I + sin(A)*P + (1-cos(A))*P^2 where
    // I is the identity and
    //
    //       +-        -+
    //   P = |  0 -z +y |
    //       | +z  0 -x |
    //       | -y +x  0 |
    //       +-        -+
    //
    // If A > 0, R represents a counterclockwise orientation about the axis in
    // the sense of looking from the tip of the axis vector towards the
    // origin.  Some algebra will show that
    //
    //   cos(A) = (trace(R)-1)/2  and  R - R^t = 2*sin(A)*P
    //
    // In the event that A = pi, R-R^t = 0 which prevents us from extracting
    // the axis through P.  Instead note that R = I+2*P^2 when A = pi, so
    // P^2 = (R-I)/2.  The diagonal entries of P^2 are x^2-1, y^2-1, and
    // z^2-1.  We can solve these for axis (x,y,z).  Because the angle is pi,
    // it does not matter which sign you choose on the square roots.

    RealType fTrace = m_afEntry[0] + m_afEntry[4] + m_afEntry[8];
    RealType fCos = ((RealType)0.5)*(fTrace-(RealType)1.0);
    rfAngle = Math<RealType>::ACos(fCos);  // in [0,PI]

    if ( rfAngle > (RealType)0.0 )
    {
        if ( rfAngle < Math<RealType>::PI )
        {
            rkAxis[0] = m_afEntry[7]-m_afEntry[5];
            rkAxis[1] = m_afEntry[2]-m_afEntry[6];
            rkAxis[2] = m_afEntry[3]-m_afEntry[1];
            rkAxis.Normalize();
        }
        else
        {
            // angle is PI
            RealType fHalfInverse;
            if ( m_afEntry[0] >= m_afEntry[4] )
            {
                // r00 >= r11
                if ( m_afEntry[0] >= m_afEntry[8] )
                {
                    // r00 is maximum diagonal term
                    rkAxis[0] = ((RealType)0.5)*Math<RealType>::Sqrt(m_afEntry[0] -
                        m_afEntry[4] - m_afEntry[8] + (RealType)1.0);
                    fHalfInverse = ((RealType)0.5)/rkAxis[0];
                    rkAxis[1] = fHalfInverse*m_afEntry[1];
                    rkAxis[2] = fHalfInverse*m_afEntry[2];
                }
                else
                {
                    // r22 is maximum diagonal term
                    rkAxis[2] = ((RealType)0.5)*Math<RealType>::Sqrt(m_afEntry[8] -
                        m_afEntry[0] - m_afEntry[4] + (RealType)1.0);
                    fHalfInverse = ((RealType)0.5)/rkAxis[2];
                    rkAxis[0] = fHalfInverse*m_afEntry[2];
                    rkAxis[1] = fHalfInverse*m_afEntry[5];
                }
            }
            else
            {
                // r11 > r00
                if ( m_afEntry[4] >= m_afEntry[8] )
                {
                    // r11 is maximum diagonal term
                    rkAxis[1] = ((RealType)0.5)*Math<RealType>::Sqrt(m_afEntry[4] -
                        m_afEntry[0] - m_afEntry[8] + (RealType)1.0);
                    fHalfInverse  = ((RealType)0.5)/rkAxis[1];
                    rkAxis[0] = fHalfInverse*m_afEntry[1];
                    rkAxis[2] = fHalfInverse*m_afEntry[5];
                }
                else
                {
                    // r22 is maximum diagonal term
                    rkAxis[2] = ((RealType)0.5)*Math<RealType>::Sqrt(m_afEntry[8] -
                        m_afEntry[0] - m_afEntry[4] + (RealType)1.0);
                    fHalfInverse = ((RealType)0.5)/rkAxis[2];
                    rkAxis[0] = fHalfInverse*m_afEntry[2];
                    rkAxis[1] = fHalfInverse*m_afEntry[5];
                }
            }
        }
    }
    else
    {
        // The angle is 0 and the matrix is the identity.  Any axis will
        // work, so just use the x-axis.
        rkAxis[0] = (RealType)1.0;
        rkAxis[1] = (RealType)0.0;
        rkAxis[2] = (RealType)0.0;
    }
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix3<RealType>::orthonormalize ()
{
    // Algorithm uses Gram-Schmidt orthogonalization.  If 'this' matrix is
    // M = [m0|m1|m2], then orthonormal output matrix is Q = [q0|q1|q2],
    //
    //   q0 = m0/|m0|
    //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
    //   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
    //
    // where |V| indicates length of vector V and A*B indicates dot
    // product of vectors A and B.

    // compute q0
    RealType fInvLength = Math<RealType>::invSqrt(m_afEntry[0]*m_afEntry[0] +
        m_afEntry[3]*m_afEntry[3] + m_afEntry[6]*m_afEntry[6]);

    m_afEntry[0] *= fInvLength;
    m_afEntry[3] *= fInvLength;
    m_afEntry[6] *= fInvLength;

    // compute q1
    RealType fDot0 = m_afEntry[0]*m_afEntry[1] + m_afEntry[3]*m_afEntry[4] +
        m_afEntry[6]*m_afEntry[7];

    m_afEntry[1] -= fDot0*m_afEntry[0];
    m_afEntry[4] -= fDot0*m_afEntry[3];
    m_afEntry[7] -= fDot0*m_afEntry[6];

    fInvLength = Math<RealType>::invSqrt(m_afEntry[1]*m_afEntry[1] +
        m_afEntry[4]*m_afEntry[4] + m_afEntry[7]*m_afEntry[7]);

    m_afEntry[1] *= fInvLength;
    m_afEntry[4] *= fInvLength;
    m_afEntry[7] *= fInvLength;

    // compute q2
    RealType fDot1 = m_afEntry[1]*m_afEntry[2] + m_afEntry[4]*m_afEntry[5] +
        m_afEntry[7]*m_afEntry[8];

    fDot0 = m_afEntry[0]*m_afEntry[2] + m_afEntry[3]*m_afEntry[5] +
        m_afEntry[6]*m_afEntry[8];

    m_afEntry[2] -= fDot0*m_afEntry[0] + fDot1*m_afEntry[1];
    m_afEntry[5] -= fDot0*m_afEntry[3] + fDot1*m_afEntry[4];
    m_afEntry[8] -= fDot0*m_afEntry[6] + fDot1*m_afEntry[7];

    fInvLength = Math<RealType>::invSqrt(m_afEntry[2]*m_afEntry[2] +
        m_afEntry[5]*m_afEntry[5] + m_afEntry[8]*m_afEntry[8]);

    m_afEntry[2] *= fInvLength;
    m_afEntry[5] *= fInvLength;
    m_afEntry[8] *= fInvLength;
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix3<RealType>::eigenDecomposition (Matrix3& rkRot, Matrix3& rkDiag) const
{
    // Factor M = R*D*R^T.  The columns of R are the eigenvectors.  The
    // diagonal entries of D are the corresponding eigenvalues.
    RealType afDiag[3], afSubd[2];
    rkRot = *this;
    bool bReflection = rkRot.tridiagonalize(afDiag,afSubd);
    bool bConverged = rkRot.QLAlgorithm(afDiag,afSubd);
    assert( bConverged );

    // (insertion) sort eigenvalues in increasing order, d0 <= d1 <= d2
    int i;
    RealType fSave;

    if ( afDiag[1] < afDiag[0] )
    {
        // swap d0 and d1
        fSave = afDiag[0];
        afDiag[0] = afDiag[1];
        afDiag[1] = fSave;

        // swap V0 and V1
        for (i = 0; i < 3; i++)
        {
            fSave = rkRot[i][0];
            rkRot[i][0] = rkRot[i][1];
            rkRot[i][1] = fSave;
        }
        bReflection = !bReflection;
    }

    if ( afDiag[2] < afDiag[1] )
    {
        // swap d1 and d2
        fSave = afDiag[1];
        afDiag[1] = afDiag[2];
        afDiag[2] = fSave;

        // swap V1 and V2
        for (i = 0; i < 3; i++)
        {
            fSave = rkRot[i][1];
            rkRot[i][1] = rkRot[i][2];
            rkRot[i][2] = fSave;
        }
        bReflection = !bReflection;
    }

    if ( afDiag[1] < afDiag[0] )
    {
        // swap d0 and d1
        fSave = afDiag[0];
        afDiag[0] = afDiag[1];
        afDiag[1] = fSave;

        // swap V0 and V1
        for (i = 0; i < 3; i++)
        {
            fSave = rkRot[i][0];
            rkRot[i][0] = rkRot[i][1];
            rkRot[i][1] = fSave;
        }
        bReflection = !bReflection;
    }

    rkDiag.makeDiagonal(afDiag[0],afDiag[1],afDiag[2]);

    if ( bReflection )
    {
        // The orthogonal transformation that diagonalizes M is a reflection.
        // Make the eigenvectors a right--handed system by changing sign on
        // the last column.
        rkRot[0][2] = -rkRot[0][2];
        rkRot[1][2] = -rkRot[1][2];
        rkRot[2][2] = -rkRot[2][2];
    }
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>& Matrix3<RealType>::fromEulerAnglesXYZ (RealType fYAngle, RealType fPAngle,
    RealType fRAngle)
{
    RealType fCos, fSin;

    fCos = Math<RealType>::Cos(fYAngle);
    fSin = Math<RealType>::Sin(fYAngle);
    Matrix3 kXMat(
        (RealType)1.0,(RealType)0.0,(RealType)0.0,
        (RealType)0.0,fCos,-fSin,
        (RealType)0.0,fSin,fCos);

    fCos = Math<RealType>::Cos(fPAngle);
    fSin = Math<RealType>::Sin(fPAngle);
    Matrix3 kYMat(
        fCos,(RealType)0.0,fSin,
        (RealType)0.0,(RealType)1.0,(RealType)0.0,
        -fSin,(RealType)0.0,fCos);

    fCos = Math<RealType>::Cos(fRAngle);
    fSin = Math<RealType>::Sin(fRAngle);
    Matrix3 kZMat(
        fCos,-fSin,(RealType)0.0,
        fSin,fCos,(RealType)0.0,
        (RealType)0.0,(RealType)0.0,(RealType)1.0);

    *this = kXMat*(kYMat*kZMat);
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>& Matrix3<RealType>::fromEulerAnglesXZY (RealType fYAngle, RealType fPAngle,
    RealType fRAngle)
{
    RealType fCos, fSin;

    fCos = Math<RealType>::Cos(fYAngle);
    fSin = Math<RealType>::Sin(fYAngle);
    Matrix3 kXMat(
        (RealType)1.0,(RealType)0.0,(RealType)0.0,
        (RealType)0.0,fCos,-fSin,
        (RealType)0.0,fSin,fCos);

    fCos = Math<RealType>::Cos(fPAngle);
    fSin = Math<RealType>::Sin(fPAngle);
    Matrix3 kZMat(
        fCos,-fSin,(RealType)0.0,
        fSin,fCos,(RealType)0.0,
        (RealType)0.0,(RealType)0.0,(RealType)1.0);

    fCos = Math<RealType>::Cos(fRAngle);
    fSin = Math<RealType>::Sin(fRAngle);
    Matrix3 kYMat(
        fCos,(RealType)0.0,fSin,
        (RealType)0.0,(RealType)1.0,(RealType)0.0,
        -fSin,(RealType)0.0,fCos);

    *this = kXMat*(kZMat*kYMat);
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>& Matrix3<RealType>::fromEulerAnglesYXZ (RealType fYAngle, RealType fPAngle,
    RealType fRAngle)
{
    RealType fCos, fSin;

    fCos = Math<RealType>::Cos(fYAngle);
    fSin = Math<RealType>::Sin(fYAngle);
    Matrix3 kYMat(
        fCos,(RealType)0.0,fSin,
        (RealType)0.0,(RealType)1.0,(RealType)0.0,
        -fSin,(RealType)0.0,fCos);

    fCos = Math<RealType>::Cos(fPAngle);
    fSin = Math<RealType>::Sin(fPAngle);
    Matrix3 kXMat(
        (RealType)1.0,(RealType)0.0,(RealType)0.0,
        (RealType)0.0,fCos,-fSin,
        (RealType)0.0,fSin,fCos);

    fCos = Math<RealType>::Cos(fRAngle);
    fSin = Math<RealType>::Sin(fRAngle);
    Matrix3 kZMat(
        fCos,-fSin,(RealType)0.0,
        fSin,fCos,(RealType)0.0,
        (RealType)0.0,(RealType)0.0,(RealType)1.0);

    *this = kYMat*(kXMat*kZMat);
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>& Matrix3<RealType>::fromEulerAnglesYZX (RealType fYAngle, RealType fPAngle,
    RealType fRAngle)
{
    RealType fCos, fSin;

    fCos = Math<RealType>::Cos(fYAngle);
    fSin = Math<RealType>::Sin(fYAngle);
    Matrix3 kYMat(
        fCos,(RealType)0.0,fSin,
        (RealType)0.0,(RealType)1.0,(RealType)0.0,
        -fSin,(RealType)0.0,fCos);

    fCos = Math<RealType>::Cos(fPAngle);
    fSin = Math<RealType>::Sin(fPAngle);
    Matrix3 kZMat(
        fCos,-fSin,(RealType)0.0,
        fSin,fCos,(RealType)0.0,
        (RealType)0.0,(RealType)0.0,(RealType)1.0);

    fCos = Math<RealType>::Cos(fRAngle);
    fSin = Math<RealType>::Sin(fRAngle);
    Matrix3 kXMat(
        (RealType)1.0,(RealType)0.0,(RealType)0.0,
        (RealType)0.0,fCos,-fSin,
        (RealType)0.0,fSin,fCos);

    *this = kYMat*(kZMat*kXMat);
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>& Matrix3<RealType>::fromEulerAnglesZXY (RealType fYAngle, RealType fPAngle,
    RealType fRAngle)
{
    RealType fCos, fSin;

    fCos = Math<RealType>::Cos(fYAngle);
    fSin = Math<RealType>::Sin(fYAngle);
    Matrix3 kZMat(
        fCos,-fSin,(RealType)0.0,
        fSin,fCos,(RealType)0.0,
        (RealType)0.0,(RealType)0.0,(RealType)1.0);

    fCos = Math<RealType>::Cos(fPAngle);
    fSin = Math<RealType>::Sin(fPAngle);
    Matrix3 kXMat(
        (RealType)1.0,(RealType)0.0,(RealType)0.0,
        (RealType)0.0,fCos,-fSin,
        (RealType)0.0,fSin,fCos);

    fCos = Math<RealType>::Cos(fRAngle);
    fSin = Math<RealType>::Sin(fRAngle);
    Matrix3 kYMat(
        fCos,(RealType)0.0,fSin,
        (RealType)0.0,(RealType)1.0,(RealType)0.0,
        -fSin,(RealType)0.0,fCos);

    *this = kZMat*(kXMat*kYMat);
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType>& Matrix3<RealType>::fromEulerAnglesZYX (RealType fYAngle, RealType fPAngle,
    RealType fRAngle)
{
    RealType fCos, fSin;

    fCos = Math<RealType>::Cos(fYAngle);
    fSin = Math<RealType>::Sin(fYAngle);
    Matrix3 kZMat(
        fCos,-fSin,(RealType)0.0,
        fSin,fCos,(RealType)0.0,
        (RealType)0.0,(RealType)0.0,(RealType)1.0);

    fCos = Math<RealType>::Cos(fPAngle);
    fSin = Math<RealType>::Sin(fPAngle);
    Matrix3 kYMat(
        fCos,(RealType)0.0,fSin,
        (RealType)0.0,(RealType)1.0,(RealType)0.0,
        -fSin,(RealType)0.0,fCos);

    fCos = Math<RealType>::Cos(fRAngle);
    fSin = Math<RealType>::Sin(fRAngle);
    Matrix3 kXMat(
        (RealType)1.0,(RealType)0.0,(RealType)0.0,
        (RealType)0.0,fCos,-fSin,
        (RealType)0.0,fSin,fCos);

    *this = kZMat*(kYMat*kXMat);
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix3<RealType>::toEulerAnglesXYZ (RealType& rfXAngle, RealType& rfYAngle,
    RealType& rfZAngle) const
{
    // rot =  cy*cz          -cy*sz           sy
    //        cz*sx*sy+cx*sz  cx*cz-sx*sy*sz -cy*sx
    //       -cx*cz*sy+sx*sz  cz*sx+cx*sy*sz  cx*cy

    if ( m_afEntry[2] < (RealType)1.0 )
    {
        if ( m_afEntry[2] > -(RealType)1.0 )
        {
            rfXAngle = Math<RealType>::ATan2(-m_afEntry[5],m_afEntry[8]);
            rfYAngle = (RealType)asin((double)m_afEntry[2]);
            rfZAngle = Math<RealType>::ATan2(-m_afEntry[1],m_afEntry[0]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  XA - ZA = -atan2(r10,r11)
            rfXAngle = -Math<RealType>::ATan2(m_afEntry[3],m_afEntry[4]);
            rfYAngle = -Math<RealType>::HALF_PI;
            rfZAngle = (RealType)0.0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  XAngle + ZAngle = atan2(r10,r11)
        rfXAngle = Math<RealType>::ATan2(m_afEntry[3],m_afEntry[4]);
        rfYAngle = Math<RealType>::HALF_PI;
        rfZAngle = (RealType)0.0;
        return false;
    }
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix3<RealType>::toEulerAnglesXZY (RealType& rfXAngle, RealType& rfZAngle,
    RealType& rfYAngle) const
{
    // rot =  cy*cz          -sz              cz*sy
    //        sx*sy+cx*cy*sz  cx*cz          -cy*sx+cx*sy*sz
    //       -cx*sy+cy*sx*sz  cz*sx           cx*cy+sx*sy*sz

    if ( m_afEntry[1] < (RealType)1.0 )
    {
        if ( m_afEntry[1] > -(RealType)1.0 )
        {
            rfXAngle = Math<RealType>::ATan2(m_afEntry[7],m_afEntry[4]);
            rfZAngle = (RealType)asin(-(double)m_afEntry[1]);
            rfYAngle = Math<RealType>::ATan2(m_afEntry[2],m_afEntry[0]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  XA - YA = atan2(r20,r22)
            rfXAngle = Math<RealType>::ATan2(m_afEntry[6],m_afEntry[8]);
            rfZAngle = Math<RealType>::HALF_PI;
            rfYAngle = (RealType)0.0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  XA + YA = atan2(-r20,r22)
        rfXAngle = Math<RealType>::ATan2(-m_afEntry[6],m_afEntry[8]);
        rfZAngle = -Math<RealType>::HALF_PI;
        rfYAngle = (RealType)0.0;
        return false;
    }
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix3<RealType>::toEulerAnglesYXZ (RealType& rfYAngle, RealType& rfXAngle,
    RealType& rfZAngle) const
{
    // rot =  cy*cz+sx*sy*sz  cz*sx*sy-cy*sz  cx*sy
    //        cx*sz           cx*cz          -sx
    //       -cz*sy+cy*sx*sz  cy*cz*sx+sy*sz  cx*cy

    if ( m_afEntry[5] < (RealType)1.0 )
    {
        if ( m_afEntry[5] > -(RealType)1.0 )
        {
            rfYAngle = Math<RealType>::ATan2(m_afEntry[2],m_afEntry[8]);
            rfXAngle = (RealType)asin(-(double)m_afEntry[5]);
            rfZAngle = Math<RealType>::ATan2(m_afEntry[3],m_afEntry[4]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  YA - ZA = atan2(r01,r00)
            rfYAngle = Math<RealType>::ATan2(m_afEntry[1],m_afEntry[0]);
            rfXAngle = Math<RealType>::HALF_PI;
            rfZAngle = (RealType)0.0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  YA + ZA = atan2(-r01,r00)
        rfYAngle = Math<RealType>::ATan2(-m_afEntry[1],m_afEntry[0]);
        rfXAngle = -Math<RealType>::HALF_PI;
        rfZAngle = (RealType)0.0;
        return false;
    }
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix3<RealType>::toEulerAnglesYZX (RealType& rfYAngle, RealType& rfZAngle,
    RealType& rfXAngle) const
{
    // rot =  cy*cz           sx*sy-cx*cy*sz  cx*sy+cy*sx*sz
    //        sz              cx*cz          -cz*sx
    //       -cz*sy           cy*sx+cx*sy*sz  cx*cy-sx*sy*sz

    if ( m_afEntry[3] < (RealType)1.0 )
    {
        if ( m_afEntry[3] > -(RealType)1.0 )
        {
            rfYAngle = Math<RealType>::ATan2(-m_afEntry[6],m_afEntry[0]);
            rfZAngle = (RealType)asin((double)m_afEntry[3]);
            rfXAngle = Math<RealType>::ATan2(-m_afEntry[5],m_afEntry[4]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  YA - XA = -atan2(r21,r22);
            rfYAngle = -Math<RealType>::ATan2(m_afEntry[7],m_afEntry[8]);
            rfZAngle = -Math<RealType>::HALF_PI;
            rfXAngle = (RealType)0.0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  YA + XA = atan2(r21,r22)
        rfYAngle = Math<RealType>::ATan2(m_afEntry[7],m_afEntry[8]);
        rfZAngle = Math<RealType>::HALF_PI;
        rfXAngle = (RealType)0.0;
        return false;
    }
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix3<RealType>::toEulerAnglesZXY (RealType& rfZAngle, RealType& rfXAngle,
    RealType& rfYAngle) const
{
    // rot =  cy*cz-sx*sy*sz -cx*sz           cz*sy+cy*sx*sz
    //        cz*sx*sy+cy*sz  cx*cz          -cy*cz*sx+sy*sz
    //       -cx*sy           sx              cx*cy

    if ( m_afEntry[7] < (RealType)1.0 )
    {
        if ( m_afEntry[7] > -(RealType)1.0 )
        {
            rfZAngle = Math<RealType>::ATan2(-m_afEntry[1],m_afEntry[4]);
            rfXAngle = (RealType)asin((double)m_afEntry[7]);
            rfYAngle = Math<RealType>::ATan2(-m_afEntry[6],m_afEntry[8]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  ZA - YA = -atan(r02,r00)
            rfZAngle = -Math<RealType>::ATan2(m_afEntry[2],m_afEntry[0]);
            rfXAngle = -Math<RealType>::HALF_PI;
            rfYAngle = (RealType)0.0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  ZA + YA = atan2(r02,r00)
        rfZAngle = Math<RealType>::ATan2(m_afEntry[2],m_afEntry[0]);
        rfXAngle = Math<RealType>::HALF_PI;
        rfYAngle = (RealType)0.0;
        return false;
    }
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix3<RealType>::toEulerAnglesZYX (RealType& rfZAngle, RealType& rfYAngle,
    RealType& rfXAngle) const
{
    // rot =  cy*cz           cz*sx*sy-cx*sz  cx*cz*sy+sx*sz
    //        cy*sz           cx*cz+sx*sy*sz -cz*sx+cx*sy*sz
    //       -sy              cy*sx           cx*cy

    if ( m_afEntry[6] < (RealType)1.0 )
    {
        if ( m_afEntry[6] > -(RealType)1.0 )
        {
            rfZAngle = Math<RealType>::ATan2(m_afEntry[3],m_afEntry[0]);
            rfYAngle = (RealType)asin(-(double)m_afEntry[6]);
            rfXAngle = Math<RealType>::ATan2(m_afEntry[7],m_afEntry[8]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  ZA - XA = -atan2(r01,r02)
            rfZAngle = -Math<RealType>::ATan2(m_afEntry[1],m_afEntry[2]);
            rfYAngle = Math<RealType>::HALF_PI;
            rfXAngle = (RealType)0.0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  ZA + XA = atan2(-r01,-r02)
        rfZAngle = Math<RealType>::ATan2(-m_afEntry[1],-m_afEntry[2]);
        rfYAngle = -Math<RealType>::HALF_PI;
        rfXAngle = (RealType)0.0;
        return false;
    }
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType> Matrix3<RealType>::slerp (RealType fT, const Matrix3& rkR0,
    const Matrix3& rkR1)
{
    Vector3<RealType> kAxis;
    RealType fAngle;
    Matrix3 kProd = rkR0.TransposeTimes(rkR1);
    kProd.ToAxisAngle(kAxis,fAngle);
    return Matrix3(kAxis,fT*fAngle);
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix3<RealType>::tridiagonalize (RealType afDiag[3], RealType afSubd[2])
{
    // Householder reduction T = Q^t M Q
    //   Input:   
    //     mat, symmetric 3x3 matrix M
    //   Output:  
    //     mat, orthogonal matrix Q (a reflection)
    //     diag, diagonal entries of T
    //     subd, subdiagonal entries of T (T is symmetric)

    RealType fM00 = m_afEntry[0];
    RealType fM01 = m_afEntry[1];
    RealType fM02 = m_afEntry[2];
    RealType fM11 = m_afEntry[4];
    RealType fM12 = m_afEntry[5];
    RealType fM22 = m_afEntry[8];

    afDiag[0] = fM00;
    if ( Math<RealType>::fAbs(fM02) >= Math<RealType>::ZERO_TOLERANCE )
    {
        afSubd[0] = Math<RealType>::sqRoot(fM01*fM01+fM02*fM02);
        RealType fInvLength = ((RealType)1.0)/afSubd[0];
        fM01 *= fInvLength;
        fM02 *= fInvLength;
        RealType fTmp = ((RealType)2.0)*fM01*fM12+fM02*(fM22-fM11);
        afDiag[1] = fM11+fM02*fTmp;
        afDiag[2] = fM22-fM02*fTmp;
        afSubd[1] = fM12-fM01*fTmp;

        m_afEntry[0] = (RealType)1.0;
        m_afEntry[1] = (RealType)0.0;
        m_afEntry[2] = (RealType)0.0;
        m_afEntry[3] = (RealType)0.0;
        m_afEntry[4] = fM01;
        m_afEntry[5] = fM02;
        m_afEntry[6] = (RealType)0.0;
        m_afEntry[7] = fM02;
        m_afEntry[8] = -fM01;
        return true;
    }
    else
    {
        afDiag[1] = fM11;
        afDiag[2] = fM22;
        afSubd[0] = fM01;
        afSubd[1] = fM12;

        m_afEntry[0] = (RealType)1.0;
        m_afEntry[1] = (RealType)0.0;
        m_afEntry[2] = (RealType)0.0;
        m_afEntry[3] = (RealType)0.0;
        m_afEntry[4] = (RealType)1.0;
        m_afEntry[5] = (RealType)0.0;
        m_afEntry[6] = (RealType)0.0;
        m_afEntry[7] = (RealType)0.0;
        m_afEntry[8] = (RealType)1.0;
        return false;
    }
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix3<RealType>::QLAlgorithm (RealType afDiag[3], RealType afSubd[2])
{
    // This is an implementation of the symmetric QR algorithm from the book
    // "Matrix Computations" by Gene H. Golub and Charles F. Van Loan, second
    // edition.  The algorithm is 8.2.3.  The implementation has a slight
    // variation to actually make it a QL algorithm, and it traps the case
    // when either of the subdiagonal terms s0 or s1 is zero and reduces the
    // 2-by-2 subblock directly.

    const int iMax = 32;
    for (int i = 0; i < iMax; i++)
    {
        float fSum, fDiff, fDiscr, fEValue0, fEValue1, fCos, fSin, fTmp;
        int iRow;

        fSum = Mathf::fAbs(afDiag[0]) + Mathf::fAbs(afDiag[1]);
        if ( Mathf::fAbs(afSubd[0]) + fSum == fSum )
        {
            // The matrix is effectively
            //       +-        -+
            //   M = | d0  0  0 |
            //       | 0  d1 s1 |
            //       | 0  s1 d2 |
            //       +-        -+

            // Compute the eigenvalues as roots of a quadratic equation.
            fSum = afDiag[1] + afDiag[2];
            fDiff = afDiag[1] - afDiag[2];
            fDiscr = Mathf::sqRoot(fDiff*fDiff + 4.0f*afSubd[1]*afSubd[1]);
            fEValue0 = 0.5f*(fSum - fDiscr);
            fEValue1 = 0.5f*(fSum + fDiscr);

            // Compute the Givens orientation.
            if ( fDiff >= 0.0f )
            {
                fCos = afSubd[1];
                fSin = afDiag[1] - fEValue0;
            }
            else
            {
                fCos = afDiag[2] - fEValue0;
                fSin = afSubd[1];
            }
            fTmp = Mathf::invSqRoot(fCos*fCos + fSin*fSin);
            fCos *= fTmp;
            fSin *= fTmp;

            // Postmultiply the current orthogonal matrix with the Givens
            // orientation.
            for (iRow = 0; iRow < 3; iRow++)
            {
                fTmp = m_afEntry[I(iRow,2)];
                m_afEntry[I(iRow,2)] = fSin*m_afEntry[I(iRow,1)] + fCos*fTmp;
                m_afEntry[I(iRow,1)] = fCos*m_afEntry[I(iRow,1)] - fSin*fTmp;
            }

            // Update the tridiagonal matrix.
            afDiag[1] = fEValue0;
            afDiag[2] = fEValue1;
            afSubd[0] = 0.0f;
            afSubd[1] = 0.0f;
            return true;
        }

        fSum = Mathf::fAbs(afDiag[1]) + Mathf::fAbs(afDiag[2]);
        if ( Mathf::fAbs(afSubd[1]) + fSum == fSum )
        {
            // The matrix is effectively
            //       +-         -+
            //   M = | d0  s0  0 |
            //       | s0  d1  0 |
            //       | 0   0  d2 |
            //       +-         -+

            // Compute the eigenvalues as roots of a quadratic equation.
            fSum = afDiag[0] + afDiag[1];
            fDiff = afDiag[0] - afDiag[1];
            fDiscr = Mathf::sqRoot(fDiff*fDiff + 4.0f*afSubd[0]*afSubd[0]);
            fEValue0 = 0.5f*(fSum - fDiscr);
            fEValue1 = 0.5f*(fSum + fDiscr);

            // Compute the Givens orientation.
            if ( fDiff >= 0.0f )
            {
                fCos = afSubd[0];
                fSin = afDiag[0] - fEValue0;
            }
            else
            {
                fCos = afDiag[1] - fEValue0;
                fSin = afSubd[0];
            }
            fTmp = Mathf::invSqRoot(fCos*fCos + fSin*fSin);
            fCos *= fTmp;
            fSin *= fTmp;

            // Postmultiply the current orthogonal matrix with the Givens
            // orientation.
            for (iRow = 0; iRow < 3; iRow++)
            {
                fTmp = m_afEntry[I(iRow,1)];
                m_afEntry[I(iRow,1)] = fSin*m_afEntry[I(iRow,0)] + fCos*fTmp;
                m_afEntry[I(iRow,0)] = fCos*m_afEntry[I(iRow,0)] - fSin*fTmp;
            }

            // Update the tridiagonal matrix.
            afDiag[0] = fEValue0;
            afDiag[1] = fEValue1;
            afSubd[0] = 0.0f;
            afSubd[1] = 0.0f;
            return true;
        }

        // The matrix is
        //       +-        -+
        //   M = | d0 s0  0 |
        //       | s0 d1 s1 |
        //       | 0  s1 d2 |
        //       +-        -+

        // Set up the parameters for the first pass of the QL step.  The
        // value for A is the difference between diagonal term D[2] and the
        // implicit shift suggested by Wilkinson.
        float fRatio = (afDiag[1]-afDiag[0])/(2.0f*afSubd[0]);
        float fRoot = Mathf::sqRoot(1.0f + fRatio*fRatio);
        float fB = afSubd[1];
        float fA = afDiag[2] - afDiag[0];
        if ( fRatio >= 0.0f )
            fA += afSubd[0]/(fRatio + fRoot);
        else
            fA += afSubd[0]/(fRatio - fRoot);

        // Compute the Givens orientation for the first pass.
        if ( Mathf::fAbs(fB) >= Mathf::fAbs(fA) )
        {
            fRatio = fA/fB;
            fSin = Mathf::invSqRoot(1.0f + fRatio*fRatio);
            fCos = fRatio*fSin;
        }
        else
        {
            fRatio = fB/fA;
            fCos = Mathf::invSqRoot(1.0f + fRatio*fRatio);
            fSin = fRatio*fCos;
        }

        // Postmultiply the current orthogonal matrix with the Givens
        // orientation.
        for (iRow = 0; iRow < 3; iRow++)
        {
            fTmp = m_afEntry[I(iRow,2)];
            m_afEntry[I(iRow,2)] = fSin*m_afEntry[I(iRow,1)]+fCos*fTmp;
            m_afEntry[I(iRow,1)] = fCos*m_afEntry[I(iRow,1)]-fSin*fTmp;
        }

        // Set up the parameters for the second pass of the QL step.  The
        // values tmp0 and tmp1 are required to fully update the tridiagonal
        // matrix at the end of the second pass.
        float fTmp0 = (afDiag[1] - afDiag[2])*fSin + 2.0f*afSubd[1]*fCos;
        float fTmp1 = fCos*afSubd[0];
        fB = fSin*afSubd[0];
        fA = fCos*fTmp0 - afSubd[1];
        fTmp0 *= fSin;

        // Compute the Givens orientation for the second pass.  The subdiagonal
        // term S[1] in the tridiagonal matrix is updated at this time.
        if ( Mathf::fAbs(fB) >= Mathf::fAbs(fA) )
        {
            fRatio = fA/fB;
            fRoot = Mathf::sqRoot(1.0f + fRatio*fRatio);
            afSubd[1] = fB*fRoot;
            fSin = 1.0f/fRoot;
            fCos = fRatio*fSin;
        }
        else
        {
            fRatio = fB/fA;
            fRoot = Mathf::sqRoot(1.0f + fRatio*fRatio);
            afSubd[1] = fA*fRoot;
            fCos = 1.0f/fRoot;
            fSin = fRatio*fCos;
        }

        // Postmultiply the current orthogonal matrix with the Givens
        // orientation.
        for (iRow = 0; iRow < 3; iRow++)
        {
            fTmp = m_afEntry[I(iRow,1)];
            m_afEntry[I(iRow,1)] = fSin*m_afEntry[I(iRow,0)]+fCos*fTmp;
            m_afEntry[I(iRow,0)] = fCos*m_afEntry[I(iRow,0)]-fSin*fTmp;
        }

        // Complete the update of the tridiagonal matrix.
        float fTmp2 = afDiag[1] - fTmp0;
        afDiag[2] += fTmp0;
        fTmp0 = (afDiag[0] - fTmp2)*fSin + 2.0f*fTmp1*fCos;
        afSubd[0] = fCos*fTmp0 - fTmp1;
        fTmp0 *= fSin;
        afDiag[1] = fTmp2 + fTmp0;
        afDiag[0] -= fTmp0;
    }
    return false;
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix3<RealType>::bidiagonalize (Matrix3& rkA, Matrix3& rkL, Matrix3& rkR)
{
    RealType afV[3], afW[3];
    RealType fLength, fSign, fT1, fInvT1, fT2;
    bool bIdentity;

    // map first column to (*,0,0)
    fLength = Math<RealType>::Sqrt(rkA[0][0]*rkA[0][0] + rkA[1][0]*rkA[1][0] +
        rkA[2][0]*rkA[2][0]);
    if ( fLength > (RealType)0.0 )
    {
        fSign = (rkA[0][0] > (RealType)0.0 ? (RealType)1.0 : -(RealType)1.0);
        fT1 = rkA[0][0] + fSign*fLength;
        fInvT1 = ((RealType)1.0)/fT1;
        afV[1] = rkA[1][0]*fInvT1;
        afV[2] = rkA[2][0]*fInvT1;

        fT2 = -((RealType)2.0)/(((RealType)1.0)+afV[1]*afV[1]+afV[2]*afV[2]);
        afW[0] = fT2*(rkA[0][0]+rkA[1][0]*afV[1]+rkA[2][0]*afV[2]);
        afW[1] = fT2*(rkA[0][1]+rkA[1][1]*afV[1]+rkA[2][1]*afV[2]);
        afW[2] = fT2*(rkA[0][2]+rkA[1][2]*afV[1]+rkA[2][2]*afV[2]);
        rkA[0][0] += afW[0];
        rkA[0][1] += afW[1];
        rkA[0][2] += afW[2];
        rkA[1][1] += afV[1]*afW[1];
        rkA[1][2] += afV[1]*afW[2];
        rkA[2][1] += afV[2]*afW[1];
        rkA[2][2] += afV[2]*afW[2];

        rkL[0][0] = ((RealType)1.0)+fT2;
        rkL[0][1] = fT2*afV[1];
        rkL[1][0] = rkL[0][1];
        rkL[0][2] = fT2*afV[2];
        rkL[2][0] = rkL[0][2];
        rkL[1][1] = ((RealType)1.0)+fT2*afV[1]*afV[1];
        rkL[1][2] = fT2*afV[1]*afV[2];
        rkL[2][1] = rkL[1][2];
        rkL[2][2] = ((RealType)1.0)+fT2*afV[2]*afV[2];
        bIdentity = false;
    }
    else
    {
        rkL = Matrix3::IDENTITY;
        bIdentity = true;
    }

    // map first row to (*,*,0)
    fLength = Math<RealType>::Sqrt(rkA[0][1]*rkA[0][1]+rkA[0][2]*rkA[0][2]);
    if ( fLength > (RealType)0.0 )
    {
        fSign = (rkA[0][1] > (RealType)0.0 ? (RealType)1.0 : -(RealType)1.0);
        fT1 = rkA[0][1] + fSign*fLength;
        afV[2] = rkA[0][2]/fT1;

        fT2 = -((RealType)2.0)/(((RealType)1.0)+afV[2]*afV[2]);
        afW[0] = fT2*(rkA[0][1]+rkA[0][2]*afV[2]);
        afW[1] = fT2*(rkA[1][1]+rkA[1][2]*afV[2]);
        afW[2] = fT2*(rkA[2][1]+rkA[2][2]*afV[2]);
        rkA[0][1] += afW[0];
        rkA[1][1] += afW[1];
        rkA[1][2] += afW[1]*afV[2];
        rkA[2][1] += afW[2];
        rkA[2][2] += afW[2]*afV[2];
        
        rkR[0][0] = (RealType)1.0;
        rkR[0][1] = (RealType)0.0;
        rkR[1][0] = (RealType)0.0;
        rkR[0][2] = (RealType)0.0;
        rkR[2][0] = (RealType)0.0;
        rkR[1][1] = ((RealType)1.0)+fT2;
        rkR[1][2] = fT2*afV[2];
        rkR[2][1] = rkR[1][2];
        rkR[2][2] = ((RealType)1.0)+fT2*afV[2]*afV[2];
    }
    else
    {
        rkR = Matrix3::IDENTITY;
    }

    // map second column to (*,*,0)
    fLength = Math<RealType>::Sqrt(rkA[1][1]*rkA[1][1]+rkA[2][1]*rkA[2][1]);
    if ( fLength > (RealType)0.0 )
    {
        fSign = (rkA[1][1] > (RealType)0.0 ? (RealType)1.0 : -(RealType)1.0);
        fT1 = rkA[1][1] + fSign*fLength;
        afV[2] = rkA[2][1]/fT1;

        fT2 = -((RealType)2.0)/(((RealType)1.0)+afV[2]*afV[2]);
        afW[1] = fT2*(rkA[1][1]+rkA[2][1]*afV[2]);
        afW[2] = fT2*(rkA[1][2]+rkA[2][2]*afV[2]);
        rkA[1][1] += afW[1];
        rkA[1][2] += afW[2];
        rkA[2][2] += afV[2]*afW[2];

        RealType fA = ((RealType)1.0)+fT2;
        RealType fB = fT2*afV[2];
        RealType fC = ((RealType)1.0)+fB*afV[2];

        if ( bIdentity )
        {
            rkL[0][0] = (RealType)1.0;
            rkL[0][1] = (RealType)0.0;
            rkL[1][0] = (RealType)0.0;
            rkL[0][2] = (RealType)0.0;
            rkL[2][0] = (RealType)0.0;
            rkL[1][1] = fA;
            rkL[1][2] = fB;
            rkL[2][1] = fB;
            rkL[2][2] = fC;
        }
        else
        {
            for (int iRow = 0; iRow < 3; iRow++)
            {
                RealType fTmp0 = rkL[iRow][1];
                RealType fTmp1 = rkL[iRow][2];
                rkL[iRow][1] = fA*fTmp0+fB*fTmp1;
                rkL[iRow][2] = fB*fTmp0+fC*fTmp1;
            }
        }
    }
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix3<RealType>::golubKahanStep (Matrix3& rkA, Matrix3& rkL, Matrix3& rkR)
{
    RealType fT11 = rkA[0][1]*rkA[0][1]+rkA[1][1]*rkA[1][1];
    RealType fT22 = rkA[1][2]*rkA[1][2]+rkA[2][2]*rkA[2][2];
    RealType fT12 = rkA[1][1]*rkA[1][2];
    RealType fTrace = fT11+fT22;
    RealType fDiff = fT11-fT22;
    RealType fDiscr = Math<RealType>::Sqrt(fDiff*fDiff+((RealType)4.0)*fT12*fT12);
    RealType fRoot1 = ((RealType)0.5)*(fTrace+fDiscr);
    RealType fRoot2 = ((RealType)0.5)*(fTrace-fDiscr);

    // adjust right
    RealType fY = rkA[0][0] - (Math<RealType>::fAbs(fRoot1-fT22) <=
        Math<RealType>::fAbs(fRoot2-fT22) ? fRoot1 : fRoot2);
    RealType fZ = rkA[0][1];
    RealType fInvLength = Math<RealType>::invSqrt(fY*fY+fZ*fZ);
    RealType fSin = fZ*fInvLength;
    RealType fCos = -fY*fInvLength;

    RealType fTmp0 = rkA[0][0];
    RealType fTmp1 = rkA[0][1];
    rkA[0][0] = fCos*fTmp0-fSin*fTmp1;
    rkA[0][1] = fSin*fTmp0+fCos*fTmp1;
    rkA[1][0] = -fSin*rkA[1][1];
    rkA[1][1] *= fCos;

    int iRow;
    for (iRow = 0; iRow < 3; iRow++)
    {
        fTmp0 = rkR[0][iRow];
        fTmp1 = rkR[1][iRow];
        rkR[0][iRow] = fCos*fTmp0-fSin*fTmp1;
        rkR[1][iRow] = fSin*fTmp0+fCos*fTmp1;
    }

    // adjust left
    fY = rkA[0][0];
    fZ = rkA[1][0];
    fInvLength = Math<RealType>::invSqrt(fY*fY+fZ*fZ);
    fSin = fZ*fInvLength;
    fCos = -fY*fInvLength;

    rkA[0][0] = fCos*rkA[0][0]-fSin*rkA[1][0];
    fTmp0 = rkA[0][1];
    fTmp1 = rkA[1][1];
    rkA[0][1] = fCos*fTmp0-fSin*fTmp1;
    rkA[1][1] = fSin*fTmp0+fCos*fTmp1;
    rkA[0][2] = -fSin*rkA[1][2];
    rkA[1][2] *= fCos;

    int iCol;
    for (iCol = 0; iCol < 3; iCol++)
    {
        fTmp0 = rkL[iCol][0];
        fTmp1 = rkL[iCol][1];
        rkL[iCol][0] = fCos*fTmp0-fSin*fTmp1;
        rkL[iCol][1] = fSin*fTmp0+fCos*fTmp1;
    }

    // adjust right
    fY = rkA[0][1];
    fZ = rkA[0][2];
    fInvLength = Math<RealType>::invSqrt(fY*fY+fZ*fZ);
    fSin = fZ*fInvLength;
    fCos = -fY*fInvLength;

    rkA[0][1] = fCos*rkA[0][1]-fSin*rkA[0][2];
    fTmp0 = rkA[1][1];
    fTmp1 = rkA[1][2];
    rkA[1][1] = fCos*fTmp0-fSin*fTmp1;
    rkA[1][2] = fSin*fTmp0+fCos*fTmp1;
    rkA[2][1] = -fSin*rkA[2][2];
    rkA[2][2] *= fCos;

    for (iRow = 0; iRow < 3; iRow++)
    {
        fTmp0 = rkR[1][iRow];
        fTmp1 = rkR[2][iRow];
        rkR[1][iRow] = fCos*fTmp0-fSin*fTmp1;
        rkR[2][iRow] = fSin*fTmp0+fCos*fTmp1;
    }

    // adjust left
    fY = rkA[1][1];
    fZ = rkA[2][1];
    fInvLength = Math<RealType>::invSqrt(fY*fY+fZ*fZ);
    fSin = fZ*fInvLength;
    fCos = -fY*fInvLength;

    rkA[1][1] = fCos*rkA[1][1]-fSin*rkA[2][1];
    fTmp0 = rkA[1][2];
    fTmp1 = rkA[2][2];
    rkA[1][2] = fCos*fTmp0-fSin*fTmp1;
    rkA[2][2] = fSin*fTmp0+fCos*fTmp1;

    for (iCol = 0; iCol < 3; iCol++)
    {
        fTmp0 = rkL[iCol][1];
        fTmp1 = rkL[iCol][2];
        rkL[iCol][1] = fCos*fTmp0-fSin*fTmp1;
        rkL[iCol][2] = fSin*fTmp0+fCos*fTmp1;
    }
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix3<RealType>::singularValueDecomposition (Matrix3& rkL, Matrix3& rkS,
    Matrix3& rkR) const
{
    int iRow, iCol;

    Matrix3 kA = *this;
    Bidiagonalize(kA,rkL,rkR);
    rkS.MakeZero();

    const int iMax = 32;
    const RealType fEpsilon = (RealType)1e-04;
    for (int i = 0; i < iMax; i++)
    {
        RealType fTmp, fTmp0, fTmp1;
        RealType fSin0, fCos0, fTan0;
        RealType fSin1, fCos1, fTan1;

        bool bTest1 = (Math<RealType>::fAbs(kA[0][1]) <=
            fEpsilon*(Math<RealType>::fAbs(kA[0][0]) +
            Math<RealType>::fAbs(kA[1][1])));
        bool bTest2 = (Math<RealType>::fAbs(kA[1][2]) <=
            fEpsilon*(Math<RealType>::fAbs(kA[1][1]) +
            Math<RealType>::fAbs(kA[2][2])));
        if ( bTest1 )
        {
            if ( bTest2 )
            {
                rkS[0][0] = kA[0][0];
                rkS[1][1] = kA[1][1];
                rkS[2][2] = kA[2][2];
                break;
            }
            else
            {
                // 2x2 closed form factorization
                fTmp = (kA[1][1]*kA[1][1] - kA[2][2]*kA[2][2] +
                    kA[1][2]*kA[1][2])/(kA[1][2]*kA[2][2]);
                fTan0 = ((RealType)0.5)*(fTmp + Math<RealType>::Sqrt(fTmp*fTmp +
                    ((RealType)4.0)));
                fCos0 = Math<RealType>::invSqrt(((RealType)1.0)+fTan0*fTan0);
                fSin0 = fTan0*fCos0;

                for (iCol = 0; iCol < 3; iCol++)
                {
                    fTmp0 = rkL[iCol][1];
                    fTmp1 = rkL[iCol][2];
                    rkL[iCol][1] = fCos0*fTmp0-fSin0*fTmp1;
                    rkL[iCol][2] = fSin0*fTmp0+fCos0*fTmp1;
                }
                
                fTan1 = (kA[1][2]-kA[2][2]*fTan0)/kA[1][1];
                fCos1 = Math<RealType>::invSqrt(((RealType)1.0)+fTan1*fTan1);
                fSin1 = -fTan1*fCos1;

                for (iRow = 0; iRow < 3; iRow++)
                {
                    fTmp0 = rkR[1][iRow];
                    fTmp1 = rkR[2][iRow];
                    rkR[1][iRow] = fCos1*fTmp0-fSin1*fTmp1;
                    rkR[2][iRow] = fSin1*fTmp0+fCos1*fTmp1;
                }

                rkS[0][0] = kA[0][0];
                rkS[1][1] = fCos0*fCos1*kA[1][1] -
                    fSin1*(fCos0*kA[1][2]-fSin0*kA[2][2]);
                rkS[2][2] = fSin0*fSin1*kA[1][1] +
                    fCos1*(fSin0*kA[1][2]+fCos0*kA[2][2]);
                break;
            }
        }
        else 
        {
            if ( bTest2 )
            {
                // 2x2 closed form factorization 
                fTmp = (kA[0][0]*kA[0][0] + kA[1][1]*kA[1][1] -
                    kA[0][1]*kA[0][1])/(kA[0][1]*kA[1][1]);
                fTan0 = ((RealType)0.5)*(-fTmp + Math<RealType>::Sqrt(fTmp*fTmp +
                    ((RealType)4.0)));
                fCos0 = Math<RealType>::invSqrt(((RealType)1.0)+fTan0*fTan0);
                fSin0 = fTan0*fCos0;

                for (iCol = 0; iCol < 3; iCol++)
                {
                    fTmp0 = rkL[iCol][0];
                    fTmp1 = rkL[iCol][1];
                    rkL[iCol][0] = fCos0*fTmp0-fSin0*fTmp1;
                    rkL[iCol][1] = fSin0*fTmp0+fCos0*fTmp1;
                }
                
                fTan1 = (kA[0][1]-kA[1][1]*fTan0)/kA[0][0];
                fCos1 = Math<RealType>::invSqrt(((RealType)1.0)+fTan1*fTan1);
                fSin1 = -fTan1*fCos1;

                for (iRow = 0; iRow < 3; iRow++)
                {
                    fTmp0 = rkR[0][iRow];
                    fTmp1 = rkR[1][iRow];
                    rkR[0][iRow] = fCos1*fTmp0-fSin1*fTmp1;
                    rkR[1][iRow] = fSin1*fTmp0+fCos1*fTmp1;
                }

                rkS[0][0] = fCos0*fCos1*kA[0][0] -
                    fSin1*(fCos0*kA[0][1]-fSin0*kA[1][1]);
                rkS[1][1] = fSin0*fSin1*kA[0][0] +
                    fCos1*(fSin0*kA[0][1]+fCos0*kA[1][1]);
                rkS[2][2] = kA[2][2];
                break;
            }
            else
            {
                GolubKahanStep(kA,rkL,rkR);
            }
        }
    }

    // positize diagonal
    for (iRow = 0; iRow < 3; iRow++)
    {
        if ( rkS[iRow][iRow] < (RealType)0.0 )
        {
            rkS[iRow][iRow] = -rkS[iRow][iRow];
            for (iCol = 0; iCol < 3; iCol++)
                rkR[iRow][iCol] = -rkR[iRow][iCol];
        }
    }
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix3<RealType>::singularValueComposition (const Matrix3& rkL,
    const Matrix3& rkS, const Matrix3& rkR)
{
    *this = rkL*(rkS*rkR);
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix3<RealType>::QDUDecomposition (Matrix3& rkQ, Matrix3& rkD,
    Matrix3& rkU) const
{
    // Factor M = QR = QDU where Q is orthogonal (orientation), D is diagonal
    // (scaling),  and U is upper triangular with ones on its diagonal
    // (shear).  Algorithm uses Gram-Schmidt orthogonalization (the QR
    // algorithm).
    //
    // If M = [ m0 | m1 | m2 ] and Q = [ q0 | q1 | q2 ], then
    //
    //   q0 = m0/|m0|
    //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
    //   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
    //
    // where |V| indicates length of vector V and A*B indicates dot
    // product of vectors A and B.  The matrix R has entries
    //
    //   r00 = q0*m0  r01 = q0*m1  r02 = q0*m2
    //   r10 = 0      r11 = q1*m1  r12 = q1*m2
    //   r20 = 0      r21 = 0      r22 = q2*m2
    //
    // so D = diag(r00,r11,r22) and U has entries u01 = r01/r00,
    // u02 = r02/r00, and u12 = r12/r11.

    // build orthogonal matrix Q
    RealType fInvLength = Math<RealType>::invSqrt(m_afEntry[0]*m_afEntry[0] +
        m_afEntry[3]*m_afEntry[3] + m_afEntry[6]*m_afEntry[6]);
    rkQ[0][0] = m_afEntry[0]*fInvLength;
    rkQ[1][0] = m_afEntry[3]*fInvLength;
    rkQ[2][0] = m_afEntry[6]*fInvLength;

    RealType fDot = rkQ[0][0]*m_afEntry[1] + rkQ[1][0]*m_afEntry[4] +
        rkQ[2][0]*m_afEntry[7];
    rkQ[0][1] = m_afEntry[1]-fDot*rkQ[0][0];
    rkQ[1][1] = m_afEntry[4]-fDot*rkQ[1][0];
    rkQ[2][1] = m_afEntry[7]-fDot*rkQ[2][0];
    fInvLength = Math<RealType>::invSqrt(rkQ[0][1]*rkQ[0][1] +
        rkQ[1][1]*rkQ[1][1] + rkQ[2][1]*rkQ[2][1]);
    rkQ[0][1] *= fInvLength;
    rkQ[1][1] *= fInvLength;
    rkQ[2][1] *= fInvLength;

    fDot = rkQ[0][0]*m_afEntry[2] + rkQ[1][0]*m_afEntry[5] +
        rkQ[2][0]*m_afEntry[8];
    rkQ[0][2] = m_afEntry[2]-fDot*rkQ[0][0];
    rkQ[1][2] = m_afEntry[5]-fDot*rkQ[1][0];
    rkQ[2][2] = m_afEntry[8]-fDot*rkQ[2][0];
    fDot = rkQ[0][1]*m_afEntry[2] + rkQ[1][1]*m_afEntry[5] +
        rkQ[2][1]*m_afEntry[8];
    rkQ[0][2] -= fDot*rkQ[0][1];
    rkQ[1][2] -= fDot*rkQ[1][1];
    rkQ[2][2] -= fDot*rkQ[2][1];
    fInvLength = Math<RealType>::invSqrt(rkQ[0][2]*rkQ[0][2] +
        rkQ[1][2]*rkQ[1][2] + rkQ[2][2]*rkQ[2][2]);
    rkQ[0][2] *= fInvLength;
    rkQ[1][2] *= fInvLength;
    rkQ[2][2] *= fInvLength;

    // guarantee that orthogonal matrix has determinant 1 (no reflections)
    RealType fDet = rkQ[0][0]*rkQ[1][1]*rkQ[2][2] + rkQ[0][1]*rkQ[1][2]*rkQ[2][0]
        +  rkQ[0][2]*rkQ[1][0]*rkQ[2][1] - rkQ[0][2]*rkQ[1][1]*rkQ[2][0]
        -  rkQ[0][1]*rkQ[1][0]*rkQ[2][2] - rkQ[0][0]*rkQ[1][2]*rkQ[2][1];

    if ( fDet < (RealType)0.0 )
    {
        for (int iRow = 0; iRow < 3; iRow++)
        {
            for (int iCol = 0; iCol < 3; iCol++)
                rkQ[iRow][iCol] = -rkQ[iRow][iCol];
        }
    }

    // build "right" matrix R
    Matrix3 kR;
    kR[0][0] = rkQ[0][0]*m_afEntry[0] + rkQ[1][0]*m_afEntry[3] +
        rkQ[2][0]*m_afEntry[6];
    kR[0][1] = rkQ[0][0]*m_afEntry[1] + rkQ[1][0]*m_afEntry[4] +
        rkQ[2][0]*m_afEntry[7];
    kR[1][1] = rkQ[0][1]*m_afEntry[1] + rkQ[1][1]*m_afEntry[4] +
        rkQ[2][1]*m_afEntry[7];
    kR[0][2] = rkQ[0][0]*m_afEntry[2] + rkQ[1][0]*m_afEntry[5] +
        rkQ[2][0]*m_afEntry[8];
    kR[1][2] = rkQ[0][1]*m_afEntry[2] + rkQ[1][1]*m_afEntry[5] +
        rkQ[2][1]*m_afEntry[8];
    kR[2][2] = rkQ[0][2]*m_afEntry[2] + rkQ[1][2]*m_afEntry[5] +
        rkQ[2][2]*m_afEntry[8];

    // the scaling component
    rkD.MakeDiagonal(kR[0][0],kR[1][1],kR[2][2]);

    // the shear component
    RealType fInvD0 = ((RealType)1.0)/rkD[0][0];
    rkU[0][0] = (RealType)1.0;
    rkU[0][1] = kR[0][1]*fInvD0;
    rkU[0][2] = kR[0][2]*fInvD0;
    rkU[1][0] = (RealType)0.0;
    rkU[1][1] = (RealType)1.0;
    rkU[1][2] = kR[1][2]/rkD[1][1];
    rkU[2][0] = (RealType)0.0;
    rkU[2][1] = (RealType)0.0;
    rkU[2][2] = (RealType)1.0;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix3<RealType> operator* (RealType fScalar, const Matrix3<RealType>& rkM)
{
    return rkM*fScalar;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector3<RealType> operator* (const Vector3<RealType>& rkV, const Matrix3<RealType>& rkM)
{
    return Vector3<RealType>(
        rkV[0]*rkM[0][0] + rkV[1]*rkM[1][0] + rkV[2]*rkM[2][0],
        rkV[0]*rkM[0][1] + rkV[1]*rkM[1][1] + rkV[2]*rkM[2][1],
        rkV[0]*rkM[0][2] + rkV[1]*rkM[1][2] + rkV[2]*rkM[2][2]);
}
//----------------------------------------------------------------------------
