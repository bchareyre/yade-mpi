/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "AAInteractingBox2InteractingSphere4ClosestFeatures.hpp"

#include <yade/yade-package-common/InteractingBox.hpp>
#include <yade/yade-package-common/InteractingSphere.hpp>
#include <yade/yade-package-common/ClosestFeatures.hpp>

bool AAInteractingBox2InteractingSphere4ClosestFeatures::go(		const shared_ptr<InteractingGeometry>& cm1,
						const shared_ptr<InteractingGeometry>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						const shared_ptr<Interaction>& c)
{
	Vector3r l,t,p,q,r;
	bool onborder = false;
	Vector3r pt1,pt2,normal;
	Real depth;
	
	shared_ptr<InteractingSphere> s = dynamic_pointer_cast<InteractingSphere>(cm2);
	shared_ptr<InteractingBox>  aaBox = dynamic_pointer_cast<InteractingBox>(cm1);
	
	p = se32.position-se31.position;
	
	l[0] = aaBox->extents[0];
	t[0] = p[0]; 
	if (t[0] < -l[0]) { t[0] = -l[0]; onborder = true; }
	if (t[0] >  l[0]) { t[0] =  l[0]; onborder = true; }

	l[1] = aaBox->extents[1];
	t[1] = p[1];
	if (t[1] < -l[1]) { t[1] = -l[1]; onborder = true; }
	if (t[1] >  l[1]) { t[1] =  l[1]; onborder = true; }

	l[2] = aaBox->extents[2];
	t[2] = p[2];
	if (t[2] < -l[2]) { t[2] = -l[2]; onborder = true; }
	if (t[2] >  l[2]) { t[2] =  l[2]; onborder = true; }

	
	if (!onborder) 
	{
		// sphere center inside box. find largest `t' value
		Real min = l[0]-fabs(t[0]);
		int mini = 0;
		for (int i=1; i<3; i++) 
		{
			Real tt = l[i]-fabs(t[i]);
			if (tt < min) 
			{
				min = tt;
				mini = i;
			}
		}
		
		// contact normal aligned with box edge along largest `t' value
		Vector3r tmp = Vector3r(0,0,0);

		tmp[mini] = (t[mini] > 0) ? 1.0 : -1.0;
		
		normal = tmp;
		
		normal.normalize();
		
		pt1 = se32.position + normal*min;
		pt2 = se32.position - normal*s->radius;
	
		shared_ptr<ClosestFeatures> cf = shared_ptr<ClosestFeatures>(new ClosestFeatures());
		cf->closestsPoints.push_back(std::pair<Vector3r,Vector3r>(pt1,pt2));
		c->interactionGeometry = cf;
		
		return true;
	}

	q = t;
	
	r = p - q;
	
	depth = s->radius-sqrt(r.dot(r));
	
	if (depth < 0)
		return false;
	
	pt1 = q + se31.position;

	normal = r;
	normal.normalize();

	pt2 = se32.position - normal * s->radius;
		
	shared_ptr<ClosestFeatures> cf = shared_ptr<ClosestFeatures>(new ClosestFeatures());
	cf->closestsPoints.push_back(std::pair<Vector3r,Vector3r>(pt1,pt2));
	c->interactionGeometry = cf;
	
	return true;	
}


bool AAInteractingBox2InteractingSphere4ClosestFeatures::goReverse(	const shared_ptr<InteractingGeometry>& cm1,
						const shared_ptr<InteractingGeometry>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						const shared_ptr<Interaction>& c)
{
	bool isInteracting = go(cm2,cm1,se32,se31,c);
	if (isInteracting)
	{
		shared_ptr<ClosestFeatures> cf = dynamic_pointer_cast<ClosestFeatures>(c->interactionGeometry);
		Vector3r tmp = cf->closestsPoints[0].first;
		cf->closestsPoints[0].first = cf->closestsPoints[0].second;		
		cf->closestsPoints[0].second = tmp;
	}
	return isInteracting;
}

