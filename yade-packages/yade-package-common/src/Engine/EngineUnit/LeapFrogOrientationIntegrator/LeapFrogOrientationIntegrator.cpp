/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "LeapFrogOrientationIntegrator.hpp"
#include "RigidBodyParameters.hpp"
#include <yade/yade-core/Omega.hpp>

// FIXME : should we pass timestep as parameter of functor
// FIXME : what's with timestepper
void LeapFrogOrientationIntegrator::go(	  const shared_ptr<PhysicalParameters>& b
						, Body* body)
{
	if(! body->isDynamic)
		return;
		
	unsigned int id = body->getId();
	
	if (prevAngularVelocities.size()<=id)
	{
		prevAngularVelocities.resize(id+1);
		firsts.resize(id+1,true);
	}

	RigidBodyParameters * rb = dynamic_cast<RigidBodyParameters*>(b.get());
	
	Real dt = Omega::instance().getTimeStep();
		
	if (!firsts[id])
		rb->angularVelocity = prevAngularVelocities[id]+rb->angularAcceleration*0.5*dt;
		
	prevAngularVelocities[id] = rb->angularVelocity+((Real)(0.5*dt))*rb->angularAcceleration;
	Vector3r axis = rb->angularVelocity;
	Real angle = axis.Normalize();
	Quaternionr q;
	q.FromAxisAngle(axis,angle*dt);
	rb->se3.orientation = q*rb->se3.orientation;
	rb->se3.orientation.Normalize();

	firsts[id] = false;
}

