// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef YADE_WM3MATRIX4_H
#define YADE_WM3MATRIX4_H

#warning Deprecated header, include <Wm3Matrix4.h> and <yade/yade-core/yadeWm3.hpp> instead

#ifndef USE_BASTARDIZED_WM3
#include<Wm3Matrix4.h>
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

//#include "Wm3Plane3.h"
#include "Vector4.hpp"
#include "Vector3.hpp"

//namespace Wm3
//{

template <class RealType>
class Matrix4
{
public:
    // If bZero is true, create the zero matrix.  Otherwise, create the
    // identity matrix.
    Matrix4 (bool bZero = true);

    // copy constructor
    Matrix4 (const Matrix4& rkM);

    // input Mrc is in row r, column c.
    Matrix4 (RealType fM00, RealType fM01, RealType fM02, RealType fM03,
             RealType fM10, RealType fM11, RealType fM12, RealType fM13,
             RealType fM20, RealType fM21, RealType fM22, RealType fM23,
             RealType fM30, RealType fM31, RealType fM32, RealType fM33);

    // Create a matrix from an array of numbers.  The input array is
    // interpreted based on the Boolean input as
    //   true:  entry[0..15]={m00,m01,m02,m03,m10,m11,m12,m13,m20,m21,m22,
    //                        m23,m30,m31,m32,m33} [row major]
    //   false: entry[0..15]={m00,m10,m20,m30,m01,m11,m21,m31,m02,m12,m22,
    //                        m32,m03,m13,m23,m33} [col major]
    Matrix4 (const RealType afEntry[16], bool bRowMajor);

	__attribute__((deprecated)) inline     void makeZero(){return MakeZero();}
    void MakeZero() ;
	__attribute__((deprecated)) inline     void makeIdentity(){return MakeIdentity();}
    void MakeIdentity() ;

    // member access
    operator const RealType* () const;
    operator RealType* ();
    const RealType* operator[] (int iRow) const;
    RealType* operator[] (int iRow);
    RealType operator() (int iRow, int iCol) const;
    RealType& operator() (int iRow, int iCol);
	__attribute__((deprecated)) inline     void setRow(int iRow, const Vector4<RealType>& rkV){return SetRow(iRow,rkV);}
    void SetRow(int iRow, const Vector4<RealType>& rkV) ;
	__attribute__((deprecated)) inline     Vector4<RealType> getRow(int iRow) const{return GetRow(iRow);}
    Vector4<RealType> GetRow(int iRow) const ;
	__attribute__((deprecated)) inline     void setColumn(int iCol, const Vector4<RealType>& rkV){return SetColumn(iCol,rkV);}
    void SetColumn(int iCol, const Vector4<RealType>& rkV) ;
	__attribute__((deprecated)) inline     Vector4<RealType> getColumn(int iCol) const{return GetColumn(iCol);}
    Vector4<RealType> GetColumn(int iCol) const ;
	__attribute__((deprecated)) inline     void getColumnMajor(RealType* afCMajor) const{return GetColumnMajor(afCMajor);}
    void GetColumnMajor(RealType* afCMajor) const ;

    // assignment
    Matrix4& operator= (const Matrix4& rkM);

    // comparison
    bool operator== (const Matrix4& rkM) const;
    bool operator!= (const Matrix4& rkM) const;
    bool operator<  (const Matrix4& rkM) const;
    bool operator<= (const Matrix4& rkM) const;
    bool operator>  (const Matrix4& rkM) const;
    bool operator>= (const Matrix4& rkM) const;

    // arithmetic operations
    Matrix4 operator+ (const Matrix4& rkM) const;
    Matrix4 operator- (const Matrix4& rkM) const;
    Matrix4 operator* (const Matrix4& rkM) const;
    Matrix4 operator* (RealType fScalar) const;
    Matrix4 operator/ (RealType fScalar) const;
    Matrix4 operator- () const;

    // arithmetic updates
    Matrix4& operator+= (const Matrix4& rkM);
    Matrix4& operator-= (const Matrix4& rkM);
    Matrix4& operator*= (RealType fScalar);
    Matrix4& operator/= (RealType fScalar);

    // matrix times vector
    Vector4<RealType> operator* (const Vector4<RealType>& rkV) const;  // M * v

    // other operations
	__attribute__((deprecated)) inline     Matrix4 transpose() const{return Transpose();}
    Matrix4 Transpose() const ;
	__attribute__((deprecated)) inline     Matrix4 transposeTimes(const Matrix4& rkM) const{return TransposeTimes(rkM);}
    Matrix4 TransposeTimes(const Matrix4& rkM) const ;
	__attribute__((deprecated)) inline     Matrix4 timesTranspose(const Matrix4& rkM) const{return TimesTranspose(rkM);}
    Matrix4 TimesTranspose(const Matrix4& rkM) const ;
	__attribute__((deprecated)) inline     Matrix4 inverse() const{return Inverse();}
    Matrix4 Inverse() const ;
	__attribute__((deprecated)) inline     Matrix4 adjoint() const{return Adjoint();}
    Matrix4 Adjoint() const ;
	__attribute__((deprecated)) inline     RealType determinant() const{return Determinant();}
    RealType Determinant() const ;
	__attribute__((deprecated)) inline     RealType qForm(const Vector4<RealType>& rkU,        const Vector4<RealType>& rkV) const{return QForm(rkU,rkV);}
    RealType QForm(const Vector4<RealType>& rkU,        const Vector4<RealType>& rkV) const ;

    // projection matrices onto a specified plane
	__attribute__((deprecated)) inline     void makeObliqueProjection(const Vector3<RealType>& rkNormal,        const Vector3<RealType>& rkPoint, const Vector3<RealType>& rkDirection){return MakeObliqueProjection(rkNormal,rkPoint,rkDirection);}
    void MakeObliqueProjection(const Vector3<RealType>& rkNormal,        const Vector3<RealType>& rkPoint, const Vector3<RealType>& rkDirection) ;
	__attribute__((deprecated)) inline     void makePerspectiveProjection(const Vector3<RealType>& rkNormal,        const Vector3<RealType>& rkPoint, const Vector3<RealType>& rkEye){return MakePerspectiveProjection(rkNormal,rkPoint,rkEye);}
    void MakePerspectiveProjection(const Vector3<RealType>& rkNormal,        const Vector3<RealType>& rkPoint, const Vector3<RealType>& rkEye) ;

    // reflection matrix through a specified plane
	__attribute__((deprecated)) inline     void makeReflection(const Vector3<RealType>& rkNormal,        const Vector3<RealType>& rkPoint){return MakeReflection(rkNormal,rkPoint);}
    void MakeReflection(const Vector3<RealType>& rkNormal,        const Vector3<RealType>& rkPoint) ;

    // special matrices
    static const Matrix4 ZERO;
    static const Matrix4 IDENTITY;

private:
    // for indexing into the 1D array of the matrix, iCol+N*iRow
    static int I (int iRow, int iCol);

    // support for comparisons
	__attribute__((deprecated)) inline     int compareArrays(const Matrix4& rkM) const{return CompareArrays(rkM);}
    int CompareArrays(const Matrix4& rkM) const ;

    RealType m_afEntry[16];
};

// c * M
template <class RealType>
Matrix4<RealType> operator* (RealType fScalar, const Matrix4<RealType>& rkM);

// v^T * M
template <class RealType>
Vector4<RealType> operator* (const Vector4<RealType>& rkV, const Matrix4<RealType>& rkM);

#include "Matrix4.ipp"

typedef Matrix4<float> Matrix4f;
typedef Matrix4<double> Matrix4d;
typedef Matrix4<Real> Matrix4r;
//}

#endif
#endif
