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
 
#include "InteractionGeometrySet2AABBFactory.hpp"
#include "InteractionGeometrySet.hpp"
#include "AABB.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void InteractionGeometrySet2AABBFactory::go(	const shared_ptr<BodyInteractionGeometry>& cm,
						shared_ptr<BodyBoundingVolume>& bv,
						const Se3r& 	)
{
	shared_ptr<InteractionGeometrySet> set = dynamic_pointer_cast<InteractionGeometrySet>(cm);
	
	shared_ptr<AABB> aabb = dynamic_pointer_cast<AABB>(bv);
	
	vector<shared_ptr<BodyInteractionGeometry> >::iterator si = set->interactionGeometries.begin();
	vector<shared_ptr<BodyInteractionGeometry> >::iterator siEnd = set->interactionGeometries.end();
	Vector3r max(-Mathr::MAX_REAL,-Mathr::MAX_REAL,-Mathr::MAX_REAL);
	Vector3r min(Mathr::MAX_REAL,Mathr::MAX_REAL,Mathr::MAX_REAL);
	for( ; si!=siEnd ; ++si)
	{
		max = max.maxVector((*si)->bv->max);
		min = min.minVector((*si)->bv->min);
	}
	
	aabb->center = (max+min)*0.5;
	aabb->halfSize = (max-min)*0.5;
	
	aabb->min = min;
	aabb->max = max;
}
	
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
