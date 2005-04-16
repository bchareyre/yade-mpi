#include "FEMBeam.hpp"


#include "AABB.hpp"
#include "Sphere.hpp"
#include "Tetrahedron.hpp"
#include "FEMSetParameters.hpp"
#include "FEMTetrahedronParameters.hpp"
#include "FEMNodeData.hpp"
#include "InteractionDescriptionSet.hpp"

#include "ActionParameterReset.hpp"
#include "ActionParameterInitializer.hpp"
#include "CundallNonViscousForceDampingFunctor.hpp"
#include "CundallNonViscousMomentumDampingFunctor.hpp"

#include "GravityCondition.hpp"

// body
#include "ComplexBody.hpp"
#include "SimpleBody.hpp"

// dispatchers
#include "BodyPhysicalParametersDispatcher.hpp"
#include "InteractionGeometryDispatcher.hpp"
#include "InteractionPhysicsDispatcher.hpp"
#include "ActionParameterDispatcher.hpp"
#include "BoundingVolumeDispatcher.hpp"

#include <boost/filesystem/convenience.hpp>

using namespace boost;
using namespace std;

FEMBeam::FEMBeam () : FileGenerator()
{
	gravity 		= Vector3r(0,-9.81,0);
	femTxtFile 		= "../data/fem.beam";
	nodeGroupMask 		= 1;
	tetrahedronGroupMask 	= 2;
}

FEMBeam::~FEMBeam ()
{ 
}

void FEMBeam::postProcessAttributes(bool)
{
}

void FEMBeam::registerAttributes()
{
	REGISTER_ATTRIBUTE(femTxtFile);
	REGISTER_ATTRIBUTE(gravity);
}

string FEMBeam::generate()
{
	rootBody = shared_ptr<ComplexBody>(new ComplexBody);
	createActors(rootBody);
	positionRootBody(rootBody);

///////// load file 

	if(femTxtFile.size() != 0 && filesystem::exists(femTxtFile) )
	{
		ifstream loadFile(femTxtFile.c_str());
//		while( ! loadFile.eof() )
		unsigned int nbNodes,nbTetrahedrons;
		loadFile >> nbNodes;
		loadFile >> nbTetrahedrons;
		for(unsigned int i = 0 ; i<nbNodes ; ++i)
		{
			Real x,y,z;
			unsigned int id;
			loadFile >> id >> x >> y >> z;
//			cerr << id << " \t" << x << " \t" << y << " \t" << z << endl;
			shared_ptr<Body> femNode;
			createNode(femNode,Vector3r(x,y,z),id);
			rootBody->bodies->insert(femNode);
		}
		for(unsigned int i = 0 ; i<nbTetrahedrons ; ++i)
		{
			unsigned int id,id1,id2,id3,id4;
			loadFile >> id >> id1 >> id2 >> id3 >> id4;
			shared_ptr<Body> femTetrahedron;
			createTetrahedron(rootBody,femTetrahedron,id,id1,id2,id3,id4);
			rootBody->bodies->insert(femTetrahedron);
		}
	}
	
	return "";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void FEMBeam::createNode(shared_ptr<Body>& body, Vector3r position, unsigned int id)
{
	body = shared_ptr<Body>(new SimpleBody(id,nodeGroupMask));
	shared_ptr<FEMNodeData> physics(new FEMNodeData);
	shared_ptr<Sphere> gSphere(new Sphere);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1) , 0 );
	
	Real radius 			= 0.5;
	
	body->isDynamic			= true;
//cerr << position << endl;	
//	physics->angularVelocity	= Vector3r(0,0,0);
//	physics->velocity		= Vector3r(0,0,0);
//	physics->mass			= 1;
//	physics->inertia		= Vector3r(1,1,1);
	physics->se3			= Se3r(position,q);

	gSphere->radius			= radius;
	gSphere->diffuseColor		= Vector3f(0.9,0.9,0.3);
	gSphere->wire			= false;
	gSphere->visible		= true;
	gSphere->shadowCaster		= true;
	
	body->geometricalModel		= gSphere;
	body->physicalParameters	= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void FEMBeam::createTetrahedron(shared_ptr<ComplexBody>& rootBody, shared_ptr<Body>& body, unsigned int id, unsigned int id1, unsigned int id2, unsigned int id3, unsigned int id4)
{
	
	body = shared_ptr<Body>(new SimpleBody(id,tetrahedronGroupMask));
	shared_ptr<FEMTetrahedronParameters> physics(new FEMTetrahedronParameters);
	shared_ptr<Tetrahedron> gTet(new Tetrahedron);
	
	body->isDynamic			= true;
	
//	physics->angularVelocity	= Vector3r(0,0,0);
//	physics->velocity		= Vector3r(0,0,0);
//	physics->mass			= 1; // FIXME
//	physics->inertia		= Vector3r(1,1,1); // FIXME
	physics->ids.clear();
	physics->ids.push_back(id1);
	physics->ids.push_back(id2);
	physics->ids.push_back(id3);
	physics->ids.push_back(id4);
	// FIXME - remove Se3 - FEMTetrahedron DOES NOT need Se3 !
	physics->se3 			= Se3r( Vector3r(0,0,0) , Quaternionr(0,0,0,0) );

	gTet->diffuseColor		= Vector3f(1,1,1);
	gTet->wire			= false;
	gTet->visible			= true;
	gTet->shadowCaster		= true;
	gTet->v1 			= (*(rootBody->bodies))[id1]->physicalParameters->se3.position;
	gTet->v2 			= (*(rootBody->bodies))[id2]->physicalParameters->se3.position;
	gTet->v3 			= (*(rootBody->bodies))[id3]->physicalParameters->se3.position;
	gTet->v4 			= (*(rootBody->bodies))[id4]->physicalParameters->se3.position;
	
	body->geometricalModel		= gTet;
	body->physicalParameters	= physics;
	
	/*
	body = shared_ptr<Body>(new SimpleBody(0,1));
	shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Sphere> gSphere(new Sphere);
	shared_ptr<InteractionSphere> iSphere(new InteractionSphere);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);
	
	Vector3r position		= Vector3r(i,j,k)*(2*maxRadius*1.1) // this formula is crazy !!
					  - Vector3r( nbSpheres[0]/2*(2*maxRadius*1.1) , -7-maxRadius*2 , nbSpheres[2]/2*(2*maxRadius*1.1) )
					  + Vector3r(Mathr::symmetricRandom(),Mathr::symmetricRandom(),Mathr::symmetricRandom())*disorder*maxRadius;
	
	Real radius 			= (Mathr::intervalRandom(minRadius,maxRadius));
	
	body->isDynamic			= true;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 4.0/3.0*Mathr::PI*radius*radius*radius*density;
	physics->inertia		= Vector3r(2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius); //
	physics->se3			= Se3r(position,q);
	physics->young			= sphereYoungModulus;
	physics->poisson		= spherePoissonRatio;
	physics->frictionAngle		= sphereFrictionDeg * Mathr::PI/180.0;

	aabb->diffuseColor		= Vector3r(0,1,0);

	gSphere->radius			= radius;
	gSphere->diffuseColor		= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());
	gSphere->wire			= false;
	gSphere->visible		= true;
	gSphere->shadowCaster		= true;
	
	iSphere->radius			= radius;
	iSphere->diffuseColor		= Vector3f(0.8,0.3,0.3);

	body->interactionGeometry	= iSphere;
	body->geometricalModel		= gSphere;
	body->boundingVolume		= aabb;
	body->physicalParameters	= physics;
	*/
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void FEMBeam::createActors(shared_ptr<ComplexBody>& rootBody)
{
	shared_ptr<BoundingVolumeDispatcher> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeDispatcher>(new BoundingVolumeDispatcher);
	boundingVolumeDispatcher->add("InteractionDescriptionSet","AABB","InteractionDescriptionSet2AABBFunctor");

//	shared_ptr<GeometricalModelDispatcher> geometricalModelDispatcher	= shared_ptr<GeometricalModelDispatcher>(new GeometricalModelDispatcher);
//	geometricalModelDispatcher->add("FEMSetParameters","LatticeSetGeometry","FEMSet2");
	
	rootBody->actors.clear();
	rootBody->actors.push_back(boundingVolumeDispatcher);
//	rootBody->actors.push_back(geometricalModelDispatcher);
	/*
	shared_ptr<ActionParameterInitializer> actionParameterInitializer(new ActionParameterInitializer);
	actionParameterInitializer->actionParameterNames.push_back("ActionParameterForce");
	actionParameterInitializer->actionParameterNames.push_back("ActionParameterMomentum");
	
	shared_ptr<InteractionGeometryDispatcher> interactionGeometryDispatcher(new InteractionGeometryDispatcher);
	interactionGeometryDispatcher->add("InteractionSphere","InteractionSphere","Sphere2Sphere4MacroMicroContactGeometry");
	interactionGeometryDispatcher->add("InteractionSphere","InteractionBox","Box2Sphere4MacroMicroContactGeometry");

	shared_ptr<InteractionPhysicsDispatcher> interactionPhysicsDispatcher(new InteractionPhysicsDispatcher);
	interactionPhysicsDispatcher->add("BodyMacroParameters","BodyMacroParameters","MacroMicroElasticRelationships");
		
	shared_ptr<BoundingVolumeDispatcher> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeDispatcher>(new BoundingVolumeDispatcher);
	boundingVolumeDispatcher->add("InteractionSphere","AABB","Sphere2AABBFunctor");
	boundingVolumeDispatcher->add("InteractionBox","AABB","Box2AABBFunctor");
	boundingVolumeDispatcher->add("InteractionDescriptionSet","AABB","InteractionDescriptionSet2AABBFunctor");
	
	shared_ptr<GravityCondition> gravityCondition(new GravityCondition);
	gravityCondition->gravity = gravity;
	
	shared_ptr<CundallNonViscousForceDampingFunctor> actionForceDamping(new CundallNonViscousForceDampingFunctor);
	actionForceDamping->damping = dampingForce;
	shared_ptr<CundallNonViscousMomentumDampingFunctor> actionMomentumDamping(new CundallNonViscousMomentumDampingFunctor);
	actionMomentumDamping->damping = dampingMomentum;
	shared_ptr<ActionParameterDispatcher> actionDampingDispatcher(new ActionParameterDispatcher);
	actionDampingDispatcher->add("ActionParameterForce","RigidBodyParameters","CundallNonViscousForceDampingFunctor",actionForceDamping);
	actionDampingDispatcher->add("ActionParameterMomentum","RigidBodyParameters","CundallNonViscousMomentumDampingFunctor",actionMomentumDamping);
	
	shared_ptr<ActionParameterDispatcher> applyActionDispatcher(new ActionParameterDispatcher);
	applyActionDispatcher->add("ActionParameterForce","RigidBodyParameters","NewtonsForceLawFunctor");
	applyActionDispatcher->add("ActionParameterMomentum","RigidBodyParameters","NewtonsMomentumLawFunctor");
	
	shared_ptr<BodyPhysicalParametersDispatcher> positionIntegrator(new BodyPhysicalParametersDispatcher);
	positionIntegrator->add("ParticleParameters","LeapFrogPositionIntegratorFunctor");
	shared_ptr<BodyPhysicalParametersDispatcher> orientationIntegrator(new BodyPhysicalParametersDispatcher);
	orientationIntegrator->add("RigidBodyParameters","LeapFrogOrientationIntegratorFunctor");
 	

	shared_ptr<SDECTimeStepper> sdecTimeStepper(new SDECTimeStepper);
	sdecTimeStepper->sdecGroupMask = 1;
	sdecTimeStepper->interval = timeStepUpdateInterval;

	rootBody->actors.clear();
	rootBody->actors.push_back(actionParameterInitializer);
	rootBody->actors.push_back(shared_ptr<Actor>(new ActionParameterReset));
	rootBody->actors.push_back(sdecTimeStepper);
	rootBody->actors.push_back(boundingVolumeDispatcher);	
	rootBody->actors.push_back(shared_ptr<Actor>(new PersistentSAPCollider));
	rootBody->actors.push_back(interactionGeometryDispatcher);
	rootBody->actors.push_back(interactionPhysicsDispatcher);
	rootBody->actors.push_back(shared_ptr<Actor>(new ElasticContactLaw));
	rootBody->actors.push_back(gravityCondition);
	rootBody->actors.push_back(actionDampingDispatcher);
	rootBody->actors.push_back(applyActionDispatcher);
	rootBody->actors.push_back(positionIntegrator);
	if(!rotationBlocked)
		rootBody->actors.push_back(orientationIntegrator);
	*/
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void FEMBeam::positionRootBody(shared_ptr<ComplexBody>& rootBody) 
{
	rootBody->isDynamic			= false;

	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);
	shared_ptr<FEMSetParameters> physics(new FEMSetParameters);
	physics->se3				= Se3r(Vector3r(0,0,0),q);
	physics->nodeGroupMask 			= nodeGroupMask;
	physics->tetrahedronGroupMask 		= tetrahedronGroupMask;
	
	shared_ptr<InteractionDescriptionSet> set(new InteractionDescriptionSet());
	
	set->diffuseColor			= Vector3f(0,0,1);

	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor			= Vector3r(0,0,1);

	shared_ptr<GeometricalModel> gm 	= dynamic_pointer_cast<GeometricalModel>(ClassFactory::instance().createShared("FEMSetGeometry"));
	gm->diffuseColor 			= Vector3r(1,1,1);
	gm->wire 				= false;
	gm->visible 				= true;
	gm->shadowCaster 			= true;
	
	rootBody->interactionGeometry 		= dynamic_pointer_cast<InteractionDescription>(set);	
	rootBody->boundingVolume		= dynamic_pointer_cast<BoundingVolume>(aabb);
	rootBody->geometricalModel 		= gm;
	rootBody->physicalParameters 		= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
 
