// 2007 © Václav Šmilauer <eudoxos@arcig.cz>
#include"Shop.hpp"

#include<yade/core/MetaBody.hpp>
#include<yade/core/Body.hpp>

#include<yade/pkg-common/MetaInteractingGeometry2AABB.hpp>
#include<yade/pkg-common/MetaInteractingGeometry.hpp>
#include<yade/pkg-common/Box.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-common/PersistentSAPCollider.hpp>

#include<yade/pkg-common/BodyRedirectionVector.hpp>
#include<yade/pkg-common/InteractionVecSet.hpp>
#include<yade/pkg-common/PhysicalActionVectorVector.hpp>

#include<yade/pkg-common/InteractingBox.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>

#include<yade/pkg-common/InteractingSphere2AABB.hpp>
#include<yade/pkg-common/InteractingBox2AABB.hpp>
#include<yade/pkg-common/MetaInteractingGeometry.hpp>
#include<yade/pkg-common/MetaInteractingGeometry2AABB.hpp>
#include<yade/pkg-common/Momentum.hpp>
#include<yade/pkg-common/Force.hpp>
#include<yade/pkg-common/NewtonsForceLaw.hpp>
#include<yade/pkg-common/NewtonsMomentumLaw.hpp>
#include<yade/pkg-common/LeapFrogPositionIntegrator.hpp>
#include<yade/pkg-common/LeapFrogOrientationIntegrator.hpp>
#include<yade/pkg-dem/InteractingSphere2InteractingSphere4SpheresContactGeometry.hpp>
#include<yade/pkg-dem/InteractingBox2InteractingSphere4SpheresContactGeometry.hpp>
/*class InteractingSphere2AABB;
class InteractingBox2AABB;
class MetaInteractingGeometry;
class MetaInteractingGeometry2AABB; */


#include<yade/pkg-common/PhysicalActionContainerReseter.hpp>
#include<yade/pkg-common/PhysicalActionContainerInitializer.hpp>
#include<yade/pkg-common/PhysicalParametersMetaEngine.hpp>

#include<yade/pkg-common/InteractionGeometryMetaEngine.hpp>
#include<yade/pkg-common/InteractionPhysicsMetaEngine.hpp>
#include<yade/pkg-common/BoundingVolumeMetaEngine.hpp>
#include<yade/pkg-common/PhysicalActionDamper.hpp>
#include<yade/pkg-common/PhysicalActionApplier.hpp>
#include<yade/pkg-common/CundallNonViscousForceDamping.hpp>
#include<yade/pkg-common/CundallNonViscousMomentumDamping.hpp>
#include<yade/pkg-common/GravityEngine.hpp>

#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-dem/ElasticCriterionTimeStepper.hpp>
#include<yade/pkg-dem/ElasticContactLaw.hpp>
#include<yade/pkg-dem/ElasticCohesiveLaw.hpp>

#ifdef EMBED_PYTHON
	#include<yade/extra/PythonRecorder.hpp>
#endif

#include<yade/extra/Tetra.hpp>

//#include<yade/extra/Clump.hpp>
//#include <yade/pkg-dem/BodyMacroParameters.hpp>


#define _SPEC_CAST(orig,cast) template<> void Shop::setDefault<orig>(string key, orig val){setDefault(key,cast(val));}
_SPEC_CAST(const char*,string);
_SPEC_CAST(char*,string);
#undef _SPEC_CAST

map<string,boost::any> Shop::defaults;

CREATE_LOGGER(Shop);

template <typename valType> valType Shop::getDefault(const string& key) {
	ensureInit();
	try{return boost::any_cast<valType>(defaults[key]);}
	catch(boost::bad_any_cast& e){
		LOG_FATAL("Cast error while getting key `"<<key<<"' of type `"<<typeid(valType).name()<<"' ("<<e.what()<<").");
		if(!hasDefault(key) || (defaults[key].type()==typeid(void))){LOG_FATAL("Key `"<<key<<"' not defined in the map.");}
		else{LOG_INFO("(key `"<<key<<"' exists and if of type `"<<defaults[key].type().name()<<"').");}
		throw;
	}
}

void Shop::init(){
	//LOG_INFO("Container length is "<<defaults.size()<<endl);
	/*for(map<string,boost::any>::iterator I=defaults.begin(); I!=defaults.end(); I++){
		LOG_INFO("Key `"<<I->first<<", type `"<<I->second.type().name()<<"'.");
	}*/

	defaults["container_is_not_empty"]=boost::any(0); // prevent loops from ensureInit();

	setDefault<int>("body_sdecGroupMask",55);
	
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
	setDefault<int>("param_timeStepUpdateInterval",300);
	setDefault("param_momentRotationLaw",true);

	setDefault("param_pythonInitExpr",string("print 'Hello world!'"));
	setDefault("param_pythonRunExpr",string(""));

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
	rootBody->interactingGeometry=YADE_PTR_CAST<InteractingGeometry>(set);	
	
	shared_ptr<AABB> aabb(new AABB); aabb->diffuseColor=Vector3r(0,0,1);
	rootBody->boundingVolume=YADE_PTR_CAST<BoundingVolume>(aabb);
	
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
	boundingVolumeDispatcher->DISPATCHER_ADD3(InteractingSphere,AABB,InteractingSphere2AABB);
	boundingVolumeDispatcher->DISPATCHER_ADD3(InteractingBox,AABB,InteractingBox2AABB);
	boundingVolumeDispatcher->DISPATCHER_ADD3(TetraMold,AABB,TetraAABB);
	boundingVolumeDispatcher->DISPATCHER_ADD3(MetaInteractingGeometry,AABB,MetaInteractingGeometry2AABB);
	rootBody->initializers.push_back(boundingVolumeDispatcher);

	//engines
	rootBody->engines.clear();

	/* big fat FIXME:
	 * for some */
	#define GO(type) try{cerr<<"Cast to" #type<<" gives: "<<getDefault<type>("body_sdecGroupMask")<<endl;} catch(boost::bad_any_cast){}
	/*	GO(unsigned short); GO(short); GO(char);GO(int);GO(unsigned int);GO(long);GO(unsigned long);GO(long long);GO(unsigned long long); */
	
	if(getDefault<int>("param_timeStepUpdateInterval")>0){
		shared_ptr<ElasticCriterionTimeStepper> sdecTimeStepper(new ElasticCriterionTimeStepper);
		sdecTimeStepper->sdecGroupMask=getDefault<int>("body_sdecGroupMask");
		sdecTimeStepper->timeStepUpdateInterval=getDefault<int>("param_timeStepUpdateInterval");
		sdecTimeStepper->timeStepUpdateInterval=300;
		rootBody->engines.push_back(sdecTimeStepper);
	}

	rootBody->engines.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));

	rootBody->engines.push_back(boundingVolumeDispatcher);

	rootBody->engines.push_back(shared_ptr<Engine>(new PersistentSAPCollider));

	shared_ptr<InteractionGeometryMetaEngine> interactionGeometryDispatcher(new InteractionGeometryMetaEngine);
	interactionGeometryDispatcher->DISPATCHER_ADD3(InteractingSphere,InteractingSphere,InteractingSphere2InteractingSphere4SpheresContactGeometry);
	interactionGeometryDispatcher->DISPATCHER_ADD3(InteractingSphere,InteractingBox,InteractingBox2InteractingSphere4SpheresContactGeometry);
	interactionGeometryDispatcher->DISPATCHER_ADD3(TetraMold,TetraMold,Tetra2TetraBang);
	rootBody->engines.push_back(interactionGeometryDispatcher);

	shared_ptr<InteractionPhysicsMetaEngine> interactionPhysicsDispatcher(new InteractionPhysicsMetaEngine);
	interactionPhysicsDispatcher->DISPATCHER_ADD3(BodyMacroParameters,BodyMacroParameters,MacroMicroElasticRelationships);
	rootBody->engines.push_back(interactionPhysicsDispatcher);
		
	shared_ptr<ElasticContactLaw> constitutiveLaw(new ElasticContactLaw);
	constitutiveLaw->sdecGroupMask = getDefault<int>("body_sdecGroupMask");
	constitutiveLaw->momentRotationLaw = getDefault<bool>("param_momentRotationLaw");
	rootBody->engines.push_back(constitutiveLaw);

	shared_ptr<ElasticCohesiveLaw> constitutiveLaw2(new ElasticCohesiveLaw);
	constitutiveLaw2->sdecGroupMask = getDefault<int>("body_sdecGroupMask");
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
		actionDampingDispatcher->DISPATCHER_ADD3_1(Force,ParticleParameters,CundallNonViscousForceDamping,actionForceDamping);
		actionDampingDispatcher->DISPATCHER_ADD3_1(Momentum,RigidBodyParameters,CundallNonViscousMomentumDamping,actionMomentumDamping);
		rootBody->engines.push_back(actionDampingDispatcher);
	}
	
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

	#ifdef EMBED_PYTHON
		if(getDefault<string>("param_pythonRunExpr").length()>0 || getDefault<string>("param_pythonInitExpr").length()>0){
			shared_ptr<PythonRecorder> pythonRecorder=shared_ptr<PythonRecorder>(new PythonRecorder);
			pythonRecorder->initExpr=getDefault<string>("param_pythonInitExpr");
			pythonRecorder->runExpr=getDefault<string>("param_pythonRunExpr");
			rootBody->engines.push_back(pythonRecorder);
		}
	#endif
}


/*! Create body - sphere. */
shared_ptr<Body> Shop::sphere(Vector3r center, Real radius){
	// body itself
	shared_ptr<Body> body=shared_ptr<Body>(new Body(body_id_t(0),getDefault<int>("body_sdecGroupMask")));
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
		shared_ptr<Body> body=shared_ptr<Body>(new Body(body_id_t(0),getDefault<int>("body_sdecGroupMask")));
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

/*! Create body - tetrahedron. */
shared_ptr<Body> Shop::tetra(Vector3r v_global[4]){
		shared_ptr<Body> body=shared_ptr<Body>(new Body(body_id_t(0),getDefault<int>("body_sdecGroupMask")));
		body->isDynamic=true;

		Vector3r centroid=(v_global[0]+v_global[1]+v_global[2]+v_global[3])*.25;
		Vector3r v[4]; for(int i=0; i<4; i++) v[i]=v_global[i]-centroid;

		shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
		physics->angularVelocity=Vector3r(0,0,0);
		physics->velocity=Vector3r(0,0,0);
		physics->mass=getDefault<double>("phys_density")*TetrahedronVolume(v);
		// inertia will be calculated below, by TetrahedronWithLocalAxesPrincipal
		physics->se3=Se3r(centroid,Quaternionr(Vector3r(0,0,1),0));
		physics->young=getDefault<double>("phys_young");
		physics->poisson=getDefault<double>("phys_poisson");
		physics->frictionAngle=getDefault<double>("phys_frictionAngle");
		body->physicalParameters=physics;

		// aabb
		shared_ptr<AABB> aabb(new AABB);
		aabb->diffuseColor=getDefault<bool>("aabb_randomColor")?Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom()):getDefault<Vector3r>("aabb_color");
		body->boundingVolume=aabb;

		//shape
		shared_ptr<Tetrahedron> shape(new Tetrahedron);
		shape->v[0]=v[0]; shape->v[1]=v[1]; shape->v[2]=v[2]; shape->v[3]=v[3];
		shape->diffuseColor=getDefault<bool>("shape_randomColor")?Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom()):getDefault<Vector3r>("shape_color");
		shape->wire=getDefault<bool>("shape_wire");
		shape->visible=getDefault<bool>("shape_visible");
		shape->shadowCaster=getDefault<bool>("shape_shadowCaster");
		body->geometricalModel=shape;

		// mold
		shared_ptr<TetraMold> mold(new TetraMold(v[0],v[1],v[2],v[3]));
		mold->diffuseColor=getDefault<bool>("mold_randomColor")?Vector3r(Mathr::UnitRandom(),Mathr::UnitRandom(),Mathr::UnitRandom()):getDefault<Vector3r>("mold_color");
		body->interactingGeometry=mold;

		// make local axes coincident with principal axes
		TetrahedronWithLocalAxesPrincipal(body);

		return body;
}
