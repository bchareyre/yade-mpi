// © 2010 Václav Šmilauer <eudoxos@arcig.cz>
#pragma once

#ifdef QUAD_PRECISION
	using quad = long double;
	using Real = quad;
#else
	using Real = double;
#endif

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using std::endl;
using std::cout;
using std::cerr;
using std::vector;
using std::string;
using std::list;
using std::pair;
using std::min;
using std::max;
using std::set;
using std::map;
using std::type_info;
using std::ifstream;
using std::ofstream;
using std::runtime_error;
using std::logic_error;
using std::invalid_argument;
using std::ios;
using std::ios_base;
using std::fstream;
using std::ostream;
using std::ostringstream;
using std::istringstream;
using std::swap;
using std::make_pair;

#include <boost/lexical_cast.hpp>
#include <boost/python.hpp>
#include <boost/python/object.hpp>
#include <boost/type_traits.hpp>
#include <boost/preprocessor.hpp>
#include <boost/python/module.hpp>
#include <boost/python/class.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/numeric/conversion/bounds.hpp>

using boost::shared_ptr;

#ifndef FOREACH
	#define FOREACH BOOST_FOREACH
#endif

#ifndef YADE_PTR_CAST
	#define YADE_PTR_CAST boost::static_pointer_cast
#endif

#ifndef YADE_CAST
	#define YADE_CAST static_cast
#endif

#ifndef YADE_PTR_DYN_CAST
	#define YADE_PTR_DYN_CAST boost::dynamic_pointer_cast
#endif

#define EIGEN_DONT_PARALLELIZE

#ifdef YADE_MASK_ARBITRARY
	#include <bitset>
#endif

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Eigenvalues>
#include <float.h>


template<typename Scalar> using Vector2 = Eigen::Matrix<Scalar,2,1>;
using Vector2i = Vector2<int>;
using Vector2r = Vector2<Real>;

template<typename Scalar> using Vector3 = Eigen::Matrix<Scalar,3,1>;
using Vector3i = Vector3<int>;
using Vector3r = Vector3<Real>;

template<typename Scalar> using Vector6 = Eigen::Matrix<Scalar,6,1>;
using Vector6i = Vector6<int>;
using Vector6r = Vector6<Real>;

template<typename Scalar> using Matrix3 = Eigen::Matrix<Scalar,3,3>;
using Matrix3i = Matrix3<int>;
using Matrix3r = Matrix3<Real>;

template<typename Scalar> using Matrix6 = Eigen::Matrix<Scalar,6,6>;
using Matrix6i = Matrix6<int>;
using Matrix6r = Matrix6<Real>;

using Quaternionr = Eigen::Quaternion<Real>;
using AngleAxisr  = Eigen::AngleAxis<Real>;
using AlignedBox2r = Eigen::AlignedBox<Real,2>;
using AlignedBox3r = Eigen::AlignedBox<Real,3>;

using Eigen::AngleAxis;
using Eigen::Quaternion;

// in some cases, we want to initialize types that have no default constructor (OpenMPAccumulator, for instance)
// template specialization will help us here
template<typename EigenMatrix> EigenMatrix ZeroInitializer(){ return EigenMatrix::Zero(); };
template<> int ZeroInitializer<int>();
template<> Real ZeroInitializer<Real>();

// io
template<class Scalar> std::ostream & operator<<(std::ostream &os, const Vector2<Scalar>& v) {
  os << v.x()<<" "<<v.y();
  return os;
}

template<class Scalar> std::ostream & operator<<(std::ostream &os, const Vector3<Scalar>& v) {
  os << v.x()<<" "<<v.y()<<" "<<v.z();
  return os;
}

template<class Scalar> std::ostream & operator<<(std::ostream &os, const Vector6<Scalar>& v) {
  os << v[0]<<" "<<v[1]<<" "<<v[2]<<" "<<v[3]<<" "<<v[4]<<" "<<v[5];
  return os;
}

template<class Scalar> std::ostream & operator<<(std::ostream &os, const Eigen::Quaternion<Scalar>& q) {
  os<<q.w()<<" "<<q.x()<<" "<<q.y()<<" "<<q.z();
  return os;
}

template<typename Scalar>
struct Math{
	static const Scalar PI;
	static const Scalar HALF_PI;
	static const Scalar TWO_PI;
	static const Scalar MAX_REAL;
	static const Scalar DEG_TO_RAD;
	static const Scalar RAD_TO_DEG;
	static const Scalar EPSILON;
	static const Scalar ZERO_TOLERANCE;
	static Scalar Sign(Scalar f){ if(f<0) return -1; if(f>0) return 1; return 0; }

	static Scalar UnitRandom(){ return ((double)rand()/((double)(RAND_MAX))); }
	static Scalar SymmetricRandom(){ return 2.*(((double)rand())/((double)(RAND_MAX)))-1.; }
	static Scalar FastInvCos0(Scalar fValue) {
	  Scalar fRoot = sqrt(((Scalar)1.0)-fValue);
	  Scalar fResult = -(Scalar)0.0187293;
	  fResult *= fValue; fResult += (Scalar)0.0742610;
	  fResult *= fValue; fResult -= (Scalar)0.2121144;
	  fResult *= fValue; fResult += (Scalar)1.5707288;
	  fResult *= fRoot;
	  return fResult;
	 }
};
using Mathr = Math<Real>;

/* this was removed in eigen3, see http://forum.kde.org/viewtopic.php?f=74&t=90914 */
template<typename MatrixT>
void Matrix_computeUnitaryPositive(const MatrixT& in, MatrixT* unitary, MatrixT* positive){
	assert(unitary); assert(positive);
	//Eigen::JacobiSVD<MatrixT> svd(in, Eigen::ComputeThinU | Eigen::ComputeThinV);
	Eigen::JacobiSVD<MatrixT> svd(in, Eigen::ComputeFullU | Eigen::ComputeFullV);
	MatrixT mU, mV, mS;
	mU = svd.matrixU();
		mV = svd.matrixV();
		mS = svd.singularValues().asDiagonal();

	*unitary=mU * mV.adjoint();
	*positive=mV * mS * mV.adjoint();
}

template<typename MatrixT>
void matrixEigenDecomposition(const MatrixT& m, MatrixT& mRot, MatrixT& mDiag){
	//assert(mRot); assert(mDiag);
	Eigen::SelfAdjointEigenSolver<MatrixT> a(m); mRot=a.eigenvectors(); mDiag=a.eigenvalues().asDiagonal();
}

/* convert Vector6r in the Voigt notation to corresponding 2nd order symmetric tensor (stored as Matrix3r)
	if strain is true, then multiply non-diagonal parts by .5
*/
template<typename Scalar>
Matrix3<Scalar> voigt_toSymmTensor(const Vector6<Scalar>& v, bool strain=false){
	Real k=(strain?.5:1.);
	Matrix3<Scalar> ret; ret<<v[0],k*v[5],k*v[4], k*v[5],v[1],k*v[3], k*v[4],k*v[3],v[2]; return ret;
}
/* convert 2nd order tensor to 6-vector (Voigt notation), symmetrizing the tensor;
	if strain is true, multiply non-diagonal compoennts by 2.
*/
template<typename Scalar>
Vector6<Scalar> tensor_toVoigt(const Matrix3<Scalar>& m, bool strain=false){
	int k=(strain?2:1);
	Vector6<Scalar> ret; ret<<m(0,0),m(1,1),m(2,2),k*.5*(m(1,2)+m(2,1)),k*.5*(m(2,0)+m(0,2)),k*.5*(m(0,1)+m(1,0)); return ret;
}

const Real NaN(std::numeric_limits<Real>::signaling_NaN());

// void quaternionToEulerAngles (const Quaternionr& q, Vector3r& eulerAngles,Real threshold=1e-6f);
template<typename Scalar> void quaterniontoGLMatrix(const Quaternion<Scalar>& q, Scalar m[16]){
	Scalar w2=2.*q.w(), x2=2.*q.x(), y2=2.*q.y(), z2=2.*q.z();
	Scalar x2w=w2*q.w(), y2w=y2*q.w(), z2w=z2*q.w();
	Scalar x2x=x2*q.x(), y2x=y2*q.x(), z2x=z2*q.x();
	Scalar x2y=y2*q.y(), y2y=y2*q.y(), z2y=z2*q.y();
	Scalar x2z=z2*q.z(), y2z=y2*q.z(), z2z=z2*q.z();
	m[0]=1.-(y2y+z2z); m[4]=y2x-z2w;      m[8]=z2x+y2w;       m[12]=0;
	m[1]=y2x+z2w;      m[5]=1.-(x2x+z2z); m[9]=z2y-x2w;       m[13]=0;
	m[2]=z2x-y2w;      m[6]=z2y+x2w;      m[10]=1.-(x2x+y2y); m[14]=0;
	m[3]=0.;           m[7]=0.;           m[11]=0.;           m[15]=1.;
}

// se3
template <class Scalar>
class Se3
{
	public :
		Vector3<Scalar>	position;
		Quaternion<Scalar>	orientation;
		Se3(){};
		Se3(Vector3<Scalar> rkP, Quaternion<Scalar> qR){ position = rkP; orientation = qR; }
		Se3(const Se3<Scalar>& s){position = s.position;orientation = s.orientation;}
		Se3(Se3<Scalar>& a,Se3<Scalar>& b){
			position  = b.orientation.inverse()*(a.position - b.position);  
			orientation = b.orientation.inverse()*a.orientation;
		}
		Se3<Scalar> inverse(){ return Se3(-(orientation.inverse()*position), orientation.inverse());}
		void toGLMatrix(float m[16]){ orientation.toGLMatrix(m); m[12] = position[0]; m[13] = position[1]; m[14] = position[2];}
		Vector3<Scalar> operator * (const Vector3<Scalar>& b ){return orientation*b+position;}
		Se3<Scalar> operator * (const Quaternion<Scalar>& b ){return Se3<Scalar>(position , orientation*b);}
		Se3<Scalar> operator * (const Se3<Scalar>& b ){return Se3<Scalar>(orientation*b.position+position,orientation*b.orientation);}
};

// functions
template<typename Scalar> Scalar unitVectorsAngle(const Vector3<Scalar>& a, const Vector3<Scalar>& b){ return acos(a.dot(b)); }
// operators


/*
 * Mask
 */
#ifdef YADE_MASK_ARBITRARY
using mask_t = std::bitset<YADE_MASK_ARBITRARY_SIZE>;
bool operator==(const mask_t& g, int i);
bool operator==(int i, const mask_t& g);
bool operator!=(const mask_t& g, int i);
bool operator!=(int i, const mask_t& g);
mask_t operator&(const mask_t& g, int i);
mask_t operator&(int i, const mask_t& g);
mask_t operator|(const mask_t& g, int i);
mask_t operator|(int i, const mask_t& g);
bool operator||(const mask_t& g, bool b);
bool operator||(bool b, const mask_t& g);
bool operator&&(const mask_t& g, bool b);
bool operator&&(bool b, const mask_t& g);
#else
using mask_t = int;
#endif

using Se3r = Se3<Real>;

/*
 * Serialization of math classes
 */

#include<boost/serialization/nvp.hpp>
#include<boost/serialization/is_bitwise_serializable.hpp>

// fast serialization (no version infor and no tracking) for basic math types
// http://www.boost.org/doc/libs/1_42_0/libs/serialization/doc/traits.html#bitwise
BOOST_IS_BITWISE_SERIALIZABLE(Vector2r);
BOOST_IS_BITWISE_SERIALIZABLE(Vector2i);
BOOST_IS_BITWISE_SERIALIZABLE(Vector3r);
BOOST_IS_BITWISE_SERIALIZABLE(Vector3i);
BOOST_IS_BITWISE_SERIALIZABLE(Vector6r);
BOOST_IS_BITWISE_SERIALIZABLE(Vector6i);
BOOST_IS_BITWISE_SERIALIZABLE(Quaternionr);
BOOST_IS_BITWISE_SERIALIZABLE(Se3r);
BOOST_IS_BITWISE_SERIALIZABLE(Matrix3r);
BOOST_IS_BITWISE_SERIALIZABLE(Matrix6r);

namespace boost {
namespace serialization {

template<class Archive>
void serialize(Archive & ar, Vector2r & g, const unsigned int version){
	Real &x=g[0], &y=g[1];
	ar & BOOST_SERIALIZATION_NVP(x) & BOOST_SERIALIZATION_NVP(y);
}

template<class Archive>
void serialize(Archive & ar, Vector2i & g, const unsigned int version){
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
void serialize(Archive & ar, Vector3i & g, const unsigned int version){
	int &x=g[0], &y=g[1], &z=g[2];
	ar & BOOST_SERIALIZATION_NVP(x) & BOOST_SERIALIZATION_NVP(y) & BOOST_SERIALIZATION_NVP(z);
}

template<class Archive>
void serialize(Archive & ar, Vector6r & g, const unsigned int version){
	Real &v0=g[0], &v1=g[1], &v2=g[2], &v3=g[3], &v4=g[4], &v5=g[5];
	ar & BOOST_SERIALIZATION_NVP(v0) & BOOST_SERIALIZATION_NVP(v1) & BOOST_SERIALIZATION_NVP(v2) & BOOST_SERIALIZATION_NVP(v3) & BOOST_SERIALIZATION_NVP(v4) & BOOST_SERIALIZATION_NVP(v5);
}

template<class Archive>
void serialize(Archive & ar, Vector6i & g, const unsigned int version){
	int &v0=g[0], &v1=g[1], &v2=g[2], &v3=g[3], &v4=g[4], &v5=g[5];
	ar & BOOST_SERIALIZATION_NVP(v0) & BOOST_SERIALIZATION_NVP(v1) & BOOST_SERIALIZATION_NVP(v2) & BOOST_SERIALIZATION_NVP(v3) & BOOST_SERIALIZATION_NVP(v4) & BOOST_SERIALIZATION_NVP(v5);
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

template<class Archive>
void serialize(Archive & ar, Matrix6r & m, const unsigned int version){
	Real &m00=m(0,0), &m01=m(0,1), &m02=m(0,2), &m03=m(0,3), &m04=m(0,4), &m05=m(0,5);
	Real &m10=m(1,0), &m11=m(1,1), &m12=m(1,2), &m13=m(1,3), &m14=m(1,4), &m15=m(1,5);
	Real &m20=m(2,0), &m21=m(2,1), &m22=m(2,2), &m23=m(2,3), &m24=m(2,4), &m25=m(2,5);
	Real &m30=m(3,0), &m31=m(3,1), &m32=m(3,2), &m33=m(3,3), &m34=m(3,4), &m35=m(3,5);
	Real &m40=m(4,0), &m41=m(4,1), &m42=m(4,2), &m43=m(4,3), &m44=m(4,4), &m45=m(4,5);
	Real &m50=m(5,0), &m51=m(5,1), &m52=m(5,2), &m53=m(5,3), &m54=m(5,4), &m55=m(5,5);
	ar & BOOST_SERIALIZATION_NVP(m00) & BOOST_SERIALIZATION_NVP(m01) & BOOST_SERIALIZATION_NVP(m02) & BOOST_SERIALIZATION_NVP(m03) & BOOST_SERIALIZATION_NVP(m04) & BOOST_SERIALIZATION_NVP(m05) &
	   BOOST_SERIALIZATION_NVP(m10) & BOOST_SERIALIZATION_NVP(m11) & BOOST_SERIALIZATION_NVP(m12) & BOOST_SERIALIZATION_NVP(m13) & BOOST_SERIALIZATION_NVP(m14) & BOOST_SERIALIZATION_NVP(m15) &
	   BOOST_SERIALIZATION_NVP(m20) & BOOST_SERIALIZATION_NVP(m21) & BOOST_SERIALIZATION_NVP(m22) & BOOST_SERIALIZATION_NVP(m23) & BOOST_SERIALIZATION_NVP(m24) & BOOST_SERIALIZATION_NVP(m25) &
	   BOOST_SERIALIZATION_NVP(m30) & BOOST_SERIALIZATION_NVP(m31) & BOOST_SERIALIZATION_NVP(m32) & BOOST_SERIALIZATION_NVP(m33) & BOOST_SERIALIZATION_NVP(m34) & BOOST_SERIALIZATION_NVP(m35) &
	   BOOST_SERIALIZATION_NVP(m40) & BOOST_SERIALIZATION_NVP(m41) & BOOST_SERIALIZATION_NVP(m42) & BOOST_SERIALIZATION_NVP(m43) & BOOST_SERIALIZATION_NVP(m44) & BOOST_SERIALIZATION_NVP(m45) &
	   BOOST_SERIALIZATION_NVP(m50) & BOOST_SERIALIZATION_NVP(m51) & BOOST_SERIALIZATION_NVP(m52) & BOOST_SERIALIZATION_NVP(m53) & BOOST_SERIALIZATION_NVP(m54) & BOOST_SERIALIZATION_NVP(m55);
}

#ifdef YADE_MASK_ARBITRARY
template<class Archive>
void serialize(Archive & ar, mask_t& v, const unsigned int version){
	std::string str = v.to_string();
	ar & BOOST_SERIALIZATION_NVP(str);
	v = mask_t(str);
}
#endif

} // namespace serialization
} // namespace boost
