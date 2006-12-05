/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "InteractingBox2InteractingSphere4SpheresContactGeometry.hpp"
#include "SpheresContactGeometry.hpp"
#include <yade/yade-package-common/InteractingSphere.hpp>
#include <yade/yade-package-common/InteractingBox.hpp>

bool InteractingBox2InteractingSphere4SpheresContactGeometry::go(
		const shared_ptr<InteractingGeometry>& cm1,
		const shared_ptr<InteractingGeometry>& cm2,
		const Se3r& se31,
		const Se3r& se32,
		const shared_ptr<Interaction>& c)
{
	Vector3r l,t,p,q,r;
	bool onborder = false;
	Vector3r pt1,pt2,normal;
	Matrix3r axisT,axis;
	Real depth;

	InteractingSphere* s = static_cast<InteractingSphere*>(cm2.get());
	InteractingBox* obb = static_cast<InteractingBox*>(cm1.get());
	
	Vector3r extents = obb->extents;

	se31.orientation.ToRotationMatrix(axisT);
	axis = axisT.Transpose();
	
	p = se32.position-se31.position;
	
	l[0] = extents[0];
	t[0] = axis.GetRow(0).Dot(p); 
	if (t[0] < -l[0]) { t[0] = -l[0]; onborder = true; }
	if (t[0] >  l[0]) { t[0] =  l[0]; onborder = true; }

	l[1] = extents[1];
	t[1] = axis.GetRow(1).Dot(p);
	if (t[1] < -l[1]) { t[1] = -l[1]; onborder = true; }
	if (t[1] >  l[1]) { t[1] =  l[1]; onborder = true; }

	l[2] = extents[2];
	t[2] = axis.GetRow(2).Dot(p);
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
		
		normal = axisT*tmp;
		
		normal.Normalize();
		
		pt1 = se32.position + normal*min;
		pt2 = se32.position - normal*s->radius;	

		// FIXME : remove those uncommented lines
		shared_ptr<SpheresContactGeometry> scm;
		if (c->isNew)
			scm = shared_ptr<SpheresContactGeometry>(new SpheresContactGeometry());
		else
			scm = dynamic_pointer_cast<SpheresContactGeometry>(c->interactionGeometry);
			
		scm->contactPoint = 0.5*(pt1+pt2);
		scm->normal = pt1-pt2;
		scm->penetrationDepth = scm->normal.Normalize();
		scm->radius1 = s->radius*2;
		scm->radius2 = s->radius;
		c->interactionGeometry = scm;
		
// FIXME : uncommente those lines	
/////////////////////////////////////////////////
// 		shared_ptr<SpheresContactGeometry> scm = shared_ptr<SpheresContactGeometry>(new SpheresContactGeometry());
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
	
	depth = s->radius-sqrt(r.Dot(r));
	
	if (depth < 0) 
		return false;

	pt1 = q + se31.position;

	normal = r;
	normal.Normalize();

	pt2 = se32.position - normal * s->radius;
	
	// FIXME : remove those uncommented lines
	shared_ptr<SpheresContactGeometry> scm;
	if (c->isNew)
		scm = shared_ptr<SpheresContactGeometry>(new SpheresContactGeometry());
	else
		scm = dynamic_pointer_cast<SpheresContactGeometry>(c->interactionGeometry);	
	scm->contactPoint = 0.5*(pt1+pt2);
	scm->normal = pt1-pt2;
	scm->penetrationDepth = scm->normal.Normalize();
	scm->radius1 = s->radius*2;
	scm->radius2 = s->radius;
	c->interactionGeometry = scm;
// FIXME : uncommente those lines	
/////////////////////////////////////////////////	
// 	shared_ptr<SpheresContactGeometry> scm = shared_ptr<SpheresContactGeometry>(new SpheresContactGeometry());
// 	scm->contactPoint = 0.5*(pt1+pt2);
// 	scm->normal = pt1-pt2;
// 	scm->penetrationDepth = scm->normal.normalize();
// 	scm->radius1 = s->radius*2;
// 	scm->radius2 = s->radius;
// 	c->interactionGeometry = scm;
	
	return true;
}


bool InteractingBox2InteractingSphere4SpheresContactGeometry::goReverse(	const shared_ptr<InteractingGeometry>& cm1,
						const shared_ptr<InteractingGeometry>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						const shared_ptr<Interaction>& c)
{
	bool isInteracting = go(cm2,cm1,se32,se31,c);
	if (isInteracting)
	{
		SpheresContactGeometry* scm = static_cast<SpheresContactGeometry*>(c->interactionGeometry.get());
		//Vector3r tmp = scm->closestsPoints[0].first;		
		//scm->closestsPoints[0].first = scm->closestsPoints[0].second;
		//scm->closestsPoints[0].second = tmp;
		scm->normal = -scm->normal;
		Real tmpR  = scm->radius1;
		scm->radius1 = scm->radius2;
		scm->radius2 = tmpR;
	}
	return isInteracting;
}

