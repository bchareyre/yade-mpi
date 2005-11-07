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
#include <yade/yade-lib-wm3-math/Vector3.hpp>
#include <yade/yade-lib-wm3-math/Quaternion.hpp>

class SDECLinkGeometry : public InteractionGeometry
{
	public :
		virtual ~SDECLinkGeometry();

		Real		 radius1
				,radius2;

		Vector3r	normal;			// new unit normal of the contact plane.

	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(SDECLinkGeometry);
	REGISTER_BASE_CLASS_NAME(InteractionGeometry);
};

REGISTER_SERIALIZABLE(SDECLinkGeometry,false);

#endif // SDECCPERMANENTLINK_HPP

