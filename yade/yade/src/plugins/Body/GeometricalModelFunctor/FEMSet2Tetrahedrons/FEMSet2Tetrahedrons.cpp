/***************************************************************************
 *   Copyright (C) 2005 by Janek Kozicki                                   *
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

#include "FEMSet2Tetrahedrons.hpp"
#include "FEMSetParameters.hpp"
#include "FEMTetrahedronData.hpp"
#include "Tetrahedron.hpp"
#include "ComplexBody.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void FEMSet2Tetrahedrons::go(	  const shared_ptr<BodyPhysicalParameters>& ph
				, shared_ptr<GeometricalModel>& 
				, const Body* body)
{
	/*
	int tetrahedronGroupMask = dynamic_cast<const FEMSetParameters*>(ph.get())->tetrahedronGroupMask;
	const ComplexBody * ncb = dynamic_cast<const ComplexBody*>(body);
	const shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
// FIXME - this copying of data! 

	for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext() )
	{
		if( bodies->getCurrent()->getGroupMask() & tetrahedronGroupMask )
		{
			LatticeBeamParameters* beam 	= dynamic_cast<LatticeBeamParameters*>  (bodies->getCurrent()->physicalParameters.get());
			
			line->length = beam->length;
		}
	}
	*/
}

