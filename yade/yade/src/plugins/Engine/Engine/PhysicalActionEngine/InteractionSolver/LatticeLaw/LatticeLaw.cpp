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
#include <yade-common/LatticeBeamParameters.hpp>
#include <yade-common/LatticeNodeParameters.hpp>
#include <yade-common/Force.hpp>

#include <yade/BodyContainer.hpp>
#include <yade/MetaBody.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

LatticeLaw::LatticeLaw() : InteractionSolver() , actionForce(new Force)
{
	nodeGroupMask = 1;
	beamGroupMask = 2;
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
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void LatticeLaw::calculateForces(Body* body)
{

	MetaBody * lattice = static_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = lattice->bodies;
	shared_ptr<PhysicalActionContainer>& actionParameters = lattice->actionParameters;
	
	for(bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext()) // loop over all beams
	{
		const shared_ptr<Body>& body = bodies->getCurrent();
		if( ! ( body->getGroupMask() & beamGroupMask ) )
			continue; // skip non-beams
		
		LatticeBeamParameters* beam = dynamic_cast<LatticeBeamParameters*>(body->physicalParameters.get() );
		
		Vector3r  force = beam->direction * ((beam->length/beam->initialLength)-1.0)*10.0;
//		cerr << force << endl;
//		static_cast<Force*> ( actionParameters->find( beam->id1 , actionForce ->getClassIndex() ).get() )->force  -= force;
//		static_cast<Force*> ( actionParameters->find( beam->id2 , actionForce ->getClassIndex() ).get() )->force  += force;
/*		
		for ( int i = 0 ; i < 4 ; ++i ) // loop over all tetrahedron nodes
		{
			FEMNodeData* femNode = static_cast<FEMNodeData*>( (*bodies)[femTet->ids[i]]->physicalParameters.get() );
			
			Vector3r displacement = femNode->se3.position - femNode->initialPosition;
			
			Ue1( i*3    , 0 ) = displacement[0]; // x displacement of node
			Ue1( i*3 + 1, 0 ) = displacement[1]; // y displacement of node
			Ue1( i*3 + 2, 0 ) = displacement[2]; // z displacement of node
		}
		
		fe = - prod( femTet->Ke_ , Ue1 ); // solve this tetrahedron
		
		for ( int i = 0 ; i < 4 ; ++i ) // loop again over all tetrahedron nodes
		{
			Vector3r force = Vector3r(	  fe( i*3     , 0 )
							, fe( i*3 + 1 , 0 )
							, fe( i*3 + 2 , 0 ));
			
			static_cast<Force*>( actionParameters
				->find( femTet->ids[i] , actionForce ->getClassIndex() ).get() )
					->force  += force;
					
			// FIXME - check what's up with invMass in NewtonsForceLaw ???
		}*/
	}

}


