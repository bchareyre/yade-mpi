// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#include"UniaxialStrainControlledTest.hpp"
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/Box.hpp>
#include<yade/pkg-common/InteractingBox.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/extra/Brefcom.hpp>
#include<boost/foreach.hpp>

#include<yade/core/InteractionContainer.hpp>

YADE_PLUGIN("USCTGen","UniaxialStrainer" /*,"UniaxialStrainSensorPusher"*/ );

/************************ UniaxialStrainer **********************/
CREATE_LOGGER(UniaxialStrainer);

void UniaxialStrainer::init(){
	needsInit=false;

	assert(posIds.size()>0);
	assert(negIds.size()>0);
	posCoords.clear(); negCoords.clear();
	FOREACH(body_id_t id,posIds){ const shared_ptr<Body>& b=Body::byId(id); posCoords.push_back(b->physicalParameters->se3.position[axis]);
		if(blockDisplacements && blockRotations) b->isDynamic=false;
		else{
			shared_ptr<PhysicalParameters> &pp=b->physicalParameters;
			if(!blockDisplacements)pp->blockedDOFs=PhysicalParameters::axisDOF(axis); else pp->blockedDOFs=PhysicalParameters::DOF_XYZ;
			if(blockRotations) pp->blockedDOFs|=PhysicalParameters::DOF_RXRYRZ;
		}
	}
	FOREACH(body_id_t id,negIds){ const shared_ptr<Body>& b=Body::byId(id); negCoords.push_back(b->physicalParameters->se3.position[axis]);
		if(blockDisplacements && blockRotations) b->isDynamic=false;
		else{
			shared_ptr<PhysicalParameters> &pp=b->physicalParameters;
			if(!blockDisplacements)pp->blockedDOFs=PhysicalParameters::axisDOF(axis); else pp->blockedDOFs=PhysicalParameters::DOF_XYZ;
			if(blockRotations) pp->blockedDOFs|=PhysicalParameters::DOF_RXRYRZ;
		}
	}

	assert(posIds.size()==posCoords.size() && negIds.size()==negCoords.size());

	originalLength=axisCoord(posIds[0])-axisCoord(negIds[0]);
	LOG_DEBUG("Reference particles: positive #"<<posIds[0]<<" at "<<axisCoord(posIds[0])<<"; negative #"<<negIds[0]<<" at "<<axisCoord(negIds[0]));
	LOG_INFO("Setting initial length to "<<originalLength<<" (between #"<<negIds[0]<<" and #"<<posIds[0]<<")");
	if(originalLength<=0) LOG_FATAL("Initial length is negative or zero (swapped reference particles?)! "<<originalLength);
	/* this happens is nan propagates from e.g. brefcom consitutive law in case 2 bodies have _exactly_ the same position
	 * (the the normal strain is 0./0.=nan). That is an user's error, however and should not happen. */
	if(isnan(originalLength)) LOG_FATAL("Initial length is NaN!");
	assert(originalLength>0 && !isnan(originalLength));

	assert(!isnan(strainRate) || !isnan(absSpeed));
	if(isnan(strainRate)){ strainRate=absSpeed/originalLength; }

	initAccelTime_s=initAccelTime>=0 ? initAccelTime : Omega::instance().getTimeStep()*(-initAccelTime);

	/* if we have default (<0) crossSectionArea, try to get it from root's AABB;
	 * this will not work if there are foreign bodies in the simulation,
	 * in which case you must give the value yourself as engine attribute.
	 *
	 * A TODO option is to get crossSectionArea as average area of bounding boxes' of ABBBs
	 * of posIds and negIds perpendicular to axis. That might be better, except for cases where
	 * reference particles on either end do not coincide with the specimen cross-section.
	 *
	 * */
	if(crossSectionArea<=0){
		shared_ptr<AABB> rbAABB;
		if (Omega::instance().getRootBody()->boundingVolume && (rbAABB=dynamic_pointer_cast<AABB>(Omega::instance().getRootBody()->boundingVolume))){
			int axis2=(axis+1)%3, axis3=(axis+2)%3; // perpendicular axes indices
			crossSectionArea=4*rbAABB->halfSize[axis2]*rbAABB->halfSize[axis3];
			LOG_INFO("Setting crossSectionArea="<<crossSectionArea<<", using axes #"<<axis2<<" and #"<<axis3<<".");
		} else {
			crossSectionArea=1.;
			LOG_WARN("No Axis Aligned Bounding Box for rootBody, using garbage value ("<<crossSectionArea<<") for crossSectionArea!");
		}
	}
	assert(crossSectionArea>0);
	prepareRecStream();
#if 0
	setupTransStrainSensors();
#endif
}

void UniaxialStrainer::applyCondition(MetaBody* rootBody){
	if(needsInit) init();
	// postconditions for initParams
	assert(posIds.size()==posCoords.size() && negIds.size()==negCoords.size() && originalLength>0 && crossSectionArea>0);
	//nothing to do
	if(posIds.size()==0 || negIds.size()==0) return;
	// linearly increase strain to the desired value
	if(abs(currentStrainRate)<abs(strainRate)){
		Real t=Omega::instance().getSimulationTime();
		currentStrainRate=(t/initAccelTime_s)*strainRate;
	} else currentStrainRate=strainRate;
	// how much do we move (in total, symmetry handled below)
	Real dAX=currentStrainRate*originalLength*Omega::instance().getTimeStep();
	if(!isnan(stopStrain)){
		Real axialLength=axisCoord(posIds[0])-axisCoord(negIds[0]);
		Real newStrain=(axialLength+dAX)/originalLength-1;
		if((newStrain*stopStrain>0) && abs(newStrain)>=stopStrain){ // same sign of newStrain and stopStrain && over the limit from below in abs values
			dAX=originalLength*(stopStrain+1)-axialLength;
			LOG_INFO("Reached stopStrain "<<stopStrain<<", deactivating self and stopping in "<<idleIterations+1<<" iterations.");
			this->active=false;
			rootBody->stopAtIteration=Omega::instance().getCurrentIteration()+1+idleIterations;
		}
	}
	if(asymmetry==0) dAX*=.5; // apply half on both sides if straining symetrically
	for(size_t i=0; i<negIds.size(); i++){
		if(asymmetry==0 || asymmetry==-1 /* for +1, don't move*/) negCoords[i]-=dAX;
		axisCoord(negIds[i])=negCoords[i]; // update current position
	}
	for(size_t i=0; i<posIds.size(); i++){
		if(asymmetry==0 || asymmetry==1 /* for -1, don't move */) posCoords[i]+=dAX;
		axisCoord(posIds[i])=posCoords[i];
	}

	Real axialLength=axisCoord(posIds[0])-axisCoord(negIds[0]);
	strain=axialLength/originalLength-1;
	if(Omega::instance().getCurrentIteration()%400==0) TRVAR5(dAX,axialLength,originalLength,currentStrainRate,strain);

	// reverse if we're over the limit strain
	if(notYetReversed && limitStrain!=0 && ((currentStrainRate>0 && strain>limitStrain) || (currentStrainRate<0 && strain<limitStrain))) { currentStrainRate*=-1; notYetReversed=false; LOG_INFO("Reversed strain rate to "<<currentStrainRate); }

	if(Omega::instance().getCurrentIteration()%10==0) {
		computeAxialForce(rootBody);
		avgStress=(sumPosForces+sumNegForces)/(2*crossSectionArea); // average nominal stress
		if(!recordFile.empty() && recStream.good()) recStream<<Omega::instance().getCurrentIteration()<<" "<<strain<<" "<<avgStress<<endl; // <<" "<<avgTransStrain<<endl;
	}
}

void UniaxialStrainer::computeAxialForce(MetaBody* rootBody){
	sumPosForces=sumNegForces=0;
	rootBody->bex.sync();
	FOREACH(body_id_t id, negIds) sumNegForces+=rootBody->bex.getForce(id)[axis];
	FOREACH(body_id_t id, posIds) sumPosForces-=rootBody->bex.getForce(id)[axis];
}

/***************************************** USCTGen **************************/
CREATE_LOGGER(USCTGen);


bool USCTGen::generate(){
	message="";
	rootBody=Shop::rootBody();
	//Shop::rootBodyActors(rootBody);
	createEngines();
	shared_ptr<UniaxialStrainer> strainer(new UniaxialStrainer);
	rootBody->engines.push_back(strainer); // updating params later
	strainer->strainRate=strainRate;
	strainer->axis=axis;
	strainer->limitStrain=limitStrain;
	
	// load spheres
	Vector3r minXYZ,maxXYZ;
	typedef vector<pair<Vector3r,Real> > vecVecReal;

	vecVecReal spheres;
	if(spheresFile.empty()){ 
		LOG_INFO("spheresFile empty, loading hardwired Shop::smallSdecXyzData (examples/small.sdec.xyz).");
		spheres=Shop::loadSpheresSmallSdecXyz(minXYZ,maxXYZ);
	}
	else spheres=Shop::loadSpheresFromFile(spheresFile,minXYZ,maxXYZ);


	TRVAR2(minXYZ,maxXYZ);
	// get spheres that are "close enough" to the strained ends
	for(vecVecReal::iterator I=spheres.begin(); I!=spheres.end(); I++){
		Vector3r C=I->first;
		Real r=I->second;
		shared_ptr<Body> S=Shop::sphere(C,r);

		// replace BodyMacroParameters by BrefcomPhysParams
		shared_ptr<BodyMacroParameters> bmp=YADE_PTR_CAST<BodyMacroParameters>(S->physicalParameters);
		shared_ptr<BrefcomPhysParams> bpp(new BrefcomPhysParams);
		#define _CP(attr) bpp->attr=bmp->attr;
		_CP(acceleration); _CP(angularVelocity); _CP(blockedDOFs); _CP(frictionAngle); _CP(inertia); _CP(mass); _CP(poisson); _CP(refSe3); _CP(se3); _CP(young); _CP(velocity);
		#undef _CP
		S->physicalParameters=bpp;

		body_id_t sId=rootBody->bodies->insert(S);

		Real distFactor=1.2;
		if (C[axis]-distFactor*r<minXYZ[axis]) {
			strainer->negIds.push_back(sId);
			strainer->negCoords.push_back(C[axis]);
			LOG_DEBUG("NEG inserted #"<<sId<<" with C[axis]="<<C[axis]);
		}
		if (C[axis]+distFactor*r>maxXYZ[axis]) {
			strainer->posIds.push_back(sId);
			strainer->posCoords.push_back(C[axis]);
			LOG_DEBUG("POS inserted #"<<sId<<" with C[axis]="<<C[axis]);
		}
	}

	return true;
}

#include<yade/extra/Brefcom.hpp>

#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/pkg-common/InteractionPhysicsEngineUnit.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-common/PhysicalActionContainerReseter.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/BoundingVolumeMetaEngine.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-common/InteractingSphere2AABB.hpp>
#include<yade/pkg-common/MetaInteractingGeometry.hpp>
#include<yade/pkg-common/MetaInteractingGeometry2AABB.hpp>
#include<yade/pkg-common/InteractionGeometryMetaEngine.hpp>
#include<yade/pkg-common/InteractionPhysicsMetaEngine.hpp>
#include<yade/pkg-dem/InteractingSphere2InteractingSphere4SpheresContactGeometry.hpp>
#include<yade/pkg-common/PhysicalActionApplier.hpp>
#include<yade/pkg-common/PhysicalParametersMetaEngine.hpp>
#include<yade/pkg-common/NewtonsForceLaw.hpp>
#include<yade/pkg-common/NewtonsMomentumLaw.hpp>
#include<yade/pkg-common/LeapFrogPositionIntegrator.hpp>
#include<yade/pkg-common/LeapFrogOrientationIntegrator.hpp>
#include<yade/pkg-common/PersistentSAPCollider.hpp>
#include<yade/pkg-dem/PositionOrientationRecorder.hpp>
#include<yade/pkg-dem/GlobalStiffnessTimeStepper.hpp>
#include<yade/pkg-common/PhysicalActionDamper.hpp>
#include<yade/pkg-common/CundallNonViscousDamping.hpp>
#include<yade/pkg-common/CundallNonViscousDamping.hpp>



void USCTGen::createEngines(){
	rootBody->initializers.clear();

	shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
		boundingVolumeDispatcher->add(new InteractingSphere2AABB);
		boundingVolumeDispatcher->add(new MetaInteractingGeometry2AABB);
		rootBody->initializers.push_back(boundingVolumeDispatcher);

	rootBody->engines.clear();

	rootBody->engines.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));
	rootBody->engines.push_back(boundingVolumeDispatcher);

	shared_ptr<PersistentSAPCollider> collider(new PersistentSAPCollider);
		collider->haveDistantTransient=true;
		rootBody->engines.push_back(collider);

	shared_ptr<InteractionGeometryMetaEngine> igeomDispatcher(new InteractionGeometryMetaEngine);
		igeomDispatcher->add(new InteractingSphere2InteractingSphere4SpheresContactGeometry);
		rootBody->engines.push_back(igeomDispatcher);

	shared_ptr<InteractionPhysicsMetaEngine> iphysDispatcher(new InteractionPhysicsMetaEngine);
		shared_ptr<BrefcomMakeContact> bmc(new BrefcomMakeContact);
		bmc->cohesiveThresholdIter=cohesiveThresholdIter;
		bmc->cohesiveThresholdIter=-1; bmc->G_over_E=1; bmc->expBending=1; bmc->xiShear=.8; bmc->sigmaT=3e9; bmc->neverDamage=true; bmc->epsCrackOnset=1e-4; bmc->relDuctility=5; bmc->transStrainCoeff=.5;
		iphysDispatcher->add(bmc);
	rootBody->engines.push_back(iphysDispatcher);

	shared_ptr<BrefcomLaw> bLaw(new BrefcomLaw);
	rootBody->engines.push_back(bLaw);

	shared_ptr<PhysicalActionDamper> dampingDispatcher(new PhysicalActionDamper);
		shared_ptr<CundallNonViscousForceDamping> forceDamper(new CundallNonViscousForceDamping);
		forceDamper->damping = damping;
		dampingDispatcher->add(forceDamper); //"Force","ParticleParameters","CundallNonViscousForceDamping",actionForceDamping);
		shared_ptr<CundallNonViscousMomentumDamping> momentumDamper(new CundallNonViscousMomentumDamping);
		momentumDamper->damping = damping;
		dampingDispatcher->add(momentumDamper); // "Momentum","RigidBodyParameters","CundallNonViscousMomentumDamping",actionMomentumDamping);
		rootBody->engines.push_back(dampingDispatcher);



	shared_ptr<PhysicalActionApplier> applyActionDispatcher(new PhysicalActionApplier);
		applyActionDispatcher->add(new NewtonsForceLaw);
		applyActionDispatcher->add(new NewtonsMomentumLaw);
		rootBody->engines.push_back(applyActionDispatcher);

	shared_ptr<PhysicalParametersMetaEngine> positionIntegrator(new PhysicalParametersMetaEngine);
		positionIntegrator->add(new LeapFrogPositionIntegrator); //DISPATCHER_ADD2(ParticleParameters,LeapFrogPositionIntegrator);
		rootBody->engines.push_back(positionIntegrator);

	shared_ptr<PhysicalParametersMetaEngine> orientationIntegrator(new PhysicalParametersMetaEngine);
		orientationIntegrator->add(new LeapFrogOrientationIntegrator);
		rootBody->engines.push_back(orientationIntegrator);

	rootBody->engines.push_back(shared_ptr<BrefcomDamageColorizer>(new BrefcomDamageColorizer));

}



