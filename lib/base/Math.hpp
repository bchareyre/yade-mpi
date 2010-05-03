// © 2010 Václav Šmilauer <eudoxos@arcig.cz>
#pragma once

#ifdef QUAD_PRECISION
	typedef long double quad;
	typedef quad Real;
#else
	typedef double Real;
#endif

#include<limits>
#include<cstdlib>

/*
 * use Eigen http://eigen.tuxfamily.org
 */
#if defined(YADE_EIGEN) and defined(YADE_NOWM3)
	// different macros for different versions of eigen:
	//  http://bitbucket.org/eigen/eigen/issue/96/eigen_dont_align-doesnt-exist-in-205-but-appears-in-web
	#define EIGEN_DONT_VECTORIZE
	#define EIGEN_DONT_ALIGN
	#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
	#include<Eigen/Core>
	USING_PART_OF_NAMESPACE_EIGEN
	//using namespace eigen; // for eigen3
	typedef Vector2<int> Vector2i;
	typedef Vector2<Real> Vector2r;
	typedef Vector3<int> Vector3i;
	typedef Vector3<Real> Vector3r;
	typedef Matrix3<Real> Matrix3r;
	typedef Quaternion<Real> Quaternionr;

	// io
	template<class ScalarType> std::ostream & operator<<(std::ostream &os, const Vector2<ScalarType>& v){ os << v.x()<<" "<<v.y(); return os; };
	template<class ScalarType> std::ostream & operator<<(std::ostream &os, const Vector3<ScalarType>& v){ os << v.x()<<" "<<v.y()<<" "<<v.z(); return os; };
	template<class ScalarType> std::ostream & operator<<(std::ostream &os, const Quaternion<ScalarType>& q){ os<<q.w()<<" "<<q.x()<<" "<<q.y()<<" "<<q.z(); return os; };
	// operators
	//template<class ScalarType> Vector3<ScalarType> operator*(ScalarType s, const Vector3<ScalarType>& v) {return v*s;}
	//template<class ScalarType> Matrix3<ScalarType> operator*(ScalarType s, const Matrix3<ScalarType>& m) { return m*s; }
	//template<class ScalarType> Quaternion<ScalarType> operator*(ScalarType s, const Quaternion<ScalarType>& q) { return q*s; }
	template<typename ScalarType> void matrixEigenDecomposition(const Matrix3r<ScalarType> m, Matrix3<ScalarType>& mRot, Matrix3<ScalarType>& mDiag){ Eigen::EigenSolver<Matrix3<ScalarType> > a(m); mDiag=a.eigenvalues().real().asDiagonal(); mRot=a.eigenvectors().real(); }
	//__attribute__((warning("Replace this function with direct AngleAxis constructor from Quaternion")))
	template<typename ScalarType> AngleAxis<ScalarType> angleAxisFromQuat(const Quaternion<ScalarType>& q){ return AngleAxis<ScalarType>(q); }
	// http://eigen.tuxfamily.org/dox/TutorialGeometry.html
	template<typename ScalarType> Matrix3<ScalarType> matrixFromEulerAnglesXYZ(ScalarType x, ScalarType y, ScalarType z){ Matrix3<ScalarType> ret=AngleAxis<ScalarType>(x,Vector3<ScalarType>::UnitX())*AngleAxis<ScalarType>(y,Vector3<ScalarType>::UnitY())*AngleAxis<ScalarType>(z,Vector3<ScalarType>::UnitZ()); return ret; }
#else
	#include<Wm3Vector2.h>
	#include<Wm3Vector3.h>
	#include<Wm3Matrix3.h>
	#include<Wm3Quaternion.h>
	#include<Wm3Math.h>
	namespace Wm3 {
		template<class T> class Math; typedef Math<Real> Mathr;
		template<class T> class Matrix3; typedef Matrix3<Real> Matrix3r;
		template<class T> class Quaternion; typedef Quaternion<Real> Quaternionr;
		template<class T> class Vector2; typedef Vector2<Real> Vector2r; typedef Vector2<int> Vector2i;
		template<class T> class Vector3; typedef Vector3<Real> Vector3r; typedef Vector3<int> Vector3i;
		template<class T> class AngleAxis; typedef AngleAxis<Real> AngleAxisr; // added to the Wm3Quaternion header
	}
	using namespace Wm3;
	template<typename ScalarType> AngleAxis<ScalarType> angleAxisFromQuat(const Quaternion<ScalarType>& q){ AngleAxis<ScalarType> aa; q.ToAxisAngle_(aa.axis(),aa.angle()); return aa; }
	template<typename ScalarType> void matrixEigenDecomposition(const Matrix3<ScalarType>& m, Matrix3<ScalarType>& mRot, Matrix3<ScalarType>& mDiag){ m.EigenDecomposition_(mRot,mDiag); }
	template<typename ScalarType> Matrix3<ScalarType> matrixFromEulerAnglesXYZ(ScalarType x, ScalarType y, ScalarType z){ return Matrix3r().FromEulerAnglesXYZ(x,y,z); }
#endif

/*
 * Compatibility bridge for Wm3 and eigen (will be removed once Wm3 is dropped and replaced by respective eigen constructs;
 * see https://www.yade-dem.org/wiki/Wm3→Eigen
 */

// replace by outer product of 2 vectors: v1*v2.transpose();
template<typename RealType>
Matrix3<RealType> makeTensorProduct (const Vector3<RealType>& rkU,  const Vector3<RealType>& rkV)
{
	Matrix3<RealType> ret;
   ret(0,0) = rkU[0]*rkV[0];
   ret(0,1) = rkU[0]*rkV[1];
   ret(0,2) = rkU[0]*rkV[2];
   ret(1,0) = rkU[1]*rkV[0];
   ret(1,1) = rkU[1]*rkV[1];
   ret(1,2) = rkU[1]*rkV[2];
   ret(2,0) = rkU[2]*rkV[0];
   ret(2,1) = rkU[2]*rkV[1];
   ret(2,2) = rkU[2]*rkV[2];
   return ret;
}
// eigen2: v.cwise().min() / max()
// eigen3: v.array().min() / max()
template<typename ScalarType> Vector2<ScalarType> componentMaxVector(const Vector2<ScalarType>& a, const Vector2<ScalarType>& b){ return Vector2<ScalarType>(std::max(a.x(),b.x()),std::max(a.y(),b.y()));}
template<typename ScalarType> Vector2<ScalarType> componentMinVector(const Vector2<ScalarType>& a, const Vector2<ScalarType>& b){ return Vector2<ScalarType>(std::min(a.x(),b.x()),std::min(a.y(),b.y()));}
template<typename ScalarType> Vector3<ScalarType> componentMaxVector(const Vector3<ScalarType>& a, const Vector3<ScalarType>& b){ return Vector3<ScalarType>(std::max(a.x(),b.x()),std::max(a.y(),b.y()),std::max(a.z(),b.z()));}
template<typename ScalarType> Vector3<ScalarType> componentMinVector(const Vector3<ScalarType>& a, const Vector3<ScalarType>& b){ return Vector3<ScalarType>(std::min(a.x(),b.x()),std::min(a.y(),b.y()),std::min(a.z(),b.z()));}
// eigen2: v1.cwise()*v2;
// eigen3: v1.array()*v2.array()
template<typename ScalarType> Vector3<ScalarType> diagMult(const Vector3<ScalarType>& a, const Vector3<ScalarType>& b){return Vector3<ScalarType>(a.x()*b.x(),a.y()*b.y(),a.z()*b.z());}
template<typename ScalarType> Vector3<ScalarType> diagDiv(const Vector3<ScalarType>& a, const Vector3<ScalarType>& b){return Vector3<ScalarType>(a.x()/b.x(),a.y()/b.y(),a.z()/b.z());}


/*
 * Extra yade math functions and classes
 */

__attribute__((unused))
const Real NaN(std::numeric_limits<Real>::signaling_NaN());

// void quaternionToEulerAngles (const Quaternionr& q, Vector3r& eulerAngles,Real threshold=1e-6f);
template<typename ScalarType> void quaterniontoGLMatrix(const Quaternion<ScalarType>& q, ScalarType m[16]){
	ScalarType w2=2.*q.w(), x2=2.*q.x(), y2=2.*q.y(), z2=2.*q.z();
	ScalarType x2w=w2*q.w(), y2w=y2*q.w(), z2w=z2*q.w();
	ScalarType x2x=x2*q.x(), y2x=y2*q.x(), z2x=z2*q.x();
	ScalarType x2y=y2*q.y(), y2y=y2*q.y(), z2y=z2*q.y();
	ScalarType x2z=z2*q.z(), y2z=y2*q.z(), z2z=z2*q.z();
	m[0]=1.-(y2y+z2z); m[4]=y2x-z2w;      m[8]=z2x+y2w;       m[12]=0;
	m[1]=y2x+z2w;      m[5]=1.-(x2x+z2z); m[9]=z2y-x2w;       m[13]=0;
	m[2]=z2x-y2w;      m[6]=z2y+x2w;      m[10]=1.-(x2x+y2y); m[14]=0;
	m[3]=0.;           m[7]=0.;           m[11]=0.;           m[15]=1.;
}



// se3
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

// functions
template<typename ScalarType> ScalarType unitVectorsAngle(const Vector3<ScalarType>& a, const Vector3<ScalarType>& b){ return acos(a.dot(b)); }
// operators
template<class ScalarType> Vector3<ScalarType> operator*(const Quaternion<ScalarType>& q, const Vector3<ScalarType>& v){ return q.rotate(v); /* check whether it converts to matrix internally, which is supposedly faster*/ }

/*
 * typedefs
 */
typedef Se3<Real> Se3r;


/*
 * Serialization of math classes
 */


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


