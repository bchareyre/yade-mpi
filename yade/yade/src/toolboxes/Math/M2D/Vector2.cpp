
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Vector2.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

//using namespace Tachyon;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

const Vector2 Vector2::ZERO(0.0f,0.0f);
const Vector2 Vector2::UNIT_X(1.0f,0.0f);
const Vector2 Vector2::UNIT_Y(0.0f,1.0f);
float Vector2::FUZZ = 0.0f;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool Vector2::operator< (const Vector2& rkVector) const
{
    float fXTmp = rkVector.x, fYTmp = rkVector.y;
    if ( FUZZ > 0.0f )
    {
        if ( fabs(x - fXTmp) <= FUZZ )
            fXTmp = x;
        if ( fabs(y - fYTmp) <= FUZZ )
            fYTmp = y;
    }

    // compare y values
    unsigned int uiTest0 = *(unsigned int*)&y;
    unsigned int uiTest1 = *(unsigned int*)&fYTmp;
    if ( uiTest0 < uiTest1 )
        return true;
    if ( uiTest0 > uiTest1 )
        return false;

    // compare x values
    uiTest0 = *(unsigned int*)&x;
    uiTest1 = *(unsigned int*)&fXTmp;
    return uiTest0 < uiTest1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool Vector2::operator<= (const Vector2& rkVector) const
{
    float fXTmp = rkVector.x, fYTmp = rkVector.y;
    if ( FUZZ > 0.0f )
    {
        if ( fabs(x - fXTmp) <= FUZZ )
            fXTmp = x;
        if ( fabs(y - fYTmp) <= FUZZ )
            fYTmp = y;
    }

    // compare y values
    unsigned int uiTest0 = *(unsigned int*)&y;
    unsigned int uiTest1 = *(unsigned int*)&fYTmp;
    if ( uiTest0 < uiTest1 )
        return true;
    if ( uiTest0 > uiTest1 )
        return false;

    // compare x values
    uiTest0 = *(unsigned int*)&x;
    uiTest1 = *(unsigned int*)&fXTmp;
    return uiTest0 <= uiTest1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool Vector2::operator> (const Vector2& rkVector) const
{
    float fXTmp = rkVector.x, fYTmp = rkVector.y;
    if ( FUZZ > 0.0f )
    {
        if ( fabs(x - fXTmp) <= FUZZ )
            fXTmp = x;
        if ( fabs(y - fYTmp) <= FUZZ )
            fYTmp = y;
    }

    // compare y values
    unsigned int uiTest0 = *(unsigned int*)&y;
    unsigned int uiTest1 = *(unsigned int*)&fYTmp;
    if ( uiTest0 > uiTest1 )
        return true;
    if ( uiTest0 < uiTest1 )
        return false;

    // compare x values
    uiTest0 = *(unsigned int*)&x;
    uiTest1 = *(unsigned int*)&fXTmp;
    return uiTest0 > uiTest1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool Vector2::operator>= (const Vector2& rkVector) const
{
    float fXTmp = rkVector.x, fYTmp = rkVector.y;
    if ( FUZZ > 0.0f )
    {
        if ( fabs(x - fXTmp) <= FUZZ )
            fXTmp = x;
        if ( fabs(y - fYTmp) <= FUZZ )
            fYTmp = y;
    }

    // compare y values
    unsigned int uiTest0 = *(unsigned int*)&y;
    unsigned int uiTest1 = *(unsigned int*)&fYTmp;
    if ( uiTest0 > uiTest1 )
        return true;
    if ( uiTest0 < uiTest1 )
        return false;

    // compare x values
    uiTest0 = *(unsigned int*)&x;
    uiTest1 = *(unsigned int*)&fXTmp;
    return uiTest0 >= uiTest1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

float Vector2::unitize (float fTolerance)
{
    float fLength = length();

    if ( fLength > fTolerance )
    {
        float fInvLength = 1.0f/fLength;
        x *= fInvLength;
        y *= fInvLength;
    }
    else
    {
        fLength = 0.0f;
    }

    return fLength;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
