/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __VECTOR2_H__
#define __VECTOR2_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Serializable.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class Vector2 : public Serializable
{

	// construction
	public : Vector2 ();
	public : Vector2 (float fX, float fY);
	public : Vector2 (float afCoordinate[2]);
	public : Vector2 (const Vector2& rkVector);

	// coordinates
	public : float x, y;

	// access vector V as V[0] = V.x, V[1] = V.y
	//
	// WARNING.  These member functions rely on
	// (1) Vector2 not having virtual functions
	// (2) the data packed in a 3*sizeof(float) memory block
	public : float& operator[] (int i) const;
	public : operator float* ();

	// assignment
	public : Vector2& operator= (const Vector2& rkVector);

	// comparison (supports fuzzy arithmetic when FUZZ > 0)
	public : bool operator== (const Vector2& rkVector) const;
	public : bool operator!= (const Vector2& rkVector) const;
	public : bool operator<  (const Vector2& rkVector) const;
	public : bool operator<= (const Vector2& rkVector) const;
	public : bool operator>  (const Vector2& rkVector) const;
	public : bool operator>= (const Vector2& rkVector) const;

	// arithmetic operations
	public : Vector2 operator+ (const Vector2& rkVector) const;
	public : Vector2 operator- (const Vector2& rkVector) const;
	public : Vector2 operator* (float fScalar) const;
	public : Vector2 operator/ (float fScalar) const;
	public : Vector2 operator- () const;
	public : friend Vector2 operator* (float fScalar, const Vector2& rkVector);

	// arithmetic updates
	public : Vector2& operator+= (const Vector2& rkVector);
	public : Vector2& operator-= (const Vector2& rkVector);
	public : Vector2& operator*= (float fScalar);
	public : Vector2& operator/= (float fScalar);

	// vector operations
	public : float length () const;
	public : float squaredLength () const;
	public : float dot (const Vector2& rkVector) const;
	public : float unitize (float fTolerance = 1e-06f);

	// special points
	public : static const Vector2 ZERO;
	public : static const Vector2 UNIT_X;
	public : static const Vector2 UNIT_Y;

	// fuzzy arithmetic (set FUZZ > 0 to enable)
	public : static float FUZZ;

	public : void registerAttributes()
	{
		REGISTER_ATTRIBUTE(x);
		REGISTER_ATTRIBUTE(y);
	}
	
	REGISTER_CLASS_NAME(Vector2);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Vector2.ipp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_CLASS(Vector2,true);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __VECTOR2_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

