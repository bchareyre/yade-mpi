/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "ParticleSet2Mesh2D.hpp"
#include "ParticleSetParameters.hpp"
#include <yade/yade-package-common/Mesh2D.hpp>
#include <yade/yade-core/MetaBody.hpp>

void ParticleSet2Mesh2D::go(	  const shared_ptr<PhysicalParameters>&
				, shared_ptr<GeometricalModel>& gm
				, const Body* body)
{
	Mesh2D* mesh2d = static_cast<Mesh2D*>(gm.get());
	
	const MetaBody * ncb = dynamic_cast<const MetaBody*>(body);
	const shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
// FIXME - this copying of Se3 between Mesh2D, and Particles inside MetaBody could be done just once, if Se3 was inside shared_ptr. This can be improved once we make indexable Parameters: Velocity, Position, Orientation, ....

	int particleSetMask = 1; // FIXME - make this a class variable
 	unsigned int i=0;

	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for(  ; bi!=biEnd ; ++bi,++i )
	{
		shared_ptr<Body> b = *bi;
		if( b->getGroupMask() & particleSetMask )// FIXME - this line is a bad hack, to make things work, we should have TWO ComplexBodies, one with cloth, second with spheres. here I assume that spheres and cloth are in the same MetaBody, and that cloth belongs to group 1
			mesh2d->vertices[i]=b->physicalParameters->se3.position;
	}
}
	
