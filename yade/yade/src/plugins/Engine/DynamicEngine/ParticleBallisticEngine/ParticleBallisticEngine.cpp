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

#include "ParticleBallisticEngine.hpp"
#include "Particle.hpp"
#include "Omega.hpp"
#include "NonConnexBody.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

ParticleBallisticEngine::ParticleBallisticEngine() : DynamicEngine()
{
	first = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

ParticleBallisticEngine::~ParticleBallisticEngine()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void ParticleBallisticEngine::registerAttributes()
{
	DynamicEngine::registerAttributes();
	REGISTER_ATTRIBUTE(damping);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void ParticleBallisticEngine::respondToCollisions(Body * body)
{
	Particle * p = dynamic_cast<Particle*>(body);
	
	Real dt = Omega::instance().getTimeStep();

	if (!first)
		p->velocity = damping*(prevVelocity+0.5*dt*p->acceleration);

	prevVelocity = p->velocity+0.5*dt*p->acceleration;

	p->se3.translation += prevVelocity*dt;

	first = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
