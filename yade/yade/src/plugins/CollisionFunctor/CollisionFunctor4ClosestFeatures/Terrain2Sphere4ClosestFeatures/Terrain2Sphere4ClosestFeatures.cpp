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

#include "Terrain2Sphere4ClosestFeatures.hpp"
#include "Sphere.hpp"
#include "Terrain.hpp"
#include "ClosestFeatures.hpp"
#include "Distances3D.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


Terrain2Sphere4ClosestFeatures::Terrain2Sphere4ClosestFeatures ()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

Terrain2Sphere4ClosestFeatures::~Terrain2Sphere4ClosestFeatures ()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Terrain2Sphere4ClosestFeatures::collide(const shared_ptr<CollisionGeometry> cm1, const shared_ptr<CollisionGeometry> cm2, const Se3& se31, const Se3& se32, shared_ptr<Interaction> c)
{
	Vector3 pt;
	std::vector<Vector3> tri;
	bool inCollision =  false;
	std::vector<int> faces;
	
	shared_ptr<Terrain> t = dynamic_pointer_cast<Terrain>(cm1);
	shared_ptr<Sphere> s  = dynamic_pointer_cast<Sphere>(cm2);
	
	Vector3 min,max;
	Vector3 radius = Vector3(s->radius,s->radius,s->radius);
	min = se32.translation-radius;
	max = se32.translation+radius;
	
	t->getFaces(AABB((max-min)*0.5,(min+max)*0.5),faces);
	tri.resize(3);
	
	shared_ptr<ClosestFeatures> cf = shared_ptr<ClosestFeatures>(new ClosestFeatures());
	
	for(unsigned int i=0;i<faces.size();i++)
	{
		int faceId = faces[i];
		tri[0] = t->vertices[t->faces[faceId][0]]+se31.translation;
		tri[1] = t->vertices[t->faces[faceId][1]]+se31.translation;
		tri[2] = t->vertices[t->faces[faceId][2]]+se31.translation;
		float d = sqrDistTriPoint(se32.translation, tri, pt);
				
		if (d<s->radius*s->radius)
		{
			Vector3 v = pt-se32.translation;
			v.unitize();
			cf->closestsPoints.push_back(std::pair<Vector3,Vector3>(pt,se32.translation+v*s->radius));
			inCollision =  true;
		}
	}
		
	if (inCollision)
		c->interactionModel = cf;

	return inCollision;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Terrain2Sphere4ClosestFeatures::reverseCollide(const shared_ptr<CollisionGeometry> cm1, const shared_ptr<CollisionGeometry> cm2,  const Se3& se31, const Se3& se32, shared_ptr<Interaction> c)
{
	bool isColliding = collide(cm2,cm1,se32,se31,c);
	if (isColliding)
	{
		shared_ptr<ClosestFeatures> cf = dynamic_pointer_cast<ClosestFeatures>(c->interactionModel);
		Vector3 tmp = cf->closestsPoints[0].first;
		cf->closestsPoints[0].first = cf->closestsPoints[0].second;		
		cf->closestsPoints[0].second = tmp;
	}
	return isColliding;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
