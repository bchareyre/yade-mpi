
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Vector3.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

const Vector3 Vector3::ZERO(0.0f,0.0f,0.0f);
const Vector3 Vector3::UNIT_X(1.0f,0.0f,0.0f);
const Vector3 Vector3::UNIT_Y(0.0f,1.0f,0.0f);
const Vector3 Vector3::UNIT_Z(0.0f,0.0f,1.0f);
float Vector3::FUZZ = 0.0f;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool Vector3::operator< (const Vector3& v) const
{
    float tmpX = v.x;
	float tmpY = v.y;
	float tmpZ = v.z;
	
    if ( FUZZ > 0.0f )
    {
        if ( fabs(x - tmpX) <= FUZZ )
            tmpX = x;
        if ( fabs(y - tmpY) <= FUZZ )
            tmpY = y;
        if ( fabs(z - tmpZ) <= FUZZ )
            tmpZ = z;
    }

    // compare z values
    unsigned int test0 = *(unsigned int*)&z;
    unsigned int test1 = *(unsigned int*)&tmpZ;
    if ( test0 < test1 )
        return true;
    if ( test0 > test1 )
        return false;

    // compare y values
    test0 = *(unsigned int*)&y;
    test1 = *(unsigned int*)&tmpY;
    if ( test0 < test1 )
        return true;
    if ( test0 > test1 )
        return false;

    // compare x values
    test0 = *(unsigned int*)&x;
    test1 = *(unsigned int*)&tmpX;
    return test0 < test1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool Vector3::operator<= (const Vector3& v) const
{
    float tmpX = v.x;
	float tmpY = v.y;
	float tmpZ = v.z;
	
    if ( FUZZ > 0.0f )
    {
        if ( fabs(x - tmpX) <= FUZZ )
            tmpX = x;
        if ( fabs(y - tmpY) <= FUZZ )
            tmpY = y;
        if ( fabs(z - tmpZ) <= FUZZ )
            tmpZ = z;
    }

    // compare z values
    unsigned int test0 = *(unsigned int*)&z;
    unsigned int test1 = *(unsigned int*)&tmpZ;
    if ( test0 < test1 )
        return true;
    if ( test0 > test1 )
        return false;

    // compare y values
    test0 = *(unsigned int*)&y;
    test1 = *(unsigned int*)&tmpY;
    if ( test0 < test1 )
        return true;
    if ( test0 > test1 )
        return false;

    // compare x values
    test0 = *(unsigned int*)&x;
    test1 = *(unsigned int*)&tmpX;
    return test0 <= test1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool Vector3::operator> (const Vector3& v) const
{
    float tmpX = v.x;
	float tmpY = v.y;
	float tmpZ = v.z;
	
    if ( FUZZ > 0.0f )
    {
        if ( fabs(x - tmpX) <= FUZZ )
            tmpX = x;
        if ( fabs(y - tmpY) <= FUZZ )
            tmpY = y;
        if ( fabs(z - tmpZ) <= FUZZ )
            tmpZ = z;
    }

    // compare z values
    unsigned int test0 = *(unsigned int*)&z;
    unsigned int test1 = *(unsigned int*)&tmpZ;
    if ( test0 > test1 )
        return true;
    if ( test0 < test1 )
        return false;

    // compare y values
    test0 = *(unsigned int*)&y;
    test1 = *(unsigned int*)&tmpY;
    if ( test0 > test1 )
        return true;
    if ( test0 < test1 )
        return false;

    // compare x values
    test0 = *(unsigned int*)&x;
    test1 = *(unsigned int*)&tmpX;
    return test0 > test1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool Vector3::operator>= (const Vector3& v) const
{
    float tmpX = v.x;
	float tmpY = v.y;
	float tmpZ = v.z;
	
    if ( FUZZ > 0.0f )
    {
        if ( fabs(x - tmpX) <= FUZZ )
            tmpX = x;
        if (fabs(y - tmpY) <= FUZZ )
            tmpY = y;
        if ( fabs(z - tmpZ) <= FUZZ )
            tmpZ = z;
    }

    // compare z values
    unsigned int test0 = *(unsigned int*)&z;
    unsigned int test1 = *(unsigned int*)&tmpZ;
    if ( test0 > test1 )
        return true;
    if ( test0 < test1 )
        return false;

    // compare y values
    test0 = *(unsigned int*)&y;
    test1 = *(unsigned int*)&tmpY;
    if ( test0 > test1 )
        return true;
    if ( test0 < test1 )
        return false;

    // compare x values
    test0 = *(unsigned int*)&x;
    test1 = *(unsigned int*)&tmpX;
    return test0 >= test1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

float Vector3::unitize (float threshold)
{
    float l = length();

    if ( l > threshold )
    {
        float invL = 1.0f/l;
        x *= invL;
        y *= invL;
        z *= invL;
    }
    else
        l = 0.0f;

    return l;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

Vector3& Vector3::normalize (float threshold)
{
    float l = length();

    if ( l > threshold )
    {
        float invL = 1.0f/l;
        x *= invL;
        y *= invL;
        z *= invL;
    }
    else
        l = 0.0f;

    return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

Vector3 Vector3::normalized(float threshold)
{
	
	float l = length();
	float invL;

	if ( l > threshold )
		invL = 1.0f/l;
	else
		invL = 0.0f;
	
	return Vector3(x*invL,y*invL,z*invL); // vcpp : warning C4172: returning address of local variable or temporary

}


void Vector3::registerAttributes()
{
	REGISTER_ATTRIBUTE(x);
	REGISTER_ATTRIBUTE(y);
	REGISTER_ATTRIBUTE(z);
}
