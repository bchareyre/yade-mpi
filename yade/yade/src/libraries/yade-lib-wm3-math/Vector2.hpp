// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3VECTOR2_H
#define WM3VECTOR2_H

#include <yade-lib-wm3-math/Math.hpp>

//namespace Wm3
//{

template <class RealType>
class Vector2
{
public:
    // construction
    Vector2 ();  // uninitialized
    Vector2 (RealType fX, RealType fY);
    Vector2 (const Vector2& rkV);

    // coordinate access
    operator const RealType* () const;
    operator RealType* ();
    RealType operator[] (int i) const;
    RealType& operator[] (int i);
    RealType x () const;
    RealType& x ();
    RealType y () const;
    RealType& y ();

    // assignment
    Vector2& operator= (const Vector2& rkV);

    // comparison
    bool operator== (const Vector2& rkV) const;
    bool operator!= (const Vector2& rkV) const;
    bool operator<  (const Vector2& rkV) const;
    bool operator<= (const Vector2& rkV) const;
    bool operator>  (const Vector2& rkV) const;
    bool operator>= (const Vector2& rkV) const;

    // arithmetic operations
    Vector2 operator+ (const Vector2& rkV) const;
    Vector2 operator- (const Vector2& rkV) const;
    Vector2 operator* (RealType fScalar) const;
    Vector2 operator/ (RealType fScalar) const;
    Vector2 operator- () const;

    // arithmetic updates
    Vector2& operator+= (const Vector2& rkV);
    Vector2& operator-= (const Vector2& rkV);
    Vector2& operator*= (RealType fScalar);
    Vector2& operator/= (RealType fScalar);

    // vector operations
    RealType length () const;
    RealType squaredLength () const;
    RealType dot (const Vector2& rkV) const;
    RealType normalize ();
    Vector2 maxVector (const Vector2& rkV) const;
    Vector2 minVector (const Vector2& rkV) const;
    Vector2 multDiag (const Vector2& rkV) const;
    
    // returns (y,-x)
    Vector2 perp () const;

    // returns (y,-x)/sqrt(x*x+y*y)
    Vector2 unitPerp () const;

    // returns DotPerp((x,y),(V.x,V.y)) = x*V.y - y*V.x
    RealType dotPerp (const Vector2& rkV) const;

    // Compute the barycentric coordinates of the point with respect to the
    // triangle <V0,V1,V2>, P = b0*V0 + b1*V1 + b2*V2, where b0 + b1 + b2 = 1.
    void getBarycentrics (const Vector2<RealType>& rkV0,
        const Vector2<RealType>& rkV1, const Vector2<RealType>& rkV2,
        RealType afBary[3]) const;

    // Gram-Schmidt orthonormalization.  Take linearly independent vectors U
    // and V and compute an orthonormal set (unit length, mutually
    // perpendicular).
    static void orthonormalize (Vector2& rkU, Vector2& rkV);

    // Input V must be initialized to a nonzero vector, output is {U,V}, an
    // orthonormal basis.  A hint is provided about whether or not V is
    // already unit length.
    static void generateOrthonormalBasis (Vector2& rkU, Vector2& rkV,
        bool bUnitLengthV);

    // special vectors
    static const Vector2 ZERO;
    static const Vector2 UNIT_X;
    static const Vector2 UNIT_Y;

private:
    // support for comparisons
    int compareArrays (const Vector2& rkV) const;

    RealType m_afTuple[2];
};

template <class RealType>
Vector2<RealType> operator* (RealType fScalar, const Vector2<RealType>& rkV);

#include "Vector2.ipp"

typedef Vector2<float> Vector2f;
typedef Vector2<double> Vector2d;
typedef Vector2<Real> Vector2r;


//}

#endif

