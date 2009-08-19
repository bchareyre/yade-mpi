/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "LeapFrogOrientationIntegrator.hpp"
#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/core/Omega.hpp>

// FIXME : should we pass timestep as parameter of functor
// FIXME : what's with timestepper
void LeapFrogOrientationIntegrator::go(	  const shared_ptr<PhysicalParameters>& b
						, Body* body, BexContainer& bex)
{
	if(!body->isDynamic) return;
		
// 	unsigned int id = body->getId();
	
// 	if (prevAngularVelocities.size()<=id)
// 	{
// 		prevAngularVelocities.resize(id+1);
// 		firsts.resize(id+1,true);
// 	}

	RigidBodyParameters * rb = YADE_CAST<RigidBodyParameters*>(b.get());
	
	Real dt = Omega::instance().getTimeStep();
		
// 	if (!firsts[id])
// 		rb->angularVelocity = prevAngularVelocities[id]+rb->angularAcceleration*0.5*dt;
		
	rb->angularVelocity = rb->angularVelocity+ dt*rb->angularAcceleration;
	Vector3r axis = rb->angularVelocity;
	Real angle = axis.Normalize();
	Quaternionr q;
	q.FromAxisAngle(axis,angle*dt);
	if(rb->blockedDOFs==0) rb->se3.orientation = q*rb->se3.orientation;
	else{
		if((rb->blockedDOFs & PhysicalParameters::DOF_RX)==0) rb->angularVelocity[0]+=dt*rb->angularAcceleration[0];
		if((rb->blockedDOFs & PhysicalParameters::DOF_RY)==0) rb->angularVelocity[1]+=dt*rb->angularAcceleration[1];
		if((rb->blockedDOFs & PhysicalParameters::DOF_RZ)==0) rb->angularVelocity[2]+=dt*rb->angularAcceleration[2];
	}
	if(bex.getMoveRotUsed() && bex.getRot(body->getId())!=Vector3r::ZERO){ Vector3r r(bex.getRot(body->getId())); Real norm=r.Normalize(); q.FromAxisAngle(r,norm); rb->se3.orientation=q*rb->se3.orientation; }

	rb->se3.orientation.Normalize();

// 	firsts[id] = false;
}

YADE_PLUGIN((LeapFrogOrientationIntegrator));
