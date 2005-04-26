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

#include "FEMLaw.hpp"
#include "FEMTetrahedronData.hpp"
#include "FEMNodeData.hpp"
#include "ActionParameterForce.hpp"

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
using namespace boost::numeric;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

FEMLaw::FEMLaw() : ConstitutiveLaw() , actionForce(new ActionParameterForce)
{
	nodeGroupMask = 1;
	tetrahedronGroupMask = 2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

FEMLaw::~FEMLaw()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void FEMLaw::registerAttributes()
{
	ConstitutiveLaw::registerAttributes();
	REGISTER_ATTRIBUTE(nodeGroupMask);
	REGISTER_ATTRIBUTE(tetrahedronGroupMask);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void FEMLaw::calculateForces(Body* body)
{
	MetaBody * fem = static_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = fem->bodies;
	shared_ptr<ActionParameterContainer>& actionParameters = fem->actionParameters;
	
	ublas::matrix<double> Ue1 , fe;
	Ue1.resize(12,1);
	fe.resize(12,1);
	
	for(bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext()) // loop over all tetrahedrons
	{
		const shared_ptr<Body>& body = bodies->getCurrent();
		if( ! ( body->getGroupMask() & tetrahedronGroupMask ) )
			continue; // skip non-tetraherons
		
		FEMTetrahedronData* femTet = static_cast<FEMTetrahedronData*>(body->physicalParameters.get() );
		
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
			
			static_cast<ActionParameterForce*>( actionParameters
				->find( femTet->ids[i] , actionForce ->getClassIndex() ).get() )
					->force  += force;
					
			// FIXME - check what's up with invMass in NewtonsForceLawFunctor ???
		}
	}
}



