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

#ifndef __VECTOR3_H__
#define __VECTOR3_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <ostream>
#include <istream>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Constants.hpp"
#include "Serializable.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class Vector3 : public Serializable
{
// coordinates
//public : float x, y, z;

	union
	{
		struct { float x,y,z; };
		float v[3];
 	};

//private: float* refs[3];

    // construction
	public : Vector3 ();
	public : Vector3 (float vx, float vy, float vz);
	public : Vector3 (float coord[3]);
	public : Vector3 (const Vector3& v);





	public : float& operator[] (int i);
	public : const float& operator[] (int i) const;
	public : operator float* ();

	// assignment
	public : Vector3& operator= (const Vector3& v);

    // comparison (supports fuzzy arithmetic when FUZZ > 0)
	public : bool operator== (const Vector3& v) const;
	public : bool operator!= (const Vector3& v) const;
	public : bool operator<  (const Vector3& v) const;
	public : bool operator<= (const Vector3& v) const;
	public : bool operator>  (const Vector3& v) const;
	public : bool operator>= (const Vector3& v) const;

    // arithmetic operations
	public : Vector3 operator+ (const Vector3& v) const;
	public : Vector3 operator- (const Vector3& v) const;
	public : Vector3 operator* (float f) const;
	public : Vector3 operator/ (float f) const;
	public : Vector3 operator- () const;
	public : friend Vector3 operator* (float f, const Vector3& v);

    // arithmetic updates
	public : Vector3& operator+= (const Vector3& v);
	public : Vector3& operator-= (const Vector3& v);
	public : Vector3& operator*= (float f);
	public : Vector3& operator/= (float f);

    // vector operations
	public : float length () const;
	public : float squaredLength () const;
	public : float dot (const Vector3& v) const;
	public : float unitize (float threshold = 1e-06f);
	public : Vector3& normalize (float threshold = 1e-06f);
	public : Vector3 normalized (float threshold = 1e-06f);
	public : Vector3 inverse (float threshold = 1e-06f);

	public : Vector3 multTerm (const Vector3& v) const;
	public : Vector3 maxTerm (const Vector3& v) const;
	public : Vector3 minTerm (const Vector3& v) const;
	public : Vector3 cross (const Vector3& v) const;
	public : Vector3 unitCross (const Vector3& v) const;

    // special points
	public : static const Vector3 ZERO;
	public : static const Vector3 UNIT_X;
	public : static const Vector3 UNIT_Y;
	public : static const Vector3 UNIT_Z;

	// fuzzy arithmetic (set FUZZ > 0 to enable)
	public : static float FUZZ;

	REGISTER_CLASS_NAME(Vector3);

	public : void registerAttributes();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(Vector3,true);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

inline std::ostream& operator<< (std::ostream& o, const Vector3& vec)
{
	o << vec[0] << " " << vec[1] << " " << vec[2];
	return o;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

inline std::istream& operator>> (std::istream& i, Vector3& vec)
{
	i >> vec[0] >> vec[1] >> vec[2];
	return i;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Vector3.ipp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __VECTOR3_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

