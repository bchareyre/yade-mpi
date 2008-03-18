// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#include"CentralGravityEngine.hpp"
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/ParticleParameters.hpp>

void CentralGravityEngine::applyCondition(Body* _rootBody){
	/* get "rootBody", which is the whole simulation */
	MetaBody* rootBody=YADE_CAST<MetaBody*>(_rootBody);
	/* get the central body, by its id; cast its Body::physicalParameters to ParticleParameters, since we need the mass */
	Real centralMass=YADE_PTR_CAST<ParticleParameters>(Body::byId(centralBody)->physicalParameters)->mass;
	/* position of the central body */
	const Vector3r& centralPos=Body::byId(centralBody)->physicalParameters->se3.position;
	/* loop over all bodies */
	BodyContainer::iterator Iend=rootBody->bodies->end();
	for(BodyContainer::iterator I=rootBody->bodies->begin(); I!=Iend; ++I){
		const shared_ptr<Body>& b(*I);
		if(b->isClumpMember()) continue; // skip clump members
		if(b->getId()==centralBody) continue; // don't gravitate central body to itself (would be div by zero)
		Real myMass=YADE_PTR_CAST<ParticleParameters>(b->physicalParameters)->mass; // mass of the current body; if the body has no mass (responsibility of the user), it will crash gracefully in debug builds
		const Vector3r& myPos=b->physicalParameters->se3.position; // position of the current body;
		Real F=kappa*centralMass*myMass/(centralPos-myPos).SquaredLength(); // force according to the gravity law
		Vector3r toCenter=centralPos-myPos; toCenter.Normalize(); // unit vector between the elements
		// apply force(s): first to the current body
		static_cast<Force*>(rootBody->physicalActions->find(b->getId(),cachedForceClassIndex).get())->force+=F*toCenter;
		// if uncommented, reverse force will be applied to the central body
		// static_cast<Force*>(rootBody->physicalActions->find(centralBody,cachedForceClassIndex).get())->force-=F*toCenter;
	}
}

YADE_PLUGIN("CentralGravityEngine");


