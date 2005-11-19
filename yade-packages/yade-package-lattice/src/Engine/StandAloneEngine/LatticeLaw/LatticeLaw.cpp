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

/*
	LatticeBeamParameters 
	
		unsigned int 	 id1
				,id2;
				
		Real  		 initialLength
				,length;
				
		Vector3r 	 initialDirection
				,direction;
				
		Real 		 criticalTensileStrain
				,criticalCompressiveStrain
				
				,longitudalStiffness
				,bendingStiffness;
		
		Vector3r 	 previousSe3;
*/

LatticeLaw::LatticeLaw() : InteractionSolver() , actionForce(new Force)
{
	nodeGroupMask = 1;
	beamGroupMask = 2;
}


LatticeLaw::~LatticeLaw()
{

}


void LatticeLaw::registerAttributes()
{
	InteractionSolver::registerAttributes();
	REGISTER_ATTRIBUTE(nodeGroupMask);
	REGISTER_ATTRIBUTE(beamGroupMask);
}

bool LatticeLaw::deleteBeam(MetaBody* metaBody , LatticeBeamParameters* beam)
{
	beam->calcStrain();
	Real strain = beam->strain;
	return 	   strain < -beam->criticalCompressiveStrain
		|| strain >  beam->criticalTensileStrain;
}


void LatticeLaw::calcBeamsPositionOrientationNewLength(Body* body, BodyContainer* bodies)
{
// FIXME - verify that this updating of length, position, orientation and color is in correct place/plugin
	LatticeBeamParameters* beam 	= static_cast<LatticeBeamParameters*>(body->physicalParameters.get());

//	cerr << "beam: " << body->getId() << "id1: " << beam->id1 << " id2: " << beam->id2 << "\n";
	Body* bodyA 			= (*(bodies))[beam->id1].get();
	Body* bodyB 			= (*(bodies))[beam->id2].get();
	Se3r& se3A 			= bodyA->physicalParameters->se3;
	Se3r& se3B 			= bodyB->physicalParameters->se3;
	
	Se3r se3Beam;
	se3Beam.position 		= (se3A.position + se3B.position)*0.5;
	Vector3r dist 			= se3A.position - se3B.position;
	
	Real length 			= dist.normalize();
	beam->direction 		= dist;
	beam->length 			= length;
	
	beam->previousSe3 		= beam->se3;
	se3Beam.orientation.align( Vector3r::UNIT_X , dist );
	beam->se3 			= se3Beam;
}

void LatticeLaw::action(Body* body)
{
	futureDeletes.clear();

	MetaBody * lattice = static_cast<MetaBody*>(body);
	BodyContainer* bodies = lattice->bodies.get();

	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	
	for(  ; bi!=biEnd ; ++bi )  // loop over all beams
	{
		Body* body = (*bi).get();
		if( ! ( body->getGroupMask() & beamGroupMask ) )
			continue; // skip non-beams
		
		// next beam
		LatticeBeamParameters* beam = static_cast<LatticeBeamParameters*>(body->physicalParameters.get() );
		
		Real      stretch      = beam->length - beam->initialLength;
		
		// 'D' from picture. how much beam wants to change length at each node, to bounce back through original length to mirror position.
		Vector3r  displacement = beam->direction * stretch;
		
		{ // check E_min, E_max criterion
			if( deleteBeam(lattice , beam) ) // calculates strain
			{
				futureDeletes.push_back(body->getId());
				continue;
			}
		}
		
		LatticeNodeParameters* node1 = static_cast<LatticeNodeParameters*>(((*(bodies))[beam->id1])->physicalParameters.get());
		LatticeNodeParameters* node2 = static_cast<LatticeNodeParameters*>(((*(bodies))[beam->id2])->physicalParameters.get());
		
		{ // give 'D' to nodes
			++(node1->count);
			++(node2->count);
			node1->displacement -= displacement;
			node2->displacement += displacement;
		}
		
		{ // 'W' from picture - previous displacement of the beam. try to do it again.
			Vector3r previousDisplacement = beam->se3.position - beam->previousSe3.position;
			node1->displacement += previousDisplacement;
			node2->displacement += previousDisplacement;
		} 
	}
	
	bi    = bodies->begin();
	biEnd = bodies->end();
	for(  ; bi!=biEnd ; ++bi )  // loop over all nodes
	{
		Body* body = (*bi).get();

		if( ! ( body->getGroupMask() & nodeGroupMask ) )
			continue; // skip non-nodes
		
		LatticeNodeParameters* node = static_cast<LatticeNodeParameters*>(body->physicalParameters.get() );
		
		{ // check nodes if they have any beams left
			if(node->count == 0) 
			{ // node not moving (marked for deletion)
			//	futureDeletes.push_back(body->getId()); // FIXME - crashes ....
				continue; 
			}
		}
		
		Vector3r displacement 	= node->displacement / node->count;
		node->displacement  	= Vector3r(0.0,0.0,0.0);
		node->count 		= 0.0;
		
		if(body->isDynamic)
			node->se3.position 	+= displacement;
			
		/* FIXME FIXME FIXME FIXME FIXME FIXME FIXME
		else // FIXME - else move only in x direction
			node->se3.position[0] 	+= displacement[0];
		*/
	}
	
	{ // store previousSe3 in the beam, calc new beam position: X_b = ( X_n1 + X_n2 ) / 2
		bi    = bodies->begin();
		biEnd = bodies->end(); 
		for(  ; bi!=biEnd ; ++bi )  // loop over all beams
		{
			Body* body = (*bi).get();
			if( body->getGroupMask() & beamGroupMask )
				calcBeamsPositionOrientationNewLength(body,bodies);
		}
	} 
	
	{ // delete all beams and nodes marked for deletion 
	  // - beams that exceeded critical tensile/compressive strain
	  // - nodes that have no more beams
		vector<unsigned int>::iterator vend = futureDeletes.end();
		for( vector<unsigned int>::iterator vsta = futureDeletes.begin() ; vsta != vend ; ++vsta)
			bodies->erase(*vsta); 
	}
}

