// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 

#include"UniaxialStrainerGen.hpp"

#include<yade/pkg-dem/ConcretePM.hpp>

#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/pkg-common/InteractionPhysicsFunctor.hpp>
#include<yade/pkg-common/PhysicalActionContainerReseter.hpp>
#include<yade/pkg-common/BoundDispatcher.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-common/InteractingSphere2AABB.hpp>
#include<yade/pkg-common/MetaInteractingGeometry.hpp>
#include<yade/pkg-common/MetaInteractingGeometry2AABB.hpp>
#include<yade/pkg-common/InteractionGeometryDispatcher.hpp>
#include<yade/pkg-common/InteractionPhysicsDispatcher.hpp>
#include<yade/pkg-common/PhysicalActionApplier.hpp>
#include<yade/pkg-common/StateMetaEngine.hpp>
#include<yade/pkg-common/InsertionSortCollider.hpp>
#include<yade/pkg-common/LawDispatcher.hpp>
#include<yade/pkg-dem/NewtonsDampedLaw.hpp>
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-dem/Dem3DofGeom_SphereSphere.hpp>

CREATE_LOGGER(UniaxialStrainerGen);

bool UniaxialStrainerGen::generate(){
	message="";
	rootBody=Shop::rootBody();
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

		// replace BodyMacroParameters by CpmMat
		shared_ptr<BodyMacroParameters> bmp=YADE_PTR_CAST<BodyMacroParameters>(S->physicalParameters);
		shared_ptr<CpmMat> bpp(new CpmMat);
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



void UniaxialStrainerGen::createEngines(){
	rootBody->initializers.clear();

	shared_ptr<BoundDispatcher> boundingVolumeDispatcher	= shared_ptr<BoundDispatcher>(new BoundDispatcher);
		boundingVolumeDispatcher->add(new InteractingSphere2AABB);
		boundingVolumeDispatcher->add(new MetaInteractingGeometry2AABB);
		rootBody->initializers.push_back(boundingVolumeDispatcher);

	rootBody->engines.clear();

	rootBody->engines.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));
	rootBody->engines.push_back(boundingVolumeDispatcher);

	shared_ptr<InsertionSortCollider> collider(new InsertionSortCollider);
		rootBody->engines.push_back(collider);

	shared_ptr<InteractionGeometryDispatcher> igeomDispatcher(new InteractionGeometryDispatcher);
		igeomDispatcher->add(shared_ptr<Ig2_Sphere_Sphere_Dem3DofGeom>(new Ig2_Sphere_Sphere_Dem3DofGeom));
		rootBody->engines.push_back(igeomDispatcher);

	shared_ptr<InteractionPhysicsDispatcher> iphysDispatcher(new InteractionPhysicsDispatcher);
		shared_ptr<Ip2_CpmMat_CpmMat_CpmPhys> bmc(new Ip2_CpmMat_CpmMat_CpmPhys);
		bmc->cohesiveThresholdIter=cohesiveThresholdIter;
		bmc->cohesiveThresholdIter=-1; bmc->G_over_E=1;bmc->sigmaT=3e9; bmc->neverDamage=true; bmc->epsCrackOnset=1e-4; bmc->relDuctility=5;
		iphysDispatcher->add(bmc);
	rootBody->engines.push_back(iphysDispatcher);

	shared_ptr<LawDispatcher> clDisp(new LawDispatcher);
		clDisp->add(shared_ptr<LawFunctor>(new Law2_Dem3DofGeom_CpmPhys_Cpm));
	rootBody->engines.push_back(clDisp);

	shared_ptr<NewtonsDampedLaw> newton(new NewtonsDampedLaw);
		newton->damping=damping;
	rootBody->engines.push_back(newton);

	rootBody->engines.push_back(shared_ptr<CpmStateUpdater>(new CpmStateUpdater));

}





YADE_REQUIRE_FEATURE(PHYSPAR);

