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
#include "MetaBody.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void FEMSet2Tetrahedrons::go(	  const shared_ptr<BodyPhysicalParameters>& ph
				, shared_ptr<GeometricalModel>& 
				, const Body* body)
{
	int tetrahedronGroupMask = dynamic_cast<const FEMSetParameters*>(ph.get())->tetrahedronGroupMask;
	const MetaBody * cb = dynamic_cast<const MetaBody*>(body);
	const shared_ptr<BodyContainer>& bodies = cb->bodies;
	
// FIXME - this copying of data! 
	for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext() )
	{
		if( bodies->getCurrent()->getGroupMask() & tetrahedronGroupMask )
		{
			Tetrahedron* tet 		= dynamic_cast<Tetrahedron*>        (bodies->getCurrent()->geometricalModel.get());
			FEMTetrahedronData* tetData	= dynamic_cast<FEMTetrahedronData*> (bodies->getCurrent()->physicalParameters.get());
			
			tet->v1 			= (*(cb->bodies))[tetData->ids[0]]->physicalParameters->se3.position;
			tet->v2 			= (*(cb->bodies))[tetData->ids[1]]->physicalParameters->se3.position;
			tet->v3 			= (*(cb->bodies))[tetData->ids[2]]->physicalParameters->se3.position;
			tet->v4 			= (*(cb->bodies))[tetData->ids[3]]->physicalParameters->se3.position;
		}
	}
}

