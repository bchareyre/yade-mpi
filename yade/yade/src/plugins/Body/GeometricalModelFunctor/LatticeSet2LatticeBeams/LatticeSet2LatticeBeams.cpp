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

#include "LatticeSet2LatticeBeams.hpp"
#include "LatticeSetParameters.hpp"
#include "LatticeBeamParameters.hpp"
#include "LineSegment.hpp"
#include "ComplexBody.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void LatticeSet2LatticeBeams::go(	  const shared_ptr<BodyPhysicalParameters>& ph
					, shared_ptr<GeometricalModel>& 
					, const Body* body)
{
	int beamGroup = dynamic_cast<const LatticeSetParameters*>(ph.get())->beamGroup;
	const ComplexBody * ncb = dynamic_cast<const ComplexBody*>(body);
	const shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
// FIXME - this copying of length between latticeBeam geometry and physics, inside ComplexBody could be done just once, if length was inside shared_ptr. This can be improved once we make indexable Parameters: Velocity, Position, Orientation, ....
cerr << "qwer\n";
	for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext() )
	{
		if( bodies->getCurrent()->getGroup() == beamGroup )
		{
			LineSegment* line 		= dynamic_cast<LineSegment*> 		(bodies->getCurrent()->geometricalModel.get());
			LatticeBeamParameters* beam 	= dynamic_cast<LatticeBeamParameters*>  (bodies->getCurrent()->physicalParameters.get());
			
			line->length = beam->length;
			// FIXME - update color too
		}
	}
}
