/***************************************************************************
 *   Copyright (C) 2004 by Janek Kozicki                                   *
 *   cosurgi@berlios.de                                                    *
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

#include "LeapFrogMomentumIntegrator.hpp"
#include "RigidBodyParameters.hpp"
#include "ParticleParameters.hpp"
#include "Omega.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


// FIXME : should we pass timestep as parameter of functor
// FIXME : what's with timestepper
void LeapFrogMomentumIntegrator::go( 	  const shared_ptr<Action>&
					, const shared_ptr<BodyPhysicalParameters>& b
					, const Body* body)
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
		rb->angularVelocity = prevAngularVelocities[id]+0.5*dt*rb->angularAcceleration;
		
	prevAngularVelocities[id] = rb->angularVelocity+0.5*dt*rb->angularAcceleration;
	Vector3r axis = rb->angularVelocity;
	Real angle = axis.normalize();
	Quaternionr q;
	q.fromAxisAngle(axis,angle*dt);
	rb->se3.orientation = q*rb->se3.orientation;
	rb->se3.orientation.normalize();

	firsts[id] = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

