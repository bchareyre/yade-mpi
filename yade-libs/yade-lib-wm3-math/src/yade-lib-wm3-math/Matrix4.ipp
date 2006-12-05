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
Matrix4<RealType>::Matrix4 (bool bZero)
{
    if ( bZero )
        makeZero();
    else
        makeIdentity();
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix4<RealType>::Matrix4 (const Matrix4& rkM)
{
    memcpy(m_afEntry,rkM.m_afEntry,16*sizeof(RealType));
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix4<RealType>::Matrix4 (RealType fM00, RealType fM01, RealType fM02, RealType fM03,
    RealType fM10, RealType fM11, RealType fM12, RealType fM13, RealType fM20, RealType fM21,
    RealType fM22, RealType fM23, RealType fM30, RealType fM31, RealType fM32, RealType fM33)
{
    m_afEntry[ 0] = fM00;
    m_afEntry[ 1] = fM01;
    m_afEntry[ 2] = fM02;
    m_afEntry[ 3] = fM03;
    m_afEntry[ 4] = fM10;
    m_afEntry[ 5] = fM11;
    m_afEntry[ 6] = fM12;
    m_afEntry[ 7] = fM13;
    m_afEntry[ 8] = fM20;
    m_afEntry[ 9] = fM21;
    m_afEntry[10] = fM22;
    m_afEntry[11] = fM23;
    m_afEntry[12] = fM30;
    m_afEntry[13] = fM31;
    m_afEntry[14] = fM32;
    m_afEntry[15] = fM33;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix4<RealType>::Matrix4 (const RealType afEntry[16], bool bRowMajor)
{
    if ( bRowMajor )
    {
        memcpy(m_afEntry,afEntry,16*sizeof(RealType));
    }
    else
    {
        m_afEntry[ 0] = afEntry[ 0];
        m_afEntry[ 1] = afEntry[ 4];
        m_afEntry[ 2] = afEntry[ 8];
        m_afEntry[ 3] = afEntry[12];
        m_afEntry[ 4] = afEntry[ 1];
        m_afEntry[ 5] = afEntry[ 5];
        m_afEntry[ 6] = afEntry[ 9];
        m_afEntry[ 7] = afEntry[13];
        m_afEntry[ 8] = afEntry[ 2];
        m_afEntry[ 9] = afEntry[ 6];
        m_afEntry[10] = afEntry[10];
        m_afEntry[11] = afEntry[14];
        m_afEntry[12] = afEntry[ 3];
        m_afEntry[13] = afEntry[ 7];
        m_afEntry[14] = afEntry[11];
        m_afEntry[15] = afEntry[15];
    }
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix4<RealType>::operator const RealType* () const
{
    return m_afEntry;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix4<RealType>::operator RealType* ()
{
    return m_afEntry;
}
//----------------------------------------------------------------------------
template <class RealType>
const RealType* Matrix4<RealType>::operator[] (int iRow) const
{
    return &m_afEntry[4*iRow];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType* Matrix4<RealType>::operator[] (int iRow)
{
    return &m_afEntry[4*iRow];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Matrix4<RealType>::operator() (int iRow, int iCol) const
{
    return m_afEntry[I(iRow,iCol)];
}
//----------------------------------------------------------------------------
template <class RealType>
RealType& Matrix4<RealType>::operator() (int iRow, int iCol)
{
    return m_afEntry[I(iRow,iCol)];
}
//----------------------------------------------------------------------------
template <class RealType>
int Matrix4<RealType>::I (int iRow, int iCol)
{
    assert( 0 <= iRow && iRow < 4 && 0 <= iCol && iCol < 4 );
    return iCol + 4*iRow;
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix4<RealType>::MakeZero() {
    memset(m_afEntry,0,16*sizeof(RealType));
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix4<RealType>::MakeIdentity() {
    m_afEntry[ 0] = (RealType)1.0;
    m_afEntry[ 1] = (RealType)0.0;
    m_afEntry[ 2] = (RealType)0.0;
    m_afEntry[ 3] = (RealType)0.0;
    m_afEntry[ 4] = (RealType)0.0;
    m_afEntry[ 5] = (RealType)1.0;
    m_afEntry[ 6] = (RealType)0.0;
    m_afEntry[ 7] = (RealType)0.0;
    m_afEntry[ 8] = (RealType)0.0;
    m_afEntry[ 9] = (RealType)0.0;
    m_afEntry[10] = (RealType)1.0;
    m_afEntry[11] = (RealType)0.0;
    m_afEntry[12] = (RealType)0.0;
    m_afEntry[13] = (RealType)0.0;
    m_afEntry[14] = (RealType)0.0;
    m_afEntry[15] = (RealType)1.0;
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix4<RealType>::SetRow(int iRow, const Vector4<RealType>& rkV) {
    assert( 0 <= iRow && iRow < 4 );
    for (int iCol = 0, i = 4*iRow; iCol < 4; iCol++, i++)
        m_afEntry[i] = rkV[iCol];
}
//----------------------------------------------------------------------------
template <class RealType>
Vector4<RealType> Matrix4<RealType>::GetRow(int iRow) const {
    assert( 0 <= iRow && iRow < 4 );
    Vector4<RealType> kV;
    for (int iCol = 0, i = 4*iRow; iCol < 4; iCol++, i++)
        kV[iCol] = m_afEntry[i];
    return kV;
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix4<RealType>::SetColumn(int iCol, const Vector4<RealType>& rkV) {
    assert( 0 <= iCol && iCol < 4 );
    for (int iRow = 0, i = iCol; iRow < 4; iRow++, i += 4)
        m_afEntry[i] = rkV[iRow];
}
//----------------------------------------------------------------------------
template <class RealType>
Vector4<RealType> Matrix4<RealType>::GetColumn(int iCol) const {
    assert( 0 <= iCol && iCol < 4 );
    Vector4<RealType> kV;
    for (int iRow = 0, i = iCol; iRow < 4; iRow++, i += 4)
        kV[iRow] = m_afEntry[i];
    return kV;
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix4<RealType>::GetColumnMajor(RealType* afCMajor) const {
    for (int iRow = 0, i = 0; iRow < 4; iRow++)
    {
        for (int iCol = 0; iCol < 4; iCol++)
            afCMajor[i++] = m_afEntry[I(iCol,iRow)];
    }
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix4<RealType>& Matrix4<RealType>::operator= (const Matrix4& rkM)
{
    memcpy(m_afEntry,rkM.m_afEntry,16*sizeof(RealType));
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
int Matrix4<RealType>::CompareArrays(const Matrix4& rkM) const {
    return memcmp(m_afEntry,rkM.m_afEntry,16*sizeof(RealType));
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix4<RealType>::operator== (const Matrix4& rkM) const
{
    return CompareArrays(rkM) == 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix4<RealType>::operator!= (const Matrix4& rkM) const
{
    return CompareArrays(rkM) != 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix4<RealType>::operator<  (const Matrix4& rkM) const
{
    return CompareArrays(rkM) < 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix4<RealType>::operator<= (const Matrix4& rkM) const
{
    return CompareArrays(rkM) <= 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix4<RealType>::operator>  (const Matrix4& rkM) const
{
    return CompareArrays(rkM) > 0;
}
//----------------------------------------------------------------------------
template <class RealType>
bool Matrix4<RealType>::operator>= (const Matrix4& rkM) const
{
    return CompareArrays(rkM) >= 0;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix4<RealType> Matrix4<RealType>::operator+ (const Matrix4& rkM) const
{
    Matrix4 kSum;
    for (int i = 0; i < 16; i++)
        kSum.m_afEntry[i] = m_afEntry[i] + rkM.m_afEntry[i];
    return kSum;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix4<RealType> Matrix4<RealType>::operator- (const Matrix4& rkM) const
{
    Matrix4 kDiff;
    for (int i = 0; i < 16; i++)
        kDiff.m_afEntry[i] = m_afEntry[i] - rkM.m_afEntry[i];
    return kDiff;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix4<RealType> Matrix4<RealType>::operator* (const Matrix4& rkM) const
{
    Matrix4 kProd;
    for (int iRow = 0; iRow < 4; iRow++)
    {
        for (int iCol = 0; iCol < 4; iCol++)
        {
            int i = I(iRow,iCol);
            kProd.m_afEntry[i] = (RealType)0.0;
            for (int iMid = 0; iMid < 4; iMid++)
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
Matrix4<RealType> Matrix4<RealType>::operator* (RealType fScalar) const
{
    Matrix4 kProd;
    for (int i = 0; i < 16; i++)
        kProd.m_afEntry[i] = fScalar*m_afEntry[i];
    return kProd;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix4<RealType> Matrix4<RealType>::operator/ (RealType fScalar) const
{
    Matrix4 kQuot;
    int i;

    if ( fScalar != (RealType)0.0 )
    {
        RealType fInvScalar = ((RealType)1.0)/fScalar;
        for (i = 0; i < 16; i++)
            kQuot.m_afEntry[i] = fInvScalar*m_afEntry[i];
    }
    else
    {
        for (i = 0; i < 16; i++)
            kQuot.m_afEntry[i] = Math<RealType>::MAX_REAL;
    }

    return kQuot;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix4<RealType> Matrix4<RealType>::operator- () const
{
    Matrix4 kNeg;
    for (int i = 0; i < 16; i++)
        kNeg.m_afEntry[i] = -m_afEntry[i];
    return kNeg;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix4<RealType>& Matrix4<RealType>::operator+= (const Matrix4& rkM)
{
    for (int i = 0; i < 16; i++)
        m_afEntry[i] += rkM.m_afEntry[i];
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix4<RealType>& Matrix4<RealType>::operator-= (const Matrix4& rkM)
{
    for (int i = 0; i < 16; i++)
        m_afEntry[i] -= rkM.m_afEntry[i];
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix4<RealType>& Matrix4<RealType>::operator*= (RealType fScalar)
{
    for (int i = 0; i < 16; i++)
        m_afEntry[i] *= fScalar;
    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix4<RealType>& Matrix4<RealType>::operator/= (RealType fScalar)
{
    int i;

    if ( fScalar != (RealType)0.0 )
    {
        RealType fInvScalar = ((RealType)1.0)/fScalar;
        for (i = 0; i < 16; i++)
            m_afEntry[i] *= fInvScalar;
    }
    else
    {
        for (i = 0; i < 16; i++)
            m_afEntry[i] = Math<RealType>::MAX_REAL;
    }

    return *this;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector4<RealType> Matrix4<RealType>::operator* (const Vector4<RealType>& rkV) const
{
    Vector4<RealType> kProd;
    for (int iRow = 0; iRow < 4; iRow++)
    {
        kProd[iRow] = (RealType)0.0;
        for (int iCol = 0; iCol < 4; iCol++)
            kProd[iRow] += m_afEntry[I(iRow,iCol)]*rkV[iCol];
            
    }
    return kProd;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix4<RealType> Matrix4<RealType>::Transpose() const {
    Matrix4 kTranspose;
    for (int iRow = 0; iRow < 4; iRow++)
    {
        for (int iCol = 0; iCol < 4; iCol++)
            kTranspose.m_afEntry[I(iRow,iCol)] = m_afEntry[I(iCol,iRow)];
    }
    return kTranspose;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix4<RealType> Matrix4<RealType>::TransposeTimes(const Matrix4& rkM) const {
    // P = A^T*B, P[r][c] = sum_m A[m][r]*B[m][c]
    Matrix4 kProd;
    for (int iRow = 0; iRow < 4; iRow++)
    {
        for (int iCol = 0; iCol < 4; iCol++)
        {
            int i = I(iRow,iCol);
            kProd.m_afEntry[i] = (RealType)0.0;
            for (int iMid = 0; iMid < 4; iMid++)
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
Matrix4<RealType> Matrix4<RealType>::TimesTranspose(const Matrix4& rkM) const {
    // P = A*B^T, P[r][c] = sum_m A[r][m]*B[c][m]
    Matrix4 kProd;
    for (int iRow = 0; iRow < 4; iRow++)
    {
        for (int iCol = 0; iCol < 4; iCol++)
        {
            int i = I(iRow,iCol);
            kProd.m_afEntry[i] = (RealType)0.0;
            for (int iMid = 0; iMid < 4; iMid++)
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
Matrix4<RealType> Matrix4<RealType>::Inverse() const {
    RealType fA0 = m_afEntry[ 0]*m_afEntry[ 5] - m_afEntry[ 1]*m_afEntry[ 4];
    RealType fA1 = m_afEntry[ 0]*m_afEntry[ 6] - m_afEntry[ 2]*m_afEntry[ 4];
    RealType fA2 = m_afEntry[ 0]*m_afEntry[ 7] - m_afEntry[ 3]*m_afEntry[ 4];
    RealType fA3 = m_afEntry[ 1]*m_afEntry[ 6] - m_afEntry[ 2]*m_afEntry[ 5];
    RealType fA4 = m_afEntry[ 1]*m_afEntry[ 7] - m_afEntry[ 3]*m_afEntry[ 5];
    RealType fA5 = m_afEntry[ 2]*m_afEntry[ 7] - m_afEntry[ 3]*m_afEntry[ 6];
    RealType fB0 = m_afEntry[ 8]*m_afEntry[13] - m_afEntry[ 9]*m_afEntry[12];
    RealType fB1 = m_afEntry[ 8]*m_afEntry[14] - m_afEntry[10]*m_afEntry[12];
    RealType fB2 = m_afEntry[ 8]*m_afEntry[15] - m_afEntry[11]*m_afEntry[12];
    RealType fB3 = m_afEntry[ 9]*m_afEntry[14] - m_afEntry[10]*m_afEntry[13];
    RealType fB4 = m_afEntry[ 9]*m_afEntry[15] - m_afEntry[11]*m_afEntry[13];
    RealType fB5 = m_afEntry[10]*m_afEntry[15] - m_afEntry[11]*m_afEntry[14];

    RealType fDet = fA0*fB5-fA1*fB4+fA2*fB3+fA3*fB2-fA4*fB1+fA5*fB0;
    if ( Math<RealType>::fAbs(fDet) <= Math<RealType>::ZERO_TOLERANCE )
        return Matrix4<RealType>::ZERO;

    Matrix4 kInv;
    kInv[0][0] = + m_afEntry[ 5]*fB5 - m_afEntry[ 6]*fB4 + m_afEntry[ 7]*fB3;
    kInv[1][0] = - m_afEntry[ 4]*fB5 + m_afEntry[ 6]*fB2 - m_afEntry[ 7]*fB1;
    kInv[2][0] = + m_afEntry[ 4]*fB4 - m_afEntry[ 5]*fB2 + m_afEntry[ 7]*fB0;
    kInv[3][0] = - m_afEntry[ 4]*fB3 + m_afEntry[ 5]*fB1 - m_afEntry[ 6]*fB0;
    kInv[0][1] = - m_afEntry[ 1]*fB5 + m_afEntry[ 2]*fB4 - m_afEntry[ 3]*fB3;
    kInv[1][1] = + m_afEntry[ 0]*fB5 - m_afEntry[ 2]*fB2 + m_afEntry[ 3]*fB1;
    kInv[2][1] = - m_afEntry[ 0]*fB4 + m_afEntry[ 1]*fB2 - m_afEntry[ 3]*fB0;
    kInv[3][1] = + m_afEntry[ 0]*fB3 - m_afEntry[ 1]*fB1 + m_afEntry[ 2]*fB0;
    kInv[0][2] = + m_afEntry[13]*fA5 - m_afEntry[14]*fA4 + m_afEntry[15]*fA3;
    kInv[1][2] = - m_afEntry[12]*fA5 + m_afEntry[14]*fA2 - m_afEntry[15]*fA1;
    kInv[2][2] = + m_afEntry[12]*fA4 - m_afEntry[13]*fA2 + m_afEntry[15]*fA0;
    kInv[3][2] = - m_afEntry[12]*fA3 + m_afEntry[13]*fA1 - m_afEntry[14]*fA0;
    kInv[0][3] = - m_afEntry[ 9]*fA5 + m_afEntry[10]*fA4 - m_afEntry[11]*fA3;
    kInv[1][3] = + m_afEntry[ 8]*fA5 - m_afEntry[10]*fA2 + m_afEntry[11]*fA1;
    kInv[2][3] = - m_afEntry[ 8]*fA4 + m_afEntry[ 9]*fA2 - m_afEntry[11]*fA0;
    kInv[3][3] = + m_afEntry[ 8]*fA3 - m_afEntry[ 9]*fA1 + m_afEntry[10]*fA0;

    RealType fInvDet = ((RealType)1.0)/fDet;
    for (int iRow = 0; iRow < 4; iRow++)
    {
        for (int iCol = 0; iCol < 4; iCol++)
            kInv[iRow][iCol] *= fInvDet;
    }

    return kInv;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix4<RealType> Matrix4<RealType>::Adjoint() const {
    RealType fA0 = m_afEntry[ 0]*m_afEntry[ 5] - m_afEntry[ 1]*m_afEntry[ 4];
    RealType fA1 = m_afEntry[ 0]*m_afEntry[ 6] - m_afEntry[ 2]*m_afEntry[ 4];
    RealType fA2 = m_afEntry[ 0]*m_afEntry[ 7] - m_afEntry[ 3]*m_afEntry[ 4];
    RealType fA3 = m_afEntry[ 1]*m_afEntry[ 6] - m_afEntry[ 2]*m_afEntry[ 5];
    RealType fA4 = m_afEntry[ 1]*m_afEntry[ 7] - m_afEntry[ 3]*m_afEntry[ 5];
    RealType fA5 = m_afEntry[ 2]*m_afEntry[ 7] - m_afEntry[ 3]*m_afEntry[ 6];
    RealType fB0 = m_afEntry[ 8]*m_afEntry[13] - m_afEntry[ 9]*m_afEntry[12];
    RealType fB1 = m_afEntry[ 8]*m_afEntry[14] - m_afEntry[10]*m_afEntry[12];
    RealType fB2 = m_afEntry[ 8]*m_afEntry[15] - m_afEntry[11]*m_afEntry[12];
    RealType fB3 = m_afEntry[ 9]*m_afEntry[14] - m_afEntry[10]*m_afEntry[13];
    RealType fB4 = m_afEntry[ 9]*m_afEntry[15] - m_afEntry[11]*m_afEntry[13];
    RealType fB5 = m_afEntry[10]*m_afEntry[15] - m_afEntry[11]*m_afEntry[14];

    Matrix4 kAdj;
    kAdj[0][0] = + m_afEntry[ 5]*fB5 - m_afEntry[ 6]*fB4 + m_afEntry[ 7]*fB3;
    kAdj[1][0] = - m_afEntry[ 4]*fB5 + m_afEntry[ 6]*fB2 - m_afEntry[ 7]*fB1;
    kAdj[2][0] = + m_afEntry[ 4]*fB4 - m_afEntry[ 5]*fB2 + m_afEntry[ 7]*fB0;
    kAdj[3][0] = - m_afEntry[ 4]*fB3 + m_afEntry[ 5]*fB1 - m_afEntry[ 6]*fB0;
    kAdj[0][1] = - m_afEntry[ 1]*fB5 + m_afEntry[ 2]*fB4 - m_afEntry[ 3]*fB3;
    kAdj[1][1] = + m_afEntry[ 0]*fB5 - m_afEntry[ 2]*fB2 + m_afEntry[ 3]*fB1;
    kAdj[2][1] = - m_afEntry[ 0]*fB4 + m_afEntry[ 1]*fB2 - m_afEntry[ 3]*fB0;
    kAdj[3][1] = + m_afEntry[ 0]*fB3 - m_afEntry[ 1]*fB1 + m_afEntry[ 2]*fB0;
    kAdj[0][2] = + m_afEntry[13]*fA5 - m_afEntry[14]*fA4 + m_afEntry[15]*fA3;
    kAdj[1][2] = - m_afEntry[12]*fA5 + m_afEntry[14]*fA2 - m_afEntry[15]*fA1;
    kAdj[2][2] = + m_afEntry[12]*fA4 - m_afEntry[13]*fA2 + m_afEntry[15]*fA0;
    kAdj[3][2] = - m_afEntry[12]*fA3 + m_afEntry[13]*fA1 - m_afEntry[14]*fA0;
    kAdj[0][3] = - m_afEntry[ 9]*fA5 + m_afEntry[10]*fA4 - m_afEntry[11]*fA3;
    kAdj[1][3] = + m_afEntry[ 8]*fA5 - m_afEntry[10]*fA2 + m_afEntry[11]*fA1;
    kAdj[2][3] = - m_afEntry[ 8]*fA4 + m_afEntry[ 9]*fA2 - m_afEntry[11]*fA0;
    kAdj[3][3] = + m_afEntry[ 8]*fA3 - m_afEntry[ 9]*fA1 + m_afEntry[10]*fA0;

    return kAdj;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Matrix4<RealType>::Determinant() const {
    RealType fA0 = m_afEntry[ 0]*m_afEntry[ 5] - m_afEntry[ 1]*m_afEntry[ 4];
    RealType fA1 = m_afEntry[ 0]*m_afEntry[ 6] - m_afEntry[ 2]*m_afEntry[ 4];
    RealType fA2 = m_afEntry[ 0]*m_afEntry[ 7] - m_afEntry[ 3]*m_afEntry[ 4];
    RealType fA3 = m_afEntry[ 1]*m_afEntry[ 6] - m_afEntry[ 2]*m_afEntry[ 5];
    RealType fA4 = m_afEntry[ 1]*m_afEntry[ 7] - m_afEntry[ 3]*m_afEntry[ 5];
    RealType fA5 = m_afEntry[ 2]*m_afEntry[ 7] - m_afEntry[ 3]*m_afEntry[ 6];
    RealType fB0 = m_afEntry[ 8]*m_afEntry[13] - m_afEntry[ 9]*m_afEntry[12];
    RealType fB1 = m_afEntry[ 8]*m_afEntry[14] - m_afEntry[10]*m_afEntry[12];
    RealType fB2 = m_afEntry[ 8]*m_afEntry[15] - m_afEntry[11]*m_afEntry[12];
    RealType fB3 = m_afEntry[ 9]*m_afEntry[14] - m_afEntry[10]*m_afEntry[13];
    RealType fB4 = m_afEntry[ 9]*m_afEntry[15] - m_afEntry[11]*m_afEntry[13];
    RealType fB5 = m_afEntry[10]*m_afEntry[15] - m_afEntry[11]*m_afEntry[14];
    RealType fDet = fA0*fB5-fA1*fB4+fA2*fB3+fA3*fB2-fA4*fB1+fA5*fB0;
    return fDet;
}
//----------------------------------------------------------------------------
template <class RealType>
RealType Matrix4<RealType>::QForm(const Vector4<RealType>& rkU,    const Vector4<RealType>& rkV) const {
    return rkU.Dot((*this)*rkV);
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix4<RealType>::MakeObliqueProjection(const Vector3<RealType>& rkNormal,    const Vector3<RealType>& rkPoint, const Vector3<RealType>& rkDirection) {
    // The projection plane is Dot(N,X-P) = 0 where N is a 3-by-1 unit-length
    // normal vector and P is a 3-by-1 point on the plane.  The projection
    // is oblique to the plane, in the direction of the 3-by-1 vector D.
    // Necessarily Dot(N,D) is not zero for this projection to make sense.
    // Given a 3-by-1 point U, compute the intersection of the line U+t*D
    // with the plane to obtain t = -Dot(N,U-P)/Dot(N,D).  Then
    //
    //   projection(U) = P + [I - D*N^T/Dot(N,D)]*(U-P)
    //
    // A 4-by-4 homogeneous transformation representing the projection is
    //
    //       +-                               -+
    //   M = | D*N^T - Dot(N,D)*I   -Dot(N,P)D |
    //       |          0^T          -Dot(N,D) |
    //       +-                               -+
    //
    // where M applies to [U^T 1]^T by M*[U^T 1]^T.  The matrix is chosen so
    // that M[3][3] > 0 whenever Dot(N,D) < 0 (projection is onto the
    // "positive side" of the plane).

    RealType fNdD = rkNormal.Dot(rkDirection);
    RealType fNdP = rkNormal.Dot(rkPoint);
    m_afEntry[ 0] = rkDirection[0]*rkNormal[0] - fNdD;
    m_afEntry[ 1] = rkDirection[0]*rkNormal[1];
    m_afEntry[ 2] = rkDirection[0]*rkNormal[2];
    m_afEntry[ 3] = -fNdP*rkDirection[0];
    m_afEntry[ 4] = rkDirection[1]*rkNormal[0];
    m_afEntry[ 5] = rkDirection[1]*rkNormal[1] - fNdD;
    m_afEntry[ 6] = rkDirection[1]*rkNormal[2];
    m_afEntry[ 7] = -fNdP*rkDirection[1];
    m_afEntry[ 8] = rkDirection[2]*rkNormal[0];
    m_afEntry[ 9] = rkDirection[2]*rkNormal[1];
    m_afEntry[10] = rkDirection[2]*rkNormal[2] - fNdD;
    m_afEntry[11] = -fNdP*rkDirection[2];
    m_afEntry[12] = 0.0f;
    m_afEntry[13] = 0.0f;
    m_afEntry[14] = 0.0f;
    m_afEntry[15] = -fNdD;
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix4<RealType>::MakePerspectiveProjection(const Vector3<RealType>& rkNormal,    const Vector3<RealType>& rkPoint, const Vector3<RealType>& rkEye) {
    //     +-                                                 -+
    // M = | Dot(N,E-P)*I - E*N^T    -(Dot(N,E-P)*I - E*N^T)*E |
    //     |        -N^t                      Dot(N,E)         |
    //     +-                                                 -+
    //
    // where E is the eye point, P is a point on the plane, and N is a
    // unit-length plane normal.

    RealType fNdEmP = rkNormal.Dot(rkEye-rkPoint);

    m_afEntry[ 0] = fNdEmP - rkEye[0]*rkNormal[0];
    m_afEntry[ 1] = -rkEye[0]*rkNormal[1];
    m_afEntry[ 2] = -rkEye[0]*rkNormal[2];
    m_afEntry[ 3] = -(m_afEntry[0]*rkEye[0] + m_afEntry[1]*rkEye[1] +
        m_afEntry[2]*rkEye[2]);
    m_afEntry[ 4] = -rkEye[1]*rkNormal[0];
    m_afEntry[ 5] = fNdEmP - rkEye[1]*rkNormal[1];
    m_afEntry[ 6] = -rkEye[1]*rkNormal[2];
    m_afEntry[ 7] = -(m_afEntry[4]*rkEye[0] + m_afEntry[5]*rkEye[1] +
        m_afEntry[6]*rkEye[2]);
    m_afEntry[ 8] = -rkEye[2]*rkNormal[0];
    m_afEntry[ 9] = -rkEye[2]*rkNormal[1];
    m_afEntry[10] = fNdEmP- rkEye[2]*rkNormal[2];
    m_afEntry[11] = -(m_afEntry[8]*rkEye[0] + m_afEntry[9]*rkEye[1] +
        m_afEntry[10]*rkEye[2]);
    m_afEntry[12] = -rkNormal[0];
    m_afEntry[13] = -rkNormal[1];
    m_afEntry[14] = -rkNormal[2];
    m_afEntry[15] = rkNormal.Dot(rkEye);
}
//----------------------------------------------------------------------------
template <class RealType>
void Matrix4<RealType>::MakeReflection(const Vector3<RealType>& rkNormal,    const Vector3<RealType>& rkPoint) {
    //     +-                         -+
    // M = | I-2*N*N^T    2*Dot(N,P)*N |
    //     |     0^T            1      |
    //     +-                         -+
    //
    // where P is a point on the plane and N is a unit-length plane normal.

    RealType fTwoNdP = ((RealType)2.0)*(rkNormal.Dot(rkPoint));

    m_afEntry[ 0] = (RealType)1.0 - ((RealType)2.0)*rkNormal[0]*rkNormal[0];
    m_afEntry[ 1] = -((RealType)2.0)*rkNormal[0]*rkNormal[1];
    m_afEntry[ 2] = -((RealType)2.0)*rkNormal[0]*rkNormal[2];
    m_afEntry[ 3] = fTwoNdP*rkNormal[0];
    m_afEntry[ 4] = -((RealType)2.0)*rkNormal[1]*rkNormal[0];
    m_afEntry[ 5] = (RealType)1.0 - ((RealType)2.0)*rkNormal[1]*rkNormal[1];
    m_afEntry[ 6] = -((RealType)2.0)*rkNormal[1]*rkNormal[2];
    m_afEntry[ 7] = fTwoNdP*rkNormal[1];
    m_afEntry[ 8] = -((RealType)2.0)*rkNormal[2]*rkNormal[0];
    m_afEntry[ 9] = -((RealType)2.0)*rkNormal[2]*rkNormal[1];
    m_afEntry[10] = (RealType)1.0 - ((RealType)2.0)*rkNormal[2]*rkNormal[2];
    m_afEntry[11] = fTwoNdP*rkNormal[2];
    m_afEntry[12] = (RealType)0.0;
    m_afEntry[13] = (RealType)0.0;
    m_afEntry[14] = (RealType)0.0;
    m_afEntry[15] = (RealType)1.0;
}
//----------------------------------------------------------------------------
template <class RealType>
Matrix4<RealType> operator* (RealType fScalar, const Matrix4<RealType>& rkM)
{
    return rkM*fScalar;
}
//----------------------------------------------------------------------------
template <class RealType>
Vector4<RealType> operator* (const Vector4<RealType>& rkV, const Matrix4<RealType>& rkM)
{
    return Vector4<RealType>(
        rkV[0]*rkM[0][0]+rkV[1]*rkM[1][0]+rkV[2]*rkM[2][0]+rkV[3]*rkM[3][0],
        rkV[0]*rkM[0][1]+rkV[1]*rkM[1][1]+rkV[2]*rkM[2][1]+rkV[3]*rkM[3][1],
        rkV[0]*rkM[0][2]+rkV[1]*rkM[1][2]+rkV[2]*rkM[2][2]+rkV[3]*rkM[3][2],
        rkV[0]*rkM[0][3]+rkV[1]*rkM[1][3]+rkV[2]*rkM[2][3]+rkV[3]*rkM[3][3]);
}
//----------------------------------------------------------------------------

