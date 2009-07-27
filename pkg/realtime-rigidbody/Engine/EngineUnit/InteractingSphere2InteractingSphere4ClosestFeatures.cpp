/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "InteractingSphere2InteractingSphere4ClosestFeatures.hpp"
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/ClosestFeatures.hpp>


bool InteractingSphere2InteractingSphere4ClosestFeatures::go(		const shared_ptr<InteractingGeometry>& cm1,
						const shared_ptr<InteractingGeometry>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						const shared_ptr<Interaction>& c)
{
	shared_ptr<InteractingSphere> s1 = YADE_PTR_CAST<InteractingSphere>(cm1);
	shared_ptr<InteractingSphere> s2 = YADE_PTR_CAST<InteractingSphere>(cm2);
	
	Vector3r v = se31.position-se32.position;
	Real l = v.Normalize();
	
	if (l<s1->radius+s2->radius)
	{
		shared_ptr<ClosestFeatures> cf = shared_ptr<ClosestFeatures>(new ClosestFeatures());
		cf->closestsPoints.push_back(std::pair<Vector3r,Vector3r>(se31.position-v*s1->radius,se32.position+v*s2->radius));
		c->interactionGeometry = cf;
		// add void interactionPhysics, even though ClosestFeatures doesn't use it. Interaction::isReal tests its presence, however.
		c->interactionPhysics=shared_ptr<InteractionPhysics>(new InteractionPhysics());

		return true;
	}
	else	
		return false;

}


bool InteractingSphere2InteractingSphere4ClosestFeatures::goReverse(	const shared_ptr<InteractingGeometry>& cm1,
						const shared_ptr<InteractingGeometry>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						const shared_ptr<Interaction>& c)
{
	return go(cm1,cm2,se31,se32,c);
}

YADE_PLUGIN("InteractingSphere2InteractingSphere4ClosestFeatures");