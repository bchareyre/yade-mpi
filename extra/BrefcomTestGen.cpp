// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#include"BrefcomTestGen.hpp"
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-dem/BodyMacroParameters.hpp>

YADE_PLUGIN("BrefcomTestGen");


/************************ BrefcomTestGen ****************************/
/*#include<yade/pkg-common/MetaInteractingGeometry2AABB.hpp>
#include<yade/pkg-common/MetaInteractingGeometry.hpp>
#include<yade/pkg-common/Box.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-common/PersistentSAPCollider.hpp>

#include<yade/pkg-common/BodyRedirectionVector.hpp>
#include<yade/pkg-common/InteractionVecSet.hpp>
#include<yade/pkg-common/PhysicalActionVectorVector.hpp>

#include<yade/pkg-common/InteractingBox.hpp>

#include<yade/pkg-common/InteractingBox2AABB.hpp>
#include<yade/pkg-common/Momentum.hpp>
#include<yade/pkg-common/Force.hpp>
#include<yade/pkg-dem/InteractingSphere2InteractingSphere4SpheresContactGeometry.hpp>
#include<yade/pkg-dem/InteractingBox2InteractingSphere4SpheresContactGeometry.hpp>*/


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
#include<yade/pkg-dem/PositionOrientationRecorder.hpp>
#include<yade/pkg-dem/GlobalStiffnessTimeStepper.hpp>
#include<yade/extra/UniaxialStrainControlledTest.hpp>

/*

#include<yade/pkg-common/PhysicalActionDamper.hpp>
#include<yade/pkg-common/CundallNonViscousForceDamping.hpp>
#include<yade/pkg-common/CundallNonViscousMomentumDamping.hpp>
#include<yade/pkg-common/GravityEngine.hpp>

#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-dem/ElasticCriterionTimeStepper.hpp>
#include<yade/pkg-dem/ElasticContactLaw.hpp>
#include<yade/pkg-dem/ElasticCohesiveLaw.hpp>

#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>
#include<yade/pkg-dem/SDECLinkGeometry.hpp>
#include<yade/pkg-dem/SDECLinkPhysics.hpp>

*/

CREATE_LOGGER(BrefcomTestGen);

void BrefcomTestGen::createEngines(){
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
		bmc->cohesiveThresholdIter=-1;
		//bmc->calibratedEpsFracture=.5; /* arbitrary, but large enough */
		iphysDispatcher->add(bmc);
	rootBody->engines.push_back(iphysDispatcher);

	shared_ptr<BrefcomLaw> bLaw(new BrefcomLaw);
	rootBody->engines.push_back(bLaw);

	shared_ptr<PhysicalActionApplier> applyActionDispatcher(new PhysicalActionApplier);
	applyActionDispatcher->add(new NewtonsForceLaw);
	applyActionDispatcher->add(new NewtonsMomentumLaw);
	rootBody->engines.push_back(applyActionDispatcher);
	
	shared_ptr<PhysicalParametersMetaEngine> positionIntegrator(new PhysicalParametersMetaEngine);
	positionIntegrator->add(new LeapFrogPositionIntegrator);
	rootBody->engines.push_back(positionIntegrator);

	shared_ptr<PhysicalParametersMetaEngine> orientationIntegrator(new PhysicalParametersMetaEngine);
	orientationIntegrator->add(new LeapFrogOrientationIntegrator);
	rootBody->engines.push_back(orientationIntegrator);

	shared_ptr<BrefcomDamageColorizer> dmg(new BrefcomDamageColorizer);
	rootBody->engines.push_back(dmg);

	shared_ptr<PositionOrientationRecorder> rec(new PositionOrientationRecorder);
	rec->outputFile="/tmp/brefcom-test";
	rec->interval=500;
	rec->saveRgb=true;
	rootBody->engines.push_back(rec);
#if 0
	shared_ptr<BrefcomStiffnessCounter> bsc(new BrefcomStiffnessCounter);
	bsc->interval=100;
	rootBody->engines.push_back(bsc);

	shared_ptr<GlobalStiffnessTimeStepper> gsts(new GlobalStiffnessTimeStepper);
	gsts->sdecGroupMask=1023;
	gsts->timeStepUpdateInterval=100;
	gsts->defaultDt=1e-4;
	rootBody->engines.push_back(gsts);
#endif
}

bool BrefcomTestGen::generate(){
	message="";
	rootBody=Shop::rootBody();

	createEngines();

	shared_ptr<UniaxialStrainer> strainer(new UniaxialStrainer);
	strainer->strainRate=strainRate;
	strainer->axis=2; // z-oriented straining
	strainer->limitStrain=-4;
	rootBody->engines.push_back(strainer);
	
	// control normal/shear ratio
	//Real zCoord=.1; Real yCoord=sqrt(1-zCoord*zCoord); // distance is always 2, with contact at origin
	Real zCoord=.9, yCoord=0;
	shared_ptr<Body>
		s1=Shop::sphere(Vector3r(0,-yCoord,-zCoord),.5),
		s2=Shop::sphere(Vector3r(0,yCoord,zCoord),.5),
		sMid=Shop::sphere(Vector3r(0,0,0.01),.5);
	body_id_t id1=rootBody->bodies->insert(s1), id2=rootBody->bodies->insert(s2); //, id3=rootBody->bodies->insert(sMid);
	
	//  engines should take care of the rest of interaction; this is what collider would do normally
	/*
	rootBody->transientInteractions->insert(id1,id2);
	rootBody->transientInteractions->find(id1,id2)->isReal=1;
	rootBody->transientInteractions->find(id1,id2)->isNew=1;
	*/

	strainer->negIds.push_back(id1); strainer->negCoords.push_back(-zCoord);
	strainer->posIds.push_back(id2); strainer->posCoords.push_back(zCoord);

	return true;
}
