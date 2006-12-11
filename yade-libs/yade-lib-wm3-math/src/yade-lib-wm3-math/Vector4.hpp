// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef YADE_WM3VECTOR4_H
#define YADE_WM3VECTOR4_H

#warning Deprecated header, include <Wm3Vector4.h> and <yade/yade-core/yadeWm3.hpp> instead

#ifndef USE_BASTARDIZED_WM3
#include<Wm3Vector4.h>
#include<yade/yade-core/yadeWm3.hpp>
using namespace Wm3;
#else

#include "Math.hpp"

//namespace Wm3
//{

template <class RealType>
class Vector4
{
public:
    // construction
    Vector4 ();  // uninitialized
    Vector4 (RealType fX, RealType fY, RealType fZ, RealType fW);
    Vector4 (const Vector4& rkV);

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
	__attribute__((deprecated)) inline     RealType w() const{return W();}
    RealType W() const ;
	__attribute__((deprecated)) inline     RealType& w(){return W();}
    RealType& W() ;

    // assignment
    Vector4& operator= (const Vector4& rkV);

    // comparison
    bool operator== (const Vector4& rkV) const;
    bool operator!= (const Vector4& rkV) const;
    bool operator<  (const Vector4& rkV) const;
    bool operator<= (const Vector4& rkV) const;
    bool operator>  (const Vector4& rkV) const;
    bool operator>= (const Vector4& rkV) const;

    // arithmetic operations
    Vector4 operator+ (const Vector4& rkV) const;
    Vector4 operator- (const Vector4& rkV) const;
    Vector4 operator* (RealType fScalar) const;
    Vector4 operator/ (RealType fScalar) const;
    Vector4 operator- () const;

    // arithmetic updates
    Vector4& operator+= (const Vector4& rkV);
    Vector4& operator-= (const Vector4& rkV);
    Vector4& operator*= (RealType fScalar);
    Vector4& operator/= (RealType fScalar);

    // vector operations
	__attribute__((deprecated)) inline     RealType length() const{return Length();}
    RealType Length() const ;
	__attribute__((deprecated)) inline     RealType squaredLength() const{return SquaredLength();}
    RealType SquaredLength() const ;
	__attribute__((deprecated)) inline     RealType dot(const Vector4& rkV) const{return Dot(rkV);}
    RealType Dot(const Vector4& rkV) const ;
	__attribute__((deprecated)) inline     RealType normalize(){return Normalize();}
    RealType Normalize() ;

	 // added by the yade team
    __attribute__((deprecated)) Vector4 maxVector (const Vector4& rkV) const;
    __attribute__((deprecated)) Vector4 minVector (const Vector4& rkV) const;
    __attribute__((deprecated)) Vector4 multDiag (const Vector4& rkV) const;
	 //end
        
// special vectors
    static const Vector4 ZERO;
    static const Vector4 UNIT_X;
    static const Vector4 UNIT_Y;
    static const Vector4 UNIT_Z;
    static const Vector4 UNIT_W;

private:
    // support for comparisons
	__attribute__((deprecated)) inline     int compareArrays(const Vector4& rkV) const{return CompareArrays(rkV);}
    int CompareArrays(const Vector4& rkV) const ;

    RealType m_afTuple[4];
};

template <class RealType>
Vector4<RealType> operator* (RealType fScalar, const Vector4<RealType>& rkV);

#include "Vector4.ipp"

typedef Vector4<float> Vector4f;
typedef Vector4<double> Vector4d;
typedef Vector4<Real> Vector4r;

//}

#endif
#endif
