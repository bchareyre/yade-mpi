/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SPHERESCONTACTGEOMETRY_HPP
#define SPHERESCONTACTGEOMETRY_HPP

#include <vector>
#include <yade/yade-core/InteractionGeometry.hpp>
#include <Wm3Vector3.h>
#include <yade/yade-lib-base/yadeWm3.hpp>

class SpheresContactGeometry : public InteractionGeometry
{
	public :
		Vector3r	 normal			// new unit normal of the contact plane.
				,contactPoint;	

		Real		 radius1
				,radius2
				,penetrationDepth;

		virtual ~SpheresContactGeometry();

	protected :
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(SpheresContactGeometry);
	REGISTER_BASE_CLASS_NAME(InteractionGeometry);
};

REGISTER_SERIALIZABLE(SpheresContactGeometry,false);

#endif // SPHERESCONTACTGEOMETRY_HPP

