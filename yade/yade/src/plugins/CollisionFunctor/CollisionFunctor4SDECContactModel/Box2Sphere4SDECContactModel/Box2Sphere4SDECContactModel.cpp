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

#include "Box2Sphere4SDECContactModel.hpp"
#include "Sphere.hpp"
#include "Box.hpp"
#include "SDECContactModel.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Box2Sphere4SDECContactModel::go(		const shared_ptr<CollisionGeometry>& cm1,
						const shared_ptr<CollisionGeometry>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						shared_ptr<Interaction>& c)
{

	//if (se31.rotation == Quaternionr())
	//	return collideAABoxSphere(cm1,cm2,se31,se32,c);
	
	Vector3r l,t,p,q,r;
	bool onborder = false;
	Vector3r pt1,pt2,normal;
	Matrix3r axisT,axis;
	float depth;

	shared_ptr<Sphere> s = dynamic_pointer_cast<Sphere>(cm2);
	shared_ptr<Box> obb = dynamic_pointer_cast<Box>(cm1);
	
	Vector3r extents = obb->extents;

	se31.rotation.toRotationMatrix(axisT);
	axis = axisT.transpose();
	
	p = se32.translation-se31.translation;
	
	l[0] = extents[0];
	t[0] = axis.getRow(0).dot(p); 
	if (t[0] < -l[0]) { t[0] = -l[0]; onborder = true; }
	if (t[0] >  l[0]) { t[0] =  l[0]; onborder = true; }

	l[1] = extents[1];
	t[1] = axis.getRow(1).dot(p);
	if (t[1] < -l[1]) { t[1] = -l[1]; onborder = true; }
	if (t[1] >  l[1]) { t[1] =  l[1]; onborder = true; }

	l[2] = extents[2];
	t[2] = axis.getRow(2).dot(p);
	if (t[2] < -l[2]) { t[2] = -l[2]; onborder = true; }
	if (t[2] >  l[2]) { t[2] =  l[2]; onborder = true; }
	
	if (!onborder) 
	{
		// sphere center inside box. find largest `t' value
		float min = l[0]-fabs(t[0]);
		int mini = 0;
		for (int i=1; i<3; i++) 
		{
			float tt = l[i]-fabs(t[i]);
			if (tt < min) 
			{
				min = tt;
				mini = i;
			}
		}
		
		// contact normal aligned with box edge along largest `t' value
		Vector3r tmp = Vector3r(0,0,0);

		tmp[mini] = (t[mini] > 0) ? 1.0 : -1.0;
		
		normal = axisT*tmp;
		
		normal.normalize();
		
		pt1 = se32.translation + normal*min;
		pt2 = se32.translation - normal*s->radius;	

		// FIXME : remove those uncommented lines
		shared_ptr<SDECContactModel> scm;
		if (c->isNew)
			scm = shared_ptr<SDECContactModel>(new SDECContactModel());
		else
			scm = dynamic_pointer_cast<SDECContactModel>(c->interactionGeometry);
			
		scm->contactPoint = 0.5*(pt1+pt2);
		scm->normal = pt1-pt2;
		scm->penetrationDepth = scm->normal.normalize();
		scm->radius1 = s->radius*2;
		scm->radius2 = s->radius;
		c->interactionGeometry = scm;
		
// FIXME : uncommente those lines	
/////////////////////////////////////////////////
// 		shared_ptr<SDECContactModel> scm = shared_ptr<SDECContactModel>(new SDECContactModel());
// 		scm->contactPoint = 0.5*(pt1+pt2);
// 		scm->normal = pt1-pt2;
// 		scm->penetrationDepth = scm->normal.normalize();
// 		scm->radius1 = s->radius*2;
// 		scm->radius2 = s->radius;
// 		c->interactionGeometry = scm;
		
		return true;	
	}
	//FIXME : use else instead and signle return
	
	q = axisT*t;
	r = p - q;
	
	depth = s->radius-sqrt(r.dot(r));
	
	if (depth < 0) 
		return false;

	pt1 = q + se31.translation;

	normal = r;
	normal.normalize();

	pt2 = se32.translation - normal * s->radius;
	
	// FIXME : remove those uncommented lines
	shared_ptr<SDECContactModel> scm;
	if (c->isNew)
		scm = shared_ptr<SDECContactModel>(new SDECContactModel());
	else
		scm = dynamic_pointer_cast<SDECContactModel>(c->interactionGeometry);	
	scm->contactPoint = 0.5*(pt1+pt2);
	scm->normal = pt1-pt2;
	scm->penetrationDepth = scm->normal.normalize();
	scm->radius1 = s->radius*2;
	scm->radius2 = s->radius;
	c->interactionGeometry = scm;
// FIXME : uncommente those lines	
/////////////////////////////////////////////////	
// 	shared_ptr<SDECContactModel> scm = shared_ptr<SDECContactModel>(new SDECContactModel());
// 	scm->contactPoint = 0.5*(pt1+pt2);
// 	scm->normal = pt1-pt2;
// 	scm->penetrationDepth = scm->normal.normalize();
// 	scm->radius1 = s->radius*2;
// 	scm->radius2 = s->radius;
// 	c->interactionGeometry = scm;
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Box2Sphere4SDECContactModel::goReverse(	const shared_ptr<CollisionGeometry>& cm1,
						const shared_ptr<CollisionGeometry>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						shared_ptr<Interaction>& c)
{
	bool isColliding = go(cm2,cm1,se32,se31,c);
	if (isColliding)
	{
		shared_ptr<SDECContactModel> scm = dynamic_pointer_cast<SDECContactModel>(c->interactionGeometry);
		//Vector3r tmp = scm->closestsPoints[0].first;		
		//scm->closestsPoints[0].first = scm->closestsPoints[0].second;
		//scm->closestsPoints[0].second = tmp;
		scm->normal = -scm->normal;
		float tmpR  = scm->radius1;
		scm->radius1 = scm->radius2;
		scm->radius2 = tmpR;
	}
	return isColliding;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
