/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "LatticeLaw.hpp"
#include "LatticeBeamParameters.hpp"
#include "LatticeBeamAngularSpring.hpp"
#include "LatticeNodeParameters.hpp"
#include "LatticeSetParameters.hpp"
#include <yade/yade-core/BodyContainer.hpp>
#include <yade/yade-core/MetaBody.hpp>

LatticeLaw::LatticeLaw() : InteractionSolver()
{
}


LatticeLaw::~LatticeLaw()
{

}

bool LatticeLaw::deleteBeam(MetaBody* metaBody , LatticeBeamParameters* beam)
{
	beam->calcStrain();
	Real strain = beam->strain;
	return 	   strain < -beam->criticalCompressiveStrain
		|| strain >  beam->criticalTensileStrain;
}


void LatticeLaw::calcBeamPositionOrientationNewLength(Body* body, BodyContainer* bodies)
{
// FIXME - verify that this updating of length, position, orientation and color is in correct place/plugin
	LatticeBeamParameters* beam 	= static_cast<LatticeBeamParameters*>(body->physicalParameters.get());

	Body* bodyA 			= (*(bodies))[beam->id1].get();
	Body* bodyB 			= (*(bodies))[beam->id2].get();
	Se3r& se3A 			= bodyA->physicalParameters->se3;
	Se3r& se3B 			= bodyB->physicalParameters->se3;
	
	Se3r se3Beam;
	se3Beam.position 		= (se3A.position + se3B.position)*0.5;
	Vector3r dist 			= se3A.position - se3B.position;
	
	Real length 			= dist.normalize();
	Vector3r previousDirection 	= beam->direction;
	beam->direction 		= dist;
	beam->length 			= length;
	
	Vector3r previousPosition	= beam->se3.position;
	se3Beam.orientation.align( Vector3r::UNIT_X , dist );
	beam->se3 			= se3Beam;
	
	beam->se3Displacement.orientation.align(previousDirection,dist);
	beam->se3Displacement.position = se3Beam.position - previousPosition;
}

void LatticeLaw::action(Body* body)
{
	futureDeletes.clear();

	MetaBody * lattice = static_cast<MetaBody*>(body);
	
	int nodeGroupMask  = static_cast<LatticeSetParameters*>(lattice->physicalParameters.get())->nodeGroupMask;
	int beamGroupMask  = static_cast<LatticeSetParameters*>(lattice->physicalParameters.get())->beamGroupMask;
	
	BodyContainer* bodies = lattice->bodies.get();

	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	
	InteractionContainer::iterator angles     = lattice->persistentInteractions->begin();
	InteractionContainer::iterator angles_end = lattice->persistentInteractions->end();
	
	{ // 'B' calculate needed beam rotations
		for(  ; angles != angles_end; ++angles )
		{
			if( 	   bodies->exists( (*angles)->getId1() ) 	// FIXME - remove this test ....
				&& bodies->exists( (*angles)->getId2() ) ) 	//
			{
				LatticeBeamParameters* beam1 = static_cast<LatticeBeamParameters*>(((*(bodies))[(*angles)->getId1()])->physicalParameters.get());
				LatticeBeamParameters* beam2 = static_cast<LatticeBeamParameters*>(((*(bodies))[(*angles)->getId2()])->physicalParameters.get());
				
				LatticeBeamAngularSpring* an = static_cast<LatticeBeamAngularSpring*>((*angles)->interactionPhysics.get());
				
				Vector3r angleDifference = an->angle - an->initialAngle;
			//	Quaternionr angleDifference = an->angle - an->initialAngle;
				
				++(beam1->count);
				++(beam2->count);
				beam1->rotation += angleDifference;
				beam2->rotation -= angleDifference;
			//	beam1->rotation = beam1->rotation*angleDifference;
			//	beam2->rotation = angleDifference*beam2->rotation;
				
			}
			// else FIXME - delete unused angularSpring
		}
	}
	
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
		
		{ // 'E_min' 'E_max' criterion
			if( deleteBeam(lattice , beam) ) // calculates strain
			{
				futureDeletes.push_back(body->getId());
				continue;
			}
		}
		
		LatticeNodeParameters* node1 = static_cast<LatticeNodeParameters*>(((*(bodies))[beam->id1])->physicalParameters.get());
		LatticeNodeParameters* node2 = static_cast<LatticeNodeParameters*>(((*(bodies))[beam->id2])->physicalParameters.get());
		
		{ // 'W' and 'R'
			++(node1->countIncremental);
			++(node2->countIncremental);
			{ // 'W' from picture - previous displacement of the beam. try to do it again.
				node1->displacementIncremental += beam->se3Displacement.position;
				node2->displacementIncremental += beam->se3Displacement.position;
			}
	
			{ // 'R' from picture - previous rotation of the beam. try to do it again.
				Vector3r halfLength = beam->length * beam->direction * 0.5;
				node1->displacementIncremental += beam->se3Displacement.orientation * ( halfLength) - halfLength;
				node2->displacementIncremental += beam->se3Displacement.orientation * (-halfLength) + halfLength;
			}
		}

		{ // give 'D' to nodes
			node1->countStiffness += beam->longitudalStiffness;
			node2->countStiffness += beam->longitudalStiffness;
			node1->displacementStiffness -= displacement * beam->longitudalStiffness;
			node2->displacementStiffness += displacement * beam->longitudalStiffness;
		}

		{ // 'B' from picture - rotate to align with neighbouring beams
			node1->countStiffness += beam->bendingStiffness;
			node2->countStiffness += beam->bendingStiffness;

			Vector3r axis 		= beam->rotation / beam->count;
			Real angle 		= axis.normalize();
			Quaternionr rotation;
			rotation.fromAxisAngle(axis,angle);
		
		//	Quaternionr rotation 	= beam->rotation;// / beam->count;
			
			Vector3r length = beam->length * beam->direction;// * 0.5; // FIXME - duplicate line
			node1->displacementStiffness += (rotation * ( length) - length) * beam->bendingStiffness;
			node2->displacementStiffness += (rotation * (-length) + length) * beam->bendingStiffness;
			
			beam->rotation 		= Vector3r(0.0,0.0,0.0);
		//	beam->rotation 		= Quaternionr(0.0,0.0,0.0,0.0);
			beam->count 		= 0.0;
		}
	}
	
	{ // move nodes
		bi    = bodies->begin();
		biEnd = bodies->end();
		for(  ; bi!=biEnd ; ++bi )  // loop over all nodes -- move them according to displacements from beams
		{
			Body* body = (*bi).get();
	
			if( ! ( body->getGroupMask() & nodeGroupMask ) )
				continue; // skip non-nodes
			
			LatticeNodeParameters* node = static_cast<LatticeNodeParameters*>(body->physicalParameters.get() );
			
			{ // check nodes if they have any beams left
				if(node->countIncremental == 0) 
				{ // node not moving (marked for deletion)
					futureDeletes.push_back(body->getId());
					continue; 
				}
			}
			Vector3r displacement 		= node->displacementIncremental / node->countIncremental + node->displacementStiffness / node->countStiffness;
			node->countIncremental 		= 0;
			node->countStiffness 		= 0;
			node->displacementIncremental 	= Vector3r(0.0,0.0,0.0);
			node->displacementStiffness 	= Vector3r(0.0,0.0,0.0);
			
			if(body->isDynamic)
				node->se3.position 	+= displacement;
				
			/* FIXME FIXME FIXME FIXME FIXME FIXME FIXME
			else // FIXME - else move only in x direction
				node->se3.position[0] 	+= displacement[0];
			*/
		}
	}
	
	{ // store calc new beam position: X_b = ( X_n1 + X_n2 ) / 2 ,
	  // store position/orientation displacement, for 'W' and 'R' in se3Displacement
		bi    = bodies->begin();
		biEnd = bodies->end(); 
		for(  ; bi!=biEnd ; ++bi )  // loop over all beams
		{
			Body* body = (*bi).get();
			if( body->getGroupMask() & beamGroupMask )
				calcBeamPositionOrientationNewLength(body,bodies);
		}
	} 
	
	{ // delete all beams and nodes marked for deletion 
	  // - beams that exceeded critical tensile/compressive strain
	  // - nodes that have no more beams
		vector<unsigned int>::iterator vend = futureDeletes.end();
		for( vector<unsigned int>::iterator vsta = futureDeletes.begin() ; vsta != vend ; ++vsta)
			bodies->erase(*vsta); 
	}
	
	{ // calculate new angles between beams
		angles     = lattice->persistentInteractions->begin();
		angles_end = lattice->persistentInteractions->end();
		for(  ; angles != angles_end; ++angles )
		{
			if( 	   bodies->exists( (*angles)->getId1() )
				&& bodies->exists( (*angles)->getId2() ) )
			{
				LatticeBeamParameters* beam1 = static_cast<LatticeBeamParameters*>(((*(bodies))[(*angles)->getId1()])->physicalParameters.get());
				LatticeBeamParameters* beam2 = static_cast<LatticeBeamParameters*>(((*(bodies))[(*angles)->getId2()])->physicalParameters.get());
			
				Quaternionr 		rotation;
				Vector3r		axis;
				Real			angle;
			
				rotation.align( beam1->direction , beam2->direction );
				rotation.toAxisAngle (axis, angle);
				(static_cast<LatticeBeamAngularSpring*>((*angles)->interactionPhysics.get()))->angle = axis*angle; 
			//	(static_cast<LatticeBeamAngularSpring*>((*angles)->interactionPhysics.get()))->angle = rotation; 
			}
			// else FIXME - delete unused angularSpring
		}
	}
}

