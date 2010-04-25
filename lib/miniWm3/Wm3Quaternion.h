	// Geometric Tools, Inc.
	// http://www.geometrictools.com
	// Copyright (c) 1998-2006.  All Rights Reserved
	//
	// The Wild Magic Library (WM3) source code is supplied under the terms of
	// the license agreement
	//     http://www.geometrictools.com/License/WildMagic3License.pdf
	// and may not be copied or disclosed except in accordance with the terms
	// of that agreement.

#ifndef WM3QUATERNION_H
#define WM3QUATERNION_H

#include "EigenCompat.h"
#include "Wm3FoundationLIB.h"
#include "Wm3Matrix3.h"

	namespace Wm3
	{

	template <class Real>
	class Quaternion
	{
	public:
		 // A quaternion is q = w + x*i + y*j + z*k where (w,x,y,z) is not
		 // necessarily a unit length vector in 4D.

		 // construction
		 Quaternion ();  // uninitialized
		 // this ordering is used by eigen as well
		 Quaternion (Real fW, Real fX, Real fY, Real fZ);
		 Quaternion (const Quaternion& rkQ);

		 // quaternion for the input rotation matrix
		 WM3_FUN Quaternion (const Matrix3<Real>& rkRot);

		 // quaternion for the rotation of the axis-angle pair
		 WM3_FUN Quaternion (const Vector3<Real>& rkAxis, Real fAngle);

		 // quaternion for the rotation matrix with specified columns
		 WM3_FUN Quaternion (const Vector3<Real> akRotColumn[3]);

		 // member access:  0 = w, 1 = x, 2 = y, 3 = z
		 operator const Real* () const;
		 operator Real* ();
		 // different ordering between eigen and wm3!!
		 __attribute__((warning("Quaternion_index_access: Wm3 and Eigen uses different storage order for quaternions, consider using x(), y(), z(), w() instead!")))
		 	WM3_FUN Real operator[] (int i) const;
		 __attribute__((warning("Quaternion_index_access: Wm3 and Eigen uses different storage order for quaternions, consider using x(), y(), z(), w() instead!")))
		 WM3_FUN Real& operator[] (int i);

		 WM3_FUN Real W () const;
		 WM3_FUN Real& W ();
		 WM3_FUN Real X () const;
		 WM3_FUN Real& X ();
		 WM3_FUN Real Y () const;
		 WM3_FUN Real& Y ();
		 WM3_FUN Real Z () const;
		 WM3_FUN Real& Z ();

		 EIG_FUN Real x() const{return X();}
		 EIG_FUN Real& x(){return X();}
		 EIG_FUN Real y() const{return Y();}
		 EIG_FUN Real& y(){return Y();}
		 EIG_FUN Real z() const{return Z();}
		 EIG_FUN Real& z(){return Z();}
		 EIG_FUN Real w() const{return W();}
		 EIG_FUN Real& w(){return W();}


		 // assignment
		 Quaternion& operator= (const Quaternion& rkQ);

    // comparison
    bool operator== (const Quaternion& rkQ) const;
    bool operator!= (const Quaternion& rkQ) const;
    WM3_FUN bool operator<  (const Quaternion& rkQ) const;
    WM3_FUN bool operator<= (const Quaternion& rkQ) const;
    WM3_FUN bool operator>  (const Quaternion& rkQ) const;
    WM3_FUN bool operator>= (const Quaternion& rkQ) const;

    // arithmetic operations
    Quaternion operator+ (const Quaternion& rkQ) const;
    Quaternion operator- (const Quaternion& rkQ) const;
    Quaternion operator* (const Quaternion& rkQ) const;
    Quaternion operator* (Real fScalar) const;
    Quaternion operator/ (Real fScalar) const;
    Quaternion operator- () const;

    // arithmetic updates
    Quaternion& operator+= (const Quaternion& rkQ);
    Quaternion& operator-= (const Quaternion& rkQ);
    Quaternion& operator*= (Real fScalar);
    Quaternion& operator/= (Real fScalar);

    // conversion between quaternions, matrices, and axis-angle
    WM3_FUN Quaternion& FromRotationMatrix (const Matrix3<Real>& rkRot);
    WM3_FUN void ToRotationMatrix (Matrix3<Real>& rkRot) const;
    WM3_FUN Quaternion& FromRotationMatrix (const Vector3<Real> akRotColumn[3]);
    WM3_FUN void ToRotationMatrix (Vector3<Real> akRotColumn[3]) const;
    WM3_FUN Quaternion& FromAxisAngle (const Vector3<Real>& rkAxis, Real fAngle);
    WM3_FUN void ToAxisAngle (Vector3<Real>& rkAxis, Real& rfAngle) const;

    // functions of a quaternion
    WM3_FUN Real Length () const;  // length of 4-tuple
    WM3_FUN Real SquaredLength () const;  // squared length of 4-tuple
    WM3_FUN Real Dot (const Quaternion& rkQ) const;  // dot product of 4-tuples
    WM3_FUN Real Normalize ();  // make the 4-tuple unit length
    WM3_FUN Quaternion Inverse () const;  // apply to non-zero quaternion
    WM3_FUN Quaternion Conjugate () const;
    WM3_FUN Quaternion Exp () const;  // apply to quaternion with w = 0
    WM3_FUN Quaternion Log () const;  // apply to unit-length quaternion
	 EIG_FUN void normalize() { (void)Normalize(); } 
	 EIG_FUN Quaternion conjugate() const { return Conjugate(); } 


    // rotation of a vector by a quaternion
    WM3_FUN Vector3<Real> Rotate (const Vector3<Real>& rkVector) const;

    // spherical linear interpolation
    WM3_FUN Quaternion& Slerp (Real fT, const Quaternion& rkP, const Quaternion& rkQ);

    WM3_FUN Quaternion& SlerpExtraSpins (Real fT, const Quaternion& rkP,
        const Quaternion& rkQ, int iExtraSpins);

    // intermediate terms for spherical quadratic interpolation
    WM3_FUN Quaternion& Intermediate (const Quaternion& rkQ0,
        const Quaternion& rkQ1, const Quaternion& rkQ2);

    // spherical quadratic interpolation
    WM3_FUN Quaternion& Squad (Real fT, const Quaternion& rkQ0,
        const Quaternion& rkA0, const Quaternion& rkA1,
        const Quaternion& rkQ1);

    // Compute a quaternion that rotates unit-length vector V1 to unit-length
    // vector V2.  The rotation is about the axis perpendicular to both V1 and
    // V2, with angle of that between V1 and V2.  If V1 and V2 are parallel,
    // any axis of rotation will do, such as the permutation (z2,x2,y2), where
    // V2 = (x2,y2,z2).
    WM3_FUN Quaternion& Align (const Vector3<Real>& rkV1, const Vector3<Real>& rkV2);
	 EIG_FUN Quaternion& setFromTwoVectors(const Vector3r& a, const Vector3r& b){ return Align(a,b); }

    // Decompose a quaternion into q = q_twist * q_swing, where q is 'this'
    // quaternion.  If V1 is the input axis and V2 is the rotation of V1 by
    // q, q_swing represents the rotation about the axis perpendicular to
    // V1 and V2 (see Quaternion::Align), and q_twist is a rotation about V1.
    WM3_FUN void DecomposeTwistTimesSwing (const Vector3<Real>& rkV1,
        Quaternion& rkTwist, Quaternion& rkSwing);

    // Decompose a quaternion into q = q_swing * q_twist, where q is 'this'
    // quaternion.  If V1 is the input axis and V2 is the rotation of V1 by
    // q, q_swing represents the rotation about the axis perpendicular to
    // V1 and V2 (see Quaternion::Align), and q_twist is a rotation about V1.
    WM3_FUN void DecomposeSwingTimesTwist (const Vector3<Real>& rkV1,
        Quaternion& rkSwing, Quaternion& rkTwist);

    // special values
    WM3_FUN WM3_ITEM static const Quaternion IDENTITY;  // the identity rotation
    WM3_FUN WM3_ITEM static const Quaternion ZERO;
	 EIG_FUN static const Quaternion Identity(){ return IDENTITY; }

private:
    // support for comparisons
    int CompareArrays (const Quaternion& rkQ) const;

    // support for FromRotationMatrix
    static int ms_iNext[3];

    Real m_afTuple[4];
};

template <class Real>
Quaternion<Real> operator* (Real fScalar, const Quaternion<Real>& rkQ);

#include "Wm3Quaternion.inl"

typedef Quaternion<float> Quaternionf;
typedef Quaternion<double> Quaterniond;

}

#endif

