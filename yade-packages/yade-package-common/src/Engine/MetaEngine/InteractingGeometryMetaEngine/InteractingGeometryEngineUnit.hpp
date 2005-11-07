/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INTERACTING_GEOMETRY_ENGINE_UNIT_HPP
#define INTERACTING_GEOMETRY_ENGINE_UNIT_HPP

#include <yade/yade-core/InteractingGeometry.hpp>
#include <yade/yade-core/GeometricalModel.hpp>
#include <yade/yade-core/Body.hpp>
#include <yade/yade-core/MetaBody.hpp>
#include <yade/yade-core/EngineUnit2D.hpp>

#include <boost/shared_ptr.hpp>
#include <string>

class InteractingGeometryEngineUnit : 	public EngineUnit2D
					<
		 				void ,
		 				TYPELIST_4(	  const shared_ptr<GeometricalModel>&
								, shared_ptr<InteractingGeometry>& // is not working when const, because functors are supposed to modify it!
								, const Se3r& // FIXME - remove Se3r, If some function needs Se3r it must find it through Body*
								, const Body* // with that - functors have all the data they may need, but it's const, so they can't modify it !
			  				  )
					>
{	
	REGISTER_CLASS_NAME(InteractingGeometryEngineUnit);
	REGISTER_BASE_CLASS_NAME(EngineUnit2D);
};

REGISTER_SERIALIZABLE(InteractingGeometryEngineUnit,false);

#endif //  INTERACTING_GEOMETRY_ENGINE_UNIT_HPP

