/*************************************************************************
*  Copyright (C) 2006 by luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "InteractingSphere2InteractingSphere4SpheresContactGeometryWater.hpp"

#include <yade/pkg-dem/SpheresContactGeometry.hpp>
#include <yade/pkg-common/InteractingSphere.hpp>



bool InteractingSphere2InteractingSphere4SpheresContactGeometryWater::go(	const shared_ptr<InteractingGeometry>& cm1,
							const shared_ptr<InteractingGeometry>& cm2,
							const Se3r& se31,
							const Se3r& se32,
							const shared_ptr<Interaction>& c)
{
	InteractingSphere* s1 = static_cast<InteractingSphere*>(cm1.get());
	InteractingSphere* s2 = static_cast<InteractingSphere*>(cm2.get());

	/// pb ???
	Vector3r normal = se32.position-se31.position;
	Real penetrationDepth = s1->radius+s2->radius-normal.Normalize();
	
	shared_ptr<SpheresContactGeometry> scm;
	
	if (c->interactionGeometry)	
	{
		scm = dynamic_pointer_cast<SpheresContactGeometry>(c->interactionGeometry);
	}

	// BEGIN .......  FIXME FIXME	- wrong hack, to make cohesion work.
	
	else	
		
	scm = shared_ptr<SpheresContactGeometry>(new SpheresContactGeometry());
		
		
// 	if (penetrationDepth>0)
	{
		scm->contactPoint = se31.position+(s1->radius-0.5*penetrationDepth)*normal;//0.5*(pt1+pt2);
		scm->normal = normal;
		scm->penetrationDepth = penetrationDepth;
		scm->radius1 = s1->radius;
		scm->radius2 = s2->radius;
		
		// ?????
		if (!c->interactionGeometry)
		c->interactionGeometry = scm;
	
	return true;
		
	}
		
// 	else	 //d�finition de g�om�trie d'interaction pour le cas ou !contact
// 	{
// 	scm->normal = normal;
// 	scm->penetrationDepth = penetrationDepth;
// 	scm->radius1 = s1->radius;
// 	scm->radius2 = s2->radius;
// 	
// 	//linkGeometry->normal.normalize();
// 	//linkGeometry->radius1 = s1->radius;
// 	//linkGeometry->radius2 = s2->radius;
// 	
// 	if (!c->interactionGeometry)
// 		c->interactionGeometry = scm;
// 		
// 	//if (!c->interactionGeometry)
// 	//	c->interactionGeometry = linkGeometry;
// 	
// 	return true;
// 	//return false;
// 	}
}



bool InteractingSphere2InteractingSphere4SpheresContactGeometryWater::goReverse(	const shared_ptr<InteractingGeometry>& cm1,
								const shared_ptr<InteractingGeometry>& cm2,
								const Se3r& se31,
								const Se3r& se32,
								const shared_ptr<Interaction>& c)
{
	return go(cm1,cm2,se31,se32,c);
}

YADE_PLUGIN();
