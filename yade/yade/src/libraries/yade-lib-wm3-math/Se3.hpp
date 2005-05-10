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

#ifndef __SE3_H__
#define __SE3_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade-lib-wm3-math/Quaternion.hpp>
#include <yade-lib-wm3-math/Vector3.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template <class RealType>
class Se3
{
	public : Vector3<RealType> position;
	public : Quaternion<RealType> orientation;

	// Construction
	public : Se3();
	public : Se3(Vector3<RealType> rkP, Quaternion<RealType> qR);
	public : Se3(const Se3<RealType>& s);
	public : Se3(Se3<RealType>& a,Se3<RealType>& b);

	public : Se3<RealType> inverse();

	public : void toGLMatrix(float m[16]);
	public : Vector3<RealType> operator * (const Vector3<RealType>& b );
	public : Se3<RealType> operator * (const Quaternion<RealType>& b );
	public : Se3<RealType> operator * (const Se3<RealType>& b );
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Se3.ipp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef Se3<float> Se3f;
typedef Se3<double> Se3d;
typedef Se3<Real> Se3r;


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __SE3_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
