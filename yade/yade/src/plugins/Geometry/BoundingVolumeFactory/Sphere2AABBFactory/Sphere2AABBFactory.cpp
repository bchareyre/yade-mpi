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
 
#include "Sphere2AABBFactory.hpp"
#include "InteractionSphere.hpp"
#include "AABB.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Sphere2AABBFactory::go(	const shared_ptr<BodyInteractionGeometry>& cm,
				shared_ptr<BodyBoundingVolume>& bv,
				const Se3r& se3	)
{
	shared_ptr<InteractionSphere> sphere = dynamic_pointer_cast<InteractionSphere>(cm);
	
	shared_ptr<AABB> aabb = dynamic_pointer_cast<AABB>(bv);
	
	aabb->center = se3.translation;
	
	aabb->halfSize = Vector3r(sphere->radius,sphere->radius,sphere->radius);
	
	aabb->min = aabb->center-aabb->halfSize;
	aabb->max = aabb->center+aabb->halfSize;	
// 	cout << "####"<<endl;
// 	cout << aabb->halfSize[0] << " "<<aabb->halfSize[1] << " " <<aabb->halfSize[2] << endl;
// 	cout << aabb->min[0] << " "<<aabb->min[1] << " " <<aabb->min[2] << endl;
// 	cout << aabb->max[0] << " "<<aabb->max[1] << " " <<aabb->max[2] << endl;
}
	
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
