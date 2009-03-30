#include"SimpleScene.hpp"
#include<yade/extra/Shop.hpp>
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-common/InteractingSphere2AABB.hpp>
#include<yade/pkg-common/InteractingBox2AABB.hpp>
#include<yade/pkg-common/MetaInteractingGeometry.hpp>
#include<yade/pkg-common/MetaInteractingGeometry2AABB.hpp>
#include<yade/pkg-common/PhysicalActionContainerReseter.hpp>
#include<yade/pkg-common/PhysicalParametersMetaEngine.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-common/Box.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-common/InteractingBox.hpp>
#include<yade/pkg-common/NewtonsForceLaw.hpp>
#include<yade/pkg-common/NewtonsMomentumLaw.hpp>
#include<yade/pkg-common/LeapFrogPositionIntegrator.hpp>
#include<yade/pkg-common/LeapFrogOrientationIntegrator.hpp>
#include<yade/pkg-dem/InteractingSphere2InteractingSphere4SpheresContactGeometry.hpp>
#include<yade/pkg-dem/InteractingBox2InteractingSphere4SpheresContactGeometry.hpp>
#include<yade/pkg-common/PhysicalParametersMetaEngine.hpp>
#include<yade/pkg-common/InteractionGeometryMetaEngine.hpp>
#include<yade/pkg-common/InteractionPhysicsMetaEngine.hpp>
#include<yade/pkg-common/BoundingVolumeMetaEngine.hpp>
#include<yade/pkg-common/PhysicalActionDamper.hpp>
#include<yade/pkg-common/PhysicalActionApplier.hpp>
#include<yade/pkg-common/CundallNonViscousDamping.hpp>
#include<yade/pkg-common/CundallNonViscousDamping.hpp>
#include<yade/pkg-common/GravityEngines.hpp>
#include<yade/pkg-dem/ElasticContactLaw.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-dem/SimpleElasticRelationships.hpp>
#include<yade/pkg-common/PersistentSAPCollider.hpp>



YADE_PLUGIN("SimpleScene");

bool SimpleScene::generate(){
	message="";
	//@
	rootBody=Shop::rootBody();
	//@
	/* initializers */
		rootBody->initializers.clear();
		//@
		shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
			boundingVolumeDispatcher->add(new InteractingSphere2AABB);
			boundingVolumeDispatcher->add(new InteractingBox2AABB);
			boundingVolumeDispatcher->add(new MetaInteractingGeometry2AABB);
			rootBody->initializers.push_back(boundingVolumeDispatcher);
	//@
	/* engines */
		rootBody->engines.clear();
		//@
		rootBody->engines.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));
		//@
		// use boundingVolumeDispatcher that we defined above
		rootBody->engines.push_back(boundingVolumeDispatcher);
		//@
		shared_ptr<PersistentSAPCollider> collider(new PersistentSAPCollider);
			rootBody->engines.push_back(collider);
		//@
		shared_ptr<InteractionGeometryMetaEngine> igeomDispatcher(new InteractionGeometryMetaEngine);
			igeomDispatcher->add(new InteractingSphere2InteractingSphere4SpheresContactGeometry);
			igeomDispatcher->add(new InteractingBox2InteractingSphere4SpheresContactGeometry);
			rootBody->engines.push_back(igeomDispatcher);
		//@
		shared_ptr<InteractionPhysicsMetaEngine> iphysDispatcher(new InteractionPhysicsMetaEngine);
			iphysDispatcher->add(new SimpleElasticRelationships);
			rootBody->engines.push_back(iphysDispatcher);
		//@
		shared_ptr<ElasticContactLaw> ecl(new ElasticContactLaw);
			rootBody->engines.push_back(ecl);
		//@
		shared_ptr<GravityEngine> ge(new GravityEngine);
			ge->gravity=Vector3r(0,0,-9.81);
			rootBody->engines.push_back(ge);
		//@
		shared_ptr<PhysicalActionDamper> dampingDispatcher(new PhysicalActionDamper);
			shared_ptr<CundallNonViscousForceDamping> forceDamper(new CundallNonViscousForceDamping);
			forceDamper->damping=0.2;
			dampingDispatcher->add(forceDamper);
			shared_ptr<CundallNonViscousMomentumDamping> momentumDamper(new CundallNonViscousMomentumDamping);
			momentumDamper->damping=0.2;
			dampingDispatcher->add(momentumDamper);
			rootBody->engines.push_back(dampingDispatcher);
		//@
		shared_ptr<PhysicalActionApplier> applyActionDispatcher(new PhysicalActionApplier);
			applyActionDispatcher->add(new NewtonsForceLaw);
			applyActionDispatcher->add(new NewtonsMomentumLaw);
			rootBody->engines.push_back(applyActionDispatcher);
		//@
		shared_ptr<PhysicalParametersMetaEngine> positionIntegrator(new PhysicalParametersMetaEngine);
			positionIntegrator->add(new LeapFrogPositionIntegrator);
			rootBody->engines.push_back(positionIntegrator);
		//@
		shared_ptr<PhysicalParametersMetaEngine> orientationIntegrator(new PhysicalParametersMetaEngine);
			orientationIntegrator->add(new LeapFrogOrientationIntegrator);
			rootBody->engines.push_back(orientationIntegrator);
	//@
	// set default values for Shop	
	Shop::setDefault("body_sdecGroupMask",1);
	Shop::setDefault("phys_density",2400.); Shop::setDefault("phys_young",30e9); Shop::setDefault("phys_poisson",.3);
	Shop::setDefault("aabb_randomColor",false);Shop::setDefault("shape_randomColor",false); Shop::setDefault("mold_randomColor",false);
	Shop::setDefault("aabb_color",Vector3r(0,1,0)); Shop::setDefault("shape_color",Vector3r(1,0,0)); Shop::setDefault("mold_color",Vector3r(1,0,0));

	//@
	shared_ptr<Body> box=Shop::box(Vector3r(0,0,0),Vector3r(.5,.5,.5));
	box->isDynamic=false;
	rootBody->bodies->insert(box);
	
	//@
	if(false){
		Vector3r extents(.5,.5,.5);
		shared_ptr<Body> b=shared_ptr<Body>(new Body(body_id_t(0),0));
		b->isDynamic=true;
		
		// phys
		shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
		physics->mass=8*extents[0]*extents[1]*extents[2]*2400;
		physics->inertia=Vector3r(physics->mass*(4*extents[1]*extents[1]+4*extents[2]*extents[2])/12.,physics->mass*(4*extents[0]*extents[0]+4*extents[2]*extents[2])/12.,physics->mass*(4*extents[0]*extents[0]+4*extents[1]*extents[1])/12.);
		physics->se3=Se3r(Vector3r(0,0,0),Quaternionr::IDENTITY);
		physics->young=30e9;
		physics->poisson=.3;
		b->physicalParameters=physics;

		// aabb
		shared_ptr<AABB> aabb(new AABB);
		aabb->diffuseColor=Vector3r(0,1,0);
		b->boundingVolume=aabb;

		//shape
		shared_ptr<Box> shape(new Box);
		shape->extents=extents;
		shape->diffuseColor=Vector3r(1,0,0);
		b->geometricalModel=shape;

		// mold
		shared_ptr<InteractingBox> mold(new InteractingBox);
		mold->extents=extents;
		mold->diffuseColor=Vector3r(1,0,0);
		b->interactingGeometry=mold;

		rootBody->bodies->insert(b);
	}

	//@
	Shop::setDefault("shape_color",Vector3r(0,1,0)); Shop::setDefault("mold_color",Vector3r(0,1,0));
	shared_ptr<Body> sphere(Shop::sphere(Vector3r(0,0,2),1));
	rootBody->bodies->insert(sphere);
	
	//@
	rootBody->dt=.2*Shop::PWaveTimeStep(rootBody);

	//@
	return true;
}
