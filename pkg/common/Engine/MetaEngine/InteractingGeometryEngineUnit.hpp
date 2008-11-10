/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INTERACTING_GEOMETRY_ENGINE_UNIT_HPP
#define INTERACTING_GEOMETRY_ENGINE_UNIT_HPP

#include<yade/core/InteractingGeometry.hpp>
#include<yade/core/GeometricalModel.hpp>
#include<yade/core/Body.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/core/EngineUnit2D.hpp>

#include <string>

/*! \brief
	Abstract interface for all classes that build InteractingGeometry from GeometricalModel

	This is useful when it's not trivial to just write the parameters of InteractingGeometry by hand.
	Then we can use this InteractingGeometryEngineUnit to build it from GeometricalModel.

	Currently it is only used to build a PolyhedralSweptSphere from Box and from Tetrahedron
	  
	\param const shared_ptr<GeometricalModel>&	exact geometry of Body
	\param Se3r&					the 3D transformation to apply to the collision model before building the bounding volume
	\param Body*					the Body inside which operation takes place
	\return shared_ptr<InteractingGeometry>&	the InteractingGeometry built (given as second argument to the function)
	
*/
class InteractingGeometryEngineUnit : 	public EngineUnit2D
					<
		 				void ,
		 				TYPELIST_4(	  const shared_ptr<GeometricalModel>&
								, shared_ptr<InteractingGeometry>&
								, const Se3r& // FIXME - remove Se3r, If some function needs Se3r it must find it through Body*
								, const Body* // with that - functors have all the data they may need
			  				  )
					>
{	
	REGISTER_CLASS_NAME(InteractingGeometryEngineUnit);
	REGISTER_BASE_CLASS_NAME(EngineUnit2D);
};

REGISTER_SERIALIZABLE(InteractingGeometryEngineUnit,false);

#endif //  INTERACTING_GEOMETRY_ENGINE_UNIT_HPP

