// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3MATRIX3_H
#define WM3MATRIX3_H

// Matrix operations are applied on the left.  For example, given a matrix M
// and a vector V, matrix-times-vector is M*V.  That is, V is treated as a
// column vector.  Some graphics APIs use V*M where V is treated as a row
// vector.  In this context the "M" matrix is really a transpose of the M as
// represented in Wild Magic.  Similarly, to apply two matrix operations M0
// and M1, in that order, you compute M1*M0 so that the transform of a vector
// is (M1*M0)*V = M1*(M0*V).  Some graphics APIs use M0*M1, but again these
// matrices are the transpose of those as represented in Wild Magic.  You
// must therefore be careful about how you interface the transformation code
// with graphics APIS.
//
// For memory organization it might seem natural to chose RealType[N][N] for the
// matrix storage, but this can be a problem on a platform/console that
// chooses to store the data in column-major rather than row-major format.
// To avoid potential portability problems, the matrix is stored as RealType[N*N]
// and organized in row-major order.  That is, the entry of the matrix in row
// r (0 <= r < N) and column c (0 <= c < N) is stored at index i = c+N*r
// (0 <= i < N*N).

// The (x,y,z) coordinate system is assumed to be right-handed.  Coordinate
// axis rotation matrices are of the form
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

#include "Vector3.hpp"

//namespace Wm3
//{

template <class RealType>
class Matrix3
{
public:
    // If bZero is true, create the zero matrix.  Otherwise, create the
    // identity matrix.
    Matrix3 (bool bZero = true);

    // copy constructor
    Matrix3 (const Matrix3& rkM);

    // input Mrc is in row r, column c.
    Matrix3 (RealType fM00, RealType fM01, RealType fM02,
             RealType fM10, RealType fM11, RealType fM12,
             RealType fM20, RealType fM21, RealType fM22);

    // Create a matrix from an array of numbers.  The input array is
    // interpreted based on the Boolean input as
    //   true:  entry[0..8]={m00,m01,m02,m10,m11,m12,m20,m21,m22} [row major]
    //   false: entry[0..8]={m00,m10,m20,m01,m11,m21,m02,m12,m22} [col major]
    Matrix3 (const RealType afEntry[9], bool bRowMajor);

    // Create matrices based on vector input.  The Boolean is interpreted as
    //   true: vectors are columns of the matrix
    //   false: vectors are rows of the matrix
    Matrix3 (const Vector3<RealType>& rkU, const Vector3<RealType>& rkV,
        const Vector3<RealType>& rkW, bool bColumns);
    Matrix3 (const Vector3<RealType>* akV, bool bColumns);

    // create a diagonal matrix
    Matrix3 (RealType fM00, RealType fM11, RealType fM22);

    // Create rotation matrices (positive angle - counterclockwise).  The
    // angle must be in radians, not degrees.
    Matrix3 (const Vector3<RealType>& rkAxis, RealType fAngle);

    // create a tensor product U*V^T
    Matrix3 (const Vector3<RealType>& rkU, const Vector3<RealType>& rkV);

    // create various matrices
    Matrix3& makeZero ();
    Matrix3& makeIdentity ();
    Matrix3& makeDiagonal (RealType fM00, RealType fM11, RealType fM22);
    Matrix3& fromAxisAngle (const Vector3<RealType>& rkAxis, RealType fAngle);
    Matrix3& makeTensorProduct (const Vector3<RealType>& rkU,
        const Vector3<RealType>& rkV);

    // member access
    operator const RealType* () const;
    operator RealType* ();
    const RealType* operator[] (int iRow) const;
    RealType* operator[] (int iRow);
    RealType operator() (int iRow, int iCol) const;
    RealType& operator() (int iRow, int iCol);
    void setRow (int iRow, const Vector3<RealType>& rkV);
    Vector3<RealType> getRow (int iRow) const;
    void setColumn (int iCol, const Vector3<RealType>& rkV);
    Vector3<RealType> getColumn (int iCol) const;
    void getColumnMajor (RealType* afCMajor) const;

    // assignment
    Matrix3& operator= (const Matrix3& rkM);

    // comparison
    bool operator== (const Matrix3& rkM) const;
    bool operator!= (const Matrix3& rkM) const;
    bool operator<  (const Matrix3& rkM) const;
    bool operator<= (const Matrix3& rkM) const;
    bool operator>  (const Matrix3& rkM) const;
    bool operator>= (const Matrix3& rkM) const;

    // arithmetic operations
    Matrix3 operator+ (const Matrix3& rkM) const;
    Matrix3 operator- (const Matrix3& rkM) const;
    Matrix3 operator* (const Matrix3& rkM) const;
    Matrix3 operator* (RealType fScalar) const;
    Matrix3 operator/ (RealType fScalar) const;
    Matrix3 operator- () const;

    // arithmetic updates
    Matrix3& operator+= (const Matrix3& rkM);
    Matrix3& operator-= (const Matrix3& rkM);
    Matrix3& operator*= (RealType fScalar);
    Matrix3& operator/= (RealType fScalar);

    // matrix times vector
    Vector3<RealType> operator* (const Vector3<RealType>& rkV) const;  // M * v

    // other operations
    Matrix3 transpose () const;  // M^T
    Matrix3 transposeTimes (const Matrix3& rkM) const;  // this^T * M
    Matrix3 timesTranspose (const Matrix3& rkM) const;  // this * M^T
    Matrix3 inverse () const;
    Matrix3 adjoint () const;
    RealType determinant () const;
    RealType qForm (const Vector3<RealType>& rkU,
        const Vector3<RealType>& rkV) const;  // u^T*M*v
    Matrix3 timesDiagonal (const Vector3<RealType>& rkDiag) const;  // M*D
    Matrix3 diagonalTimes (const Vector3<RealType>& rkDiag) const;  // D*M

    // The matrix must be a rotation for these functions to be valid.  The
    // last function uses Gram-Schmidt orthonormalization applied to the
    // columns of the rotation matrix.  The angle must be in radians, not
    // degrees.
    void toAxisAngle (Vector3<RealType>& rkAxis, RealType& rfAngle) const;
    void orthonormalize ();

    // The matrix must be symmetric.  Factor M = R * D * R^T where
    // R = [u0|u1|u2] is a rotation matrix with columns u0, u1, and u2 and
    // D = diag(d0,d1,d2) is a diagonal matrix whose diagonal entries are d0,
    // d1, and d2.  The eigenvector u[i] corresponds to eigenvector d[i].
    // The eigenvalues are ordered as d0 <= d1 <= d2.
    void eigenDecomposition (Matrix3& rkRot, Matrix3& rkDiag) const;

    // The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
    // where yaw is rotation about the Up vector, pitch is rotation about the
    // Right axis, and roll is rotation about the Direction axis.
    Matrix3& fromEulerAnglesXYZ (RealType fYAngle, RealType fPAngle, RealType fRAngle);
    Matrix3& fromEulerAnglesXZY (RealType fYAngle, RealType fPAngle, RealType fRAngle);
    Matrix3& fromEulerAnglesYXZ (RealType fYAngle, RealType fPAngle, RealType fRAngle);
    Matrix3& fromEulerAnglesYZX (RealType fYAngle, RealType fPAngle, RealType fRAngle);
    Matrix3& fromEulerAnglesZXY (RealType fYAngle, RealType fPAngle, RealType fRAngle);
    Matrix3& fromEulerAnglesZYX (RealType fYAngle, RealType fPAngle, RealType fRAngle);
    bool toEulerAnglesXYZ (RealType& rfYAngle, RealType& rfPAngle,
        RealType& rfRAngle) const;
    bool toEulerAnglesXZY (RealType& rfYAngle, RealType& rfPAngle,
        RealType& rfRAngle) const;
    bool toEulerAnglesYXZ (RealType& rfYAngle, RealType& rfPAngle,
        RealType& rfRAngle) const;
    bool toEulerAnglesYZX (RealType& rfYAngle, RealType& rfPAngle,
        RealType& rfRAngle) const;
    bool toEulerAnglesZXY (RealType& rfYAngle, RealType& rfPAngle,
        RealType& rfRAngle) const;
    bool toEulerAnglesZYX (RealType& rfYAngle, RealType& rfPAngle,
        RealType& rfRAngle) const;

    // SLERP (spherical linear interpolation) without quaternions.  Computes
    // R(t) = R0*(Transpose(R0)*R1)^t.  If Q is a rotation matrix with
    // unit-length axis U and angle A, then Q^t is a rotation matrix with
    // unit-length axis U and rotation angle t*A.
    static Matrix3 slerp (RealType fT, const Matrix3& rkR0,
        const Matrix3& rkR1);

    // Singular value decomposition, M = L*S*R, where L and R are orthogonal
    // and S is a diagonal matrix whose diagonal entries are nonnegative.
    void singularValueDecomposition (Matrix3& rkL, Matrix3& rkS,
        Matrix3& rkR) const;
    void singularValueComposition (const Matrix3& rkL, const Matrix3& rkS,
        const Matrix3& rkR);

    // factor M = Q*D*U with orthogonal Q, diagonal D, upper triangular U
    void QDUDecomposition (Matrix3& rkQ, Matrix3& rkD, Matrix3& rkU) const;

    // special matrices
    static const Matrix3 ZERO;
    static const Matrix3 IDENTITY;

private:
    // Support for eigendecomposition.  The Tridiagonalize function applies
    // a Householder transformation to the matrix.  If that transformation
    // is the identity (the matrix is already tridiagonal), then the return
    // value is 'false'.  Otherwise, the transformation is a reflection and
    // the return value is 'true'.  The QLAlgorithm returns 'true' iff the
    // QL iteration scheme converged.
    bool tridiagonalize (RealType afDiag[3], RealType afSubd[2]);
    bool QLAlgorithm (RealType afDiag[3], RealType afSubd[2]);

    // support for singular value decomposition
    static void bidiagonalize (Matrix3& rkA, Matrix3& rkL, Matrix3& rkR);
    static void golubKahanStep (Matrix3& rkA, Matrix3& rkL, Matrix3& rkR);

    // for indexing into the 1D array of the matrix, iCol+N*iRow
    static int I (int iRow, int iCol);

    // support for comparisons
    int compareArrays (const Matrix3& rkM) const;

    RealType m_afEntry[9];
};

// c * M
template <class RealType>
Matrix3<RealType> operator* (RealType fScalar, const Matrix3<RealType>& rkM);

// v^T * M
template <class RealType>
Vector3<RealType> operator* (const Vector3<RealType>& rkV, const Matrix3<RealType>& rkM);

#include "Matrix3.ipp"

typedef Matrix3<float> Matrix3f;
typedef Matrix3<double> Matrix3d;
typedef Matrix3<Real> Matrix3r;

//}

#endif

