// 2007 © Václav Šmilauer <eudoxos@arcig.cz>
#include"Shop.hpp"

#include<yade/yade-core/MetaBody.hpp>
#include<yade/yade-core/Body.hpp>

#include<yade/yade-package-common/MetaInteractingGeometry2AABB.hpp>
#include<yade/yade-package-common/MetaInteractingGeometry.hpp>
#include<yade/yade-package-common/Box.hpp>
#include<yade/yade-package-common/AABB.hpp>
#include<yade/yade-package-common/Sphere.hpp>
#include<yade/yade-package-common/PersistentSAPCollider.hpp>

#include<yade/yade-package-common/BodyRedirectionVector.hpp>
#include<yade/yade-package-common/InteractionVecSet.hpp>
#include<yade/yade-package-common/PhysicalActionVectorVector.hpp>

#include<yade/yade-package-common/InteractingBox.hpp>
#include<yade/yade-package-common/InteractingSphere.hpp>


#include<yade/yade-package-common/PhysicalActionContainerReseter.hpp>
#include<yade/yade-package-common/PhysicalActionContainerInitializer.hpp>
#include<yade/yade-package-common/PhysicalParametersMetaEngine.hpp>

#include<yade/yade-package-common/InteractionGeometryMetaEngine.hpp>
#include<yade/yade-package-common/InteractionPhysicsMetaEngine.hpp>
#include<yade/yade-package-common/BoundingVolumeMetaEngine.hpp>
#include<yade/yade-package-common/PhysicalActionDamper.hpp>
#include<yade/yade-package-common/PhysicalActionApplier.hpp>
#include<yade/yade-package-common/CundallNonViscousForceDamping.hpp>
#include<yade/yade-package-common/CundallNonViscousMomentumDamping.hpp>
#include<yade/yade-package-common/GravityEngine.hpp>

#include<yade/yade-package-dem/BodyMacroParameters.hpp>
#include<yade/yade-package-dem/ElasticCriterionTimeStepper.hpp>
#include<yade/yade-package-dem/ElasticContactLaw.hpp>
#include<yade/yade-package-dem/ElasticCohesiveLaw.hpp>

#include<yade/yade-extra/PythonRecorder.hpp>
#include<yade/yade-extra/Clump.hpp>
//#include "BodyMacroParameters.hpp"


#define _SPEC_CAST(orig,cast) template<> void Shop::setDefault<orig>(string key, orig val){setDefault(key,cast(val));}
_SPEC_CAST(const char*,string);
_SPEC_CAST(char*,string);
_SPEC_CAST(int,long);
_SPEC_CAST(unsigned int,long);
_SPEC_CAST(unsigned long,long);
#undef _SPEC_CAST

map<string,boost::any> Shop::defaults;

void Shop::init(){
	defaults["container_is_not_empty"]=boost::any(0); // prevent loops from ensureInit();

	setDefault("body_sdecGroupMask",55);
	
	setDefault("phys_density",2e3);
	setDefault("phys_young",4e7);
	setDefault("phys_poisson",.3);
	setDefault("phys_frictionAngle",0.5236); //30˚
	setDefault("phys_se3_orientation",Quaternionr(Vector3r(0,0,1),0));

	setDefault("aabb_randomColor",false);
	setDefault("aabb_color",Vector3r(0,1,0));

	setDefault("mold_randomColor",true);
	setDefault("mold_color",Vector3r(1,0,0));

	setDefault("shape_randomColor",true);
	setDefault("shape_color",Vector3r(0,0,1));

	setDefault("shape_wire",false);
	setDefault("shape_visible",true);
	setDefault("shape_shadowCaster",true);

	setDefault("param_damping",.2);
	setDefault("param_gravity",Vector3r(0,0,-10));
	setDefault("param_timeStepUpdateInterval",300);
	setDefault("param_momentRotationLaw",true);

	setDefault("param_python",false);

}

/*! Create root body. */
shared_ptr<MetaBody> Shop::rootBody(){
	shared_ptr<MetaBody> rootBody = shared_ptr<MetaBody>(new MetaBody);
	rootBody->isDynamic=false;
	
	shared_ptr<ParticleParameters> physics(new ParticleParameters);
	physics->se3=Se3r(Vector3r(0,0,0),Quaternionr(Vector3r(0,0,1),0));
	physics->mass=0;
	physics->velocity=Vector3r::ZERO;
	physics->acceleration=Vector3r::ZERO;
	rootBody->physicalParameters=physics;

	shared_ptr<MetaInteractingGeometry> set(new MetaInteractingGeometry());	set->diffuseColor=Vector3r(0,0,1);
	rootBody->interactingGeometry=dynamic_pointer_cast<InteractingGeometry>(set);	
	
	shared_ptr<AABB> aabb(new AABB); aabb->diffuseColor=Vector3r(0,0,1);
	rootBody->boundingVolume=dynamic_pointer_cast<BoundingVolume>(aabb);
	
	rootBody->persistentInteractions=shared_ptr<InteractionContainer>(new InteractionVecSet);
	rootBody->transientInteractions=shared_ptr<InteractionContainer>(new InteractionVecSet);
	rootBody->physicalActions=shared_ptr<PhysicalActionContainer>(new PhysicalActionVectorVector);
	rootBody->bodies=shared_ptr<BodyContainer>(new BodyRedirectionVector);

	return rootBody;
}


/*! Assign default set of actors (initializers and engines) to an existing MetaBody.
 */
void Shop::rootBodyActors(shared_ptr<MetaBody> rootBody){
	// initializers	
	rootBody->initializers.clear();

	shared_ptr<PhysicalActionContainerInitializer> physicalActionInitializer(new PhysicalActionContainerInitializer);
	physicalActionInitializer->physicalActionNames.push_back("Force");
	physicalActionInitializer->physicalActionNames.push_back("Momentum");
	rootBody->initializers.push_back(physicalActionInitializer);
	
	shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
	boundingVolumeDispatcher->add("InteractingSphere","AABB","InteractingSphere2AABB");
	boundingVolumeDispatcher->add("InteractingBox","AABB","InteractingBox2AABB");
	boundingVolumeDispatcher->add("MetaInteractingGeometry","AABB","MetaInteractingGeometry2AABB");
	rootBody->initializers.push_back(boundingVolumeDispatcher);

	//engines
	rootBody->engines.clear();

	if(getDefault<long>("param_timeStepUpdateInterval")>0){
		shared_ptr<ElasticCriterionTimeStepper> sdecTimeStepper(new ElasticCriterionTimeStepper);
		sdecTimeStepper->sdecGroupMask=getDefault<long>("body_sdecGroupMask");
		sdecTimeStepper->timeStepUpdateInterval=getDefault<long>("param_timeStepUpdateInterval");
		rootBody->engines.push_back(sdecTimeStepper);
	}

	rootBody->engines.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));

	rootBody->engines.push_back(boundingVolumeDispatcher);

	rootBody->engines.push_back(shared_ptr<Engine>(new PersistentSAPCollider));

	shared_ptr<InteractionGeometryMetaEngine> interactionGeometryDispatcher(new InteractionGeometryMetaEngine);
	interactionGeometryDispatcher->add("InteractingSphere","InteractingSphere","InteractingSphere2InteractingSphere4SpheresContactGeometry");
	interactionGeometryDispatcher->add("InteractingSphere","InteractingBox","InteractingBox2InteractingSphere4SpheresContactGeometry");
	rootBody->engines.push_back(interactionGeometryDispatcher);

	shared_ptr<InteractionPhysicsMetaEngine> interactionPhysicsDispatcher(new InteractionPhysicsMetaEngine);
	interactionPhysicsDispatcher->add("BodyMacroParameters","BodyMacroParameters","MacroMicroElasticRelationships");
	rootBody->engines.push_back(interactionPhysicsDispatcher);
		
	shared_ptr<ElasticContactLaw> constitutiveLaw(new ElasticContactLaw);
	constitutiveLaw->sdecGroupMask = getDefault<long>("body_sdecGroupMask");
	constitutiveLaw->momentRotationLaw = getDefault<bool>("param_momentRotationLaw");
	rootBody->engines.push_back(constitutiveLaw);

	shared_ptr<ElasticCohesiveLaw> constitutiveLaw2(new ElasticCohesiveLaw);
	constitutiveLaw2->sdecGroupMask = getDefault<long>("body_sdecGroupMask");
	constitutiveLaw2->momentRotationLaw = getDefault<bool>("param_momentRotationLaw");
	rootBody->engines.push_back(constitutiveLaw2);
	
	if(getDefault<Vector3r>("param_gravity")!=Vector3r(0,0,0)){
		shared_ptr<GravityEngine> gravityCondition(new GravityEngine);
		gravityCondition->gravity=getDefault<Vector3r>("param_gravity");
		rootBody->engines.push_back(gravityCondition);
	}
	
	if(getDefault<double>("param_damping")>0){
		shared_ptr<CundallNonViscousForceDamping> actionForceDamping(new CundallNonViscousForceDamping);
		actionForceDamping->damping = getDefault<double>("param_damping");
		shared_ptr<CundallNonViscousMomentumDamping> actionMomentumDamping(new CundallNonViscousMomentumDamping);
		actionMomentumDamping->damping = getDefault<double>("param_damping");
		shared_ptr<PhysicalActionDamper> actionDampingDispatcher(new PhysicalActionDamper);
		actionDampingDispatcher->add("Force","ParticleParameters","CundallNonViscousForceDamping",actionForceDamping);
		actionDampingDispatcher->add("Momentum","RigidBodyParameters","CundallNonViscousMomentumDamping",actionMomentumDamping);
		rootBody->engines.push_back(actionDampingDispatcher);
	}
	
	shared_ptr<PhysicalActionApplier> applyActionDispatcher(new PhysicalActionApplier);
	applyActionDispatcher->add("Force","ParticleParameters","NewtonsForceLaw");
	applyActionDispatcher->add("Momentum","RigidBodyParameters","NewtonsMomentumLaw");
	rootBody->engines.push_back(applyActionDispatcher);
	
	shared_ptr<PhysicalParametersMetaEngine> positionIntegrator(new PhysicalParametersMetaEngine);
	positionIntegrator->add("ParticleParameters","LeapFrogPositionIntegrator");
	rootBody->engines.push_back(positionIntegrator);

	shared_ptr<PhysicalParametersMetaEngine> orientationIntegrator(new PhysicalParametersMetaEngine);
	orientationIntegrator->add("RigidBodyParameters","LeapFrogOrientationIntegrator");
	rootBody->engines.push_back(orientationIntegrator);

	// clumps do not need to subscribe currently (that will most likely change, though)
	rootBody->engines.push_back(shared_ptr<ClumpMemberMover>(new ClumpMemberMover));

	#ifdef EMBED_PYTHON
		if(getDefault<string>("param_pythonExpr").size()>0){
			shared_ptr<PythonRecorder> pythonRecorder=shared_ptr<PythonRecorder>(new PythonRecorder);
			pythonRecorder->expression=getDefault<string>("param_pythonExpr");
			rootBody->engines.push_back(pythonRecorder);
		}
	#endif
}


/*! Create body - sphere. */
shared_ptr<Body> Shop::sphere(Vector3r center, Real radius){
	// body itself
	shared_ptr<Body> body=shared_ptr<Body>(new Body(0,getDefault<long>("body_sdecGroupMask")));
	body->isDynamic=true;

	// physics
	shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	physics->angularVelocity=Vector3r(0,0,0);
	physics->velocity=Vector3r(0,0,0);
	physics->mass=4.0/3.0*Mathr::PI*radius*radius*radius*getDefault<double>("phys_density");
	physics->inertia=Vector3r(2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius);
	physics->se3=Se3r(center,Quaternionr(Vector3r(0,0,1),0));
	physics->young=getDefault<double>("phys_young");
	physics->poisson=getDefault<double>("phys_poisson");
	physics->frictionAngle=getDefault<double>("phys_frictionAngle");
	body->physicalParameters=physics;

	// aabb
	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor=getDefault<bool>("aabb_randomColor")?Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom()):getDefault<Vector3r>("aabb_color");
	body->boundingVolume=aabb;

	// mold
	shared_ptr<InteractingSphere> mold(new InteractingSphere);
	mold->radius=radius;
	mold->diffuseColor=getDefault<bool>("mold_randomColor")?Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom()):getDefault<Vector3r>("mold_color");
	body->interactingGeometry=mold;

	//shape
	shared_ptr<Sphere> shape(new Sphere);
	shape->radius=radius;
	shape->diffuseColor=getDefault<bool>("shape_randomColor")?Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom()):getDefault<Vector3r>("shape_color");
	shape->wire=getDefault<bool>("shape_wire");
	shape->visible=getDefault<bool>("shape_visible");
	shape->shadowCaster=getDefault<bool>("shape_shadowCaster");
	body->geometricalModel=shape;

	return body;

}

/*! Create body - box. */
shared_ptr<Body> Shop::box(Vector3r center, Vector3r extents){
		shared_ptr<Body> body=shared_ptr<Body>(new Body(0,getDefault<long>("body_sdecGroupMask")));
		body->isDynamic=true;

		shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
		physics->angularVelocity=Vector3r(0,0,0);
		physics->velocity=Vector3r(0,0,0);
		physics->mass=8*extents[0]*extents[1]*extents[2]*getDefault<double>("phys_density");
		physics->inertia=Vector3r(physics->mass*(4*extents[1]*extents[1]+4*extents[2]*extents[2])/12.,physics->mass*(4*extents[0]*extents[0]+4*extents[2]*extents[2])/12.,physics->mass*(4*extents[0]*extents[0]+4*extents[1]*extents[1])/12.);
		physics->se3=Se3r(center,Quaternionr(Vector3r(0,0,1),0));
		physics->young=getDefault<double>("phys_young");
		physics->poisson=getDefault<double>("phys_poisson");
		physics->frictionAngle=getDefault<double>("phys_frictionAngle");
		body->physicalParameters=physics;

		// aabb
		shared_ptr<AABB> aabb(new AABB);
		aabb->diffuseColor=getDefault<bool>("aabb_randomColor")?Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom()):getDefault<Vector3r>("aabb_color");
		body->boundingVolume=aabb;

		//shape
		shared_ptr<Box> shape(new Box);
		shape->extents=extents;
		shape->diffuseColor=getDefault<bool>("shape_randomColor")?Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom()):getDefault<Vector3r>("shape_color");
		shape->wire=getDefault<bool>("shape_wire");
		shape->visible=getDefault<bool>("shape_visible");
		shape->shadowCaster=getDefault<bool>("shape_shadowCaster");
		body->geometricalModel=shape;

		// mold
		shared_ptr<InteractingBox> mold(new InteractingBox);
		mold->extents=extents;
		mold->diffuseColor=getDefault<bool>("mold_randomColor")?Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom()):getDefault<Vector3r>("mold_color");
		body->interactingGeometry=mold;

		return body;
}
