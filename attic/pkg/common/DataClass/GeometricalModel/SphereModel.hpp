/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once


#ifdef YADE_GEOMETRICALMODEL
#include<yade/core/GeometricalModel.hpp>
class SphereModel : public GeometricalModel
{
	public :
		Real radius;

		SphereModel ();
		virtual ~SphereModel ();

	REGISTER_ATTRIBUTES(GeometricalModel,(radius));
	REGISTER_CLASS_AND_BASE(SphereModel,GeometricalModel);
	REGISTER_CLASS_INDEX(SphereModel,GeometricalModel);
};

REGISTER_SERIALIZABLE(SphereModel);
#else
	#error This file may be included only with the shape feature enabled.
#endif
