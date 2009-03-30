/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"GravityEngines.hpp"
#include<yade/pkg-common/ParticleParameters.hpp>
#include<yade/core/MetaBody.hpp>

YADE_PLUGIN("GravityEngine","CentralGravityEngine","AxialGravityEngine");


void GravityEngine::applyCondition(MetaBody* ncb){
	/* skip bodies that are within a clump;
	 * even if they are marked isDynamic==false, forces applied to them are passed to the clump, which is dynamic;
	 * and since clump is a body with mass equal to the sum of masses of its components, it would have gravity applied twice.
	 *
	 * The choice is to skip (b->isClumpMember()) or (b->isClump()). We rather skip members,
	 * since that will apply smaller number of forces. */
	FOREACH(const shared_ptr<Body>& b, *ncb->bodies){
		if(b->isClumpMember()) continue;
		shared_ptr<ParticleParameters> p=YADE_PTR_CAST<ParticleParameters>(b->physicalParameters);
		if(p!=0) //not everything derives from ParticleParameters; this line was    assert(p); - Janek
		ncb->bex.addForce(b->getId(),gravity*p->mass);
	}
}

void CentralGravityEngine::applyCondition(MetaBody* rootBody){
	const Vector3r& centralPos=Body::byId(centralBody)->physicalParameters->se3.position;
	FOREACH(const shared_ptr<Body>& b, *rootBody->bodies){
		if(b->isClumpMember() || b->getId()==centralBody) continue; // skip clump members and central body
		Real F=accel*YADE_PTR_CAST<ParticleParameters>(b->physicalParameters)->mass;
		Vector3r toCenter=centralPos-b->physicalParameters->se3.position; toCenter.Normalize();
		rootBody->bex.addForce(b->getId(),F*toCenter);
		if(reciprocal) rootBody->bex.addForce(centralBody,-F*toCenter);
	}
}

void AxialGravityEngine::applyCondition(MetaBody* rootBody){
	FOREACH(const shared_ptr<Body>&b, *rootBody->bodies){
		if(b->isClumpMember()) continue;
		Real myMass=YADE_PTR_CAST<ParticleParameters>(b->physicalParameters)->mass;
		/* http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html */
		const Vector3r& x0=b->physicalParameters->se3.position;
		const Vector3r& x1=axisPoint;
		const Vector3r x2=axisPoint+axisDirection;
		Vector3r closestAxisPoint=(x2-x1) * /* t */ (-(x1-x0).Dot(x2-x1))/((x2-x1).SquaredLength());
		Vector3r toAxis=closestAxisPoint-x0; toAxis.Normalize();
		rootBody->bex.addForce(b->getId(),acceleration*myMass*toAxis);
	}
}
