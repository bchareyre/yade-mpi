// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#include"UniaxialStrainControlledTest.hpp"
#include<yade/pkg-common/Force.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-dem/SDECLinkGeometry.hpp>
#include<yade/pkg-dem/SDECLinkPhysics.hpp>
#include<yade/extra/Brefcom.hpp>
#include<boost/foreach.hpp>

#include<yade/core/InteractionContainer.hpp>

YADE_PLUGIN("USCTGen","UniaxialStrainer");


/************************ UniaxialStrainer **********************/
CREATE_LOGGER(UniaxialStrainer);

void UniaxialStrainer::init(){
	needsInit=false;

	assert(posIds.size()>0);
	assert(negIds.size()>0);
	posCoords.clear(); negCoords.clear();
	BOOST_FOREACH(body_id_t id,posIds){ const shared_ptr<Body>& b=Body::byId(id); posCoords.push_back(b->physicalParameters->se3.position[axis]); b->isDynamic=false;}
	BOOST_FOREACH(body_id_t id,negIds){ const shared_ptr<Body>& b=Body::byId(id); negCoords.push_back(b->physicalParameters->se3.position[axis]); b->isDynamic=false;}
	assert(posIds.size()==posCoords.size() && negIds.size()==negCoords.size());

	originalLength=axisCoord(posIds[0])-axisCoord(negIds[0]);
	LOG_DEBUG("Reference particles: positive #"<<posIds[0]<<" at "<<axisCoord(posIds[0])<<"; negative #"<<negIds[0]<<" at "<<axisCoord(negIds[0]));
	LOG_INFO("Setting initial length to "<<originalLength);
	if(originalLength<=0) LOG_FATAL("Initial length is negative or zero (swapped reference particles?)! "<<originalLength);
	assert(originalLength>0);
	
	shared_ptr<AABB> rbAABB;
	if (Omega::instance().getRootBody()->boundingVolume && (rbAABB=dynamic_pointer_cast<AABB>(Omega::instance().getRootBody()->boundingVolume))){
		int axis2=(axis+1)%3, axis3=(axis+2)%3; // perpendicular axes indices
		crossSectionArea=4*rbAABB->halfSize[axis2]*rbAABB->halfSize[axis3];
		LOG_INFO("Setting crossSectionArea="<<crossSectionArea<<", using axes #"<<axis2<<" and #"<<axis3<<".");
	} else {
		crossSectionArea=1.;
		LOG_WARN("No Axis Aligned Bounding Box for rootBody, using garbage value ("<<crossSectionArea<<") for crossSectionArea!");
	}
	assert(crossSectionArea>0);

	recStream.open("/tmp/usct.data");
}

void UniaxialStrainer::applyCondition(MetaBody* rootBody){
	if(needsInit) init();
	// postconditions for initParams
	assert(posIds.size()==posCoords.size() && negIds.size()==negCoords.size() && originalLength>0 && crossSectionArea>0);
	//nothing to do
	if(posIds.size()==0 || negIds.size()==0) return;
	// linearly increase strain to the desired value
	if(abs(currentStrainRate)<abs(strainRate))currentStrainRate+=strainRate*.01; else currentStrainRate=strainRate;
	// how much do we move; in the symmetric case, half of the strain is applied at each end;
	Real dAX=(asymmetry==0?.5:1)*currentStrainRate*originalLength*Omega::instance().getTimeStep();
	for(size_t i=0; i<negIds.size(); i++){
		if(asymmetry==0 || asymmetry==-1 /* for +1, don't move*/) negCoords[i]-=dAX;
		axisCoord(negIds[i])=negCoords[i]; // update current position
	}
	for(size_t i=0; i<posIds.size(); i++){
		if(asymmetry==0 || asymmetry==1 /* for -1, don't move */) posCoords[i]+=dAX;
		axisCoord(posIds[i])=posCoords[i];
	}

	Real axialLength=axisCoord(posIds[0])-axisCoord(negIds[0]);
	Real strain=axialLength/originalLength-1;
	if(Omega::instance().getCurrentIteration()%400==0) TRVAR5(dAX,axialLength,originalLength,currentStrainRate,strain);

	// reverse if we're over the limit strain
	if(notYetReversed && limitStrain!=0 && ((currentStrainRate>0 && strain>limitStrain) || (currentStrainRate<0 && strain<limitStrain))) { currentStrainRate*=-1; notYetReversed=false; LOG_INFO("Reversed strain rate to "<<currentStrainRate); }

	if(Omega::instance().getCurrentIteration()%50==0 && recStream.good()) {
		computeAxialForce(rootBody);
		Real midPos=Body::byId(1)->physicalParameters->se3.position[axis];
		Real avgStress=(sumPosForces+sumNegForces)/(2*crossSectionArea); // average nominal stress
		recStream<<Omega::instance().getCurrentIteration()<<" "<<strain<<" "<<avgStress<<" "<<sumPosForces<<" "<<sumNegForces<<" "<<posCoords[0]<<" "<<negCoords[0]<<" "<<midPos<<endl;
	}
}

void UniaxialStrainer::computeAxialForce(MetaBody* rootBody){
	#if 0
		// for testing only
		foreach(shared_ptr<Interaction> i, *rootBody->transientInteractions){ cerr<<"Testing foreach for interactions: "<<i->getId1()<<" "<<i->getId2()<<endl; }
		foreach(shared_ptr<Body> b, *rootBody->bodies){ cerr<<"Testing foreach for bodies: "<<b->getId()<<endl; }
	#endif
	sumPosForces=0; sumNegForces=0;
		shared_ptr<Force> f(new Force);
		for(size_t i=0; i<negIds.size(); i++){
			sumNegForces+=static_pointer_cast<Force>(rootBody->physicalActions->find(negIds[i],f->getClassIndex()))->force[axis];
		}
		for(size_t i=0; i<posIds.size(); i++){
			sumPosForces-=static_pointer_cast<Force>(rootBody->physicalActions->find(posIds[i],f->getClassIndex()))->force[axis];
		}
	//TRVAR2(sumPosForces,sumNegForces);
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

#if 0
	/* clump spheres together if requested */
	if(clumped){
		shared_ptr<Clump> clump=shared_ptr<Clump>(new Clump());
		shared_ptr<Body> clumpAsBody(static_pointer_cast<Body>(clump));
	}
#endif
	return true;
}

#include<yade/extra/Brefcom.hpp>

#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/pkg-common/Force.hpp>
#include<yade/pkg-common/Momentum.hpp>
#include<yade/pkg-common/InteractionPhysicsEngineUnit.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-common/PhysicalActionContainerReseter.hpp>
#include<yade/pkg-common/PhysicalActionContainerInitializer.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/BoundingVolumeMetaEngine.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-common/InteractingSphere2AABB.hpp>
#include<yade/pkg-common/MetaInteractingGeometry.hpp>
#include<yade/pkg-common/MetaInteractingGeometry2AABB.hpp>
#include<yade/pkg-common/InteractionGeometryMetaEngine.hpp>
#include<yade/pkg-common/InteractionPhysicsMetaEngine.hpp>
#include<yade/pkg-dem/InteractingSphere2InteractingSphere4DistantSpheresContactGeometry.hpp>
#include<yade/pkg-common/PhysicalActionApplier.hpp>
#include<yade/pkg-common/PhysicalParametersMetaEngine.hpp>
#include<yade/pkg-common/NewtonsForceLaw.hpp>
#include<yade/pkg-common/NewtonsMomentumLaw.hpp>
#include<yade/pkg-common/LeapFrogPositionIntegrator.hpp>
#include<yade/pkg-common/LeapFrogOrientationIntegrator.hpp>
#include<yade/pkg-common/PersistentSAPCollider.hpp>
#include<yade/pkg-dem/GlobalStiffnessCounter.hpp>
#include<yade/pkg-dem/PositionOrientationRecorder.hpp>
#include<yade/pkg-dem/GlobalStiffnessTimeStepper.hpp>
#include<yade/pkg-common/PhysicalActionDamper.hpp>
#include<yade/pkg-common/CundallNonViscousForceDamping.hpp>
#include<yade/pkg-common/CundallNonViscousMomentumDamping.hpp>



void USCTGen::createEngines(){
	rootBody->initializers.clear();

	shared_ptr<PhysicalActionContainerInitializer> physicalActionInitializer(new PhysicalActionContainerInitializer);
	physicalActionInitializer->physicalActionNames.push_back("Force");
	physicalActionInitializer->physicalActionNames.push_back("Momentum");
	physicalActionInitializer->physicalActionNames.push_back("GlobalStiffness");
	rootBody->initializers.push_back(physicalActionInitializer);
	
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
		igeomDispatcher->add(new InteractingSphere2InteractingSphere4DistantSpheresContactGeometry);
		rootBody->engines.push_back(igeomDispatcher);

	shared_ptr<InteractionPhysicsMetaEngine> iphysDispatcher(new InteractionPhysicsMetaEngine);
		shared_ptr<BrefcomMakeContact> bmc(new BrefcomMakeContact);
		bmc->cohesiveThresholdIter=cohesiveThresholdIter;
		bmc->expBending=20;
		//bmc->calibratedEpsFracture=3e-4;
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
#if 0
	shared_ptr<GlobalStiffnessCounter> globalStiffnessCounter(new GlobalStiffnessCounter);
	globalStiffnessCounter->sdecGroupMask=1023;
	globalStiffnessCounter->interval=100;
	globalStiffnessCounter->assumeElasticSpheres=false;

	shared_ptr<GlobalStiffnessTimeStepper> globalStiffnessTimeStepper(new GlobalStiffnessTimeStepper);
	globalStiffnessTimeStepper->sdecGroupMask=1023; // BIN 111111111, should always match
	globalStiffnessTimeStepper->timeStepUpdateInterval=100;
	globalStiffnessTimeStepper->defaultDt=1e-6;
	rootBody->engines.push_back(globalStiffnessTimeStepper);

	rootBody->engines.push_back(globalStiffnessCounter);
#endif

	rootBody->engines.push_back(shared_ptr<BrefcomDamageColorizer>(new BrefcomDamageColorizer));

	shared_ptr<PositionOrientationRecorder> por(new PositionOrientationRecorder);
	por->outputFile="/tmp/usct-traction";
	por->interval=300;
	por->saveRgb=true;
	rootBody->engines.push_back(por);

}


