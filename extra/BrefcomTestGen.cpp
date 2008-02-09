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
	rootBody->initializers.push_back(physicalActionInitializer);
	
	shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
	boundingVolumeDispatcher->DISPATCHER_ADD3(InteractingSphere,AABB,InteractingSphere2AABB);
	boundingVolumeDispatcher->DISPATCHER_ADD3(MetaInteractingGeometry,AABB,MetaInteractingGeometry2AABB);
	rootBody->initializers.push_back(boundingVolumeDispatcher);

	rootBody->engines.clear();

	rootBody->engines.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));
	rootBody->engines.push_back(boundingVolumeDispatcher);

	shared_ptr<PersistentSAPCollider> collider(new PersistentSAPCollider);
	collider->haveDistantTransient=true;
	rootBody->engines.push_back(collider);

	shared_ptr<InteractionGeometryMetaEngine> igeomDispatcher(new InteractionGeometryMetaEngine);
	igeomDispatcher->DISPATCHER_ADD3(InteractingSphere,InteractingSphere,InteractingSphere2InteractingSphere4DistantSpheresContactGeometry);
	rootBody->engines.push_back(igeomDispatcher);

	shared_ptr<InteractionPhysicsMetaEngine> iphysDispatcher(new InteractionPhysicsMetaEngine);
	iphysDispatcher->DISPATCHER_ADD3(BodyMacroParameters,BodyMacroParameters,BrefcomMakeContact);
	rootBody->engines.push_back(iphysDispatcher);

	shared_ptr<BrefcomLaw> bLaw(new BrefcomLaw);
	rootBody->engines.push_back(bLaw);

	shared_ptr<PhysicalActionApplier> applyActionDispatcher(new PhysicalActionApplier);
	applyActionDispatcher->DISPATCHER_ADD3(Force,ParticleParameters,NewtonsForceLaw);
	applyActionDispatcher->DISPATCHER_ADD3(Momentum,RigidBodyParameters,NewtonsMomentumLaw);
	rootBody->engines.push_back(applyActionDispatcher);
	
	shared_ptr<PhysicalParametersMetaEngine> positionIntegrator(new PhysicalParametersMetaEngine);
	positionIntegrator->DISPATCHER_ADD2(ParticleParameters,LeapFrogPositionIntegrator);
	rootBody->engines.push_back(positionIntegrator);

	shared_ptr<PhysicalParametersMetaEngine> orientationIntegrator(new PhysicalParametersMetaEngine);
	orientationIntegrator->DISPATCHER_ADD2(RigidBodyParameters,LeapFrogOrientationIntegrator);
	rootBody->engines.push_back(orientationIntegrator);
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
	Real zCoord=1, yCoord=0;
	shared_ptr<Body>
		s1=Shop::sphere(Vector3r(0,-yCoord,-zCoord),1),
		s2=Shop::sphere(Vector3r(0,yCoord,zCoord),1),
		sMid=Shop::sphere(Vector3r(0,0,0.01),1);
	body_id_t id1=rootBody->bodies->insert(s1), id2=rootBody->bodies->insert(s2), id3=rootBody->bodies->insert(sMid);
	
	//  engines should take care of the rest of interaction; this is what collider would do normally
	/*
	rootBody->transientInteractions->insert(id1,id2);
	rootBody->transientInteractions->find(id1,id2)->isReal=1;
	rootBody->transientInteractions->find(id1,id2)->isNew=1;
	*/

	strainer->negIds.push_back(id1); strainer->negCoords.push_back(-1);
	strainer->posIds.push_back(id2); strainer->posCoords.push_back(1);

	return true;
}
