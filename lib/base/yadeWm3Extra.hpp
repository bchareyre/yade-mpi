#ifndef YADE_WM3_EXTRA_HPP
#define YADE_WM3_EXTRA_HPP

#include<Wm3Vector2.h>
#include<Wm3Vector3.h>
#include<Wm3Vector4.h>
#include<Wm3Matrix2.h>
#include<Wm3Matrix3.h>
#include<Wm3Matrix4.h>
#include<Wm3Quaternion.h>
using namespace Wm3;

#include"yadeWm3.hpp"


/*************************************** OPERATORS *********************************************/

Vector2r operator*(Real fScalar, const Vector2r& rkV);
template<class RealType1, class RealType2>
Vector3<RealType2> operator* (RealType1 fScalar, const Vector3<RealType2>& rkV);

/*__attribute__((deprecated)) Vector3f operator*(const double s, const Vector3f& v);
__attribute__((deprecated)) Vector3d operator*(const float s, const Vector3d& v);
__attribute__((deprecated)) Vector3f operator*(const Vector3f& v, const double s);
__attribute__((deprecated)) Vector3d operator*(const Vector3d& v, const float s);*/

std::ostream & operator<< (std::ostream &os, const Vector3r &v);
std::ostream & operator<< (std::ostream &os, const Quaternionr &q);

template<class RealType1, class RealType2>
Vector4<RealType2> operator* (RealType1 fScalar, const Vector4<RealType2>& rkV);

Matrix2r operator*(Real fScalar, const Matrix2r& rkM);
Vector2r operator*(const Vector2r& rkV, const Matrix2r& rkM);
Matrix3r operator*(Real fScalar, const Matrix3r& rkM);
Vector3r operator*(const Vector3r rkV, const Matrix3r rkM);
Matrix4r operator*(Real fScalar, const Matrix4r& rkM);
Vector4r operator*(const Vector4r& rkV, const Matrix4r& rkM);

template<class RealType1, class RealType2>
Quaternion<RealType2> operator* (RealType1 fScalar, const Quaternion<RealType2>& rkQ);

Vector3r operator*(const Quaternionr& q, const Vector3r& v);

/************************* END OF OPERATORS ************************/

/************************************** SE3 class *******************************/

template <class RealType>
class Se3
{
	public :
		Vector3<RealType>	position;
		Quaternion<RealType>	orientation;

		Se3(){};
		Se3(Vector3<RealType> rkP, Quaternion<RealType> qR){ position = rkP; orientation = qR; }
		Se3(const Se3<RealType>& s){position = s.position;orientation = s.orientation;}
		Se3(Se3<RealType>& a,Se3<RealType>& b){
			position  = b.orientation.inverse()*(a.position - b.position);  
			orientation = b.orientation.inverse()*a.orientation;
		}
		Se3<RealType> inverse(){ return Se3(-(orientation.inverse()*position), orientation.inverse());}

		void toGLMatrix(float m[16]){ orientation.toGLMatrix(m); m[12] = position[0]; m[13] = position[1]; m[14] = position[2];}

		Vector3<RealType> operator * (const Vector3<RealType>& b ){return orientation*b+position;}
		Se3<RealType> operator * (const Quaternion<RealType>& b ){return Se3<RealType>(position , orientation*b);}
		Se3<RealType> operator * (const Se3<RealType>& b ){return Se3<RealType>(orientation*b.position+position,orientation*b.orientation);}
};

typedef Se3<float> Se3f;
typedef Se3<double> Se3d;
typedef Se3<Real> Se3r;

/************************************* end SE3 class ****************************/


/*Vector2 std::maxVector (const Vector2& rkV) const;
Vector2 std::minVector (const Vector2& rkV) const;
Vector2 multDiag (const Vector2& rkV) const;*/

Vector2r componentMaxVector(const Vector2r& a, const Vector2r& rkV);
Vector2r componentMinVector(const Vector2r& a, const Vector2r& rkV);
Vector2r diagMult(const Vector2r& a, const Vector2r& rkV);



/*RealType angleBetweenUnitVectors(const Vector3r& rkV) const;
Vector3r std::maxVector (const Vector3r& rkV) const;
Vector3r std::minVector (const Vector3r& rkV) const;
Vector3r multDiag (const Vector3r& rkV) const;
Vector3r divDiag (const Vector3r& rkV) const;*/

Vector3r componentMaxVector (const Vector3r& a, const Vector3r& rkV);
Vector3r componentMinVector (const Vector3r& a, const Vector3r& rkV);
Vector3r diagMult (const Vector3r& a, const Vector3r& rkV);
Vector3r diagDiv (const Vector3r& a, const Vector3r& rkV);
Real unitVectorsAngle(const Vector3r& a, const Vector3r& rkV);

Real componentSum(const Vector3r& v);



/*Vector4r std::maxVector (const Vector4r& rkV) const;
Vector4r std::minVector (const Vector4r& rkV) const;
Vector4r multDiag (const Vector4r& rkV) const;*/
Vector4r componentMaxVector (const Vector4r& a, const Vector4r& rkV);
Vector4r componentMinVector (const Vector4r& a, const Vector4r& rkV);
Vector4r diagMult (const Vector4r& a, const Vector4r& rkV);
//template<class RealType1, class RealType2>


/*void toAxes (Vector3r& axis1, Vector3r& axis2, Vector3r& axis3) const; 
void fromAxes (const Vector3r& axis1,const Vector3r& axis2,const Vector3r& axis3); 
void toGLMatrix(RealType m[16]) const;
void toEulerAngles (Vector3r& eulerAngles,RealType threshold = 1e-06f) const;
Vector3r operator* (const Vector3r& v) const;*/


Quaternionr quaternionFromAxes (const Vector3r& axis1,const Vector3r& axis2,const Vector3r& axis3);
void quaternionToAxes(const Quaternionr& q, Vector3r& axis1, Vector3r& axis2, Vector3r& axis3);
void quaternionToEulerAngles (const Quaternionr& q, Vector3r& eulerAngles,Real threshold=1e-6f);
void quaterniontoGLMatrix(const Quaternionr& q, Real m[16]);




#endif
