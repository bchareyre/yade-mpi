/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"GravityEngines.hpp"
#include<yade/core/Scene.hpp>

YADE_PLUGIN((GravityEngine)(CentralGravityEngine)(AxialGravityEngine));

void GravityEngine::action(Scene*){
	/* skip bodies that are within a clump;
	 * even if they are marked isDynamic==false, forces applied to them are passed to the clump, which is dynamic;
	 * and since clump is a body with mass equal to the sum of masses of its components, it would have gravity applied twice.
	 *
	 * The choice is to skip (b->isClumpMember()) or (b->isClump()). We rather skip members,
	 * since that will apply smaller number of forces. */
	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
		if(!b || b->isClumpMember()) continue;
		scene->forces.addForce(b->getId(),gravity*b->state->mass);
	}
}

void CentralGravityEngine::action(Scene*){
	const Vector3r& centralPos=Body::byId(centralBody)->state->pos;
	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
		if(!b || b->isClumpMember() || b->getId()==centralBody) continue; // skip clump members and central body
		Real F=accel*b->state->mass;
		Vector3r toCenter=centralPos-b->state->pos; toCenter.Normalize();
		scene->forces.addForce(b->getId(),F*toCenter);
		if(reciprocal) scene->forces.addForce(centralBody,-F*toCenter);
	}
}

void AxialGravityEngine::action(Scene*){
	FOREACH(const shared_ptr<Body>&b, *scene->bodies){
		if(!b || b->isClumpMember()) continue;
		/* http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html */
		const Vector3r& x0=b->state->pos;
		const Vector3r& x1=axisPoint;
		const Vector3r x2=axisPoint+axisDirection;
		Vector3r closestAxisPoint=(x2-x1) * /* t */ (-(x1-x0).Dot(x2-x1))/((x2-x1).SquaredLength());
		Vector3r toAxis=closestAxisPoint-x0; toAxis.Normalize();
		scene->forces.addForce(b->getId(),acceleration*b->state->mass*toAxis);
	}
}
