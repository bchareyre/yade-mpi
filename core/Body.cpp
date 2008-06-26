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

//! This could be -1 if id_t is re-typedef'ed as `int'
const body_id_t Body::ID_NONE=body_id_t(-1);

const shared_ptr<Body>& Body::byId(body_id_t _id, MetaBody* rb){return (*((rb?rb:Omega::instance().getRootBody().get())->bodies))[_id];}


// we must initialize id = 0, otherwise BodyContainer will crash.
Body::Body () : 
	  Serializable()
	, id(0)
	, groupMask(1)
	,clumpId(ID_NONE)
{
}

Body::Body (body_id_t newId, int newGroup) :
	  Serializable()
	, id(newId)
	, groupMask(newGroup)
	,clumpId(ID_NONE)
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
	REGISTER_ATTRIBUTE(clumpId);
}

