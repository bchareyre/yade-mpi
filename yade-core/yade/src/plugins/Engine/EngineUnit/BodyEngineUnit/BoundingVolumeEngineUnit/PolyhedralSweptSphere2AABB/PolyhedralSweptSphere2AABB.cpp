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

#include "PolyhedralSweptSphere2AABB.hpp"

#include <yade-common/PolyhedralSweptSphere.hpp>
#include <yade-common/AABB.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void PolyhedralSweptSphere2AABB::go(	  const shared_ptr<InteractingGeometry>& cm
				, shared_ptr<BoundingVolume>& bv
				, const Se3r& se3
				, const Body*	body)
{
	PolyhedralSweptSphere* pss = static_cast<PolyhedralSweptSphere*>(cm.get());
	AABB* aabb = static_cast<AABB*>(bv.get());
	
	
	vector<Vector3r>::iterator vi = pss->vertices.begin();
	vector<Vector3r>::iterator viEnd = pss->vertices.end();
	Vector3r min = Vector3r(Mathr::MAX_REAL,Mathr::MAX_REAL,Mathr::MAX_REAL);
	Vector3r max = -min;
	for( ; vi!=viEnd ; ++vi)
	{
		Vector3r p = (*vi);
		p = body->physicalParameters->se3*p; // FIXME : se"*p is not compiling !!??
		max = p.maxVector(max);
		min = p.minVector(min);
	}
	
	min -= Vector3r(pss->radius,pss->radius,pss->radius);
	max += Vector3r(pss->radius,pss->radius,pss->radius);
	aabb->center = 0.5*(min+max);
	aabb->halfSize = 0.5*(max-min);
	aabb->min = min;
	aabb->max = max;	

}
	
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
