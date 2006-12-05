// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3VECTOR3_H
#define WM3VECTOR3_H

#include "Math.hpp"
#include <algorithm>

#include <iostream>

//namespace Wm3
//{

template <class RealType>
class Vector3;

template <class RealType>
std::ostream & operator<< (std::ostream &os, const Vector3<RealType> &v);

template <class RealType>
class Vector3
{
public:
    // construction
    Vector3 ();  // uninitialized
    Vector3 (RealType fX, RealType fY, RealType fZ);
    Vector3 (const Vector3& rkV);

    // coordinate access
    operator const RealType* () const;
    operator RealType* ();
    RealType operator[] (int i) const;
    RealType& operator[] (int i);
	__attribute__((deprecated)) inline     RealType x() const{return X();}
    RealType X() const ;
	__attribute__((deprecated)) inline     RealType& x(){return X();}
    RealType& X() ;
	__attribute__((deprecated)) inline     RealType y() const{return Y();}
    RealType Y() const ;
	__attribute__((deprecated)) inline     RealType& y(){return Y();}
    RealType& Y() ;
	__attribute__((deprecated)) inline     RealType z() const{return Z();}
    RealType Z() const ;
	__attribute__((deprecated)) inline     RealType& z(){return Z();}
    RealType& Z() ;

    // assignment
    Vector3& operator= (const Vector3& rkV);

    // comparison
    bool operator== (const Vector3& rkV) const;
    bool operator!= (const Vector3& rkV) const;
    bool operator<  (const Vector3& rkV) const;
    bool operator<= (const Vector3& rkV) const;
    bool operator>  (const Vector3& rkV) const;
    bool operator>= (const Vector3& rkV) const;

    // arithmetic operations
    Vector3 operator+ (const Vector3& rkV) const;
    Vector3 operator- (const Vector3& rkV) const;
    Vector3 operator* (RealType fScalar) const;
    Vector3 operator/ (RealType fScalar) const;
    Vector3 operator- () const;

    // arithmetic updates
    Vector3& operator+= (const Vector3& rkV);
    Vector3& operator-= (const Vector3& rkV);
    Vector3& operator*= (RealType fScalar);
    Vector3& operator/= (RealType fScalar);

    // vector operations
	__attribute__((deprecated)) inline     RealType length() const{return Length();}
    RealType Length() const ;
	__attribute__((deprecated)) inline     RealType squaredLength() const{return SquaredLength();}
    RealType SquaredLength() const ;
	__attribute__((deprecated)) inline     RealType dot(const Vector3& rkV) const{return Dot(rkV);}
    RealType Dot(const Vector3& rkV) const ;
    RealType angleBetweenUnitVectors(const Vector3& rkV) const;
	__attribute__((deprecated)) inline     RealType normalize(){return Normalize();}
    RealType Normalize() ;
    Vector3 maxVector (const Vector3& rkV) const;
    Vector3 minVector (const Vector3& rkV) const;
    Vector3 multDiag (const Vector3& rkV) const;
    Vector3 divDiag (const Vector3& rkV) const;

    // The cross products are computed using the right-handed rule.  Be aware
    // that some graphics APIs use a left-handed rule.  If you have to compute
    // a cross product with these functions and send the result to the API
    // that expects left-handed, you will need to change sign on the vector
    // (replace each component value c by -c).
	__attribute__((deprecated)) inline     Vector3 cross(const Vector3& rkV) const{return Cross(rkV);}
    Vector3 Cross(const Vector3& rkV) const ;
	__attribute__((deprecated)) inline     Vector3 unitCross(const Vector3& rkV) const{return UnitCross(rkV);}
    Vector3 UnitCross(const Vector3& rkV) const ;

    // Compute the barycentric coordinates of the point with respect to the
    // tetrahedron <V0,V1,V2,V3>, P = b0*V0 + b1*V1 + b2*V2 + b3*V3, where
    // b0 + b1 + b2 + b3 = 1.
	__attribute__((deprecated)) inline     void getBarycentrics(const Vector3<RealType>& rkV0,        const Vector3<RealType>& rkV1, const Vector3<RealType>& rkV2,        const Vector3<RealType>& rkV3, RealType afBary[4]) const{return GetBarycentrics(rkV0,rkV1,rkV2,rkV3,afBary);}
    void GetBarycentrics(const Vector3<RealType>& rkV0,        const Vector3<RealType>& rkV1, const Vector3<RealType>& rkV2,        const Vector3<RealType>& rkV3, RealType afBary[4]) const ;

    // Gram-Schmidt orthonormalization.  Take linearly independent vectors
    // U, V, and W and compute an orthonormal set (unit length, mutually
    // perpendicular).
	__attribute__((deprecated)) inline     static void orthonormalize(Vector3& rkU, Vector3& rkV, Vector3& rkW){return Orthonormalize(rkU,rkV,rkW);}
    static void Orthonormalize(Vector3& rkU, Vector3& rkV, Vector3& rkW) ;
	__attribute__((deprecated)) inline     static void orthonormalize(Vector3* akV){return Orthonormalize(akV);}
    static void Orthonormalize(Vector3* akV) ;

    // Input W must be initialized to a nonzero vector, output is {U,V,W},
    // an orthonormal basis.  A hint is provided about whether or not W
    // is already unit length.
	__attribute__((deprecated)) inline     static void generateOrthonormalBasis(Vector3& rkU, Vector3& rkV,        Vector3& rkW, bool bUnitLengthW){return GenerateOrthonormalBasis(rkU,rkV,rkW,bUnitLengthW);}
    static void GenerateOrthonormalBasis(Vector3& rkU, Vector3& rkV,        Vector3& rkW, bool bUnitLengthW) ;

    // special vectors
    static const Vector3 ZERO;
    static const Vector3 UNIT_X;
    static const Vector3 UNIT_Y;
    static const Vector3 UNIT_Z;
    
    friend std::ostream & operator<< <> (std::ostream &os, const Vector3<RealType> &v);
private:
    // support for comparisons
	__attribute__((deprecated)) inline     int compareArrays(const Vector3& rkV) const{return CompareArrays(rkV);}
    int CompareArrays(const Vector3& rkV) const ;

    RealType m_afTuple[3];
};

template <class RealType,typename RealType2>
Vector3<RealType> operator* (RealType2 fScalar, const Vector3<RealType>& rkV);

template <class RealType>
std::ostream & operator<< (std::ostream &os, const Vector3<RealType> &v)
{
    return os << v[0] << " " << v[1] << " " << v[2];
}

#include "Vector3.ipp"

typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;
typedef Vector3<Real> Vector3r;

//}

#endif

