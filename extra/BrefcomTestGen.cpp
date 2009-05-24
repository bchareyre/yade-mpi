// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#include"BrefcomTestGen.hpp"
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-dem/BodyMacroParameters.hpp>

YADE_PLUGIN("BrefcomTestGen");




#include<yade/pkg-common/PhysicalActionContainerReseter.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/BoundingVolumeMetaEngine.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-common/InteractingSphere2AABB.hpp>
#include<yade/pkg-common/MetaInteractingGeometry.hpp>
#include<yade/pkg-common/MetaInteractingGeometry2AABB.hpp>
#include<yade/pkg-common/InteractionGeometryMetaEngine.hpp>
#include<yade/pkg-common/InteractionPhysicsMetaEngine.hpp>
#include<yade/pkg-common/PhysicalActionApplier.hpp>
#include<yade/pkg-common/PhysicalParametersMetaEngine.hpp>
#include<yade/pkg-common/NewtonsForceLaw.hpp>
#include<yade/pkg-common/NewtonsMomentumLaw.hpp>
#include<yade/pkg-common/LeapFrogPositionIntegrator.hpp>
#include<yade/pkg-common/LeapFrogOrientationIntegrator.hpp>
#include<yade/pkg-common/PersistentSAPCollider.hpp>
#include<yade/pkg-common/ConstitutiveLawDispatcher.hpp>
#include<yade/pkg-dem/PositionOrientationRecorder.hpp>
#include<yade/pkg-dem/GlobalStiffnessTimeStepper.hpp>
#include<yade/pkg-dem/Dem3DofGeom_SphereSphere.hpp>
#include<yade/extra/UniaxialStrainControlledTest.hpp>


CREATE_LOGGER(BrefcomTestGen);

void BrefcomTestGen::createEngines(){
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
	shared_ptr<ef2_Sphere_Sphere_Dem3DofGeom> ef2ssd3d(new ef2_Sphere_Sphere_Dem3DofGeom);
	igeomDispatcher->add(ef2ssd3d);
	rootBody->engines.push_back(igeomDispatcher);

	shared_ptr<InteractionPhysicsMetaEngine> iphysDispatcher(new InteractionPhysicsMetaEngine);
		shared_ptr<Ip2_CpmMat_CpmMat_CpmPhys> bmc(new Ip2_CpmMat_CpmMat_CpmPhys);
		bmc->cohesiveThresholdIter=-1; bmc->G_over_E=1; bmc->sigmaT=3e9; bmc->neverDamage=true; bmc->epsCrackOnset=1e-4; bmc->relDuctility=5;
		//bmc->calibratedEpsFracture=.5; /* arbitrary, but large enough */
		iphysDispatcher->add(bmc);
	rootBody->engines.push_back(iphysDispatcher);

	shared_ptr<ConstitutiveLawDispatcher> clDisp(new ConstitutiveLawDispatcher);
		clDisp->add(shared_ptr<ConstitutiveLaw>(new Law2_Dem3DofGeom_CpmPhys_Cpm));
	rootBody->engines.push_back(clDisp);

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

	shared_ptr<CpmPhysDamageColorizer> dmg(new CpmPhysDamageColorizer);
	rootBody->engines.push_back(dmg);

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
