/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "LeapFrogIntegrator.hpp"
#include "RigidBodyParameters.hpp"
#include "ParticleParameters.hpp"
#include "Omega.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


// FIXME : should we pass timestep as parameter of functor
// FIXME : what's with timestepper
void LeapFrogIntegrator::go(const shared_ptr<BodyPhysicalParameters>& b,unsigned int id)
{

	if (prevVelocities.size()<=id)
	{
		prevVelocities.resize(id+1);
		firsts.resize(id+1,true);
	}

	ParticleParameters * p = dynamic_cast<ParticleParameters*>(b.get());

	Real dt = Omega::instance().getTimeStep();

	if (!firsts[id])
		p->velocity = prevVelocities[id]+0.5*dt*p->acceleration;

	prevVelocities[id] = p->velocity+0.5*dt*p->acceleration;
	p->se3.translation += prevVelocities[id]*dt;

	
	// FIXME : maybe split into 2 LeapFrogIntegrator
	RigidBodyParameters * rb = dynamic_cast<RigidBodyParameters*>(b.get());
	if(rb)
	{		
		if (prevAngularVelocities.size()<=id)
			prevAngularVelocities.resize(id+1);
			
		if (!firsts[id])
			rb->angularVelocity = prevAngularVelocities[id]+0.5*dt*rb->angularAcceleration;
	
		prevAngularVelocities[id] = rb->angularVelocity+0.5*dt*rb->angularAcceleration;
		Vector3r axis = rb->angularVelocity;
		Real angle = axis.normalize();
		Quaternionr q;
		q.fromAxisAngle(axis,angle*dt);
		rb->se3.rotation = q*rb->se3.rotation;
		rb->se3.rotation.normalize();
	}
	firsts[id] = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
