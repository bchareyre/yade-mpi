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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MATRIX3__H__
#define _MATRIX3__H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Vector3.hpp"
#include "Serializable.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// NOTE.  The (x,y,z) coordinate system is assumed to be right-handed.
// Coordinate axis rotation matrices are of the form
//   RX =    1       0       0
//           0     cos(t) -sin(t)
//           0     sin(t)  cos(t)
// where t > 0 indicates a counterclockwise rotation in the yz-plane
//   RY =  cos(t)    0     sin(t)
//           0       1       0
//        -sin(t)    0     cos(t)
// where t > 0 indicates a counterclockwise rotation in the zx-plane
//   RZ =  cos(t) -sin(t)    0
//         sin(t)  cos(t)    0
//           0       0       1
// where t > 0 indicates a counterclockwise rotation in the xy-plane.

class Matrix3 : public Serializable
{

	// construction
	public : Matrix3 ();
	public : Matrix3 (const float aafEntry[3][3]);
	public : Matrix3 (const Matrix3& rkMatrix);
	public : Matrix3 (float fM00, float fM01, float fM02,
			  float fM10, float fM11, float fM12,
			  float fM20, float fM21, float fM22);

	// member access, allows use of construct mat[r][c]
	public : float* operator[] (int iRow) const;
	public : operator float* ();
	public : Vector3 GetColumn (int iCol) const;
	public : Vector3 GetRow (int iCol) const;

	// assignment and comparison
	public :  Matrix3& operator= (const Matrix3& rkMatrix);
	public : bool operator== (const Matrix3& rkMatrix) const;
	public : bool operator!= (const Matrix3& rkMatrix) const;

	// arithmetic operations
	public : Matrix3 operator+ (const Matrix3& rkMatrix) const;
	public : Matrix3 operator- (const Matrix3& rkMatrix) const;
	public : Matrix3 operator* (const Matrix3& rkMatrix) const;
	public : Matrix3 operator- () const;

	// matrix * vector [3x3 * 3x1 = 3x1]
	public : Vector3 operator* (const Vector3& rkVector) const;

	// vector * matrix [1x3 * 3x3 = 1x3]
	public : friend Vector3 operator* (const Vector3& rkVector, const Matrix3& rkMatrix);

	// matrix * scalar
	public : Matrix3 operator* (float fScalar) const;

	// scalar * matrix
	public : friend Matrix3 operator* (float fScalar, const Matrix3& rkMatrix);

	// M0.TransposeTimes(M1) = M0^t*M1 where M0^t is the transpose of M0
	public : Matrix3 TransposeTimes (const Matrix3& rkM) const;

	// M0.TimesTranspose(M1) = M0*M1^t where M1^t is the transpose of M1
	public : Matrix3 TimesTranspose (const Matrix3& rkM) const;

	// utilities
	public : Matrix3 Transpose () const;
	public : bool Inverse (Matrix3& rkInverse, float fTolerance = 1e-06f) const;
	public : Matrix3 Inverse (float fTolerance = 1e-06f) const;
	public : float Determinant () const;

	// SLERP (spherical linear interpolation) without quaternions.  Computes
	// R(t) = R0*(Transpose(R0)*R1)^t.  If Q is a rotation matrix with
	// unit-length axis U and angle A, then Q^t is a rotation matrix with
	// unit-length axis U and rotation angle t*A.
	public : static Matrix3 Slerp (float fT, const Matrix3& rkR0, const Matrix3& rkR1);

	// singular value decomposition
	public : void SingularValueDecomposition (Matrix3& rkL, Vector3& rkS, Matrix3& rkR) const;
	public : void SingularValueComposition (const Matrix3& rkL, const Vector3& rkS, const Matrix3& rkR);

	// Gram-Schmidt orthonormalization (applied to columns of rotation matrix)
	public : void Orthonormalize ();

	// orthogonal Q, diagonal D, upper triangular U stored as (u01,u02,u12)
	public : void QDUDecomposition (Matrix3& rkQ, Vector3& rkD, Vector3& rkU) const;

	public : float SpectralNorm () const;

	// matrix must be orthonormal
	public : void ToAxisAngle (Vector3& rkAxis, float& rfRadians) const;
	public : void FromAxisAngle (const Vector3& rkAxis, float fRadians);

	// The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
	// where yaw is rotation about the Up vector, pitch is rotation about the
	// Right axis, and roll is rotation about the Direction axis.
	public : bool ToEulerAnglesXYZ (float& rfYAngle, float& rfPAngle, float& rfRAngle) const;
    	public : bool ToEulerAnglesXZY (float& rfYAngle, float& rfPAngle, float& rfRAngle) const;
	public : bool ToEulerAnglesYXZ (float& rfYAngle, float& rfPAngle, float& rfRAngle) const;
	public : bool ToEulerAnglesYZX (float& rfYAngle, float& rfPAngle, float& rfRAngle) const;
	public : bool ToEulerAnglesZXY (float& rfYAngle, float& rfPAngle, float& rfRAngle) const;
	public : bool ToEulerAnglesZYX (float& rfYAngle, float& rfPAngle, float& rfRAngle) const;
	public : void FromEulerAnglesXYZ (float fYAngle, float fPAngle, float fRAngle);
	public : void FromEulerAnglesXZY (float fYAngle, float fPAngle, float fRAngle);
	public : void FromEulerAnglesYXZ (float fYAngle, float fPAngle, float fRAngle);
	public : void FromEulerAnglesYZX (float fYAngle, float fPAngle, float fRAngle);
	public : void FromEulerAnglesZXY (float fYAngle, float fPAngle, float fRAngle);
	public : void FromEulerAnglesZYX (float fYAngle, float fPAngle, float fRAngle);

	// eigensolver, matrix must be symmetric
	public : void EigenSolveSymmetric (float afEigenvalue[3], Vector3 akEigenvector[3]) const;

	public : static void TensorProduct (const Vector3& rkU, const Vector3& rkV, Matrix3& rkProduct);
	public : static const float EPSILON;
	public : static const Matrix3 ZERO;
	public : static const Matrix3 IDENTITY;

	// support for eigensolver
	protected : void Tridiagonal (float afDiag[3], float afSubDiag[3]);
	protected : bool QLAlgorithm (float afDiag[3], float afSubDiag[3]);

	// support for singular value decomposition
	protected : static const float ms_fSvdEpsilon;
	protected : static const int ms_iSvdMaxIterations;
	protected : static void Bidiagonalize (Matrix3& kA, Matrix3& kL, Matrix3& kR);
	protected : static void GolubKahanStep (Matrix3& kA, Matrix3& kL, Matrix3& kR);
	// support for spectral norm
	protected : static float MaxCubicRoot (float afCoeff[3]);

	protected : float m[3][3];

	public : void registerAttributes()
	{
		REGISTER_ATTRIBUTE(m[0][0]);
		REGISTER_ATTRIBUTE(m[0][1]);
		REGISTER_ATTRIBUTE(m[0][2]);
		REGISTER_ATTRIBUTE(m[1][0]);
		REGISTER_ATTRIBUTE(m[1][1]);
		REGISTER_ATTRIBUTE(m[1][2]);
		REGISTER_ATTRIBUTE(m[2][0]);
		REGISTER_ATTRIBUTE(m[2][1]);
		REGISTER_ATTRIBUTE(m[2][2]);
	}

	REGISTER_CLASS_NAME(Matrix3);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(Matrix3, true);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Matrix3.ipp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


