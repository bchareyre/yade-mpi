/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
 
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <assert.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline Matrix3::Matrix3 () : Serializable()
{
    // For efficiency reasons, do not initialize matrix.
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline Matrix3::Matrix3 (const float aafEntry[3][3]) : Serializable()
{
    memcpy(this->m,aafEntry,9*sizeof(float));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline Matrix3::Matrix3 (const Matrix3& matrix) : Serializable()
{
    memcpy(m,matrix.m,9*sizeof(float));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline Matrix3::Matrix3 (float fM00, float fM01, float fM02,
				  float fM10, float fM11, float fM12, 
				  float fM20, float fM21, float fM22) : Serializable()
{
    m[0][0] = fM00;
    m[0][1] = fM01;
    m[0][2] = fM02;
    m[1][0] = fM10;
    m[1][1] = fM11;
    m[1][2] = fM12;
    m[2][0] = fM20;
    m[2][1] = fM21;
    m[2][2] = fM22;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline float* Matrix3::operator[] (int iRow) const
{
    return (float*)&m[iRow][0];
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline Matrix3::operator float* ()
{
    return &m[0][0];
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline Vector3 Matrix3::GetColumn (int iCol) const
{
    assert( 0 <= iCol && iCol < 3 );
    return Vector3(m[0][iCol],m[1][iCol],m[2][iCol]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline Vector3 Matrix3::GetRow (int iRow) const
{
    assert( 0 <= iRow && iRow < 3 );
    return Vector3(m[iRow][0],m[iRow][1],m[iRow][2]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline Matrix3& Matrix3::operator= (const Matrix3& rkMatrix)
{
    memcpy(m,rkMatrix.m,9*sizeof(float));
    return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool Matrix3::operator== (const Matrix3& rkMatrix) const
{
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
        {
            if ( m[iRow][iCol] != rkMatrix.m[iRow][iCol] )
                return false;
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool Matrix3::operator!= (const Matrix3& rkMatrix) const
{
    return !operator==(rkMatrix);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline Matrix3 Matrix3::operator+ (const Matrix3& rkMatrix) const
{
    Matrix3 kSum;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
        {
            kSum.m[iRow][iCol] = m[iRow][iCol] +
                rkMatrix.m[iRow][iCol];
        }
    }
    return kSum;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline Matrix3 Matrix3::operator- (const Matrix3& rkMatrix) const
{
    Matrix3 kDiff;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
        {
            kDiff.m[iRow][iCol] = m[iRow][iCol] -
                rkMatrix.m[iRow][iCol];
        }
    }
    return kDiff;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline Matrix3 Matrix3::operator* (const Matrix3& rkMatrix) const
{
    Matrix3 kProd;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
        {
            kProd.m[iRow][iCol] =
                m[iRow][0]*rkMatrix.m[0][iCol] +
                m[iRow][1]*rkMatrix.m[1][iCol] +
                m[iRow][2]*rkMatrix.m[2][iCol];
        }
    }
    return kProd;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline Vector3 Matrix3::operator* (const Vector3& rkPoint) const
{
    Vector3 kProd;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        kProd[iRow] =
            m[iRow][0]*rkPoint[0] +
            m[iRow][1]*rkPoint[1] +
            m[iRow][2]*rkPoint[2];
    }
    return kProd;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline Vector3 operator* (const Vector3& rkPoint, const Matrix3& rkMatrix)
{
    Vector3 kProd;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        kProd[iRow] =
            rkPoint[0]*rkMatrix.m[0][iRow] +
            rkPoint[1]*rkMatrix.m[1][iRow] +
            rkPoint[2]*rkMatrix.m[2][iRow];
    }
    return kProd;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline Matrix3 Matrix3::operator- () const
{
    Matrix3 kNeg;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
            kNeg[iRow][iCol] = -m[iRow][iCol];
    }
    return kNeg;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline Matrix3 Matrix3::operator* (float fScalar) const
{
    Matrix3 kProd;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
            kProd[iRow][iCol] = fScalar*m[iRow][iCol];
    }
    return kProd;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline Matrix3 operator* (float fScalar, const Matrix3& rkMatrix)
{
    Matrix3 kProd;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
            kProd[iRow][iCol] = fScalar*rkMatrix.m[iRow][iCol];
    }
    return kProd;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline Matrix3 Matrix3::TransposeTimes (const Matrix3& rkM) const
{
    Matrix3 kProd;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
        {
            kProd[iRow][iCol] = 0.0f;
            for (int iMid = 0; iMid < 3; iMid++)
            {
                kProd[iRow][iCol] += m[iMid][iRow]*
                    rkM.m[iMid][iCol];
            }
        }
    }
    return kProd;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline Matrix3 Matrix3::TimesTranspose (const Matrix3& rkM) const
{
    Matrix3 kProd;
    for (int iRow = 0; iRow < 3; iRow++)
	{
        for (int iCol = 0; iCol < 3; iCol++)
        {
            kProd[iRow][iCol] = 0.0f;
            for (int iMid = 0; iMid < 3; iMid++)
                kProd[iRow][iCol] += m[iRow][iMid]*rkM.m[iCol][iMid];
        }
    }
    return kProd;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline Matrix3 Matrix3::Transpose () const
{
    Matrix3 kTranspose;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
            kTranspose[iRow][iCol] = m[iCol][iRow];
    }
    return kTranspose;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool Matrix3::Inverse (Matrix3& rkInverse, float fTolerance) const
{
    // Invert a 3x3 using cofactors.  This is about 8 times faster than
    // the Numerical Recipes code which uses Gaussian elimination.

    rkInverse[0][0] = m[1][1]*m[2][2] - m[1][2]*m[2][1];
    rkInverse[0][1] = m[0][2]*m[2][1] - m[0][1]*m[2][2];
    rkInverse[0][2] = m[0][1]*m[1][2] - m[0][2]*m[1][1];
    rkInverse[1][0] = m[1][2]*m[2][0] - m[1][0]*m[2][2];
    rkInverse[1][1] = m[0][0]*m[2][2] - m[0][2]*m[2][0];
    rkInverse[1][2] = m[0][2]*m[1][0] - m[0][0]*m[1][2];
    rkInverse[2][0] = m[1][0]*m[2][1] - m[1][1]*m[2][0];
    rkInverse[2][1] = m[0][1]*m[2][0] - m[0][0]*m[2][1];
    rkInverse[2][2] = m[0][0]*m[1][1] - m[0][1]*m[1][0];

    float fDet = m[0][0]*rkInverse[0][0] + m[0][1]*rkInverse[1][0]+ m[0][2]*rkInverse[2][0];

    if ( fabs(fDet) <= fTolerance )
        return false;

    float fInvDet = 1.0f/fDet;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
            rkInverse[iRow][iCol] *= fInvDet;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline Matrix3 Matrix3::Inverse (float fTolerance) const
{
    Matrix3 kInverse = Matrix3::ZERO;
    Inverse(kInverse,fTolerance);
    return kInverse;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline float Matrix3::Determinant () const
{
    float fCofactor00 = m[1][1]*m[2][2] - m[1][2]*m[2][1];
    float fCofactor10 = m[1][2]*m[2][0] - m[1][0]*m[2][2];
    float fCofactor20 = m[1][0]*m[2][1] - m[1][1]*m[2][0];

    float fDet = m[0][0]*fCofactor00 + m[0][1]*fCofactor10 + m[0][2]*fCofactor20;

    return fDet;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline Matrix3 Matrix3::Slerp (float fT, const Matrix3& rkR0, const Matrix3& rkR1)
{
    Vector3 kAxis;
    float fAngle;
    Matrix3 kProd = rkR0.TransposeTimes(rkR1);
    kProd.ToAxisAngle(kAxis,fAngle);

    Matrix3 kR;
    kR.FromAxisAngle(kAxis,fT*fAngle);

    return kR;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////


