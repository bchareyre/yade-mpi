/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*  with help from Emmanuel Frangin                                       *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "FEMLaw.hpp"
#include<yade/pkg-fem/FEMTetrahedronData.hpp>
#include<yade/pkg-fem/FEMNodeData.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>

using namespace boost::numeric;


FEMLaw::FEMLaw() : InteractionSolver()
{
	nodeGroupMask = 1;
	tetrahedronGroupMask = 2;
}


FEMLaw::~FEMLaw()
{

}




void FEMLaw::action(MetaBody* fem)
{
	shared_ptr<BodyContainer>& bodies = fem->bodies;
	
	ublas::matrix<double> Ue1 , fe;
	Ue1.resize(12,1);
	fe.resize(12,1);
	
	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for(  ; bi!=biEnd ; ++bi )
	{
		shared_ptr<Body> body = *bi;
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
			fem->bex.addForce(femTet->ids[i],force);
					
		}
	}
}



YADE_PLUGIN((FEMLaw));