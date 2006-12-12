#include "yadeWm3Extra.hpp"
// for std::min and std::max
#include<cstdlib>

/*consider moving all the functions here to the hpp and declare them as extern inline. */


/**************** explicit instantiations ****************************/
/* this template garbage will go away once we dump Real and go with double forver */
template Vector3<float> operator*<double, float>(double, Wm3::Vector3<float> const&);

/****** end of explicit instantiations *******/



Vector2r operator*(Real fScalar, const Vector2r& rkV){return Vector2r(fScalar*rkV[0],fScalar*rkV[1]);}

template<class RealType1, class RealType2>
Vector3<RealType2> operator* (RealType1 fScalar, const Vector3<RealType2>& rkV){ return Vector3<RealType2>(fScalar*rkV[0],fScalar*rkV[1],fScalar*rkV[2]);}

/*__attribute__((deprecated)) Vector3f operator*(const double s, const Vector3f& v){return Vector3f(s*v[0],s*v[1],s*v[2]);}
__attribute__((deprecated)) Vector3d operator*(const float s, const Vector3d& v){return Vector3d(s*v[0],s*v[1],s*v[2]);}
__attribute__((deprecated)) Vector3f operator*(const Vector3f& v, const double s){return Vector3f(s*v[0],s*v[1],s*v[2]);}
__attribute__((deprecated)) Vector3d operator*(const Vector3d& v, const float s){return Vector3d(s*v[0],s*v[1],s*v[2]);}*/

std::ostream & operator<< (std::ostream &os, const Vector3r &v){ return os << v[0] << " " << v[1] << " " << v[2];}

template<class RealType1, class RealType2>
Vector4<RealType2> operator* (RealType1 fScalar, const Vector4<RealType2>& rkV){ return Vector4<RealType2>(fScalar*rkV[0],fScalar*rkV[1],fScalar*rkV[2],fScalar*rkV[3]);}



Matrix2r operator*(Real fScalar, const Matrix2r& rkM){return rkM*fScalar;}
// v^T * M
Vector2r operator* (const Vector2r& rkV, const Matrix2r& rkM){
	 return Vector2r(rkV[0]*rkM[0][0] + rkV[1]*rkM[1][0],rkV[0]*rkM[0][1] + rkV[1]*rkM[1][1]);
}

// c * M
Matrix3r operator* (Real fScalar, const Matrix3r& rkM){return rkM*fScalar;}
// v^T * M
Vector3r operator* (const Vector3r rkV, const Matrix3r rkM){
	return Vector3r(rkV[0]*rkM[0][0] + rkV[1]*rkM[1][0] + rkV[2]*rkM[2][0],
	rkV[0]*rkM[0][1] + rkV[1]*rkM[1][1] + rkV[2]*rkM[2][1],
	rkV[0]*rkM[0][2] + rkV[1]*rkM[1][2] + rkV[2]*rkM[2][2]);
}

// c * M
Matrix4r operator* (Real fScalar, const Matrix4r& rkM){return rkM*fScalar;}
// v^T * M
Vector4r operator* (const Vector4r& rkV, const Matrix4r& rkM){
	return Vector4r(rkV[0]*rkM[0][0]+rkV[1]*rkM[1][0]+rkV[2]*rkM[2][0]+rkV[3]*rkM[3][0],
	rkV[0]*rkM[0][1]+rkV[1]*rkM[1][1]+rkV[2]*rkM[2][1]+rkV[3]*rkM[3][1],
	rkV[0]*rkM[0][2]+rkV[1]*rkM[1][2]+rkV[2]*rkM[2][2]+rkV[3]*rkM[3][2],
	rkV[0]*rkM[0][3]+rkV[1]*rkM[1][3]+rkV[2]*rkM[2][3]+rkV[3]*rkM[3][3]);
}


template<class RealType1, class RealType2>
Quaternion<RealType2> operator* (RealType1 fScalar, const Quaternion<RealType2>& rkQ){
	 Quaternion<RealType2> kProd;
	 for (int i = 0; i < 4; i++) kProd[i] = fScalar*rkQ[i];
	 return kProd;
}


Vector3r operator* (const Quaternionr& q, const Vector3r& v)
{
	 // Given a vector u = (x0,y0,z0) and a unit length quaternion
	 // q = <w,x,y,z>, the vector v = (x1,y1,z1) which represents the
	 // orientation of u by q is v = q*u*q^{-1} where * indicates quaternion
	 // multiplication and where u is treated as the quaternion <0,x0,y0,z0>.
	 // Note that q^{-1} = <w,-x,-y,-z>, so no float work is required to
	 // invert q.  Now
	 //
	 //   q*u*q^{-1} = q*<0,x0,y0,z0>*q^{-1}
	 //     = q*(x0*i+y0*j+z0*k)*q^{-1}
	 //     = x0*(q*i*q^{-1})+y0*(q*j*q^{-1})+z0*(q*k*q^{-1})
	 //
	 // As 3-vectors, q*i*q^{-1}, q*j*q^{-1}, and 2*k*q^{-1} are the columns
	 // of the orientation matrix computed in Quaternion::ToRotationMatrix.
	 // The vector v is obtained as the product of that orientation matrix with
	 // vector u.  As such, the quaternion representation of a orientation
	 // matrix requires less space than the matrix and more time to compute
	 // the rotated vector.  Typical space-time tradeoff...

	 Matrix3r m;
	 q.ToRotationMatrix(m);
	 return m*v;
}



/************************* END OF OPERATORS ************************/




/*Vector2 std::maxVector (const Vector2& rkV) const;
Vector2 std::minVector (const Vector2& rkV) const;
Vector2 multDiag (const Vector2& rkV) const;*/

Vector2r componentMaxVector(const Vector2r& a, const Vector2r& rkV) {return Vector2r(std::max(a.X(),rkV.X()),std::max(a.Y(),rkV.Y()));}
Vector2r componentMinVector(const Vector2r& a, const Vector2r& rkV)  {return Vector2r(std::min(a.X(),rkV.X()),std::min(a.Y(),rkV.Y()));}
Vector2r diagMult(const Vector2r& a, const Vector2r& rkV)  { return Vector2r(a.X()*rkV.X(),a.Y()*rkV.Y());}

/*RealType angleBetweenUnitVectors(const Vector3r& rkV) const;
Vector3r std::maxVector (const Vector3r& rkV) const;
Vector3r std::minVector (const Vector3r& rkV) const;
Vector3r multDiag (const Vector3r& rkV) const;
Vector3r divDiag (const Vector3r& rkV) const;*/

Vector3r componentMaxVector (const Vector3r& a, const Vector3r& rkV)  { return Vector3r(std::max(a.X(),rkV.X()),std::max(a.Y(),rkV.Y()),std::max(a.Z(),rkV.Z()));}
Vector3r componentMinVector (const Vector3r& a, const Vector3r& rkV)  { return Vector3r(std::min(a.X(),rkV.X()),std::min(a.Y(),rkV.Y()),std::min(a.Z(),rkV.Z())); }
Vector3r diagMult (const Vector3r& a, const Vector3r& rkV)  {	return Vector3r(a.X()*rkV.X(),a.Y()*rkV.Y(),a.Z()*rkV.Z()); }
Vector3r diagDiv (const Vector3r& a, const Vector3r& rkV)  { return Vector3r(a.X()/rkV.X(),a.Y()/rkV.Y(),a.Z()/rkV.Z()); }
Real unitVectorsAngle(const Vector3r& a, const Vector3r& rkV)  { return Mathr::ACos(a.Dot(rkV)); }



/*Vector4r std::maxVector (const Vector4r& rkV) const;
Vector4r std::minVector (const Vector4r& rkV) const;
Vector4r multDiag (const Vector4r& rkV) const;*/
Vector4r componentMaxVector (const Vector4r& a, const Vector4r& rkV) {return Vector4r(std::max(a.X(),rkV.X()),std::max(a.Y(),rkV.Y()),std::max(a.Z(),rkV.Z()),std::max(a.W(),rkV.W()));}
Vector4r componentMinVector (const Vector4r& a, const Vector4r& rkV) {return Vector4r(std::min(a.X(),rkV.X()),std::min(a.Y(),rkV.Y()),std::min(a.Z(),rkV.Z()),std::min(a.W(),rkV.W()));}
Vector4r diagMult (const Vector4r& a, const Vector4r& rkV) {return Vector4r(a.X()*rkV.X(),a.Y()*rkV.Y(),a.Z()*rkV.Z(),a.W()*rkV.W());}
//template<class RealType1, class RealType2>


/*void toAxes (Vector3r& axis1, Vector3r& axis2, Vector3r& axis3) const; 
void fromAxes (const Vector3r& axis1,const Vector3r& axis2,const Vector3r& axis3); 
void toGLMatrix(RealType m[16]) const;
void toEulerAngles (Vector3r& eulerAngles,RealType threshold = 1e-06f) const;
Vector3r operator* (const Vector3r& v) const;*/


Quaternionr quaternionFromAxes (const Vector3r& axis1,const Vector3r& axis2,const Vector3r& axis3){
	Matrix3r m;
	m[0][0] = axis1.X(); m[1][0] = axis1.Y(); m[2][0] = axis1.Z();
	m[0][1] = axis2.X(); m[1][1] = axis2.Y(); m[2][1] = axis2.Z();
	m[0][2] = axis3.X(); m[1][2] = axis3.Y(); m[2][2] = axis3.Z();
	Quaternionr ret;
	ret.FromRotationMatrix(m);
	return ret;
}

void quaternionToAxes(const Quaternionr& q, Vector3r& axis1, Vector3r& axis2, Vector3r& axis3){
	Matrix3r m;
	q.ToRotationMatrix(m);
   axis1.X() = m[0][0]; axis1.Y() = m[1][0]; axis1.Z() = m[2][0];
	axis2.X() = m[0][1]; axis2.Y() = m[1][1]; axis2.Z() = m[2][1];
	axis3.X() = m[0][2]; axis3.Y() = m[1][2]; axis3.Z() = m[2][2];
}

//template <class RealType>
void quaternionToEulerAngles (const Quaternionr& q, Vector3r& eulerAngles,Real threshold){
	Real heading,attitude,bank;
	Real test = q.X()*q.Y() + q.Z()*q.W();
	if (test > 0.5-threshold /*0.499*/) // singularity at north pole
	{ 
		heading = 2 * Mathr::ATan2(q.X(),q.W());
		attitude = Mathr::HALF_PI;
		bank = 0;
	}
	if (test < threshold-0.5 /*-0.499*/) // singularity at south pole
	{
		heading = -2 * Mathr::ATan2(q.X(),q.W());
		attitude = - Mathr::HALF_PI;
		bank = 0;
	}
	else
	{
		Real sqx = q.X()*q.X();
		Real sqy = q.Y()*q.Y();
		Real sqz = q.Z()*q.Z();
		heading = Mathr::ATan2(2*q.Y()*q.W()-2*q.X()*q.Z() , 1 - 2*sqy - 2*sqz);
		attitude = Mathr::ASin(2*test);
		bank = Mathr::ATan2(2*q.X()*q.W()-2*q.Y()*q.Z() , 1 - 2*sqx - 2*sqz);
	}
	eulerAngles[0] = bank;
 	eulerAngles[1] = heading;
 	eulerAngles[2] = attitude;
//  	Matrix3 m;
//   	this->normalize();
//  	this->toRotationMatrix(m);
//   	m.ToEulerAnglesXYZ(eulerAngles[0],eulerAngles[1],eulerAngles[2]);
}

//template <class RealType>
void quaterniontoGLMatrix(const Quaternionr& q, Real m[16]) {
	// FIXME: why float? and not RealType?
    float x2  = 2.0f*q.X();
    float y2  = 2.0f*q.Y();
    float z2  = 2.0f*q.Z();
    float x2w = x2*q.W();
    float y2w = y2*q.W();
    float z2w = z2*q.W();
    float x2x = x2*q.X();
    float y2x = y2*q.X();
    float z2x = z2*q.X();
    float y2y = y2*q.Y();
    float z2y = z2*q.Y();
    float z2z = z2*q.Z();

    m[0]  = 1.0f-(y2y+z2z);
    m[4]  = y2x-z2w;
    m[8]  = z2x+y2w;
    m[1]  = y2x+z2w;
    m[5]  = 1.0f-(x2x+z2z);
    m[9]  = z2y-x2w;
    m[2]  = z2x-y2w;
    m[6]  = z2y+x2w;
    m[10] = 1.0f-(x2x+y2y);

	m[12] = 0.0l;
	m[13] = 0.0l;
	m[14] = 0.0l;

	m[3] = 0.0l;
	m[7] = 0.0l;
	m[11] = 0.0l;
	m[15] = 1.0l;
}
