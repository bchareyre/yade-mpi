/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once


#include<yade/core/InteractionGeometry.hpp>


class SpringGeometry : public InteractionGeometry
{
	public :
		Vector3r p1;
		Vector3r p2;
		SpringGeometry();
		virtual ~SpringGeometry();
	
	REGISTER_ATTRIBUTES(InteractionGeometry,(p1)(p2));
	REGISTER_CLASS_NAME(SpringGeometry);
	REGISTER_BASE_CLASS_NAME(InteractionGeometry);

	REGISTER_CLASS_INDEX(SpringGeometry,InteractionGeometry);
};

REGISTER_SERIALIZABLE(SpringGeometry);


