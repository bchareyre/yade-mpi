// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#include"UniaxialStrainControlledTest.hpp"
#include<yade/pkg-common/Force.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/Box.hpp>
#include<yade/pkg-common/InteractingBox.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-dem/SDECLinkGeometry.hpp>
#include<yade/pkg-dem/SDECLinkPhysics.hpp>
#include<yade/extra/Brefcom.hpp>
#include<boost/foreach.hpp>

#include<yade/core/InteractionContainer.hpp>

YADE_PLUGIN("USCTGen","UniaxialStrainer","UniaxialStrainSensorPusher");

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
	LOG_INFO("Setting initial length to "<<originalLength);
	if(originalLength<=0) LOG_FATAL("Initial length is negative or zero (swapped reference particles?)! "<<originalLength);
	/* this happens is nan propagates from e.g. brefcom consitutive law in case 2 bodies have _exactly_ the same position
	 * (the the normal strain is 0./0.=nan). That is an user's error, however and should not happen. */
	if(isnan(originalLength)) LOG_FATAL("Initial length is NaN!");
	assert(originalLength>0 && !isnan(originalLength));

	/* if we have default (<0) crossSectionArea, try to get it from root's AABB;
	 * this will not work if there are foreign bodies in the simulation (like transStrainSensors),
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

	setupTransStrainSensors();

	prepareRecStream();
}

/* Initialize UniaxialStrainSensorPusher so that subscribed bodies and forces are consistent.
 * Apply small forces on those bodies, cap their velocity and reset orientation.
 * Return vector of widths in the direction of sensor pairs.
 */
void UniaxialStrainer::pushTransStrainSensors(MetaBody* rb, vector<Real>& widths){
	if(transStrainSensors.size()==0) return;
	if(!sensorsPusher){
		int count=0;
		FOREACH(const shared_ptr<Engine>& e,rb->engines){
			if(e->getClassName()=="UniaxialStrainSensorPusher"){ count++; sensorsPusher=static_pointer_cast<UniaxialStrainSensorPusher>(e); }
		}
		if(count>1) LOG_ERROR("Multiple UniaxialStrainSensorPusher's found, using the last one for transversal strain sensors!");
		if(count<1) { LOG_ERROR("No UniaxialStrainSensorPusher found, transversal strain sensors will not work!"); return; }
		sensorsPusher->subscribedBodies.clear();
		FOREACH(body_id_t id, transStrainSensors) sensorsPusher->subscribedBodies.push_back(id);
		sensorsPusher->forces.resize(transStrainSensors.size());
		//TRVAR3(transStrainSensors.size(),sensorsPusher->subscribedBodies.size(),sensorsPusher->forces.size());
	}
	assert((sensorsPusher->subscribedBodies.size()==transStrainSensors.size()) && (sensorsPusher->subscribedBodies.size()==sensorsPusher->forces.size()));
	Real forceMagnitude=.001*abs(avgStress)*transStrainSensorArea;
	Real maxVelocity=2*abs(strainRate)*originalLength; // move at max 5 × faster than strained ends
	/* reset orientation to identity and limit velocity */
	FOREACH(body_id_t id, transStrainSensors){
		const shared_ptr<Body>& b=Body::byId(id); const shared_ptr<ParticleParameters>& pp=YADE_PTR_CAST<ParticleParameters>(b->physicalParameters);
		pp->se3.orientation=Quaternionr::IDENTITY;
		if(pp->velocity.SquaredLength()>pow(maxVelocity,2)){ pp->velocity.Normalize(); pp->velocity*=maxVelocity; }
	}
	/* calcuate and store force that will be applied in UniaxialStrainSensorPusher */
	widths.clear();
	for(int i=1; i<=(transStrainSensors.size()==2?1:2); i++){
		int transAxis=(axis+i)%3, perpTransAxis=(i==1?(axis+2)%3:/* i==2 */ (axis+1)%3);
		Vector3r F; F[axis]=0; F[perpTransAxis]=0; F[transAxis]=forceMagnitude;
		body_id_t n1=2*(i-1), n2=2*(i-1)+1;
		sensorsPusher->forces[n1]=+F; sensorsPusher->forces[n2]=-F;
		const shared_ptr<Body>& lo=Body::byId(transStrainSensors[n1]), hi=Body::byId(transStrainSensors[n2]);
		Real wd=hi->physicalParameters->se3.position[transAxis]-lo->physicalParameters->se3.position[transAxis]-static_pointer_cast<Box>(hi->geometricalModel)->extents[transAxis]-static_pointer_cast<Box>(lo->geometricalModel)->extents[transAxis];
		// negative width? Apply no more force, reset velocity to 0
		if(wd<=0) {
			/* doesn't work... Why? */
			//LOG_ERROR("Width is negative, resetting forces and velocities");
			sensorsPusher->forces[n1]=sensorsPusher->forces[n2]=Vector3r::ZERO;
			YADE_PTR_CAST<ParticleParameters>(lo->physicalParameters)->velocity=Vector3r::ZERO;
			YADE_PTR_CAST<ParticleParameters>(hi->physicalParameters)->velocity=Vector3r::ZERO;
			wd=0;
		}
		widths.push_back(wd);
	}
}

void UniaxialStrainer::setupTransStrainSensors(){
	assert(transStrainSensors.size()==0 || transStrainSensors.size()==2 || transStrainSensors.size()==4);
	if(transStrainSensors.size()==0) return;
	assert(Omega::instance().getRootBody()->boundingVolume);
	shared_ptr<AABB> rbAABB=dynamic_pointer_cast<AABB>(Omega::instance().getRootBody()->boundingVolume);
	assert(rbAABB);
	TRVAR2(rbAABB->center,rbAABB->halfSize);
	transStrainSensorArea=0;
	int numCouples=(transStrainSensors.size()==2?1:2);
	originalWidths.clear();
	for(int i=1; i<=numCouples; i++){
		int transAxis=(axis+i)%3, perpTransAxis=(i==1?(axis+2)%3:/* i==2 */ (axis+1)%3);
		TRVAR3(axis,transAxis,perpTransAxis);
		originalWidths.push_back(2*rbAABB->halfSize[transAxis]);
		body_id_t sensId[]={transStrainSensors[2*(i-1)],transStrainSensors[2*(i-1)+1]};
		// do the same on either side, only positioning will be different
		FOREACH(body_id_t id, sensId){
			shared_ptr<Body> b=Body::byId(id);
			shared_ptr<RigidBodyParameters> rbp=dynamic_pointer_cast<RigidBodyParameters>(b->physicalParameters);
			shared_ptr<Box> box=dynamic_pointer_cast<Box>(b->geometricalModel);
			shared_ptr<InteractingBox> iBox=dynamic_pointer_cast<InteractingBox>(b->interactingGeometry);
			assert(rbp && box && iBox);
			LOG_DEBUG("Setting up transversal strain sensor, #"<<id);
			// change box size: axis,transAxis: length_specimen_along_axis)/10; perpTransAxis: width_specimen*1.5
			Vector3r ext;
			ext[axis]=ext[transAxis]=.1*rbAABB->halfSize[axis]; ext[perpTransAxis]=1.5*rbAABB->halfSize[perpTransAxis];
			box->extents=iBox->extents=ext;
			// reset orientation
			rbp->se3.orientation=Quaternionr::IDENTITY;
			// set isDynamic==True, GeometricalMode::wire=true;
			b->isDynamic=true; box->wire=true;
			// set position so that it touches AABB of rootBody
			int sign=(id==sensId[0]?-1:1); // first id goes underneath (pushed in the dir of +transAxis), the other one goes up
			rbp->se3.position[axis]=rbAABB->center[axis];
			rbp->se3.position[perpTransAxis]=rbAABB->center[perpTransAxis];
			rbp->se3.position[transAxis]=rbAABB->center[transAxis]+sign*rbAABB->halfSize[transAxis]+sign*iBox->extents[transAxis];
			TRVAR2(box->extents,rbp->se3.position);
			transStrainSensorArea+=(box->extents[axis]*rbAABB->halfSize[perpTransAxis]);
		}
	}
	transStrainSensorArea/=transStrainSensors.size();
}

void UniaxialStrainer::applyCondition(MetaBody* rootBody){
	if(needsInit) init();
	// postconditions for initParams
	assert(posIds.size()==posCoords.size() && negIds.size()==negCoords.size() && originalLength>0 && crossSectionArea>0);
	//nothing to do
	if(posIds.size()==0 || negIds.size()==0) return;
	// linearly increase strain to the desired value
	if(abs(currentStrainRate)<abs(strainRate))currentStrainRate+=strainRate*.005; else currentStrainRate=strainRate;
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
	strain=axialLength/originalLength-1;
	if(Omega::instance().getCurrentIteration()%400==0) TRVAR5(dAX,axialLength,originalLength,currentStrainRate,strain);

	// reverse if we're over the limit strain
	if(notYetReversed && limitStrain!=0 && ((currentStrainRate>0 && strain>limitStrain) || (currentStrainRate<0 && strain<limitStrain))) { currentStrainRate*=-1; notYetReversed=false; LOG_INFO("Reversed strain rate to "<<currentStrainRate); }

	if(Omega::instance().getCurrentIteration()%10==0 ) {
		computeAxialForce(rootBody);
		vector<Real> widths;
		pushTransStrainSensors(rootBody,widths);
		assert(widths.size()==originalWidths.size());
		for(size_t i=0; i<widths.size(); i++) avgTransStrain+=(widths[i]/originalWidths[i]-1); avgTransStrain/=widths.size();
		avgStress=(sumPosForces+sumNegForces)/(2*crossSectionArea); // average nominal stress
		if(recStream.good()) recStream<<Omega::instance().getCurrentIteration()<<" "<<strain<<" "<<avgStress<<" "<<avgTransStrain<<endl;
	}
}

void UniaxialStrainer::computeAxialForce(MetaBody* rootBody){
	sumPosForces=sumNegForces=0;
	FOREACH(body_id_t id, negIds) sumNegForces+=Shop::Bex::force(id)[axis];
	FOREACH(body_id_t id, posIds) sumPosForces-=Shop::Bex::force(id)[axis];
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
#include<yade/pkg-common/CundallNonViscousDamping.hpp>
#include<yade/pkg-common/CundallNonViscousDamping.hpp>



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
		bmc->cohesiveThresholdIter=-1; bmc->G_over_E=1; bmc->expBending=1; bmc->xiShear=.8; bmc->sigmaT=3e9; bmc->neverDamage=true; bmc->epsCrackOnset=1e-4; bmc->relDuctility=5;
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


