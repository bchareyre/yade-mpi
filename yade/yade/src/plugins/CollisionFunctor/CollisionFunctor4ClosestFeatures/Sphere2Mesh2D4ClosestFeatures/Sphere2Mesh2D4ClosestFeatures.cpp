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
#include "Sphere.hpp"
#include "Mesh2D.hpp"
#include "ClosestFeatures.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


Sphere2Mesh2D4ClosestFeatures::Sphere2Mesh2D4ClosestFeatures ()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

Sphere2Mesh2D4ClosestFeatures::~Sphere2Mesh2D4ClosestFeatures ()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Sphere2Mesh2D4ClosestFeatures::collide(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2, const Se3& se31, const Se3& se32, shared_ptr<Interaction> c)
{

	shared_ptr<Sphere> s = dynamic_pointer_cast<Sphere>(cm1);
	shared_ptr<Mesh2D> m = dynamic_pointer_cast<Mesh2D>(cm2);

	shared_ptr<ClosestFeatures> cf = shared_ptr<ClosestFeatures>(new ClosestFeatures());

	cf->closestsPoints.resize(0);

	Vector3 center = se31.translation;

	//cout << m->vertices.size() << endl;
	
	for(int i=0;i<m->vertices.size();i++)
	{
		Vector3 v = m->vertices[i]-center;
		float l = v.unitize();

		//cout << i << " " << l << " " << s->radius << endl;
		if (l<s->radius)
		{
			//cout << i << " " << l << " " << s->radius << endl;
			cf->closestsPoints.push_back(std::pair<Vector3,Vector3>(m->vertices[i],center+v*s->radius));
		}
	}

	if (cf->closestsPoints.size()>0)
	{
		//cout <<"______________" << cf->closestsPoints.size() << endl;
		c->interactionModel = cf;
		//cout << "collision!!!!!!!!!" << endl;
		return true;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Sphere2Mesh2D4ClosestFeatures::reverseCollide(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2,  const Se3& se31, const Se3& se32, shared_ptr<Interaction> c)
{
	bool isColliding = collide(cm2,cm1,se32,se31,c);
	if (isColliding)
	{
		shared_ptr<ClosestFeatures> cf = dynamic_pointer_cast<ClosestFeatures>(c->interactionModel);

		for(int i=0;i<cf->closestsPoints.size();i++)
		{
			Vector3 tmp = cf->closestsPoints[i].first;
			cf->closestsPoints[i].first = cf->closestsPoints[i].second;
			cf->closestsPoints[i].second = tmp;
		}
	}
	return isColliding;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
