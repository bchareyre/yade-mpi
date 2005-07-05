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

#include "LatticeLaw.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-common/LatticeBeamParameters.hpp>
#include <yade/yade-common/LatticeNodeParameters.hpp>
#include <yade/yade-common/Force.hpp>

#include <yade/yade-core/BodyContainer.hpp>
#include <yade/yade-core/MetaBody.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

LatticeLaw::LatticeLaw() : InteractionSolver() , actionForce(new Force)
{
	nodeGroupMask = 1;
	beamGroupMask = 2;
	
	maxDispl     = 0.0004;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

LatticeLaw::~LatticeLaw()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void LatticeLaw::registerAttributes()
{
	InteractionSolver::registerAttributes();
	REGISTER_ATTRIBUTE(nodeGroupMask);
	REGISTER_ATTRIBUTE(beamGroupMask);
	REGISTER_ATTRIBUTE(maxDispl);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void LatticeLaw::calculateForces(Body* body)
{

	MetaBody * lattice = static_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = lattice->bodies;
	shared_ptr<PhysicalActionContainer>& actionParameters = lattice->actionParameters;
	
//	Real dt = Omega::instance().getTimeStep();
	
	for(bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext()) // loop over all beams
	{
		const shared_ptr<Body>& body = bodies->getCurrent();
		if( ! ( body->getGroupMask() & beamGroupMask ) )
			continue; // skip non-beams
		
		LatticeBeamParameters* beam = static_cast<LatticeBeamParameters*>(body->physicalParameters.get() );
		
//		Vector3r  force = beam->direction * ((beam->length/beam->initialLength)-1.0)/stiffness;

		Vector3r  displacement = beam->direction * (beam->length - beam->initialLength) * 0.5;
		
		if( displacement.squaredLength() > maxDispl /*FIXME - different name*/ ) // delete beam
		{
			bodies->erase(body->getId());
			continue;
		}
		
//		cerr << beam->direction.length() << endl;
//		static_cast<Force*> ( actionParameters->find( beam->id1 , actionForce ->getClassIndex() ).get() )->force  -= force;
//		static_cast<Force*> ( actionParameters->find( beam->id2 , actionForce ->getClassIndex() ).get() )->force  += force;

//		shared_ptr<Body>& bodyA = (*(rootBody->bodies))[beam->id1];
//		shared_ptr<Body>& bodyB = (*(rootBody->bodies))[beam->id2];
		
		LatticeNodeParameters* node1 = static_cast<LatticeNodeParameters*>(((*(bodies))[beam->id1])->physicalParameters.get());
		LatticeNodeParameters* node2 = static_cast<LatticeNodeParameters*>(((*(bodies))[beam->id2])->physicalParameters.get());

//		cerr << node1 << endl;
//		cerr << node2 << endl; 
	
		++(node1->count);
		++(node2->count);
		
		node1->displacement -= displacement;
		node2->displacement += displacement;

	}

	for(bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext()) // loop over all nodes
	{
		const shared_ptr<Body>& body = bodies->getCurrent();
		if( ! ( body->getGroupMask() & nodeGroupMask ) )
			continue; // skip non-nodes
		
		LatticeNodeParameters* node = static_cast<LatticeNodeParameters*>(body->physicalParameters.get() );
		
		if(node->count == 0) 
			continue; 
		
		Vector3r displacement 	= node->displacement / node->count;
		node->displacement  	= Vector3r(0.0,0.0,0.0);
		node->count 		= 0.0;
		
		if(body->isDynamic)
			node->se3.position 	+= displacement;
	}

}


