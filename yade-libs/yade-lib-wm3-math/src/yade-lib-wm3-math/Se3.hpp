/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SE3_HPP
#define SE3_HPP


#include "Quaternion.hpp"
#include "Vector3.hpp"


template <class RealType>
class Se3
{
	public :
		Vector3<RealType>	position;
		Quaternion<RealType>	orientation;

		Se3();
		Se3(Vector3<RealType> rkP, Quaternion<RealType> qR);
		Se3(const Se3<RealType>& s);
		Se3(Se3<RealType>& a,Se3<RealType>& b);

		Se3<RealType> inverse();

		void toGLMatrix(float m[16]);
		Vector3<RealType> operator * (const Vector3<RealType>& b );
		Se3<RealType> operator * (const Quaternion<RealType>& b );
		Se3<RealType> operator * (const Se3<RealType>& b );
};


#include "Se3.ipp"


typedef Se3<float> Se3f;
typedef Se3<double> Se3d;
typedef Se3<Real> Se3r;

#endif // SE3_HPP

