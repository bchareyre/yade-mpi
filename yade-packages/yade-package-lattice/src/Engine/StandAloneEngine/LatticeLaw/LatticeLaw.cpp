/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "LatticeLaw.hpp"
#include "LatticeBeamParameters.hpp"
#include "LatticeNodeParameters.hpp"
#include <yade/yade-package-common/Force.hpp>
#include <yade/yade-core/BodyContainer.hpp>
#include <yade/yade-core/MetaBody.hpp>


LatticeLaw::LatticeLaw() : Engine() , actionForce(new Force)
{
	nodeGroupMask = 1;
	beamGroupMask = 2;
	
	maxDispl     = 0.0004;
}


LatticeLaw::~LatticeLaw()
{

}


void LatticeLaw::registerAttributes()
{
	Engine::registerAttributes();
	REGISTER_ATTRIBUTE(nodeGroupMask);
	REGISTER_ATTRIBUTE(beamGroupMask);
	REGISTER_ATTRIBUTE(maxDispl);
}


void LatticeLaw::action(Body* body)
{

	MetaBody * lattice = static_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = lattice->bodies;
	shared_ptr<PhysicalActionContainer>& actionParameters = lattice->actionParameters;
	
//	Real dt = Omega::instance().getTimeStep();
	
	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for(  ; bi!=biEnd ; ++bi )  // loop over all beams
	{
		shared_ptr<Body> body = *bi;
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

	bi    = bodies->begin();
	biEnd    = bodies->end();
	for(  ; bi!=biEnd ; ++bi )  // loop over all nodes
	{
		shared_ptr<Body> body = *bi;

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


