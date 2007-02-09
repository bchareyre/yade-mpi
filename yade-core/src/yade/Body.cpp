/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Body.hpp"

#include <limits>
#include "MetaBody.hpp"
#include "Omega.hpp"

#ifdef HIGHLEVEL_CLUMPS
	const id_t Body::ID_NONE=UINT_MAX;
	/*! the definition will change once Omega disappears, but the interface should be the same. */
	shared_ptr<Body> Body::byId(Body::id_t _id){return (*(Omega::instance().getRootBody()->bodies))[_id];}
#endif

// we must initialize id = 0, otherwise BodyContainer will crash.
Body::Body () : 
	  Serializable()
	, id(0)
	, groupMask(1)
	#ifdef HIGHLEVEL_CLUMPS
	,clumpId(ID_NONE)
	#endif
{
}

Body::Body (unsigned int newId, int newGroup) :
	  Serializable()
	, id(newId)
	, groupMask(newGroup)
	#ifdef HIGHLEVEL_CLUMPS
	,clumpId(ID_NONE)
	#endif
	, physicalParameters(shared_ptr<PhysicalParameters>())
	, geometricalModel(shared_ptr<GeometricalModel>())
	, interactingGeometry(shared_ptr<InteractingGeometry>())
	, boundingVolume(shared_ptr<BoundingVolume>())
	
{
}


void Body::registerAttributes()
{
	REGISTER_ATTRIBUTE(id);
	REGISTER_ATTRIBUTE(groupMask);
	REGISTER_ATTRIBUTE(isDynamic); // FIXME? : only used for interaction : no collision between 2 non dynamic objects

	REGISTER_ATTRIBUTE(physicalParameters);
	REGISTER_ATTRIBUTE(geometricalModel);
	REGISTER_ATTRIBUTE(interactingGeometry);
	REGISTER_ATTRIBUTE(boundingVolume);
	#ifdef HIGHLEVEL_CLUMPS
	REGISTER_ATTRIBUTE(clumpId);
	#endif
}

