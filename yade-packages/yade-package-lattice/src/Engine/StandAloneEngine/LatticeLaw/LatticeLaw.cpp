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
#include "NonLocalDependency.hpp"
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

bool LatticeLaw::deleteBeam(MetaBody* metaBody , LatticeBeamParameters* beam, Body* b)
{
	Real strain;
	if ( nonlocal) // calculate strain with non-local law
		strain = beam->nonLocalStrain / beam->nonLocalDivisor;
	else
		strain = beam->strain();
	return     strain < -beam->criticalCompressiveStrain
		|| strain >  beam->criticalTensileStrain;
}


void LatticeLaw::calcBeamPositionOrientationNewLength(Body* body, BodyContainer* bodies)
{
// FIXME - verify that this updating of length, position, orientation and color is in correct place/plugin
	LatticeBeamParameters* beam 	  = static_cast<LatticeBeamParameters*>(body->physicalParameters.get());

	Body* bodyA 			  = (*(bodies))[beam->id1].get();
	Body* bodyB 			  = (*(bodies))[beam->id2].get();
	Se3r& se3A 			  = bodyA->physicalParameters->se3;
	Se3r& se3B 			  = bodyB->physicalParameters->se3;
	
	Se3r se3Beam;
	se3Beam.position 		  = (se3A.position + se3B.position)*0.5;
	Vector3r dist 			  =  se3A.position - se3B.position;
	
	Real length 			  = dist.normalize();
	beam->direction 		  = dist;
	beam->length 			  = length;
	
	se3Beam.orientation.align( Vector3r::UNIT_X , dist );
	
	beam->se3Displacement.position    = se3Beam.position - beam->se3.position;
	beam->se3Displacement.orientation = se3Beam.orientation * beam->se3.orientation.inverse();
	
	beam->se3 			  = se3Beam;
}

void LatticeLaw::action(Body* body)
{
	futureDeletes.clear();

	MetaBody * lattice = static_cast<MetaBody*>(body);
	//std::list<std::list<LatticeSetParameters::NonLocalInteraction> >
	//std::vector<std::list<LatticeSetParameters::NonLocalInteraction , std::__malloc_alloc_template<sizeof(LatticeSetParameters::NonLocalInteraction)> > >& nonl = static_cast<LatticeSetParameters*>(lattice->physicalParameters.get())->nonl;
	void* nonl = static_cast<LatticeSetParameters*>(lattice->physicalParameters.get())->nonl;
	Real FIXME_range                   = static_cast<LatticeSetParameters*>(lattice->physicalParameters.get())->range;
	bool FIXME_useBendTensileSoftening = static_cast<LatticeSetParameters*>(lattice->physicalParameters.get())->useBendTensileSoftening;
	bool FIXME_useStiffnessSoftening   = static_cast<LatticeSetParameters*>(lattice->physicalParameters.get())->useStiffnessSoftening;
	//std::cerr << "zz:" << FIXME_useStiffnessSoftening << "\n";
	unsigned long int& total = static_cast<LatticeSetParameters*>(lattice->physicalParameters.get())->total;
	//nonlocal = ( nonl.size() != 0 );
	nonlocal = ( total != 0 );

	int nodeGroupMask  = static_cast<LatticeSetParameters*>(lattice->physicalParameters.get())->nodeGroupMask;
	int beamGroupMask  = static_cast<LatticeSetParameters*>(lattice->physicalParameters.get())->beamGroupMask;

	BodyContainer* bodies = lattice->bodies.get();

	InteractionContainer::iterator angles     = lattice->persistentInteractions->begin();
	InteractionContainer::iterator angles_end = lattice->persistentInteractions->end();

	{ // 'B' calculate needed beam rotations - FIXME : only 2D !
		for(  ; angles != angles_end; ++angles )
		{
			if(        bodies->exists( (*angles)->getId1() )        // FIXME - remove this test ....
				&& bodies->exists( (*angles)->getId2() ) ) 	//
			{
				LatticeBeamParameters* beam1 = static_cast<LatticeBeamParameters*>(((*(bodies))[(*angles)->getId1()])->physicalParameters.get());
				LatticeBeamParameters* beam2 = static_cast<LatticeBeamParameters*>(((*(bodies))[(*angles)->getId2()])->physicalParameters.get());
				
				LatticeBeamAngularSpring* an = static_cast<LatticeBeamAngularSpring*>((*angles)->interactionPhysics.get());
				
			//	Vector3r angleDifference = an->angle - an->initialAngle;
			//	Quaternionr angleDifference = an->angle * an->initialAngle.inverse();
				Real angleDifference = an->planeAngle - an->initialPlaneAngle;
				
				if( angleDifference > 0 )
					while(angleDifference > Mathr::PI) angleDifference -= Mathr::TWO_PI;
				else
					while(angleDifference < -Mathr::PI) angleDifference += Mathr::TWO_PI;
				
				++(beam1->count);
				++(beam2->count);
			
				if(an->lastCrossProduct.dot(Vector3r(0,0,1)) > 0.0 xor an->planeSwap180 )
				{
					beam1->bendingRotation -= angleDifference;
					beam2->bendingRotation += angleDifference;
				}
				else
				{
					beam1->bendingRotation += angleDifference;
					beam2->bendingRotation -= angleDifference;
				}
			
			}
			// else FIXME - delete unused angularSpring
		}
	}

	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();

	for(  ; bi!=biEnd ; ++bi )  // clear all non-local vaules in all beams.
	{
		Body* body = (*bi).get();
		if( ! ( body->getGroupMask() & beamGroupMask ) )
			continue; // skip non-beams

		LatticeBeamParameters* beam = static_cast<LatticeBeamParameters*>(body->physicalParameters.get() );

		beam->nonLocalStrain = 0.0;
		beam->nonLocalDivisor = 0.0;
	}

        //if (lattice->volatileInteractions->size() != 0) // calculate non-local strain of beams
        if ( nonlocal ) // calculate non-local strain of beams
        {
		//angles     = lattice->volatileInteractions->begin(); // FIXME - angles is a *very* confusing name
		//angles_end = lattice->volatileInteractions->end();
		//std::list<std::list<LatticeSetParameters::NonLocalInteraction> >
	//	std::vector<std::list<LatticeSetParameters::NonLocalInteraction , std::__malloc_alloc_template<sizeof(LatticeSetParameters::NonLocalInteraction)> > >::iterator nnb = nonl.begin();
		//std::list<std::list<LatticeSetParameters::NonLocalInteraction> >
	//	std::vector<std::list<LatticeSetParameters::NonLocalInteraction , std::__malloc_alloc_template<sizeof(LatticeSetParameters::NonLocalInteraction)> > >::iterator nne = nonl.end();

		//for(  ; angles != angles_end; ++angles )
	//	for( ; nnb != nne ; ++nnb )
	//	{
			unsigned long int counter = 0;
			//std::list<LatticeSetParameters::NonLocalInteraction>::iterator nb = nnb->begin();
			//std::list<LatticeSetParameters::NonLocalInteraction>::iterator ne = nnb->end();
			LatticeSetParameters::NonLocalInteraction* nb = reinterpret_cast<LatticeSetParameters::NonLocalInteraction*>(nonl);
			//for( ; nb != ne ; ++nb )
			//for( ; counter != total ; nb+=sizeof(LatticeSetParameters::NonLocalInteraction) , ++counter )
			for( ; counter != total ; ++nb , ++counter )
			{
				//if(      bodies->exists( (*angles)->getId1() )
				//      && bodies->exists( (*angles)->getId2() ) ) // both beams exist ..
				if(	   bodies->exists( nb->id1 )
					&& bodies->exists( nb->id2 ) ) // both beams exist ..
				{
					//LatticeBeamParameters* beam1 = static_cast<LatticeBeamParameters*>(((*(bodies))[(*angles)->getId1()])->physicalParameters.get());
					//LatticeBeamParameters* beam2 = static_cast<LatticeBeamParameters*>(((*(bodies))[(*angles)->getId2()])->physicalParameters.get());
					LatticeBeamParameters* beam1 = static_cast<LatticeBeamParameters*>(((*(bodies))[nb->id1])->physicalParameters.get());
					LatticeBeamParameters* beam2 = static_cast<LatticeBeamParameters*>(((*(bodies))[nb->id2])->physicalParameters.get());

					Real cosAngle = std::abs(beam1->direction.dot(beam2->direction)); // direction is a unit vector, abs() - because I want a number between 0 .. 1
				// recalculate gaussValue ... ... maybe should be updated every 10th (interval) iteration?
				//	Real& gaussValue = static_cast<NonLocalDependency*>((*angles)->interactionPhysics.get())->gaussValue;
					//Real gaussValue = static_cast<NonLocalDependency*>((*angles)->interactionPhysics.get())->gaussValue;
	/////				Real gaussValue = nb->gaussValue;

					Real sqDist = (beam1->se3.position - beam2->se3.position).squaredLength();
					Real len = FIXME_range;//0.003; // FIXME,FIXME FIXME !!!
					static Real sqPi = std::sqrt(Mathr::PI);
					Real gaussValue  = std::exp( - (sqDist/(len*len)) ) / ( len * sqPi );
				//	Real gaussValue  = std::exp( - std::pow( dist / len , 2) ) / ( len * sqPi );

					Real b2 = beam2->length * gaussValue * cosAngle;
					Real b1 = beam1->length * gaussValue * cosAngle;
					beam1->nonLocalStrain += beam2->strain() * b2;// * cosAngle;
					beam2->nonLocalStrain += beam1->strain() * b1;// * cosAngle;
					beam1->nonLocalDivisor += b2;
					beam2->nonLocalDivisor += b1;
				}
				// else FIXME - delete unused NonLocalDependency
			}
	//	} 
	}


	bi    = bodies->begin();
	biEnd = bodies->end();

	for(  ; bi!=biEnd ; ++bi )  // loop over all beams
	{
		Body* body = (*bi).get();
		if( ! ( body->getGroupMask() & beamGroupMask ) )
			continue; // skip non-beams

		// next beam
		LatticeBeamParameters* beam = static_cast<LatticeBeamParameters*>(body->physicalParameters.get() );

		Real      stretch      = beam->length - beam->initialLength;

		// 'D' from picture. how much beam wants to change length at each node, to bounce back through original length to mirror position.
		Vector3r  displacementLongitudal = beam->direction * stretch;// * 0.5;

		{ // 'E_min' 'E_max' criterion
			if( deleteBeam(lattice , beam, body) ) // calculates strain
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
				node1->displacementIncremental += beam->se3Displacement.position; // * 0.9;
				node2->displacementIncremental += beam->se3Displacement.position; // * 0.9;
			}

			{ // 'R' from picture - previous rotation of the beam. try to do it again.
				Vector3r halfLength = beam->length * beam->direction * 0.5; // * 0.9;
				node1->displacementIncremental += beam->se3Displacement.orientation * ( halfLength) - halfLength;
				node2->displacementIncremental += beam->se3Displacement.orientation * (-halfLength) + halfLength;
			}
		}

		Real FIXME_useStiffnessSoftening_Factor=1.0;
		if(FIXME_useStiffnessSoftening)
		{ // give 'D' to nodes
			Real sTrAiN = beam->strain();                                                  // only with over half of tension. (compression is ignored)
			FIXME_useStiffnessSoftening_Factor = sTrAiN > beam->criticalTensileStrain*0.5 
				? (2.0-sTrAiN*2.0/beam->criticalTensileStrain) : 1.0 ;
			if(FIXME_useStiffnessSoftening_Factor<0.01) FIXME_useStiffnessSoftening_Factor=0.01;

			assert(FIXME_useStiffnessSoftening_Factor<=1.0);

			Real FIXME_longitudalStiffness = beam->longitudalStiffness*FIXME_useStiffnessSoftening_Factor;
			node1->countStiffness += FIXME_longitudalStiffness;
			node2->countStiffness += FIXME_longitudalStiffness;
			node1->displacementStiffness -= displacementLongitudal * FIXME_longitudalStiffness;
			node2->displacementStiffness += displacementLongitudal * FIXME_longitudalStiffness;
		}
		else
		{ // give 'D' to nodes
			node1->countStiffness += beam->longitudalStiffness;
			node2->countStiffness += beam->longitudalStiffness;
			node1->displacementStiffness -= displacementLongitudal * beam->longitudalStiffness;
			node2->displacementStiffness += displacementLongitudal * beam->longitudalStiffness;
		}

		if( beam->count != 0 )
		if(FIXME_useBendTensileSoftening) // mo?e ten kawa?ek by by? w ró?nych wariantach tej samej funkcji. wariantach - to znaczy np. inna wirtualna specjalizacja. (czyli mam kilka klas, ka?da liczy mój model nieco inaczej)
		{ // 'B' from picture - rotate to align with neighbouring beams
			Real FIXME_bendingStiffness = beam->bendingStiffness * FIXME_useStiffnessSoftening_Factor;
			Real strain                     = beam->strain();

			Real factor;

			if( strain > 0 )
			{
				//                              (   0 ... 0.5 ... 1 ... inf          ) 
				factor                  = 1.0 - (strain / beam->criticalTensileStrain); // positive
				if(factor< 0.0) factor  = 0.0;
				FIXME_bendingStiffness *=factor;
			}
			else
			{
				//                        (  0 ... -0.5 ... -1 ... -inf            )
				factor                  = (strain / beam->criticalCompressiveStrain); // negative
				if(factor<-1.0) factor  =-1.0;
				FIXME_bendingStiffness -= (1.0-FIXME_bendingStiffness)*factor;
			}
			/*
			if( strain > 0 )
			{
				//                        (   0 ... 0.5 ... 1 ... inf          ) 
				factor                  = (strain / beam->criticalTensileStrain); // positive
				if(factor> 1.0) factor  = 1.0;
				FIXME_bendingStiffness +=(1.0-FIXME_bendingStiffness)*factor;
			}
			else
			{
				//                              (  0 ... -0.5 ... -1 ... -inf            )
				factor                  = 1.0 + (strain / beam->criticalCompressiveStrain); // negative
				if(factor< 0.0) factor  = 0.0;
				FIXME_bendingStiffness *= factor;
			}
			*/
			node1->countStiffness += FIXME_bendingStiffness;
			node2->countStiffness += FIXME_bendingStiffness;
			Vector3r axis           = Vector3r( 0.0 , 0.0 , 1.0 ); // 2D - always rotation around z 
			Real angle              = beam->bendingRotation;
			angle /= beam->count;
			Quaternionr rotation;
			rotation.fromAxisAngle(axis,angle);
			Vector3r length = beam->length * beam->direction;// * 0.5;// beam->bendingStiffness / beam->count;// * 0.5; // FIXME - duplicate line
			node1->displacementStiffness += (rotation * ( length) - length) * FIXME_bendingStiffness;
			node2->displacementStiffness += (rotation * (-length) + length) * FIXME_bendingStiffness;
			beam->bendingRotation   = 0.0;
			beam->count             = 0.0;
		}
		else
		{ // 'B' from picture - rotate to align with neighbouring beams

			node1->countStiffness += beam->bendingStiffness  * FIXME_useStiffnessSoftening_Factor;
			node2->countStiffness += beam->bendingStiffness  * FIXME_useStiffnessSoftening_Factor;

			Vector3r axis           = Vector3r( 0.0 , 0.0 , 1.0 ); // 2D - always rotation around z 
			Real angle              = beam->bendingRotation;
			angle /= beam->count;
			
			Quaternionr rotation;
			rotation.fromAxisAngle(axis,angle);
			
			Vector3r length = beam->length * beam->direction;// * 0.5;// beam->bendingStiffness / beam->count;// * 0.5; // FIXME - duplicate line

			node1->displacementStiffness += (rotation * ( length) - length) * beam->bendingStiffness  * FIXME_useStiffnessSoftening_Factor;
			node2->displacementStiffness += (rotation * (-length) + length) * beam->bendingStiffness  * FIXME_useStiffnessSoftening_Factor;

			beam->bendingRotation 	= 0.0;
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
			// FIXME - correct formula
		//	std::cerr << "displ "<< body->getId() <<": (" <<  node->displacementIncremental << ")/" << node->countIncremental <<"+("<< node->displacementStiffness <<")/"<< node->countStiffness << "\n";
			Vector3r displacementTotal      = node->displacementIncremental / node->countIncremental + node->displacementStiffness / node->countStiffness;

			// FIXME - formula in first submisstion to articles of hydroengineering:
		//	Vector3r displacementTotal      = (node->displacementIncremental + node->displacementStiffness / node->countStiffness )/ node->countIncremental;

			// FIXME - formula from old version (better ?!?!)
		//	Vector3r displacementTotal              = (node->displacementIncremental + node->displacementStiffness) / node->countStiffness;

			node->countIncremental          = 0;
			node->countStiffness            = 0;
			node->displacementIncremental 	= Vector3r(0.0,0.0,0.0);
			node->displacementStiffness     = Vector3r(0.0,0.0,0.0);

			if(body->isDynamic)
			{
				node->se3.position      += displacementTotal;
			}
			// FIXME FIXME FIXME FIXME FIXME FIXME FIXME
			else
			{
				if(!roughEdges)// FIXME - else move only in x and z directions
				{
					node->se3.position[0]   += displacementTotal[0];
					node->se3.position[2]   += displacementTotal[2];
				}
			}
			if(ensure2D)
				node->se3.position[2] = 0; // ensure 2D
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
				Vector3r	newCP;
				Vector3r&	lastCP		= (static_cast<LatticeBeamAngularSpring*>((*angles)->interactionPhysics.get()))->lastCrossProduct;
				bool&		planeSwap180	= (static_cast<LatticeBeamAngularSpring*>((*angles)->interactionPhysics.get()))->planeSwap180;
				bool		oldPS180	= planeSwap180;
				Real		planeAngle;
/*
				planeAngle =	(
							beam1->direction.cross(beam2->direction)[2] > 0.0 ? 1.0 : -1.0
						)
						*
						beam1->direction.angleBetweenUnitVectors(beam2->direction);
*/
				newCP		= beam1->direction.cross(beam2->direction);
				if(newCP.dot(lastCP)< 0.0)
					planeSwap180=!planeSwap180;
				
				planeAngle	= ( planeSwap180 ? 1.0 : -1.0 ) * beam1->direction.angleBetweenUnitVectors(beam2->direction);

				if((*angles)->getId2()==7)
					std::cerr << planeAngle << "\n";

				(static_cast<LatticeBeamAngularSpring*>((*angles)->interactionPhysics.get()))->planeAngle = planeAngle;

				if(newCP.squaredLength() > 0.01)
					lastCP = newCP;
				else
					planeSwap180 = oldPS180;
			}
			// else FIXME - delete unused angularSpring
		}
	}
}

