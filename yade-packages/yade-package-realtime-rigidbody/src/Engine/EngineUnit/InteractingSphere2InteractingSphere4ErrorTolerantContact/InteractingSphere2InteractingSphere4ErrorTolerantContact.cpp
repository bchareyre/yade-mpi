/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "InteractingSphere2InteractingSphere4ErrorTolerantContact.hpp"
#include "ErrorTolerantContact.hpp"
#include <yade/yade-package-common/Sphere.hpp>


bool InteractingSphere2InteractingSphere4ErrorTolerantContact::go(		const shared_ptr<InteractingGeometry>& cm1,
						const shared_ptr<InteractingGeometry>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						const shared_ptr<Interaction>& c)
{
	shared_ptr<Sphere> s1 = YADE_PTR_CAST<Sphere>(cm1);
	shared_ptr<Sphere> s2 = YADE_PTR_CAST<Sphere>(cm2);

	Vector3r normal = se32.position-se31.position;
	float penetrationDepth = s1->radius+s2->radius-normal.Normalize();

	if (penetrationDepth>0)
	{
		shared_ptr<ErrorTolerantContact> cm = shared_ptr<ErrorTolerantContact>(new ErrorTolerantContact());

		Vector3r pt1 = se31.position+normal*s1->radius;
		Vector3r pt2 = se32.position-normal*s2->radius;
		cm->closestPoints.push_back(std::pair<Vector3r,Vector3r>(pt1,pt2));
		cm->o1p1 = pt1-se31.position;
		cm->o2p2 = pt2-se32.position;
		cm->normal = normal;
		c->interactionGeometry = cm;
		return true;
	}
	else	
		return false;

}


bool InteractingSphere2InteractingSphere4ErrorTolerantContact::goReverse(	const shared_ptr<InteractingGeometry>& cm1,
						const shared_ptr<InteractingGeometry>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						const shared_ptr<Interaction>& c)
{
	return go(cm1,cm2,se31,se32,c);
}


