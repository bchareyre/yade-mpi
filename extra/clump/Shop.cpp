// 2007 © Václav Šmilauer <eudoxos@arcig.cz>
#include"Shop.hpp"

#include<boost/filesystem/convenience.hpp>

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
#include<yade/pkg-dem/MacroMicroElasticRelationships.hpp>
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

#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>
#include<yade/pkg-dem/SDECLinkGeometry.hpp>
#include<yade/pkg-dem/SDECLinkPhysics.hpp>


#if 0
#ifdef EMBED_PYTHON
	#include<yade/extra/PythonRecorder.hpp>
#endif
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
	boundingVolumeDispatcher->add(new InteractingSphere2AABB);
	boundingVolumeDispatcher->add(new InteractingBox2AABB);
	boundingVolumeDispatcher->add(new TetraAABB);
	boundingVolumeDispatcher->add(new MetaInteractingGeometry2AABB);
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
	interactionGeometryDispatcher->add(new InteractingSphere2InteractingSphere4SpheresContactGeometry);
	interactionGeometryDispatcher->add(new InteractingBox2InteractingSphere4SpheresContactGeometry);
	interactionGeometryDispatcher->add(new Tetra2TetraBang);
	rootBody->engines.push_back(interactionGeometryDispatcher);

	shared_ptr<InteractionPhysicsMetaEngine> interactionPhysicsDispatcher(new InteractionPhysicsMetaEngine);
	interactionPhysicsDispatcher->add(new MacroMicroElasticRelationships);
	rootBody->engines.push_back(interactionPhysicsDispatcher);
		
	shared_ptr<ElasticContactLaw> constitutiveLaw(new ElasticContactLaw);
	constitutiveLaw->sdecGroupMask = getDefault<int>("body_sdecGroupMask");
	constitutiveLaw->momentRotationLaw = getDefault<bool>("param_momentRotationLaw");
	rootBody->engines.push_back(constitutiveLaw);

	shared_ptr<ElasticCohesiveLaw> constitutiveLaw2(new ElasticCohesiveLaw);
	constitutiveLaw2->sdecGroupMask = getDefault<int>("body_sdecGroupMask");
	constitutiveLaw2->momentRotationLaw = getDefault<bool>("param_momentRotationLaw");
	rootBody->engines.push_back(constitutiveLaw2);

	shared_ptr<TetraLaw> constitutiveLaw3(new TetraLaw);
	constitutiveLaw3->sdecGroupMask = getDefault<int>("body_sdecGroupMask");
	rootBody->engines.push_back(constitutiveLaw3);
	
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
		actionDampingDispatcher->add(actionForceDamping);
		actionDampingDispatcher->add(actionMomentumDamping);
		rootBody->engines.push_back(actionDampingDispatcher);
	}
	
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
#if 0
	#ifdef EMBED_PYTHON
		if(getDefault<string>("param_pythonRunExpr").length()>0 || getDefault<string>("param_pythonInitExpr").length()>0){
			shared_ptr<PythonRecorder> pythonRecorder=shared_ptr<PythonRecorder>(new PythonRecorder);
			pythonRecorder->initExpr=getDefault<string>("param_pythonInitExpr");
			pythonRecorder->runExpr=getDefault<string>("param_pythonRunExpr");
			rootBody->engines.push_back(pythonRecorder);
		}
	#endif
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


void Shop::saveSpheresToFile(string fname){
	const shared_ptr<MetaBody>& rootBody=Omega::instance().getRootBody();
	ofstream f(fname.c_str());
	if(!f.good()) throw runtime_error("Unable to open file `"+fname+"'");

	for(BodyContainer::iterator I=rootBody->bodies->begin(); I!=rootBody->bodies->end(); ++I){
		const shared_ptr<Body>& b=*I;
		if (!b->isDynamic) continue;
		shared_ptr<InteractingSphere>	intSph=dynamic_pointer_cast<InteractingSphere>(b->interactingGeometry);
		if(!intSph) continue;
		const Vector3r& pos=b->physicalParameters->se3.position;
		f<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<" "<<intSph->radius<<" "<<1<<" "<<1<<endl;
	}
	f.close();
}

vector<pair<Vector3r,Real> > Shop::loadSpheresFromFile(string fname, Vector3r& minXYZ, Vector3r& maxXYZ){
	if(!boost::filesystem::exists(fname)) {
		throw std::runtime_error(string("File with spheres `")+fname+"' doesn't exist.");
	}
	vector<pair<Vector3r,Real> > spheres;
	ifstream sphereFile(fname.c_str());
	if(!sphereFile.good()) throw std::runtime_error("File with spheres `"+fname+"' couldn't be opened.");
	int tmp1,tmp2;
	Vector3r C;
	Real r;
	while(!sphereFile.eof()){
		sphereFile>>C[0]; sphereFile>>C[1]; sphereFile>>C[2];
		sphereFile>>r;
		// TRVAR3(spheres.size(),C,r);
		sphereFile>>tmp1;
		if(r==1) continue; // arrrgh, boxes have 5 record/line, spheres have 6; 4th number for box is 1 and we assume there is no sphere with radius 1; Wenjie, I'm gonna tell you one day how smart this format is.
		if(sphereFile.eof()) continue; // prevents trailing newlines from copying last sphere as well
		sphereFile>>tmp2; // read the 6th (unused) number for spheres
		for(int j=0; j<3; j++) { minXYZ[j]=(spheres.size()>0?min(C[j]-r,minXYZ[j]):C[j]-r); maxXYZ[j]=(spheres.size()>0?max(C[j]+r,maxXYZ[j]):C[j]+r);}
		spheres.push_back(pair<Vector3r,Real>(C,r));
	}
	TRVAR2(minXYZ,maxXYZ);
	return spheres;
}


/* Create permanent link between partcles that have transient link now, return number of created permalinks.
 *
 * Needs valid Omega instance. Stiffness parameters are copied from the transientInteraction. Order of forceId1 and forceId2 is irrelevant.
 *
 * @param cohesionMask mask that must hold for _both_ Bodies in interaction, unless it is zero and is ignored.
 * @param linkOk is function expression: bool linkOK(body_id_t,body_id_t) will tell us, whether link between two particular bodies should be created. Defaults to true (always create).
 **/
int Shop::createCohesion(Real limitNormalForce, Real limitShearForce, int cohesionMask, boost::function<bool(body_id_t,body_id_t)> linkOK){
	int numNewLinks=0;
	shared_ptr<MetaBody> rb=YADE_PTR_CAST<MetaBody>(Omega::instance().getRootBody());
	vector<pair<body_id_t,body_id_t> > toBeDeleted;

	// loop over transient interactions
	for(InteractionContainer::iterator I=rb->transientInteractions->begin(); I!=rb->transientInteractions->end(); ++I){
		const shared_ptr<Interaction>& contact=*I;
		if (!contact->isReal) continue; // only overlapping AABBs, not body contact
		int id1=contact->getId1(), id2=contact->getId2();
		LOG_DEBUG("Considering linking #"<<id1<<" + #"<<id2<<"...");

		if (!linkOK(id1,id2)) {LOG_DEBUG("Disallowed by linkOK."); continue; }// user didn't want to link these

		const shared_ptr<Body>& b1=Body::byId(id1), b2=Body::byId(id2);
		if (!(cohesionMask==0 || (b1->getGroupMask() & cohesionMask) && (b2->getGroupMask() & cohesionMask))) {LOG_DEBUG("Mask mismatch."); continue; }// if mask is valid for both bodies or is zero, go ahead
		// if we have sphere without interacting sphere, it is (most likely) a bug anyway -- no need to dynamic-cast in non-debug builds
		shared_ptr<SpheresContactGeometry> contGeom=dynamic_pointer_cast<SpheresContactGeometry>(contact->interactionGeometry);
		shared_ptr<ElasticContactInteraction> contPhys=dynamic_pointer_cast<ElasticContactInteraction>(contact->interactionPhysics);
		shared_ptr<InteractingSphere>	intSph1=dynamic_pointer_cast<InteractingSphere>(b1->interactingGeometry);
		shared_ptr<InteractingSphere>	intSph2=dynamic_pointer_cast<InteractingSphere>(b2->interactingGeometry);

		if(!(contGeom && contPhys && intSph1 && intSph2)) { LOG_DEBUG("Non-spherical elemnt(s) or inelastic contact."); continue;}

		// replace this transient contact by permanent contact
		// we don't need to delete the transient one since collider will (should) do it in the next loop.
		toBeDeleted.push_back(pair<body_id_t,body_id_t>(id1,id2));

		shared_ptr<Interaction> link(new Interaction(id1,id2));
		shared_ptr<SDECLinkGeometry> linkGeom(new SDECLinkGeometry);
		shared_ptr<SDECLinkPhysics> linkPhys(new SDECLinkPhysics);

		linkGeom->radius1=intSph1->radius-.5*abs(intSph1->radius-intSph2->radius);
		linkGeom->radius2=intSph2->radius-.5*abs(intSph1->radius-intSph2->radius);
		linkGeom->normal=contGeom->normal;
		link->interactionGeometry=linkGeom;

		linkPhys->initialKn=contPhys->kn; linkPhys->initialKs=contPhys->ks;
		linkPhys->initialEquilibriumDistance=intSph1->radius+intSph2->radius;
		linkPhys->kn=linkPhys->initialKn; linkPhys->ks=linkPhys->initialKs; linkPhys->equilibriumDistance=linkPhys->initialEquilibriumDistance;
		linkPhys->heta=1;
		linkPhys->knMax=contPhys->normalForce.Length()*10000; // limitNormalForce;
		linkPhys->ksMax=contPhys->normalForce.Length()*10000; // limitShearForce;
		link->interactionPhysics=linkPhys;

		link->isReal=true;
		link->isNew=true; // only true if linkPhys doesn't exist; but we've just created it ourselves
		rb->persistentInteractions->insert(link);
		numNewLinks++;
		LOG_DEBUG("LINKED #"<<id1<<" + #"<<id2<<"! (knMax="<<linkPhys->knMax<<", ksMax="<<linkPhys->ksMax<<")");
	}
	// TODO: warn user if CohesiveElasticLaw is not active

	return numNewLinks;
}
