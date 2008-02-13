// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#include"UniaxialStrainControlledTest.hpp"
#include<yade/pkg-common/Force.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-dem/SDECLinkGeometry.hpp>
#include<yade/pkg-dem/SDECLinkPhysics.hpp>
#include<yade/extra/Brefcom.hpp>
YADE_PLUGIN("USCTGen","UniaxialStrainer");


/************************ UniaxialStrainer **********************/
CREATE_LOGGER(UniaxialStrainer);

void UniaxialStrainer::applyCondition(Body* _rootBody){
	if(posIds.size()==0 || negIds.size()==0) return;
	assert(posIds.size()==posCoords.size() && negIds.size()==negCoords.size());

	if(originalLength<0) {
		originalLength=USCT_AXIS_COORD(posIds[0])-USCT_AXIS_COORD(negIds[0]);
		LOG_DEBUG("Reference particles: positive #"<<posIds[0]<<" at "<<USCT_AXIS_COORD(posIds[0])<<
			"; negative #"<<negIds[0]<<" at "<<USCT_AXIS_COORD(negIds[0]));
		LOG_INFO("Setting initial length to "<<originalLength);
	}
	
	shared_ptr<AABB> rbAABB;
	if(crossSectionArea<=0){
		if (_rootBody->boundingVolume && (rbAABB=dynamic_pointer_cast<AABB>(_rootBody->boundingVolume))){
			int axis2=(axis+1)%3, axis3=(axis+2)%3; // perpendicular axes indices
			crossSectionArea=4*rbAABB->halfSize[axis2]*rbAABB->halfSize[axis3];
			LOG_INFO("Setting crossSectionArea="<<crossSectionArea<<", using axes #"<<axis2<<" and #"<<axis3<<".");
		} else {
			LOG_WARN("No Axis Aligned Bounding Box for rootBody, using garbage value ("<<crossSectionArea<<") for crossSectionArea!");
		}
	}

	// linearly increase strain to the desired value
	if(abs(currentStrainRate)<abs(strainRate))currentStrainRate+=strainRate*.01; else currentStrainRate=strainRate;

	Real dAX=.5*currentStrainRate*originalLength*Omega::instance().getTimeStep();
	for(size_t i=0; i<negIds.size(); i++){
		//TRVAR1(USCT_AXIS_COORD(negIds[i]));
		negCoords[i]-=dAX;
		USCT_AXIS_COORD(negIds[i])=negCoords[i]; // update current position
		negCoords[i]-=dAX; //USCT_AXIS_COORD(negIds[i]); // store current position
		//if(strain<-0.000155 && i==0) LOG_DEBUG("Moved #"<<negIds[i]<<" by "<<-dAX<<" to "<<negCoords[i]);
	}
	for(size_t i=0; i<posIds.size(); i++){
		posCoords[i]+=dAX;
		USCT_AXIS_COORD(posIds[i])=posCoords[i];
		//if(strain<-0.000155 && i==0) LOG_DEBUG("Moved #"<<posIds[i]<<" by "<<dAX<<" to "<<posCoords[i]);
	}

	Real axialLength=USCT_AXIS_COORD(posIds[0])-USCT_AXIS_COORD(negIds[0]);
	Real strain=axialLength/originalLength-1;
	if(Omega::instance().getCurrentIteration()%400==0) TRVAR5(dAX,axialLength,originalLength,currentStrainRate,strain);
	// reverse if we're over the limit strain
	// if(notYetReversed && limitStrain!=0 && ((currentStrainRate>0 && strain>limitStrain) || (currentStrainRate<0 && strain<limitStrain))) { currentStrainRate*=-1; notYetReversed=false; LOG_INFO("Reversed strain rate to "<<currentStrainRate); }
	MetaBody* rootBody=static_cast<MetaBody*>(_rootBody);
	if(Omega::instance().getCurrentIteration()%50==0 && recStream.good()) {
		computeAxialForce(rootBody);
		Real midPos=Body::byId(1)->physicalParameters->se3.position[axis];
		Real avgStress=(sumPosForces+sumNegForces)/(2*crossSectionArea); // average nominal stress
		recStream<<Omega::instance().getCurrentIteration()<<" "<<strain<<" "<<avgStress<<" "<<sumPosForces<<" "<<sumNegForces<<" "<<posCoords[0]<<" "<<negCoords[0]<<" "<<midPos<<endl;
	}
}

bool UniaxialStrainer::idInVector(body_id_t id, const vector<body_id_t>& V){
	for(size_t i=0; i<V.size(); i++){ if(V[i]==id) return true; }
	return false;
}

void UniaxialStrainer::computeAxialForce(MetaBody* rootBody){
	sumPosForces=0; sumNegForces=0;
	#if 0
		for(InteractionContainer::iterator I=rootBody->transientInteractions->begin(); I!=rootBody->transientInteractions->end(); ++I){
			if(!(*I)->isReal) { continue; }
			const shared_ptr<BrefcomContact>& BC=dynamic_pointer_cast<BrefcomContact>((*I)->interactionPhysics);
			//const shared_ptr<SpheresContactGeometry>& SCG=dynamic_pointer_cast<SpheresContactGeometry>((*I)->interactionGeometry);
			if(/* (!SCG) || */ (!BC) || (!BC->isStructural)) { continue; }
			body_id_t id1=(*I)->getId1(), id2=(*I)->getId2(), id;

			/* following: if (id1 || id2) ∈ (posIds||negIds): add axis.Dot(contact->Fs+contact->Fn) to (sumPosForces||sumNegForces) */
			bool pos1=idInVector(id1,posIds), pos2=idInVector(id2,posIds), neg1=idInVector(id1,negIds), neg2=idInVector(id2,negIds);
			if(!(pos1&&pos2)) id = pos1 ? id1 : (pos2 ? id2 : Body::ID_NONE);
			if(id!=Body::ID_NONE){
				Vector3r ax=Vector3r::ZERO; ax[axis]=-1;
				sumPosForces+=(id==id1?1:-1)*BC->Fn.Dot(ax); // sumPosForces+=ax.Dot(BC->Fs+BC->Fn); /* TRVAR1(ax.Dot(BC->Fs+BC->Fn)); */
			}
			if(!(neg1&&neg2)) id = neg1 ? id1 : (neg2 ? id2 : Body::ID_NONE);
			if(id!=Body::ID_NONE){
				Vector3r ax=Vector3r::ZERO; ax[axis]=1;
				sumNegForces+=(id==id1?1:-1)*BC->Fn.Dot(ax); //sumNegForces+=ax.Dot(BC->Fs+BC->Fn); /* TRVAR1(ax.Dot(BC->Fs+BC->Fn)); */
			}
		}
	#else
		shared_ptr<Force> f(new Force);
		for(size_t i=0; i<negIds.size(); i++){
			sumNegForces+=static_pointer_cast<Force>(rootBody->physicalActions->find(negIds[i],f->getClassIndex()))->force[axis];
		}
		for(size_t i=0; i<posIds.size(); i++){
			sumPosForces-=static_pointer_cast<Force>(rootBody->physicalActions->find(posIds[i],f->getClassIndex()))->force[axis];
		}
	#endif
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
	Shop::setDefault("phys_young",30e7);

	vecVecReal spheres=Shop::loadSpheresFromFile(spheresFile,minXYZ,maxXYZ);
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
	// create links between spheres
	Real distFactor=1.2;
	for(BodyContainer::iterator I1=rootBody->bodies->begin(); I1!=rootBody->bodies->end(); ++I1){
		for(BodyContainer::iterator I2=rootBody->bodies->begin(); I2!=rootBody->bodies->end(); ++I2){
			Vector3r C1=(*I1)->physicalParameters->se3.position, C2=(*I2)->physicalParameters->se3.position;
			const shared_ptr<InteractingSphere>& is1=dynamic_pointer_cast<InteractingSphere>((*I1)->interactingGeometry), is2=dynamic_pointer_cast<InteractingSphere>((*I2)->interactingGeometry);
			assert(is1 && is2);
			Real r1=is1->radius, r2=is2->radius;
			if((C2-C1).Length()<(r1+r2)*distFactor){
					shared_ptr<Interaction> link(new Interaction((*I1)->getId(),(*I2)->getId()));
					shared_ptr<SDECLinkGeometry> geom(new SDECLinkGeometry);
					shared_ptr<SDECLinkPhysics> phys(new SDECLinkPhysics);
					geom->radius1=r1-.5*abs(r1-r2); geom->radius2=r2-.5*abs(r1-r2);
					link->interactionGeometry=geom;

					phys->initialKn=50000000; phys->knMax=550000000;
					phys->initialKs=5000000; phys->ksMax=550000000;
					phys->heta=1;
					phys->initialEquilibriumDistance=(C1-C2).Length();
					link->interactionPhysics=phys;

					link->isReal=true; link->isNew=false;
					rootBody->persistentInteractions->insert(link);
					//LOG_DEBUG("Linked #"<<(*I1)->getId()<<" and #"<<(*I2)->getId()<<".");
			}
		}
	}
	// remove collider
	for(vector<shared_ptr<Engine> >::iterator I=rootBody->engines.begin(); I!=rootBody->engines.end(); ++I){
		if((*I)->getClassName()=="PersistentSAPCollider") {
			rootBody->engines.erase(I);
			LOG_DEBUG("Removed PersistentSAPCollider engine.");
			break;
		}
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
		iphysDispatcher->add(new BrefcomMakeContact);
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

	shared_ptr<GlobalStiffnessCounter> globalStiffnessCounter(new GlobalStiffnessCounter);
	globalStiffnessCounter->sdecGroupMask=1023;
	globalStiffnessCounter->interval=100;

	shared_ptr<GlobalStiffnessTimeStepper> globalStiffnessTimeStepper(new GlobalStiffnessTimeStepper);
	globalStiffnessTimeStepper->sdecGroupMask=1023; // BIN 111111111, should always match
	globalStiffnessTimeStepper->timeStepUpdateInterval=100;
	globalStiffnessTimeStepper->defaultDt=1e-6;
	rootBody->engines.push_back(globalStiffnessTimeStepper);


	rootBody->engines.push_back(globalStiffnessCounter);
}


