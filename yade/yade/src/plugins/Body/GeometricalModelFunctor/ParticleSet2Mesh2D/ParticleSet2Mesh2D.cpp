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

#include "ParticleSet2Mesh2D.hpp"
#include "ParticleSetParameters.hpp"
#include "Mesh2D.hpp"
#include "MetaBody.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void ParticleSet2Mesh2D::go(	  const shared_ptr<PhysicalParameters>&
				, shared_ptr<GeometricalModel>& gm
				, const Body* body)
{
	Mesh2D* mesh2d = static_cast<Mesh2D*>(gm.get());
	
	const MetaBody * ncb = dynamic_cast<const MetaBody*>(body);
	const shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
// FIXME - this copying of Se3 between Mesh2D, and Particles inside MetaBody could be done just once, if Se3 was inside shared_ptr. This can be improved once we make indexable Parameters: Velocity, Position, Orientation, ....

	int particleSetMask = 1; // FIXME - make this a class variable
 	unsigned int i=0;
	for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext() , ++i )
	{
		if( bodies->getCurrent()->getGroupMask() & particleSetMask )// FIXME - this line is a bad hack, to make things work, we should have TWO ComplexBodies, one with cloth, second with spheres. here I assume that spheres and cloth are in the same MetaBody, and that cloth belongs to group 1
			mesh2d->vertices[i]=bodies->getCurrent()->physicalParameters->se3.position;
	}
}
	
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
