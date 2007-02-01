/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SDECLINKGEOMETRY_HPP
#define SDECLINKGEOMETRY_HPP

#include <vector>
#include <yade/yade-core/InteractionGeometry.hpp>
#include <Wm3Vector3.h>
#include <yade/yade-lib-base/yadeWm3.hpp>
#include <Wm3Quaternion.h>

class SDECLinkGeometry : public InteractionGeometry
{
	public :
		SDECLinkGeometry();
		virtual ~SDECLinkGeometry();

		Real		 radius1 // FIXME : lots of stuff is the same as in SpheresContactGeometry, so it should inherit from it.
				,radius2;

		Vector3r	normal;			// new unit normal of the contact plane.

	protected :
		void registerAttributes();

	REGISTER_CLASS_NAME(SDECLinkGeometry);
	REGISTER_BASE_CLASS_NAME(InteractionGeometry);

	REGISTER_CLASS_INDEX(SDECLinkGeometry,InteractionGeometry);
};

REGISTER_SERIALIZABLE(SDECLinkGeometry,false);

#endif // SDECCPERMANENTLINK_HPP

