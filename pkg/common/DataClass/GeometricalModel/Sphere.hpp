/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SPHERE_HPP
#define SPHERE_HPP

#include<yade/core/GeometricalModel.hpp>

class Sphere : public GeometricalModel
{
	public :
		Real radius;

		Sphere ();
		virtual ~Sphere ();

	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(Sphere);
	REGISTER_BASE_CLASS_NAME(GeometricalModel);
	REGISTER_CLASS_INDEX(Sphere,GeometricalModel);
};

REGISTER_SERIALIZABLE(Sphere);

#endif // SPHERE_HPP

