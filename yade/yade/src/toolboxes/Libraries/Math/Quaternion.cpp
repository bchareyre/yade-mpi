
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Quaternion.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

static float epsilon = 1e-03f;
Quaternion Quaternion::ZERO(0.0f,0.0f,0.0f,0.0f);
Quaternion Quaternion::IDENTITY(1.0f,0.0f,0.0f,0.0f);
float Quaternion::FUZZ = 0.0f;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Quaternion::fromRotationMatrix (const Matrix3& m)
{
    // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    // article "Quaternion Calculus and Fast Animation".

    float trace = m[0][0]+m[1][1]+m[2][2];
    float root;

    if ( trace > 0.0f )
    {
        // |w| > 1/2, may as well choose w > 1/2
        root = sqrt(trace + 1.0f);  // 2w
        w = 0.5f*root;
        root = 0.5f/root;  // 1/(4w)
        x = (m[2][1]-m[1][2])*root;
        y = (m[0][2]-m[2][0])*root;
        z = (m[1][0]-m[0][1])*root;
    }
    else
    {
        // |w| <= 1/2
        static int next[3] = { 1, 2, 0 };
        int i = 0;
        if ( m[1][1] > m[0][0] )
            i = 1;
        if ( m[2][2] > m[i][i] )
            i = 2;
        int j = next[i];
        int k = next[j];

        root = sqrt(m[i][i]-m[j][j]-m[k][k] + 1.0f);
        float* q[3] = { &x, &y, &z };
        *q[i] = 0.5f*root;
        root = 0.5f/root;
        w = (m[k][j]-m[j][k])*root;
        *q[j] = (m[j][i]+m[i][j])*root;
        *q[k] = (m[k][i]+m[i][k])*root;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Quaternion::toRotationMatrix (Matrix3& m) const
{
    float x2  = 2.0f*x;
    float y2  = 2.0f*y;
    float z2  = 2.0f*z;
    float x2w = x2*w;
    float y2w = y2*w;
    float z2w = z2*w;
    float x2x = x2*x;
    float y2x = y2*x;
    float z2x = z2*x;
    float y2y = y2*y;
    float z2y = z2*y;
    float z2z = z2*z;

    m[0][0] = 1.0f-(y2y+z2z);
    m[0][1] = y2x-z2w;
    m[0][2] = z2x+y2w;
    m[1][0] = y2x+z2w;
    m[1][1] = 1.0f-(x2x+z2z);
    m[1][2] = z2y-x2w;
    m[2][0] = z2x-y2w;
    m[2][1] = z2y+x2w;
    m[2][2] = 1.0f-(x2x+y2y);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Quaternion::fromAngleAxis (const float& angle, const Vector3& axis)
{
    // assert:  axis[] is unit length
    //
	// The quaternion representing the rotation is
	//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

    float halfAngle = 0.5f*angle;
    float sinus = sin(halfAngle);
    w = cos(halfAngle);

    x = sinus*axis.x;
    y = sinus*axis.y;
    z = sinus*axis.z;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Quaternion::toAngleAxis (float& angle, Vector3& axis) const
{
    // The quaternion representing the rotation is
    //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

   // cerr.precision(40);

    float sqrLength = x*x+y*y+z*z;
    if ( sqrLength > 0.0f )
    {
//         if (w>1)
// 		cerr << "w>1 " << w << endl;
// 	else if (w<-1)
// 		cerr << "w<-1" << endl;
// 	else if (w==-1)
// 		cerr << "w==-1" << endl;
// 	else if (w==1)
// 		cerr << "w==1" << endl;

	angle = 2.0f*acos(w);
        float invLength = 1.0f/sqrt(sqrLength);
        axis.x = x*invLength;
        axis.y = y*invLength;
        axis.z = z*invLength;
    }
    else
    {
        // angle is 0 (mod 2*pi), so any axis will do
        angle = 0.0f;
        axis.x = 1.0f;
        axis.y = 0.0f;
        axis.z = 0.0f;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Quaternion::fromAxes (const Vector3& axis1,const Vector3& axis2,const Vector3& axis3)
{
	Matrix3 m;

        m[0][0] = axis1.x;
        m[1][0] = axis1.y;
        m[2][0] = axis1.z;
	
        m[0][1] = axis2.x;
        m[1][1] = axis2.y;
        m[2][1] = axis2.z;

        m[0][2] = axis3.x;
        m[1][2] = axis3.y;
        m[2][2] = axis3.z;

	fromRotationMatrix(m);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Quaternion::toAxes ( Vector3& axis1, Vector3& axis2 ,Vector3& axis3) const
{
	Matrix3 m;

	toRotationMatrix(m);

        axis1.x = m[0][0];
        axis1.y = m[1][0];
        axis1.z = m[2][0];

	axis2.x = m[0][1];
        axis2.y = m[1][1];
        axis2.z = m[2][1];


	axis3.x = m[0][2];
        axis3.y = m[1][2];
        axis3.z = m[2][2];
  
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Quaternion::toEulerAngles (Vector3& eulerAngles,float threshold)
{
	float heading,attitude,bank;
	float test = x*y + z*w;
	if (test > 0.5-threshold /*0.499*/) // singularity at north pole
	{ 
		heading = 2 * atan2(x,w);
		attitude = Constants::HALF_PI;
		bank = 0;
	}
	if (test < threshold-0.5 /*-0.499*/) // singularity at south pole
	{
		heading = -2 * atan2(x,w);
		attitude = - Constants::HALF_PI;
		bank = 0;
	}
	else
	{
		float sqx = x*x;
		float sqy = y*y;
		float sqz = z*z;
		heading = atan2(2*y*w-2*x*z , 1 - 2*sqy - 2*sqz);
		attitude = asin(2*test);
		bank = atan2(2*x*w-2*y*z , 1 - 2*sqx - 2*sqz);
	}
	eulerAngles[0] = bank;
 	eulerAngles[1] = heading;
 	eulerAngles[2] = attitude;
	

//  	Matrix3 m;
//   	this->normalize();
//  	this->toRotationMatrix(m);
//   	m.ToEulerAnglesXYZ(eulerAngles[0],eulerAngles[1],eulerAngles[2]);

}

void Quaternion::toGLMatrix(float m[16])
{

    float x2  = 2.0f*x;
    float y2  = 2.0f*y;
    float z2  = 2.0f*z;
    float x2w = x2*w;
    float y2w = y2*w;
    float z2w = z2*w;
    float x2x = x2*x;
    float y2x = y2*x;
    float z2x = z2*x;
    float y2y = y2*y;
    float z2y = z2*y;
    float z2z = z2*z;

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

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

Quaternion Quaternion::exp () const
{
    // If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
    // exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
    // use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

    float angle = sqrt(x*x+y*y+z*z);
    float sinus = sin(angle);

    Quaternion res;
    res.w = cos(angle);

    if ( fabs(sinus) >= epsilon )
    {
        float coeff = sinus/angle;
        res.x = coeff*x;
        res.y = coeff*y;
        res.z = coeff*z;
    }
    else
    {
        res.x = x;
        res.y = y;
        res.z = z;
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

Quaternion Quaternion::log () const
{
    // If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
    // log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
    // sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

    Quaternion res;
    res.w = 0.0f;

    if ( fabs(w) < 1.0f )
    {
        float angle = acos(w);
        float sinus = sin(angle);
        if ( fabs(sinus) >= epsilon )
        {
            float coeff = angle/sinus;
            res.x = coeff*x;
            res.y = coeff*y;
            res.z = coeff*z;
            return res;
        }
    }

    res.x = x;
    res.y = y;
    res.z = z;

    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

Vector3 Quaternion::operator* (const Vector3& v) const
{
    // Given a vector u = (x0,y0,z0) and a unit length quaternion
    // q = <w,x,y,z>, the vector v = (x1,y1,z1) which represents the
    // rotation of u by q is v = q*u*q^{-1} where * indicates quaternion
    // multiplication and where u is treated as the quaternion <0,x0,y0,z0>.
    // Note that q^{-1} = <w,-x,-y,-z>, so no float work is required to
    // invert q.  Now
    //
    //   q*u*q^{-1} = q*<0,x0,y0,z0>*q^{-1}
    //     = q*(x0*i+y0*j+z0*k)*q^{-1}
    //     = x0*(q*i*q^{-1})+y0*(q*j*q^{-1})+z0*(q*k*q^{-1})
    //
    // As 3-vectors, q*i*q^{-1}, q*j*q^{-1}, and 2*k*q^{-1} are the columns
    // of the rotation matrix computed in Quaternion::ToRotationMatrix.
    // The vector v is obtained as the product of that rotation matrix with
    // vector u.  As such, the quaternion representation of a rotation
    // matrix requires less space than the matrix and more time to compute
    // the rotated vector.  Typical space-time tradeoff...

    Matrix3 m;
    toRotationMatrix(m);
    return m*v;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

Quaternion Quaternion::slerp (float t, const Quaternion& p, const Quaternion& q)
{
    float cosinus = p.dot(q);
    float angle = acos(cosinus);

    if ( fabs(angle) < epsilon )
        return p;

    float sinus = sin(angle);
    float invSinus = 1.0f/sinus;
    float coeff0 = sin((1.0f-t)*angle)*invSinus;
    float coeff1 = sin(t*angle)*invSinus;
    return coeff0*p + coeff1*q;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

Quaternion Quaternion::squad (float t, const Quaternion& p, const Quaternion& a, const Quaternion& b, const Quaternion& q)
{
    float slerpT = 2.0f*t*(1.0f-t);
    Quaternion slerpP = slerp(t,p,q);
    Quaternion slerpQ = slerp(t,a,b);
    return slerp(slerpT,slerpP,slerpQ);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////


Quaternion Quaternion::inverse () const
{
    float n = w*w+x*x+y*y+z*z;
    if ( n > 0.0f )
    {
        float invN = 1.0f/n;
        return Quaternion(w*invN,-x*invN,-y*invN,-z*invN);
    }
    else
    {
        // return an invalid result to flag the error
        return ZERO;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

Quaternion Quaternion::invert ()
{
    float n = w*w+x*x+y*y+z*z;

    if ( n > 0.0f )
    {
        float invN = 1.0f/n;
        w *= invN;
		x *=-invN;
		y *=-invN;
		z *=-invN;
		return *this;
    }
    else
    {
        // return an invalid result to flag the error
        return ZERO;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

Quaternion Quaternion::unitInverse () const
{
    // assert:  'this' is unit length
    return Quaternion(w,-x,-y,-z);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

Quaternion Quaternion::alignAxis(Vector3& v1, Vector3& v2, float threshold)
{
    Vector3 biSector = v1 + v2;
    biSector.normalize();

    float cosHalfAngle = v1.dot(biSector);
    Vector3 cross;

    if ( cosHalfAngle != 0.0 )
        cross = v1.cross(biSector);
    else
        cross = v1.unitCross(Vector3(v2.z,v2.x,v2.y));

    return Quaternion(cosHalfAngle,cross.x,cross.y,cross.z);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Quaternion::registerAttributes()
{
	REGISTER_ATTRIBUTE(w);
	REGISTER_ATTRIBUTE(x);
	REGISTER_ATTRIBUTE(y);
	REGISTER_ATTRIBUTE(z);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Quaternion::processAttributes()
{
	// FIXME : it is not working for saving
	//float angle = w;
	//Vector3 axis(x,y,z);
	//fromAngleAxis(angle,axis);
}
