// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#include"CentralGravityEngine.hpp"
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/ParticleParameters.hpp>

void CentralGravityEngine::applyCondition(Body* _rootBody){
	MetaBody* rootBody=YADE_CAST<MetaBody*>(_rootBody);
	Real centralMass=YADE_PTR_CAST<ParticleParameters>(Body::byId(centralBody)->physicalParameters)->mass;
	const Vector3r& centralPos=Body::byId(centralBody)->physicalParameters->se3.position;
	BodyContainer::iterator Iend=rootBody->bodies->end();
	for(BodyContainer::iterator I=rootBody->bodies->begin(); I!=Iend; ++I){
		const shared_ptr<Body>& b(*I);
		if(b->isClumpMember()) continue; // skip clump members
		if(b->getId()==centralBody) continue; // don't gravitate central body to itself
		Real myMass=YADE_PTR_CAST<ParticleParameters>(b->physicalParameters)->mass; // will crash nicely in debug builds
		const Vector3r& myPos=b->physicalParameters->se3.position;
		Real F=kappa*centralMass*myMass/(centralPos-myPos).SquaredLength();
		Vector3r toCenter=centralPos-myPos; toCenter.Normalize();
		static_cast<Force*>(rootBody->physicalActions->find(b->getId(),cachedForceClassIndex).get())->force+=F*toCenter;
	}
}

YADE_PLUGIN("CentralGravityEngine");


