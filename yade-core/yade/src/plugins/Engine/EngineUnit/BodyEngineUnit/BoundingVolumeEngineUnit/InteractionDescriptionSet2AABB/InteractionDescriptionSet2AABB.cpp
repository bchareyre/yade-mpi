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

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
 
#include "InteractionDescriptionSet2AABB.hpp"
#include <yade-common/MetaInteractingGeometry.hpp>
#include <yade-common/AABB.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void InteractionDescriptionSet2AABB::go(	  const shared_ptr<InteractingGeometry>&
						, shared_ptr<BoundingVolume>& bv
						, const Se3r&
						, const Body* body )
{
	AABB* aabb = static_cast<AABB*>(bv.get());
	
	Vector3r max(-Mathr::MAX_REAL,-Mathr::MAX_REAL,-Mathr::MAX_REAL);
	Vector3r min( Mathr::MAX_REAL, Mathr::MAX_REAL, Mathr::MAX_REAL);
	
	const MetaBody * ncb = dynamic_cast<const MetaBody*>(body);
	const shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
	for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext())
	{
		shared_ptr<Body>& b = bodies->getCurrent();
		if(b->boundingVolume)
		{
	 		max = max.maxVector(b->boundingVolume->max);
 			min = min.minVector(b->boundingVolume->min);
		} 
		else
		{
	 		max = max.maxVector(b->physicalParameters->se3.position);
 			min = min.minVector(b->physicalParameters->se3.position);
		}
	}
	
	aabb->center = (max+min)*0.5;
	aabb->halfSize = (max-min)*0.5;
	
	aabb->min = min;
	aabb->max = max;
}
	
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
