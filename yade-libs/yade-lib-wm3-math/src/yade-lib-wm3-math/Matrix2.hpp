// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3MATRIX2_H
#define WM3MATRIX2_H

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

// Rotation matrices are of the form
//   R = cos(t) -sin(t)
//       sin(t)  cos(t)
// where t > 0 indicates a counterclockwise orientation in the xy-plane.

#include "Vector2.hpp"

//namespace Wm3
//{

template <class RealType>
class Matrix2
{
public:
    // If bZero is true, create the zero matrix.  Otherwise, create the
    // identity matrix.
    Matrix2 (bool bZero = true);

    // copy constructor
    Matrix2 (const Matrix2& rkM);

    // input Mrc is in row r, column c.
    Matrix2 (RealType fM00, RealType fM01, RealType fM10, RealType fM11);

    // Create a matrix from an array of numbers.  The input array is
    // interpreted based on the Boolean input as
    //   true:  entry[0..3] = {m00,m01,m10,m11}  [row major]
    //   false: entry[0..3] = {m00,m10,m01,m11}  [column major]
    Matrix2 (const RealType afEntry[4], bool bRowMajor);

    // Create matrices based on vector input.  The Boolean is interpreted as
    //   true: vectors are columns of the matrix
    //   false: vectors are rows of the matrix
    Matrix2 (const Vector2<RealType>& rkU, const Vector2<RealType>& rkV,
        bool bColumns);
    Matrix2 (const Vector2<RealType>* akV, bool bColumns);

    // create a diagonal matrix
    Matrix2 (RealType fM00, RealType fM11);

    // create a orientation matrix (positive angle - counterclockwise)
    Matrix2 (RealType fAngle);

    // create a tensor product U*V^T
    Matrix2 (const Vector2<RealType>& rkU, const Vector2<RealType>& rkV);

    // create various matrices
	__attribute__((deprecated)) inline     void makeZero(){return MakeZero();}
    void MakeZero() ;
	__attribute__((deprecated)) inline     void makeIdentity(){return MakeIdentity();}
    void MakeIdentity() ;
	__attribute__((deprecated)) inline     void makeDiagonal(RealType fM00, RealType fM11){return MakeDiagonal(fM00,fM11);}
    void MakeDiagonal(RealType fM00, RealType fM11) ;
	__attribute__((deprecated)) inline     void fromAngle(RealType fAngle){return FromAngle(fAngle);}
    void FromAngle(RealType fAngle) ;
	__attribute__((deprecated)) inline     void makeTensorProduct(const Vector2<RealType>& rkU,        const Vector2<RealType>& rkV){return MakeTensorProduct(rkU,rkV);}
    void MakeTensorProduct(const Vector2<RealType>& rkU,        const Vector2<RealType>& rkV) ;

    // member access
    operator const RealType* () const;
    operator RealType* ();
    const RealType* operator[] (int iRow) const;
    RealType* operator[] (int iRow);
    RealType operator() (int iRow, int iCol) const;
    RealType& operator() (int iRow, int iCol);
	__attribute__((deprecated)) inline     void setRow(int iRow, const Vector2<RealType>& rkV){return SetRow(iRow,rkV);}
    void SetRow(int iRow, const Vector2<RealType>& rkV) ;
	__attribute__((deprecated)) inline     Vector2<RealType> getRow(int iRow) const{return GetRow(iRow);}
    Vector2<RealType> GetRow(int iRow) const ;
	__attribute__((deprecated)) inline     void setColumn(int iCol, const Vector2<RealType>& rkV){return SetColumn(iCol,rkV);}
    void SetColumn(int iCol, const Vector2<RealType>& rkV) ;
	__attribute__((deprecated)) inline     Vector2<RealType> getColumn(int iCol) const{return GetColumn(iCol);}
    Vector2<RealType> GetColumn(int iCol) const ;
	__attribute__((deprecated)) inline     void getColumnMajor(RealType* afCMajor) const{return GetColumnMajor(afCMajor);}
    void GetColumnMajor(RealType* afCMajor) const ;

    // assignment
    Matrix2& operator= (const Matrix2& rkM);

    // comparison
    bool operator== (const Matrix2& rkM) const;
    bool operator!= (const Matrix2& rkM) const;
    bool operator<  (const Matrix2& rkM) const;
    bool operator<= (const Matrix2& rkM) const;
    bool operator>  (const Matrix2& rkM) const;
    bool operator>= (const Matrix2& rkM) const;

    // arithmetic operations
    Matrix2 operator+ (const Matrix2& rkM) const;
    Matrix2 operator- (const Matrix2& rkM) const;
    Matrix2 operator* (const Matrix2& rkM) const;
    Matrix2 operator* (RealType fScalar) const;
    Matrix2 operator/ (RealType fScalar) const;
    Matrix2 operator- () const;

    // arithmetic updates
    Matrix2& operator+= (const Matrix2& rkM);
    Matrix2& operator-= (const Matrix2& rkM);
    Matrix2& operator*= (RealType fScalar);
    Matrix2& operator/= (RealType fScalar);

    // matrix times vector
    Vector2<RealType> operator* (const Vector2<RealType>& rkV) const;  // M * v

    // other operations
	__attribute__((deprecated)) inline     Matrix2 transpose() const{return Transpose();}
    Matrix2 Transpose() const ;
	__attribute__((deprecated)) inline     Matrix2 transposeTimes(const Matrix2& rkM) const{return TransposeTimes(rkM);}
    Matrix2 TransposeTimes(const Matrix2& rkM) const ;
	__attribute__((deprecated)) inline     Matrix2 timesTranspose(const Matrix2& rkM) const{return TimesTranspose(rkM);}
    Matrix2 TimesTranspose(const Matrix2& rkM) const ;
	__attribute__((deprecated)) inline     Matrix2 inverse() const{return Inverse();}
    Matrix2 Inverse() const ;
	__attribute__((deprecated)) inline     Matrix2 adjoint() const{return Adjoint();}
    Matrix2 Adjoint() const ;
	__attribute__((deprecated)) inline     RealType determinant() const{return Determinant();}
    RealType Determinant() const ;
	__attribute__((deprecated)) inline     RealType qForm(const Vector2<RealType>& rkU,        const Vector2<RealType>& rkV) const{return QForm(rkU,rkV);}
    RealType QForm(const Vector2<RealType>& rkU,        const Vector2<RealType>& rkV) const ;

    // The matrix must be a orientation for these functions to be valid.  The
    // last function uses Gram-Schmidt orthonormalization applied to the
    // columns of the orientation matrix.  The angle must be in radians, not
    // degrees.
	__attribute__((deprecated)) inline     void toAngle(RealType& rfAngle) const{return ToAngle(rfAngle);}
    void ToAngle(RealType& rfAngle) const ;
	__attribute__((deprecated)) inline     void orthonormalize(){return Orthonormalize();}
    void Orthonormalize() ;

    // The matrix must be symmetric.  Factor M = R * D * R^T where
    // R = [u0|u1] is a orientation matrix with columns u0 and u1 and
    // D = diag(d0,d1) is a diagonal matrix whose diagonal entries are d0 and
    // d1.  The eigenvector u[i] corresponds to eigenvector d[i].  The
    // eigenvalues are ordered as d0 <= d1.
	__attribute__((deprecated)) inline     void eigenDecomposition(Matrix2& rkRot, Matrix2& rkDiag) const{return EigenDecomposition(rkRot,rkDiag);}
    void EigenDecomposition(Matrix2& rkRot, Matrix2& rkDiag) const ;

    static const Matrix2 ZERO;
    static const Matrix2 IDENTITY;

private:
    // for indexing into the 1D array of the matrix, iCol+N*iRow
    static int I (int iRow, int iCol);

    // support for comparisons
	__attribute__((deprecated)) inline     int compareArrays(const Matrix2& rkM) const{return CompareArrays(rkM);}
    int CompareArrays(const Matrix2& rkM) const ;
    
    // matrix stored in row-major order
    RealType m_afEntry[4];
};

// c * M
template <class RealType>
Matrix2<RealType> operator* (RealType fScalar, const Matrix2<RealType>& rkM);

// v^T * M
template <class RealType>
Vector2<RealType> operator* (const Vector2<RealType>& rkV, const Matrix2<RealType>& rkM);

#include "Matrix2.ipp"

typedef Matrix2<float> Matrix2f;
typedef Matrix2<double> Matrix2d;
typedef Matrix2<Real> Matrix2r;

//}

#endif

