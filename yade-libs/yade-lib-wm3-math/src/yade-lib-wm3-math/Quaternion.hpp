// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3QUATERNION_H
#define WM3QUATERNION_H

#include "Matrix3.hpp"
#include "Vector3.hpp"
//namespace Wm3
//{

template <class RealType>
class Quaternion
{
public:
	// A quaternion is q = w + x*i + y*j + z*k where (w,x,y,z) is not
	// necessarily a unit length vector in 4D.

	// construction
	Quaternion ();  // uninitialized
	Quaternion (RealType fW, RealType fX, RealType fY, RealType fZ);
	Quaternion (const Quaternion& rkQ);

	// quaternion for the input orientation matrix
	Quaternion (const Matrix3<RealType>& rkRot);

	// quaternion for the orientation of the axis-angle pair
	Quaternion (const Vector3<RealType>& rkAxis, RealType fAngle);

	// quaternion for the orientation matrix with specified columns
	Quaternion (const Vector3<RealType> akRotColumn[3]);

	// member access:  0 = w, 1 = x, 2 = y, 3 = z
	operator const RealType* () const;
	operator RealType* ();
	RealType operator[] (int i) const;
	RealType& operator[] (int i);
	__attribute__((deprecated)) inline 	RealType w() const{return W();}
	RealType W() const ;
	__attribute__((deprecated)) inline 	RealType& w(){return W();}
	RealType& W() ;
	__attribute__((deprecated)) inline 	RealType x() const{return X();}
	RealType X() const ;
	__attribute__((deprecated)) inline 	RealType& x(){return X();}
	RealType& X() ;
	__attribute__((deprecated)) inline 	RealType y() const{return Y();}
	RealType Y() const ;
	__attribute__((deprecated)) inline 	RealType& y(){return Y();}
	RealType& Y() ;
	__attribute__((deprecated)) inline 	RealType z() const{return Z();}
	RealType Z() const ;
	__attribute__((deprecated)) inline 	RealType& z(){return Z();}
	RealType& Z() ;

	// assignment
	Quaternion& operator= (const Quaternion& rkQ);

	// comparison
	bool operator== (const Quaternion& rkQ) const;
	bool operator!= (const Quaternion& rkQ) const;
	bool operator<  (const Quaternion& rkQ) const;
	bool operator<= (const Quaternion& rkQ) const;
	bool operator>  (const Quaternion& rkQ) const;
	bool operator>= (const Quaternion& rkQ) const;

	// arithmetic operations
	Quaternion operator+ (const Quaternion& rkQ) const;
	Quaternion operator- (const Quaternion& rkQ) const;
	Quaternion operator* (const Quaternion& rkQ) const;
	Quaternion operator* (RealType fScalar) const;
	Quaternion operator/ (RealType fScalar) const;
	Quaternion operator- () const;

	// arithmetic updates
	Quaternion& operator+= (const Quaternion& rkQ);
	Quaternion& operator-= (const Quaternion& rkQ);
	Quaternion& operator*= (RealType fScalar);
	Quaternion& operator/= (RealType fScalar);

	
	Vector3<RealType> operator* (const Vector3<RealType>& v) const;
	
	// conversion between quaternions, matrices, and axis-angle
	__attribute__((deprecated)) inline 	Quaternion& fromRotationMatrix(const Matrix3<RealType>& rkRot){return FromRotationMatrix(rkRot);}
	Quaternion& FromRotationMatrix(const Matrix3<RealType>& rkRot) ;
	__attribute__((deprecated)) inline 	void toRotationMatrix(Matrix3<RealType>& rkRot) const{return ToRotationMatrix(rkRot);}
	void ToRotationMatrix(Matrix3<RealType>& rkRot) const ;
	__attribute__((deprecated)) inline 	Quaternion& fromRotationMatrix(const Vector3<RealType> akRotColumn[3]){return FromRotationMatrix(akRotColumn);}
	Quaternion& FromRotationMatrix(const Vector3<RealType> akRotColumn[3]) ;
	__attribute__((deprecated)) inline 	void toRotationMatrix(Vector3<RealType> akRotColumn[3]) const{return ToRotationMatrix(akRotColumn);}
	void ToRotationMatrix(Vector3<RealType> akRotColumn[3]) const ;
	__attribute__((deprecated)) inline 	Quaternion& fromAxisAngle(const Vector3<RealType>& rkAxis, RealType fAngle){return FromAxisAngle(rkAxis,fAngle);}
	Quaternion& FromAxisAngle(const Vector3<RealType>& rkAxis, RealType fAngle) ;
	__attribute__((deprecated)) inline 	void toAxisAngle(Vector3<RealType>& rkAxis, RealType& rfAngle) const{return ToAxisAngle(rkAxis,rfAngle);}
	void ToAxisAngle(Vector3<RealType>& rkAxis, RealType& rfAngle) const ;
	
	void toAxes (Vector3<RealType>& axis1, Vector3<RealType>& axis2, Vector3<RealType>& axis3) const; 
	void fromAxes (const Vector3<RealType>& axis1,const Vector3<RealType>& axis2,const Vector3<RealType>& axis3); 
	void toGLMatrix(RealType m[16]) const;
	void toEulerAngles (Vector3<RealType>& eulerAngles,RealType threshold = 1e-06f) const;
	
	// functions of a quaternion
	__attribute__((deprecated)) inline 	RealType length() const{return Length();}
	RealType Length() const ;
	__attribute__((deprecated)) inline 	RealType squaredLength() const{return SquaredLength();}
	RealType SquaredLength() const ;
	__attribute__((deprecated)) inline 	RealType dot(const Quaternion& rkQ) const{return Dot(rkQ);}
	RealType Dot(const Quaternion& rkQ) const ;
	__attribute__((deprecated)) inline 	RealType normalize(){return Normalize();}
	RealType Normalize() ;
	__attribute__((deprecated)) inline 	Quaternion inverse() const{return Inverse();}
	Quaternion Inverse() const ;
	__attribute__((deprecated)) inline 	Quaternion conjugate() const{return Conjugate();}
	Quaternion Conjugate() const ;
	__attribute__((deprecated)) inline 	Quaternion exp() const{return Exp();}
	Quaternion Exp() const ;
	__attribute__((deprecated)) inline 	Quaternion log() const{return Log();}
	Quaternion Log() const ;
	Quaternion& power(RealType q);

	// orientation of a vector by a quaternion
	__attribute__((deprecated)) inline 	Vector3<RealType> rotate(const Vector3<RealType>& rkVector) const{return Rotate(rkVector);}
	Vector3<RealType> Rotate(const Vector3<RealType>& rkVector) const ;

	// spherical linear interpolation
	__attribute__((deprecated)) inline 	Quaternion& slerp(RealType fT, const Quaternion& rkP, const Quaternion& rkQ){return Slerp(fT,rkP,rkQ);}
	Quaternion& Slerp(RealType fT, const Quaternion& rkP, const Quaternion& rkQ) ;

	__attribute__((deprecated)) inline 	Quaternion& slerpExtraSpins(RealType fT, const Quaternion& rkP,		const Quaternion& rkQ, int iExtraSpins){return SlerpExtraSpins(fT,rkP,rkQ,iExtraSpins);}
	Quaternion& SlerpExtraSpins(RealType fT, const Quaternion& rkP,		const Quaternion& rkQ, int iExtraSpins) ;

	// intermediate terms for spherical quadratic interpolation
	__attribute__((deprecated)) inline 	Quaternion& intermediate(const Quaternion& rkQ0,		const Quaternion& rkQ1, const Quaternion& rkQ2){return Intermediate(rkQ0,rkQ1,rkQ2);}
	Quaternion& Intermediate(const Quaternion& rkQ0,		const Quaternion& rkQ1, const Quaternion& rkQ2) ;

	// spherical quadratic interpolation
	__attribute__((deprecated)) inline 	Quaternion& squad(RealType fT, const Quaternion& rkQ0,		const Quaternion& rkA0, const Quaternion& rkA1,		const Quaternion& rkQ1){return Squad(fT,rkQ0,rkA0,rkA1,rkQ1);}
	Quaternion& Squad(RealType fT, const Quaternion& rkQ0,		const Quaternion& rkA0, const Quaternion& rkA1,		const Quaternion& rkQ1) ;

	// Compute a quaternion that rotates unit-length vector V1 to unit-length
	// vector V2.  The orientation is about the axis perpendicular to both V1 and
	// V2, with angle of that between V1 and V2.  If V1 and V2 are parallel,
	// any axis of orientation will do, such as the permutation (z2,x2,y2), where
	// V2 = (x2,y2,z2).
	__attribute__((deprecated)) inline 	Quaternion& align(const Vector3<RealType>& rkV1, const Vector3<RealType>& rkV2){return Align(rkV1,rkV2);}
	Quaternion& Align(const Vector3<RealType>& rkV1, const Vector3<RealType>& rkV2) ;

	// Decompose a quaternion into q = q_twist * q_swing, where q is 'this'
	// quaternion.  If V1 is the input axis and V2 is the orientation of V1 by
	// q, q_swing represents the orientation about the axis perpendicular to
	// V1 and V2 (see Quaternion::Align), and q_twist is a orientation about V1.
	__attribute__((deprecated)) inline 	void decomposeTwistTimesSwing(const Vector3<RealType>& rkV1,		Quaternion& rkTwist, Quaternion& rkSwing){return DecomposeTwistTimesSwing(rkV1,rkTwist,rkSwing);}
	void DecomposeTwistTimesSwing(const Vector3<RealType>& rkV1,		Quaternion& rkTwist, Quaternion& rkSwing) ;

	// Decompose a quaternion into q = q_swing * q_twist, where q is 'this'
	// quaternion.  If V1 is the input axis and V2 is the orientation of V1 by
	// q, q_swing represents the orientation about the axis perpendicular to
	// V1 and V2 (see Quaternion::Align), and q_twist is a orientation about V1.
	__attribute__((deprecated)) inline 	void decomposeSwingTimesTwist(const Vector3<RealType>& rkV1,		Quaternion& rkSwing, Quaternion& rkTwist){return DecomposeSwingTimesTwist(rkV1,rkSwing,rkTwist);}
	void DecomposeSwingTimesTwist(const Vector3<RealType>& rkV1,		Quaternion& rkSwing, Quaternion& rkTwist) ;

	// special values
	static const Quaternion IDENTITY;  // the identity orientation
	static const Quaternion ZERO;

private:
	// support for comparisons
	__attribute__((deprecated)) inline 	int compareArrays(const Quaternion& rkQ) const{return CompareArrays(rkQ);}
	int CompareArrays(const Quaternion& rkQ) const ;

	// support for FromRotationMatrix
	static int ms_iNext[3];

	RealType m_afTuple[4];
};

template <class RealType>
Quaternion<RealType> operator* (RealType fScalar, const Quaternion<RealType>& rkQ);

#include "Quaternion.ipp"

typedef Quaternion<float> Quaternionf;
typedef Quaternion<double> Quaterniond;
typedef Quaternion<Real> Quaternionr;
//}

#endif

