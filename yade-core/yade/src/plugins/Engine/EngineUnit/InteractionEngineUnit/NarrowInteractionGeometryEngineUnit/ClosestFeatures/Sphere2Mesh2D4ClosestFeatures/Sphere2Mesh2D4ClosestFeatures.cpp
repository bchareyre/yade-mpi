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

#include "Sphere2Mesh2D4ClosestFeatures.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-common/Sphere.hpp>
#include <yade/yade-common/Mesh2D.hpp>
#include <yade/yade-common/ClosestFeatures.hpp>
#include <yade/yade-lib-computational-geometry/Distances3D.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Sphere2Mesh2D4ClosestFeatures::go(		const shared_ptr<InteractingGeometry>& cm1,
						const shared_ptr<InteractingGeometry>& cm2,
						const Se3r& se31,
						const Se3r& /*se32*/,
						const shared_ptr<Interaction>& c)
{

	shared_ptr<Sphere> s = dynamic_pointer_cast<Sphere>(cm1);
	shared_ptr<Mesh2D> m = dynamic_pointer_cast<Mesh2D>(cm2);

	shared_ptr<ClosestFeatures> cf = shared_ptr<ClosestFeatures>(new ClosestFeatures());

	cf->closestsPoints.resize(0);

	Vector3r center = se31.position;
	
	/*for(int i=0;i<m->vertices.size();i++)
	{
		Vector3r v = m->vertices[i]-center;
		Real l = v.unitize();

		if (l<s->radius)
		{
			cf->verticesId.push_back(i);
			cf->closestsPoints.push_back(std::pair<Vector3r,Vector3r>(center+v*s->radius,m->vertices[i]));
		}
	}*/

	std::vector<Vector3r> tri;
	tri.resize(3);
	Vector3r pt;
	
	for(unsigned int i=0;i<m->faces.size();i++)
	{
		tri[0] = m->vertices[m->faces[i][0]];
		tri[1] = m->vertices[m->faces[i][1]];
		tri[2] = m->vertices[m->faces[i][2]];
		Real d = sqrDistTriPoint(center, tri, pt);
				
		if (d<s->radius*s->radius)
		{
			Vector3r v = pt-center;
			v.normalize();
			cf->closestsPoints.push_back(std::pair<Vector3r,Vector3r>(center+v*s->radius,pt));
			cf->verticesId.push_back(i);
		}
	}
	

	if (cf->closestsPoints.size()>0)
	{
		c->interactionGeometry = cf;
		return true;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Sphere2Mesh2D4ClosestFeatures::goReverse(	const shared_ptr<InteractingGeometry>& cm1,
						const shared_ptr<InteractingGeometry>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						const shared_ptr<Interaction>& c)
{
	bool isInteracting = go(cm2,cm1,se32,se31,c);
	if (isInteracting)
	{
		shared_ptr<ClosestFeatures> cf = dynamic_pointer_cast<ClosestFeatures>(c->interactionGeometry);

		for(unsigned int i=0;i<cf->closestsPoints.size();i++)
		{
			Vector3r tmp = cf->closestsPoints[i].first;
			cf->closestsPoints[i].first = cf->closestsPoints[i].second;
			cf->closestsPoints[i].second = tmp;
		}
	}
	return isInteracting;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
