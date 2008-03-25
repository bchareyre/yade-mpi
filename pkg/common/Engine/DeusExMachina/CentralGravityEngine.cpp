// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#include"CentralGravityEngine.hpp"
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/ParticleParameters.hpp>

YADE_PLUGIN("CentralGravityEngine","AxialGravityEngine");

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
CREATE_LOGGER(AxialGravityEngine);
void AxialGravityEngine::applyCondition(Body* _rootBody){
	MetaBody* rootBody=YADE_CAST<MetaBody*>(_rootBody);
	BodyContainer::iterator Iend=rootBody->bodies->end();
	for(BodyContainer::iterator I=rootBody->bodies->begin(); I!=Iend; ++I){
		const shared_ptr<Body>& b(*I);
		if(b->isClumpMember()) continue;
		Real myMass=YADE_PTR_CAST<ParticleParameters>(b->physicalParameters)->mass;
		/* http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html */
		const Vector3r& x0=b->physicalParameters->se3.position;
		const Vector3r& x1=axisPoint;
		const Vector3r x2=axisPoint+axisDirection;
		Vector3r closestAxisPoint=(x2-x1) * /* t */ (-(x1-x0).Dot(x2-x1))/((x2-x1).SquaredLength());
		Vector3r toAxis=closestAxisPoint-x0; toAxis.Normalize();
		static_pointer_cast<Force>(rootBody->physicalActions->find(b->getId(),cachedForceClassIndex))->force+=acceleration*myMass*toAxis;
		//if(b->getId()==20){ TRVAR3(toAxis,acceleration*myMass*toAxis,static_pointer_cast<Force>(rootBody->physicalActions->find(b->getId(),cachedForceClassIndex))->force); }
	}
}


