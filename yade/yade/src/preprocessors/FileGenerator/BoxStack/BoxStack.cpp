#include "BoxStack.hpp"


#include "Box.hpp"
#include "AABB.hpp"
#include "Sphere.hpp"
#include "ComplexBody.hpp"
#include "SimpleBody.hpp"
#include "SimpleSpringLaw.hpp"
#include "SAPCollider.hpp"
#include "RigidBodyParameters.hpp"
#include "Translator.hpp"
#include <fstream>
#include "IOManager.hpp"
#include "InteractionBox.hpp"
#include "InteractionSphere.hpp"
#include "InteractionGeometryDispatcher.hpp"
#include "ActionParameterDispatcher.hpp"
#include "ActionReset.hpp"
#include "CundallNonViscousForceDamping.hpp"
#include "CundallNonViscousMomentumDamping.hpp"
#include "ActionParameterDispatcher.hpp"

#include "BoundingVolumeDispatcher.hpp"
#include "InteractionDescriptionSet2AABBFunctor.hpp"
#include "InteractionDescriptionSet.hpp"

#include "ActionParameterDispatcher.hpp"

BoxStack::BoxStack () : FileGenerator()
{
	nbBoxes		= Vector3r(1,5,7);
	boxSize		= Vector3r(6,4,8);
	boxDensity	= 1;
	bulletVelocity	= Vector3r(-120,0,0);
	bulletPosition	= Vector3r(80,0,0);
	bulletSize	= 12;
	bulletDensity	= 2;
	dampingForce	= 0.6;
	dampingMomentum = 0.9;
	kinematicBullet	= false;
}

BoxStack::~BoxStack ()
{

}

void BoxStack::registerAttributes()
{
	FileGenerator::registerAttributes();
	REGISTER_ATTRIBUTE(nbBoxes);
	REGISTER_ATTRIBUTE(boxSize);
	REGISTER_ATTRIBUTE(boxDensity);
//	REGISTER_ATTRIBUTE(kinematicBullet);
	REGISTER_ATTRIBUTE(bulletSize);
	REGISTER_ATTRIBUTE(bulletDensity);
	REGISTER_ATTRIBUTE(bulletPosition);
	REGISTER_ATTRIBUTE(bulletVelocity);
	REGISTER_ATTRIBUTE(dampingForce);
	REGISTER_ATTRIBUTE(dampingMomentum);
}

string BoxStack::generate()
{
	rootBody = shared_ptr<ComplexBody>(new ComplexBody);

	createActors(rootBody);
	positionRootBody(rootBody);
	shared_ptr<Body> body;
	
	createKinematicBox(body, Vector3r(0,  -5,  0), Vector3r(  200, 5, 200), false);
	rootBody->bodies->insert(body);
	
	for(int i=0;i<nbBoxes[0];i++)
		for(int j=0;j<nbBoxes[1];j++)
			for(int k=0;k<nbBoxes[2];k++)
 			{
				shared_ptr<Body> box;
				createBox(box,i,j,k);
				rootBody->bodies->insert(box);
 			}

	createSphere(body);
	rootBody->bodies->insert(body);
 			
	return "";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


// FIXME - all the functions below, are a modified copy of functions in RotatingBox.cpp
void BoxStack::createBox(shared_ptr<Body>& body, int i, int j, int k)
{
	body = shared_ptr<Body>(new SimpleBody(0,1));
	shared_ptr<RigidBodyParameters> physics(new RigidBodyParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Box> gBox(new Box);
	shared_ptr<InteractionBox> iBox(new InteractionBox);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);
		
	Vector3r position		= Vector3r(i*boxSize[0],j*boxSize[1],k*boxSize[2])*2
					  - Vector3r(0,-boxSize[1],nbBoxes[2]*boxSize[2]-boxSize[2])
					  + Vector3r(0,0,(j%2)*boxSize[2]);
				  
	Vector3r size 			= boxSize;
	body->isDynamic			= true;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= size[0]*size[1]*size[2]*boxDensity;
	physics->inertia		= Vector3r(physics->mass*(size[1]*size[1]+size[2]*size[2])/3,physics->mass*(size[0]*size[0]+size[2]*size[2])/3,physics->mass*(size[1]*size[1]+size[0]*size[0])/3);
	physics->se3			= Se3r(position,q);

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

void BoxStack::createSphere(shared_ptr<Body>& body)
{
	body = shared_ptr<Body>(new SimpleBody(0,1));
	shared_ptr<RigidBodyParameters> physics(new RigidBodyParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Sphere> gSphere(new Sphere);
	shared_ptr<InteractionSphere> iSphere(new InteractionSphere);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);
		
	Vector3r position 		= Vector3r(0,nbBoxes[1]*boxSize[1],0)
					  + bulletPosition;
				  
	Real radius 			= bulletSize;
	
	body->isDynamic			= true;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= bulletVelocity;
	physics->mass			= 4.0/3.0*Mathr::PI*radius*radius*bulletDensity;
	physics->inertia		= Vector3r(2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius); //
	physics->se3			= Se3r(position,q);

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

void BoxStack::createKinematicBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents,bool wire)
{
	body = shared_ptr<Body>(new SimpleBody(0,1));
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

void BoxStack::createActors(shared_ptr<ComplexBody>& rootBody)
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
	shared_ptr<ActionParameterDispatcher> actionDampingDispatcher(new ActionParameterDispatcher);
	actionDampingDispatcher->add("ActionForce","ParticleParameters","CundallNonViscousForceDamping",actionForceDamping);
	actionDampingDispatcher->add("ActionMomentum","RigidBodyParameters","CundallNonViscousMomentumDamping",actionMomentumDamping);
	
	shared_ptr<ActionParameterDispatcher> applyActionDispatcher(new ActionParameterDispatcher);
	applyActionDispatcher->add("ActionForce","ParticleParameters","NewtonsForceLaw");
	applyActionDispatcher->add("ActionMomentum","RigidBodyParameters","NewtonsMomentumLaw");
	
	shared_ptr<ActionParameterDispatcher> timeIntegratorDispatcher(new ActionParameterDispatcher);
	timeIntegratorDispatcher->add("ActionForce","ParticleParameters","LeapFrogForceIntegrator");
	timeIntegratorDispatcher->add("ActionMomentum","RigidBodyParameters","LeapFrogMomentumIntegrator");
 	
// 	shared_ptr<Rotor> kinematic = shared_ptr<Rotor>(new Rotor);
// 	kinematic->angularVelocity  = rotationSpeed;
// 	rotationAxis.normalize();
// 	kinematic->rotationAxis  = rotationAxis;
// 	kinematic->rotateAroundZero = true;
// 	for(int i=0;i<7;i++)
// 		kinematic->subscribedBodies.push_back(i);
	
	rootBody->actors.clear();
	rootBody->actors.push_back(shared_ptr<Actor>(new ActionReset));
	rootBody->actors.push_back(boundingVolumeDispatcher);
	rootBody->actors.push_back(shared_ptr<Actor>(new SAPCollider));
	rootBody->actors.push_back(interactionGeometryDispatcher);
	rootBody->actors.push_back(shared_ptr<Actor>(new SimpleSpringLaw));
	rootBody->actors.push_back(actionDampingDispatcher);
	rootBody->actors.push_back(applyActionDispatcher);
	rootBody->actors.push_back(timeIntegratorDispatcher);
//	if(isRotating)
//		rootBody->actors.push_back(kinematic);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void BoxStack::positionRootBody(shared_ptr<ComplexBody>& rootBody)
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
