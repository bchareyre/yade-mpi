#include "RotatingBox.hpp"


#include "Box.hpp"
#include "AABB.hpp"
#include "Sphere.hpp"
#include "ComplexBody.hpp"
#include "SimpleBody.hpp"
#include "SimpleSpringDynamicEngine.hpp"
#include "SAPCollider.hpp"
#include "RigidBodyParameters.hpp"
#include "Rotor.hpp"
#include <fstream>
#include "IOManager.hpp"
#include "InteractionBox.hpp"
#include "InteractionSphere.hpp"
#include "InteractionGeometryDispatcher.hpp"
#include "ActionDispatcher.hpp"
#include "ActionReset.hpp"
#include "CundallNonViscousForceDamping.hpp"
#include "CundallNonViscousMomentumDamping.hpp"
#include "ActionDispatcher.hpp"

#include "BoundingVolumeDispatcher.hpp"
#include "InteractionDescriptionSet2AABBFunctor.hpp"
#include "InteractionDescriptionSet.hpp"

#include "TimeIntegratorDispatcher.hpp"

RotatingBox::RotatingBox () : FileGenerator()
{
	nbSpheres	= Vector3r(3,3,5);
	nbBoxes		= Vector3r(3,3,4);
	minSize		= 3;
	maxSize		= 5;
	disorder	= 1.1;
	densityBox	= 1;
	densitySphere	= 1;
	dampingForce	= 0.2;
	dampingMomentum = 0.6;
	isRotating	= true;
	rotationSpeed	= 0.05;
	rotationAxis	= Vector3r(1,1,1);
	middleWireFrame = true;
}

RotatingBox::~RotatingBox ()
{
}

void RotatingBox::registerAttributes()
{
	FileGenerator::registerAttributes();
	REGISTER_ATTRIBUTE(nbSpheres);
	REGISTER_ATTRIBUTE(nbBoxes);
	REGISTER_ATTRIBUTE(minSize);
	REGISTER_ATTRIBUTE(maxSize);
	REGISTER_ATTRIBUTE(densityBox);
	REGISTER_ATTRIBUTE(densitySphere);
	REGISTER_ATTRIBUTE(dampingForce);
	REGISTER_ATTRIBUTE(dampingMomentum);
	REGISTER_ATTRIBUTE(isRotating);
	REGISTER_ATTRIBUTE(rotationSpeed);
	REGISTER_ATTRIBUTE(rotationAxis);
	REGISTER_ATTRIBUTE(middleWireFrame);
//	REGISTER_ATTRIBUTE(disorder);
}

string RotatingBox::generate()
{
	rootBody = shared_ptr<ComplexBody>(new ComplexBody);

	createActors(rootBody);
	positionRootBody(rootBody);
	shared_ptr<Body> body;
	
	createKinematicBox(body, Vector3r(  0,  0, 10), Vector3r( 50,  5, 40),middleWireFrame);	rootBody->bodies->insert(body);
	createKinematicBox(body, Vector3r(-55,  0,  0), Vector3r(  5, 60, 50),true );	rootBody->bodies->insert(body);
	createKinematicBox(body, Vector3r( 55,  0,  0), Vector3r(  5, 60, 50),true );	rootBody->bodies->insert(body);
	createKinematicBox(body, Vector3r(  0,-55,  0), Vector3r( 50,  5, 50),true );	rootBody->bodies->insert(body);
	createKinematicBox(body, Vector3r(  0, 55,  0), Vector3r( 50,  5, 50),true );	rootBody->bodies->insert(body);
	createKinematicBox(body, Vector3r(  0,  0,-55), Vector3r( 60, 60,  5),true );	rootBody->bodies->insert(body);
	createKinematicBox(body, Vector3r(  0,  0, 55), Vector3r( 60, 60,  5),true );	rootBody->bodies->insert(body);

	for(int i=0;i<nbSpheres[0];i++)
		for(int j=0;j<nbSpheres[1];j++)
			for(int k=0;k<nbSpheres[2];k++)
			{
				shared_ptr<Body> sphere;
				createSphere(sphere,i,j,k);
				rootBody->bodies->insert(sphere);
			}

	for(int i=0;i<nbBoxes[0];i++)
		for(int j=0;j<nbBoxes[1];j++)
			for(int k=0;k<nbBoxes[2];k++)
 			{
				shared_ptr<Body> box;
				createBox(box,i,j,k);
				rootBody->bodies->insert(box);
 			}

	return "";
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void RotatingBox::createBox(shared_ptr<Body>& body, int i, int j, int k)
{
	body = shared_ptr<Body>(new SimpleBody(0,0));
	shared_ptr<RigidBodyParameters> physics(new RigidBodyParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Box> gBox(new Box);
	shared_ptr<InteractionBox> iBox(new InteractionBox);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);
		
	Vector3r translation		= Vector3r(i,j,k)*10
					  - Vector3r(15,35,25)
					  + Vector3r(Mathr::symmetricRandom(),Mathr::symmetricRandom(),Mathr::symmetricRandom());
				  
	Vector3r size 			= Vector3r(     (Mathr::intervalRandom(minSize,maxSize))
							,(Mathr::intervalRandom(minSize,maxSize))
							,(Mathr::intervalRandom(minSize,maxSize))
						);
	body->isDynamic			= true;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= size[0]*size[1]*size[2]*densityBox;
	physics->inertia		= Vector3r(physics->mass*(size[1]*size[1]+size[2]*size[2])/3,physics->mass*(size[0]*size[0]+size[2]*size[2])/3,physics->mass*(size[1]*size[1]+size[0]*size[0])/3);
	physics->se3			= Se3r(translation,q);

	aabb->diffuseColor		= Vector3r(0,1,0);
	
	gBox->extents			= size;
	gBox->diffuseColor		= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());
	gBox->wire			= false;
	gBox->visible			= true;
	gBox->shadowCaster		= true;
	
	iBox->extents			= size;
	iBox->diffuseColor		= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());

	body->interactionGeometry	= iBox;
	body->geometricalModel		= gBox;
	body->boundingVolume		= aabb;
	body->physicalParameters	= physics;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void RotatingBox::createSphere(shared_ptr<Body>& body, int i, int j, int k)
{
	body = shared_ptr<Body>(new SimpleBody(0,0));
	shared_ptr<RigidBodyParameters> physics(new RigidBodyParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Sphere> gSphere(new Sphere);
	shared_ptr<InteractionSphere> iSphere(new InteractionSphere);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);
		
	Vector3r translation 		= Vector3r(i,j,k)*10
					  - Vector3r(45,45,45)
					  + Vector3r(Mathr::symmetricRandom(),Mathr::symmetricRandom(),Mathr::symmetricRandom());
				  
	Real radius 			= (Mathr::intervalRandom(minSize,maxSize));
	
	body->isDynamic			= true;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 4.0/3.0*Mathr::PI*radius*radius*densitySphere;
	physics->inertia		= Vector3r(2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius); //
	physics->se3			= Se3r(translation,q);

	aabb->diffuseColor		= Vector3r(0,1,0);
	
	gSphere->radius			= radius;
	gSphere->diffuseColor		= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());
	gSphere->wire			= false;
	gSphere->visible		= true;
	gSphere->shadowCaster		= true;
	
	iSphere->radius			= radius;
	iSphere->diffuseColor		= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());

	body->interactionGeometry	= iSphere;
	body->geometricalModel		= gSphere;
	body->boundingVolume		= aabb;
	body->physicalParameters	= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void RotatingBox::createKinematicBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents,bool wire)
{
	body = shared_ptr<Body>(new SimpleBody(0,0));
	shared_ptr<RigidBodyParameters> physics(new RigidBodyParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Box> gBox(new Box);
	shared_ptr<InteractionBox> iBox(new InteractionBox);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);

	body->isDynamic			= false;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 0;
	physics->inertia		= Vector3r(0,0,0);
	physics->se3			= Se3r(position,q);

	aabb->diffuseColor		= Vector3r(1,0,0);

	gBox->extents			= extents;
	gBox->diffuseColor		= Vector3f(1,1,1);
	gBox->wire			= wire;
	gBox->visible			= true;
	gBox->shadowCaster		= false;
	
	iBox->extents			= extents;
	iBox->diffuseColor		= Vector3f(1,1,1);

	body->boundingVolume		= aabb;
	body->interactionGeometry	= iBox;
	body->geometricalModel		= gBox;
	body->physicalParameters	= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void RotatingBox::createActors(shared_ptr<ComplexBody>& rootBody)
{
	shared_ptr<InteractionGeometryDispatcher> interactionGeometryDispatcher(new InteractionGeometryDispatcher);
	interactionGeometryDispatcher->add("InteractionSphere","InteractionSphere","Sphere2Sphere4ClosestFeatures");
	interactionGeometryDispatcher->add("InteractionSphere","InteractionBox","Box2Sphere4ClosestFeatures");
	interactionGeometryDispatcher->add("InteractionBox","InteractionBox","Box2Box4ClosestFeatures");

	shared_ptr<BoundingVolumeDispatcher> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeDispatcher>(new BoundingVolumeDispatcher);
	boundingVolumeDispatcher->add("InteractionSphere","AABB","Sphere2AABBFunctor");
	boundingVolumeDispatcher->add("InteractionBox","AABB","Box2AABBFunctor");
	boundingVolumeDispatcher->add("InteractionDescriptionSet","AABB","InteractionDescriptionSet2AABBFunctor");
		
	shared_ptr<CundallNonViscousForceDamping> actionForceDamping(new CundallNonViscousForceDamping);
	actionForceDamping->damping = dampingForce;
	shared_ptr<CundallNonViscousMomentumDamping> actionMomentumDamping(new CundallNonViscousMomentumDamping);
	actionMomentumDamping->damping = dampingMomentum;
	shared_ptr<ActionDispatcher> actionDampingDispatcher(new ActionDispatcher);
	actionDampingDispatcher->add("ActionForce","ParticleParameters","CundallNonViscousForceDamping",actionForceDamping);
	actionDampingDispatcher->add("ActionMomentum","RigidBodyParameters","CundallNonViscousMomentumDamping",actionMomentumDamping);
	
	shared_ptr<ActionDispatcher> applyActionDispatcher(new ActionDispatcher);
	applyActionDispatcher->add("ActionForce","ParticleParameters","ApplyActionForce2Particle");
	applyActionDispatcher->add("ActionMomentum","RigidBodyParameters","ApplyActionMomentum2RigidBody");
	
	shared_ptr<TimeIntegratorDispatcher> timeIntegratorDispatcher(new TimeIntegratorDispatcher);
 	timeIntegratorDispatcher->add("RigidBodyParameters","LeapFrogIntegrator");
 	
	shared_ptr<Rotor> kinematic = shared_ptr<Rotor>(new Rotor);
 	kinematic->angularVelocity  = rotationSpeed;
	rotationAxis.normalize();
 	kinematic->rotationAxis  = rotationAxis;
 	kinematic->rotateAroundZero = true;
	
 	for(int i=0;i<7;i++)
 		kinematic->subscribedBodies.push_back(i);
	
	rootBody->actors.clear();
	rootBody->actors.push_back(shared_ptr<Actor>(new ActionReset));
	rootBody->actors.push_back(boundingVolumeDispatcher);
	rootBody->actors.push_back(shared_ptr<Actor>(new SAPCollider));
	rootBody->actors.push_back(interactionGeometryDispatcher);
	rootBody->actors.push_back(shared_ptr<Actor>(new SimpleSpringDynamicEngine));
	rootBody->actors.push_back(actionDampingDispatcher);
	rootBody->actors.push_back(applyActionDispatcher);
	rootBody->actors.push_back(timeIntegratorDispatcher);
	if(isRotating)
		rootBody->actors.push_back(kinematic);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void RotatingBox::positionRootBody(shared_ptr<ComplexBody>& rootBody)
{
	rootBody->isDynamic			= false;
	Quaternionr q;	q.fromAxisAngle( Vector3r(0,0,1),0);
	shared_ptr<ParticleParameters> physics(new ParticleParameters); // FIXME : fix indexable class BodyPhysicalParameters
	physics->se3				= Se3r(Vector3r(0,0,0),q);
	physics->mass				= 0;
	physics->velocity			= Vector3r::ZERO;
	physics->acceleration			= Vector3r::ZERO;
	
	shared_ptr<InteractionDescriptionSet> set(new InteractionDescriptionSet());
	set->diffuseColor			= Vector3f(0,0,1);

	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor			= Vector3r(0,0,1);
	
	rootBody->interactionGeometry		= dynamic_pointer_cast<InteractionDescription>(set);	
	rootBody->boundingVolume		= dynamic_pointer_cast<BoundingVolume>(aabb);
	rootBody->physicalParameters 		= physics;
}
