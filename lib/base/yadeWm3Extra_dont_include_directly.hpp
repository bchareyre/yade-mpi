#pragma once 

#include<Wm3Vector2.h>
#include<Wm3Vector3.h>
#include<Wm3Matrix3.h>
#include<Wm3Quaternion.h>
using namespace Wm3;

// this will be replaced by AngleAxisr(Quaternionr) ctor once we use Eigen everywhere
template<typename RealType>
AngleAxis<RealType> angleAxisFromQuat(const Quaternion<RealType>& q){ AngleAxis<RealType> aa; q.ToAxisAngle(aa.axis(),aa.angle()); return aa; }

#include"yadeWm3_dont_include_directly.hpp"
#include<limits>


/*************************************** OPERATORS *********************************************/

Vector2r operator*(Real fScalar, const Vector2r& rkV);
template<class RealType1, class RealType2>
Vector3<RealType2> operator* (RealType1 fScalar, const Vector3<RealType2>& rkV);

std::ostream & operator<< (std::ostream &os, const Vector3r &v);
std::ostream & operator<< (std::ostream &os, const Quaternionr &q);

Matrix3r operator*(Real fScalar, const Matrix3r& rkM);
Vector3r operator*(const Vector3r rkV, const Matrix3r rkM);

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

__attribute__((unused))
const Real NaN(std::numeric_limits<Real>::signaling_NaN());

/************************************* end SE3 class ****************************/


Vector2r componentMaxVector(const Vector2r& a, const Vector2r& rkV);
Vector2r componentMinVector(const Vector2r& a, const Vector2r& rkV);
Vector2r diagMult(const Vector2r& a, const Vector2r& rkV);

Vector3r componentMaxVector (const Vector3r& a, const Vector3r& rkV);
Vector3r componentMinVector (const Vector3r& a, const Vector3r& rkV);
Vector3r diagMult (const Vector3r& a, const Vector3r& rkV);
Vector3r diagDiv (const Vector3r& a, const Vector3r& rkV);
Real unitVectorsAngle(const Vector3r& a, const Vector3r& rkV);

Real componentSum(const Vector3r& v);



Quaternionr quaternionFromAxes (const Vector3r& axis1,const Vector3r& axis2,const Vector3r& axis3);
void quaternionToAxes(const Quaternionr& q, Vector3r& axis1, Vector3r& axis2, Vector3r& axis3);
void quaternionToEulerAngles (const Quaternionr& q, Vector3r& eulerAngles,Real threshold=1e-6f);
void quaterniontoGLMatrix(const Quaternionr& q, Real m[16]);

// gccxml chokes on the boost::serialization code; this part is not needed if wrapping miniWm3 anyway
#if !defined(__GCCXML__) and defined(YADE_BOOST_SERIALIZATION)

#include<boost/serialization/nvp.hpp>
#include<boost/serialization/is_bitwise_serializable.hpp>

// fast serialization (no version infor and no tracking) for basic math types
// http://www.boost.org/doc/libs/1_42_0/libs/serialization/doc/traits.html#bitwise
BOOST_IS_BITWISE_SERIALIZABLE(Vector2r);
BOOST_IS_BITWISE_SERIALIZABLE(Vector2<int>);
BOOST_IS_BITWISE_SERIALIZABLE(Vector3r);
BOOST_IS_BITWISE_SERIALIZABLE(Vector3<int>);
BOOST_IS_BITWISE_SERIALIZABLE(Quaternionr);
BOOST_IS_BITWISE_SERIALIZABLE(Se3r);
BOOST_IS_BITWISE_SERIALIZABLE(Matrix3r);

namespace boost {
namespace serialization {

template<class Archive>
void serialize(Archive & ar, Vector2r & g, const unsigned int version){
	Real &x=g[0], &y=g[1];
	ar & BOOST_SERIALIZATION_NVP(x) & BOOST_SERIALIZATION_NVP(y);
}

template<class Archive>
void serialize(Archive & ar, Vector2<int> & g, const unsigned int version){
	int &x=g[0], &y=g[1];
	ar & BOOST_SERIALIZATION_NVP(x) & BOOST_SERIALIZATION_NVP(y);
}

template<class Archive>
void serialize(Archive & ar, Vector3r & g, const unsigned int version)
{
	Real &x=g[0], &y=g[1], &z=g[2];
	ar & BOOST_SERIALIZATION_NVP(x) & BOOST_SERIALIZATION_NVP(y) & BOOST_SERIALIZATION_NVP(z);
}

template<class Archive>
void serialize(Archive & ar, Vector3<int> & g, const unsigned int version){
	int &x=g[0], &y=g[1], &z=g[2];
	ar & BOOST_SERIALIZATION_NVP(x) & BOOST_SERIALIZATION_NVP(y) & BOOST_SERIALIZATION_NVP(z);
}

template<class Archive>
void serialize(Archive & ar, Quaternionr & g, const unsigned int version)
{
	Real &w=g.w(), &x=g.x(), &y=g.y(), &z=g.z();
	ar & BOOST_SERIALIZATION_NVP(w) & BOOST_SERIALIZATION_NVP(x) & BOOST_SERIALIZATION_NVP(y) & BOOST_SERIALIZATION_NVP(z);
}

template<class Archive>
void serialize(Archive & ar, Se3r & g, const unsigned int version){
	Vector3r& position=g.position; Quaternionr& orientation=g.orientation;
	ar & BOOST_SERIALIZATION_NVP(position) & BOOST_SERIALIZATION_NVP(orientation);
}

template<class Archive>
void serialize(Archive & ar, Matrix3r & m, const unsigned int version){
	Real &m00=m(0,0), &m01=m(0,1), &m02=m(0,2), &m10=m(1,0), &m11=m(1,1), &m12=m(1,2), &m20=m(2,0), &m21=m(2,1), &m22=m(2,2);
	ar & BOOST_SERIALIZATION_NVP(m00) & BOOST_SERIALIZATION_NVP(m01) & BOOST_SERIALIZATION_NVP(m02) &
		BOOST_SERIALIZATION_NVP(m10) & BOOST_SERIALIZATION_NVP(m11) & BOOST_SERIALIZATION_NVP(m12) &
		BOOST_SERIALIZATION_NVP(m20) & BOOST_SERIALIZATION_NVP(m21) & BOOST_SERIALIZATION_NVP(m22);
}

} // namespace serialization
} // namespace boost

#endif /* __GCCXML */

