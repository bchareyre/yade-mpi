#include "SDECSpheresPlane.hpp"


#include "Box.hpp"
#include "AABB.hpp"
#include "Sphere.hpp"
#include "ComplexBody.hpp"
#include "SAPCollider.hpp"
#include "PersistentSAPCollider.hpp"
#include "SDECParameters.hpp"
#include <fstream>
#include "IOManager.hpp"
#include "SDECDynamicEngine.hpp"
#include "SDECParameters.hpp"
#include "SDECLinkGeometry.hpp"
#include "Interaction.hpp"
#include "BoundingVolumeDispatcher.hpp"
#include "InteractionDescriptionSet2AABBFunctor.hpp"
#include "InteractionDescriptionSet.hpp"
#include "SDECMacroMicroElasticRelationships.hpp"
#include "SDECTimeStepper.hpp"


#include "ActionDispatcher.hpp"
#include "ActionDispatcher.hpp"
#include "CundallNonViscousForceDamping.hpp"
#include "CundallNonViscousMomentumDamping.hpp"

#include "InteractionGeometryDispatcher.hpp"
#include "InteractionPhysicsDispatcher.hpp"
#include "SingleBody.hpp"
#include "InteractionBox.hpp"
#include "InteractionSphere.hpp"
#include "ActionDispatcher.hpp"
#include "ActionReset.hpp"

SDECSpheresPlane::SDECSpheresPlane () : FileGenerator()
{
	nbSpheres = Vector3r(2,3,2);
	minRadius = 5;
	maxRadius = 5;
	groundSize = Vector3r(200,5,200);
	dampingForce = 0.3;
	dampingMomentum = 0.3;
	timeStepUpdateInterval = 300;
	sphereYoungModulus   = 15000000.0;
	//sphereYoungModulus   = 10000;
	spherePoissonRatio  = 0.2;
	sphereFrictionDeg   = 18.0;
	density = 2600;
	rotationBlocked = false;
}

SDECSpheresPlane::~SDECSpheresPlane ()
{

}

void SDECSpheresPlane::postProcessAttributes(bool)
{
}

void SDECSpheresPlane::registerAttributes()
{
	REGISTER_ATTRIBUTE(nbSpheres);
	REGISTER_ATTRIBUTE(minRadius);
	REGISTER_ATTRIBUTE(maxRadius);
	REGISTER_ATTRIBUTE(sphereYoungModulus);
	REGISTER_ATTRIBUTE(spherePoissonRatio);
	REGISTER_ATTRIBUTE(sphereFrictionDeg);
	REGISTER_ATTRIBUTE(density);
	REGISTER_ATTRIBUTE(groundSize);
	REGISTER_ATTRIBUTE(dampingForce);
	REGISTER_ATTRIBUTE(dampingMomentum);
	REGISTER_ATTRIBUTE(rotationBlocked);
	REGISTER_ATTRIBUTE(timeStepUpdateInterval);
}

string SDECSpheresPlane::generate()
{
	rootBody = shared_ptr<ComplexBody>(new ComplexBody);

	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);

	shared_ptr<InteractionGeometryDispatcher> interactionGeometryDispatcher(new InteractionGeometryDispatcher);
	interactionGeometryDispatcher->add("InteractionSphere","InteractionSphere","Sphere2Sphere4SDECContactModel");
	interactionGeometryDispatcher->add("InteractionSphere","InteractionBox","Box2Sphere4SDECContactModel");

	shared_ptr<InteractionPhysicsDispatcher> interactionPhysicsDispatcher(new InteractionPhysicsDispatcher);
	interactionPhysicsDispatcher->add("SDECParameters","SDECParameters","SDECMacroMicroElasticRelationships");
		
	shared_ptr<BoundingVolumeDispatcher> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeDispatcher>(new BoundingVolumeDispatcher);
	boundingVolumeDispatcher->add("InteractionSphere","AABB","Sphere2AABBFunctor");
	boundingVolumeDispatcher->add("InteractionBox","AABB","Box2AABBFunctor");
	boundingVolumeDispatcher->add("InteractionDescriptionSet","AABB","InteractionDescriptionSet2AABBFunctor");
	
	shared_ptr<CundallNonViscousForceDamping> actionForceDamping(new CundallNonViscousForceDamping);
	actionForceDamping->damping = dampingForce;
	shared_ptr<CundallNonViscousMomentumDamping> actionMomentumDamping(new CundallNonViscousMomentumDamping);
	actionMomentumDamping->damping = dampingMomentum;
	shared_ptr<ActionDispatcher> actionDampingDispatcher(new ActionDispatcher);
	actionDampingDispatcher->add("ActionForce","RigidBodyParameters","CundallNonViscousForceDamping",actionForceDamping);
	actionDampingDispatcher->add("ActionMomentum","RigidBodyParameters","CundallNonViscousMomentumDamping",actionMomentumDamping);
	
	shared_ptr<ActionDispatcher> applyActionDispatcher(new ActionDispatcher);
	applyActionDispatcher->add("ActionForce","RigidBodyParameters","ApplyActionForce2Particle");
	applyActionDispatcher->add("ActionMomentum","RigidBodyParameters","ApplyActionMomentum2RigidBody");
	
	shared_ptr<ActionDispatcher> timeIntegratorDispatcher(new ActionDispatcher);
	timeIntegratorDispatcher->add("ActionForce","ParticleParameters","LeapFrogForceIntegrator");
	if(!rotationBlocked)
		timeIntegratorDispatcher->add("ActionMomentum","RigidBodyParameters","LeapFrogMomentumIntegrator");
	
	shared_ptr<SDECTimeStepper> sdecTimeStepper(new SDECTimeStepper);
	sdecTimeStepper->sdecGroupMask = 1;
	sdecTimeStepper->interval = timeStepUpdateInterval;

	rootBody->actors.clear();
	rootBody->actors.push_back(shared_ptr<Actor>(new ActionReset));
	rootBody->actors.push_back(sdecTimeStepper);
	rootBody->actors.push_back(boundingVolumeDispatcher);	
	rootBody->actors.push_back(shared_ptr<Actor>(new PersistentSAPCollider));
	rootBody->actors.push_back(interactionGeometryDispatcher);
	rootBody->actors.push_back(interactionPhysicsDispatcher);
	rootBody->actors.push_back(shared_ptr<Actor>(new SDECDynamicEngine));
	rootBody->actors.push_back(actionDampingDispatcher);
	rootBody->actors.push_back(applyActionDispatcher);
	rootBody->actors.push_back(timeIntegratorDispatcher);
	

	//FIXME : use a default one
	shared_ptr<ParticleParameters> physics2(new ParticleParameters); // FIXME : fix indexable class BodyPhysicalParameters
	physics2->se3		= Se3r(Vector3r(0,0,0),q);
	physics2->mass		= 0;
	physics2->velocity	= Vector3r(0,0,0);
	rootBody->physicalParameters = physics2;
		
	rootBody->permanentInteractions->clear();

	rootBody->isDynamic		= false;

	shared_ptr<AABB> aabb;
	shared_ptr<Box> box;
	
	shared_ptr<InteractionDescriptionSet> set(new InteractionDescriptionSet());
	set->diffuseColor	= Vector3f(0,0,1);
	rootBody->interactionGeometry		= dynamic_pointer_cast<InteractionDescription>(set);	
	aabb					= shared_ptr<AABB>(new AABB);
	aabb->diffuseColor			= Vector3r(0,0,1);
	rootBody->boundingVolume		= dynamic_pointer_cast<BoundingVolume>(aabb);

///////// ground

	shared_ptr<Body> ground;
	createBox(ground, Vector3r(0,0,0), groundSize);
	rootBody->bodies->insert(ground);

///////// spheres

	for(int i=0;i<nbSpheres[0];i++)
		for(int j=0;j<nbSpheres[1];j++)
			for(int k=0;k<nbSpheres[2];k++)
			{
				shared_ptr<Body> sphere;
				createSphere(sphere,i,j,k);
				rootBody->bodies->insert(sphere);
			}
	
	return "";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SDECSpheresPlane::createSphere(shared_ptr<Body>& body, int i, int j, int k)
{
	body = shared_ptr<Body>(new SingleBody(0,1));
	shared_ptr<SDECParameters> physics(new SDECParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Sphere> gSphere(new Sphere);
	shared_ptr<InteractionSphere> iSphere(new InteractionSphere);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);
	
	Vector3r translation		= Vector3r(i,j,k)*(2*maxRadius*1.1) // this formula is crazy !!
					  - Vector3r( nbSpheres[0]/2*(2*maxRadius*1.1) , -7-maxRadius*2 , nbSpheres[2]/2*(2*maxRadius*1.1) )
					  + Vector3r(Mathr::symmetricRandom()*1.1,Mathr::symmetricRandom()*1.1,Mathr::symmetricRandom()*1.1);
	
	Real radius 			= (Mathr::intervalRandom(minRadius,maxRadius));
	
	body->isDynamic			= true;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 4.0/3.0*Mathr::PI*radius*radius*density;
	physics->inertia		= Vector3r(2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius); //
	physics->se3			= Se3r(translation,q);
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
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void SDECSpheresPlane::createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents)
{
	body = shared_ptr<Body>(new SingleBody(0,1));
	shared_ptr<SDECParameters> physics(new SDECParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Box> gBox(new Box);
	shared_ptr<InteractionBox> iBox(new InteractionBox);
	
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);

	body->isDynamic			= false;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= extents[0]*extents[1]*extents[2]*density*2; 
	physics->inertia		= Vector3r(
							  physics->mass*(extents[1]*extents[1]+extents[2]*extents[2])/3
							, physics->mass*(extents[0]*extents[0]+extents[2]*extents[2])/3
							, physics->mass*(extents[1]*extents[1]+extents[0]*extents[0])/3
						);
	//physics->mass			= 0;
	//physics->inertia		= Vector3r(0,0,0);
	physics->se3			= Se3r(position,q);
	physics->young			= sphereYoungModulus;
	physics->poisson		= spherePoissonRatio;
	physics->frictionAngle		= sphereFrictionDeg * Mathr::PI/180.0;

	aabb->diffuseColor		= Vector3r(1,0,0);

	gBox->extents			= extents;
	gBox->diffuseColor		= Vector3f(1,1,1);
	gBox->wire			= false;
	gBox->visible			= true;
	gBox->shadowCaster		= true;
	
	iBox->extents			= extents;
	iBox->diffuseColor		= Vector3f(1,1,1);

	body->boundingVolume		= aabb;
	body->interactionGeometry	= iBox;
	body->geometricalModel		= gBox;
	body->physicalParameters	= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

