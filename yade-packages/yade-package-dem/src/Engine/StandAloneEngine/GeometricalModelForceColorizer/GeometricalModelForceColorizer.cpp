/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GeometricalModelForceColorizer.hpp"
#include <yade/yade-package-common/RigidBodyParameters.hpp>
#include <yade/yade-core/Omega.hpp>
#include <yade/yade-core/MetaBody.hpp>
#include <yade/yade-package-common/Force.hpp>
#include <boost/lexical_cast.hpp>


GeometricalModelForceColorizer::GeometricalModelForceColorizer () : StandAloneEngine(), actionForce(new Force)
{
}


bool GeometricalModelForceColorizer::isActivated()
{
	return true;
//	return ((Omega::instance().getCurrentIteration() % interval == 0) && (ofile));
}


void GeometricalModelForceColorizer::action(Body * body)
{
	// FIXME the same in GLDrawLatticeBeamState.cpp

	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	BodyContainer* bodies = ncb->bodies.get();
	
	Real min=10000,max=-10000;

	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for( ; bi!=biEnd ; ++bi ) // first loop to calculate min/max
	{
		Body* body = (*bi).get();
		if(body->isDynamic)
		{
			unsigned int i = body -> getId();
			Vector3r force = dynamic_cast<Force*>(ncb->physicalActions->find( i , actionForce->getClassIndex() ) . get() )->force;
			min = std::min( force[0] , std::min( force[1] , std::min( force[2], min ) ) );
			max = std::max( force[0] , std::max( force[1] , std::max( force[2], max ) ) );
		}

	}
	
	bi    = bodies->begin();
	biEnd = bodies->end();
	for( ; bi!=biEnd ; ++bi ) // second loop to assign correct color
	{
		Body* body = (*bi).get();
		if(body->isDynamic)
		{
			GeometricalModel* gm = body->geometricalModel.get();
			unsigned int i = body -> getId();
			Vector3r force = dynamic_cast<Force*>(ncb->physicalActions->find( i , actionForce->getClassIndex() ) . get() )->force;
			gm->diffuseColor[0] = (force[0]-min)/(max-min);
			gm->diffuseColor[1] = (force[1]-min)/(max-min);
			gm->diffuseColor[2] = (force[2]-min)/(max-min);
		}

	}
		
}

