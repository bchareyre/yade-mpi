/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "NewtonsMomentumLaw.hpp"
#include "RigidBodyParameters.hpp"
#include "Momentum.hpp"
#include<yade/lib-base/yadeWm3Extra.hpp>


void NewtonsMomentumLaw::go( 	  const shared_ptr<PhysicalAction>& a
				, const shared_ptr<PhysicalParameters>& b
				, const Body* bb)
{
	Momentum * am = static_cast<Momentum*>(a.get());
	RigidBodyParameters * rb = static_cast<RigidBodyParameters*>(b.get());
	
	//FIXME : should be += and we should add an Engine that reset acceleration at the beginning
	if(bb->isStandalone()) rb->angularAcceleration=diagDiv(am->momentum,rb->inertia);
	else if(bb->isClump()) rb->angularAcceleration+=diagDiv(am->momentum,rb->inertia);
	else { // isClumpMember()
		const shared_ptr<Body>& clump(Body::byId(bb->clumpId));
		RigidBodyParameters* clumpRBP=YADE_CAST<RigidBodyParameters*>(clump->physicalParameters.get());
		/* angularAcceleration is reset by ClumpMemberMover engine */
		clumpRBP->angularAcceleration+=diagDiv(am->momentum,clumpRBP->inertia);
	}
}


