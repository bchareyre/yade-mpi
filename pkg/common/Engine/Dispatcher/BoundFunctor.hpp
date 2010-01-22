/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/Bound.hpp>
#include<yade/core/Shape.hpp>
#include<yade/core/Body.hpp>
#include<yade/core/Scene.hpp>
#include<yade/core/Functor.hpp>

#include <string>

/*! \brief
	Abstract interface for all classes that build Bound from Shape

	This is useful when it's not trivial to build the bounding volume from (some complicated) interaction model.
	 
	For example if you want to build an Aabb from a box which is not initially aligned with the world
	axis, it is not easy to write by hand into the configuration file the center and size of this Aabb.
	Instead you can use a BoundFunctor that will compute for you the correct value.
		
	\param const shared_ptr<Shape>&	the collision model from wich we want to extract the bounding volume
	\param Se3r&					the 3D transformation to apply to the collision model before building the bounding volume
	\param Body*					the Body inside which operation takes place
	\return shared_ptr<Bound>&		shared pointer to the bounding volume built (given as second argument to the function)
	
*/
class BoundFunctor : public Functor2D
				<
		 			void ,
		 			TYPELIST_4(	  const shared_ptr<Shape>&
							, shared_ptr<Bound>&
							, const Se3r& 	// FIXME - remove Se3r, because not everything is supposed to have it. 
									// If some function needs Se3r it must find it through Body*
							, const Body*	// with that - functors have all the data they may need, but it's const
			  			  )
				>
{	
	public: virtual ~BoundFunctor();
	YADE_CLASS_BASE_DOC_ATTRS(BoundFunctor,Functor,"Functor for creating/updating Body::bound.",/*no attrs*/);
};

REGISTER_SERIALIZABLE(BoundFunctor);


