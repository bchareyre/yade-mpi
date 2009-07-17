/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/InteractingGeometry.hpp>

class InteractingSphere : public InteractingGeometry
{
	public :
		Real radius;

		InteractingSphere ();
		virtual ~InteractingSphere ();

	REGISTER_ATTRIBUTES(InteractingGeometry,(radius));
	REGISTER_CLASS_NAME(InteractingSphere);
	REGISTER_BASE_CLASS_NAME(InteractingGeometry);
	REGISTER_CLASS_INDEX(InteractingSphere,InteractingGeometry);
};

REGISTER_SERIALIZABLE(InteractingSphere);


