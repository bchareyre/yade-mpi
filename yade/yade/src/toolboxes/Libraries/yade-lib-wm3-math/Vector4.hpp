// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3VECTOR4_H
#define WM3VECTOR4_H

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
    RealType x () const;
    RealType& x ();
    RealType y () const;
    RealType& y ();
    RealType z () const;
    RealType& z ();
    RealType w () const;
    RealType& w ();

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
    RealType length () const;
    RealType squaredLength () const;
    RealType dot (const Vector4& rkV) const;
    RealType normalize ();
    Vector4 maxVector (const Vector4& rkV) const;
    Vector4 minVector (const Vector4& rkV) const;
    Vector4 multDiag (const Vector4& rkV) const;
        
// special vectors
    static const Vector4 ZERO;
    static const Vector4 UNIT_X;
    static const Vector4 UNIT_Y;
    static const Vector4 UNIT_Z;
    static const Vector4 UNIT_W;

private:
    // support for comparisons
    int compareArrays (const Vector4& rkV) const;

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

