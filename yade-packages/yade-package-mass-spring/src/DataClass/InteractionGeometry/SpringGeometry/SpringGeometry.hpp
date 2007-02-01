/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SPRINGGEOMETRY_HPP
#define SPRINGGEOMETRY_HPP


#include <yade/yade-core/InteractionGeometry.hpp>


class SpringGeometry : public InteractionGeometry
{
	public :
		Vector3r p1;
		Vector3r p2;
		SpringGeometry();
		virtual ~SpringGeometry();
	
	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(SpringGeometry);
	REGISTER_BASE_CLASS_NAME(InteractionGeometry);

	REGISTER_CLASS_INDEX(SpringGeometry,InteractionGeometry);
};

REGISTER_SERIALIZABLE(SpringGeometry,false);

#endif // SPRINGGEOMETRY_HPP

