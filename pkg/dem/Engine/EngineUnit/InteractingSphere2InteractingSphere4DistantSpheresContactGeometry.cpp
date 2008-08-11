/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"InteractingSphere2InteractingSphere4DistantSpheresContactGeometry.hpp"
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>

#include<yade/lib-base/yadeWm3Extra.hpp>


InteractingSphere2InteractingSphere4DistantSpheresContactGeometry::InteractingSphere2InteractingSphere4DistantSpheresContactGeometry()
{
	interactionDetectionFactor = 1;
}

void InteractingSphere2InteractingSphere4DistantSpheresContactGeometry::registerAttributes()
{	
	REGISTER_ATTRIBUTE(interactionDetectionFactor);
}

bool InteractingSphere2InteractingSphere4DistantSpheresContactGeometry::go(	const shared_ptr<InteractingGeometry>& cm1,
							const shared_ptr<InteractingGeometry>& cm2,
							const Se3r& se31,
							const Se3r& se32,
							const shared_ptr<Interaction>& c)
{
	InteractingSphere *s1 = static_cast<InteractingSphere*>(cm1.get()), *s2=static_cast<InteractingSphere*>(cm2.get());

	Vector3r normal=se32.position-se31.position;
	Real penetrationDepthSq = pow(interactionDetectionFactor*(s1->radius+s2->radius), 2) - normal.SquaredLength();
	//cerr<<"Depth is "<<penetrationDepthSq<<endl;

	shared_ptr<SpheresContactGeometry> scm(c->interactionGeometry ? YADE_PTR_CAST<SpheresContactGeometry>(c->interactionGeometry) : shared_ptr<SpheresContactGeometry>(new SpheresContactGeometry()));

	if (penetrationDepthSq>0 || c->isReal) {
		//cerr<<__FILE__<<":"<<__LINE__<<endl;
		Real penetrationDepth = s1->radius+s2->radius-normal.Normalize(); /* normalize wirks in-place and returns length before normalization; from here, normal is unit vector */
		scm->contactPoint = se31.position+(s1->radius-0.5*penetrationDepth)*normal;
		scm->normal=normal;
		scm->penetrationDepth=penetrationDepth;
		scm->radius1=s1->radius;
		scm->radius2=s2->radius;
		if (!c->interactionGeometry){ c->interactionGeometry = scm; }
		return true;
	} else return false;
}


bool InteractingSphere2InteractingSphere4DistantSpheresContactGeometry::goReverse(	const shared_ptr<InteractingGeometry>& cm1,
								const shared_ptr<InteractingGeometry>& cm2,
								const Se3r& se31,
								const Se3r& se32,
								const shared_ptr<Interaction>& c)
{
	return go(cm1,cm2,se31,se32,c);
}

YADE_PLUGIN();
