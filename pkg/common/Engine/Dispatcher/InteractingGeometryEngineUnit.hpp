/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/Shape.hpp>
#include<yade/core/GeometricalModel.hpp>
#include<yade/core/Body.hpp>
#include<yade/core/Scene.hpp>
#include<yade/core/Functor.hpp>


#include <string>

#ifdef YADE_GEOMETRICALMODEL
/*! \brief
	Abstract interface for all classes that build Shape from GeometricalModel

	This is useful when it's not trivial to just write the parameters of Shape by hand.
	Then we can use this InteractingGeometryEngineUnit to build it from GeometricalModel.

	Currently it is only used to build a PolyhedralSweptSphere from BoxModel and from Tetrahedron
	  
	\param const shared_ptr<GeometricalModel>&	exact geometry of Body
	\param Se3r&					the 3D transformation to apply to the collision model before building the bounding volume
	\param Body*					the Body inside which operation takes place
	\return shared_ptr<Shape>&	the Shape built (given as second argument to the function)
	
*/
class InteractingGeometryEngineUnit : 	public Functor2D
					<
		 				void ,
		 				TYPELIST_4(	  const shared_ptr<GeometricalModel>&
								, shared_ptr<Shape>&
								, const Se3r& // FIXME - remove Se3r, If some function needs Se3r it must find it through Body*
								, const Body* // with that - functors have all the data they may need
			  				  )
					>
{
	public: virtual ~InteractingGeometryEngineUnit();	
	REGISTER_CLASS_AND_BASE(InteractingGeometryEngineUnit,Functor2D);
	REGISTER_ATTRIBUTES(Functor,/* no attributes here */);
};

REGISTER_SERIALIZABLE(InteractingGeometryEngineUnit);
#endif

