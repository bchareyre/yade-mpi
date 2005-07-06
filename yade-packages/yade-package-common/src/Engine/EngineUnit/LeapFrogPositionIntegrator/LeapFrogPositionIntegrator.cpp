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

#include "LeapFrogPositionIntegrator.hpp" 

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "ParticleParameters.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/Omega.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


// FIXME : should we pass timestep as parameter of functor
// FIXME : what's with timestepper
void LeapFrogPositionIntegrator::go(       const shared_ptr<PhysicalParameters>& b
						, Body* body)
{
	if(! body->isDynamic)
		return;

	unsigned int id = body->getId();
	
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
	p->se3.position += prevVelocities[id]*dt;

	firsts[id] = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

