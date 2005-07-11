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

#include "MassSpringLaw.hpp"

#include "SpringGeometry.hpp"
#include "SpringPhysics.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/Omega.hpp>
#include <yade/yade-core/MetaBody.hpp>
#include <yade/yade-package-common/Mesh2D.hpp>
#include <yade/yade-package-common/ParticleParameters.hpp>
#include <yade/yade-package-common/Force.hpp>
#include <yade/yade-package-common/Momentum.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

MassSpringLaw::MassSpringLaw () : InteractionSolver(), actionForce(new Force) , actionMomentum(new Momentum)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void MassSpringLaw::registerAttributes()
{
	REGISTER_ATTRIBUTE(springGroupMask);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void MassSpringLaw::calculateForces(Body * body)
{
	MetaBody * massSpring = dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = massSpring->bodies;
	shared_ptr<InteractionContainer>& persistentInteractions = massSpring->persistentInteractions;
	shared_ptr<PhysicalActionContainer>& actionParameters = massSpring->actionParameters;
	

	InteractionContainer::iterator ii    = persistentInteractions->begin();
	InteractionContainer::iterator iiEnd = persistentInteractions->end();
	for(  ; ii!=iiEnd ; ++ii )
	{	
		shared_ptr<Interaction> spring = *ii;
		if (spring->isReal)
		{
			int id1 = spring->getId1();
			int id2 = spring->getId2();
			
			if( !(  (*bodies)[id1]->getGroupMask() & (*bodies)[id2]->getGroupMask() & springGroupMask) )
				continue; // skip other groups
			
			ParticleParameters * p1 = static_cast<ParticleParameters*>((*bodies)[id1]->physicalParameters.get());
			ParticleParameters * p2 = static_cast<ParticleParameters*>((*bodies)[id2]->physicalParameters.get());
			
			SpringPhysics* physics		= static_cast<SpringPhysics*>(spring->interactionPhysics.get());
			SpringGeometry* geometry	= static_cast<SpringGeometry*>(spring->interactionGeometry.get());
			
			Vector3r v1 = p2->se3.position;
			Vector3r v2 = p1->se3.position;
			
			Real l  = (v2-v1).length();
			
			Real l0 = physics->initialLength;
			
			Vector3r dir = (v2-v1);
			dir.normalize();
			
			Real e  = (l-l0)/l0;
			Real relativeVelocity = dir.dot((p1->velocity-p2->velocity));
			Vector3r f3 = (e*physics->stiffness + relativeVelocity* ( 1.0 - physics->damping )  )*dir;
			
			static_cast<Force*>   ( actionParameters->find( id1 , actionForce->getClassIndex() ).get() )->force    -= f3;
			static_cast<Force*>   ( actionParameters->find( id2 , actionForce->getClassIndex() ).get() )->force    += f3;
		}
	}
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
