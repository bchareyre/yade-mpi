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

#ifndef __QUATERNION_H__
#define __QUATERNION_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <ostream>
#include <istream>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Matrix3.hpp"
#include "Serializable.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class Quaternion : public Serializable
{

	public : float w, x, y, z;

	// construction and destruction
	public : Quaternion (float qw = 1.0f, float qx = 0.0f, float qy = 0.0f, float qz = 0.0f);
	public : Quaternion (const Quaternion& q);
	public : Quaternion& normalize();

	// conversion between quaternions, matrices, and angle-axes
	public : void fromRotationMatrix (const Matrix3& m);
	public : void toRotationMatrix (Matrix3& m) const;
	public : void fromAngleAxis (const float& angle, const Vector3& axis);
	public : void toAngleAxis (float& angle, Vector3& axis) const;
	public : void fromAxes (const Vector3* axis);
	public : void toAxes (Vector3* axis) const;
	public : void toGLMatrix(float m[16]);

	// arithmetic operations
	public : bool operator== (const Quaternion& q) const;
	public : Quaternion& operator= (const Quaternion& q);
	public : Quaternion operator+ (const Quaternion& q) const;
	public : Quaternion operator- (const Quaternion& q) const;
	public : Quaternion operator* (const Quaternion& q) const;
	public : Quaternion operator* (float fScalar) const;
	public : friend Quaternion operator* (float f, const Quaternion& q);
	public : Quaternion operator- () const;

	// functions of a quaternion
	public : float dot (const Quaternion& q) const;  // dot product
	public : float norm () const;  // squared-length
	public : Quaternion inverse () const;  // apply to non-zero quaternion
	public : Quaternion invert ();  // apply to non-zero quaternion
	public : Quaternion unitInverse () const;  // apply to unit-length quaternion
	public : Quaternion exp () const;
	public : Quaternion log () const;

	// rotation of a vector by a quaternion
	public : Vector3 operator* (const Vector3& v) const;

	// spherical linear interpolation
	public : static Quaternion slerp (float t, const Quaternion& p,  const Quaternion& q);

	// spherical quadratic interpolation
	public : static Quaternion squad (float t, const Quaternion& p, const Quaternion& a, const Quaternion& b, const Quaternion& q);

	// special values
	public : static Quaternion ZERO;
	public : static Quaternion IDENTITY;
	public : static float FUZZ;
	public : void registerAttributes();
	public : void processAttributes();

	REGISTER_CLASS_NAME(Quaternion);

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(Quaternion,true);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

inline std::ostream& operator<< (std::ostream& o, const Quaternion& q)
{
	o << q.w << " " << q.x << " " << q.y << " " << q.z;
	return o;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

inline std::istream& operator>> (std::istream& i, Quaternion& q)
{
	i >> q.w >> q.x >> q.y >> q.z;
	return i;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Quaternion.ipp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __QUATERNION_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
