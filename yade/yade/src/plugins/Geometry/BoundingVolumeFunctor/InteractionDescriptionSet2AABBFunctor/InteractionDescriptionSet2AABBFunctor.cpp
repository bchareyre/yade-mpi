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
 
#include "InteractionDescriptionSet2AABBFunctor.hpp"
#include "InteractionDescriptionSet.hpp"
#include "AABB.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void InteractionDescriptionSet2AABBFunctor::go(	const shared_ptr<InteractionDescription>&,
						const shared_ptr<BoundingVolume>& bv,
						const Se3r&,
						const Body* body )
{
//	shared_ptr<InteractionDescriptionSet> set = dynamic_pointer_cast<InteractionDescriptionSet>(cm);
	
	shared_ptr<AABB> aabb = dynamic_pointer_cast<AABB>(bv);
	
//	vector<shared_ptr<InteractionDescription> >::iterator si = set->interactionGeometries.begin();
//	vector<shared_ptr<InteractionDescription> >::iterator siEnd = set->interactionGeometries.end();
	
	Vector3r max(-Mathr::MAX_REAL,-Mathr::MAX_REAL,-Mathr::MAX_REAL);
	Vector3r min( Mathr::MAX_REAL, Mathr::MAX_REAL, Mathr::MAX_REAL);
	
	const ComplexBody * ncb = dynamic_cast<const ComplexBody*>(body);
	const shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
	for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext())
	{
		shared_ptr<Body>& b = bodies->getCurrent();
 		max = max.maxVector(b->boundingVolume->max);
 		min = min.minVector(b->boundingVolume->min);
	}
	
// 	for( ; si!=siEnd ; ++si)
// 	{
// 		max = max.maxVector((*si)->bv->max);
// 		min = min.minVector((*si)->bv->min);
// 	}
	
	aabb->center = (max+min)*0.5;
	aabb->halfSize = (max-min)*0.5;
	
	aabb->min = min;
	aabb->max = max;
}
	
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
