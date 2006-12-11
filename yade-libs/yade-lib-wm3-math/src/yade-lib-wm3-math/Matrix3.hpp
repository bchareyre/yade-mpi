// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef YADE_WM3MATRIX3_H
#define YADE_WM3MATRIX3_H

#warning Deprecated header, include <Wm3Matrix3.h> and <yade/yade-core/yadeWm3.hpp> instead

#ifndef USE_BASTARDIZED_WM3
#include<Wm3Matrix3.h>
#include<yade/yade-core/yadeWm3.hpp>
using namespace Wm3;
#else

#include"Math.hpp"

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
// axis orientation matrices are of the form
//   RX =    1       0       0
//           0     cos(t) -sin(t)
//           0     sin(t)  cos(t)
// where t > 0 indicates a counterclockwise orientation in the yz-plane
//   RY =  cos(t)    0     sin(t)
//           0       1       0
//        -sin(t)    0     cos(t)
// where t > 0 indicates a counterclockwise orientation in the zx-plane
//   RZ =  cos(t) -sin(t)    0
//         sin(t)  cos(t)    0
//           0       0       1
// where t > 0 indicates a counterclockwise orientation in the xy-plane.

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

    // Create orientation matrices (positive angle - counterclockwise).  The
    // angle must be in radians, not degrees.
    Matrix3 (const Vector3<RealType>& rkAxis, RealType fAngle);

    // create a tensor product U*V^T
    Matrix3 (const Vector3<RealType>& rkU, const Vector3<RealType>& rkV);

    // create various matrices
	__attribute__((deprecated)) inline     Matrix3& makeZero(){return MakeZero();}
    Matrix3& MakeZero() ;
	__attribute__((deprecated)) inline     Matrix3& makeIdentity(){return MakeIdentity();}
    Matrix3& MakeIdentity() ;
	__attribute__((deprecated)) inline     Matrix3& makeDiagonal(RealType fM00, RealType fM11, RealType fM22){return MakeDiagonal(fM00,fM11,fM22);}
    Matrix3& MakeDiagonal(RealType fM00, RealType fM11, RealType fM22) ;
	__attribute__((deprecated)) inline     Matrix3& fromAxisAngle(const Vector3<RealType>& rkAxis, RealType fAngle){return FromAxisAngle(rkAxis,fAngle);}
    Matrix3& FromAxisAngle(const Vector3<RealType>& rkAxis, RealType fAngle) ;
	__attribute__((deprecated)) inline     Matrix3& makeTensorProduct(const Vector3<RealType>& rkU,        const Vector3<RealType>& rkV){return MakeTensorProduct(rkU,rkV);}
    Matrix3& MakeTensorProduct(const Vector3<RealType>& rkU,        const Vector3<RealType>& rkV) ;

    // member access
    operator const RealType* () const;
    operator RealType* ();
    const RealType* operator[] (int iRow) const;
    RealType* operator[] (int iRow);
    RealType operator() (int iRow, int iCol) const;
    RealType& operator() (int iRow, int iCol);
	__attribute__((deprecated)) inline     void setRow(int iRow, const Vector3<RealType>& rkV){return SetRow(iRow,rkV);}
    void SetRow(int iRow, const Vector3<RealType>& rkV) ;
	__attribute__((deprecated)) inline     Vector3<RealType> getRow(int iRow) const{return GetRow(iRow);}
    Vector3<RealType> GetRow(int iRow) const ;
	__attribute__((deprecated)) inline     void setColumn(int iCol, const Vector3<RealType>& rkV){return SetColumn(iCol,rkV);}
    void SetColumn(int iCol, const Vector3<RealType>& rkV) ;
	__attribute__((deprecated)) inline     Vector3<RealType> getColumn(int iCol) const{return GetColumn(iCol);}
    Vector3<RealType> GetColumn(int iCol) const ;
	__attribute__((deprecated)) inline     void getColumnMajor(RealType* afCMajor) const{return GetColumnMajor(afCMajor);}
    void GetColumnMajor(RealType* afCMajor) const ;

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
	__attribute__((deprecated)) inline     Matrix3 transpose() const{return Transpose();}
    Matrix3 Transpose() const ;
	__attribute__((deprecated)) inline     Matrix3 transposeTimes(const Matrix3& rkM) const{return TransposeTimes(rkM);}
    Matrix3 TransposeTimes(const Matrix3& rkM) const ;
	__attribute__((deprecated)) inline     Matrix3 timesTranspose(const Matrix3& rkM) const{return TimesTranspose(rkM);}
    Matrix3 TimesTranspose(const Matrix3& rkM) const ;
	__attribute__((deprecated)) inline     Matrix3 inverse() const{return Inverse();}
    Matrix3 Inverse() const ;
	__attribute__((deprecated)) inline     Matrix3 adjoint() const{return Adjoint();}
    Matrix3 Adjoint() const ;
	__attribute__((deprecated)) inline     RealType determinant() const{return Determinant();}
    RealType Determinant() const ;
	__attribute__((deprecated)) inline     RealType qForm(const Vector3<RealType>& rkU,        const Vector3<RealType>& rkV) const{return QForm(rkU,rkV);}
    RealType QForm(const Vector3<RealType>& rkU,        const Vector3<RealType>& rkV) const ;
	__attribute__((deprecated)) inline     Matrix3 timesDiagonal(const Vector3<RealType>& rkDiag) const{return TimesDiagonal(rkDiag);}
    Matrix3 TimesDiagonal(const Vector3<RealType>& rkDiag) const ;
	__attribute__((deprecated)) inline     Matrix3 diagonalTimes(const Vector3<RealType>& rkDiag) const{return DiagonalTimes(rkDiag);}
    Matrix3 DiagonalTimes(const Vector3<RealType>& rkDiag) const ;

    // The matrix must be a orientation for these functions to be valid.  The
    // last function uses Gram-Schmidt orthonormalization applied to the
    // columns of the orientation matrix.  The angle must be in radians, not
    // degrees.
	__attribute__((deprecated)) inline     void toAxisAngle(Vector3<RealType>& rkAxis, RealType& rfAngle) const{return ToAxisAngle(rkAxis,rfAngle);}
    void ToAxisAngle(Vector3<RealType>& rkAxis, RealType& rfAngle) const ;
	__attribute__((deprecated)) inline     void orthonormalize(){return Orthonormalize();}
    void Orthonormalize() ;

    // The matrix must be symmetric.  Factor M = R * D * R^T where
    // R = [u0|u1|u2] is a orientation matrix with columns u0, u1, and u2 and
    // D = diag(d0,d1,d2) is a diagonal matrix whose diagonal entries are d0,
    // d1, and d2.  The eigenvector u[i] corresponds to eigenvector d[i].
    // The eigenvalues are ordered as d0 <= d1 <= d2.
	__attribute__((deprecated)) inline     void eigenDecomposition(Matrix3& rkRot, Matrix3& rkDiag) const{return EigenDecomposition(rkRot,rkDiag);}
    void EigenDecomposition(Matrix3& rkRot, Matrix3& rkDiag) const ;

    // The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
    // where yaw is orientation about the Up vector, pitch is orientation about the
    // Right axis, and roll is orientation about the Direction axis.
	__attribute__((deprecated)) inline     Matrix3& fromEulerAnglesXYZ(RealType fYAngle, RealType fPAngle, RealType fRAngle){return FromEulerAnglesXYZ(fYAngle,fPAngle,fRAngle);}
    Matrix3& FromEulerAnglesXYZ(RealType fYAngle, RealType fPAngle, RealType fRAngle) ;
	__attribute__((deprecated)) inline     Matrix3& fromEulerAnglesXZY(RealType fYAngle, RealType fPAngle, RealType fRAngle){return FromEulerAnglesXZY(fYAngle,fPAngle,fRAngle);}
    Matrix3& FromEulerAnglesXZY(RealType fYAngle, RealType fPAngle, RealType fRAngle) ;
	__attribute__((deprecated)) inline     Matrix3& fromEulerAnglesYXZ(RealType fYAngle, RealType fPAngle, RealType fRAngle){return FromEulerAnglesYXZ(fYAngle,fPAngle,fRAngle);}
    Matrix3& FromEulerAnglesYXZ(RealType fYAngle, RealType fPAngle, RealType fRAngle) ;
	__attribute__((deprecated)) inline     Matrix3& fromEulerAnglesYZX(RealType fYAngle, RealType fPAngle, RealType fRAngle){return FromEulerAnglesYZX(fYAngle,fPAngle,fRAngle);}
    Matrix3& FromEulerAnglesYZX(RealType fYAngle, RealType fPAngle, RealType fRAngle) ;
	__attribute__((deprecated)) inline     Matrix3& fromEulerAnglesZXY(RealType fYAngle, RealType fPAngle, RealType fRAngle){return FromEulerAnglesZXY(fYAngle,fPAngle,fRAngle);}
    Matrix3& FromEulerAnglesZXY(RealType fYAngle, RealType fPAngle, RealType fRAngle) ;
	__attribute__((deprecated)) inline     Matrix3& fromEulerAnglesZYX(RealType fYAngle, RealType fPAngle, RealType fRAngle){return FromEulerAnglesZYX(fYAngle,fPAngle,fRAngle);}
    Matrix3& FromEulerAnglesZYX(RealType fYAngle, RealType fPAngle, RealType fRAngle) ;
	__attribute__((deprecated)) inline     bool toEulerAnglesXYZ(RealType& rfYAngle, RealType& rfPAngle,        RealType& rfRAngle) const{return ToEulerAnglesXYZ(rfYAngle,rfPAngle,rfRAngle);}
    bool ToEulerAnglesXYZ(RealType& rfYAngle, RealType& rfPAngle,        RealType& rfRAngle) const ;
	__attribute__((deprecated)) inline     bool toEulerAnglesXZY(RealType& rfYAngle, RealType& rfPAngle,        RealType& rfRAngle) const{return ToEulerAnglesXZY(rfYAngle,rfPAngle,rfRAngle);}
    bool ToEulerAnglesXZY(RealType& rfYAngle, RealType& rfPAngle,        RealType& rfRAngle) const ;
	__attribute__((deprecated)) inline     bool toEulerAnglesYXZ(RealType& rfYAngle, RealType& rfPAngle,        RealType& rfRAngle) const{return ToEulerAnglesYXZ(rfYAngle,rfPAngle,rfRAngle);}
    bool ToEulerAnglesYXZ(RealType& rfYAngle, RealType& rfPAngle,        RealType& rfRAngle) const ;
	__attribute__((deprecated)) inline     bool toEulerAnglesYZX(RealType& rfYAngle, RealType& rfPAngle,        RealType& rfRAngle) const{return ToEulerAnglesYZX(rfYAngle,rfPAngle,rfRAngle);}
    bool ToEulerAnglesYZX(RealType& rfYAngle, RealType& rfPAngle,        RealType& rfRAngle) const ;
	__attribute__((deprecated)) inline     bool toEulerAnglesZXY(RealType& rfYAngle, RealType& rfPAngle,        RealType& rfRAngle) const{return ToEulerAnglesZXY(rfYAngle,rfPAngle,rfRAngle);}
    bool ToEulerAnglesZXY(RealType& rfYAngle, RealType& rfPAngle,        RealType& rfRAngle) const ;
	__attribute__((deprecated)) inline     bool toEulerAnglesZYX(RealType& rfYAngle, RealType& rfPAngle,        RealType& rfRAngle) const{return ToEulerAnglesZYX(rfYAngle,rfPAngle,rfRAngle);}
    bool ToEulerAnglesZYX(RealType& rfYAngle, RealType& rfPAngle,        RealType& rfRAngle) const ;

    // SLERP (spherical linear interpolation) without quaternions.  Computes
    // R(t) = R0*(Transpose(R0)*R1)^t.  If Q is a orientation matrix with
    // unit-length axis U and angle A, then Q^t is a orientation matrix with
    // unit-length axis U and orientation angle t*A.
	__attribute__((deprecated)) inline     static Matrix3 slerp(RealType fT, const Matrix3& rkR0,        const Matrix3& rkR1){return Slerp(fT,rkR0,rkR1);}
    static Matrix3 Slerp(RealType fT, const Matrix3& rkR0,        const Matrix3& rkR1) ;

    // Singular value decomposition, M = L*S*R, where L and R are orthogonal
    // and S is a diagonal matrix whose diagonal entries are nonnegative.
	__attribute__((deprecated)) inline     void singularValueDecomposition(Matrix3& rkL, Matrix3& rkS,        Matrix3& rkR) const{return SingularValueDecomposition(rkL,rkS,rkR);}
    void SingularValueDecomposition(Matrix3& rkL, Matrix3& rkS,        Matrix3& rkR) const ;
	__attribute__((deprecated)) inline     void singularValueComposition(const Matrix3& rkL, const Matrix3& rkS,        const Matrix3& rkR){return SingularValueComposition(rkL,rkS,rkR);}
    void SingularValueComposition(const Matrix3& rkL, const Matrix3& rkS,        const Matrix3& rkR) ;

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
	__attribute__((deprecated)) inline     bool tridiagonalize(RealType afDiag[3], RealType afSubd[2]){return Tridiagonalize(afDiag,afSubd);}
    bool Tridiagonalize(RealType afDiag[3], RealType afSubd[2]) ;
    bool QLAlgorithm (RealType afDiag[3], RealType afSubd[2]);

    // support for singular value decomposition
	__attribute__((deprecated)) inline     static void bidiagonalize(Matrix3& rkA, Matrix3& rkL, Matrix3& rkR){return Bidiagonalize(rkA,rkL,rkR);}
    static void Bidiagonalize(Matrix3& rkA, Matrix3& rkL, Matrix3& rkR) ;
	__attribute__((deprecated)) inline     static void golubKahanStep(Matrix3& rkA, Matrix3& rkL, Matrix3& rkR){return GolubKahanStep(rkA,rkL,rkR);}
    static void GolubKahanStep(Matrix3& rkA, Matrix3& rkL, Matrix3& rkR) ;

    // for indexing into the 1D array of the matrix, iCol+N*iRow
    static int I (int iRow, int iCol);

    // support for comparisons
	__attribute__((deprecated)) inline     int compareArrays(const Matrix3& rkM) const{return CompareArrays(rkM);}
    int CompareArrays(const Matrix3& rkM) const ;

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
#endif
